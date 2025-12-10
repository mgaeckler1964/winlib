/*
		Project:		Windows Class Library
		Module:			POPUP.H
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

#ifndef POPUP_H
#define POPUP_H

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib\callwin.h>

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

class PopupWindow : public CallbackWindow
{
	private:
	static STRING	className;
	virtual STRING	getWindowClassName() const;
	static void registerClass();

	protected:
	SuccessCode create( BasicWindow *parent, const char *resourceName );

	public:
	PopupWindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
		setStyle( WS_POPUPWINDOW|WS_VISIBLE );
		registerClass();
	}
	SuccessCode create( BasicWindow *parent, int nWidth=0, int nHeight=0 );

	protected:
	virtual ProcessStatus handleOk();
	virtual ProcessStatus handleCancel();
	virtual ProcessStatus handleButtonClick( int cmd );
};

class ModalPopup : public PopupWindow
{
	int		modalResult;
	HWND	activeWindow;

	private:
	void DisableAllWindows();
	void EnableAllWindows();

	protected:
	void setModalResult( int result )
	{
		modalResult = result;
	}

	SuccessCode create( BasicWindow * parent, const char *resourceName )
	{
		SuccessCode		errFlag = PopupWindow::create( parent, resourceName );
		if( !BasicWindow::isVisible() )
		{
			close();
			errFlag = scERROR;
		}
		else if( errFlag == scSUCCESS )
			mainLoop();

		return errFlag;
	}
	using PopupWindow::create;
	virtual ProcessStatus handleOk();
	virtual ProcessStatus handleCancel();
	virtual ProcessStatus handleDestroy();
	virtual ProcessStatus handleShow();
	virtual ProcessStatus handleHide();

	public:
	ModalPopup(BasicWindow *owner=NULL) : PopupWindow( owner )
	{
		setStyle( DS_MODALFRAME|WS_POPUPWINDOW|WS_VISIBLE );
		modalResult = 0;
		activeWindow = 0;
	}
	int getModalResult() const
	{
		return modalResult;
	}
};

class OverlappedWindow : public CallbackWindow
{
	private:
	static STRING	className;
	virtual STRING	getWindowClassName() const;
	static void registerClass();

	public:
	OverlappedWindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
		setStyle( WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_VISIBLE );
		registerClass();
	}
	SuccessCode create( BasicWindow *parent, int width=0, int height=0 )
	{
		if( width || height )
			resize( width, height );
		return CallbackWindow::create( parent );
	}
	SuccessCode create( BasicWindow *parent, const char *resourceName );
};

class ToolWindow : public CallbackWindow
{
	private:
	static STRING	className;
	virtual STRING	getWindowClassName() const;
	static void registerClass();

	public:
	ToolWindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
		setStyle( WS_VISIBLE );
		setExStyle( WS_EX_TOOLWINDOW );

		registerClass();
	}
	SuccessCode create( BasicWindow *parent, int width, int height, const STRING &title )
	{
		setText( title );
		resize( width, height );
		return CallbackWindow::create( parent );
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

