/*
		Project:		Windows Class Library
		Module:			GDI.H
		Description:	Windows GDI handles
		Author:			Martin Gäckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Germany, Munich ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

#ifndef WINLIB_GDI_H
#define WINLIB_GDI_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

enum SystemColor
{
	scDIALOG_FACE = COLOR_3DFACE,
	scBUTTON_FACE = COLOR_BTNFACE,
	scWINDOW_FACE = COLOR_WINDOW,
	scWINDOW_TEXT = COLOR_WINDOWTEXT,
	scSELECTION   = COLOR_HIGHLIGHT
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

template <class GdiType> 
class GdiHandle
{
	private:
	GdiType		handle;

	GdiHandle( const GdiHandle &src );

	public:
	GdiHandle( const GdiType &newHandle = 0 )
	{
		handle = newHandle;
	}
	~GdiHandle()
	{
		DeleteObject( handle );
	}
	const GdiHandle & operator = ( const GdiType &newHandle )
	{
		DeleteObject( handle );
		handle = newHandle;

		return *this;
	}

	const GdiType &getHandle( void ) const
	{
		return handle;
	}
};

template <class GdiType> 
class GdiObject;

template <class GdiType> 
class GdiObjectHandle : public GdiHandle<GdiType>
{
	friend class GdiObject<GdiType>;

	size_t		usageCounter;

	public:
	GdiObjectHandle( const GdiType &newHandle ) : GdiHandle<GdiType>( newHandle )
	{
		usageCounter = 1;
	}
};

template <class GdiType> 
class GdiObject
{
	GdiObjectHandle<GdiType>	*handle;
	HDC							deviceContext;

	void remove( void )
	{
		if( handle && !--handle->usageCounter )
			delete handle;
	}
	void setHandle( GdiObjectHandle<GdiType> *newHandle )
	{
		remove();
		handle = newHandle;
	}

	protected:
	GdiObject( HDC newDevice = NULL )
	{
		handle = NULL;
		deviceContext = newDevice;
	}
	GdiObject( GdiType newHandle )
	{
		handle = newHandle ? new GdiObjectHandle<GdiType>( newHandle ) : NULL;
		deviceContext = NULL;
	}
	const GdiObject & operator = ( GdiType src )
	{
		setHandle( src );

		return *this;
	}
	GdiObject( const GdiObject &src )
	{
		handle = src.handle;
		deviceContext = src.deviceContext;

		if( handle )
			handle->usageCounter++;
	}
	const GdiObject & operator = ( const GdiObject &src )
	{
		if( this != &src && this->handle != src.handle )
		{
			remove();
			handle = src.handle;
			if( handle )
				handle->usageCounter++;
			if( !deviceContext )
				deviceContext = src.deviceContext;
		}

		return *this;
	}
	~GdiObject()
	{
		remove();
	}

	void clear( void )
	{
		remove();
		handle = NULL;
	}
	void setHandle( const GdiType &newHandle )
	{
		remove();
		if( newHandle )
		{
			handle = new GdiObjectHandle<GdiType>( newHandle );
			if( deviceContext )
				SelectObject( deviceContext, newHandle );
		}
		else
			handle = NULL;
	}
	GdiType getHandle( void ) const
	{
		return handle ? handle->getHandle() : NULL;
	}

	public:
	void connect( HDC hDC )
	{
		deviceContext = hDC;
		GdiType handle = getHandle();
		if( handle )
			SelectObject( deviceContext, handle );
	}
	void disconnect( void )
	{
		deviceContext = NULL;
	}
	HDC getConnection( void ) const
	{
		return deviceContext;
	}
	operator bool () const
	{
		return getHandle() != NULL;
	}
	bool operator ! () const
	{
		return getHandle() == NULL;
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
