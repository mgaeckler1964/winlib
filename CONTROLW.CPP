/*
		Project:		Windows Class Library
		Module: 		CONTROLW.CPP
		Description:	Control Windows
						The "Control Window Class" describes a window, which is
						a windows defined control e.g. button
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

#include <stdio.h>
#include <string.h>

#include <gak/string.h>
#include <gak/textReader.h>
#include <gak/strFiles.h>
#include <gak/stdlib.h>

#include <winlib/controlw.h>
#include <winlib/device.h>

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
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static UnorderedSet<ControlWindow*>	subclassedControls;

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

const char Label::className[] = "Label";
const char Button::className[] = "Button";
const char PushButton::className[] = "PushButton";
const char RadioButton::className[] = "RadioButton";
const char CheckBox::className[] = "CheckBox";
const char GroupBox::className[] = "GroupBox";
const char EDIT::className[] = "EDIT";
const char MemoControl::className[] = "MemoControl";
const char EditControl::className[] = "EditControl";
const char ListBox::className[] = "ListBox";
const char ComboBox::className[] = "ComboBox";
const char ScrollBar::className[] = "ScrollBar";
const char TrackBar::className[] = "TrackBar";
const char DateTimePicker::className[] = "DateTimePicker";
const char UpDownButton::className[] = "UpDownButton";
const char TreeView::className[] = "TreeView";

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

LRESULT CALLBACK ControlWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// --------------------------------------------------------------------- //
// ----- class private ------------------------------------------------- //
// --------------------------------------------------------------------- //

/*
--------------------------------------------------------------------------------
	TreeNode
--------------------------------------------------------------------------------
*/
TreeNode *TreeNode::findItem( HTREEITEM itemHandle ) const
{
	if( itemHandle == handle )
		return const_cast<TreeNode*>(this);

	for( size_t i=0; i<children.size(); i++ )
	{
		TreeNode *result = children[i]->findItem( itemHandle );
		if( result )
			return result;
	}

	return NULL;
}

TreeNode *TreeNode::findItem( void *data ) const
{
	if( this->data == data )
		return const_cast<TreeNode*>(this);

	for( size_t i=0; i<children.size(); i++ )
	{
		TreeNode *result = children[i]->findItem( data );
		if( result )
			return result;
	}

	return NULL;
}

