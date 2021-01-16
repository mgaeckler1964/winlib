/*
		Project:		Windows Class Library
		Module: 		CALLWIN.H
		Description:	Implementation of CallbackWindow-class
						A CallbackWindow is a window that has an application 
						specific callback function
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

#ifndef CALL_WIN_H
#define CALL_WIN_H

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
#include <shellapi.h>
#include <commdlg.h>
#include <Commctrl.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/list.h>
#include <gak/string.h>
#include <winlib/basicwin.h>
#include <winlib/winapp.h>
#include <winlib/font.h>
#include <WINLIB/Layout.h>

#include "accelerators.h"

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
#	pragma comment( lib, "Comctl32.lib" )
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

const int STATUS_HEIGHT		=20;

const int APPL_TIMER_ID		=1;
const int TOOL_WIN_TIMER_ID	=2;

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

enum _ProcessStatus { psPROCESSED, psDO_DEFAULT, psSEND_TO_PARENT, psPOST_TO_PARENT };
typedef EnumWrapper<_ProcessStatus>	ProcessStatus;

enum HorizScrollCode 
{
	hscLEFT=SB_LEFT, 
	hscLINE_LEFT=SB_LINELEFT, hscPAGE_LEFT=SB_PAGELEFT,
	hscLINE_RIGHT=SB_LINERIGHT, hscPAGE_RIGHT=SB_PAGERIGHT,
	hscRIGHT=SB_RIGHT,
	hscTHUMB_POSITION=SB_THUMBPOSITION,hscTHUMB_TRACK=SB_THUMBTRACK
};

enum VertScrollCode 
{
	vscTOP=SB_TOP, 
	vscLINE_UP=SB_LINEUP, vscPAGE_UP=SB_PAGEUP,
	vscLINE_DOWN=SB_LINEDOWN, vscPAGE_DOWN=SB_PAGEDOWN,
	vscBOTTOM=SB_BOTTOM,
	vscTHUMB_POSITION=SB_THUMBPOSITION,vscTHUMB_TRACK=SB_THUMBTRACK
};

enum LeftButton
{
	lbDOUBLE_CLICK=WM_LBUTTONDBLCLK, 
	lbDOWN=WM_LBUTTONDOWN, 
	lbUP=WM_LBUTTONUP
};

enum RightButton
{
	rbDOUBLE_CLICK=WM_RBUTTONDBLCLK, 
	rbDOWN=WM_RBUTTONDOWN, 
	rbUP=WM_RBUTTONUP
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class SearchDialog;
class ToolTipWindow;
class ControlWindow;
class TreeView;
class TreeNode;
class EDIT;
class Device;

/*
	The Call Window Class
	=====================

	The "Call Window Class" describes a window which has a call back.
*/

class CallbackWindow : public BasicWindow, public gak::ListEntry
{
	friend LRESULT PASCAL WindowProc( HWND, UINT, WPARAM, LPARAM );
	friend LRESULT PASCAL MDIchildWindowProc( HWND, UINT, WPARAM, LPARAM );

#ifdef _MSC_VER
	friend LRESULT CALLBACK controlProcedure(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR );
#endif
	friend class MDIwindow;
	friend class ModalDialog;
	friend class NonModalDialog;
	friend class MDIchildWindow;

	// data
	private:

	static HACCEL			s_hAccel;
	static AcceleratorMap	s_accelerators;
	static UINT 			findMessage;
	static SearchDialog		*findDialog;

	Menu				*menu;
	EDIT				*statusBar;
	ToolTipWindow		*toolWin;
	bool				applicationTimer, toolWinTimer;

	TreeView			*dragTreeView;
	TreeNode			*dragItem;

	bool				m_allowNotifications;
	bool				m_dialogProcessing;
	bool				terminate, m_closed, destroyed;
	bool				altKey, shiftKey, controlKey;
	LRESULT				dialogCode;
#ifndef NDEBUG
	gak::Stack<UINT>	m_messageStack;
	bool				m_watchMe;
#endif
	int					m_callBackCount;
	LayoutManager		*layoutManager;

	Size				clientSize;

	protected:
	LRESULT				callBackValue;

	protected:
	void watch()
	{
#ifndef NDEBUG
		m_watchMe = true;
#endif
	}
	static void fillDefaultClass( WNDCLASS *wc )
	{
		/*
			make the class global, so that it can be used by resource DLLs
		*/
		wc->style			= CS_GLOBALCLASS;
		wc->lpfnWndProc		= WindowProc;
		wc->cbClsExtra		= 0;
		wc->cbWndExtra		= 0;
		wc->hInstance		= Application::getAppInstance();
		wc->hIcon			= appObject->getIcon();
		wc->hCursor			= LoadCursor( 0, IDC_ARROW );
		wc->hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wc->lpszMenuName   	= NULL;
	}
	static bool registerClass( WNDCLASS *wc )
	{
		return ::RegisterClass( wc ) ? true : false;
	}

	// creating and destroying
	protected:
	CallbackWindow( BasicWindow *owner );
	~CallbackWindow();

