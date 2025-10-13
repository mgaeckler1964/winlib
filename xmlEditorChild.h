/*
		Project:		Windows Class Library
		Module:			xmlEditorChild.h
		Description:	A control used to edit XML data
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

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Device;
class XMLeditorChild;

namespace internal
{

class ResizeException
{
	int m_incWidth;

	public:
	ResizeException( int incWidth )
	{
		m_incWidth = incWidth;
	}
	int getIncWidth() const
	{
		return m_incWidth;
	}
};

class XML_VIEWER_BOX;
typedef gak::SharedObjectPointer<XML_VIEWER_BOX>	XML_VIEWER_BOX_PTR;

/*
	A XML_LINE_CHUNG represents one part of a line, with the same format
	I can contain either a part of an inline element or an inline block element.
*/
class XML_LINE_CHUNK
{
	Point				m_position;

	gak::xml::XmlText	*m_theElement;
	STRING				*m_text;

	XML_VIEWER_BOX_PTR	m_theInlineBlockBox;

	public:
	int					width;
	size_t				startPos, len;

	XML_LINE_CHUNK()
	{
		m_theInlineBlockBox = nullptr;
		m_theElement = nullptr;
		m_text = nullptr;
	}

	void moveBy( int x, int y );
	void moveRight( int x )
	{
		moveBy( x, 0 );
	}

	const Point &getPosition() const
	{
		return m_position;
	}
	void setPosition( int x, int y )
	{
		m_position.x = x;
		m_position.y = y;
	}

	void setText( gak::xml::XmlText *theElement, STRING *text )
	{
		m_theInlineBlockBox = nullptr;
		m_theElement = theElement;
		m_text = text;
	}
	gak::xml::XmlText *getTextElement() const
	{
		return m_theElement;
	}
	const STRING &getText() const
	{
		return *m_text;
	}
	STRING *getTextPtr() const
	{
		return m_text;
	}
	bool isTextElement() const
	{
		return m_theElement && m_text;
	}

	const XML_VIEWER_BOX *getInlineBox() const
	{
		return m_theInlineBlockBox;
	}
	XML_VIEWER_BOX *getInlineBox()
	{
		return m_theInlineBlockBox;
	}
	bool isBlockElement() const
	{
		return bool(m_theInlineBlockBox);
	}
	void setBlockElement( XML_VIEWER_BOX *theInlineBlockBox )
	{
		m_theInlineBlockBox = theInlineBlockBox;
		m_theElement = nullptr;
		m_text = nullptr;
	}
#ifdef _DEBUG
	void dump( FILE *fp, unsigned level )
	{
		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf(
			fp,
			"Pos: %d,%d width: %d\n",
			m_position.x, m_position.y,
			width
		);

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		if( m_theElement )
			fprintf( fp, "CHUNK Element %s", (const char *)m_theElement->getParent()->getTag() );
		else
			fputs( "No Element", fp );
		fprintf( fp, "%p\n", getInlineBox() );

		if( !m_theElement && !getInlineBox() )
			fputs( ">>> WARN: EMPTY CHUNK\n", fp );
		else if( m_theElement && getInlineBox() )
			fputs( ">>> WARN: OVERLOADED CHUNK\n", fp );
	}
#endif
};

/*
	A XML Line represent one line of a block element
*/
struct XML_LINE
{
	Point						m_position;
	int							m_lineWidth, m_lineHeight, m_maxWidth;
	gak::Array<XML_LINE_CHUNK>	m_theLine;

	XML_LINE()
	{
		m_position.x = 0;
		m_position.y = 0;
		m_maxWidth = m_lineWidth = m_lineHeight = 0;
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
		return m_theLine[chunk];
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
			m_position.x, m_position.y,
			m_lineWidth, m_lineHeight
		);
		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf( fp, "Num Chunks: %d\n", int(m_theLine.size()) );

		for( size_t i=0; i<m_theLine.size(); i++ )
			m_theLine[i].dump( fp, newLevel );

	}
#endif

};

class XML_CURSOR_POS
{
	XML_VIEWER_BOX_PTR	m_viewerBox;
	size_t				m_line, m_chunk, m_insertPos;
	int					m_horizOffset;

