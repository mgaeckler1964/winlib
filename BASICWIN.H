/*
		Project:		Windows Class Library
		Module: 		BASICWIN.H
		Description:	The Basic Window Class
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

#ifndef BASIC_WIN_H
#define BASIC_WIN_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

#ifndef WIN_SCROLL
#define WIN_SCROLL		1
#define WIN_SCROLL_INFO	0
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#ifdef __BORLANDC__
#	pragma option -w-8084
#endif

#include <stdlib.h>

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/gaklib.h>
#include <gak/string.h>
#include <gak/xml.h>

#include <winlib/layout.h>
#include <winlib/brush.h>
#include <winlib/font.h>
#include <winlib/menu.h>
#include <winlib/winapp.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

#ifdef _MSC_VER
#	pragma warning( disable: 4355 4365 4512 4625 4626 )
#	pragma warning( 1: 4061 4063 4263 )
#	pragma comment( lib, "winlib.lib" )
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

extern "C"
{
	LRESULT PASCAL WindowProc( HWND, UINT, WPARAM, LPARAM );
	LRESULT PASCAL MDIchildWindowProc( HWND, UINT, WPARAM, LPARAM );
};

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

inline STRING loadString( int id )
{
	return appObject->loadString( id );
}

inline Point MAKEPOINT( DWORD lParam )
{
	Point	result;

	result.x = LOWORD( lParam );
	result.y = HIWORD( lParam );

	return result;
}

inline Size MAKESIZE( DWORD lParam )
{
	Size	result;

	result.width = LOWORD( lParam );
	result.height = HIWORD( lParam );

	return result;
}

inline bool WritePrivateProfileInt( LPSTR	appName,
									LPSTR	key,
									int 	value,
									LPSTR	file )
{
	char	buffer[32];

	return WritePrivateProfileString( appName, key,
									  itoa( value, buffer, 10 ),
									  file )
			? true : false;
}

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

template < typename enumT >
class EnumWrapper
{
	enumT code;

	public:
	EnumWrapper( enumT val ) : code(val) {}
	bool operator == ( enumT val ) const
	{
		return code == val;
	}
	bool operator != ( enumT val ) const
	{
		return code != val;
	}
};
enum _SuccessCode { scSUCCESS, scERROR };
typedef EnumWrapper<_SuccessCode>	SuccessCode;


typedef unsigned short	WindowClass_t;
const WindowClass_t ForeignWindowFlag	= 0x00;
const WindowClass_t WinlibWindowFlag	= 0x01;
const WindowClass_t CallbackWindowFlag	= 0x02;
const WindowClass_t ControlWindowFlag	= 0x04;
const WindowClass_t SearchDialogFlag	= 0x08;
const WindowClass_t FileDialogFlag		= 0x10;

typedef gak::UnorderedSet<BasicWindow*>		ChildWindows;
typedef gak::ArrayOfPointer<BasicWindow>	OwnedWindows;

/*
	BASIC WINDOW CLASS
	==================

	The "Basic Window Class" describes any window. It is the BASE-Class for
	all other window classes.
*/

class FrameChild;

class ForeignWindow
{
	HWND				m_winHandle;
	unsigned long		m_style, m_exStyle;
	bool				m_caret;
#if !WIN_SCROLL
	bool				hScroll, vScroll;
#endif
	WindowClass_t		m_windowType;

	/*
		device contexts
	*/
	private:
	friend class DrawDevice;
	friend class RepaintDevice;
	friend class Font;
	
	HDC getDC( void ) const
	{
		return GetDC( m_winHandle );
	}
	bool releaseDC( HDC context ) const
	{
		return ReleaseDC( m_winHandle, context ) ? true : false;
	}
	HDC beginPaint( PAINTSTRUCT *ps ) const
	{
		return BeginPaint( m_winHandle, ps );
	}
	void endPaint( const PAINTSTRUCT *ps ) const
	{
		EndPaint( m_winHandle, ps );
	}

	// (de)contructors
	protected:
	ForeignWindow( WindowClass_t windowType )
	{
		m_windowType = windowType;
		m_winHandle	= NULL;

		m_exStyle = m_style = 0;
		m_caret = false;

#if !WIN_SCROLL
		hScroll = vScroll = false;
#endif
	}