	public:
	static void registerSearchDialog( SearchDialog *newFindDialog );

	protected:
	bool isShiftKey( void ) const
	{
		return shiftKey;
	}
	bool isAltKey( void ) const
	{
		return altKey;
	}
	bool isControlKey( void ) const
	{
		return controlKey;
	}
	void setClosedFlag( void )
	{
		m_closed = true;
	}
	bool mayBeDeleted( void ) const
	{
		return bool(m_closed && destroyed);
	}

	protected:

	SuccessCode create( BasicWindow *parent, LPSTR lpParam=NULL )
	{
		SuccessCode error = BasicWindow::create( parent, menu, lpParam );
		if( error == scSUCCESS )
		{
			getFont().selectFont( Font::sfAnsiVar );
//			getFont().setFontName("MS Sans Serif").setFontSize(8).create();
		}

		return error;
	}

	public:
	virtual SuccessCode close( void );
	void acceptFiles( void )
	{
		DragAcceptFiles( handle(), true );
	}
	void noFileAccept( void )
	{
		DragAcceptFiles( handle(), true );
	}

	// mouse capture
	protected:
	void captureMouse( void )
	{
		SetCapture( handle() );
	}
	void releaseMouse( void )
	{
		ReleaseCapture();
	}

	/*
		===============================================================================================
		the message functions routines
		===============================================================================================
	*/
	// WM_INITDIALOG and WM_CREATE
	public:
	virtual ProcessStatus handleCreate( void );
	virtual void getControls();
	// WM_CLOSE
	virtual SuccessCode handleClose( void );

	// WM_DESTROY
	virtual ProcessStatus handleDestroy( void );

	// WM_SHOWWINDOW
	virtual ProcessStatus handleShow( void );
	virtual ProcessStatus handleHide( void );

	// WM_MDIACTIVATE
	virtual void handleActivate( bool );

	// WM_ERASEBKGND 
	virtual ProcessStatus handleErase( Device &hDC );

	// WM_PAINT
	virtual ProcessStatus handleRepaint( Device &hDC );

	// WM_COMMAND
	virtual ProcessStatus handleEditChange( int control );
	virtual ProcessStatus handleButtonClick( int control );
	virtual ProcessStatus handleSelectionChange( int control );
	virtual ProcessStatus handleNotification( int notification, int control );
	virtual ProcessStatus handleCommand( int cmd );

	// WM_VSCROLL, WM_HSCROLL
	virtual ProcessStatus handleScrollControl( int control );
	virtual ProcessStatus handleVertScroll( VertScrollCode scrollCode, int nPos, HWND scrollBar );
	virtual ProcessStatus handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND scrollBar );

	// WM_CAPTURECHANGED
	virtual ProcessStatus handleMouseRelease( void );

	// WM_MOUSEMOVE
	virtual ProcessStatus handleMouseMove( WPARAM modifier, const Point &position );

	// WM_LBUTTONDBLCLK, WM_LBUTTONDOWN, WM_LBUTTONUP
	virtual ProcessStatus handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position );

	// WM_RBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP
	virtual ProcessStatus handleRightButton( RightButton rightButton, WPARAM modifier, const Point &position );

	// WM_RESIZE
	virtual ProcessStatus handleResize( const Size &newSize );

	// WM_DROPFILES
	virtual void handleFile( const char *filename, size_t idx, size_t numFiles );
	private:
			void handleFiles( HDROP );
	protected:

	// find & replace dialogs
	virtual void handleFind( const char *, bool, bool, bool );
	virtual void handleReplace(	const char *, const char *,
								bool, bool, bool );

	// WM_DDE_INITIATE
	virtual ProcessStatus ddeInitiate( const char *app, const char *topic );
	// WM_DDE_EXECUTE
	virtual ProcessStatus ddeExecute( const char *command );

	// WM_HELP
	virtual void showHelp( int controlType, int controlId );

	// WM_TIMER
	virtual void handleTimer( void );

	// WM_SETFOCUS
	virtual void handleFocus( void );
	// WM_SYSKEYDOWN, WM_KEYDOWN
	virtual ProcessStatus handleKeyDown( int key );
	// WM_SYSKEYUP, WM_KEYUP
	virtual ProcessStatus handleKeyUp( int key );
	// WM_CHAR
	virtual ProcessStatus handleCharacterInput( int c );
	// WM_KILLFOCUS
	virtual void handleKillFocus( void );

	// all other messages
	virtual ProcessStatus handleMessage( UINT msg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT doDefault( UINT, WPARAM, LPARAM );

			void handleFindReplace( FINDREPLACE *buffer );
			void checkModifiers( unsigned msg, WPARAM wParam, LPARAM lParam );
			void checkModifiers( const MSG &msg )
			{
				checkModifiers( msg.message, msg.wParam, msg.lParam );
			}
			LRESULT callBack( const unsigned msg, const WPARAM wParam, const LPARAM lParam );

	static	LRESULT dispatch( HWND handle, unsigned msg, WPARAM wParam, LPARAM lParam );

	virtual bool handleTreeViewDrag( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dragOver );
	virtual void handleTreeViewDrop( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dropTarget );

	/*
		message loop
	*/
	protected:
	void terminateLoop()
	{
		terminate = true;
	}
	public:
	void mainLoop( void );
	void exitApplication( int exitCode = 0 )
	{
		PostQuitMessage( exitCode );
	}



	/*
		control messages
	*/
#ifdef _MSC_VER
	private:
	LRESULT controlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam );
	protected:
	virtual ProcessStatus preControlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam );
	virtual void postControlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam );
	void catchControl( BasicWindow *control )
	{
		SetWindowSubclass( control->handle(), controlProcedure, UINT_PTR(control), DWORD_PTR(this) );
	}
	void releaseControl( BasicWindow *control )
	{
		RemoveWindowSubclass( control->handle(), controlProcedure, UINT_PTR(control) );
	}
	public:
