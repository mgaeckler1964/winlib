/*
		Project:		Windows Class Library
		Module:			xmlEditorChild.h
		Description:	A control used to edit XML data
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

#ifndef XML_EDITOR_CHILD_H
#define XML_EDITOR_CHILD_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib/childWin.h>
#include <gak/xml.h>
#include <gak/shared.h>

#include <stdio.h>

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

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

#define WM_XML_ITEM_CLICK	WM_USER
#define WM_XML_ITEM_CHANGED	WM_USER+1


// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Device;
class XML_VIEWER_BOX;
class XMLeditorChild;

typedef gak::SharedObjectPointer<XML_VIEWER_BOX>	XML_VIEWER_BOX_PTR;

/*
	A XML_LINE_CHUNG represents one part of a line, with the same format
	I can contain either a part of an inline element or an inline block element.
*/
class XML_LINE_CHUNK
{
	Point				position;

	gak::xml::XmlText	*theElement;
	STRING				*text;

	XML_VIEWER_BOX_PTR	theInlineBlockBox;

	public:
	int					width;
	size_t				startPos, len;

	XML_LINE_CHUNK()
	{
		theInlineBlockBox = NULL;
		theElement = NULL;
		text = NULL;
	}

	void moveBy( int x, int y );
	void moveRight( int x )
	{
		moveBy( x, 0 );
	}

	const Point &getPosition( void ) const
	{
		return position;
	}
	void setPosition( int x, int y )
	{
		position.x = x;
		position.y = y;
	}

	void setText( gak::xml::XmlText *theElement, STRING *text )
	{
		theInlineBlockBox = NULL;
		this->theElement = theElement;
		this->text = text;
	}
	gak::xml::XmlText *getTextElement( void ) const
	{
		return theElement;
	}
	const STRING &getText( void ) const
	{
		return *text;
	}
	STRING *getTextPtr( void ) const
	{
		return text;
	}
	bool isTextElement( void ) const
	{
		return theElement && text;
	}

	const XML_VIEWER_BOX *getInlineBox( void ) const
	{
		return theInlineBlockBox;
	}
	XML_VIEWER_BOX *getInlineBox( void )
	{
		return theInlineBlockBox;
	}
	bool isBlockElement( void ) const
	{
		return bool(theInlineBlockBox);
	}
	void setBlockElement( XML_VIEWER_BOX *theInlineBlockBox )
	{
		this->theInlineBlockBox = theInlineBlockBox;
		this->theElement = NULL;
		this->text = NULL;
	}
#ifdef _DEBUG
	void dump( FILE *fp, unsigned level )
	{
		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf(
			fp,
			"Pos: %d,%d width: %d\n",
			position.x, position.y,
			width
		);

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		if( theElement )
			fprintf( fp, "CHUNK Element %s", (const char *)theElement->getParent()->getTag() );
		else
			fputs( "No Element", fp );
		fprintf( fp, "%p\n", getInlineBox() );

		if( !theElement && !getInlineBox() )
			fputs( ">>> WARN: EMPTY CHUNK\n", fp );
		else if( theElement && getInlineBox() )
			fputs( ">>> WARN: OVERLOADED CHUNK\n", fp );
	}
#endif
};

/*
	A XML Line represent one line of a block element
*/
struct XML_LINE
{
	Point						position;
	int							lineWidth, lineHeight, maxWidth;
	gak::Array<XML_LINE_CHUNK>	theLine;

	XML_LINE()
	{
		position.x = 0;
		position.y = 0;
		maxWidth = lineWidth = lineHeight = 0;
	}
	void moveBy( int x, int y );
	void moveRight( int x )
	{
		moveBy( x, 0 );
	}
	void moveDown( int y )
	{
		moveBy( 0, y );
	}
	const XML_LINE_CHUNK &getChunk( size_t chunk ) const
	{
		return theLine[chunk];
	}
#ifdef _DEBUG
	void dump( FILE *fp, unsigned level )
	{
		int newLevel = level+1;
		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf(
			fp,
			"Pos: %d,%d width: %d height: %d\n",
			position.x, position.y,
			lineWidth, lineHeight
		);
		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf( fp, "Num Chunks: %d\n", (int)theLine.size() );

		for( size_t i=0; i<theLine.size(); i++ )
			theLine[i].dump( fp, newLevel );

	}
#endif

};

