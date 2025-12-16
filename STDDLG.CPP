/*
		Project:		Windows Class Library
		Module: 		STDDLG.CPP
		Description:	Implementation of standard dialogs
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

#include <stdlib.h>

#include <winlib/stddlg.h>
#include <winlib/winapp.h>

#include <gak/array.h>

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

static Array<SaveFileAsDialog*>	saveDialogs;

#ifdef __WIN64__
UINT_PTR
#else
UINT 
#endif
APIENTRY OFNHookProc(
	HWND	hdlg,	// handle to child dialog window
	UINT	uiMsg,	// message identifier
	WPARAM	wParam,	// message parameter
	LPARAM	lParam 	// message parameter
)
{
	SaveFileAsDialog	*theDialog = NULL;
	if( uiMsg == WM_INITDIALOG )
	{
		OPENFILENAME	*ofn = (OPENFILENAME *)lParam;
		theDialog = (SaveFileAsDialog *)ofn->lCustData;
		theDialog->setHandle( hdlg );
	}
	else
	{
		for( size_t i =0; i<saveDialogs.size(); i++ )
		{
			theDialog = saveDialogs[i];
			if( theDialog->handle() == hdlg )
				break;
		}
	}
	if( theDialog && theDialog->handle() == hdlg )
		theDialog->handleMessage( uiMsg, wParam, lParam );

	return 0;
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

FileDialog::FileDialog() : BasicWindow( FileDialogFlag, NULL ), m_fileBuffer(10240)
{
	*m_fileBuffer = 0;

	memset( &m_ofn,0, sizeof( OPENFILENAME ) );
	m_ofn.lStructSize = sizeof( OPENFILENAME );
	m_ofn.nFilterIndex = 1;
	m_ofn.lpstrFile	 = m_fileBuffer;
	m_ofn.nMaxFile	 = 10239;
}

SaveFileAsDialog::SaveFileAsDialog()
{
	saveDialogs += this;
}

SaveFileAsDialog::~SaveFileAsDialog()
{
	saveDialogs.removeElementVal( this );
}

SearchDialog::SearchDialog() : BasicWindow( SearchDialogFlag, (BasicWindow*)NULL )
{
	memset( &m_findInfo, 0, sizeof( m_findInfo ) );
	m_findInfo.lStructSize = sizeof( m_findInfo );
	m_findText[0] = 0;
	m_findInfo.lpstrFindWhat = m_findText;
	m_findInfo.wFindWhatLen = sizeof( m_findText );
	m_findInfo.Flags = FR_DOWN;
}

ReplaceDialog::ReplaceDialog()
{
	m_replaceText[0] = 0;
	m_findInfo.lpstrReplaceWith = m_replaceText;
	m_findInfo.wReplaceWithLen = sizeof( m_replaceText );
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void SaveFileAsDialog::handleMessage(
	UINT uiMsg,	// message identifier
	WPARAM wParam,	// message parameter
	LPARAM  	// message parameter
)
{
	doEnterFunction("SaveFileAsDialog::handleMessage");
	if( uiMsg == WM_INITDIALOG )
	{
		filterList.initHandle( this, 1136 );
		filenameEdit.initHandle( this, 1136 );

	}
	else if( uiMsg == WM_COMMAND )
	{
		int myID = LOWORD( wParam );
		if( myID == 1136 )
		{
			int myFilter = filterList.getSelection();
			STRING myExtension = defaultExtensions[myFilter];
			STRING myName = filenameEdit.getText();
			int		dotPos = int(myName.searchRChar( '.' ));
			int		slashPos = int(myName.searchRChar( DIRECTORY_DELIMITER ));
			if( dotPos > slashPos )
			{
				CI_STRING	oldExt = myName.subString( dotPos+1 );
				if( oldExt != myExtension )
				{
					myName.cut( dotPos+1 );
					myName += myExtension;
					filenameEdit.setText( myName );
				}
			}
			else
			{
				myName += '.';
				myName += myExtension;
				filenameEdit.setText( myName );
			}
		}
	}
}

bool ColorDialog::create( 
	const BasicWindow *parent,
	COLORREF *value
)
{
	m_cc.hwndOwner	= parent->handle();
	m_cc.rgbResult	= *value;
	m_cc.lpCustColors = m_palette;

	m_cc.Flags		= CC_RGBINIT;

	if( ChooseColor( &m_cc ) )
	{
		*value = m_cc.rgbResult;
		return true;
	}
	return false;
}

bool ColorDialog::createWithPalette( 
	const BasicWindow *parent, COLORREF *value,
	const char *paletteName 
)
{
	STRING	keyName = paletteName ? paletteName : "colorEntry";
	const size_t nameLen = keyName.strlen();

	for( int i=0; i<NUM_COLORS; i++ )
	{
		keyName += formatNumber(i);
		STRING	colorStr = appObject->GetProfile( nullptr, keyName, "16777215" );
		m_palette[i] = colorStr.getValueN<COLORREF>();
		keyName.cut(nameLen);
	}
	if( create( parent, value ) )
	{
		for( int i=0; i<NUM_COLORS; i++ )
		{
			keyName += formatNumber(i);
			STRING	colorStr = formatNumber(m_palette[i]);

			appObject->WriteProfile( false, nullptr, keyName, colorStr );
			keyName.cut(nameLen);
		}

		return true;										// ok clicked
	}
	return false;											// cancel
}

bool SearchDialog::create( const CallbackWindow *parent, const char *text, bool wholeWord, bool matchCase, bool downSearch )
{
	int	flags = 0;

	if( text )
	{
		strncpy( m_findText, text, sizeof(m_findText) );
		m_findText[sizeof(m_findText)-1] = 0;
	}
	if( wholeWord )
		flags |= FR_WHOLEWORD;
	if( matchCase )
		flags |= FR_MATCHCASE;
	if( downSearch )
		flags |= FR_DOWN;

	m_findInfo.Flags = flags;
	m_findInfo.hwndOwner = parent->handle();
	parent->registerSearchDialog( this );
	setHandle( FindText( &m_findInfo ) );

	return false;
}

bool ReplaceDialog::create( const CallbackWindow *parent, const char*newFindText, const char *newReplaceText, bool wholeWord, bool matchCase )
{
	int	flags = 0;

	if( newFindText )
	{
		strncpy( m_findText, newFindText, sizeof(m_findText) );
		m_findText[sizeof(m_findText)-1] = 0;
	}
	if( newReplaceText )
	{
		strncpy( m_replaceText, newReplaceText, sizeof(m_replaceText) );
		m_replaceText[sizeof(m_replaceText)-1] = 0;
	}
	if( wholeWord )
		flags |= FR_WHOLEWORD;
	if( matchCase )
		flags |= FR_MATCHCASE;

	m_findInfo.Flags = flags;
	m_findInfo.hwndOwner = parent->handle();
	parent->registerSearchDialog( this );
	setHandle( ReplaceText( &m_findInfo ) );

	return false;
}

bool PrinterDialog::create( const BasicWindow *parent )
{
	m_printDlg.hwndOwner = parent->handle();
	m_printDlg.Flags = PD_ALLPAGES|PD_RETURNDC;

	return bool(PrintDlg( &m_printDlg ));
}

bool FontDialog::create( const BasicWindow *parent )
{
	m_fontDlg.hwndOwner = parent->handle();
	if( !(m_fontDlg.Flags & CF_BOTH) )
		m_fontDlg.Flags |= CF_BOTH;
	m_fontDlg.Flags |= CF_INITTOLOGFONTSTRUCT;

	return bool(ChooseFont( &m_fontDlg ));
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


