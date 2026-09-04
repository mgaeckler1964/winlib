/*
		Project:		Windows Class Library
		Module: 		BASICWIN.H
		Description:	The Basic Window Class
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

#ifndef BASIC_WIN_H
#define BASIC_WIN_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

#if defined(WIN_SCROLL) && !WIN_SCROLL
#error "WIN_SCROLL no longer supported"
#endif

#if WIN_SCROLL_INFO
#error WIN_SCROLL_INFO no longer supported"
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

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <winlib/ForeignWindow.h>
#include <winlib/brush.h>
#include <winlib/font.h>
#include <winlib/layout.h>

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
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

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
	return WritePrivateProfileString( 
		appName, key, gak::formatNumber(value), file 
	) ? true : false;
}

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

typedef gak::UnorderedSet<BasicWindow*>		ChildWindows;
typedef gak::ArrayOfPointer<BasicWindow>	OwnedWindows;

/*
	BASIC WINDOW CLASS
	==================

	The "Basic Window Class" describes any window. It is the BASE-Class for
	all other window classes.
*/

class FrameChild;

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

	enum WindowState {
		wsNew, wsInCreation, wsLoading, wsReady
	} m_state;

	public:
	void setLoading()
	{
		m_state = wsLoading;
	}
	void setReady()
	{
		m_state = wsReady;
	}
	bool isReady() const
	{
		return m_state == wsReady;
	}
	private:
	void addWindow( BasicWindow *child )
	{
		m_ownedWindows += child;
	}

	// do nothing, just forbid
	BasicWindow( const BasicWindow &source );
	BasicWindow &operator = ( const BasicWindow &source );

	// (de)contructors
	void init()
	{
		m_autoWidth = m_autoHeight = false;
		m_x = m_y = m_width = m_height = CW_USEDEFAULT;
		m_widthOffset = m_heightOffset = 0;

		m_resource	= nullptr;
		m_layoutData= nullptr;
		m_owner		= nullptr;
		m_parent	= nullptr;
		m_controlId	= -1;
		m_state		= wsNew;

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
	BasicWindow(HWND foreignHandle) : ForeignWindow(foreignHandle)
	{
		init();
		m_owner = nullptr;
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
	SuccessCode create( BasicWindow *parent, Menu *menu=nullptr, void *lpParam=nullptr )
	{
		STRING className = getWindowClassName();

		setParent( parent );
		unsigned long	style = getStyle();
		unsigned long	exStyle = getExStyle();

		if( parent && hasStyle( WS_CHILD ) )
		{
			parent->m_childWindows.addElement( this );
		}

		m_state = wsInCreation;
		HWND winHandle = CreateWindowEx(
			exStyle, 
			className.c_str(), 
			m_caption.isEmpty() ? 0 : m_caption.c_str(), 
			style,
			m_x, m_y, m_width, m_height,
			parent ? parent->handle() : 0,
			menu ? menu->getMenu() : nullptr,
			Application::getAppInstance(), 
			lpParam ? lpParam : this
		);

		ForeignWindow::setHandle( winHandle );
		if( winHandle )
		{
			if( m_controlId >= 0 )
			{
				setId( m_controlId );
			}

			if( !m_width || !m_height )
			{
				resize( m_width, m_height );
			}

			m_state = wsReady;
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
	virtual bool canClose();
	virtual SuccessCode close();

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
	int getId() const
	{
		return m_controlId;
	}

	/*
		resources
	*/
	void setResource( gak::xml::Element *resource );
	gak::xml::Element *getResource() const
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
	const BasicWindow *getParent() const
	{
		return m_parent;
	}
	const BasicWindow *getTopWindow() const;
	const BasicWindow *getOwner() const
	{
		return m_owner;
	}
	HWND getParentHandle() const
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
	const STRING &getTitle() const
	{
		return m_caption;
	}

	/*
		tool help
	*/
	const STRING &getToolhelp() const
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
	LayoutData *getLayoutData()
	{
		return m_layoutData;
	}
	const LayoutData *getLayoutData() const
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
	void resize()
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
	Size getSize() const
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
	const ChildWindows &getChildren() const
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

	void setBackgroundColorByRef( COLORREF color )
	{
		setBackgroundColor( bcSELECT, color );
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
	void clearBrush()
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
