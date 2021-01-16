/*
		Project:		Windows Class Library
		Module:			splash.c
		Description:	Helper function to display a splash screen
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
inline HINSTANCE GetInstanceId( void )
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

static const char 	szStartupClass[] = "STARTUP";
static int		menuHeight		= 0;
static HWND		hwndStartWin	= 0;
static HDC		hdcMemory		= 0;

static int		bitmapWidth, bitmapHeight;
static char		*textBuffer		= NULL;

/* --------------------------------------------------------------------- */
/* ----- exported datas ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- module functions ---------------------------------------------- */
/* --------------------------------------------------------------------- */

static bool RegisterStartupWindow( void )
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

	wc.lpszClassName = szStartupClass;

	if( !RegisterClass(&wc) )
	{
		return startupRegistered;
	}

	menuHeight = GetSystemMetrics( SM_CYMENU );

	startupRegistered = true;
	return startupRegistered;
}

static void DrawWindow( HDC hdc )
{
	BitBlt( hdc, 0, menuHeight, bitmapWidth, bitmapHeight, hdcMemory, 0, 0, SRCCOPY );
	if( textBuffer )
	{
		SIZE	size;
		int		len = (int)strlen( textBuffer );

		SetTextAlign( hdc, TA_TOP );
		TextOut( hdc, 0, 0, textBuffer, len );

		GetTextExtentPoint32( hdc, textBuffer, len, &size );
		SelectObject( hdc, GetStockObject( WHITE_BRUSH ) );
		SelectObject( hdc, GetStockObject( WHITE_PEN ) );
		Rectangle( hdc, size.cx, 0, bitmapWidth, size.cy );
	}
}

/* --------------------------------------------------------------------- */
/* ----- entry points -------------------------------------------------- */
/* --------------------------------------------------------------------- */

LRESULT _export pascal StartupWindowProc( HWND window, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_PAINT && (window == hwndStartWin || hwndStartWin == NULL) )
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
	LOGPALETTE	*lPal = 0;
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
		lPal = (LOGPALETTE *)malloc( sizeof( LOGPALETTE ) + numColors * sizeof( PALETTEENTRY ) );
		lPal->palVersion = 0x300;
		lPal->palNumEntries = (WORD)numColors;
		for( i=0; i<numColors; i++ )
		{
			lPal->palPalEntry[i].peRed = pBitmap->bmiColors[i].rgbRed;
			lPal->palPalEntry[i].peGreen = pBitmap->bmiColors[i].rgbGreen;
			lPal->palPalEntry[i].peBlue = pBitmap->bmiColors[i].rgbBlue;
			lPal->palPalEntry[i].peFlags = 0;
		}
		hPal = CreatePalette( lPal );
		bitmapData = (void*)&(pBitmap->bmiColors[i]);
	}

	bitmapWidth = pBitmap->bmiHeader.biWidth;
	bitmapHeight = pBitmap->bmiHeader.biHeight;

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
	hwndStartWin = CreateWindow( szStartupClass, title,
								 title && *title ? WS_OVERLAPPED : WS_POPUP, (int)xPos, (int)yPos,
								 (int)width, (int)height+menuHeight, NULL, NULL,
								 GetInstanceId(), NULL );

	textBuffer = strdup("Loading program, please wait");
	ShowWindow( hwndStartWin, SW_SHOW );

	// SetWindowPos( hwndStartWin, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );

	hdc = GetDC( hwndStartWin );

	SelectPalette( hdc, hPal, TRUE );
	RealizePalette( hdc );

	hbmpMyBitmap = CreateDIBitmap( hdc, &pBitmap->bmiHeader, CBM_INIT, bitmapData, pBitmap, DIB_RGB_COLORS );

	hdcMemory = CreateCompatibleDC( hdc );
	SelectObject( hdcMemory, hbmpMyBitmap );

	DrawWindow( hdc );

	DeleteObject( hbmpMyBitmap );

	if( hPal )
		DeleteObject( hPal );

	if( lPal )
		free( lPal );

	FreeResource( hBitmap );

	ReleaseDC( hwndStartWin, hdc );
}

void print2StartWindow( const char *format, ... )
{
	va_list		args;
	char		logBuffer[10240];

	if( !hwndStartWin )
/***/	return;

	va_start( args, format );
	vsprintf( logBuffer, format, args );
	va_end( args );

	if( textBuffer )
		free( textBuffer );
	textBuffer = strdup(logBuffer);

	{
		HDC	hdc = GetDC( hwndStartWin );
		DrawWindow( hdc );
		ReleaseDC( hwndStartWin, hdc );
//		UpdateWindow( hwndStartWin );
		idleLoop();
	}
}

void closeStartup( void )
{
	DestroyWindow( hwndStartWin );

	DeleteDC( hdcMemory );

	if( textBuffer )
		free( textBuffer );

	hwndStartWin = NULL;
	hdcMemory = NULL;

	textBuffer = NULL;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -a.
#	pragma option -p.
#endif

