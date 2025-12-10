/*
		Project:		Windows Class Library
		Module:			splash.c
		Description:	Helper function to display a splash screen
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

/* --------------------------------------------------------------------- */
/* ----- includes ------------------------------------------------------ */
/* --------------------------------------------------------------------- */

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/gaklib.h>
#include <winlib/winlib.h>

/* --------------------------------------------------------------------- */
/* ----- module switches ----------------------------------------------- */
/* --------------------------------------------------------------------- */

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -a4
#	pragma option -pc
#endif

/* --------------------------------------------------------------------- */
/* ----- constants ----------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- type definitions ---------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- macros -------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/*
inline HINSTANCE GetInstanceId()
{
	return 0;  // ok for Win95 und NT to display startup window
}
*/

#define GetInstanceId() ((HINSTANCE)0)

/* --------------------------------------------------------------------- */
/* ----- prototypes ---------------------------------------------------- */
/* --------------------------------------------------------------------- */

LRESULT _export pascal StartupWindowProc( HWND window, UINT msg, WPARAM wParam, LPARAM lParam );

/* --------------------------------------------------------------------- */
/* ----- imported datas ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- module statics ------------------------------------------------ */
/* --------------------------------------------------------------------- */

static const char	s_StartupClass[] = "STARTUP";
static int			s_menuHeight	= 0;
static HWND			s_hwndStartWin	= 0;
static HDC			s_hdcMemory		= 0;

static int			s_bitmapWidth, s_bitmapHeight;
static char			*s_textBuffer	= nullptr;

/* --------------------------------------------------------------------- */
/* ----- exported datas ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- module functions ---------------------------------------------- */
/* --------------------------------------------------------------------- */

static bool RegisterStartupWindow()
{
	static bool	startupRegistered = false;

	WNDCLASS 	wc;

	if( startupRegistered )
	{
		return startupRegistered;
	}

	/* Register the window class. */
	wc.style         = 0;
	wc.lpfnWndProc   = StartupWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetInstanceId();
	wc.hInstance     = 0;
	wc.hIcon         = 0;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;

	wc.lpszClassName = s_StartupClass;

	if( !RegisterClass(&wc) )
	{
		return startupRegistered;
	}

	s_menuHeight = GetSystemMetrics( SM_CYMENU );

	startupRegistered = true;
	return startupRegistered;
}

static void DrawWindow( HDC hdc )
{
	BitBlt( hdc, 0, s_menuHeight, s_bitmapWidth, s_bitmapHeight, s_hdcMemory, 0, 0, SRCCOPY );
	if( s_textBuffer )
	{
		SIZE	size;
		int		len = (int)strlen( s_textBuffer );

		SetTextAlign( hdc, TA_TOP );
		TextOut( hdc, 0, 0, s_textBuffer, len );

		GetTextExtentPoint32( hdc, s_textBuffer, len, &size );
		SelectObject( hdc, GetStockObject( WHITE_BRUSH ) );
		SelectObject( hdc, GetStockObject( WHITE_PEN ) );
		Rectangle( hdc, size.cx, 0, s_bitmapWidth, size.cy );
	}
}

/* --------------------------------------------------------------------- */
/* ----- entry points -------------------------------------------------- */
/* --------------------------------------------------------------------- */

LRESULT _export pascal StartupWindowProc( HWND window, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_PAINT && (window == s_hwndStartWin || s_hwndStartWin == nullptr) )
	{
		PAINTSTRUCT	ps;
		HDC	hDC = BeginPaint( window, &ps );
		DrawWindow( hDC );
		EndPaint( window, &ps );

/***/	return 0;
	}

	return DefWindowProc( window, msg, wParam, lParam );
}

