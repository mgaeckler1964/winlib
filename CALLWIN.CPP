/*
		Project:		Windows Class Library
		Module: 		CALLWIN.CPP
		Description:	Implementation of CallbackWindow-class
						A CallbackWindow is a window that has an application 
						specific callback function
		Author:			Martin G�ckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin G�ckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin G�ckler, Germany, Munich ``AS IS''
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

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <windows.h>
#include <dde.h>
#include <commdlg.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <winlib/winapp.h>
#include <winlib/callwin.h>
#include <winlib/dialogwi.h>
#include <winlib/stddlg.h>
#include <winlib/tooltip.h>
#include <WINLIB\CONTROLW.H>

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

using namespace gak;

#undef FINDMSGSTRING
#define FINDMSGSTRING "commdlg_FindReplace"

// --------------------------------------------------------------------- //
// ----- module statics ------------------------------------------------ //
// --------------------------------------------------------------------- //

static ListContainer	*openWin = NULL;

// --------------------------------------------------------------------- //
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

HACCEL			CallbackWindow::s_hAccel	= 0;
AcceleratorMap	CallbackWindow::s_accelerators;
UINT			CallbackWindow::findMessage	= UINT(-1);
SearchDialog	*CallbackWindow::findDialog	= NULL;

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

CallbackWindow::CallbackWindow( BasicWindow *owner )
: BasicWindow( CallbackWindowFlag, owner )
{
	doEnterFunction("CallbackWindow::CallbackWindow");
#ifndef NDEBUG
	m_watchMe = false;
#endif
	if( int(findMessage) < 0 )
	{
		findMessage = RegisterWindowMessage( FINDMSGSTRING );
	}

	menu  = 0;
	statusBar = NULL;
	toolWin = NULL;
	toolWinTimer = applicationTimer = 0;
	layoutManager = NULL;

	m_callBackCount = 0;

	m_allowNotifications = true;
	m_dialogProcessing = false;
	
	altKey = shiftKey = controlKey = false;
	terminate = m_closed = destroyed = false;

	dragTreeView = NULL;
	dragItem = NULL;

	if( !openWin )
	{
		openWin = new ListContainer;
	}

	openWin->addElement( (ListEntry *)this );
}

CallbackWindow::~CallbackWindow()
{
	assert( !m_callBackCount );
	removeTimer();
	removeToolHelp();
	removeStatusBar();
	openWin->removeObject( (ListEntry *)this );

	if( layoutManager )
	{
		delete layoutManager;
	}
	if( menu )
	{
		delete menu;
	}
}

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void CallbackWindow::registerSearchDialog( SearchDialog *newFindDialog )
{
	if( findDialog )
	{
		findDialog->close();
		delete findDialog;
	}

	findDialog = newFindDialog;
}

LRESULT CallbackWindow::dispatch( HWND handle, unsigned msg, WPARAM wParam, LPARAM lParam )
{
	doEnterFunction("CallbackWindow::dispatch");
	CallbackWindow	*curWindow;
	ListCursor		cursor;
	LRESULT			returnVal = true;

	for( 
		curWindow =	openWin ? (CallbackWindow*)openWin->getFirst( &cursor ) : NULL;
		curWindow;
		curWindow = (CallbackWindow *)cursor.getNext()
	)
	{
		if( curWindow->handle() == handle )
		{
			curWindow->m_callBackCount++;
#ifndef NDEBUG
			curWindow->m_messageStack.push( msg );
#endif
			try
			{
#ifndef NDEBUG
				if( curWindow->m_watchMe )
				{
					doLogValue( msg );
					doLogValue( wParam );
					doLogValue( lParam );
					doLogValue( curWindow->m_callBackCount );
				}
#endif
				returnVal = curWindow->callBack( msg, wParam, lParam );
#ifndef NDEBUG
				curWindow->m_messageStack.pop();
#endif
				curWindow->m_callBackCount--;
#ifndef NDEBUG
				if( curWindow->m_watchMe )
				{
					doLogValue( msg );
					doLogValue( wParam );
					doLogValue( lParam );
					doLogValue( curWindow->m_callBackCount );
				}
#endif
			}
			catch( ... )
			{
#ifndef NDEBUG
				curWindow->m_messageStack.pop();
#endif
				curWindow->m_callBackCount--;
				throw;
			}

			if( curWindow->mayBeDeleted() && !curWindow->m_callBackCount )
			{
				bool deleted = false;
				BasicWindow *parent = const_cast<BasicWindow *>(curWindow->getParent());
				BasicWindow	*owner = const_cast<BasicWindow *>(curWindow->getOwner());
				if( parent )
				{
					deleted = parent->handleChildClose( curWindow, deleted );
				}
				if( owner && owner != parent )
				{
					owner->handleChildClose( curWindow, deleted );
				}
			}
/*v*/		break;
		}
	}

	return returnVal;
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

