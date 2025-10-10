/*
		Project:		Windows Class Library
		Module: 		MDIWIN.CPP
		Description:	The MDI-window classes
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
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib\mdiwin.h>
#include <winlib\winapp.h>

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

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

STRING MDIwindow::className;

STRING MDIchildWindow::className;

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void MDIchildWindow::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS wc;

		className = appObject->getApplication();
		className += "_MDI_CHILD_WIN";

		fillDefaultClass( &wc );
		wc.lpszClassName = className;
		wc.lpfnWndProc		= MDIchildWindowProc;
		registered = CallbackWindow::registerClass( &wc );
	}
}

void MDIwindow::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		className = appObject->getApplication();
		className += "_MDI_WIN";

		fillDefaultClass( &wc );

		wc.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE + 1);
		wc.lpszClassName	= className;

		registered = CallbackWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

/*
	MDIchildWindow
*/
STRING MDIchildWindow::getWindowClassName() const
{
	return className;
}

void MDIchildWindow::handleActivate( bool activate )
{
	if( activate )
		m_clientParent->setActiveWindow( this, m_menu, m_winMenu );
	else
		m_clientParent->setNoWindow();

	m_clientParent->getParent()->drawMenuBar();
}

ProcessStatus MDIchildWindow::handleDestroy()
{
	// tell parent, that i'm leaving
	m_clientParent->removeChild( this );

	// remove from registered windows
	clrHandle();

	return psPROCESSED;
}

SuccessCode MDIchildWindow::close()
{
	setClosedFlag();
	m_clientParent->delChild( this );

	return scSUCCESS;
}

LRESULT MDIchildWindow::doDefault( UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefMDIChildProc( handle(), message, wParam, lParam );
}

/*
	MDIwindow
*/
STRING MDIwindow::getWindowClassName() const
{
	return className;
}

SuccessCode MDIwindow::handleClose()
{
	if( clientWindow.handleClose() == scSUCCESS )
	{
		close();
		return scSUCCESS;
	}

	return scERROR;
}

ProcessStatus MDIwindow::handleResize( const Size &point )
{
	if( hasStatusBar() )
	{
		clientWindow.resize( point.width, point.height - STATUS_HEIGHT );
		return psPROCESSED;
	}
	return psDO_DEFAULT;
}

LRESULT MDIwindow::doDefault( UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefFrameProc( handle(), clientWindow.handle(),
					message, wParam, lParam );
}

bool MDIwindow::translateAccell( MSG &msg ) const
{
	return (TranslateMDISysAccel( clientWindow.handle(), &msg )
			|| CallbackWindow::translateAccell( msg ))
			? true
			: false;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

/*
	CHILD_WIN
*/
SuccessCode MDIchildWindow::create(
	const MDIwindow *parent,
	const char *title, int menuId, int winMenu
)
{
	MDICREATESTRUCT 	mdiCreate;
	HWND				handle;

	if( menuId )
	{
		const Menu *menu = setMenu( menuId );
		if( !menu )
			return scERROR;
		m_winMenu = menu->getSubMenu( winMenu );
	}
	m_clientParent = const_cast<MDIclientWindow *>(parent->getClient());
	mdiCreate.szClass = className;
	mdiCreate.szTitle = (LPSTR)title;
	mdiCreate.hOwner  = appObject->getAppInstance();
	mdiCreate.x 	  = CW_USEDEFAULT;
	mdiCreate.y 	  = CW_USEDEFAULT;
	mdiCreate.cx	  = CW_USEDEFAULT;
	mdiCreate.cy	  = CW_USEDEFAULT;
	mdiCreate.style   = 0;
	mdiCreate.lParam  = LPARAM(this);

	handle = m_clientParent->createChild( &mdiCreate );

	if( handle )
	{
		m_clientParent->newChild( this );
		return scSUCCESS;
	}
	else
	{
		return scERROR;
	}
}

SuccessCode MDIchildWindow::create( 
	const MDIwindow *parent,
	int titleId, int menuId, int winMenu
)
{
	STRING	title = appObject->loadString( titleId );

	if( title.isNullPtr() )
		return scERROR;
	return create( parent, title, menuId, winMenu );
}

/*
	MDIclientWindow
*/
STRING MDIclientWindow::getWindowClassName() const
{
	return "MDICLIENT";
}

void MDIclientWindow::closeAllChildren()
{
	MDIchildWindow	*current;
	ListCursor 	cursor;

	current = (MDIchildWindow *)((MdiChildList *)m_childList.getFirst( &cursor ));
	while( current )
	{
		delChild( current );
		current = (MDIchildWindow *)((MdiChildList *)cursor.getNext());
	}
}

SuccessCode MDIclientWindow::handleClose()
{
	MDIchildWindow	*current;
	ListCursor	 	cursor;

	current = (MDIchildWindow *)((MdiChildList *)m_childList.getFirst( &cursor ));
	while( current )
	{
		if( current->handleClose() == scERROR )
			return scERROR;
		current = (MDIchildWindow *)((MdiChildList *)cursor.getNext());
	}
	return scSUCCESS;
}

SuccessCode MDIclientWindow::close()
{
	closeAllChildren();
	// DestroyMenu( menu );

	return scSUCCESS;
}

/*
	MDIwindow
*/
SuccessCode MDIwindow::create( int menuId, int winMenu )
{
	doEnterFunction("MDIwindow::create");

	setMenu( menuId );
	SuccessCode error = CallbackWindow::create( nullptr );

	if( error == scSUCCESS )
	{
		error = clientWindow.create(this, m_menu ? m_menu->getSubMenu( winMenu ) : nullptr );
	}
	return error;
}

SuccessCode MDIwindow::close()
{
	clientWindow.close();
	return CallbackWindow::close();
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

extern "C" LRESULT PASCAL MDIchildWindowProc(
	HWND handle, UINT message,
	WPARAM wParam, LPARAM lParam 
)
{
	doEnterFunction("MDIchildWindowProc");

	LRESULT			returnVal;

	if( message == WM_CREATE || message == WM_NCCREATE )
	{
		CREATESTRUCT	*createStruct = (CREATESTRUCT *)lParam;
		MDICREATESTRUCT	*mdiCreateStruct = (MDICREATESTRUCT *)createStruct->lpCreateParams;
		CallbackWindow	*curWindow = (CallbackWindow *)mdiCreateStruct->lParam;

		if( !curWindow->handle() )
			curWindow->setHandle( handle );
		returnVal = curWindow->callBack( message, wParam, lParam );
	}
	else
	{
		returnVal = CallbackWindow::dispatch( handle, message, wParam, lParam );
	}
	return returnVal;
}

}	// namespace winlib


#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