void openStartup( const char *title, const char *bitmap )
{
	HRSRC		hRes;
	HGLOBAL		hBitmap;
	BITMAPINFO	*pBitmap;
	long		i, numColors;
	gak::Buffer<LOGPALETTE>	_lPal(nullptr);
	HPALETTE	hPal = 0;
	void		*bitmapData = 0;

	HDC			hdc;
	HBITMAP		hbmpMyBitmap;
	long		maxX, xPos, width;
	long		maxY, yPos, height;

	// showBusyMouse();

	hRes = FindResource( NULL, bitmap, RT_BITMAP );
	hBitmap	= LoadResource( NULL, hRes );
	pBitmap = (BITMAPINFO *)LockResource( hBitmap );

	if( pBitmap->bmiHeader.biClrUsed )
		numColors = (int)(pBitmap->bmiHeader.biClrUsed);
	else
		numColors = 1<<pBitmap->bmiHeader.biBitCount;

	if( numColors && numColors <= 256 )
	{
		_lPal = malloc( sizeof( LOGPALETTE ) + numColors * sizeof( PALETTEENTRY ) );
		_lPal->palVersion = 0x300;
		_lPal->palNumEntries = (WORD)numColors;
		for( i=0; i<numColors; i++ )
		{
			_lPal->palPalEntry[i].peRed = pBitmap->bmiColors[i].rgbRed;
			_lPal->palPalEntry[i].peGreen = pBitmap->bmiColors[i].rgbGreen;
			_lPal->palPalEntry[i].peBlue = pBitmap->bmiColors[i].rgbBlue;
			_lPal->palPalEntry[i].peFlags = 0;
		}
		hPal = CreatePalette( _lPal );
		bitmapData = (void*)&(pBitmap->bmiColors[i]);
	}

	s_bitmapWidth = pBitmap->bmiHeader.biWidth;
	s_bitmapHeight = pBitmap->bmiHeader.biHeight;

	maxX  = GetSystemMetrics( SM_CXFULLSCREEN );
	width = pBitmap->bmiHeader.biWidth;
	xPos  = (maxX - width) / 2;

	maxY = GetSystemMetrics( SM_CYFULLSCREEN );
	height = pBitmap->bmiHeader.biHeight + GetSystemMetrics( SM_CYCAPTION );
	yPos  = (maxY - height) / 2;

	if( !RegisterStartupWindow() )
	{
/*@*/	return;
	}

	/* Create the window. */
	s_hwndStartWin = CreateWindow( s_StartupClass, title,
								 title && *title ? WS_OVERLAPPED : WS_POPUP, (int)xPos, (int)yPos,
								 (int)width, (int)height+s_menuHeight, NULL, NULL,
								 GetInstanceId(), NULL );

	s_textBuffer = strdup("Loading program, please wait");
	ShowWindow( s_hwndStartWin, SW_SHOW );

	// SetWindowPos( hwndStartWin, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );

	hdc = GetDC( s_hwndStartWin );

	SelectPalette( hdc, hPal, TRUE );
	RealizePalette( hdc );

	hbmpMyBitmap = CreateDIBitmap( hdc, &pBitmap->bmiHeader, CBM_INIT, bitmapData, pBitmap, DIB_RGB_COLORS );

	s_hdcMemory = CreateCompatibleDC( hdc );
	SelectObject( s_hdcMemory, hbmpMyBitmap );

	DrawWindow( hdc );

	DeleteObject( hbmpMyBitmap );

	if( hPal )
		DeleteObject( hPal );

	FreeResource( hBitmap );

	ReleaseDC( s_hwndStartWin, hdc );
}

void print2StartWindow( const char *format, ... )
{
	va_list		args;
	char		logBuffer[10240];

	if( !s_hwndStartWin )
/***/	return;

	va_start( args, format );
	vsprintf( logBuffer, format, args );
	va_end( args );

	if( s_textBuffer )
		free( s_textBuffer );
	s_textBuffer = strdup(logBuffer);

	{
		HDC	hdc = GetDC( s_hwndStartWin );
		DrawWindow( hdc );
		ReleaseDC( s_hwndStartWin, hdc );
//		UpdateWindow( s_hwndStartWin );
		idleLoop();
	}
}

void closeStartup()
{
	DestroyWindow( s_hwndStartWin );

	DeleteDC( s_hdcMemory );

	if( s_textBuffer )
		free( s_textBuffer );

	s_hwndStartWin = nullptr;
	s_hdcMemory = nullptr;

	s_textBuffer = nullptr;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -a.
#	pragma option -p.
#endif

