/*
		Project:		Windows Class Library
		Module:			Device.H
		Description:	Graphic devices
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
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

namespace colors
{
	// color codes copied from here: https://www.rapidtables.com/web/color/RGB_Color.html

	static const COLORREF	BLACK=RGB(0,0,0);
	static const COLORREF	WHITE=RGB(255,255,255);
	static const COLORREF	RED=RGB(255,0,0);
	static const COLORREF	LIME=RGB(0,255,0);
	static const COLORREF	BLUE=RGB(0,0,255);
	static const COLORREF	YELLOW=RGB( 255, 255, 0 );
	static const COLORREF	MAGENTA=RGB( 255, 0, 255 );
	static const COLORREF	CYAN=RGB( 0, 255, 255 );
	static const COLORREF	MAROON=RGB(128,0,0);
	static const COLORREF	GREEN=RGB(0,128,0);
	static const COLORREF	NAVY=RGB(0,0,128);
	static const COLORREF	OLIVE=RGB(128,128,0);
	static const COLORREF	PURPE=RGB(128,0,128);
	static const COLORREF	TEAL=RGB(0,128,128);
	static const COLORREF	GRAY=RGB(128,128,128);
	static const COLORREF	SILVER=RGB(192,192,192);

	static const COLORREF	DARK_RED=RGB(139,0,0);
	static const COLORREF	BROWN=RGB(165,42,42);
	static const COLORREF	FIREBRICK=RGB(178,34,34);
	static const COLORREF	CRIMSON=RGB(220,20,60);
	static const COLORREF	TOMATO=RGB(255,99,71);
	static const COLORREF	CORAL=RGB(255,127,80);
	static const COLORREF	INDIAN_RED=RGB(205,92,92);
	static const COLORREF	SALMON=RGB(240,128,128);
	static const COLORREF	ORANGE=RGB(255,165,0);
	static const COLORREF	GOLD=RGB(255,215,0);
	static const COLORREF	GOLD_ROD=RGB(218,165,32);
	static const COLORREF	KHAKI=RGB(240,230,140);
	static const COLORREF	YELLOW_GREEN=RGB(154,205,50);
	static const COLORREF	OLIVE_DRAB=RGB(107,142,35);
	static const COLORREF	LAWN_GREEN=RGB(124,252,0);
	static const COLORREF	GREEN_YELLOW=RGB(173,255,47);
	static const COLORREF	DARK_GREEN=RGB(0,100,0);
	static const COLORREF	FORREST_GREEN=RGB(34,139,34);
	static const COLORREF	LIME_GREEN=RGB(50,205,50);
	static const COLORREF	SPRING_GREEN=RGB(0,255,127);
	static const COLORREF	SEA_GREEN=RGB(46,139,87);
	static const COLORREF	TURQUOISE=RGB(64,224,208);
	static const COLORREF	AQUA_MARINE=RGB(127,255,212);

	static const COLORREF	POWDER_BLUE=RGB(176,224,230);
	static const COLORREF	CADET_BLUE=RGB(95,158,160);
	static const COLORREF	STEEL_BLUE=RGB(70,130,180);
	static const COLORREF	CORN_FLOWER_BLUE=RGB(100,149,237);
	static const COLORREF	DODGER_BLUE=RGB(30,144,255);
	static const COLORREF	SKY_BLUE=RGB(135,206,235);
	static const COLORREF	MIDNIGHT_BLUE=RGB(25,25,112);
	static const COLORREF	DARK_BLUE=RGB(0,0,139);
	static const COLORREF	ROYAL_BLUE=RGB(65,105,225);
	static const COLORREF	BLUE_VIOLET=RGB(138,43,226);
	static const COLORREF	INDIGO=RGB(75,0,130);
	static const COLORREF	SLATE_BLUE=RGB(106,90,205);
	static const COLORREF	THISTLE=RGB(216,191,216);
	static const COLORREF	PLUM=RGB(221,160,221);
	static const COLORREF	VIOLET=RGB(238,130,238);
	static const COLORREF	ORCHID=RGB(218,112,214);
	static const COLORREF	PINK=RGB(255,192,203);
	static const COLORREF	ANTIQUE_WHITE=RGB(250,235,215);
	static const COLORREF	BEIGE=RGB(245,245,220);
	static const COLORREF	BISQUE=RGB(255,228,196);
	static const COLORREF	BLANCHED_ALMOND=RGB(255,235,205);
	static const COLORREF	WHEAT=RGB(245,222,179);
	static const COLORREF	CORN_SILK=RGB(255,248,220);
	static const COLORREF	LEMON_CHIFFON=RGB(255,250,205);
	static const COLORREF	SADDLE_BROWN=RGB(139,69,19);
	static const COLORREF	SIENNA=RGB(160,82,45);
	static const COLORREF	CHOCOLATE=RGB(210,105,30);
	static const COLORREF	PERU=RGB(205,133,63);
	static const COLORREF	SANDY_BROWN=RGB(244,164,96);
	static const COLORREF	BURLY_WOOD=RGB(222,184,135);
	static const COLORREF	TAN=RGB(210,180,140);
	static const COLORREF	ROSY_BROWN=RGB(188,143,143);
	static const COLORREF	MOCCASIN=RGB(255,228,181);
	static const COLORREF	NAVAJO_WHITE=RGB(255,222,173);
	static const COLORREF	REACH_PUFF=RGB(255,218,185);
	static const COLORREF	MISTY_ROSE=RGB(255,228,225);
	static const COLORREF	LAVENDER_BLUSH=RGB(255,240,245);
	static const COLORREF	LINEN=RGB(250,240,230);
	static const COLORREF	OLD_LACE=RGB(253,245,230);
	static const COLORREF	PAPAYA_WHIP=RGB(255,239,213);
	static const COLORREF	SEA_SHELL=RGB(255,245,238);
	static const COLORREF	MINT_GREEN=RGB(245,255,250);
	static const COLORREF	LAVENDER=RGB(230,230,250);
	static const COLORREF	FLORAL_WHITE=RGB(255,250,240);
	static const COLORREF	ALICE_BLUE=RGB(240,248,255);
	static const COLORREF	GHOST_WHITE=RGB(248,248,255);
	static const COLORREF	HONEY_DEW=RGB(240,255,240);
	static const COLORREF	IVORY=RGB(255,255,240);
	static const COLORREF	AURE=RGB(240,255,255);
	static const COLORREF	SNOW=RGB(255,250,250);
}

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

inline unsigned getGrayValue( unsigned red, unsigned green, unsigned blue )
{
	return (red + green + blue)/3;
}

inline unsigned getGrayValue( COLORREF color )
{
	return getGrayValue( GetRValue(color), GetGValue(color), GetBValue(color) );
}

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Device
{
	public:
	enum HorizontalAlignment{ haLeft = TA_LEFT, haCenter = TA_CENTER, haRight = TA_RIGHT };
	enum VerticalAlignment{ vaTop = TA_TOP, vaBaseline = TA_BASELINE , vaBottom = TA_BOTTOM };

	protected:
	HDC				m_device;

	private:
	Pen				m_pen;
	Brush			m_brush;

	Font			m_font;
	int				m_tabWidth;

	GdiHandle<HRGN>	m_clipRegion;

	private:
	// disallow
	Device( const Device &src );
	const Device & operator = ( const Device &src );

	private:
	void connect()
	{
		m_brush.connect( m_device );
		m_pen.connect( m_device );
		m_font.connect( m_device );
	}

	protected:
	/*
		the device is used for drawing on a printer or memory device
	*/
	Device( HDC context ) : m_brush( context ), m_pen( context ), m_font( context )
	{
		m_device = context;

		setTextAlignment( haLeft, vaTop );
	}

	//public:
	friend class MemoryDevice;
	friend class Bitmap;
	operator HDC () const
	{
		return m_device;
	}

	public:
	~Device()
	{
		if( m_device )
		{
			DeleteDC( m_device );
		}
	}

	operator bool () const
	{
		return m_device != nullptr;
	}
	bool operator ! () const
	{
		return m_device == nullptr;
	}

	void setTabWidth( int numChars )
	{
		TEXTMETRIC		tm;

		GetTextMetrics( m_device, &tm );
		m_tabWidth = numChars * tm.tmAveCharWidth;
	}
	int getTabWidth() const
	{
		return m_tabWidth;
	}
	void printText( const char *text ) const;
	int getCharWidth() const;
	int getNumLines() const;
	int getLineHeight() const;

	void moveTo( int x, int y ) const
	{
		MoveToEx( m_device, x, y, nullptr );
	}
	void moveTo( const Point &point ) const
	{
		moveTo( point.x, point.y );
	}

	void getPosition( int *x, int *y ) const
	{
		Point	pos;

		GetCurrentPositionEx( m_device, &pos );

		if( x )
			*x = pos.x;
		if( y )
			*y = pos.y;
	}
	void getTextExtentWithBreak( const char *text, Size *size ) const;
	void getTextExtent( const char *text, size_t len, Size *size ) const;
	void getTextExtent( const char *text, size_t startPos, size_t len, Size *size ) const
	{
		getTextExtent( text+startPos, len, size ); 
	}
	void getTextExtent( char text, Size *size ) const
	{
		getTextExtent( &text, 1, size );
	}
	void getTextExtent( const STRING &text, Size *size ) const
	{
		getTextExtent( text, text.strlen(), size );
	}

	int	getxOffset() const
	{
		return GetDeviceCaps( m_device, PHYSICALOFFSETX );
	}
	int	getyOffset() const
	{
		return GetDeviceCaps( m_device, PHYSICALOFFSETY );
	}
	int	getxResolution() const
	{
		return GetDeviceCaps( m_device, LOGPIXELSX );
	}
	int	getyResolution() const
	{
		return GetDeviceCaps( m_device, LOGPIXELSY );
	}


	/*
		brush
	*/
	void selectBrush( const Brush &brush )
	{
		m_brush = brush;
		m_brush.connect( m_device );
	}
	Brush &getBrush()
	{
		return m_brush;
	}

	/*
		pen
	*/
	void selectPen( const Pen &pen )
	{
		m_pen = pen;
		m_pen.connect( m_device );
	}
	Pen &getPen()
	{
		return m_pen;
	}

	/*
		font
	*/

	void selectFont()
	{
		if( !m_font.isAssigned() )
			m_font.create();
	}
	void selectFont( const Font &newFont )
	{
		m_font = newFont;
		m_font.connect( m_device );
		selectFont();
	}
	void setTextColor( COLORREF color )
	{
		SetTextColor( m_device, color ); 
	}
	void setTextColor( unsigned char red, unsigned char green, unsigned char blue )
	{
		setTextColor( RGB( red, green, blue ) ); 
	}
	void setBackgroundColor( COLORREF color, int mode = OPAQUE )
	{
		SetBkMode( m_device,  mode );
		SetBkColor( m_device, color );
	}
	void setBackgroundColor( unsigned char red, unsigned char green, unsigned char blue, int mode = OPAQUE )
	{
		setBackgroundColor( RGB( red, green, blue ), mode );
	}
	void clrBackgroundColor()
	{
		SetBkMode( m_device, TRANSPARENT );
	}
	const Font &getFont() const
	{
		return m_font;
	}
	Font &getFont()
	{
		return m_font;
	}
	void setDefaultFont()
	{
		SelectObject( m_device, GetStockObject( ANSI_VAR_FONT ) );
	}
	int setLetterSpacing( int letterSpace )
	{
		return SetTextCharacterExtra( m_device, letterSpace );
	}

	/*
		paint operations
	*/
	void polyline( const Point *points, int cnt ) const
	{
		Polyline( m_device, points, cnt );
	}
	template <typename ContainerT>
	void polyline( const ContainerT &points ) const
	{
		polyline( points.getDataBuffer(), int(points.size()) );
	}
	void polygon( const Point *points, int cnt ) const
	{
		Polygon( m_device, points, cnt );
	}
	template <typename ContainerT>
	void polygon( const ContainerT &points ) const
	{
		polygon( points.getDataBuffer(), int(points.size()) );
	}

	void lineTo( int x, int y ) const
	{
		LineTo( m_device, x, y );
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
		Ellipse( m_device, left, top, right, bottom );
	}
	void ellipse( int x, int y, int radius ) const
	{
		Ellipse( m_device, x-radius, y-radius, x+radius, y+radius );
	}

	void pie(int x, int y, int radius, double startAngle, double endAngle );
	void pie(const Point &center, int radius, double startAngle, double endAngle )
	{
		pie(center.x, center.y, radius, startAngle, endAngle );
	}

	void rectangle( int left, int top, int right, int bottom ) const
	{
		Rectangle( m_device, left, top, right, bottom );
	}
	void rectangle( const RectBorder &rect ) const
	{
		Rectangle( m_device, rect.left, rect.top, rect.right, rect.bottom );
	}

	void textOut( int x, int y, const STRING &text )
	{
		moveTo( x, y );
		TextOut(
			m_device,
			x, y,
			text,
			int(text.strlen())
		);
	}
	void drawText( const char *text, size_t len, const RectBorder &rect, unsigned format )
	{
//		SetTextAlign( device, TA_TOP|TA_LEFT|TA_NOUPDATECP );
		moveTo( rect.left, rect.top );
		DrawText( m_device, text, int(len), const_cast<RectBorder *>(&rect), format );
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
		return SetROP2( m_device, drawMode );
	}

	void setTextAlignment( HorizontalAlignment ha, VerticalAlignment va ) const
	{
		SetTextAlign( m_device, int(ha)|int(va)|TA_UPDATECP );
	}

	void removeClipping()
	{
		SelectClipRgn( m_device, nullptr );
	}
	void setClipping(  int left, int top, int right, int bottom  )
	{
		m_clipRegion = CreateRectRgn( left, top, right, bottom );
		SelectClipRgn( m_device, m_clipRegion.getHandle() );
	}
	/*
		palette
	*/
	void selectPalette( const Palette &palette )
	{
		SelectPalette( m_device, palette, true );
		RealizePalette( m_device );
	}

	/*
		bitmaps
	*/
	void setDIBits( const Bitmap &bitmap, unsigned uStartScan, unsigned numLines, void *data, unsigned color )
	{
		SetDIBits(
			m_device, bitmap, 
			uStartScan, numLines, 
			data,
			bitmap.getInfo(), color
		);
	}
	void drawBitmap( int x, int y, const Bitmap &bitmap );

	void setPixel( int x, int y, COLORREF color ) const
	{
		SetPixel( m_device, x,y, color );
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
		return MulDiv(ptValue, GetDeviceCaps(m_device, LOGPIXELSY), 72);
	}
	int picaToPixel( int pcValue ) const
	{
		return MulDiv(pcValue, GetDeviceCaps(m_device, LOGPIXELSY), 6);
	}
	int cmToPixel( int cmValue ) const
	{
		return MulDiv(cmValue, GetDeviceCaps(m_device, LOGPIXELSY)*100, 254);
	}
	int mmToPixel( int mmValue ) const
	{
		return MulDiv(mmValue, GetDeviceCaps(m_device, LOGPIXELSY)*10, 254);
	}
	int inToPixel( int inValue ) const
	{
		return inValue * GetDeviceCaps(m_device, LOGPIXELSY);
	}

	/*
		calculating # pixels with double values
	*/
	int pointToPixel( double ptValue ) const
	{
		return int(ptValue * double(GetDeviceCaps(m_device, LOGPIXELSY)) / 72.0 +0.5);
	}
	int picaToPixel( double pcValue ) const
	{
		return int(pcValue * double(GetDeviceCaps(m_device, LOGPIXELSY)) / 6.0 +0.5);
	}
	int cmToPixel( double cmValue ) const
	{
		return int(cmValue * double(GetDeviceCaps(m_device, LOGPIXELSY)) / 2.54 +0.5);
	}
	int mmToPixel( double mmValue ) const
	{
		return int(mmValue * double(GetDeviceCaps(m_device, LOGPIXELSY)) / 25.4 +0.5);
	}
	int inToPixel( double inValue ) const
	{
		return int(inValue * double(GetDeviceCaps(m_device, LOGPIXELSY)) + 0.5);
	}


	/*
		calculating pixels to other dimensions
	*/
	double pixelToPoint( int pxValue ) const
	{
		return (double)pxValue * 72.0 / (double)GetDeviceCaps(m_device, LOGPIXELSY);
	}
	double pixelToPica( int pxValue ) const
	{
		return (double)pxValue * 6.0 / (double)GetDeviceCaps(m_device, LOGPIXELSY);
	}
	double pixelToCM( int pxValue ) const
	{
		return (double)pxValue * 2.54 / (double)GetDeviceCaps(m_device, LOGPIXELSY);
	}
	double pixelToMM( int pxValue ) const
	{
		return (double)pxValue * 25.4 / (double)GetDeviceCaps(m_device, LOGPIXELSY);
	}
	double pixelToIN( int pxValue ) const
	{
		return (double)pxValue / (double)GetDeviceCaps(m_device, LOGPIXELSY);
	}
};