	public:
	ForeignWindow( HWND theHandle )
	{
		m_windowType = ForeignWindowFlag;
		m_winHandle	= theHandle;

		m_style = getStyle();
		m_exStyle = getExStyle();
		m_caret = false;

#if !WIN_SCROLL
		hScroll = style & WS_HSCROLL;
		vScroll = style & WS_VSCROLL;
#endif
	}
	const ForeignWindow &operator = ( HWND theHandle )
	{
		m_windowType = ForeignWindowFlag;
		m_winHandle	= theHandle;

		m_style = getStyle();
		m_exStyle = getExStyle();
		m_caret = false;

#if !WIN_SCROLL
		hScroll = style & WS_HSCROLL;
		vScroll = style & WS_VSCROLL;
#endif
		return *this;
	}

	virtual ~ForeignWindow();


	/*
		the handle
	*/
	void setHandle( HWND handle )
	{
		m_winHandle = handle;
	}
	void clrHandle( void )			// for the dialog & child windows
	{
		m_winHandle = HWND(-1);		// to ensure, that this object
									// no longer recieves messages
	}
	bool isValid() const
	{
		return int(m_winHandle)>0;
	}
	HWND handle( void ) const
	{
		return m_winHandle;
	}
	virtual SuccessCode close( void );
	operator bool ( void ) const
	{
		return isValid();
	}
	bool operator ! ( void ) const
	{
		return !isValid();
	}

	/*
		control id
	*/
	void setId( int controlId )
	{
		if( isValid() )
		{
			SetWindowLong( m_winHandle, GWL_ID, controlId );
		}
	}
	int getId( void ) const
	{
		return isValid() ? GetWindowLong( m_winHandle, GWL_ID ) : -1;
	}

	/*
		window types
	*/
	virtual STRING getWindowClassName( void ) const;
	WindowClass_t getWindowClass( void ) const
	{
		return m_windowType;
	}

	/*
		window hiararchy
	*/
	void mouseToChildPosition( const Point &position, const ForeignWindow *child, Point *newPosition )
	{
		*newPosition = position;
		mouseToChildPosition( child, newPosition );
	}
	void mouseToChildPosition( const ForeignWindow *child, Point *position )
	{
		ClientToScreen( handle(), position );
		ScreenToClient( child->handle(), position );
	}

	/*
		searching for windows
	*/
	static ForeignWindow findWindow( const char *className=NULL, const char *title=NULL )
	{
		return ForeignWindow( ::FindWindow( className, title ) );
	}

	/*
		send a message to a window
	*/

	LRESULT message( UINT message, WPARAM wParam=0, LPARAM lParam=0 ) const
	{
		return SendMessage( m_winHandle, message, wParam, lParam );
	}
	void postMessage( UINT message, WPARAM wParam=0, LPARAM lParam=0 ) const
	{
		PostMessage( m_winHandle, message, wParam, lParam );
	}

	/*
		message box
	*/
	int messageBox( const char	*text,
					const char	*title=NULL,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		return MessageBox( handle(), LPCSTR(text), LPCSTR(title), type );
	}
	int messageBox( int			textId,
					int			titleId=0,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		STRING	text = loadString( textId );

		STRING	title;
		if( titleId )
			title = loadString( titleId );

		return messageBox( text, title, type );
	}
	int messageBox( const char	*text,
					int			titleId,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		STRING	title;
		if( titleId )
			title = loadString( titleId );

		return messageBox( text, title, type );
	}
	int messageBox( int			textId,
					const char	*title,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		STRING	text = loadString( textId );

		if( !text.isEmpty() )
			return messageBox( text, title, type );
		else
			return 0;
	}

	template<typename TextT, typename TitleT>
	int messageError( TextT text, TitleT title=0 )
	{
		return messageBox( text, title, MB_APPLMODAL|MB_ICONERROR|MB_OK );
	}

	/*
		displaying window
	*/
	void show( int nCmdShow=SW_SHOW )
	{
		if( nCmdShow != SW_HIDE )
			m_style |= WS_VISIBLE; // addStyle( WS_VISIBLE );
		else
			m_style &= ~WS_VISIBLE; // removeStyle( WS_VISIBLE );
		if( isValid() )
		{
			ShowWindow( m_winHandle, nCmdShow );
		}

#if !WIN_SCROLL
		if( !hScroll )
			hideVertScrollBar();
		if( !vScroll )
			hideHorizScrollBar();
#endif
	}
	void hide( void )
	{
		show( SW_HIDE );
	}
	void minimize( void )
	{
		show( SW_MINIMIZE );
	}
	void maximize( void )
	{
		show( SW_MAXIMIZE );
	}
	bool isVisible() const
	{
		return getStyle() & WS_VISIBLE;
	}

