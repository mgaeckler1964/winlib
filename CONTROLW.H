/*
		Project:		Windows Class Library
		Module: 		CONTROLW.H
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

#ifndef CONTROL_WIN_H
#define CONTROL_WIN_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif
// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/t_string.h>
#include <gak/array.h>
#include <gak/stopWatch.h>

#include <winlib/dialogwi.h>
#include <winlib/font.h>
#include <winlib/DEVICE.H>

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
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

#define BEGIN_SELECTION 	(size_t)-1
#define END_SELECTION		(size_t)-2

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

int CALLBACK WordBreakProc( LPSTR text, int cursPos, int length, int action );
class PrintDevice;
class PopupMenu;

class ControlWindow : public BasicWindow
{
	WNDPROC		m_wpOrigControlProc;
	size_t		m_subclassCounter;
	PopupMenu	*m_popupMenu;

	protected:
	ControlWindow( BasicWindow *owner=NULL ) : BasicWindow( ControlWindowFlag, owner )
	{
		m_wpOrigControlProc = NULL;
		m_subclassCounter = 0;
		m_popupMenu = NULL;
		setStyle( WS_CHILD|WS_VISIBLE|WS_TABSTOP );
	}
	void subclassControl( void );
	void restoreProc( void );

	public:
	virtual SuccessCode create( CallbackWindow *parent )
	{
		addStyle( WS_CHILD );
		SuccessCode error = BasicWindow::create( parent );
		if( error == scSUCCESS && parent )
		{
			Font &font = getFont();
			if( !font.isAssigned() )
			{
				message( WM_SETFONT, parent->message( WM_GETFONT ), true );
				font.readFont();
			}
			else
			{
				message( WM_SETFONT, WPARAM(HFONT(font)), true );
			}
		}
		if( m_subclassCounter && !m_wpOrigControlProc )
		{
			subclassControl();
			--m_subclassCounter;
		}
		return error;
	}
	SuccessCode create( CallbackWindow *parent, int x, int y, int w, int h )
	{
		sizeNmove( x, y, w, h );

		return create( parent );
	}
	virtual SuccessCode close( void );
	void initHandle( BasicWindow *parent, int controlId )
	{
		BasicWindow::initHandle( parent, controlId );
		parent->addChild( this );
	}
	virtual LRESULT callback( UINT uMsg, WPARAM wParam, LPARAM lParam  );
	void setPopupMenu( PopupMenu *menu )
	{
		subclassControl();
		m_popupMenu = menu;
	}
	void clrPopupMenu()
	{
		restoreProc();
		m_popupMenu = NULL;
	}
};

class Button : public ControlWindow
{
	static const char className[];

	Bitmap		bitmap;
	virtual STRING getWindowClassName( void ) const;

	private:
	void clearSiblingsFlag( unsigned int msg ) const;

	protected:
	friend class DialogWindow;
	Button( BasicWindow *owner = NULL ) : ControlWindow( owner ) {}

	public:
	bool isActive( void ) const
	{
		return message( BM_GETCHECK ) ? true : false;
	}
	void setActive( void ) const
	{
		message( BM_SETCHECK, BST_CHECKED );
		clearSiblingsFlag( BM_SETCHECK );
	}
	void setActive( bool active ) const
	{
		message( BM_SETCHECK, active ? BST_CHECKED : BST_UNCHECKED );
		if( active )
			clearSiblingsFlag( BM_SETCHECK );
	}
	void clrActive( void ) const
	{
		message( BM_SETCHECK, BST_UNCHECKED );
	}

	bool isHighlighted( void ) const
	{
		return message( BM_GETSTATE, 0, 0L ) ? true : false;
	}
	void setHighlight( void ) const
	{
		message( BM_SETSTATE, 1, 0 );
		clearSiblingsFlag( BM_SETSTATE );
	}
	void setHighlight( bool highlight ) const
	{
		message( BM_SETSTATE, highlight, 0 );
		if( highlight )
			clearSiblingsFlag( BM_SETSTATE );
	}
	void clrHighlight( void ) const
	{
		message( BM_SETSTATE, 0, 0 );
	}

	template <typename ResourceIdT>
	bool setBitmap( const ResourceIdT &resourceId )
	{
		bool			success = false;
		unsigned long	style = getStyle();

		bitmap = Application::loadBitmap( resourceId );
		if( bitmap )
		{
			if( !(style & BS_BITMAP) || (style & BS_ICON) )
			{
				removeStyle( BS_ICON );
				style = addStyle( BS_BITMAP );
				message( BM_SETSTYLE, style, FALSE ); 
			}
			message( BM_SETIMAGE, IMAGE_BITMAP, LPARAM(HBITMAP(bitmap)) );
			success = true;
		}
		else if( style & BS_BITMAP )
		{
			style = removeStyle( BS_BITMAP );
			message( BM_SETSTYLE, style, TRUE ); 
		}
		return success;
	}
	template <typename ResourceIdT>
	bool setIcon( ResourceIdT resourceId )
	{
		bool			success = false;
		unsigned long	style = getStyle();

		HICON icon = Application::loadIcon( resourceId );
		if( icon )
		{
			if( !(style & BS_ICON) || (style & BS_BITMAP) )
			{
				removeStyle( BS_BITMAP );
				style = addStyle( BS_ICON );
				message( BM_SETSTYLE, style, FALSE ); 
			}
			message( BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon );
			success = true;
		}
		else if( style & BS_ICON )
		{
			style = removeStyle( BS_ICON );
			message( BM_SETSTYLE, style, TRUE ); 
		}
		return success;
	}
	void initHandle( BasicWindow *parent, int controlId )
	{
		ControlWindow::initHandle( parent, controlId );
		if( hasStyle( BS_BITMAP ) )
			setBitmap( controlId );
		else if( hasStyle( BS_ICON ) )
			setIcon( controlId );
	};
	SuccessCode create( CallbackWindow *parent )
	{
		SuccessCode error = ControlWindow::create( parent );
		if( hasStyle( BS_BITMAP ) )
			setBitmap( getId() );
		else if ( hasStyle( BS_ICON ) )
			setIcon( getId() );

		return error;
	}
	SuccessCode create( CallbackWindow *parent, int x, int y, int w, int h )
	{
		sizeNmove( x, y, w, h );

		return create( parent );
	}
};

class PushButton : public Button
{
	public:
	static const char className[];
	PushButton( BasicWindow *owner=NULL ) : Button( owner ) {}
};

class CheckBox : public Button
{
	public:
	static const char className[];
	CheckBox( BasicWindow *owner = NULL ) : Button( owner )
	{
		addStyle( BS_AUTOCHECKBOX );
		setWidthOffset( 25 );
	}
};

class GroupBox : public Button
{
	public:
	static const char className[];
	GroupBox( BasicWindow *owner = NULL ) : Button( owner )
	{
		addStyle( BS_GROUPBOX );
	}
};

class RadioButton : public Button
{
	public:
	static const char className[];

	RadioButton( BasicWindow *owner = NULL ) : Button( owner )
	{
		addStyle( BS_AUTORADIOBUTTON );
		setWidthOffset( 25 );
	}
};

class ComboBox : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	int getSelectedTextLen( int index ) const
	{
		return int(message( CB_GETLBTEXTLEN, index ));
	}
	public:
	static const char className[];
	ComboBox( BasicWindow *owner = NULL ) : ControlWindow( owner )
	{
		addStyle( CBS_DROPDOWNLIST|CBS_NOINTEGRALHEIGHT );
	}

	void addEntry( const char *text )
	{
		message( CB_ADDSTRING, 0, LPARAM(text) );
	}
	void clearEntries( void )
	{
		message( CB_RESETCONTENT );
	}
	void deleteEntry( int entryId )
	{
		message( CB_DELETESTRING, entryId );
	}
	int getSelection( void ) const
	{
		return (int)message( CB_GETCURSEL );
	}
	STRING getSelectedText( void ) const;
	int findEntry( const char *entry ) const
	{
		return int(message( CB_FINDSTRINGEXACT, WPARAM(-1), LPARAM(entry) ));
	}

	void selectEntry( int entryId ) const
	{
		message( CB_SETCURSEL, entryId );
	}
	void selectEntry( const char *entry ) const
	{
		message( CB_SELECTSTRING, WPARAM(-1), LPARAM(entry) );
	}
	size_t getNumItems( void ) const
	{
		return message( CB_GETCOUNT );
	}
};

class ListBox : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];
	ListBox( BasicWindow *owner = NULL ) : ControlWindow( owner )
	{
		addStyle(LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|WS_VSCROLL|WS_HSCROLL|WS_BORDER|WS_TABSTOP);
	}

	void setTabStops( int numTabs, int *tabs ) const
	{
		message( LB_SETTABSTOPS, WPARAM(numTabs), LPARAM(tabs) );
	}
	void insertEntry( int entryId, const char *text ) const
	{
		message( LB_INSERTSTRING, WPARAM(entryId), LPARAM(LPSTR(text)) );
	}
	void addEntry( const char *text ) const
	{
		message( LB_ADDSTRING, 0, LPARAM(LPSTR(text)) );
	}
	STRING getEntry( int entryId ) const
	{
		STRING entry;
		size_t newSize = message( LB_GETTEXTLEN, entryId );
		char *buffer = entry.setActSize(newSize);
		buffer[newSize] = 0;
		message( LB_GETTEXT, entryId, LPARAM(buffer) );
		buffer[newSize] = 0;

		return entry;
	}
	void clearEntries( void ) const
	{
		message( LB_RESETCONTENT );
	}
	void clearSelection( void );

	void selectEntry( int entryId );
	void unselectEntry( int entryId );
	void selectEntry( const char *entry )
	{
		int	idx;

		idx = (int)message( LB_FINDSTRINGEXACT, WPARAM(-1), LPARAM(entry) );
		if( idx != LB_ERR )
			selectEntry( idx );
	}

	void deleteEntry( int idx ) const
	{
		idx = (int)message( LB_DELETESTRING, idx, 0 );
	}

	void deleteEntry( const char *entry ) const
	{
		int	idx;

		idx = (int)message( LB_FINDSTRINGEXACT, WPARAM(-1), LPARAM(entry) );
		if( idx != LB_ERR )
			deleteEntry( idx );
	}
	void replaceEntry( int idx, const char *text ) const
	{
		deleteEntry( idx );
		insertEntry( idx, text );
	}

	bool isMultipleSelection( void ) const
	{
		return hasStyle( LBS_MULTIPLESEL ) || hasStyle( LBS_EXTENDEDSEL );
	}
	int getSelection( void ) const;

	bool isSelected( size_t entry ) const
	{
		return message( LB_GETSEL, entry, 0L ) > 0 ? true : false;
	}
	size_t	getNumEntries( void ) const
	{
		return size_t(message( LB_GETCOUNT ));
	}
	size_t	getNumVisible( void ) const;
	size_t	getFirstVisible( void ) const
	{
		return message( LB_GETTOPINDEX );
	}
	void setFirstVisible( size_t first ) const
	{
		message( LB_SETTOPINDEX, first );
	}
	void setVisible( size_t item ) const;
	void showLast() const
	{
		setVisible(getNumEntries());
	}


	void	setItems( T_STRING items );
	void	setSelectedItems( T_STRING items );
	void	setSelectedItems( const gak::ArrayOfInts &data );
	STRING	getSelectedItems( void ) const;
	size_t	getSelectedItems( gak::ArrayOfInts *data ) const;

	size_t getNumSelected( void ) const;

	void enableMultiSelection( void )
	{
		addStyle( LBS_MULTIPLESEL );
	}
	void disableMultiSelection( void )
	{
		removeStyle( LBS_MULTIPLESEL );
	}
	void enableExtendedSelection( void )
	{
		addStyle( LBS_EXTENDEDSEL );
	}
	void disableExtendedSelection( void )
	{
		removeStyle( LBS_EXTENDEDSEL );
	}

};

class Label : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];
	Label( BasicWindow *owner=NULL ) : ControlWindow( owner )
	{
		addStyle( SS_SIMPLE );
	}
	void setText( const STRING &text )
	{
		ControlWindow::setText( text );
		invalidateWindow();
		resize();
	}
	template <typename ResourceIdT>
	bool setIcon( ResourceIdT resourceId )
	{
		bool			success = false;
		unsigned long	style = getStyle();

		HICON icon = Application::loadIcon( resourceId );
		if( icon )
		{
			if( !(style & SS_ICON) || (style & SS_BITMAP) )
			{
				removeStyle( SS_BITMAP );
				style = addStyle( SS_ICON );
			}
			message( STM_SETICON, (WPARAM)icon );
			success = true;
		}
		else if( style & SS_ICON )
		{
			style = removeStyle( SS_ICON );
		}
		return success;
	}
};

class EDIT : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;
	//public:
	static const char className[];
	//private:

	public:
	enum EditCharset { csANSI, csOEM, csUTF8 };

	private:
	enum EditCharset	m_charSet;
	bool				m_editable;
	int					m_tabWidth;
	RLINE_ENDS			m_lineEnd;

	public:
	EDIT( BasicWindow *owner=NULL ) : ControlWindow( owner )
	{
		m_lineEnd	= RL_END_CRLF;
		m_tabWidth	= 8;
		m_editable	= true;
		m_charSet	= csANSI;

		addStyle( WS_BORDER|ES_NOHIDESEL|ES_AUTOHSCROLL );
	}
	void readFromFile( const char *fileName );
	bool saveToFile( const char *fileName );
	size_t searchText(
		const char	*text,
		size_t		startPos,
		bool		wholeWord,
		bool		matchCase,
		bool		downSearch,
		const char	*replaceText=NULL,
		bool		all=false
	);
	void replaceSelection( const char *replace )
	{
		message( EM_REPLACESEL, 0, LPARAM((char far *)replace) );
	}
	void setFont( Font &font )
	{
		ControlWindow::setFont( font );

		setTabWidth( m_tabWidth );
	}
	void setTabWidth( unsigned int numChar )
	{
		Size		charWidth;
		DrawDevice	hDC( this );
		hDC.getTextExtent( " ", 1, &charWidth );
		m_tabWidth = numChar;
		unsigned int numPixels = numChar * charWidth.width;
		numPixels = numPixels*4 / LOWORD(GetDialogBaseUnits());
		message( EM_SETTABSTOPS, 1, LPARAM(&numPixels) );
		invalidateWindow();
	}
	int getTabWidth( void ) const
	{
		return m_tabWidth;
	}
	void setEditable( void )
	{
		m_editable = true;
		message( EM_SETREADONLY, false );
	}
	void setReadonly( void )
	{
		m_editable = false;
		message( EM_SETREADONLY, true );
	}
	bool isEditable( void ) const
	{
		return m_editable;
	}
	bool isChanged() const
	{
		return message( EM_GETMODIFY ) ? true : false;
	}
	void setChanged()
	{
		message( EM_SETMODIFY, true );
	}

	void setLineEnd( RLINE_ENDS lineEnd )
	{
		if( m_editable && m_lineEnd != lineEnd )
		{
			m_lineEnd = lineEnd;
			setChanged();
		}
	}
	RLINE_ENDS getLineEnd() const
	{
		return m_lineEnd;
	}

	void setCharset( EditCharset charset );
	void convertCharset( EditCharset charset );

	bool isOemConvert() const
	{
		return m_charSet == csOEM;
	}
	enum EditCharset getCharset() const
	{
		return m_charSet;
	}

	std::size_t getFirstVisible()
	{
		return message(EM_GETFIRSTVISIBLELINE);
	}
	void setFirstVisible( std::size_t line )
	{
		/// TODO check, whether setSelection is required
		setSelection( 0, 0 );
		message(EM_LINESCROLL, 0, line);
	}
	STRING getSelection() const;
	void getSelection( size_t *start, size_t *end = NULL ) const;
	void getCursorPosition( size_t *line, size_t *colum ) const;
	void setSelection( size_t start, size_t end );
	void hideSelection()
	{
		removeStyle( ES_NOHIDESEL );
	}
	void showSelection()
	{
		addStyle( ES_NOHIDESEL );
	}

	void indent( void );
	void unindent( void );

	void cut( void ) const
	{
		message( WM_CUT );
	}
	void copy( void ) const
	{
		message( WM_COPY );
	}
	void paste( void ) const
	{
		message( WM_PASTE );
	}
	void clear( void ) const
	{
		message( WM_CLEAR );
	}
	void selectAll( void )
	{
		setSelection( 0, getTextLength() );
	}
	void deleteLine( void )
	{
		size_t	lineStart, lineEnd;

		getCursorPosition( &lineStart, &lineEnd );
		lineEnd = message( EM_LINEINDEX, lineStart-- );
		lineStart = message( EM_LINEINDEX, lineStart );

		setSelection( lineStart, lineEnd );

		cut();
	}

	void undo( void ) const
	{
		message( EM_UNDO );
	}

	void operator = ( const STRING &value )
	{
		setText( value );
	}

	STRING getString( void ) const
	{
		return getText( isOemConvert() );
	}
	operator STRING( void ) const
	{
		return getString();
	}

	void print( const char *fileName, PrintDevice *printer );

	void setLineBreak( void )
	{
		removeStyle( ES_AUTOHSCROLL|WS_HSCROLL );
		message( EM_FMTLINES, true );

	}
	void clrLineBreak( void )
	{
		addStyle( ES_AUTOHSCROLL|WS_HSCROLL );
		message( EM_FMTLINES, false );
	}
	bool isLineBreak( void )
	{
		return (bool)(!hasStyle( ES_AUTOHSCROLL|WS_HSCROLL ));
	}
	void setAutoIndent( void )
	{
		subclassControl();
	}

	void clrAutoIndent( void )
	{
		restoreProc();
	}
	virtual LRESULT callback( UINT uMsg, WPARAM wParam, LPARAM lParam  );
};

class MemoControl : public EDIT
{
	public:
	static const char className[];

	MemoControl( BasicWindow *owner=NULL ) : EDIT( owner )
	{
		addStyle( ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL|WS_HSCROLL|WS_VSCROLL );
	}
	SuccessCode create( CallbackWindow *parent, int x, int y, int w, int h )
	{
		SuccessCode	errFlag = EDIT::create( parent, x, y, w, h );

		if( errFlag == scSUCCESS )
		{
			message( EM_SETWORDBREAKPROC, 0, LPARAM(WordBreakProc) );
			message( EM_LIMITTEXT );
		}
		return errFlag;
	}
	SuccessCode create( CallbackWindow *parent )
	{
		SuccessCode	errFlag = EDIT::create( parent );

		if( errFlag == scSUCCESS )
		{
			message( EM_SETWORDBREAKPROC, 0, LPARAM(WordBreakProc) );
			message( EM_LIMITTEXT );
		}
		return errFlag;
	}
	void operator = ( const STRING &value )
	{
		setText( value );
	}
};

class EditControl : public EDIT
{
	public:
	static const char className[];

	EditControl( BasicWindow *owner=NULL ) : EDIT( owner )
	{
	}
	void operator = ( const STRING &value )
	{
		setText( value );
	}
};

class UpDownButton : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];
	UpDownButton( BasicWindow *owner=NULL ) : ControlWindow( owner )
	{
		addStyle( UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_AUTOBUDDY|UDS_ARROWKEYS );
	}
	int getPosition( void ) const
	{
		return int(message( UDM_GETPOS ));
	}
	void setPosition( short position )
	{
		message( UDM_SETPOS, 0, MAKELONG( position, 0 ) );
	}
	void setRange( short minVal, short maxVal )
	{
		message( UDM_SETRANGE, 0, MAKELONG( maxVal, minVal ) );
	}
};

class ScrollBar : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];

	ScrollBar( BasicWindow *owner=NULL ) : ControlWindow( owner ) {}
	SuccessCode create( CallbackWindow *parent, int x, int y, int w, int h )
	{
		addStyle( ( w >= h ) ? SBS_HORZ : SBS_VERT );

		return ControlWindow::create( parent, x, y, w, h );
	}
	SuccessCode create( CallbackWindow *parent )
	{
		return ControlWindow::create( parent );
	}

	void setRange( short minVal, short maxVal )
	{
		message( SBM_SETRANGE, minVal, maxVal );
	}
	int getPosition( void ) const
	{
		return int(message( SBM_GETPOS ));
	}
	void setPosition( short position )
	{
		message( SBM_SETPOS, position, true );
	}
};

class TrackBar : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];

	TrackBar( BasicWindow *owner=NULL ) : ControlWindow( owner ) {}
	int getPosition( void ) const
	{
		return int(message( TBM_GETPOS ));
	}
	void setPosition( short position )
	{
		message( TBM_SETPOS, true, position );
	}
	void setRange( short minVal, short maxVal )
	{
		message( TBM_SETRANGE, true, MAKELONG( minVal, maxVal ) );
	}
	void setTic( short position )
	{
		message( TBM_SETTIC, 0, position );
	}
	void setTics( const short *position, size_t numTicks );
};

class DateTimePicker : public ControlWindow
{
	virtual STRING getWindowClassName( void ) const;

	public:
	static const char className[];

	DateTimePicker( BasicWindow *owner=NULL ) : ControlWindow( owner ) {}

	long getSystemTime(SYSTEMTIME &result)
	{
		return DateTime_GetSystemtime(handle(), &result ); 
	}
	long setSystemTime(const SYSTEMTIME &time)
	{
		return DateTime_SetSystemtime(handle(), GDT_VALID, &time ); 
	}
	long setStopWatch(const gak::StopWatch &watch);
	long setTimeT( time_t seconds );

	long setMinTime(const SYSTEMTIME &time)
	{
		return DateTime_SetRange( handle(), GDTR_MIN, &time );
	}
	long setMaxTime(const SYSTEMTIME &time)
	{
		return DateTime_SetRange( handle(), GDTR_MAX, &time );
	}
};

class TreeNode
{
	friend class TreeView;

	HTREEITEM						handle;
	TreeNode						*parent;
	gak::ArrayOfPointer<TreeNode>	children;
	void							*data;
	STRING							label;

	TreeNode *findItem( HTREEITEM itemHandle ) const;
	TreeNode *findItem( void *data ) const;

	public:
	TreeNode( HTREEITEM handle = NULL, TreeNode *parent = NULL ) 
	: handle( handle ), parent( parent )
	{
		data = NULL;
	}

	HTREEITEM getHandle( void ) const
	{
		return handle;
	}
	TreeNode *getParent( void ) const
	{
		return parent;
	}
	void addChild( TreeNode *child )
	{
		children += child;
	}
	void addChild( TreeNode *child, size_t index )
	{
		children.insertElement( child, index );
	}
	void removeChild( TreeNode *child )
	{
		children.removeElementVal( child );
	}

	TreeNode *getFirstChild( void ) const
	{
		if( children.size() )
			return children[0];
		else
			return NULL;
	}
	TreeNode *getLastChild( void ) const
	{
		if( children.size() )
			return children[children.size() -1];
		else
			return NULL;
	}
	void setData( void *newData )
	{
		data = newData;
	}
	void *getData( void ) const
	{
		return data;
	}
	void setLabel( const STRING &newLabel )
	{
		label = newLabel;
	}
	const STRING &getLabel( void ) const
	{
		return label;
	}
	size_t getIndex( void )
	{
		return parent ? parent->getIndex( this ) : 0;
	}
	size_t getIndex( TreeNode *child )
	{
		return children.findElement( child );
	}
	TreeNode *getChild( size_t index ) const
	{
		return children[index];
	}
	size_t getNumChildren( void ) const
	{
		return children.size();
	}
};

class TreeView : public ControlWindow
{
	TreeNode	*root;
	public:
	static const char className[];
	private:

	virtual STRING getWindowClassName( void ) const;

	TreeNode *addItem( TreeNode *parent, const STRING &label, void *data, size_t index )
	{
		TV_INSERTSTRUCT	insertStruct;

		if( !parent )
		{
			clear();
			insertStruct.hParent = TVI_ROOT;
			insertStruct.hInsertAfter = TVI_ROOT;
		}
		else
		{
			insertStruct.hParent = parent->getHandle();
			HTREEITEM	destHandle;
			if( index == 0 )
				destHandle = TVI_FIRST;
			else if( index >= parent->getNumChildren() )
			{
				index = parent->getNumChildren();
				destHandle = TVI_LAST;
			}
			else
				destHandle = parent->getChild( index-1 )->getHandle();

			insertStruct.hInsertAfter = destHandle;
		}
		insertStruct.item.mask = TVIF_TEXT;
		insertStruct.item.pszText = label;
		insertStruct.item.cchTextMax = int(label.strlen());

		TreeNode *newNode = new TreeNode(
			HTREEITEM(
				message( TVM_INSERTITEM, 0, LPARAM( &insertStruct ) )
			),
			parent
		);
		newNode->setData( data );
		newNode->setLabel( label );
		if( !parent )
			root = newNode;
		else
		{
			parent->addChild( newNode, index );
		}

		return newNode;
	}

	public:
	TreeView( BasicWindow *owner=NULL ) : ControlWindow( owner )
	{
		root = NULL;
		addStyle( TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS );
	}
	~TreeView()
	{
		clear();
	}

	TreeNode *appendItem( TreeNode *parent, const STRING &label, void *data )
	{
		return addItem( parent, label, data, size_t(-1) );
	}
	TreeNode *moveItem( TreeNode *item, TreeNode *parent, size_t index=-1 );

	void changeLabel( TreeNode *node, const STRING &label )
	{
		TV_ITEM	itemStruct;
		itemStruct.mask = TVIF_HANDLE|TVIF_TEXT;
		itemStruct.hItem = node->getHandle();
		itemStruct.pszText = label;
		itemStruct.cchTextMax = int(label.strlen());
		message( TVM_SETITEM, 0, LPARAM( &itemStruct ) );
		node->setLabel( label );
	}
	void deleteItem( TreeNode *item )
	{
		TreeNode	*parent = item->getParent();
		if( parent )
		{
			parent->removeChild( item );
			message( TVM_DELETEITEM, 0, LPARAM( item->getHandle() ) );

			delete item;
		}
		else
		{
			clear();
		}
	}
	void clear( )
	{
		if( root )
		{
			message( TVM_DELETEITEM, 0, LPARAM( TVI_ROOT ) );
			delete root;
			root = NULL;
		}
	}
	TreeNode *getSelection() const
	{
		if( root )
		{
			HTREEITEM	item = HTREEITEM(message( TVM_GETNEXTITEM, TVGN_CARET ));

			return root->findItem( item );
		}
		else
			return NULL;
	}
	void selectItem( TreeNode *item )
	{
		message( TVM_SELECTITEM, TVGN_CARET, LPARAM( item->getHandle() ) );
	}
	TreeNode *findItem( void *data ) const
	{
		return root ? root->findItem( data ) : NULL;
	}
	void enableDragDrop()
	{
		removeStyle( TVS_DISABLEDRAGDROP );
	}
	TreeNode *startDrag( NM_TREEVIEW *lpnmtv )
	{
		TreeNode *item = root->findItem( lpnmtv->itemNew.hItem );
		if( item )
		{
			HIMAGELIST	himl;		// handle to image list 
  			RectBorder	rcItem;		// bounding rectangle of item 
  
  
			// Tell the tree-view control to create an image to use 
  
			// for dragging. 
  
			himl = TreeView_CreateDragImage(handle(), lpnmtv->itemNew.hItem); 
  
			// Get the bounding rectangle of the item being dragged. 
  
			TreeView_GetItemRect(handle(), lpnmtv->itemNew.hItem, &rcItem, TRUE); 
  
			// Start the drag operation. 
  
			ImageList_BeginDrag(himl, 0, 0, 0);
			ImageList_DragEnter(handle(), lpnmtv->ptDrag.x, lpnmtv->ptDrag.x);
		}

		return item;
	}
	TreeNode *doDrag( const Point &point )
	{
		TreeNode		*dragOver = NULL;
		HTREEITEM		htiTarget;	// Handle to target item. 
		TV_HITTESTINFO	tvht;		// Hit test information.

		ImageList_DragMove(point.x, point.y);
		// Turn off the dragged image so the background can be refreshed.
  
		ImageList_DragShowNolock(FALSE); 
                  
		// Find out if the pointer is on the item. If it is, 
  
		// highlight the item as a drop target. 
  
		tvht.pt.x = point.x; 
		tvht.pt.y = point.y; 
		if ((htiTarget = TreeView_HitTest(handle(), &tvht)) != NULL) 
		{
			dragOver = root->findItem( htiTarget );
			TreeView_SelectDropTarget(handle(), htiTarget); 
		} 
		ImageList_DragShowNolock(TRUE);

		return dragOver;
	}
	TreeNode *doDrop( void )
	{
		TreeNode *target = NULL;

		HTREEITEM htiDest = TreeView_GetDropHilight(handle());
		if (htiDest != NULL)
		{
			target = root->findItem( htiDest );
		}
		ImageList_EndDrag(); 
		TreeView_SelectDropTarget( handle(), NULL );

		return target;
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