inline void CallbackWindow::resizeStatusBar( Size *mySize )
{
	if( statusBar )
	{
		statusBar->sizeNmove( 0, mySize->height - STATUS_HEIGHT, mySize->width, STATUS_HEIGHT );
//		mySize->y -= STATUS_HEIGHT;
	}
}

void CallbackWindow::handleFiles( HDROP drop )
{
	size_t	numFiles,
			i;
	char	fileName[1024];

	numFiles = DragQueryFile( drop, UINT(-1), NULL, 0 );
	for( i=0; i < numFiles; )
	{
		UINT result = DragQueryFile( drop, UINT(i), fileName, sizeof( fileName ) );
		if( result )
			handleFile( fileName, ++i, numFiles );
	}
	DragFinish( drop );
}

void CallbackWindow::handleFindReplace( FINDREPLACE *findReplace )
{
	if( findReplace->Flags & FR_DIALOGTERM )
	{
		delete findDialog;
		findDialog = NULL;
		return;
	}
	else if( findReplace->Flags & FR_FINDNEXT )
	{
		handleFind(	findDialog->getFindText(),
					findReplace->Flags & FR_WHOLEWORD ? true : false,
					findReplace->Flags & FR_MATCHCASE ? true : false,
					findReplace->Flags & FR_DOWN      ? true : false );
	}
	else if( findReplace->Flags & FR_REPLACE )
	{
		handleReplace(	findDialog->getFindText(),
						((ReplaceDialog*)findDialog)->getReplaceText(),
						false,
						findReplace->Flags & FR_WHOLEWORD ? true : false,
						findReplace->Flags & FR_MATCHCASE ? true : false );
	}
	else if( findReplace->Flags & FR_REPLACEALL )
	{
		handleReplace(	findDialog->getFindText(),
						((ReplaceDialog*)findDialog)->getReplaceText(),
						true,
						findReplace->Flags & FR_WHOLEWORD ? true : false,
						findReplace->Flags & FR_MATCHCASE ? true : false );
	}
}

bool CallbackWindow::isDialogMessage( MSG &msg )
{
	if( m_dialogProcessing )
	{
		checkModifiers( msg );

#if 0
		// this prevent the control to receive tabs
		if( findChild( msg->hwnd ) && msg->message == WM_KEYDOWN && msg->wParam == VK_TAB )
		{
			int direction = isShiftKey() ? GW_HWNDPREV : GW_HWNDNEXT;
			for( 
				HWND next = GetNextWindow( msg->hwnd, direction );
				next;
				next = GetNextWindow( next, direction ) 
			)
			{
				BasicWindow *child = findChild( next );
				if( child )
				{
					if( child->hasStyle( WS_TABSTOP ) && child->isEnabled() )
					{
						child->focus();
						break;
					}
				}
			}

			return true;
		}
		else
#endif
			return IsDialogMessage( handle(), &msg );
	}

	return false;	
}