	/*
		focus/enable/disable control
	*/
	void focus( void )
	{
		if( IsIconic( m_winHandle ) )
		{
			show( SW_RESTORE );
		}
		SetFocus( m_winHandle );
	}
	bool hasFocus( void ) const
	{
		return GetForegroundWindow() == handle();
	}
	void enable( void ) const
	{
		EnableWindow( m_winHandle, true );
	}
	void disable( void ) const
	{
		EnableWindow( m_winHandle, false );
	}
	bool isEnabled( void ) const
	{
		return IsWindowEnabled( m_winHandle ) ? true : false;
	}
	/*
		force redraw
	*/
	void invalidateWindow( bool clear=true ) const
	{
		InvalidateRect( m_winHandle, NULL, clear );
	}
	void invalidateRect( const RectBorder &rect, bool clear=true ) const
	{
		InvalidateRect( m_winHandle, &rect, clear );
	}

	// modifying the window
	/*
		the style
		=========
	*/
	unsigned long getStyle( void ) const
	{
		return isValid() ? GetWindowLong( m_winHandle, GWL_STYLE ) : m_style;
	}
	unsigned long getExStyle( void ) const
	{
		return isValid() ? GetWindowLong( m_winHandle, GWL_EXSTYLE ) : m_exStyle;
	}
	void setStyle( unsigned long style )
	{
		m_style = style;
		if( isValid() )
		{
			SetWindowLong( m_winHandle, GWL_STYLE, style );
		}
	}
	void setExStyle( unsigned long exStyle )
	{
		m_exStyle = exStyle;
		if( isValid() )
		{
			SetWindowLong( m_winHandle, GWL_EXSTYLE, exStyle );
		}
	}
	unsigned long addStyle( unsigned long style )
	{
		unsigned long oldStyle = getStyle();
		unsigned long newStyle = oldStyle | style;

		if( newStyle != oldStyle )
			setStyle( newStyle );

		return newStyle;
	}
	unsigned long addExStyle( unsigned long style )
	{
		unsigned long oldStyle = getExStyle();
		unsigned long newStyle = oldStyle | style;

		if( newStyle != oldStyle )
			setExStyle( newStyle );

		return newStyle;
	}
	unsigned long removeStyle( unsigned long style )
	{
		unsigned long oldStyle = getStyle();
		unsigned long newStyle = oldStyle & ~style;

		if( newStyle != oldStyle )
			setStyle( newStyle );

		return newStyle;
	}
	unsigned long removeExStyle( unsigned long style )
	{
		unsigned long oldStyle = getExStyle();
		unsigned long newStyle = oldStyle & ~style;

		if( newStyle != oldStyle )
			setExStyle( newStyle );

		return newStyle;
	}
	bool hasStyle( unsigned long style ) const
	{
		return (getStyle() & style) == style;
	}
	bool hasExStyle( unsigned long style ) const
	{
		return (getExStyle() & style) == style;
	}
	bool hasAnyStyle( unsigned long style ) const
	{
		return (getStyle() & style) != 0;
	}
	bool hasAnyExStyle( unsigned long style ) const
	{
		return (getStyle() & style) != 0;
	}

	/*
		the caption/text
		================
	*/
	int getTextLength( void ) const
	{
		return int(message( WM_GETTEXTLENGTH ));
	}
	STRING getText( bool oemConvert = false ) const;
	void setText( const STRING &text )
	{
		if( isValid() )
		{
			SetWindowText( m_winHandle, text.convertToCharset( STR_ANSI ) );
		}
	}
	void setText( int id )
	{
		setText( loadString( id ) );
	}