class XML_CURSOR_POS
{
	XML_VIEWER_BOX_PTR	viewerBox;
	size_t				line, chunk, insertPos;
	int					horizOffset;

	public:
	gak::xml::Element *getElement( void ) const;

	void setPosition(
		const XML_VIEWER_BOX_PTR	&viewerBox,
		size_t						line,
		size_t						chunk,
		size_t						insertPos,
		int							horizOffset
	)
	{
		this->viewerBox = viewerBox;
		this->line = line;
		this->chunk = chunk;
		this->insertPos = insertPos;
		this->horizOffset = horizOffset;
	}
	void setPosition(
		size_t	line,
		size_t	chunk,
		size_t	insertPos,
		int		horizOffset
	)
	{
		this->line = line;
		this->chunk = chunk;
		this->insertPos = insertPos;
		this->horizOffset = horizOffset;
	}
	void setPosition(
		size_t	chunk,
		size_t	insertPos,
		int		horizOffset
	)
	{
		this->chunk = chunk;
		this->insertPos = insertPos;
		this->horizOffset = horizOffset;
	}
	void setPosition(
		size_t insertPos,
		int	   horizOffset
	)
	{
		this->insertPos = insertPos;
		this->horizOffset = horizOffset;
	}
	void movePosition(
		int	insertPos,
		int	horizOffset
	)
	{
		this->insertPos += insertPos;
		this->horizOffset += horizOffset;
	}
	void clear( void )
	{
		this->viewerBox = NULL;
		this->line = size_t(-1);
		this->chunk = size_t(-1);
		this->insertPos = 0;
		this->horizOffset = 0;
	}

	const XML_VIEWER_BOX *getViewerBox( void ) const
	{
		return viewerBox;
	}
	XML_VIEWER_BOX *getViewerBox( void )
	{
		return viewerBox;
	}
	size_t getLine( void ) const
	{
		return line;
	}
	size_t getChunk( void ) const
	{
		return chunk;
	}
	size_t getInsertPos( void ) const
	{
		return insertPos;
	}
	int getHorizOffset( void ) const
	{
		return horizOffset;
	}
};

/*
	A XML_VIEWWER_ITEM represents one content item of an XML block element.
	the item can be either an inline element or an inline block element
*/
class XML_VIEWER_ITEM
{
	// the inline bloc element
	XML_VIEWER_BOX_PTR	viewerBox;

	// OR the XML Element
	gak::xml::Element	*theElement;
	bool				autoWrap, preserveBlanks, preserveBreaks;
	STRING				text;

	public:
	XML_VIEWER_ITEM()
	{
		theElement = NULL;
	}
	~XML_VIEWER_ITEM()
	{
	}
	void clear( void )
	{
		viewerBox = NULL;
		theElement = NULL;
	}
	/*
		for line boxes
	*/
	void setXmlElement(
		gak::xml::Element *xmlElement, const STRING &text,
		bool autoWrap, bool preserveBlanks, bool preserveBreaks
	)
	{
		viewerBox = NULL;
		theElement = xmlElement;
		this->text = text;
		this->autoWrap = autoWrap;
		this->preserveBlanks = preserveBlanks;
		this->preserveBreaks = preserveBreaks;
	}
	gak::xml::Element *getXmlElement( void  ) const
	{
		return theElement;
	}
	const STRING &getXmlText( void ) const
	{
		return text;
	}
	STRING *getXmlTextPtr( void )
	{
		return &text;
	}
	bool getAutoWrap( void ) const
	{
		return autoWrap;
	}
	bool getPreserveBreaks( void ) const
	{
		return preserveBreaks;
	}
	bool getPreserveBlanks( void ) const
	{
		return preserveBlanks;
	}

