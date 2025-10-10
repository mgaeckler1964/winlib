/*
		Project:		Windows Class Library
		Module: 		STREDIT.CPP
		Description:	A popup window to edit one string
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
	m_value = string;

	setText( title );
	if( ModalPopup::create( parent, labelWidth+430, controlHeight + 70 ) == scSUCCESS )
	{
		m_label.create( this, 8, 8, labelWidth, 20 );
		m_label.setText( title );

		if( singleLine )
		{
			EditControl	*newControl = new EditControl( this );
			newControl->create( this, labelWidth+16, 8, 400, controlHeight );
			m_theStringEdit = newControl;
		}
		else
		{
			MemoControl	*newControl = new MemoControl( this );
			newControl->create( this, labelWidth+16, 8, 400, controlHeight );
			m_theStringEdit = newControl;
		}

		m_theStringEdit->setText( string );

		m_okButton.create( this, 8, controlHeight+16, 50, 20 );
		m_okButton.setText( "OK" );
		m_okButton.setId( IDOK );

		m_cancelButton.create( this, 66, controlHeight+16, 50, 20 );
		m_cancelButton.setText( "Cancel" );
		m_cancelButton.setId( IDCANCEL );

		mainLoop();
	}

	return m_value;
}

ProcessStatus StringEditor::handleOk()
{
	m_value = m_theStringEdit->getString();
	return ModalPopup::handleOk();
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