	/*
		size and position
		=================
	*/
	void resize( int width, int height )
	{
		if( isValid() )
		{
			SetWindowPos( m_winHandle, 0, -1, -1, width, height, SWP_NOMOVE|SWP_NOZORDER );
		}
	}
	void move( int xPos, int yPos )
	{
		if( isValid() )
		{
			SetWindowPos( m_winHandle, 0, xPos, yPos, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
		}
	}
	void sizeNmove( int xPos, int yPos, int width, int height )
	{
		if( isValid() )
		{
			SetWindowPos( m_winHandle, 0, xPos, yPos, width, height, SWP_NOZORDER );
		}
	}

	Size getClientSize( void ) const
	{
		if( isValid() )
		{
			RectBorder	rect = getClientRectangle();
			return Size( rect.getWidth(), rect.getHeight() );
		}
		else
		{
			return Size( 0, 0 );
		}
	}

	RectBorder getClientRectangle( void ) const
	{
		RectBorder rect;
		GetClientRect( m_winHandle, &rect );
		return rect;
	}

	RectBorder getWindowRectangle( void ) const
	{
		RectBorder rect;
		GetWindowRect( m_winHandle, &rect );
		return rect;
	}
	RectBorder getRelativeRectangle( void ) const
	{
		RectBorder rect = getWindowRectangle();

		HWND	parent;

		parent = GetParent( m_winHandle );
		if( parent )
		{
			Point	corner = rect.getTopLeft();
			ScreenToClient( parent, &corner );
			rect.setTopLeft( corner );

			corner = rect.getBottomRight();
			ScreenToClient( parent, &corner );
			rect.setBottomRight( corner );
		}

		return rect;
	}
	Size getSize( void )
	{
		if( isValid() )
		{
			RectBorder	rect = getWindowRectangle();
			return Size( rect.getWidth(), rect.getHeight() );
		}
		else
		{
			return Size( 0, 0 );
		}
	}
	void getPosition( int *xPos, int *yPos ) const
	{
		RectBorder	rect = getWindowRectangle();
		*xPos = rect.left;
		*yPos = rect.top;
	}
	void getRelativePosition( int *xPos, int *yPos ) const
	{
		RectBorder	rect = getRelativeRectangle();
		*xPos = rect.left;
		*yPos = rect.top;
	}
	Point getPosition( void ) const
	{
		RectBorder	rect = getWindowRectangle();
		return Point( rect.left, rect.top );
	}
	Point getRelativePosition( void ) const
	{
		RectBorder	rect = getRelativeRectangle();
		return Point( rect.left, rect.top );
	}

	Point winPoint2Screen( const Point &winPnt )
	{
		RectBorder	rect = getWindowRectangle();
		return Point(winPnt.x + rect.left,winPnt.y + rect.top);
	}


	/*
		Frames/Borders
		==============
	*/
	void addBorder( void )
	{
		addStyle( WS_BORDER );
	}
	void remoweBorder( void )
	{
		removeStyle( WS_BORDER );
	}

	// wtf is that here? this is for static controls!!!
	void addFrame( void )
	{
		addStyle( SS_BLACKFRAME );
	}
	void remoweFrame( void )
	{
		removeStyle( SS_BLACKFRAME );
	}

	/*
		Scrollbars
		==========
	*/
	void showVertScrollBar( int min, int max )
	{
#if !WIN_SCROLL
		vScroll = true;
#endif
		SetScrollRange( m_winHandle, SB_VERT, min, max, true );
		ShowScrollBar( m_winHandle, SB_VERT, true );
	}
	void hideVertScrollBar( void )
	{
#if !WIN_SCROLL
		vScroll = false;
#endif
		ShowScrollBar( m_winHandle, SB_VERT, false );
	}
	int getVertScrollPos( void ) const
	{
		return GetScrollPos( m_winHandle, SB_VERT );
	}
	int setVertScrollPos( int newPos ) const
	{
		return SetScrollPos( m_winHandle, SB_VERT, newPos, true );
	}
	bool hasVertScrollBar( void ) const
	{
#if WIN_SCROLL
		bool	vScroll;

		if( !hasStyle( WS_VSCROLL ) )
			vScroll = false;
		else
		{
#	if WIN_SCROLL_INFO
			SCROLLBARINFO	si;

			si.cbSize = sizeof( si );
			if( !GetScrollBarInfo( handle(), OBJID_VSCROLL, &si ) )
				vScroll = false;
			else
				vScroll = (si.rgstate[0] & STATE_SYSTEM_INVISIBLE|STATE_SYSTEM_UNAVAILABLE) == 0;
#	else
			vScroll = true;
#	endif
		}
#endif
		return vScroll;
	}

	void showHorizScrollBar( int min, int max )
	{
		SetScrollRange( m_winHandle, SB_HORZ, min, max, true );
		ShowScrollBar( m_winHandle, SB_HORZ, true );
#if !WIN_SCROLL
		hScroll = true;
#endif
	}
	void hideHorizScrollBar( void )
	{
		ShowScrollBar( m_winHandle, SB_HORZ, false );
#if !WIN_SCROLL
		hScroll = false;
#endif
	}
	int getHorizScrollPos( void ) const
	{
		return GetScrollPos( m_winHandle, SB_HORZ );
	}
	int setHorizScrollPos( int newPos ) const
	{
		return SetScrollPos( m_winHandle, SB_HORZ, newPos, true );
	}
	bool hasHorizScrollBar( void ) const
	{
#if WIN_SCROLL
		bool	hScroll;

		if( !hasStyle( WS_HSCROLL ) )
			hScroll = false;
		else
		{
#	if WIN_SCROLL_INFO
			SCROLLBARINFO	si;

			si.cbSize = sizeof( si );
			if( !GetScrollBarInfo( handle(), OBJID_HSCROLL, &si ) )
				hScroll = false;
			else
				hScroll = (si.rgstate[0] & STATE_SYSTEM_INVISIBLE|STATE_SYSTEM_UNAVAILABLE) == 0;
#	else
			hScroll = true;
#	endif
		}
#endif
		return hScroll;
	}
	void scrollHorizontal( int position )
	{
		message( WM_HSCROLL, MAKELONG( SB_THUMBPOSITION, WORD(position) ) );	
	}
	void scrollVertical( int position )
	{
		message( WM_VSCROLL, MAKELONG( SB_THUMBPOSITION, (WORD)position ) );
	}

	// Class Style
	void redrawResize( void ) const
	{
		DWORD classStyle;

		classStyle = GetClassLong( m_winHandle, GCL_STYLE );
		classStyle |= CS_HREDRAW|CS_VREDRAW;
		SetClassLong( m_winHandle, GCL_STYLE, classStyle );
	}
	void noRedrawResize( void ) const
	{
		DWORD classStyle;

		classStyle = GetClassLong( m_winHandle, GCL_STYLE );
		classStyle &= ~(CS_HREDRAW|CS_VREDRAW);
		SetClassLong( m_winHandle, GCL_STYLE, classStyle );
	}
	/*
		icon
	*/
	void setIcon( HICON icon )
	{
		// SetClassLong( winHandle, GCL_HICON, (DWORD)icon );
		message( WM_SETICON, ICON_BIG, (LPARAM)icon );
	}
	void setIcon( const char *iconName )
	{
		setIcon( appObject->loadIcon( iconName ) );
	}
	void setIcon( int iconId )
	{
		setIcon( appObject->loadIcon( iconId ) );
	}

	/*
		Caret
	*/
	bool hasCaret( void ) const
	{
		return m_caret;
	}
	void createCaret( int x, int y, int height )
	{
		m_caret = true;
		CreateCaret( m_winHandle, NULL, 0, height );
		SetCaretPos( x, y );
		ShowCaret( m_winHandle );
	}
	void showCaret( void ) const
	{
		ShowCaret( m_winHandle );
	}
	void moveCaret( int x, int y ) const
	{
		SetCaretPos( x, y );
	}
	void hideCaret( void ) const
	{
		HideCaret( m_winHandle );
	}
	void destroyCaret( void )
	{
		m_caret = false;
		DestroyCaret();
	}
};

class BasicWindow : public ForeignWindow
{
	private:
	const BasicWindow	*m_parent;