#endif


	/*
		accelerators
	*/
	virtual bool translateAccell( MSG &msg ) const;
	static void setAccelerators( const char *accelerators )
	{
		s_hAccel = accelerators ? appObject->loadAccelerators( accelerators ) : 0;
	}
	static void setAccelerators( int id )
	{
		s_hAccel = appObject->loadAccelerators( id );
	}
	static void setAccelerators( ACCEL *accelerators, int numItems )
	{
		s_hAccel = CreateAcceleratorTable( accelerators, numItems );

		s_accelerators.addElements( static_cast<Accelerators*>(accelerators), numItems );
	}
	static Accelerators* getAccelerator( int comand )
	{
		return s_accelerators.hasElement( comand ) ? &s_accelerators[comand] : NULL;
	}
	/*
		dialogs
	*/
	private:
	bool isDialogMessage( MSG &msg );
	public:
	void allowDialogProcessing()
	{
		m_dialogProcessing = true;
	}
	void allowNotifications()
	{
		m_allowNotifications = true;
	}
	void disallowNotifications()
	{
		m_allowNotifications = false;
	}
	void doModalDialog( int id );

	/*
		menu_bar
	*/
	const Menu *setMenu( const char *menuName )
	{
		if( menu )
			delete menu;
		return menu = appObject->loadMenu( menuName );
	}
	const Menu *setMenu( int menuId )
	{
		if( menu )
			delete menu;
		return menu = appObject->loadMenu( menuId );
	}
	const Menu *getMenu( void ) const
	{
		return menu;
	}
	void drawMenuBar( void ) const
	{
		DrawMenuBar( handle() );
	}

	/*
		timer
	*/
	private:
	void removeTimer( bool appTimer )
	{
		KillTimer( handle(), appTimer ? APPL_TIMER_ID : TOOL_WIN_TIMER_ID );
		if( appTimer )
			applicationTimer = false;
		else
			toolWinTimer = false;
	}
	void createTimer( int milliSeconds, bool appTimer )
	{
		if( milliSeconds )
		{
			SetTimer( handle(), appTimer ? APPL_TIMER_ID : TOOL_WIN_TIMER_ID, milliSeconds, NULL );
			if( appTimer )
				applicationTimer = true;
			else
				toolWinTimer = true;
		}
		else 
			removeTimer( appTimer );
	}
	public:
	void setTimer( int milliSeconds )
	{
		createTimer( milliSeconds, true );
	}
	void removeTimer( void )
	{
		removeTimer( true );
	}
	bool hasTimer( void ) const
	{
		return applicationTimer;
	}

	/*
		tool help
	*/
	void showToolHelp( const char *text );
	void removeToolHelp( void );

	/*
		status bar
	*/
	private:
	void resizeStatusBar( Size *mySize );

	public:
	void addStatusBar( void );
	bool hasStatusBar( void ) const
	{
		return( statusBar ) ? true : false;
	}
	void setStatusText( const STRING &value );
	void removeStatusBar( void );

	/*
		scrolling
	*/
	void scrollHorizontal( int position )
	{
		handleHorizScroll( hscTHUMB_POSITION, position, NULL );
	}
	void scrollVertical( int position )
	{
		handleVertScroll( vscTHUMB_POSITION, position, NULL );
	}

	/*
		control windows
	*/
	private:
	ProcessStatus checkControlColor( HWND handle, HDC device );


	/*
		children and layout manager
	*/
	public:
	void setLayoutManager( LayoutManager *newLayoutManager )
	{
		if( layoutManager )
			delete layoutManager;
		layoutManager = newLayoutManager;
	}
	LayoutManager *getLayoutManager( void ) const
	{
		return layoutManager;
	}
	Size calcSize( const Size &newSize ) const
	{
		Size	result;
		if( layoutManager )
		{
			result = layoutManager->calcSize( getChildren(), newSize );
		}
		else
		{
			result = newSize;
		}
		return result;
	}
	void doLayout( const Size &newSize ) const
	{
		if( layoutManager )
		{
			layoutManager->doLayout( getChildren(), newSize );
			invalidateWindow();
		}
	}
	void doLayout( void ) const
	{
		if( layoutManager )
		{
			layoutManager->doLayout( getChildren(), getClientSize() );
			invalidateWindow();
		}
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