	/*
		for inline boxes
	*/
	void setViewerBox( XML_VIEWER_BOX *xmlViewerBox )
	{
		viewerBox = xmlViewerBox;
		theElement = NULL;
	}
	const XML_VIEWER_BOX *getViewerBox( void  ) const
	{
		return viewerBox;
	}
	XML_VIEWER_BOX *getViewerBox( void  )
	{
		return viewerBox;
	}
};

/*
	A XML_VIEWER_BOX is created for each xml block element
*/
class XML_VIEWER_BOX : public gak::SharedObject
{
	friend class XML_TABLE_ROW_VIEWER_BOX;

	protected:

	enum CURSOR_DIRECTION { CURSOR_UP, CURSOR_DOWN };

	RectBorder			docPosition;
	gak::css::Position	position;
	gak::css::Display	display;
	gak::css::ListStyle	listStyle;

	int					itemNumber;

	int					margin_left,
						margin_right,
						margin_top,
						margin_bottom;

	int					padding_left,
						padding_right,
						padding_top,
						padding_bottom;

	int					border_width_left,
						border_width_right,
						border_width_top,
						border_width_bottom;

	gak::css::Border	border_style_left,
						border_style_right,
						border_style_top,
						border_style_bottom;

	int					maxWidth,
						innerWidth,
						innerWidthPercent;

	int					maxHeight, innerHeight, innerHeightPercent;

	gak::xml::Element	*theElement;
	XML_VIEWER_BOX_PTR	parentBox;

	gak::Array<XML_VIEWER_ITEM>		childElements;
	gak::Array<XML_VIEWER_BOX_PTR>	subBoxes;
	gak::Array<RectBorder>			leftBoxes, rightBoxes;
	gak::Array<XML_LINE>			theContent;

	static STRING prepareText(
		const STRING &text, bool preserveBlanks, bool preserveBreaks
	);
	static int cssSizeToPixel(
		const Device &context, const STRING &cssString,
		gak::xml::Element	*fontSource
	);

	void createBox(
		const Device &context, gak::xml::Element *theRoot,
		gak::css::Display display, gak::css::Position position,
		int itemNumber
	);
	virtual XML_VIEWER_BOX_PTR createBox(
		gak::css::Display childDisplay,
		int itemNumber
	);
	void findBoxes( const Device &context, gak::xml::Element *theRoot );

	static void createFont(
		Device &context, gak::xml::Element *theText , bool withBackground
	);
	static int getLineHeight( Device &context, gak::xml::Element *theText  );
	int findTop( int y, int boxWidth, int innerLeft, int innerRight );
	size_t findLeftBox( int y );
	int findLeft( int y, int minLeft );
	size_t findRightBox( int y );
	int findRight( int y, int minRight );
	static int findCharPos(
		Device &context, int cursorPos,
		const STRING &text, size_t startPos, size_t *len
	);
	static int findWrapPos(
		Device &context, int maxWidth, bool breakWord,
		const STRING &text, size_t startPos, size_t *len
	);
	bool findCharPosInLine(
		Device &context, size_t line, int screenPosition,
		XML_CURSOR_POS *cursorPos,
		CURSOR_DIRECTION direction
	) const;