LRESULT CallbackWindow::callBack( const unsigned int message, const WPARAM wParam, const LPARAM lParam )
{
//	doEnterFunction("CallbackWindow::callBack");

	ProcessStatus	msgStatus = psPROCESSED;

	callBackValue = 0;	// show that we have processed the message
	// find/replace
	if( message == findMessage )
	{
		handleFindReplace( (FINDREPLACE *)lParam );
	}
	else switch( message )
	{
		// The window messages

		// creating/destroying
		case WM_INITDIALOG:
			callBackValue = 1;		// set the input focus to the first item
		case WM_CREATE:
		{
			m_closed = destroyed = false;
			setIcon( appObject->getIcon() );

			if( !getResource() )
			{
				// we cannot handle the creation before all controls were created
				msgStatus = handleCreate();
			}

			break;
		}
		case WM_SHOWWINDOW:
			if( wParam )
				msgStatus = handleShow();
			else
				msgStatus = handleHide();
			break;
		case WM_ENDSESSION:
			if( !wParam )
				break;
		case WM_CLOSE:
			m_closed = (handleClose() == scSUCCESS);
			break;
		case WM_DESTROY:
			msgStatus = handleDestroy();
			if( this == appObject->getMainWindow() )
				exitApplication();
			destroyed = true;
			break;

		// activation
		case WM_MDIACTIVATE:
			handleActivate( (HWND)lParam == handle() ? true : false );
			break;

		// sizing
		// WM_WINDOWPOSCHANGED

		case WM_SIZE:
		{
			clientSize = MAKESIZE( DWORD(lParam) );
			resizeStatusBar( &clientSize );
			msgStatus = handleResize( clientSize );
			break;
		}
		// repainting
		case WM_ERASEBKGND:
		{
			callBackValue = 1;
			BackgroundDevice dc( this, HDC(wParam) );
			msgStatus = handleErase( dc );
			break;
		}
		case WM_PAINT:
		{
			RepaintDevice dc( this );
			msgStatus = handleRepaint( dc );
			break;
		}

		// scroll bars
		case WM_VSCROLL:
			if( toolWin )
			{
				removeToolHelp();
			}
			msgStatus = handleVertScroll( VertScrollCode( LOWORD( wParam ) ), HIWORD( wParam ), HWND(lParam) );
			break;

		case WM_HSCROLL:
			if( toolWin )
			{
				removeToolHelp();
			}
			msgStatus = handleHorizScroll( HorizScrollCode( LOWORD( wParam ) ), HIWORD( wParam ), HWND(lParam) );
			break;

		// command buttons / menus
		case WM_NOTIFY:
		{
			if( toolWin )
			{
				removeToolHelp();
			}
			if( !m_allowNotifications )
			{
				msgStatus = psDO_DEFAULT;
			}
			else
			{
				NMHDR *notify = reinterpret_cast<NMHDR *>(lParam);
				if( notify->code == TVN_SELCHANGED )
				{
					NM_TREEVIEW  *notify = reinterpret_cast<NM_TREEVIEW *>(lParam);
					if( notify->action != TVC_UNKNOWN )
					{
						msgStatus = handleSelectionChange( int(notify->hdr.idFrom) );
					}
				}
				else if( notify->code == TVN_BEGINDRAG )
				{
					TreeView *treeView = static_cast<TreeView *>(findChild( notify->hwndFrom ));
					if( treeView )
					{
						NM_TREEVIEW  *notify = reinterpret_cast<NM_TREEVIEW *>(lParam);
						if( (dragItem = treeView->startDrag( notify )) != NULL )
						{
							msgStatus = psPROCESSED;
							dragTreeView = treeView;
							//ShowCursor( FALSE );
							captureMouse();
						}
					}
				}
			}
			break;
		}
		case WM_COMMAND:
			if( toolWin )
			{
				removeToolHelp();
			}
			if( !m_allowNotifications )
			{
				msgStatus = psDO_DEFAULT;
			}
			else
			{
				{
					int command = LOWORD( wParam );
					if( lParam == 0 )
					{
						msgStatus = handleCommand( command );
					}
					else
					{
						int notify =  HIWORD( wParam );
						switch( notify )
						{
							case EN_CHANGE:
							case CBN_EDITCHANGE:
								msgStatus = handleEditChange( command );
								break;
							case BN_CLICKED:
								msgStatus = handleButtonClick( command );
								break;

							case TVN_SELCHANGED:
							case LBN_SELCHANGE:
//							case CBN_SELCHANGE:
								msgStatus = handleSelectionChange( command );
								break;
							default:
								msgStatus = handleNotification( notify, command );
								break;
						}
					}
				}
			}

			break;

		// The mouse
		case WM_CAPTURECHANGED:
			msgStatus = handleMouseRelease();
			break;

		case WM_MOUSEMOVE:
		{
			controlKey = wParam & MK_CONTROL;
			shiftKey = wParam & MK_SHIFT;

			Point point = MAKEPOINT( DWORD(lParam) );
			if( dragTreeView )
			{
				msgStatus = psPROCESSED;
				mouseToChildPosition( dragTreeView, &point );
				TreeNode	*dragOver = dragTreeView->doDrag( point );
				if( dragOver )
				{
					handleTreeViewDrag( dragTreeView, dragItem, dragOver );
				}
			}
			else
			{
				msgStatus = handleMouseMove( wParam, point );
			}
			break;
		}
		case WM_LBUTTONUP:
			if( dragTreeView )
			{
				TreeNode *dropTarget = dragTreeView->doDrop();
				if( dropTarget && dropTarget != dragItem )
				{
					handleTreeViewDrop( dragTreeView, dragItem, dropTarget );
				}
				//ShowCursor( TRUE );
				releaseMouse();
				dragTreeView = NULL;
				msgStatus = psPROCESSED;
				break;
			}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			controlKey = wParam & MK_CONTROL;
			shiftKey = wParam & MK_SHIFT;

			if( toolWin )
			{
				removeToolHelp();
			}
			else
			{
				Point point = MAKEPOINT( DWORD(lParam) );
				msgStatus = handleLeftButton( LeftButton(message), wParam, point );
			}
			break;
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			controlKey = wParam & MK_CONTROL;
			shiftKey = wParam & MK_SHIFT;

			if( toolWin )
			{
				removeToolHelp();
			}
			else
			{
				Point point = MAKEPOINT( DWORD(lParam) );
				msgStatus = handleRightButton( RightButton(message), wParam, point );
			}
			break;
		// Drag and Drop
		case WM_DROPFILES:
			msgStatus = psPROCESSED;
			handleFiles( (HDROP)wParam );
			break;

		// DDE messages
		case WM_DDE_INITIATE:
		{
			ATOM	appAtom;
			ATOM	topicAtom;
			char	application[32];
			char	topic[32];

			appAtom = LOWORD( lParam );
			topicAtom = HIWORD( lParam );

			GlobalGetAtomName( appAtom, application, sizeof( application ) );
			GlobalGetAtomName( topicAtom, topic, sizeof( topic ) );

			if( ddeInitiate( application, topic ) == psPROCESSED )
			{

				LPARAM lParam = PackDDElParam( WM_DDE_ACK, appAtom, topicAtom );
				SendMessage( HWND(wParam), WM_DDE_ACK, WPARAM(handle()), lParam );
			}
			else
			{
				msgStatus = psDO_DEFAULT;
			}

			break;
		}

		case WM_DDE_EXECUTE:
		{
			HGLOBAL	command = HGLOBAL(lParam);
			char	*commandPtr = static_cast<char *>(GlobalLock( command ));

			if( commandPtr )
			{
				if( ddeExecute( commandPtr ) == psPROCESSED )
				{
					GlobalUnlock( command );
					LPARAM lParam = PackDDElParam( WM_DDE_ACK, 0, UINT(command) );
					PostMessage( HWND(wParam), WM_DDE_ACK, WPARAM(handle()), lParam );
				}
				else
				{
					GlobalUnlock( command );
					msgStatus = psDO_DEFAULT;
				}
			}
			break;
		}

		case WM_DDE_TERMINATE:
			PostMessage( HWND(wParam), WM_DDE_TERMINATE, WPARAM(handle()), 0L );
			break;

		// help
		case WM_HELP:
		{
			if( toolWin )
			{
				removeToolHelp();
			}

			LPHELPINFO	info = LPHELPINFO(lParam);
			showHelp( info->iContextType, info->iCtrlId );
			break;
		}

		// timer
		case WM_TIMER:
			if( wParam == APPL_TIMER_ID )
			{
				handleTimer();
			}
			else
			{
				removeToolHelp();
			}
			break;

		// keyboard input
		case WM_GETDLGCODE:
			callBackValue = DLGC_WANTCHARS|DLGC_WANTARROWS|DLGC_WANTTAB|DLGC_WANTALLKEYS;
			if( wParam != 0 )
			{
				callBackValue |= DLGC_WANTTAB;
			}
			break;

		case WM_SETFOCUS:
			msgStatus = psDO_DEFAULT;
			handleFocus();
			break;

		case WM_KILLFOCUS:
			msgStatus = psDO_DEFAULT;
			if( hasCaret() )
				destroyCaret();
			handleKillFocus();
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if( wParam == VK_MENU )
				altKey = true;
			else if( wParam == VK_SHIFT )
				shiftKey = true;
			else if( wParam == VK_CONTROL )
				controlKey = true;

			msgStatus = handleKeyDown( int(wParam) );
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if( wParam == VK_MENU )
				altKey = false;
			else if( wParam == VK_SHIFT )
				shiftKey = false;
			else if( wParam == VK_CONTROL )
				controlKey = false;

			msgStatus = handleKeyUp( int(wParam) );
			break;
		case WM_CHAR:
			altKey = lParam & (1<<29);
			msgStatus = handleCharacterInput( int(wParam) );
			break;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			msgStatus = checkControlColor( HWND( lParam ), HDC(wParam) );
			break;

		case WM_SETFONT:
		{
			const ChildWindows &children = getChildren();
			for( 
				ChildWindows::const_iterator it = children.cbegin(), endIT = children.cend();
				it != endIT;
				++it
			)
			{
				BasicWindow *child = *it;
				if( !child->getFont().isAssigned() )
				{
					child->message( WM_SETFONT, wParam, lParam );
				}
			}
			break;
		}
		case WM_GETFONT:
			callBackValue = LRESULT(HFONT(getFont()));
			if( !callBackValue && getParent() )
				callBackValue = getParent()->message( WM_GETFONT );

			msgStatus = psPROCESSED;
			break;

		// The special (rarely used) messages
		default:
			msgStatus = handleMessage( message, wParam, lParam );
			break;
	}

	const BasicWindow *parent;
	if( (parent=getParent()) != NULL )
	{
		if( msgStatus == psSEND_TO_PARENT )
		{
			parent->message( message, wParam, lParam );
			msgStatus = psPROCESSED;
		}
		else if( msgStatus == psPOST_TO_PARENT )
		{
			parent->postMessage( message, wParam, lParam );
			msgStatus = psPROCESSED;
		}
	}

	LRESULT result = msgStatus == psDO_DEFAULT ? doDefault( message, wParam, lParam ) : callBackValue;

	if( destroyed && m_callBackCount <= 1 )
	{
		clrHandle();
	}

	return result;
}