	public:
	gak::xml::Element *getElement() const;

	void setPosition(
		const XML_VIEWER_BOX_PTR	&viewerBox,
		size_t						line,
		size_t						chunk,
		size_t						insertPos,
		int							horizOffset
	)
	{
		m_viewerBox = viewerBox;
		m_line = line;
		m_chunk = chunk;
		m_insertPos = insertPos;
		m_horizOffset = horizOffset;
	}
	void setPosition(
		size_t	line,
		size_t	chunk,
		size_t	insertPos,
		int		horizOffset
	)
	{
		m_line = line;
		m_chunk = chunk;
		m_insertPos = insertPos;
		m_horizOffset = horizOffset;
	}
	void setPosition(
		size_t	chunk,
		size_t	insertPos,
		int		horizOffset
	)
	{
		m_chunk = chunk;
		m_insertPos = insertPos;
		m_horizOffset = horizOffset;
	}
	void setPosition(
		size_t insertPos,
		int	   horizOffset
	)
	{
		m_insertPos = insertPos;
		m_horizOffset = horizOffset;
	}
	void movePosition(
		int	insertPos,
		int	horizOffset
	)
	{
		m_insertPos += insertPos;
		m_horizOffset += horizOffset;
	}
	void clear()
	{
		m_viewerBox = nullptr;
		m_line = size_t(-1);
		m_chunk = size_t(-1);
		m_insertPos = 0;
		m_horizOffset = 0;
	}

	const XML_VIEWER_BOX *getViewerBox() const
	{
		return m_viewerBox;
	}
	XML_VIEWER_BOX *getViewerBox()
	{
		return m_viewerBox;
	}
	size_t getLine() const
	{
		return m_line;
	}
	size_t getChunk() const
	{
		return m_chunk;
	}
	size_t getInsertPos() const
	{
		return m_insertPos;
	}
	int getHorizOffset() const
	{
		return m_horizOffset;
	}
};

/*
	A XML_VIEWWER_ITEM represents one content item of an XML block element.
	the item can be either an inline element or an inline block element
*/
class XML_VIEWER_ITEM
{
	// the inline bloc element
	XML_VIEWER_BOX_PTR	m_viewerBox;

	// OR the XML Element
	gak::xml::Element	*m_theElement;
	bool				m_autoWrap, m_preserveBlanks, m_preserveBreaks;
	STRING				m_text;

	public:
	XML_VIEWER_ITEM()
	{
		m_theElement = nullptr;
	}
	~XML_VIEWER_ITEM()
	{
	}
	void clear()
	{
		m_viewerBox = nullptr;
		m_theElement = nullptr;
	}
	/*
		for line boxes
	*/
	void setXmlElement(
		gak::xml::Element *xmlElement, const STRING &text,
		bool autoWrap, bool preserveBlanks, bool preserveBreaks
	)
	{
		m_viewerBox = nullptr;
		m_theElement = xmlElement;
		m_text = text;
		m_autoWrap = autoWrap;
		m_preserveBlanks = preserveBlanks;
		m_preserveBreaks = preserveBreaks;
	}
	gak::xml::Element *getXmlElement( void  ) const
	{
		return m_theElement;
	}
	const STRING &getXmlText() const
	{
		return m_text;
	}
	STRING *getXmlTextPtr()
	{
		return &m_text;
	}
	bool getAutoWrap() const
	{
		return m_autoWrap;
	}
	bool getPreserveBreaks() const
	{
		return m_preserveBreaks;
	}
	bool getPreserveBlanks() const
	{
		return m_preserveBlanks;
	}

