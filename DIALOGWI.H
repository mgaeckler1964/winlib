/*
		Project:		Windows Class Library
		Module:			DIALOGWI.H
		Description:	Implementation of dialog windows
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


#ifndef DIALOG_WIN_H
#define DIALOG_WIN_H

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak\string.h>

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

class DialogWindow : public CallbackWindow
{
	public:
	DialogWindow( BasicWindow *owner ) : CallbackWindow( owner )
	{
	}
	// text of an control
	void setButtonLabel( int buttonId, const char *label ) const
	{
		SetDlgItemText( handle(), buttonId, (LPSTR)label );
	}
	void getButtonLabel( int buttonId, char *text, int size ) const
	{
		GetDlgItemText( handle(), buttonId, text, size );
	}
	void setEditText( int editId, const char *text ) const
	{
		setButtonLabel( editId, text );
	}
	void getEditText( int buttonId, char *text, int size ) const
	{
		getButtonLabel( buttonId, text, size );
	}
	STRING getEditText( int buttonId );

	void setStaticText( int editId, const char *text ) const
	{
		setButtonLabel( editId, text );
	}
	void getStaticText( int buttonId, char *text, int size ) const
	{
		getButtonLabel( buttonId, text, size );
	}

	// check mark
	void setButtonCheck( int buttonId )
	{
		SendMessage( GetDlgItem( handle(), buttonId ), BM_SETCHECK,
					 true, 0L );
	}
	void clrButtonCheck( int buttonId )
	{
		SendMessage( GetDlgItem( handle(), buttonId ), BM_SETCHECK,
					 false, 0L );
	}
	bool isButtonChecked( int buttonId )
	{
		return SendMessage( GetDlgItem( handle(), buttonId ), BM_GETCHECK, true, 0L ) ? true : false;
	}

	// enable/disable
	void showButton( int buttonId )
	{
		ShowWindow( GetDlgItem( handle(), buttonId ), SW_SHOW );
	}
	void disableButton( int buttonId )
	{
		EnableWindow( GetDlgItem( handle(), buttonId ), false );
	}
	void disableText( int editId )
	{
		disableButton( editId );
	}

	// the message handlers
	virtual LRESULT doDefault( UINT msg, WPARAM wParam, LPARAM lParam );
	virtual ProcessStatus handleOk();
	virtual ProcessStatus handleCancel();
	virtual SuccessCode handleClose();
	virtual ProcessStatus handleButtonClick( int cmd );
	virtual ProcessStatus handleCommand( int cmd );
	virtual ProcessStatus handleCreate();
};

class ModalDialog : public DialogWindow
{
	int m_modalResult;

	public:
	ModalDialog(BasicWindow *owner=NULL) : DialogWindow( owner ) , m_modalResult(IDCANCEL) { }
	int create( const char *dialog, const BasicWindow *parent )
	{
		int retVal;

		retVal = int(
			DialogBoxParam(
				Application::getResInstance(), (LPSTR)dialog,
				parent ? parent->handle() : 0,
				(DLGPROC)WindowProc,
				LPARAM(this)
			)
		);
		clrHandle();
		return retVal;
	}

	int create( int dialog, const BasicWindow *parent )
	{
		return create( (const char *)MAKEINTRESOURCE(dialog), parent );
	}

	virtual SuccessCode close();
	SuccessCode close( int result )
	{
		m_modalResult = result;
		return close();
	}

	protected:
	virtual ProcessStatus handleCommand( int cmd );
};


class NonModalDialog : public DialogWindow
{
	public:
	NonModalDialog( BasicWindow *owner )
		: DialogWindow( owner ) 
	{
		allowDialogProcessing();
	}
	SuccessCode create( const char *dialog, const BasicWindow *parent=NULL )
	{
		HWND	handle;

		setParent( parent );
		handle = CreateDialogParam(
			Application::getResInstance(), (LPSTR)dialog,
			parent ? parent->handle() : 0,
			(DLGPROC)WindowProc,
			LPARAM(this)
		);
		if( handle )
		{
			setHandle( handle );
			return scSUCCESS;
		}
		else
		{
			return scERROR;
		}
	}

	SuccessCode create( int dialog, const BasicWindow *parent=NULL )
	{
		return create( (const char *)MAKEINTRESOURCE(dialog), parent );
	}
};

class ApplicationDialog : public NonModalDialog
{
	public:
	ApplicationDialog() : NonModalDialog( NULL ) {}
	SuccessCode create( const char *dialog )
	{
		return NonModalDialog::create( dialog, NULL );
	}

	SuccessCode create( int dialog )
	{
		return NonModalDialog::create( dialog, NULL );
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