void CallbackWindow::checkModifiers( unsigned uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if( wParam == VK_MENU )
				altKey = true;
			else if( wParam == VK_SHIFT )
				shiftKey = true;
			else if( wParam == VK_CONTROL )
				controlKey = true;
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if( wParam == VK_MENU )
				altKey = false;
			else if( wParam == VK_SHIFT )
				shiftKey = false;
			else if( wParam == VK_CONTROL )
				controlKey = false;
			break;
		case WM_CHAR:
			altKey = lParam & (1<<29);
			break;
	}
}

#ifdef _MSC_VER
LRESULT CallbackWindow::controlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam )
{

	checkModifiers( uMsg, wParam, lParam );

	ProcessStatus doDefault = preControlCallback( control, uMsg, wParam, lParam );
	if( doDefault == psDO_DEFAULT )
		callBackValue = DefSubclassProc( control->handle(), uMsg, wParam, lParam );
	postControlCallback( control, uMsg, wParam, lParam );

	return callBackValue;
}
#endif

ProcessStatus CallbackWindow::checkControlColor( HWND handle, HDC device )
{
	BasicWindow *control = findChild( handle );
	if( control )
	{
		const Brush &brush = control->getBackgroundBrush();
		if( bool(brush) )
		{
			COLORREF backgroundColor = brush.getBackgroundColor();
			SetBkColor( device, backgroundColor );
			callBackValue = LRESULT(HBRUSH(brush));
			return psPROCESSED;
		}
	}

	return psDO_DEFAULT;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

bool CallbackWindow::handleTreeViewDrag( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dragOver )
{
	bool allow = false;

	const BasicWindow *parent = getParent();

	if( parent && (parent->getWindowClass() & CallbackWindowFlag) )
	{
		CallbackWindow	*cbParent = static_cast<CallbackWindow *>(
			const_cast<BasicWindow*>( parent )
		);
		allow = cbParent->handleTreeViewDrag( dragTreeView, dragItem, dragOver );
	}

	return allow;
}

void CallbackWindow::handleTreeViewDrop( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dropTarget )
{
	const BasicWindow *parent = getParent();

	if( parent && (parent->getWindowClass() & CallbackWindowFlag) )
	{
		CallbackWindow	*cbParent = static_cast<CallbackWindow *>(
			const_cast<BasicWindow*>( parent )
		);
		cbParent->handleTreeViewDrop( dragTreeView, dragItem, dropTarget );
	}
}

SuccessCode CallbackWindow::close( void )
{
	removeTimer();
	removeToolHelp();
	removeStatusBar();

	m_closed = true;

	return BasicWindow::close();
}

ProcessStatus CallbackWindow::handleShow( void )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleHide( void )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleCreate( void )
{
	return psPROCESSED;			// don't call default funtion
}

void CallbackWindow::getControls( void )
{
}

SuccessCode CallbackWindow::handleClose( void )
{
	if( canClose() )
		return close();

	return scERROR;
}

ProcessStatus CallbackWindow::handleDestroy( void )
{
	return psPROCESSED;
}

void CallbackWindow::handleActivate( bool )
{
}

ProcessStatus CallbackWindow::handleResize( const Size &newSize )
{
	doLayout( newSize );

	return psDO_DEFAULT; 
}

ProcessStatus CallbackWindow::handleErase( Device &hDC )
{
	const Brush	&background = getBackgroundBrush();
	if( bool(background) )
	{
		Size	size = getClientSize();
		hDC.getPen().selectPen( Pen::spNull );
		hDC.selectBrush( background );
		hDC.rectangle( 0, 0, size.width+1, size.height+1 );
		return psPROCESSED;
	}
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleRepaint( Device & )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleEditChange( int /* editControl */ )
{
	return psSEND_TO_PARENT;
}

ProcessStatus CallbackWindow::handleSelectionChange( int /* control */ )
{
	return psSEND_TO_PARENT;
}

ProcessStatus CallbackWindow::handleNotification( int /* notification*/ , int /* control */ )
{
	return psSEND_TO_PARENT;
}

ProcessStatus CallbackWindow::handleButtonClick( int buttonControl )
{
	return handleCommand( buttonControl );
}

ProcessStatus CallbackWindow::handleCommand( int /* command */ )
{
	return psPOST_TO_PARENT;
}

ProcessStatus CallbackWindow::handleScrollControl( int control )
{
	return handleCommand( control );
}

ProcessStatus CallbackWindow::handleVertScroll( VertScrollCode , int , HWND scrollBar )
{
	int id = GetWindowLong( scrollBar, GWL_ID );
	if( id >= 0 )
		return handleScrollControl( id );
	else
		return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleHorizScroll( HorizScrollCode , int , HWND scrollBar )
{
	int id = GetWindowLong( scrollBar, GWL_ID );
	if( id >= 0 )
		return handleScrollControl( id );
	else
		return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleMouseRelease( void )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleMouseMove( WPARAM, const Point & )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleLeftButton( LeftButton, WPARAM, const Point & )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleRightButton( RightButton, WPARAM, const Point & )
{
	return psDO_DEFAULT;
}

void CallbackWindow::handleFile( const char *, size_t, size_t )
{
}

void CallbackWindow::handleFind( const char *, bool, bool, bool )
{
}

void CallbackWindow::handleReplace( const char *, const char *,
								bool, bool, bool )
{
}

ProcessStatus CallbackWindow::handleMessage( UINT, WPARAM, LPARAM )
{
	return psDO_DEFAULT;
}

LRESULT CallbackWindow::doDefault( UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( handle(), message, wParam, lParam );
}

bool CallbackWindow::translateAccell( MSG &msg ) const
{
	return s_hAccel && TranslateAccelerator( handle(), s_hAccel, &msg )
			? true
			: false;
}

ProcessStatus CallbackWindow::ddeInitiate( const char *, const char * )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::ddeExecute( const char * )
{
	return psDO_DEFAULT;
}


void CallbackWindow::showHelp( int , int controlId )
{
	toolWin = new ToolTipWindow;

	toolWin->create( this, controlId );
	createTimer( 5000, false );
}

void CallbackWindow::handleTimer( void )
{
}



void CallbackWindow::handleFocus( void )
{
}

void CallbackWindow::handleKillFocus( void )
{
}

ProcessStatus CallbackWindow::handleCharacterInput( int )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleKeyDown( int )
{
	return psDO_DEFAULT;
}

ProcessStatus CallbackWindow::handleKeyUp( int )
{
	return psDO_DEFAULT;
}

#ifdef _MSC_VER
ProcessStatus CallbackWindow::preControlCallback( BasicWindow * /*control*/, unsigned /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	return psDO_DEFAULT;
}

void CallbackWindow::postControlCallback( BasicWindow * /*control*/, unsigned /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
}
#endif

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void CallbackWindow::mainLoop( void )
{
	MSG 			msg;
	CallbackWindow	*curWindow;
	ListCursor		cursor;

	m_callBackCount++;	// do not delete the main window during the mainloop

	while( !terminate && GetMessage( &msg, 0, 0, 0 ) )
	{
		if( !msg.hwnd && handle() )
		{
			SendMessage( handle(), msg.message, msg.wParam, msg.lParam );
		}
		else if( !translateAccell( msg ) )
		{
			if( findDialog
			&&	findDialog->isValid()
			&&	IsDialogMessage( findDialog->handle(), &msg ) )
			{
/*^*/			continue;
			}

			curWindow = openWin ? static_cast<CallbackWindow *>(openWin->getFirst(&cursor)) : NULL;
			while( curWindow )
			{
				if( curWindow->isValid()
				&&  curWindow->isDialogMessage( msg ) )
				{
					break;
				}
				curWindow = static_cast<CallbackWindow *>(cursor.getNext());
			}
			if( curWindow )
			{
/*^*/			continue;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	m_callBackCount--;
}

void CallbackWindow::doModalDialog( int id )
{
	ModalDialog	dialog;

	dialog.create( id, this );
}

void CallbackWindow::showToolHelp( const char *text )
{
	if( toolWin )
	{
		toolWin->changeText( text );
	}
	else
	{
		toolWin = new ToolTipWindow;

		toolWin->create( this, text );
		createTimer( 5000, false );
	}
}

void CallbackWindow::removeToolHelp( void )
{
	if( toolWin )
	{
		toolWin->close();
		delete toolWin;
		toolWin = 0;
		removeTimer( false );
	}
}

void CallbackWindow::addStatusBar( void )
{
	if( !statusBar )
	{
		statusBar = new EditControl;

		if( statusBar )
		{
			Size	mySize = getClientSize();
			statusBar->create( this, 0, mySize.height - STATUS_HEIGHT, mySize.width, STATUS_HEIGHT );
			statusBar->setReadonly();
			resize( mySize );
		}
	}
}

void CallbackWindow::setStatusText( const STRING &value )
{
	if( statusBar )
		*statusBar = value;
}

void CallbackWindow::removeStatusBar( void )
{
	if( statusBar )
	{
		statusBar->close();
		delete statusBar;

		statusBar = 0;
	}
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

extern "C" LRESULT PASCAL WindowProc(
	HWND handle, UINT message,
	WPARAM wParam, LPARAM lParam 
)
{
	LRESULT returnVal = 0;
	try
	{
		CallbackWindow	*curWindow = NULL;
		ListCursor		cursor;

		if( message == WM_CREATE || message == WM_NCCREATE )
		{
			CREATESTRUCT *createStruct = (CREATESTRUCT *)lParam;
			curWindow = (CallbackWindow *)createStruct->lpCreateParams;
			if( !curWindow )
			{
				int controlId = GetWindowLong( handle, GWL_ID );
				for(
					curWindow = openWin ? (CallbackWindow*)openWin->getFirst( &cursor ) : NULL;
					curWindow;
					curWindow = (CallbackWindow *)cursor.getNext()
				)
				{
					if( curWindow->getId() == controlId )
						break;
				}
			}
		}
		else if( message == WM_INITDIALOG )
			curWindow = (CallbackWindow *)lParam;

		if( curWindow )
		{
			if( !curWindow->handle() )
				curWindow->setHandle( handle );
			returnVal = curWindow->callBack( message, wParam, lParam );
		}
		else
		{
			returnVal = CallbackWindow::dispatch( handle, message, wParam, lParam );
		}
	}
	catch( std::exception &e )
	{
		::MessageBox( NULL, e.what(), "Error", MB_ICONERROR );
	}
	catch( ... )
	{
		::MessageBox( NULL, "Unknown error", "Error", MB_ICONERROR );
	}
	return returnVal;
}

#ifdef _MSC_VER
LRESULT CALLBACK controlProcedure(
	HWND /*hWnd*/,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
)
{
	CallbackWindow	*window = reinterpret_cast<CallbackWindow*>(dwRefData);
	ControlWindow	*control = reinterpret_cast<ControlWindow*>(uIdSubclass);

	return window->controlCallback( control, uMsg, wParam, lParam );
}
#endif

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

