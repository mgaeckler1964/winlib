/*
		Project:		Windows Class Library
		Module: 		MDIWIN.H
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
	MDIclientWindow	*m_clientParent;

	Menu	*m_winMenu;

	private:
	static STRING	className;
	virtual STRING	getWindowClassName() const;
	static void registerClass();

	protected:
	MDIchildWindow() : CallbackWindow( nullptr )
	{
		m_clientParent = nullptr;
		m_winMenu  = nullptr;
		registerClass();
	}
	~MDIchildWindow()
	{
		if( m_winMenu )
			delete m_winMenu;
	}
	public:
	SuccessCode create( const MDIwindow *parent,
				 const char *title, int menuId, int winMenu );
	SuccessCode create( const MDIwindow *parent,
				 int titleId, int menuId, int winMenu );
	virtual SuccessCode close();

	// message handling
	private:
	virtual LRESULT doDefault( UINT, WPARAM, LPARAM );
	protected:
	virtual void handleActivate( bool );
	virtual ProcessStatus handleDestroy();

	// querrying
	public:
	const MDIwindow *getMDIparent() const;
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

	virtual STRING getWindowClassName() const;

	private:
	Menu				*m_winMenu;
	const MDIwindow		*m_myParent;

	gak::ListContainer	m_childList;
	gak::ListCursor	 	m_cursor;
	MDIchildWindow		*m_activeChild;

	private:
	MDIclientWindow()
	{
		m_myParent	= nullptr;
		m_activeChild = nullptr;
		m_winMenu 	= nullptr;
		setStyle( WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE );
	}
	~MDIclientWindow()
	{
		if( m_winMenu )
			delete m_winMenu;
	}

	SuccessCode create( MDIwindow *parent, Menu *winMenu )
	{
		doEnterFunction("MDIclientWindow::create");
		CLIENTCREATESTRUCT	clientCreate;

		m_winMenu = winMenu;
		m_myParent	  = parent;

		clientCreate.hWindowMenu  = winMenu ? winMenu->getMenu() : nullptr;
		clientCreate.idFirstChild = 1024;

		SuccessCode error = BasicWindow::create(
			reinterpret_cast<BasicWindow*>(parent), 0, (LPSTR)&clientCreate
		);
		return error;
	}

	virtual SuccessCode close();
	virtual SuccessCode handleClose();

	// add new child from children's chain
	void newChild( MDIchildWindow *newChild )
	{
		m_childList.addElement( (gak::ListEntry *)((MdiChildList*)newChild) );
	}
	// remove child from our chain
	void removeChild( MDIchildWindow *delChild )
	{
		m_childList.removeObject( (gak::ListEntry *)((MdiChildList*)delChild) );
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
	void closeAllChildren();

	const MDIwindow *getParent() const
	{
		return m_myParent;
	}

	// handle active child
	void setActiveWindow(
		MDIchildWindow *newActiveChild,
		Menu *menu, Menu *winMenu
	)
	{
		m_activeChild = newActiveChild;
		if( menu )
			message( WM_MDISETMENU, WPARAM(menu->getMenu()), LPARAM(winMenu ? winMenu->getMenu() : nullptr) );
	}
	void setNoWindow();
	void sendMenuCommand( int menu )
	{
		if( m_activeChild )
			m_activeChild->message( WM_COMMAND, menu );
	}
	MDIchildWindow *getActiveChild() const
	{
		return m_activeChild;
	}
	MDIchildWindow *getFirstChild()
	{
		return (MDIchildWindow*)(MdiChildList*)m_childList.getFirst( &m_cursor );
	}
	MDIchildWindow *getNextChild()
	{
		return (MDIchildWindow*)(MdiChildList*)m_cursor.getNext();
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
	virtual STRING	getWindowClassName() const;
	static void registerClass();

	MDIclientWindow	clientWindow;

	// creating and destroying
	public:
	MDIwindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
		registerClass();
		setStyle( WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );
	}
	SuccessCode create( int menuId, int winMenu );
	virtual SuccessCode close();

	// children arrangment
	public:
	void cascadeChildren() const
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
	void arrangeIcons() const
	{
		clientWindow.message( WM_MDIICONARRANGE, 0, 0L );
	}
	void closeAllChildren()
	{
		clientWindow.closeAllChildren();
	}

	// windows message handling
	private:
	virtual LRESULT doDefault( UINT, WPARAM, LPARAM );

	virtual bool translateAccell( MSG &msg ) const;
	public:
	virtual SuccessCode handleClose();
	virtual ProcessStatus handleResize( const Size & );

	// menu commands for children
	public:
	void sendMenuCommand( int menu )
	{
		clientWindow.sendMenuCommand( menu );
	}

	// Querrying
	public:
	const MDIclientWindow *getClient() const
	{
		return &clientWindow;
	}
	MDIchildWindow *getActiveChild() const
	{
		return clientWindow.getActiveChild();
	}
	MDIchildWindow *getFirstChild()
	{
		return clientWindow.getFirstChild();
	}
	MDIchildWindow *getNextChild()
	{
		return clientWindow.getNextChild();
	}
};

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

inline const MDIwindow *MDIchildWindow::getMDIparent() const
{
	return m_clientParent->getParent();
}

inline void MDIclientWindow::setNoWindow()
{
	m_activeChild = nullptr;
	message(
		WM_MDISETMENU, WPARAM(m_myParent->getMenu()), LPARAM(m_winMenu ? m_winMenu->getMenu() : nullptr) 
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