	void getScreenPosition(
		int vertOffset, int horizOffset,
		RectBorder *screenPosition
	)
	{
		*screenPosition = docPosition;
		if( position < gak::css::POS_FIXED )
		{
			screenPosition->left -= horizOffset;
			screenPosition->top -= vertOffset;;
			screenPosition->right -= horizOffset;
			screenPosition->bottom -= vertOffset;
		}
	}
	public:
	XML_VIEWER_BOX( int itemNumber=0 )
	{
		theElement = NULL;
		parentBox = NULL;
		memset( &docPosition, 0, sizeof( docPosition ) );
		this->itemNumber = itemNumber;
	}
	~XML_VIEWER_BOX()
	{
	}
	virtual void buildBoxTree(
		const Device &context, gak::xml::Element *theRoot, XML_VIEWER_BOX *container
	);


	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
	void drawLine(
		Device &context, XMLeditorChild *theWindow,
		XML_LINE &theLine, bool includeBlocks
	);
	void draw( Device &context, XMLeditorChild *theWindow );
	gak::css::Styles *getCssStyle( void );
	void moveBy( int x, int y );

	void moveRight( int x )
	{
		moveBy( x, 0 );
	}
	void moveDown( int y )
	{
		moveBy( 0, y );
	}

	bool findLineNChunk(
		Device &context, gak::xml::XmlText *xmlText, size_t insertPos,
		XML_CURSOR_POS *cursorPos
	);

	bool findElementScreenPosition(
		gak::xml::Element	*theElement,
		int					*x, int *y,
		XML_CURSOR_POS		*cursorPos
	) const;
	gak::xml::Element *findElementOnScreen(
		Device &context,
		int x, int y, int horizOffset, int vertOffset,
		XML_CURSOR_POS *cursorPos
	) const;

	size_t getNumLines( void ) const
	{
		return theContent.size();
	}
	const XML_LINE &getLine( size_t line ) const
	{
		return theContent[line];
	}
	gak::xml::Element *getElement( void ) const
	{
		return theElement;
	}
	gak::xml::Element *getElement( size_t line, size_t chunk ) const
	{
		const XML_LINE &theLine = getLine( line );
		const XML_LINE_CHUNK &theChunk = theLine.getChunk( chunk );
		return theChunk.getTextElement();
	}

	gak::xml::Element *locateFirstElement(
		XML_CURSOR_POS *cursorPos
	) const;

	bool locateNextLine(
		Device &context,
		XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos,
		int oldPosition
	);
	bool locateNextChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos );
	bool moveCursorDown( Device &context, XML_CURSOR_POS *cursorPos );
	bool moveCursorRight( Device &context, XML_CURSOR_POS *cursorPos );
	bool moveCursorRightWord( Device &context, XML_CURSOR_POS *cursorPos );
	void moveCursorEnd( XML_CURSOR_POS *cursorPos );

	gak::xml::Element *locateLastElement(
		XML_CURSOR_POS *cursorPos
	) const;
	bool locatePrevLine(
		Device &context,
		XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos,
		int oldPosition
	);
	bool locatePrevChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos );
	bool moveCursorLeft( Device &context, XML_CURSOR_POS *cursorPos );
	bool moveCursorLeftWord( Device &context, XML_CURSOR_POS *cursorPos );
	void  moveCursorStart( XML_CURSOR_POS *cursorPos );

	bool moveCursorUp( Device &context, XML_CURSOR_POS *cursorPos );

	gak::xml::XmlText *insertCharacter(
		Device &context, XMLeditorChild *theWindow, XML_CURSOR_POS *cursorPos,
		int c,
		bool *doWrap
	);
	gak::xml::XmlText *deleteCharacter(
		Device &context, XMLeditorChild *theWindow,
		const XML_CURSOR_POS *cursorPos,
		bool *doWrap
	);
	const XML_VIEWER_BOX *getParent( void ) const
	{
		return parentBox;
	}

	int getItemNumber( void ) const
	{
		return itemNumber;
	}

