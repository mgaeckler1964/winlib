/*
		Project:		Windows Class Library
		Module:			ForeignWindow.h
		Description:	ForeignWindow a class also used for windows in 
						other programs
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1991-2026 Martin Gäckler

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

#ifndef FOREIGN_WINOW_H
#define FOREIGN_WINOW_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <windows.h>
#include <gak/string.h>

#include <WINLIB/rectangle.h>
#include <WINLIB/WINAPP.H>

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

inline gak::STRING loadString( int id )
{
	return appObject->loadString( id );
}

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

typedef unsigned short	WindowClass_t;
const WindowClass_t ForeignWindowFlag	= 0x00;
const WindowClass_t WinlibWindowFlag	= 0x01;
const WindowClass_t CallbackWindowFlag	= 0x02;
const WindowClass_t ControlWindowFlag	= 0x04;
const WindowClass_t SearchDialogFlag	= 0x08;
const WindowClass_t FileDialogFlag		= 0x10;

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

class ForeignWindow
{
	HWND				m_winHandle;
	unsigned long		m_style, m_exStyle;
	bool				m_caret;
	WindowClass_t		m_windowType;

	/*
		device contexts
	*/
	private:
	friend class DrawDevice;
	friend class RepaintDevice;
	friend class Font;
	
	HDC getDC() const
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

	}

	public:
	ForeignWindow( HWND theHandle=0 )
	{
		m_windowType = ForeignWindowFlag;
		m_winHandle	= theHandle;

		m_style = getStyle();
		m_exStyle = getExStyle();
		m_caret = false;
	}
	const ForeignWindow &operator = ( HWND theHandle )
	{
		m_windowType = ForeignWindowFlag;
		m_winHandle	= theHandle;

		m_style = getStyle();
		m_exStyle = getExStyle();
		m_caret = false;

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
	void clrHandle()			// for the dialog & child windows
	{
		m_winHandle = HWND(-1);		// to ensure, that this object
									// no longer recieves messages
	}
	bool isValid() const
	{
		return int(m_winHandle)>0;
	}
	HWND handle() const
	{
		return m_winHandle;
	}
	virtual SuccessCode close();
	operator bool () const
	{
		return isValid();
	}
	bool operator ! () const
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
	int getId() const
	{
		return isValid() ? GetWindowLong( m_winHandle, GWL_ID ) : -1;
	}

	/*
		window types
	*/
	virtual gak::STRING getWindowClassName() const;
	WindowClass_t getWindowClass() const
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

	LRESULT message( unsigned message, WPARAM wParam=0, LPARAM lParam=0 ) const
	{
		return SendMessage( m_winHandle, message, wParam, lParam );
	}
	void postMessage( unsigned message, WPARAM wParam=0, LPARAM lParam=0 ) const
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
		gak::STRING	text = loadString( textId );

		gak::STRING	title;
		if( titleId )
			title = loadString( titleId );

		return messageBox( text, title, type );
	}
	int messageBox( const char	*text,
					int			titleId,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		gak::STRING	title;
		if( titleId )
			title = loadString( titleId );

		return messageBox( text, title, type );
	}
	int messageBox( int			textId,
					const char	*title,
					unsigned	type=MB_APPLMODAL|MB_ICONSTOP|MB_OK ) const
	{
		gak::STRING	text = loadString( textId );

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
	}
	void hide()
	{
		show( SW_HIDE );
	}
	void minimize()
	{
		show( SW_MINIMIZE );
	}
	void maximize()
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
	void setForeground()
	{
		SetForegroundWindow(m_winHandle);
	}

	void focus( bool withForground = false )
	{
		if( !(getStyle() & WS_CHILD) )
		{
			if( IsIconic( m_winHandle ) )
			{
				show( SW_RESTORE );
			}
			SetWindowPos(m_winHandle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			if( withForground )
				setForeground();
		}
		SetFocus( m_winHandle );
	}
	bool hasFocus() const
	{
		return GetForegroundWindow() == handle();
	}
	void enable() const
	{
		EnableWindow( m_winHandle, true );
	}
	void disable() const
	{
		EnableWindow( m_winHandle, false );
	}
	bool isEnabled() const
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
	unsigned long getStyle() const
	{
		return isValid() ? GetWindowLong( m_winHandle, GWL_STYLE ) : m_style;
	}
	unsigned long getExStyle() const
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
	int getTextLength() const
	{
		return int(message( WM_GETTEXTLENGTH ));
	}
	gak::STRING getText( bool oemConvert = false ) const;
	void setText( const gak::STRING &text )
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

	Size getClientSize() const
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

	RectBorder getClientRectangle() const
	{
		RectBorder rect;
		GetClientRect( m_winHandle, &rect );
		return rect;
	}

	RectBorder getWindowRectangle() const
	{
		RectBorder rect;
		GetWindowRect( m_winHandle, &rect );
		return rect;
	}
	HWND getParentHandle() const
	{
		return GetParent( m_winHandle );
	}
	bool isWindowVisible() const
	{
		return IsWindowVisible(m_winHandle);
	}
	
	RectBorder getRelativeRectangle() const
	{
		RectBorder rect = getWindowRectangle();

		HWND	parent;

		parent = getParentHandle();
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
	Size getSize() const
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
	Point getPosition() const
	{
		RectBorder	rect = getWindowRectangle();
		return Point( rect.left, rect.top );
	}
	Point getRelativePosition() const
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
	void addBorder()
	{
		addStyle( WS_BORDER );
	}
	void remoweBorder()
	{
		removeStyle( WS_BORDER );
	}

	// wtf is that here? this is for static controls!!!
	void addFrame()
	{
		addStyle( SS_BLACKFRAME );
	}
	void remoweFrame()
	{
		removeStyle( SS_BLACKFRAME );
	}

	/*
		Scrollbars
		==========
	*/
	void showVertScrollBar( int min, int max )
	{
		SetScrollRange( m_winHandle, SB_VERT, min, max, true );
		ShowScrollBar( m_winHandle, SB_VERT, true );
	}
	void hideVertScrollBar()
	{
		ShowScrollBar( m_winHandle, SB_VERT, false );
	}
	int getVertScrollPos() const
	{
		return GetScrollPos( m_winHandle, SB_VERT );
	}
	int setVertScrollPos( int newPos ) const
	{
		return SetScrollPos( m_winHandle, SB_VERT, newPos, true );
	}
	bool hasVertScrollBar() const
	{
		bool	vScroll;

		if( !hasStyle( WS_VSCROLL ) )
			vScroll = false;
		else
		{
			vScroll = true;
		}
		return vScroll;
	}

	void showHorizScrollBar( int min, int max )
	{
		SetScrollRange( m_winHandle, SB_HORZ, min, max, true );
		ShowScrollBar( m_winHandle, SB_HORZ, true );
	}
	void hideHorizScrollBar()
	{
		ShowScrollBar( m_winHandle, SB_HORZ, false );
	}
	int getHorizScrollPos() const
	{
		return GetScrollPos( m_winHandle, SB_HORZ );
	}
	int setHorizScrollPos( int newPos ) const
	{
		return SetScrollPos( m_winHandle, SB_HORZ, newPos, true );
	}
	bool hasHorizScrollBar() const
	{
		bool	hScroll;

		if( !hasStyle( WS_HSCROLL ) )
			hScroll = false;
		else
		{
			hScroll = true;
		}
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
	void redrawResize() const
	{
		DWORD classStyle;

		classStyle = GetClassLong( m_winHandle, GCL_STYLE );
		classStyle |= CS_HREDRAW|CS_VREDRAW;
		SetClassLong( m_winHandle, GCL_STYLE, classStyle );
	}
	void noRedrawResize() const
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
	bool hasCaret() const
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
	void showCaret() const
	{
		ShowCaret( m_winHandle );
	}
	void moveCaret( int x, int y ) const
	{
		SetCaretPos( x, y );
	}
	void hideCaret() const
	{
		HideCaret( m_winHandle );
	}
	void destroyCaret()
	{
		m_caret = false;
		DestroyCaret();
	}

	/*
		copy data
	*/
	private:
	void copyData( const void *data, std::size_t len, int typeID, const ForeignWindow *src  ) const
	{
		COPYDATASTRUCT cds;
		cds.dwData = typeID;
		cds.cbData = DWORD(len);
		cds.lpData = const_cast<void *>(data);

		SendMessage(handle(), WM_COPYDATA, WPARAM(src ? src->handle() : nullptr), LPARAM(&cds));		
	}
	public:
	void copyData( const gak::STRING &data, int typeID=0, const ForeignWindow *src=nullptr ) const
	{
		copyData(data.c_str(), data.size()+1, typeID, src );		
	}
	void copyData( const char *data, int typeID=0, const ForeignWindow *src=nullptr ) const
	{
		copyData(data, strlen(data)+1, typeID, src);		
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

#endif	// FOREIGN_WINOW_H
