/*
		Project:		Windows Class Library
		Module:			DIALOGWI.CPP
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif


// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib\dialogwi.h>
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

// --------------------------------------------------------------------- //
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus DialogWindow::handleOk()
{
	return psDO_DEFAULT;
}

ProcessStatus DialogWindow::handleCancel()
{
	return psDO_DEFAULT;
}

SuccessCode DialogWindow::handleClose()
{
	// windows sends WM_COMMAND with IDCANCEL which will close the form
	// therefore we do not need the standard implementation here
	return scERROR;
}

ProcessStatus DialogWindow::handleButtonClick( int cmd )
{
	if( cmd >= IDOK	&& cmd <= IDNO )
	{
		if( cmd == IDOK && handleOk() == psPROCESSED )
			return psPROCESSED;								// continue dialog

		else if( cmd == IDCANCEL && handleCancel() == psPROCESSED )
			return psPROCESSED;								// continue dialog

		else if( !canClose() )
			return psPROCESSED;

		close();
		return psPROCESSED;
	}
	else
	{
		return CallbackWindow::handleButtonClick( cmd );
	}
}

ProcessStatus DialogWindow::handleCommand( int cmd )
{
	ProcessStatus status = handleButtonClick( cmd );
	if( status == psDO_DEFAULT )
	{
		status = CallbackWindow::handleCommand(cmd);
	}
	return status;
}

ProcessStatus DialogWindow::handleCreate()
{
	CI_STRING	childClass;
	for( ForeignWindow child = GetWindow( handle(), GW_CHILD ); child; child=GetWindow( child.handle(), GW_HWNDNEXT ) )
	{
		childClass = child.getWindowClassName();
		if( childClass == "BUTTON" )
		{
			bool			needImage = false;
			unsigned long style = child.getStyle();
			if( style & BS_ICON )
			{
				if( !child.message( BM_GETIMAGE, IMAGE_ICON ) )
					needImage = true;
			}
			else if( style & BS_BITMAP )
			{
				if( !child.message( BM_GETIMAGE, IMAGE_BITMAP ) )
					needImage = true;
			}

			if( needImage )
			{
				Button *newButton = new Button( this );
				newButton->initHandle( this, GetWindowLong( child.handle(), GWL_ID ) );
			}
		}
	}
	return CallbackWindow::handleCreate();
}

ProcessStatus ModalDialog::handleCommand( int cmd )
{
	m_modalResult = cmd;
	return DialogWindow::handleCommand( cmd );
}

LRESULT DialogWindow::doDefault( UINT, WPARAM, LPARAM )
{
	return false;
}

SuccessCode ModalDialog::close()
{
	return EndDialog( handle(), m_modalResult ) ? scSUCCESS : scERROR;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

STRING DialogWindow::getEditText( int buttonId )
{
	char	tmpBuff[1024];
	getEditText( buttonId, tmpBuff, sizeof( tmpBuff ) );
	STRING value( tmpBuff );

	return value;
}

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


