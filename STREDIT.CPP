/*
		Project:		Windows Class Library
		Module: 		STREDIT.CPP
		Description:	A popup window to edit one string
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

#include <winlib\stredit.h>

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

STRING StringEditor::create( BasicWindow *parent, const char *title, const char *string, bool singleLine )
{
	int		labelWidth = int(strlen( title ) * 10);
	int		controlHeight = singleLine ? 20 : 200;
	theValue = string;

	setText( title );
	if( ModalPopup::create( parent, labelWidth+430, controlHeight + 70 ) == scSUCCESS )
	{
		label.create( this, 8, 8, labelWidth, 20 );
		label.setText( title );

		if( singleLine )
		{
			EditControl	*newControl = new EditControl( this );
			newControl->create( this, labelWidth+16, 8, 400, controlHeight );
			theStringEdit = newControl;
		}
		else
		{
			MemoControl	*newControl = new MemoControl( this );
			newControl->create( this, labelWidth+16, 8, 400, controlHeight );
			theStringEdit = newControl;
		}

		theStringEdit->setText( string );

		okButton.create( this, 8, controlHeight+16, 50, 20 );
		okButton.setText( "OK" );
		okButton.setId( IDOK );

		cancelButton.create( this, 66, controlHeight+16, 50, 20 );
		cancelButton.setText( "Cancel" );
		cancelButton.setId( IDCANCEL );

		mainLoop();
	}

	return theValue;
}

ProcessStatus StringEditor::handleOk( void )
{
	theValue = theStringEdit->getString();
	return ModalPopup::handleOk();
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

