/*
		Project:		Windows Class Library
		Module:			bitmap.h
		Description:	Used to process windows bitmaps
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
	BITMAPINFO		*bitmapInfo;
	Size			size;

	void setBitmap( HBITMAP newBitmap, int width, int height )
	{
		size.width = width;
		size.height = height;

		setHandle( newBitmap );
	}
	friend class Device;
	friend class MemoryDevice;
	friend class Button;
	operator HBITMAP ( void ) const
	{
		return getHandle();
	}

	Bitmap( HDC memoryDevice ) 
	: GdiObject<HBITMAP>( memoryDevice )
	{
		bitmapInfo = NULL;
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
		bitmapInfo = NULL;
	}
	const Bitmap & operator = ( HBITMAP handle )
	{
		setHandle( handle );
		bitmapInfo = NULL;

		return *this;
	}


	public:
	Bitmap() 
	{
		bitmapInfo = NULL;
	}
	Bitmap( const Bitmap &src )	: GdiObject<HBITMAP>( src )
	{
		size = src.size;
		bitmapInfo = NULL;
	}
	const Bitmap & operator = ( const Bitmap &src )
	{
		if( this != &src )
		{
			clearBitmap();
			GdiObject<HBITMAP>::operator = ( src );
			size = src.size;
		}

		return *this;
	}
	~Bitmap() 
	{
		if( bitmapInfo )
			free( bitmapInfo );
	}
	void clearBitmap( void )
	{
		clear();
		if( bitmapInfo )
		{
			free( bitmapInfo );
			bitmapInfo = NULL;
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
		if( bitmapInfo )
			free( bitmapInfo );

		bitmapInfo = (BITMAPINFO *)calloc( 
			1, 
			sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * numColors 
		);

		bitmapInfo->bmiHeader.biSize         = sizeof( BITMAPINFOHEADER );
		bitmapInfo->bmiHeader.biWidth        = size.width;
		bitmapInfo->bmiHeader.biHeight       = size.height;
		bitmapInfo->bmiHeader.biPlanes       = 1;
		bitmapInfo->bmiHeader.biBitCount     = WORD(depth);
		bitmapInfo->bmiHeader.biCompression  = 0;
		bitmapInfo->bmiHeader.biClrUsed      = DWORD(numColors);
		bitmapInfo->bmiHeader.biClrImportant = DWORD(numColors);
	}
	int getWidth() const
	{
		return size.width;
	}
	int getHeight() const
	{
		return size.height;
	}
	RGBQUAD & operator [] ( size_t idx )
	{
		return bitmapInfo->bmiColors[idx];
	}
	const RGBQUAD & operator [] ( size_t idx ) const
	{
		return bitmapInfo->bmiColors[idx];
	}
	const BITMAPINFO *getInfo() const
	{
		return bitmapInfo;
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