/*
--------------------------------------------------------------------------------
	Button
--------------------------------------------------------------------------------
*/
void Button::clearSiblingsFlag( unsigned int msg ) const
{
	unsigned long style = getStyle();
	if( (style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON )
	{
		if( !(style & WS_GROUP) )
		{
			for( 
				HWND sibling = GetNextWindow( handle(), GW_HWNDPREV );
				sibling != 0;
				sibling = GetNextWindow( sibling, GW_HWNDPREV )
			)
			{
				unsigned long style = GetWindowLong( sibling, GWL_STYLE );
				if( (style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON )
					SendMessage( sibling, msg, 0, 0 );
				if( style & WS_GROUP )
/*v*/				break;
			}
		}
		for( 
			HWND sibling = GetNextWindow( handle(), GW_HWNDNEXT );
			sibling != 0;
			sibling = GetNextWindow( sibling, GW_HWNDNEXT )
		)
		{
			unsigned long style = GetWindowLong( sibling, GWL_STYLE );
			if( style & WS_GROUP )
/*v*/			break;
			if( (style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON )
				SendMessage( sibling, msg, 0, 0 );
		}
	}
}



// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

void ControlWindow::subclassControl( void )
{
	if( !m_wpOrigControlProc )
	{
#if defined( __WIN32__ )
		WNDPROC oldProc = (WNDPROC)SetWindowLong(
			handle(),
			GWL_WNDPROC, (LONG)ControlWindowProc
		);
#elif defined( __WIN64__ )
		WNDPROC oldProc = (WNDPROC)SetWindowLongPtr(
			handle(),
			GWLP_WNDPROC, (LONG_PTR)ControlWindowProc
		);
#else
		#error "What Windows?"
#endif
		subclassedControls.addElement( this );
		m_wpOrigControlProc = oldProc;
	}
	m_subclassCounter++;
}

void ControlWindow::restoreProc( void )
{
	if( --m_subclassCounter && m_wpOrigControlProc )
	{
#if defined( __WIN32__ )
		(WNDPROC)SetWindowLong(
			handle(),
			GWL_WNDPROC, (LONG)m_wpOrigControlProc
		);
#elif defined( __WIN64__ )
		(WNDPROC)SetWindowLongPtr(
			handle(),
			GWLP_WNDPROC, (LONG_PTR)m_wpOrigControlProc
		);
#else
	#error "What Windows?"
#endif
		subclassedControls.removeElement( this );
	}
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

SuccessCode ControlWindow::close( void )
{
	SuccessCode result = scSUCCESS;
	if( handle() )
	{
		result = BasicWindow::close();

		BasicWindow *parent = const_cast<BasicWindow*>(getParent());
		if( parent )
		{
			parent->handleChildClose( this, false );
		}
	}
	return result;
}

STRING Button::getWindowClassName( void ) const
{
	return className;	// "BUTTON";
}

STRING TrackBar::getWindowClassName( void ) const
{
	return TRACKBAR_CLASS;	// "msctls_trackbar32";
}

STRING DateTimePicker::getWindowClassName( void ) const
{
	return DATETIMEPICK_CLASS;	// "SysDateTimePick32";
}

STRING UpDownButton::getWindowClassName( void ) const
{
	return UPDOWN_CLASS;	// "msctls_updown32";
}

STRING EDIT::getWindowClassName( void ) const
{
	return className;	// "EDIT";
}

STRING ComboBox::getWindowClassName( void ) const
{
	return className;	// "COMBOBOX";
}

STRING ListBox::getWindowClassName( void ) const
{
	return className;	// "LISTBOX";
}

STRING Label::getWindowClassName( void ) const
{
	return "STATIC";
}

STRING ScrollBar::getWindowClassName( void ) const
{
	return className;	// "scrollbar";
}

STRING TreeView::getWindowClassName( void ) const
{
	return WC_TREEVIEW; // "SysTreeView";
}



LRESULT ControlWindow::callback( UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	if( m_popupMenu && uMsg == WM_RBUTTONDOWN )
	{
		POINT p;
		GetCursorPos(&p);
		int id = TrackPopupMenu(m_popupMenu->getMenu(), TPM_LEFTALIGN|TPM_VCENTERALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, p.x, p.y, 0, handle(), NULL );
		doLogValue(id);
/***/	return getParent()->message(WM_COMMAND, id);
	}
	return CallWindowProc(m_wpOrigControlProc, handle(), uMsg, wParam, lParam);
}

LRESULT EDIT::callback( UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	if( uMsg == WM_CHAR )
	{
		if( wParam == '\r' || wParam == '\n' )
		{
			long start;

			message( EM_GETSEL, (WPARAM)&start, NULL );
			char *text = new char[start+1];
			if( text )
			{
				STRING	indentText;

				GetWindowText( handle(), text, start+1 );
				char *cp = text+start;
				char *endPos = cp;
				while( cp >= text && *cp != '\n' && *cp != '\r' )
					cp--;

				if( cp >= text )
				{
					if( *cp == '\r' || *cp == '\n' )
						cp++;

					while( *cp && *cp != '\r' && *cp != '\n' && isspace( *cp ) && cp < endPos )
						indentText += *cp++;
				}

				delete [] text;

				ControlWindow::callback( uMsg, wParam, lParam );
				for( size_t i=0; i< strlen( indentText ); i++ )
					ControlWindow::callback(
						uMsg, indentText[i],
						lParam
					);
			}
		}
		else
			ControlWindow::callback( uMsg, wParam, lParam );

		return 0;
	}
	else
		return ControlWindow::callback( uMsg, wParam, lParam );
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

/*
--------------------------------------------------------------------------------
	ComboBox
--------------------------------------------------------------------------------
*/

STRING ComboBox::getSelectedText( void ) const
{
	STRING	text;
	int	selectedIndex = getSelection();
	if( selectedIndex >= 0 )
	{
		int textLen = getSelectedTextLen( selectedIndex );
		char *cp = (char *)malloc( textLen +2 );
		if( cp )
		{
			message( CB_GETLBTEXT, selectedIndex, (long)cp );
			text = cp;
			free( cp );
		}
	}

	return text;
}

/*
--------------------------------------------------------------------------------
	ListBox
--------------------------------------------------------------------------------
*/

int ListBox::getSelection( void ) const
{
	int index;

	if( isMultipleSelection() )
	{
		ArrayOfInts	selected;
		size_t selCount = getSelectedItems( &selected );
		if( selCount )
			index = selected[0];
		else
			index = -1;
	}
	else
		index = int(message( LB_GETCURSEL, 0, 0L ));

	return index;
}

size_t	ListBox::getNumVisible( void ) const
{
	int itemHeigth = message( LB_GETITEMHEIGHT );
	int myHeight = getSize().height;
	return size_t(myHeight/itemHeigth);
}

void ListBox::setVisible( size_t item ) const
{
	size_t first = getFirstVisible();
	if( item < first )
	{
		setFirstVisible(item);
	}
	else
	{
		size_t numVisible = getNumVisible();
		if( item > first+numVisible )
		{
			setFirstVisible(item-numVisible);
		}
	}
}

void ListBox::setItems( T_STRING items )
{
	STRING	selItem;

	message( LB_RESETCONTENT );
	selItem = items.getFirstToken( "\n" );
	while( 1 )
	{
		if( !selItem[0U] )
/*v*/		break;
		addEntry( (const char *)selItem );
		selItem = items.getNextToken();
	}
}

void ListBox::clearSelection( void )
{
	if( isMultipleSelection() )
	{
		size_t	numEntries = getNumEntries();
		message( LB_SELITEMRANGE, false, MAKELPARAM( 0, numEntries ) );
	}
	else
	{
		message( LB_SETCURSEL, WPARAM(-1) );
	}
}

void ListBox::setSelectedItems( T_STRING items )
{
	clearSelection();
	STRING	selItem = items.getFirstToken( "\n" );

	while( 1 )
	{
		if( !selItem[0U] )
/*v*/		break;
		selectEntry( (const char *)selItem );
		selItem = items.getNextToken();
	}
}

void ListBox::setSelectedItems( const ArrayOfInts &selected )
{
	clearSelection();

	if( isMultipleSelection() )
	{
		for( size_t i=0; i<selected.size(); i++ )
		{
			message( LB_SETSEL, true, MAKELPARAM( selected[i], 0 ) );
		}
	}
	else if( selected.size() )
		message( LB_SETCURSEL, selected[0] );
}

STRING ListBox::getSelectedItems( void ) const
{
	STRING		selectedItems;
	ArrayOfInts	selected;
	size_t		selCount = getSelectedItems( &selected );

	if( selCount )
	{
		gak::Buffer<char>	tmpBuff(NULL);
		size_t				oldSize = 0;

		for( size_t i = 0; i<selCount; i++ )
		{
			size_t newSize = message( LB_GETTEXTLEN, selected[i] );
			if( newSize > oldSize )
			{
				tmpBuff.resize(newSize+1);
				if( tmpBuff )
					oldSize = newSize;
			}

			if( tmpBuff )
			{
				message( LB_GETTEXT, selected[i], LPARAM(static_cast<char*>(tmpBuff)) );
				if( !selectedItems.isEmpty() )
					selectedItems += '\n';
				selectedItems += static_cast<const char*>(tmpBuff);
			}
		}
	}

	return selectedItems;
}

size_t	ListBox::getSelectedItems( ArrayOfInts *selected ) const
{
	size_t	selCount = getNumSelected();

	if( selCount )
	{
		selected->clear();
		int	*selItems = selected->createElements( selCount );

		if( selItems )
		{
			if( isMultipleSelection() )
				message( LB_GETSELITEMS, selCount, LPARAM(selItems) );
			else
				*selItems = int(message( LB_GETCURSEL, 0, 0L ));
		}
	}

	return selCount;
}

size_t ListBox::getNumSelected( void ) const
{
	if( isMultipleSelection() )
		return size_t(message( LB_GETSELCOUNT, 0, 0 ) );
	else if( getSelection() >= 0 )
		return 1;
	else
		return 0;
}

void ListBox::selectEntry( int entryId )
{
	if( isMultipleSelection() )
		message( LB_SETSEL, true, MAKELPARAM( entryId, 0 ) );
	else
		message( LB_SETCURSEL, entryId );
}

void ListBox::unselectEntry( int entryId )
{
	if( isMultipleSelection() )
		message( LB_SETSEL, false, MAKELPARAM( entryId, 0 ) );
	else
		message( LB_SETCURSEL, WPARAM(-1) );
}

/*
--------------------------------------------------------------------------------
	EDIT
--------------------------------------------------------------------------------
*/
void EDIT::setCharset( enum editCharset newCharset )
{
	if( newCharset != charSet )
	{
		STRING	text = *this;

		if( newCharset == CS_OEM )
			addStyle( ES_OEMCONVERT );
		else if( charSet == CS_OEM )
			removeStyle( ES_OEMCONVERT );

		if( newCharset == CS_UTF8 )
			text = text.decodeUTF8();
		else if( charSet == CS_UTF8 )
			text = text.encodeUTF8();

		charSet = newCharset;

		*this = text;
	}
}

void EDIT::convertCharset( enum editCharset newCharset )
{
	if( newCharset != charSet )
	{
		if( newCharset == CS_OEM )
			addStyle( ES_OEMCONVERT );
		else if( charSet == CS_OEM )
			removeStyle( ES_OEMCONVERT );

		charSet = newCharset;
		setChanged();
	}
}

void EDIT::getSelection( size_t *start, size_t *end ) const
{
	DWORD	startDword, endDword;
	message( EM_GETSEL, reinterpret_cast<WPARAM>(&startDword), reinterpret_cast<LPARAM>(&endDword) );
	if( start )
	{
		*start = startDword;
	}
	if( end )
	{
		*end = endDword;
	}
}

STRING EDIT::getSelection( void ) const
{
	STRING	selection;
	size_t	start, end;

	getSelection( &start, &end );

	if( end > start )
	{
		STRING text = getText( isOemConvert() );
		selection = text.subString( start, end-start );
	}

	return selection;
}

void EDIT::getCursorPosition( std::size_t *oLine, std::size_t *oColum ) const
{
	std::size_t	charIndex;

	getSelection( &charIndex );

	std::size_t line = message( EM_LINEFROMCHAR, WPARAM(charIndex) );
	std::size_t colum = charIndex - message( EM_LINEINDEX, WPARAM(line) );
	*oLine = ++line;
	*oColum = ++colum;
}

void EDIT::setSelection( std::size_t start, std::size_t end )
{
	message( EM_SETSEL, start, end );
	message( EM_SCROLLCARET );
	focus();
}

void EDIT::indent( void )
{
	STRING	theText;
	size_t	startPos, endPos, newEndPos;

	theText = getString();
	getSelection( &startPos, &endPos );
	newEndPos = endPos;

	// walk from end to start of block
	while( endPos > startPos )
	{
		if( theText[endPos-1] == '\n' )
		{
			theText.insChar( endPos, '\t' );
			newEndPos++;
		}
		endPos--;
	}

	// find start of first line
	if( startPos > 0 && theText[startPos-1] != '\n' )
		do
		{
			startPos--;
		}while( startPos > 0 && theText[startPos-1] != '\n' );

	theText.insChar( startPos, '\t' );
	newEndPos++;

	setText( theText );

	setSelection( startPos, newEndPos );
	setChanged();
}

void EDIT::unindent( void )
{
	STRING	theText;
	int		i, tabWidth = getTabWidth();
	size_t	startPos, endPos, newEndPos;

	theText = getString();
	getSelection( &startPos, &endPos );
	newEndPos = endPos;

	// walk from end to start of block
	while( endPos > startPos )
	{
		if( theText[endPos-1] == '\n' )
		{
			for( i=0; i<tabWidth; i++ )
			{
				if( theText[endPos] == '\t' )
				{
					theText.delChar( endPos );
					newEndPos--;
/*v*/				break;
				}
				if( theText[endPos] == ' ' )
				{
					theText.delChar( endPos );
					newEndPos--;
				}
				else
/*v*/				break;
			}
		}
		endPos--;
	}

	// find start of first line
	if( startPos > 0 && theText[startPos-1] != '\n' )
		do
		{
			startPos--;
		}while( startPos > 0 && theText[startPos-1] != '\n' );

	for( i=0; i<tabWidth; i++ )
	{
		if( theText[startPos] == '\t' )
		{
			theText.delChar( startPos );
			newEndPos--;
/*v*/		break;
		}
		if( theText[startPos] == ' ' )
		{
			theText.delChar( startPos );
			newEndPos--;
		}
		else
/*v*/		break;
	}

	if( newEndPos < startPos )
		newEndPos = startPos;

	setText( theText );
	setSelection( startPos, newEndPos );
	setChanged();
}

void EDIT::readFromFile( const char *fileName )
{
	STRING			file;

	if( !strAccess( fileName, 0 ) )
	{
		file.readFromFile( fileName );
		lineEnd = file.getLineEnds();

		if( lineEnd != RL_END_CRLF )
			file.replaceLineEnds(RL_END_CRLF);

		if( charSet == CS_UTF8 )
			file = file.decodeUTF8(  );
		else if( isOemConvert() )
			file.setCharSet( STR_OEM );

		setText( file );
		message( EM_SETMODIFY, false, 0l );
	}
}

bool EDIT::saveToFile( const char *fileName )
{
	STRING	text;
	bool	errorFlag;

	text = getText( isOemConvert() );
	if( lineEnd != RL_END_CRLF )
		text.replaceLineEnds( lineEnd );

	if( charSet == CS_UTF8 )
		text = text.encodeUTF8();

	errorFlag = text.writeToFile( fileName );
	if( !errorFlag )
		message( EM_SETMODIFY, false, 0l );

	return errorFlag;
}

size_t EDIT::searchText(
	const char	*text,
	size_t		startPos,
	bool		wholeWord,
	bool		matchCase,
	bool		downSearch,
	const char	*replaceText,
	bool		all
)
{
	doEnterFunction( "EDIT::searchText" );

	size_t	start, end, findPos;
	STRING	str;

	str = getText( isOemConvert() );

	if( startPos == BEGIN_SELECTION
	||  startPos == END_SELECTION )
	{
		getSelection( &start, &end );
		if( startPos == BEGIN_SELECTION )
			startPos = start;
		else
			startPos = end;
	}

	findPos = str.searchText( text, startPos, wholeWord, matchCase, downSearch );
	if( findPos != str.no_index )
	{
		setSelection( findPos, findPos + strlen( text ) );
		if( replaceText )
		{
			if( !all )
			{
				replaceSelection( replaceText );
				setSelection( findPos, + findPos+strlen( replaceText ) );
			}
			else
			{
				size_t newLen = strlen( replaceText );

				do
				{
					str.replaceText( findPos, strlen( text ), replaceText );
					findPos = str.searchText( text,
						findPos+newLen, wholeWord, matchCase, downSearch
					);
				} while( findPos != (size_t)-1 );

				setText( str );
				message( EM_SETMODIFY, true, 0l );
			}
		}

		focus();
	}
	else
	{
		MessageBeep( MB_ICONASTERISK );
	}
	return findPos;
}

void EDIT::print( const char *fileName, PrintDevice *printer )
{
	STRING	cp;

	cp = getText();
	printer->setTabWidth( tabWidth );
	printer->printText( fileName, cp );
}

/*
--------------------------------------------------------------------------------
	TrackBar
--------------------------------------------------------------------------------
*/

void TrackBar::setTics( const short *position, size_t numTicks )
{
	for( size_t i=0; i<numTicks; i++ )
		setTic( *position++ );
}

/*
--------------------------------------------------------------------------------
	TreeView
--------------------------------------------------------------------------------
*/

TreeNode *TreeView::moveItem( TreeNode *item, TreeNode *parent, size_t index )
{
	if( item->getParent() == parent && item->getIndex() < index )
		index++;

	TreeNode *newItem = addItem( parent, item->getLabel(), item->getData(), index );
	size_t		numChildren = item->getNumChildren();
	for( size_t i=0; i<numChildren; i++ )
	{
		moveItem( item->getFirstChild(), newItem, i );
	}
	deleteItem( item );

	return newItem;
}

/*
--------------------------------------------------------------------------------
	DateTimePicker
--------------------------------------------------------------------------------
*/
long DateTimePicker::setStopWatch(const gak::StopWatch &watch)
{
	SYSTEMTIME systime;
	memset( &systime, 0, sizeof(systime));
	gak::Seconds<> timer = watch.get< gak::Seconds<> >();
	systime.wDay = 1;
	systime.wMonth = 1;
	systime.wYear = 1970;
	systime.wHour = WORD(timer.hoursPart());
	systime.wMinute = WORD(timer.minutesPart());
	systime.wSecond = WORD(timer.secondsPart());

	return setSystemTime(systime);; 
}

long DateTimePicker::setTimeT( time_t seconds )
{
	gak::DateTime dt(seconds);
	SYSTEMTIME systime;
	memset( &systime, 0, sizeof(systime));
	systime.wDay = dt.getDay();
	systime.wMonth = WORD(dt.getMonth());
	systime.wYear = dt.getYear();
	systime.wHour = dt.getHour();
	systime.wMinute = dt.getMinute();
	systime.wSecond = dt.getSecond();

	return setSystemTime(systime);; 
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#define WORD_DELIMITERS		" \t\n\r+-*/=<>%(){}[].,:;?!\\|§$&#@\'\""

int CALLBACK WordBreakProc( LPSTR text, int cursPos, int length, int action )
{
	doEnterFunction( "WordBreakProc" );

	static int lastAction = -1;

	switch( action )
	{
	case WB_ISDELIMITER:
		if( strchr( WORD_DELIMITERS, text[cursPos] ) )
			cursPos = 1;
		else
			cursPos = 0;
		break;

	case WB_LEFT:
		if( lastAction != WB_ISDELIMITER )
		{
			while( cursPos > 0 )
			{
				cursPos--;
				if( !strchr( WORD_DELIMITERS, text[cursPos] ) )
					break;
			}
			while( cursPos >= 0 )
			{
				if( strchr( WORD_DELIMITERS, text[cursPos] ) )
					break;
				cursPos--;
			}
			cursPos++;
		}
		break;

	case WB_RIGHT:
		if( strchr( WORD_DELIMITERS, text[cursPos] ) )
			while( cursPos < length )
			{
				if( !strchr( WORD_DELIMITERS, text[cursPos] ) )
					break;
				cursPos++;
			}
		else
			while( cursPos < length )
			{
				if( strchr( WORD_DELIMITERS, text[cursPos] ) )
					break;
				cursPos++;
			}

		break;
	default:
		break;
	}
	lastAction = action;

	return cursPos;
}

LRESULT CALLBACK ControlWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	for( size_t i=0; i<subclassedControls.size(); i++ )
	{
		ControlWindow	*control = subclassedControls[i];
		if( control->handle() == hwnd )
			return control->callback( uMsg, wParam, lParam );
	}

	return 0;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


