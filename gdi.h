/*
		Project:		Windows Class Library
		Module:			GDI.H
		Description:	Windows GDI handles
		Author:			Martin G�ckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin G�ckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin G�ckler, Linz, Austria ``AS IS''
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
	GdiType		m_handle;

	GdiHandle( const GdiHandle &src );

	public:
	GdiHandle( const GdiType &newHandle = 0 )
	{
		m_handle = newHandle;
	}
	~GdiHandle()
	{
		DeleteObject( m_handle );
	}
	const GdiHandle & operator = ( const GdiType &newHandle )
	{
		DeleteObject( m_handle );
		m_handle = newHandle;

		return *this;
	}

	const GdiType &getHandle() const
	{
		return m_handle;
	}
};

template <class GdiType> 
class GdiObject;

template <class GdiType> 
class GdiObjectHandle : public GdiHandle<GdiType>
{
	friend class GdiObject<GdiType>;

	size_t		m_usageCounter;

	public:
	GdiObjectHandle( const GdiType &newHandle ) : GdiHandle<GdiType>( newHandle )
	{
		m_usageCounter = 1;
	}
};

template <class GdiType> 
class GdiObject
{
	GdiObjectHandle<GdiType>	*m_handle;
	HDC							m_deviceContext;

	void remove()
	{
		if( m_handle && !--m_handle->m_usageCounter )
			delete m_handle;
	}
	void setHandle( GdiObjectHandle<GdiType> *newHandle )
	{
		remove();
		m_handle = newHandle;
	}

	protected:
	GdiObject( HDC newDevice = nullptr )
	{
		m_handle = nullptr;
		m_deviceContext = newDevice;
	}
	GdiObject( GdiType newHandle )
	{
		m_handle = newHandle ? new GdiObjectHandle<GdiType>( newHandle ) : nullptr;
		m_deviceContext = nullptr;
	}
	const GdiObject & operator = ( GdiType src )
	{
		setHandle( src );

		return *this;
	}
	GdiObject( const GdiObject &src )
	{
		m_handle = src.m_handle;
		m_deviceContext = src.m_deviceContext;

		if( m_handle )
			m_handle->m_usageCounter++;
	}
	const GdiObject & operator = ( const GdiObject &src )
	{
		if( this != &src && m_handle != src.m_handle )
		{
			remove();
			m_handle = src.m_handle;
			if( m_handle )
				m_handle->m_usageCounter++;
			if( !m_deviceContext )
				m_deviceContext = src.m_deviceContext;
		}

		return *this;
	}
	~GdiObject()
	{
		remove();
	}

	void clear()
	{
		remove();
		m_handle = nullptr;
	}
	void setHandle( const GdiType &newHandle )
	{
		remove();
		if( newHandle )
		{
			m_handle = new GdiObjectHandle<GdiType>( newHandle );
			if( m_deviceContext )
				SelectObject( m_deviceContext, newHandle );
		}
		else
			m_handle = nullptr;
	}
	GdiType getHandle() const
	{
		return m_handle ? m_handle->getHandle() : nullptr;
	}

	public:
	void connect( HDC hDC )
	{
		m_deviceContext = hDC;
		GdiType handle = getHandle();
		if( handle )
			SelectObject( m_deviceContext, handle );
	}
	void disconnect()
	{
		m_deviceContext = nullptr;
	}
	HDC getConnection() const
	{
		return m_deviceContext;
	}
	operator bool () const
	{
		return getHandle() != nullptr;
	}
	bool operator ! () const
	{
		return getHandle() == nullptr;
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