#ifdef _DEBUG
	void dump( FILE *fp, unsigned level )
	{
		unsigned newLevel = level+1;

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		if( theElement )
			fprintf( fp, "%s\n", (const char *)theElement->getTag() );
		else
			fputs( "<NO ELEMENT>\n", fp );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf( fp, "Num Lines: %d\n", (int)theContent.size() );
		for( size_t i=0; i<theContent.size(); i++ )
			theContent[i].dump( fp, newLevel );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fputs( "Subboxes:\n", fp );
		for( size_t i=0; i<subBoxes.size(); i++ )
			(*subBoxes[i]).dump( fp, newLevel );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fputs( "Childboxes:\n", fp );
		for( size_t i=0; i<childElements.size(); i++ )
		{
			XML_VIEWER_ITEM	&theItem = childElements[i];
			XML_VIEWER_BOX	*childBox = theItem.getViewerBox();
			if( childBox )
				childBox->dump( fp, newLevel );
		}
	}
#endif
};

class XML_TABLE_VIEWER_BOX : public XML_VIEWER_BOX
{
	gak::ArrayOfInts	styleWidths, columnWidths;

	public:
	XML_TABLE_VIEWER_BOX(int itemNumber) : XML_VIEWER_BOX(itemNumber) {}

	virtual void buildBoxTree(
		const Device &context, gak::xml::Element *theRoot, XML_VIEWER_BOX *container
	);
	virtual XML_VIEWER_BOX_PTR createBox(
		gak::css::Display childDisplay,
		int itemNumber
	);
	using XML_VIEWER_BOX::createBox;
	void calcColumnWidth(
		Device &context, int windowWidth, int windowHeight
	);
	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
	void initColumn( int itemNumber )
	{
		columnWidths[itemNumber] = 0;
		styleWidths[itemNumber] = 0;
	}
	int getColumnWidth( int itemNumber )
	{
		return columnWidths[itemNumber];
	}
	void setColumnWidth( int itemNumber, int width )
	{
		int &oldWidth = columnWidths[itemNumber];
		if( oldWidth < width )
			oldWidth = width;
	}
	void setStyleWidth( int itemNumber, int width )
	{
		int &oldWidth = styleWidths[itemNumber];
		if( oldWidth < width )
			oldWidth = width;
	}
};

class XML_TABLE_GROUP_VIEWER_BOX : public XML_VIEWER_BOX
{
	public:
	XML_TABLE_GROUP_VIEWER_BOX(int itemNumber) : XML_VIEWER_BOX(itemNumber) {}

	virtual void buildBoxTree(
		const Device &context, gak::xml::Element *theRoot, XML_VIEWER_BOX *container
	);
	virtual XML_VIEWER_BOX_PTR createBox(
		gak::css::Display childDisplay,
		int itemNumber
	);
	using XML_VIEWER_BOX::createBox;
	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
	void calcColumnWidth(
		Device &context, int windowWidth, int windowHeight
	);


	void initColumn( int itemNumber )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		table->initColumn( itemNumber );
	}
	int getColumnWidth( int itemNumber )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		return table->getColumnWidth( itemNumber );
	}
	void setColumnWidth( int itemNumber, int width )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		table->setColumnWidth( itemNumber, width );
	}
	void setStyleWidth( int itemNumber, int width )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		table->setStyleWidth( itemNumber, width );
	}
};

class XML_TABLE_ROW_VIEWER_BOX : public XML_VIEWER_BOX
{
	public:
	XML_TABLE_ROW_VIEWER_BOX(int itemNumber) : XML_VIEWER_BOX(itemNumber) {}

	virtual void buildBoxTree(
		const Device &context, gak::xml::Element *theRoot, XML_VIEWER_BOX *container
	);
	void calcColumnWidth(
		Device &context, int windowWidth, int windowHeight
	);
	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
	virtual XML_VIEWER_BOX_PTR createBox(
		gak::css::Display childDisplay,
		int itemNumber
	);
	using XML_VIEWER_BOX::createBox;
	void initColumn( int itemNumber )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		group->initColumn( itemNumber );
	}
	int getColumnWidth( int itemNumber )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		return group->getColumnWidth( itemNumber );
	}
	void setColumnWidth( int itemNumber, int width )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		group->setColumnWidth( itemNumber, width );
	}
	void setStyleWidth( int itemNumber, int width )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		group->setStyleWidth( itemNumber, width );
	}
};

