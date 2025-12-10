/*
		Project:		Windows Class Library
		Module:			POPUP.CPP
		Description:	Popup Windows
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

#include <winlib/popup.h>
#include <WINLIB/resource.h>

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

STRING	PopupWindow::className;
STRING	OverlappedWindow::className;
STRING	ToolWindow::className;

/*
	popup && parent:
	over parent, no taskbar, auto close, pos rel desk

	popup && !parent:
	under parent, taskbar, no auto close, pos rel desk

	overlapped && parent:
	over parent, no taskbar, auto close, pos rel desk

	overlapped && !parent:
	under parent, taskbar, no auto close, pos rel desk
*/

/* --------------------------------------------------------------------- */
/* ----- module functions ---------------------------------------------- */
/* --------------------------------------------------------------------- */

BOOL _export pascal EnableDisableProc( HWND window, LPARAM lParam )
{
	bool	bEnable = lParam && (HWND(lParam) != window) ? false : true;
	
	EnableWindow( window, bEnable );

	return true;
}

void ModalPopup::DisableAllWindows()
{
	FARPROC proc;

	activeWindow = GetActiveWindow();

	proc = MakeProcInstance( (FARPROC)EnableDisableProc, getInstance() );
	EnumThreadWindows( GetCurrentThreadId(), (WNDENUMPROC)proc, (LPARAM)handle() );
	FreeProcInstance( proc );
}

void ModalPopup::EnableAllWindows()
{
	FARPROC proc;

	proc = MakeProcInstance( (FARPROC)EnableDisableProc, getInstance() );
	EnumThreadWindows( GetCurrentThreadId(), (WNDENUMPROC)proc, (LPARAM)false );

	FreeProcInstance( proc );
	if( activeWindow )
	{
		SetActiveWindow( activeWindow );
		activeWindow = 0;
	}
}

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void PopupWindow::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS wc;

		className = appObject->getApplication();
		className += "_POPUP_WINDOW";

		fillDefaultClass( &wc );
		wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);

		wc.lpszClassName = className;
		registered = CallbackWindow::registerClass( &wc );
	}
}

void OverlappedWindow::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS wc;

		className = appObject->getApplication();
		className += "_OVERLAPPED_WINDOW";

		fillDefaultClass( &wc );
		wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);

		wc.lpszClassName = className;
		registered = CallbackWindow::registerClass( &wc );
	}
}

void ToolWindow::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS wc;

		className = appObject->getApplication();
		className += "_TOOL_WINDOW";

		fillDefaultClass( &wc );
		wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);


		wc.lpszClassName = className;
		registered = CallbackWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING PopupWindow::getWindowClassName() const
{
	return className;
}

STRING OverlappedWindow::getWindowClassName() const
{
	return className;
}

STRING ToolWindow::getWindowClassName() const
{
	return className;
}

SuccessCode PopupWindow::create( BasicWindow *parent, const char *resourceName )
{
	xml::Element	*resource = appObject->getFormResource( resourceName );

	if( resource )
	{
		createForm( appObject->getGuiFile(), resource, this, parent, false );

		return scSUCCESS;
	}
	else
	{
		return scERROR;
	}
}

SuccessCode PopupWindow::create( BasicWindow *parent, int nWidth, int nHeight )
{
	int		x, y;
	if( !nWidth || !nHeight )
	{
		Size	size = getSize();
		if( !nWidth )
			nWidth = size.width;
		if( !nHeight )
			nHeight = size.height;
	}
	if( parent )
	{
		RectBorder	rect = parent->getWindowRectangle();

		x = (rect.left + rect.right)/2;
		x -= nWidth/2;

		y = (rect.top + rect.bottom)/2;
		y -= nHeight/2;

		if( x < 0 )
		{
			x = 0;
		}
		if( y < 0 )
		{
			y = 0;
		}
	}
	else
	{
		x = y = 0;
	}

	STRING title = getTitle();
	if( !title.isEmpty() )
		addStyle( WS_CAPTION );

	addStyle( WS_POPUP );

	sizeNmove( x, y, nWidth, nHeight );
	return CallbackWindow::create( parent );
};

ProcessStatus ModalPopup::handleShow()
{
	PopupWindow::handleShow();
	DisableAllWindows();

	return psDO_DEFAULT;
}

ProcessStatus ModalPopup::handleHide()
{
	EnableAllWindows();
	return psDO_DEFAULT;
}

ProcessStatus ModalPopup::handleDestroy()
{
	EnableAllWindows();

	terminateLoop();

	return psDO_DEFAULT;
}

ProcessStatus PopupWindow::handleOk()
{
	if( canClose() )
	{
		close();
		return psPROCESSED;
	}
	return psDO_DEFAULT;
}

ProcessStatus PopupWindow::handleCancel()
{
	if( canClose() )
	{
		close();
		return psPROCESSED;
	}
	return psDO_DEFAULT;
}

ProcessStatus PopupWindow::handleButtonClick( int cmd )
{
	if( cmd == IDOK && handleOk() == psPROCESSED )
		return psPROCESSED;

	else if( cmd == IDCANCEL && handleCancel() == psPROCESSED )
		return psPROCESSED;

	return CallbackWindow::handleButtonClick( cmd );
}

ProcessStatus ModalPopup::handleOk()
{
	setModalResult( IDOK );
	return PopupWindow::handleOk();
}

ProcessStatus ModalPopup::handleCancel()
{
	setModalResult( IDCANCEL );
	return PopupWindow::handleCancel();
}

SuccessCode OverlappedWindow::create( BasicWindow *parent, const char *resourceName )
{
	xml::Element	*resource = appObject->getFormResource( resourceName );

	if( resource )
	{
		createForm( appObject->getGuiFile(), resource, this, parent, false );

		return scSUCCESS;
	}
	else
		return scERROR;
}


}	// namespace winlib


#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