class WindowDevice : public Device
{
	protected:
	const BasicWindow *m_window;
	WindowDevice( const BasicWindow *theWindow, HDC context ) 
	: Device( context ), m_window( theWindow )
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
		m_window->releaseDC( m_device );
		m_device = nullptr;
	}
};

class RepaintDevice : public WindowDevice
{
	PAINTSTRUCT	m_ps;

	friend class CallbackWindow;

	private:
	RepaintDevice( const BasicWindow *theWindow ) 
	: WindowDevice( theWindow, theWindow->beginPaint( &m_ps ) )
	{}
	~RepaintDevice()
	{
		m_window->endPaint( &m_ps );
		m_device = nullptr;
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
		m_device = nullptr;		// don't delete
	}
};

class MemoryDevice : public Device
{
	const Device	&m_target;
	Bitmap			m_bitmap;

	public:
	MemoryDevice( const Device &target, const Size &size ) 
	: Device( CreateCompatibleDC ( target ) ),
	m_target( target ),
	m_bitmap( *this )
	{
		m_bitmap.create( target, size );
	}
	MemoryDevice( const Device &target, const Bitmap &bitmap ) 
	: Device( CreateCompatibleDC ( target ) ),
	m_target( target ),
	m_bitmap( bitmap )
	{
		m_bitmap.connect( *this );
	}

	void drawToWindow()
	{
		BitBlt( m_target, 0, 0, m_bitmap.getWidth(), m_bitmap.getHeight(), *this, 0, 0, SRCCOPY );
	}
#if 0
	void drawScaledToWindow()
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
		StartDoc( m_device, &dInfo );
	}
	void startPage()	const
	{
		StartPage( m_device );
	}
	void endPage()	const
	{
		EndPage( m_device );
	}
	void endDocument()	const
	{
		EndDoc( m_device );
	}
	void printText( const char *document, const char *text ) const;
};

class MetaDevice : public Device
{
	HMETAFILE		m_metaFile;
	public:
	MetaDevice( const char *fileName = nullptr ) : Device( CreateMetaFile( fileName ) )
	{
		m_metaFile = nullptr;
	}
	~MetaDevice()
	{
//		if( m_metaFile )						// not yet
//			DeleteMetaFile( m_metaFile );
	}

	HMETAFILE createFile()
	{
		if( !m_metaFile )
			m_metaFile = CloseMetaFile( m_device );

		return m_metaFile;
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

