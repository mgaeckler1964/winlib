/*
		Project:		Windows Class Library
		Module:			Device.H
		Description:	Graphic devices
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

#ifndef DEVICE_H
#define DEVICE_H

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

#include <winlib/basicwin.h>
#include <winlib/font.h>
#include <winlib/pen.h>
#include <winlib/brush.h>
#include <winlib/bitmap.h>
#include <winlib/palette.h>

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
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Device
{
	public:
	enum HorizontalAlignment{ haLeft = TA_LEFT, haCenter = TA_CENTER, haRight = TA_RIGHT };
	enum VerticalAlignment{ vaTop = TA_TOP, vaBaseline = TA_BASELINE , vaBottom = TA_BOTTOM };

	protected:
	HDC				device;

	private:
	Pen				myPen;
	Brush			myBrush;

	Font			theFont;
	int				tabWidth;

	GdiHandle<HRGN>	clipRegion;

	private:
	// disallow
	Device( const Device &src );
	const Device & operator = ( const Device &src );

	private:
	void connect()
	{
		myBrush.connect( device );
		myPen.connect( device );
		theFont.connect( device );
	}

	protected:
	/*
		the device is used for drawing on a printer or memory device
	*/
	Device( HDC context ) : myBrush( context ), myPen( context ), theFont( context )
	{
		device = context;

		setTextAlignment( haLeft, vaTop );
	}

	//public:
	friend class MemoryDevice;
	friend class Bitmap;
	operator HDC ( void ) const
	{
		return device;
	}

	public:
	~Device()
	{
		if( device )
		{
			DeleteDC( device );
		}
	}

	operator bool () const
	{
		return device != NULL;
	}
	bool operator ! () const
	{
		return device == NULL;
	}

	void setTabWidth( int numChars )
	{
		TEXTMETRIC		tm;

		GetTextMetrics( device, &tm );
		tabWidth = numChars * tm.tmAveCharWidth;
	}
	int getTabWidth( void ) const
	{
		return tabWidth;
	}
	void printText( const char *text ) const;
	int getCharWidth( void ) const;
	int getNumLines( void ) const;
	int getLineHeight( void ) const;

	void moveTo( int x, int y ) const
	{
		MoveToEx( device, x, y, NULL );
	}
	void moveTo( const Point &point ) const
	{
		moveTo( point.x, point.y );
	}

	void getPosition( int *x, int *y ) const
	{
		Point	pos;

		GetCurrentPositionEx( device, &pos );

		if( x )
			*x = pos.x;
		if( y )
			*y = pos.y;
	}
	void getTextExtentWithBreak( const char *text, Size *size );
	void getTextExtent( const char *text, size_t len, Size *size );
	void getTextExtent( const char *text, size_t startPos, size_t len, Size *size )
	{
		getTextExtent( text+startPos, len, size ); 
	}
	void getTextExtent( char text, Size *size )
	{
		getTextExtent( &text, 1, size );
	}

	int	getxOffset( void ) const
	{
		return GetDeviceCaps( device, PHYSICALOFFSETX );
	}
	int	getyOffset( void ) const
	{
		return GetDeviceCaps( device, PHYSICALOFFSETY );
	}
	int	getxResolution( void ) const
	{
		return GetDeviceCaps( device, LOGPIXELSX );
	}
	int	getyResolution( void ) const
	{
		return GetDeviceCaps( device, LOGPIXELSY );
	}


	/*
		brush
	*/
	void selectBrush( const Brush &newBrush )
	{
		myBrush = newBrush;
		myBrush.connect( device );
	}
	Brush &getBrush( void )
	{
		return myBrush;
	}

	/*
		pen
	*/
	void selectPen( const Pen &newPen )
	{
		myPen = newPen;
		myPen.connect( device );
	}
	Pen &getPen( void )
	{
		return myPen;
	}

	/*
		font
	*/

	void selectFont( void )
	{
		if( !theFont.isAssigned() )
			theFont.create();
	}
	void selectFont( const Font &newFont )
	{
		theFont = newFont;
		theFont.connect( device );
		selectFont();
	}
	void setTextColor( unsigned char red, unsigned char green, unsigned char blue )
	{
		SetTextColor( device, RGB( red, green, blue ) ); 
	}
	void setBackgroundColor( COLORREF color, int mode = OPAQUE )
	{
		SetBkMode( device,  mode );
		SetBkColor( device, color );
	}
	void setBackgroundColor( unsigned char red, unsigned char green, unsigned char blue, int mode = OPAQUE )
	{
		setBackgroundColor( RGB( red, green, blue ), mode );
	}
	void clrBackgroundColor( void )
	{
		SetBkMode( device, TRANSPARENT );
	}
	const Font &getFont( void ) const
	{
		return theFont;
	}
	Font &getFont( void )
	{
		return theFont;
	}
	void setDefaultFont( void )
	{
		SelectObject( device, GetStockObject( ANSI_VAR_FONT ) );
	}
	int setLetterSpacing( int letterSpace )
	{
		return SetTextCharacterExtra( device, letterSpace );
	}

	/*
		paint operations
	*/
	void lineTo( int x, int y ) const
	{
		LineTo( device, x, y );
	}
	void lineTo( const Point &point ) const
	{
		lineTo( point.x, point.y );
	}
	void line( int x1, int y1, int x2, int y2 ) const
	{
		moveTo( x1, y1 );
		lineTo( x2, y2 );
	}
	void line( const Point &start, const Point &end ) const
	{
		moveTo( start );
		lineTo( end );
	}
	void verticalLine( int x, int y1, int y2 )
	{
		moveTo( x, y1 );
		lineTo( x, y2 );
	}
	void horizontalLine( int x1, int x2, int y )
	{
		moveTo( x1, y );
		lineTo( x2, y );
	}

	void ellipse( int left, int top, int right, int bottom ) const
	{
		Ellipse( device, left, top, right, bottom );
	}
	void ellipse( int x, int y, int radius ) const
	{
		Ellipse( device, x-radius, y-radius, x+radius, y+radius );
	}

	void rectangle( int left, int top, int right, int bottom ) const
	{
		Rectangle( device, left, top, right, bottom );
	}
	void rectangle( const RectBorder &rect ) const
	{
		Rectangle( device, rect.left, rect.top, rect.right, rect.bottom );
	}

	void textOut( int x, int y, const STRING &text )
	{
		moveTo( x, y );
		TextOut(
			device,
			x, y,
			text,
			int(text.strlen())
		);
	}
	void drawText( const char *text, size_t len, const RectBorder &rect, unsigned format )
	{
//		SetTextAlign( device, TA_TOP|TA_LEFT|TA_NOUPDATECP );
		moveTo( rect.left, rect.top );
		DrawText( device, text, int(len), const_cast<RectBorder *>(&rect), format );
	}
	void drawText( const STRING &text, const RectBorder &rect, unsigned format )
	{
		drawText( text, text.strlen(), rect, format );
	}

	/*
		draw modes
	*/
	int setROP2( int drawMode ) const
	{
		return SetROP2( device, drawMode );
	}

	void setTextAlignment( HorizontalAlignment ha, VerticalAlignment va ) const
	{
		SetTextAlign( device, int(ha)|int(va)|TA_UPDATECP );
	}

	void removeClipping( void )
	{
		SelectClipRgn( device, NULL );
	}
	void setClipping(  int left, int top, int right, int bottom  )
	{
		clipRegion = CreateRectRgn( left, top, right, bottom );
		SelectClipRgn( device, clipRegion.getHandle() );
	}
	/*
		palette
	*/
	void selectPalette( const Palette &palette )
	{
		SelectPalette( device, palette, true );
		RealizePalette( device );
	}

	/*
		bitmaps
	*/
	void setDIBits( const Bitmap &bitmap, unsigned uStartScan, unsigned numLines, void *data, unsigned color )
	{
		SetDIBits(
			device, bitmap, 
			uStartScan, numLines, 
			data,
			bitmap.getInfo(), color
		);
	}
	void drawBitmap( int x, int y, const Bitmap &bitmap );

	void setPixel( int x, int y, COLORREF color ) const
	{
		SetPixel( device, x,y, color );
	}
	void setPixel( int x, int y, unsigned char red, unsigned char green, unsigned char blue ) const
	{
		setPixel( x,y, RGB(red, green, blue) );
	}

	/*
		calculating # pixels with int source
	*/
	int pointToPixel( int ptValue ) const
	{
		return MulDiv(ptValue, GetDeviceCaps(device, LOGPIXELSY), 72);
	}
	int picaToPixel( int pcValue ) const
	{
		return MulDiv(pcValue, GetDeviceCaps(device, LOGPIXELSY), 6);
	}
	int cmToPixel( int cmValue ) const
	{
		return MulDiv(cmValue, GetDeviceCaps(device, LOGPIXELSY)*100, 254);
	}
	int mmToPixel( int mmValue ) const
	{
		return MulDiv(mmValue, GetDeviceCaps(device, LOGPIXELSY)*10, 254);
	}
	int inToPixel( int inValue ) const
	{
		return inValue * GetDeviceCaps(device, LOGPIXELSY);
	}

	/*
		calculating # pixels with double values
	*/
	int pointToPixel( double ptValue ) const
	{
		return int(ptValue * double(GetDeviceCaps(device, LOGPIXELSY)) / 72.0 +0.5);
	}
	int picaToPixel( double pcValue ) const
	{
		return int(pcValue * double(GetDeviceCaps(device, LOGPIXELSY)) / 6.0 +0.5);
	}
	int cmToPixel( double cmValue ) const
	{
		return int(cmValue * double(GetDeviceCaps(device, LOGPIXELSY)) / 2.54 +0.5);
	}
	int mmToPixel( double mmValue ) const
	{
		return int(mmValue * double(GetDeviceCaps(device, LOGPIXELSY)) / 25.4 +0.5);
	}
	int inToPixel( double inValue ) const
	{
		return int(inValue * double(GetDeviceCaps(device, LOGPIXELSY)) + 0.5);
	}


	/*
		calculating pixels to other dimensions
	*/
	double pixelToPoint( int pxValue ) const
	{
		return (double)pxValue * 72.0 / (double)GetDeviceCaps(device, LOGPIXELSY);
	}
	double pixelToPica( int pxValue ) const
	{
		return (double)pxValue * 6.0 / (double)GetDeviceCaps(device, LOGPIXELSY);
	}
	double pixelToCM( int pxValue ) const
	{
		return (double)pxValue * 2.54 / (double)GetDeviceCaps(device, LOGPIXELSY);
	}
	double pixelToMM( int pxValue ) const
	{
		return (double)pxValue * 25.4 / (double)GetDeviceCaps(device, LOGPIXELSY);
	}
	double pixelToIN( int pxValue ) const
	{
		return (double)pxValue / (double)GetDeviceCaps(device, LOGPIXELSY);
	}
};