	BasicWindow			*m_owner;
	OwnedWindows		m_ownedWindows;
	ChildWindows		m_childWindows;

	Brush				m_backGroundBrush;
	Font				m_font;

	int 				m_controlId;

	STRING				m_caption;
	STRING				m_toolHelp;

	LayoutData			*m_layoutData;
	bool				m_autoWidth, m_autoHeight;
	int					m_x, m_y, m_width, m_height,
						m_widthOffset, m_heightOffset;

	int					m_tag;

	gak::xml::Element	*m_resource;

	void addWindow( BasicWindow *child )
	{
		m_ownedWindows += child;
	}

	// do nothing, just forbid
	BasicWindow( const BasicWindow &source );
	BasicWindow &operator = ( const BasicWindow &source );

	// (de)contructors
	void init( void )
	{
		m_autoWidth = m_autoHeight = false;
		m_x = m_y = m_width = m_height = CW_USEDEFAULT;
		m_widthOffset = m_heightOffset = 0;

		m_resource	= NULL;
		m_layoutData= NULL;
		m_owner		= NULL;
		m_parent	= NULL;
		m_controlId	= -1;

		m_tag = 0;
	}
	void readWindowData()
	{
		if( m_controlId <= 0 )
		{
			m_controlId = GetWindowLong( handle(), GWL_ID );
		}
		if( m_caption.isEmpty() )
		{
			m_caption = getText();
		}
	}

