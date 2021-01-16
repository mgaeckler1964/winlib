/*
		Project:		Windows Class Library
		Module: 		MDIWIN.H
		Description:	The MDI-window classes
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

#ifndef MDI_WIN_H
#define MDI_WIN_H

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/list.h>
#include <winlib/callwin.h>
#include <winlib/controlw.h>
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

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

/*
	Child Window Class
	==================

	The "Child Window Class" describes the child windows of a MDI window.
*/

class MDIclientWindow;
class MDIwindow;

class MdiChildList : public gak::ListEntry
{
};

class MDIchildWindow : public CallbackWindow, public MdiChildList
{
	private:
	MDIclientWindow	*myParent;

	Menu	*winMenu;

	private:
	static STRING	className;
	virtual STRING	getWindowClassName( void ) const;
	static void registerClass( void );

	protected:
	MDIchildWindow() : CallbackWindow( NULL )
	{
		myParent = NULL;
		winMenu  = NULL;
		registerClass();
	}
	~MDIchildWindow()
	{
		if( winMenu )
			delete winMenu;
	}
	public:
	SuccessCode create( const MDIwindow *parent,
				 const char *title, int menuId, int winMenu );
	SuccessCode create( const MDIwindow *parent,
				 int titleId, int menuId, int winMenu );
	virtual SuccessCode close( void );

	// message handling
	private:
	virtual LRESULT doDefault( UINT, WPARAM, LPARAM );
	protected:
	virtual void handleActivate( bool );
	virtual ProcessStatus handleDestroy( void );

	// querrying
	public:
	const MDIwindow *getMDIparent( void ) const;
};

/*
	The Client Window Class
	=======================

	The "Client Window Class" describes a window, which is the client area
	of an MDI window.
*/

class MDIclientWindow : virtual public ControlWindow
{
	friend class MDIwindow;
	friend class MDIchildWindow;

	virtual STRING getWindowClassName( void ) const;

	private:
	Menu				*winMenu;
	const MDIwindow		*myParent;

	gak::ListContainer	childList;
	gak::ListCursor	 	cursor;
	MDIchildWindow		*activeChild;

	private:
	MDIclientWindow()
	{
		myParent	= NULL;
		activeChild = NULL;
		winMenu 	= NULL;
		setStyle( WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE );
	}
	~MDIclientWindow()
	{
		if( winMenu )
			delete winMenu;
	}

	SuccessCode create( MDIwindow *parent, Menu *winMenu )
	{
		doEnterFunction("MDIclientWindow::create");
		CLIENTCREATESTRUCT	clientCreate;

		this->winMenu = winMenu;
		myParent	  = parent;

		clientCreate.hWindowMenu  = winMenu ? winMenu->getMenu() : NULL;
		clientCreate.idFirstChild = 1024;

		SuccessCode error = BasicWindow::create(
			reinterpret_cast<BasicWindow*>(parent), 0, (LPSTR)&clientCreate
		);
		return error;
	}

	virtual SuccessCode close( void );
	virtual SuccessCode handleClose( void );

	// add new child from children's chain
	void newChild( MDIchildWindow *newChild )
	{
		childList.addElement( (gak::ListEntry *)((MdiChildList*)newChild) );
	}
	// remove child from our chain
	void removeChild( MDIchildWindow *delChild )
	{
		childList.removeObject( (gak::ListEntry *)((MdiChildList*)delChild) );
	}

	HWND createChild( MDICREATESTRUCT *mdiCreateStruct )
	{
		HWND handle;

		handle = (HWND)LOWORD(
						message( WM_MDICREATE,0,
								(LONG)(LPMDICREATESTRUCT)mdiCreateStruct ) );
		return handle;
	}
	// delete child then remove from chain
	void delChild( const MDIchildWindow *delChild )
	{
		message( WM_MDIDESTROY, WPARAM(delChild->handle()), 0 );
	}
	void closeAllChildren( void );

	const MDIwindow *getParent( void ) const
	{
		return myParent;
	}

	// handle active child
	void setActiveWindow(
		MDIchildWindow *newActiveChild,
		Menu *menu, Menu *winMenu
	)
	{
		activeChild = newActiveChild;
		if( menu )
			message( WM_MDISETMENU, WPARAM(menu->getMenu()), LPARAM(winMenu ? winMenu->getMenu() : NULL) );
	}
	void setNoWindow( void );
	void sendMenuCommand( int menu )
	{
		if( activeChild )
			activeChild->message( WM_COMMAND, menu );
	}
	MDIchildWindow *getActiveChild( void ) const
	{
		return activeChild;
	}
	MDIchildWindow *getFirstChild( void )
	{
		return (MDIchildWindow*)(MdiChildList*)childList.getFirst( &cursor );
	}
	MDIchildWindow *getNextChild( void )
	{
		return (MDIchildWindow*)(MdiChildList*)cursor.getNext();
	}
};

/*
	The MDI Window Class
	====================

	The "MDI Window Class" describes a window, which is a "Multiple Document
	Interface" Window.
*/

class MDIwindow : public CallbackWindow
{
	private:
	static STRING	className;
	virtual STRING	getWindowClassName( void ) const;
	static void registerClass( void );

	MDIclientWindow	clientWindow;

	// creating and destroying
	public:
	MDIwindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
		registerClass();
		setStyle( WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );
	}
	SuccessCode create( int menuId, int winMenu );
	virtual SuccessCode close( void );

	// children arrangment
	public:
	void cascadeChildren( void ) const
	{
		clientWindow.message( WM_MDICASCADE, 0, 0L );
	}
	void tileChildren( bool horizontal = true ) const
	{
		clientWindow.message( WM_MDITILE, horizontal
												? MDITILE_HORIZONTAL
												: MDITILE_VERTICAL,
								0L );
	}
	void arrangeIcons( void ) const
	{
		clientWindow.message( WM_MDIICONARRANGE, 0, 0L );
	}
	void closeAllChildren( void )
	{
		clientWindow.closeAllChildren();
	}

	// windows message handling
	private:
	virtual LRESULT doDefault( UINT, WPARAM, LPARAM );

	virtual bool translateAccell( MSG &msg ) const;
	public:
	virtual SuccessCode handleClose( void );
	virtual ProcessStatus handleResize( const Size & );

	// menu commands for children
	public:
	void sendMenuCommand( int menu )
	{
		clientWindow.sendMenuCommand( menu );
	}

	// Querrying
	public:
	const MDIclientWindow *getClient( void ) const
	{
		return &clientWindow;
	}
	MDIchildWindow *getActiveChild( void ) const
	{
		return clientWindow.getActiveChild();
	}
	MDIchildWindow *getFirstChild( void )
	{
		return clientWindow.getFirstChild();
	}
	MDIchildWindow *getNextChild( void )
	{
		return clientWindow.getNextChild();
	}
};

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

inline const MDIwindow *MDIchildWindow::getMDIparent( void ) const
{
	return myParent->getParent();
}

inline void MDIclientWindow::setNoWindow( void )
{
	activeChild = NULL;
	message(
		WM_MDISETMENU, (WPARAM)myParent->getMenu(), (LPARAM)(winMenu ? winMenu->getMenu() : NULL) 
	);
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