	/*
		for inline boxes
	*/
	void setViewerBox( XML_VIEWER_BOX *xmlViewerBox )
	{
		m_viewerBox = xmlViewerBox;
		m_theElement = nullptr;
	}
	const XML_VIEWER_BOX *getViewerBox( void  ) const
	{
		return m_viewerBox;
	}
	XML_VIEWER_BOX *getViewerBox( void  )
	{
		return m_viewerBox;
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

	RectBorder			m_docPosition;
	gak::css::Position	m_position;
	gak::css::Display	m_display;
	gak::css::ListStyle	m_listStyle;

	int					m_itemNumber;

	int					m_margin_left,
						m_margin_right,
						m_margin_top,
						m_margin_bottom;

	int					m_padding_left,
						m_padding_right,
						m_padding_top,
						m_padding_bottom;

	int					m_border_width_left,
						m_border_width_right,
						m_border_width_top,
						m_border_width_bottom;

	gak::css::Border	m_border_style_left,
						m_border_style_right,
						m_border_style_top,
						m_border_style_bottom;

	int					m_maxWidth,
						m_innerWidth,
						m_innerWidthPercent;

	int					m_maxHeight, m_innerHeight, m_innerHeightPercent;

	gak::xml::Element	*m_theElement;
	XML_VIEWER_BOX_PTR	m_parentBox;

	gak::Array<XML_VIEWER_ITEM>		m_childElements;
	gak::Array<XML_VIEWER_BOX_PTR>	m_subBoxes;
	gak::Array<RectBorder>			m_leftBoxes, m_rightBoxes;
	gak::Array<XML_LINE>			m_theContent;

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
		*screenPosition = m_docPosition;
		if( m_position < gak::css::POS_FIXED )
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
		m_theElement = nullptr;
		m_parentBox = nullptr;
		memset( &m_docPosition, 0, sizeof( m_docPosition ) );
		m_itemNumber = itemNumber;
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
	gak::css::Styles *getCssStyle();
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

	size_t getNumLines() const
	{
		return m_theContent.size();
	}
	const XML_LINE &getLine( size_t line ) const
	{
		return m_theContent[line];
	}
	gak::xml::Element *getElement() const
	{
		return m_theElement;
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
	const XML_VIEWER_BOX *getParent() const
	{
		return m_parentBox;
	}

	int getItemNumber() const
	{
		return m_itemNumber;
	}

#ifdef _DEBUG
	void dump( FILE *fp, unsigned level )
	{
		unsigned newLevel = level+1;

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		if( m_theElement )
			fprintf( fp, "%s\n", (const char *)m_theElement->getTag() );
		else
			fputs( "<NO ELEMENT>\n", fp );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fprintf( fp, "Num Lines: %d\n", (int)m_theContent.size() );
		for( size_t i=0; i<m_theContent.size(); i++ )
			m_theContent[i].dump( fp, newLevel );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fputs( "Subboxes:\n", fp );
		for( size_t i=0; i<m_subBoxes.size(); i++ )
			m_subBoxes[i]->dump( fp, newLevel );

		for( unsigned i=0; i<level; i++ )
			fputs( "    ", fp );
		fputs( "Childboxes:\n", fp );
		for( size_t i=0; i<m_childElements.size(); i++ )
		{
			XML_VIEWER_ITEM	&theItem = m_childElements[i];
			XML_VIEWER_BOX	*childBox = theItem.getViewerBox();
			if( childBox )
				childBox->dump( fp, newLevel );
		}
	}
#endif
};

class XML_TABLE_VIEWER_BOX : public XML_VIEWER_BOX
{
	gak::ArrayOfInts	m_styleWidths, m_columnWidths;

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
		m_columnWidths[itemNumber] = 0;
		m_styleWidths[itemNumber] = 0;
	}
	int getColumnWidth( int itemNumber )
	{
		return m_columnWidths[itemNumber];
	}
	void setColumnWidth( int itemNumber, int width )
	{
		int &oldWidth = m_columnWidths[itemNumber];
		if( oldWidth < width )
			oldWidth = width;
	}
	void setStyleWidth( int itemNumber, int width )
	{
		int &oldWidth = m_styleWidths[itemNumber];
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
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		table->initColumn( itemNumber );
	}
	int getColumnWidth( int itemNumber )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		return table->getColumnWidth( itemNumber );
	}
	void setColumnWidth( int itemNumber, int width )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		table->setColumnWidth( itemNumber, width );
	}
	void setStyleWidth( int itemNumber, int width )
	{
		XML_TABLE_VIEWER_BOX *table = (XML_TABLE_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
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
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		group->initColumn( itemNumber );
	}
	int getColumnWidth( int itemNumber )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		return group->getColumnWidth( itemNumber );
	}
	void setColumnWidth( int itemNumber, int width )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		group->setColumnWidth( itemNumber, width );
	}
	void setStyleWidth( int itemNumber, int width )
	{
		XML_TABLE_GROUP_VIEWER_BOX *group = (XML_TABLE_GROUP_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
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
	void initColumn()
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		row->initColumn( m_itemNumber );
	}
	int getColumnWidth()
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		return row->getColumnWidth( m_itemNumber );
	}
	void setColumnWidth( int width )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		row->setColumnWidth( m_itemNumber, width );
	}
	void setStyleWidth( int width )
	{
		XML_TABLE_ROW_VIEWER_BOX *row = (XML_TABLE_ROW_VIEWER_BOX *)((XML_VIEWER_BOX*)m_parentBox);
		row->setStyleWidth( m_itemNumber, width );
	}
	virtual int calcSize(
		Device &context,
		int x, int y,
		int maxWidth, int windowWidth, int windowHeight,
		RectBorder *screen
	);
};

}	// namespace internal