	public:
	~BasicWindow();
	BasicWindow( WindowClass_t windowType, BasicWindow *owner ) : ForeignWindow( windowType ), m_font( this )
	{
		init();
		m_owner = owner;
		if( owner )
		{
			owner->addWindow( this );
		}
	}

	/*
		creation
	*/
	protected:

	friend
	#ifdef __WIN64__
	UINT_PTR
	#else
	UINT 
	#endif
	APIENTRY OFNHookProc( HWND, UINT, WPARAM, LPARAM );
	void setHandle( HWND handle )	// called by WindowProc
	{
		doEnterFunctionEx( gakLogging::llDetail, "BasicWindow::setHandle" );

		ForeignWindow::setHandle( handle );
		readWindowData();
	}
	SuccessCode create( BasicWindow *parent, Menu *menu=NULL, LPVOID lpParam=NULL )
	{
		STRING className = getWindowClassName();

		setParent( parent );
		unsigned long	style = getStyle();
		unsigned long	exStyle = getExStyle();
#if !WIN_SCROLL
		bool			hScroll = (style & WS_HSCROLL);
		bool			vScroll = (style & WS_VSCROLL);

		style |= WS_HSCROLL|WS_VSCROLL;
#endif

		if( parent && hasStyle( WS_CHILD ) )
		{
			parent->m_childWindows.addElement( this );
		}

		HWND winHandle = CreateWindowEx(
			exStyle, 
			LPCSTR(className), 
			m_caption.isEmpty() ? NULL : LPCSTR(m_caption), 
			style,
			m_x, m_y, m_width, m_height,
			parent ? parent->handle() : 0,
			menu ? menu->getMenu() : NULL, 
			Application::getAppInstance(), 
			lpParam ? lpParam : this
		);

		ForeignWindow::setHandle( winHandle );
		if( winHandle )
		{
			/*
				during creation and the call of read window data,
				style may be set to invisible
				thus we have to update the style now
			*/
#if !WIN_SCROLL
			if( !hScroll )
				hideVertScrollBar();
			if( !vScroll )
				hideHorizScrollBar();
#endif

			if( m_controlId >= 0 )
			{
				setId( m_controlId );
			}

			if( !m_width || !m_height )
			{
				resize( m_width, m_height );
			}

			return scSUCCESS;
		}
		return scERROR;
	}

	public:
	template <class TForm> 
	void setHandle( TForm *form )
	{
		setHandle( form->Handle );
	}
	virtual bool canClose( void );
	virtual SuccessCode close( void );


	void invalidateRect( const RectBorder &rect, bool clear=true ) const
	{
		ForeignWindow::invalidateRect( rect, clear );
		const BasicWindow *parent = getParent();
		if( parent )
		{
			RectBorder	clientRect = getRelativeRectangle();
			RectBorder	newRect = clientRect;
			newRect.top += clientRect.top;
			newRect.bottom += clientRect.top;
			newRect.left += clientRect.left;
			newRect.right += clientRect.left;

			parent->invalidateRect( newRect, clear );
		}
	}
	void invalidateWindow( bool clear=true ) const
	{
		ForeignWindow::invalidateWindow( clear );
		const BasicWindow *parent = getParent();
		if( parent )
		{
			parent->invalidateRect( getRelativeRectangle(), clear );
		}
	}

	/*
		for control windows
	*/
	void initHandle( const BasicWindow *parent, int controlId )
	{
		m_parent = parent;
		m_controlId = controlId;
		HWND parentHandle = parent->handle();
		if( parentHandle > 0 )
		{
			setHandle( GetDlgItem( parentHandle, controlId ) );
		}

		if( handle() )
		{
			readWindowData();
		}
	};
	void setId( int controlId )
	{
		m_controlId = controlId;
		if( isValid() )
		{
			ForeignWindow::setId( controlId );
		}
		else if( m_parent )
		{
			initHandle( m_parent, controlId );
		}
		if( m_toolHelp.isEmpty() )
		{
			m_toolHelp = appObject->loadString( controlId );
		}
	}
	int getId( void ) const
	{
		return m_controlId;
	}