class WindowDevice : public Device
{
	protected:
	const BasicWindow *theWindow;
	WindowDevice( const BasicWindow *theWindow, HDC context ) 
	: Device( context ), theWindow( theWindow )
	{}
};

class DrawDevice : public WindowDevice
{
	public:
	DrawDevice( const BasicWindow *theWindow ) 
	: WindowDevice( theWindow, theWindow->getDC() )
	{
		selectFont( theWindow->getFont() );
	}
	~DrawDevice()
	{
		theWindow->releaseDC( device );
		device = NULL;
	}
};

class RepaintDevice : public WindowDevice
{
	PAINTSTRUCT	ps;

	friend class CallbackWindow;

	private:
	RepaintDevice( const BasicWindow *theWindow ) 
	: WindowDevice( theWindow, theWindow->beginPaint( &ps ) )
	{}
	~RepaintDevice()
	{
		theWindow->endPaint( &ps );
		device = NULL;
	}
};

class BackgroundDevice : public WindowDevice
{
	public:
	BackgroundDevice( const BasicWindow *theWindow, HDC context ) 
	: WindowDevice( theWindow, context )
	{}
	~BackgroundDevice()
	{
		device = NULL;		// don't delete
	}
};

class MemoryDevice : public Device
{
	const Device	&target;
	Bitmap			bitmap;

