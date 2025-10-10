/*
		Project:		Windows Class Library
		Module:			bitmap.h
		Description:	Used to process windows bitmaps
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
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

#ifndef WIN_BITMAP_H
#define WIN_BITMAP_H

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

#include <WINLIB/gdi.h>
#include <WINLIB/rectangle.h>

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

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Device;
class Application;

class Bitmap : public GdiObject<HBITMAP>
{
	BITMAPINFO		*m_bitmapInfo;
	Size			m_size;

	void setBitmap( HBITMAP newBitmap, int width, int height )
	{
		m_size.width = width;
		m_size.height = height;

		setHandle( newBitmap );
	}
	friend class Device;
	friend class MemoryDevice;
	friend class Button;
	operator HBITMAP () const
	{
		return getHandle();
	}

	Bitmap( HDC memoryDevice ) 
	: GdiObject<HBITMAP>( memoryDevice )
	{
		m_bitmapInfo = nullptr;
	}

	void create( HDC targetDevice, int width, int height )
	{
		setBitmap(
			CreateCompatibleBitmap ( targetDevice, width, height ),
			width, height
		);
	}
	void create( HDC targetDevice, const Size &size )
	{
		create( targetDevice, size.width, size.height );
	}

	friend class Application;
	Bitmap( HBITMAP handle ) : GdiObject<HBITMAP>( handle )
	{
		m_bitmapInfo = nullptr;
	}
	const Bitmap & operator = ( HBITMAP handle )
	{
		setHandle( handle );
		m_bitmapInfo = nullptr;

		return *this;
	}


	public:
	Bitmap() 
	{
		m_bitmapInfo = nullptr;
	}
	Bitmap( const Bitmap &src )	: GdiObject<HBITMAP>( src )
	{
		m_size = src.m_size;
		m_bitmapInfo = nullptr;
	}
	const Bitmap & operator = ( const Bitmap &src )
	{
		if( this != &src )
		{
			clearBitmap();
			GdiObject<HBITMAP>::operator = ( src );
			m_size = src.m_size;
		}

		return *this;
	}
	~Bitmap() 
	{
		if( m_bitmapInfo )
			free( m_bitmapInfo );
	}
	void clearBitmap()
	{
		clear();
		if( m_bitmapInfo )
		{
			free( m_bitmapInfo );
			m_bitmapInfo = nullptr;
		};
	}

	// implementation is in device.h to have it inline
	void create( const Device &targetDevice, int width, int height );
	void create( const Device &targetDevice, const Size &size )
	{
		create( targetDevice, size.width, size.height );
	}
	void createInfo( int depth, size_t numColors )
	{
		if( m_bitmapInfo )
			free( m_bitmapInfo );

		m_bitmapInfo = (BITMAPINFO *)calloc( 
			1, 
			sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * numColors 
		);

		m_bitmapInfo->bmiHeader.biSize         = sizeof( BITMAPINFOHEADER );
		m_bitmapInfo->bmiHeader.biWidth        = m_size.width;
		m_bitmapInfo->bmiHeader.biHeight       = m_size.height;
		m_bitmapInfo->bmiHeader.biPlanes       = 1;
		m_bitmapInfo->bmiHeader.biBitCount     = WORD(depth);
		m_bitmapInfo->bmiHeader.biCompression  = 0;
		m_bitmapInfo->bmiHeader.biClrUsed      = DWORD(numColors);
		m_bitmapInfo->bmiHeader.biClrImportant = DWORD(numColors);
	}
	int getWidth() const
	{
		return m_size.width;
	}
	int getHeight() const
	{
		return m_size.height;
	}
	RGBQUAD & operator [] ( size_t idx )
	{
		return m_bitmapInfo->bmiColors[idx];
	}
	const RGBQUAD & operator [] ( size_t idx ) const
	{
		return m_bitmapInfo->bmiColors[idx];
	}
	const BITMAPINFO *getInfo() const
	{
		return m_bitmapInfo;
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

}	// namespace winlib"

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