	/*
		resources
	*/
	void setResource( gak::xml::Element *resource );
	gak::xml::Element *getResource( void ) const
	{
		return m_resource;
	}

	/*
		hiararchy
	*/
	protected:
	void setParent( const BasicWindow *parent )
	{
		m_parent = parent;
	}
	public:
	const BasicWindow *getParent( void ) const
	{
		return m_parent;
	}
	const BasicWindow *getTopWindow( void ) const;
	const BasicWindow *getOwner( void ) const
	{
		return m_owner;
	}
	HWND getParentHandle( void ) const
	{
		if( m_parent )
		{
			return m_parent->handle();
		}
		else
		{
			return 0;
		}
	}

	/*
		title
	*/
	using ForeignWindow::setText;
	BasicWindow &setText( const STRING &title )
	{
		m_caption = title;
		ForeignWindow::setText( title );
		checkSize( m_width, m_height );

		return *this;
	}
	const STRING &getTitle( void ) const
	{
		return m_caption;
	}

	/*
		tool help
	*/
	const STRING &getToolhelp( void ) const
	{
		return m_toolHelp;
	}
	BasicWindow &setToolhelp( const STRING &help )
	{
		m_toolHelp = help;

		return *this;
	}

	/*
		layout and size
	*/
	void setWidthOffset( int width )
	{
		m_widthOffset = width;
	}
	void setKeightOffset( int width )
	{
		m_heightOffset = width;
	}
	void saveWindowRect( const char *windowName = NULL ) const;
	void restoreWindowRect( const char *windowName = NULL, int nCmdShow = SW_SHOW );
	void restoreWindowPos( const char *windowName = NULL, int nCmdShow = SW_SHOW );

	void setLayoutData( LayoutData *layoutData )
	{
		if( m_layoutData )
		{
			delete m_layoutData;
		}
		m_layoutData = layoutData;
	}
	LayoutData *getLayoutData( void )
	{
		return m_layoutData;
	}
	const LayoutData *getLayoutData( void ) const
	{
		return m_layoutData;
	}

	void checkSize( int width, int height );
	void resize( int width, int height )
	{
		checkSize( width, height );
		if( isValid() )
		{
			ForeignWindow::resize( m_width, m_height );
		}
	}
	void resize( const Size &size )
	{
		resize( size.width, size.height );
	}
	void resize( void )
	{
		if( m_autoWidth || m_autoHeight )
		{
			int newWidth = m_autoWidth ? 0 : m_width;
			int newHeight = m_autoHeight ? 0 : m_height;
			resize( newWidth, newHeight );
		}
	}
	void move( int x, int y )
	{
		m_x = x;
		m_y = y;
		ForeignWindow::move( x, y );
	}
	void move( const Point &pos )
	{
		move( pos.x, pos.y );
	}
	void move( size_t zOrder )
	{
		BasicWindow	*parent = const_cast<BasicWindow*>(getParent());
		if( parent )
		{
			ChildWindows &mySiblings = const_cast<ChildWindows&>(parent->getChildren());
			mySiblings.moveElement( this, zOrder );
			if( isValid() )
			{
				SetWindowPos( 
					handle(), 
					zOrder ? mySiblings[zOrder-1]->handle() : HWND_TOP,		// HWND_BOTTOM
					0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
				);
			}
		}
	}
	void move( BasicWindow *parent )
	{
		SetParent( handle(), parent->handle() );
		if( m_parent )
		{
			const_cast<BasicWindow*>(m_parent)->m_childWindows.removeElement( this );
			if( m_parent == m_owner )
			{
				const_cast<BasicWindow*>(m_owner)->m_ownedWindows.removeElementVal( this );
				parent->m_ownedWindows += this;
				m_owner = parent;
			}
		}
		m_parent = parent;
		parent->m_childWindows.addElement( this );
	}
	void sizeNmove( int x, int y, int width, int height )
	{
		m_x = x;
		m_y = y;
		checkSize( width, height );
		ForeignWindow::sizeNmove( x, y, m_width, m_height );
	}
	void sizeNmove( const RectSize &dimension )
	{
		sizeNmove( dimension.x, dimension.y, dimension.width, dimension.height );
	}
	void sizeNmove( const RectBorder &dimension )
	{
		sizeNmove( dimension.left, dimension.top, dimension.getWidth(), dimension.getHeight() );
	}
	Size getSize( void )
	{
		if( isValid()  )
		{
			return ForeignWindow::getSize();
		}
		else
		{
			return Size( m_width, m_height );
		}
	}