	public:
	MemoryDevice( const Device &target, const Size &size ) 
	: Device( CreateCompatibleDC ( target ) ),
	target( target ),
	bitmap( *this )
	{
		bitmap.create( target, size );
	}
	MemoryDevice( const Device &target, const Bitmap &bitmap ) 
	: Device( CreateCompatibleDC ( target ) ),
	target( target ),
	bitmap( bitmap )
	{
		this->bitmap.connect( *this );
	}

	void drawToWindow( void )
	{
		BitBlt( target, 0, 0, bitmap.getWidth(), bitmap.getHeight(), *this, 0, 0, SRCCOPY );
	}
#if 0
	void drawScaledToWindow( void )
	{
		SetMapMode( *this, GetMapMode( target ));

		POINT	corners[2];

		corners[0].x = 0;
		corners[0].y = 0;
		corners[1].x = bitmap.getWidth();
		corners[1].y = bitmap.getHeight();

		DPtoLP( target, corners, arraySize( corners ) );

		BitBlt(
			target, 0, 0, corners[1].x, corners[1].y,
			*this, corners[0].x, corners[0].y, SRCCOPY
		);
	}
#endif
};

class PrintDevice : public Device
{
	public:
	PrintDevice( HDC context ) : Device( context ) {}

	void startDocument( const char *file )	const
	{
		DOCINFO		dInfo;

		memset( &dInfo, 0, sizeof( dInfo ) );
		dInfo.cbSize = sizeof( dInfo );
		dInfo.lpszDocName = file;
		StartDoc( device, &dInfo );
	}
	void startPage( void )	const
	{
		StartPage( device );
	}
	void endPage( void )	const
	{
		EndPage( device );
	}
	void endDocument( void )	const
	{
		EndDoc( device );
	}
	void printText( const char *document, const char *text ) const;
};

class MetaDevice : public Device
{
	HMETAFILE		metaFile;
	public:
	MetaDevice( const char *fileName = NULL ) : Device( CreateMetaFile( fileName ) )
	{
		metaFile = NULL;
	}
	~MetaDevice()
	{
//		if( metaFile )						// not yet
//			DeleteMetaFile( metaFile );
	}

	HMETAFILE createFile( void )
	{
		if( !metaFile )
			metaFile = CloseMetaFile( device );

		return metaFile;
	}
};

struct PRINTER
{
	STRING	printerName;
	STRING	portName;
};

typedef gak::Array<PRINTER>	PRINTER_ARRAY;
int	GetListOfPrinters( PRINTER_ARRAY &list );


inline void Device::drawBitmap( int x, int y, const Bitmap &bitmap )
{
	MemoryDevice	hDCmem( *this, bitmap );

	BitBlt(
		*this, x, y, bitmap.getWidth(), bitmap.getHeight(),
		hDCmem, 0, 0, SRCCOPY
	);
}

inline void Bitmap::create( const Device &targetDevice, int width, int height )
{
	create ( HDC(targetDevice), width, height );
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif

