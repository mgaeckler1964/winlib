/*
		Project:		Windows Class Library
		Module: 		STDDLG.CPP
		Description:	Implementation of standard dialogs
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

FileDialog::FileDialog() : BasicWindow( FileDialogFlag, NULL )
{
	fileBuffer = (char*)malloc( 10240 );
	*fileBuffer = 0;

	memset( &ofn,0, sizeof( OPENFILENAME ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.nFilterIndex = 1;
	ofn.lpstrFile	 = fileBuffer;
	ofn.nMaxFile	 = 10239;
}

FileDialog::~FileDialog()
{
	free( fileBuffer );
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
	memset( &findInfo, 0, sizeof( findInfo ) );
	findInfo.lStructSize = sizeof( findInfo );
	findText[0] = 0;
	findInfo.lpstrFindWhat = findText;
	findInfo.wFindWhatLen = sizeof( findText );
	findInfo.Flags = FR_DOWN;
}

ReplaceDialog::ReplaceDialog()
{
	replaceText[0] = 0;
	findInfo.lpstrReplaceWith = replaceText;
	findInfo.wReplaceWithLen = sizeof( replaceText );
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
	cc.hwndOwner	= parent->handle();
	cc.rgbResult	= *value;
	cc.lpCustColors = palette;

	cc.Flags		= CC_RGBINIT;

	if( ChooseColor( &cc ) )
	{
		*value = cc.rgbResult;
		return true;
	}
	return false;
}

bool ColorDialog::createWithPalette( 
	const BasicWindow *parent, COLORREF *value,
	const char *paletteName 
)
{
	int 	i;
	char	keyName[256],
			color[256];
	STRING	colorStr;
	size_t	nameLen;

	strcpy( keyName, paletteName ? paletteName : "colorEntry" );
	nameLen = strlen( keyName );

	for( i=0; i<16; i++ )
	{
		itoa( i, keyName + nameLen, 10 );
		colorStr = appObject->GetProfile( NULL, keyName, "16777215" );
		palette[i] = atol( color );
	}
	if( create( parent, value ) )
	{
		for( i=0; i<16; i++ )
		{
			itoa( i, keyName + nameLen, 10 );
			ultoa( palette[i], color, 10 );

			appObject->WriteProfile( false, NULL, keyName, color );
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
		strncpy( findText, text, sizeof(findText) );
		findText[sizeof(findText)-1] = 0;
	}
	if( wholeWord )
		flags |= FR_WHOLEWORD;
	if( matchCase )
		flags |= FR_MATCHCASE;
	if( downSearch )
		flags |= FR_DOWN;

	findInfo.Flags = flags;
	findInfo.hwndOwner = parent->handle();
	parent->registerSearchDialog( this );
	setHandle( FindText( &findInfo ) );

	return false;
}

bool ReplaceDialog::create( const CallbackWindow *parent, const char*newFindText, const char *newReplaceText, bool wholeWord, bool matchCase )
{
	int	flags = 0;

	if( newFindText )
	{
		strncpy( findText, newFindText, sizeof(findText) );
		findText[sizeof(findText)-1] = 0;
	}
	if( newReplaceText )
	{
		strncpy( replaceText, newReplaceText, sizeof(replaceText) );
		replaceText[sizeof(replaceText)-1] = 0;
	}
	if( wholeWord )
		flags |= FR_WHOLEWORD;
	if( matchCase )
		flags |= FR_MATCHCASE;

	findInfo.Flags = flags;
	findInfo.hwndOwner = parent->handle();
	parent->registerSearchDialog( this );
	setHandle( ReplaceText( &findInfo ) );

	return false;
}

bool PrinterDialog::create( const BasicWindow *parent )
{
	printDlg.hwndOwner = parent->handle();
	printDlg.Flags = PD_ALLPAGES|PD_RETURNDC;

	return (bool)PrintDlg( &printDlg );
}

bool FontDialog::create( const BasicWindow *parent )
{
	fontDlg.hwndOwner = parent->handle();
	if( !(fontDlg.Flags & CF_BOTH) )
		fontDlg.Flags |= CF_BOTH;
	fontDlg.Flags |= CF_INITTOLOGFONTSTRUCT;

	return (bool)ChooseFont( &fontDlg );
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