	/*
		children
	*/
	virtual bool handleChildClose( BasicWindow *child, bool deleted );
	void addChild( BasicWindow *child )
	{
		m_childWindows.addElement( child );
	}
	const ChildWindows &getChildren( void ) const
	{
		return m_childWindows;
	}
	FrameChild *findFrame( const Point &position ) const;
	BasicWindow *findChild( const Point &position ) const;
	BasicWindow *findChild( gak::xml::Element *resource ) const;
	BasicWindow *findChild( int id ) const;
	BasicWindow *findChild( HWND handle ) const;

	/*
		colors
	*/
	void setBackgroundColorByRef( COLORREF color )
	{
		m_backGroundBrush.create( color );
		if( handle() )
		{
			invalidateWindow();
		}
	}
	void setBackgroundColor( unsigned char red, unsigned char green, unsigned char blue )
	{
		setBackgroundColorByRef( RGB(red, green, blue ) );
	}
	void setBackgroundColorByIndex( SystemColor nIndex )
	{
		m_backGroundBrush.createSyscolor( nIndex );
		if( handle() )
		{
			invalidateWindow();
		}
	}
	enum BackgroundColor { 
		bcDEFAULT, 

		bcWHITE_BRUSH, 
		bcLTGRAY_BRUSH, 
		bcGRAY_BRUSH, 
		bcDKGRAY_BRUSH, 
		bcBLACK_BRUSH, 
		bcNULL_BRUSH, 

		bcCOLOR_SCROLLBAR,
		bcCOLOR_BACKGROUND,
		bcCOLOR_ACTIVECAPTION,
		bcCOLOR_INACTIVECAPTION,
		bcCOLOR_MENU,
		bcCOLOR_WINDOW,
		bcCOLOR_WINDOWFRAME,
		bcCOLOR_MENUTEXT,
		bcCOLOR_WINDOWTEXT,
		bcCOLOR_CAPTIONTEXT,
		bcCOLOR_ACTIVEBORDER,
		bcCOLOR_INACTIVEBORDER,
		bcCOLOR_APPWORKSPACE,
		bcCOLOR_HIGHLIGHT,
		bcCOLOR_HIGHLIGHTTEXT,
		bcCOLOR_BTNFACE,
		bcCOLOR_BTNSHADOW,
		bcCOLOR_GRAYTEXT,
		bcCOLOR_BTNTEXT,
		bcCOLOR_INACTIVECAPTIONTEXT,
		bcCOLOR_BTNHIGHLIGHT,

		bcSELECT 
	};
	void setBackgroundColor( BackgroundColor type, unsigned char red, unsigned char green, unsigned char blue )
	{
		setBackgroundColor( type, RGB( red, green, blue ) );
	}
	void setBackgroundColor( BackgroundColor type, COLORREF color )
	{
		if( type == bcDEFAULT )
			m_backGroundBrush.clearBrush();
		else if( type >= bcWHITE_BRUSH && type <= bcNULL_BRUSH )
			m_backGroundBrush.selectBrush( Brush::SystemBrush(type - bcWHITE_BRUSH) );	// ... 
		else if( type == bcSELECT )
			m_backGroundBrush.create( color );
		else
			m_backGroundBrush.createSyscolor( SystemColor(type - bcCOLOR_SCROLLBAR) );

		if( handle() )
			invalidateWindow();
	}
	void clearBrush( void )
	{
		m_backGroundBrush.clearBrush();
		if( handle() )
			invalidateWindow();
	}
	const Brush &getBackgroundBrush() const
	{
		return m_backGroundBrush;
	}

	/*
		font
	*/
	Font &getFont()
	{
		return m_font;
	}
	const Font &getFont() const
	{
		return m_font;
	}
	void setFont( Font &font )
	{
		if( !font.isAssigned() )
			font.create();

		if( isValid() )
		{
			m_font = font;
			message( WM_SETFONT, WPARAM(HFONT(font)), true );
			resize();
		}
	}
	void inheritFont( )
	{
		if( this->isValid() && m_parent && m_parent->isValid() )
		{
			message( WM_SETFONT, m_parent->message( WM_GETFONT ), true );
			m_font.readFont();
		}
	}

	/*
		tag
	*/
	int getTag() const
	{
		return m_tag;
	}
	void setTag( int tag )
	{
		m_tag = tag;
	}
};


}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