class XML_TABLE_CELL_VIEWER_BOX : public XML_VIEWER_BOX
{
	public:
	XML_TABLE_CELL_VIEWER_BOX(int itemNumber) : XML_VIEWER_BOX(itemNumber) {}

	virtual void buildBoxTree(
		const Device &context, gak::xml::Element *theRoot, XML_VIEWER_BOX *container
	);
	void initColumn( void )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		row->initColumn( itemNumber );
	}
	int getColumnWidth( void )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		return row->getColumnWidth( itemNumber );
	}
	void setColumnWidth( int width )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		row->setColumnWidth( itemNumber, width );
	}
	void setStyleWidth( int width )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)parentBox);
		row->setStyleWidth( itemNumber, width );
	}
	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
};

class XMLeditorChild : public ChildWindow
{
	private:
	bool				xmlFrames;
	XML_VIEWER_BOX_PTR	theViewerBox;
	int					vertOffset, horizOffset;
	Size				size;
	RectBorder			boxSize;

	XML_CURSOR_POS		cursorPos;
	bool				cursorVisible;

#ifdef _DEBUG
	void dump( void )
	{
		STRING	tmp = getenv( "TEMP" );
		tmp += "\\temp.txt";
		FILE *fp = fopen( tmp, "w" );
		theViewerBox->dump( fp, 0 );
		fclose( fp );
	}
#endif
	void enableCursor( void )
	{
		if( !hasTimer() )
		{
			cursorVisible = false;
			setTimer( 500 );
		}
		focus();
	}
	void disableCursor( Device &context )
	{
		if( hasTimer() )
		{
			hideCursor(context);
			cursorPos.clear();
			removeTimer();
		}
	}
	void hideCursor( Device &context )
	{
		if( cursorVisible )
			drawCursor( context );
	}

	static void registerClass( void );
	virtual STRING getWindowClassName( void ) const;
	public:
	static const char className[];
	private:

	void drawCursor( Device &context );

	virtual void handleTimer( void );
	virtual ProcessStatus handleVertScroll( VertScrollCode scrollCode, int nPos, HWND scrollBar );
	virtual ProcessStatus handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND );
	virtual ProcessStatus handleRepaint( Device &hDC );
	virtual ProcessStatus handleResize( const Size &newSize );
	virtual ProcessStatus handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position );
	virtual void handleFocus( void );
	virtual ProcessStatus handleKeyDown( int key );
	virtual ProcessStatus handleCharacterInput( int c );
	virtual void handleKillFocus( void );

	public:
	XMLeditorChild( BasicWindow *owner ) 
	: ChildWindow( owner )
	{
		registerClass();
		theViewerBox = NULL;
		xmlFrames = false;
		cursorVisible = false;
		vertOffset = horizOffset = 0;
	}

	void refresh( void );
	void setDocument( gak::xml::Document *newDocument );
	void showElement( gak::xml::Element *theElement );
	void enableXmlFrames( void )
	{
		if( !xmlFrames )
		{
			xmlFrames = true;
			invalidateWindow();
		}
	}
	void disableXmlFrames( void )
	{
		if( xmlFrames )
		{
			xmlFrames = false;
			invalidateWindow();
		}
	}
	bool isXmlFrameEnabled( void ) const
	{
		return xmlFrames;
	}
	int getVertOffset( void ) const
	{
		return vertOffset;
	}
	int getHorizOffset( void ) const
	{
		return horizOffset;
	}
	int getWindowHeight( void ) const
	{
		return size.height;
	}
	int getWindowWidth( void ) const
	{
		return size.width;
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

inline gak::xml::Element	*XML_CURSOR_POS::getElement( void ) const
{
	return viewerBox->getElement( line, chunk );
}
// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

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

#endif