class XMLeditorChild : public ChildWindow
{
	private:
	bool							m_xmlFrames;
	internal::XML_VIEWER_BOX_PTR	m_theViewerBox;
	int								m_vertOffset, m_horizOffset;
	Size							m_size;
	RectBorder						m_boxSize;

	internal::XML_CURSOR_POS		m_cursorPos;
	bool							m_cursorVisible;

#ifdef _DEBUG
	void dump()
	{
		STRING	tmp = getenv( "TEMP" );
		tmp += "\\temp.txt";
		FILE *fp = fopen( tmp, "w" );
		m_theViewerBox->dump( fp, 0 );
		fclose( fp );
	}
#endif
	void enableCursor()
	{
		if( !hasTimer() )
		{
			m_cursorVisible = false;
			setTimer( 500 );
		}
		focus();
	}
	void disableCursor( Device &context )
	{
		if( hasTimer() )
		{
			hideCursor(context);
			m_cursorPos.clear();
			removeTimer();
		}
	}
	void hideCursor( Device &context )
	{
		if( m_cursorVisible )
			drawCursor( context );
	}

	static void registerClass();
	virtual STRING getWindowClassName() const;
	public:
	static const char className[];
	private:

	void drawCursor( Device &context );

	virtual void handleTimer();
	virtual ProcessStatus handleVertScroll( VertScrollCode scrollCode, int nPos, HWND scrollBar );
	virtual ProcessStatus handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND );
	virtual ProcessStatus handleRepaint( Device &hDC );
	virtual ProcessStatus handleResize( const Size &newSize );
	virtual ProcessStatus handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position );
	virtual void handleFocus();
	virtual ProcessStatus handleKeyDown( int key );
	virtual ProcessStatus handleCharacterInput( int c );
	virtual void handleKillFocus();

	public:
	XMLeditorChild( BasicWindow *owner ) 
	: ChildWindow( owner )
	{
		registerClass();
		m_theViewerBox = nullptr;
		m_xmlFrames = false;
		m_cursorVisible = false;
		m_vertOffset = m_horizOffset = 0;
	}

	void refresh();
	void setDocument( gak::xml::Document *newDocument );
	void showElement( gak::xml::Element *theElement );
	void enableXmlFrames()
	{
		if( !m_xmlFrames )
		{
			m_xmlFrames = true;
			invalidateWindow();
		}
	}
	void disableXmlFrames()
	{
		if( m_xmlFrames )
		{
			m_xmlFrames = false;
			invalidateWindow();
		}
	}
	bool isXmlFrameEnabled() const
	{
		return m_xmlFrames;
	}
	int getVertOffset() const
	{
		return m_vertOffset;
	}
	int getHorizOffset() const
	{
		return m_horizOffset;
	}
	int getWindowHeight() const
	{
		return m_size.height;
	}
	int getWindowWidth() const
	{
		return m_size.width;
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

namespace internal
{
	inline gak::xml::Element	*XML_CURSOR_POS::getElement() const
	{
		return m_viewerBox->getElement( m_line, m_chunk );
	}
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
