/*
		Project:		Windows Class Library
		Module:			xmlEditorChild.cpp
		Description:	internal classes for xml editor
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

#include <gak/xml.h>

#include <winlib/xmlEditorChild.h>
#include <winlib/device.h>
#include <WINLIB/messages.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
 
#	pragma option -x
#	pragma option -xd
#	pragma option -xf
#endif

namespace winlib
{
namespace internal
{

using namespace gak;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

const int DEFAULT_FONT_SIZE=14;

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

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

inline void XML_LINE_CHUNK::moveBy( int x, int y )
{
	m_position.x += x;
	m_position.y += y;
	if( m_theInlineBlockBox )
		m_theInlineBlockBox->moveBy( x, y );
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

STRING XML_VIEWER_BOX::prepareText(
	const STRING &text, bool preserveBlanks, bool preserveBreaks
)
{
	size_t	numChars = 0;
	STRING	result;
	char	c;
	bool	space=false;

	for( size_t i=0; i<text.strlen(); i++ )
	{
		c = text[i];
		if( c == '\t' && preserveBlanks )
		{
			for( size_t i=numChars%8; i<8; i++ )
			{
				numChars++;
				result += ' ';
			}
		}
		else if( c == '\n' && preserveBreaks )
		{
			numChars = 0;
			result += c;
			space = true;
		}
		else if( isspace( c ) && !preserveBlanks )
		{
			if( !space )
			{
				numChars++;
				result += ' ';
				space = true;
			}
		}
		else
		{
			numChars++;
			result += c;
			space = false;
		}
	}

	return result;
}

int XML_VIEWER_BOX::findCharPos(
	Device &context, int cursorPos,
	const STRING &text, size_t startPos, size_t *len
)
{
	Size	size;

	size_t	minCharPos = startPos, maxCharPos = text.strlen();
	size_t	currentCharPos = startPos;

	while( minCharPos < maxCharPos )
	{
		currentCharPos = (maxCharPos + minCharPos)/2;
		context.getTextExtent( text, startPos, currentCharPos-startPos, &size );
		if( size.width > cursorPos )
			maxCharPos = currentCharPos;
		else if( size.width < cursorPos && minCharPos < currentCharPos)
			minCharPos = currentCharPos;
		else
			break;
	}


	*len = currentCharPos-startPos;

	return size.width;
}

bool XML_VIEWER_BOX::findCharPosInLine(
	Device &context, size_t line, int screenPosition,
	XML_CURSOR_POS 		*cursorPos,
	CURSOR_DIRECTION	direction
) const
{
	bool	found = false;
	int		newPosition;

	const XML_LINE	&theLine = m_theContent[line];
	size_t			numChunks = theLine.m_theLine.size();

	for( size_t chunk = 0; chunk <numChunks; chunk++ )
	{
		const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];

		if( theChunk.getPosition().x <= screenPosition
		&& theChunk.getPosition().x + theChunk.width >= screenPosition )
		{
			if( theChunk.isTextElement() )
			{
				size_t	insertPos;
				createFont( context, theChunk.getTextElement(), false );
				newPosition = findCharPos(
					context, screenPosition - theChunk.getPosition().x,
					theChunk.getText(), theChunk.startPos,
					&insertPos
				);

				insertPos += theChunk.startPos;
				cursorPos->setPosition(
					this, line, chunk, insertPos, newPosition
				);

				found = true;
			}
			else
			{
				const XML_VIEWER_BOX	*inlineBox = theChunk.getInlineBox();
				size_t					numLines = inlineBox->getNumLines();
#if DEBUG_LOG
				xml::Element	*myElement = getElement();
				xml::Element	*chunkElement = inlineBox->getElement();
				if( myElement )
				{
					doLogValueEx( gakLogging::llDetail, myElement->getTag() );
				}
				if( chunkElement )
				{
					doLogValueEx( gakLogging::llDetail, chunkElement->getTag() );
				}
#endif
				if( numLines )
				{
					found = inlineBox->findCharPosInLine(
						context, direction == CURSOR_UP ? numLines-1 : 0,
						screenPosition, cursorPos, direction );
				}
				if( chunk && !found ) // ??? 
				{
					found = findCharPosInLine(
						context, line, theChunk.getPosition().x-1,
						cursorPos, direction
					);
				}
				if( !found && chunk +1 < numChunks )
				{
					found = findCharPosInLine(
						context, line,
						theChunk.getPosition().x+theChunk.width+1,
						cursorPos, direction
					);
				}
			}
			break;
		}
	}

	if( !found && screenPosition < theLine.m_position.x )
	{
		// set the cursor to the first text chunk we found
		for( size_t chunk = 0; chunk <numChunks; chunk++ )
		{
			const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];

			if( theChunk.isTextElement() )
			{
				cursorPos->setPosition(
					this, line, chunk, theChunk.startPos, 0
				);

				found = true;
				break;
			}
		}
	}
	if( !found && screenPosition > theLine.m_position.x+theLine.m_lineWidth )
	{
		// set the cursor to the last text chunk we found
		for( size_t chunk = numChunks-1; chunk != -1; chunk-- )
		{
			const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];

			if( theChunk.isTextElement() )
			{
				cursorPos->setPosition(
					this, line, chunk,
					theChunk.startPos+theChunk.len, theChunk.width
				);

				found = true;
				break;
			}
		}
	}
	return found;
}

int XML_VIEWER_BOX::findWrapPos(
	Device &context, int maxWidth, bool breakWord,
	const STRING &text, size_t startPos, size_t *len
)
{
	Size	size;
	size_t	spacePos;

	size_t	minBreakPos = startPos, maxBreakPos = text.strlen();
	size_t	currentBreakPos = startPos;

	while( minBreakPos < maxBreakPos )
	{
		currentBreakPos = (maxBreakPos + minBreakPos)/2;
		context.getTextExtent( text, startPos, currentBreakPos-startPos, &size );
		if( size.width > maxWidth )
			maxBreakPos = currentBreakPos;
		else if( size.width < maxWidth )
			minBreakPos = currentBreakPos+1;
		else
			break;
	}


	if( isspace( text[currentBreakPos] ) && size.width <= maxWidth )
	{
		*len = currentBreakPos-startPos;
	}
	else
	{
		STRING	testText = text.subString( startPos, currentBreakPos-startPos );

		spacePos = testText.searchRChar( ' ' );
		if( spacePos != (size_t)-1 )
		{
			context.getTextExtent( text, startPos, spacePos, &size );
			*len = spacePos;

		}
		else if( breakWord )
		{
			*len = currentBreakPos-startPos;
		}
		else
		{
			*len = 0;
			size.width = 0;
		}
	}

	return size.width;
}

void XML_VIEWER_BOX::createFont(
	Device &context, xml::Element *theText, bool withColor
)
{
	bool		colorFound;

	css::Styles	*cssStyle = theText->getCssStyle();
	Font		&theFont = context.getFont();
	int			textDecorations = theText->getTextDecorations();

	theFont.deleteFont();

	STRING	fontName = cssStyle->getFontFamily();
	if( !fontName.isEmpty() )
	{
		theFont.setFontName( fontName );
	}

	CI_STRING	fontSizeStr = cssStyle->getFontSize();
	if( !fontSizeStr.isEmpty() )
	{
		theFont.setFontSizePixel( cssSizeToPixel( context, fontSizeStr, theText->getParent() )  );
	}
	else
		theFont.setFontSizePixel( DEFAULT_FONT_SIZE );

	CI_STRING	fontWeight = cssStyle->getFontWeight();
	if( !fontWeight.isEmpty() )
	{
		if( fontWeight == "bold" )
			theFont.setBold();
		else
			theFont.setNormal();
	}
	else
		theFont.setNormal();

	theFont.setUnderline( textDecorations & css::DECO_FLAG_UNDERLINE );
	theFont.setStrikeOut( textDecorations & css::DECO_FLAG_STROKE );

	css::FontStyle fontStyle = cssStyle->getFontStyle();
	theFont.setItalic( fontStyle > css::FONT_NORMAL );

	context.selectFont();
	context.setLetterSpacing(
		cssSizeToPixel( context, cssStyle->getLetterSpacing(), theText )
	);

	if( withColor )
	{
		css::Color	color;
		if( cssStyle->getColor( &color ) )
			context.setTextColor( color.red, color.green, color.blue );
		else
			context.setTextColor( 0, 0, 0 );

		colorFound = cssStyle->getBackgroundColor( &color );
		if( colorFound )
			context.setBackgroundColor( color.red, color.green, color.blue );
		else
			context.clrBackgroundColor();
	}
}

int XML_VIEWER_BOX::getLineHeight( Device &context, xml::Element *theText  )
{
	STRING	lineHeight = theText->getCssStyle()->getLineHeight();

	if( !lineHeight.isEmpty() )
		return cssSizeToPixel( context, lineHeight, theText );
	else
		return int(double(context.getFont().getFontSizePixel()) * 1.2 + 0.5);
}

int XML_VIEWER_BOX::cssSizeToPixel(
	const Device &context, const STRING &cssString,
	xml::Element	*fontSource
)
{
	double cssValue = atof( cssString );

	if( cssString.endsWith( "pt" ) )
		return context.pointToPixel( cssValue );
	else if( cssString.endsWith( "pc" ) )
		return context.picaToPixel( cssValue );
	else if( cssString.endsWith( "mm" ) )
		return context.mmToPixel( cssValue );
	else if( cssString.endsWith( "cm" ) )
		return context.cmToPixel( cssValue );
	else if( cssString.endsWith( "in" ) )
		return context.inToPixel( cssValue );
	else if( cssString.endsWith( "em" ) || cssString.endsWith( "ex" ) )
	{
		// we need the fontsize but do not use inherited values
		double		fontSize;
		css::Value	fontCssValue;
		while( fontSource )
		{
			fontCssValue = fontSource->getCssStyle()->getFontSize();
			if( !fontCssValue.isInherited() && !fontCssValue.isEmpty() )
/*v*/			break;

			fontSource = fontSource->getParent();
		}
		if( !fontCssValue.isInherited()
		&& !fontCssValue.isEmpty() )
		{
			fontSize = cssSizeToPixel(
				context, fontCssValue, fontSource->getParent()
			);
		}
		else
			fontSize = DEFAULT_FONT_SIZE;
		cssValue *= fontSize;

		if( cssString.endsWith( "ex" ) )
			cssValue /= 2;
	}

	return int(cssValue+0.5);
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

int XML_VIEWER_BOX::findTop( int y, int boxWidth, int innerLeft, int innerRight )
{
	int		leftBorder, rightBorder, nextY;

	size_t	leftRectIdx = 0;
	size_t	rightRectIdx = 0;

	leftBorder = innerLeft;
	rightBorder = innerRight;

	while( leftRectIdx < m_leftBoxes.size()
	|| rightRectIdx < m_rightBoxes.size() )
	{
		leftRectIdx = findLeftBox( y );
		rightRectIdx = findRightBox( y );


		if( leftRectIdx < m_leftBoxes.size() )
		{
			const RectBorder &leftRect = m_leftBoxes[leftRectIdx];
			leftBorder = leftRect.right;
			nextY = leftRect.bottom;
		}

		if( rightRectIdx < m_rightBoxes.size() )
		{
			const RectBorder &rightRect = m_rightBoxes[rightRectIdx];
			rightBorder = rightRect.left;
			if( rightRect.bottom < nextY
			|| leftRectIdx >= m_leftBoxes.size())
				nextY = rightRect.bottom;
		}

		if( rightBorder-leftBorder >= boxWidth )
/*v*/		break;

		y = nextY+1;
		leftBorder = innerLeft;
		rightBorder = innerRight;

	}
	if( rightBorder-leftBorder < boxWidth )
		y = -1;		// not found

	return y;
}

size_t XML_VIEWER_BOX::findLeftBox( int y )
{
	size_t	leftBoxIdx = (size_t)-1;
	int		left = 0;

	for( size_t i=0; i<m_leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_leftBoxes[i];
		if( boxRect.top <= y && boxRect.bottom > y )
		{
			if( boxRect.right > left )
			{
				left = boxRect.right;
				leftBoxIdx = i;
			}
		}
	}

	return leftBoxIdx;
}

int XML_VIEWER_BOX::findLeft( int y, int minLeft )
{
	int left = minLeft;
	for( size_t i=0; i<m_leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_leftBoxes[i];
		if( boxRect.top <= y && boxRect.bottom > y )
		{
			if( boxRect.right > left )
				left = boxRect.right;
		}
	}

	return left;
}

size_t XML_VIEWER_BOX::findRightBox( int y )
{
	size_t	rightBoxIdx = (size_t)-1;;
	int		right = m_docPosition.right;

	for( size_t i=0; i<m_rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_rightBoxes[i];
		if( boxRect.top <= y && boxRect.bottom > y )
		{
			if( boxRect.left < right )
			{
				right = boxRect.left;
				rightBoxIdx = i;
			}
		}
	}

	return rightBoxIdx;
}

int XML_VIEWER_BOX::findRight( int y, int maxRight )
{
	int right = maxRight;
	for( size_t i=0; i<m_rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_rightBoxes[i];
		if( boxRect.top <= y && boxRect.bottom > y )
		{
			if( boxRect.left < right )
				right = boxRect.left;
		}
	}

	return right;
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

css::Styles *XML_VIEWER_BOX::getCssStyle()
{
	css::Styles		*theStyles = nullptr;
	XML_VIEWER_BOX	*theBox = this;
	xml::Element	*theElement = theBox->m_theElement;

	while( theBox && !theStyles )
	{
		if( theElement )
		{
			theStyles = theElement->getCssStyle();
			break;
		}

		theBox = theBox->m_parentBox;
		theElement = theBox->m_theElement;
	}

	return theStyles;
}

/*
--------------------------------------------------------------------------------
	text changing
--------------------------------------------------------------------------------
*/
xml::XmlText *XML_VIEWER_BOX::insertCharacter(
	Device &context, XMLeditorChild *theWindow,
	XML_CURSOR_POS *cursorPos, int c,
	bool *doWrap
)
{
	xml::XmlText	*xmlText = nullptr;

	*doWrap = false;

	if( cursorPos->getViewerBox() )
	{
		XML_LINE &theLine = m_theContent[cursorPos->getLine()];
		XML_LINE_CHUNK &theChunk = theLine.m_theLine[cursorPos->getChunk()];
		if( theChunk.isTextElement() )
		{
			Size			size;
			size_t			insertPos = cursorPos->getInsertPos();
			xml::XmlText	*theElement = theChunk.getTextElement();

			theChunk.getTextPtr()->insChar( insertPos, char(c) );
			theChunk.len++;

			createFont( context, theElement, false );
			context.getTextExtent( char(c), &size );
			cursorPos->movePosition( 1, size.width );
			xmlText = theElement;

			if( theLine.m_lineWidth + size.width <= theLine.m_maxWidth )
			{
				// yeah we do not need an eintire refresh
				// move the cursor right
				theLine.m_lineWidth += size.width;
				theChunk.width += size.width;

				// move other junks right
				bool otherElementsFound = false;
				for(
					size_t chunk = cursorPos->getChunk()+1;
					chunk < theLine.m_theLine.size();
					chunk++ )
				{
					XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
					theChunk.moveRight( size.width );
					otherElementsFound = true;
				}

				for(
					size_t line=cursorPos->getLine()+1;
					!otherElementsFound && line < m_theContent.size();
					line++
				)
				{
					XML_LINE &theLine = m_theContent[line];
					for(
						size_t chunk = 0;
						!otherElementsFound && chunk < theLine.m_theLine.size();
						chunk++ )
					{
						XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
						if( theChunk.getTextElement() == theElement )
							theChunk.startPos++;
						else
							otherElementsFound = true;
					}
				}
				drawLine( context, theWindow, theLine, true );
			}
			else
				*doWrap = true;

			theElement->setValue( theChunk.getText() );
		}
	}

	return xmlText;
}

xml::XmlText *XML_VIEWER_BOX::deleteCharacter(
	Device &context, XMLeditorChild *theWindow,
	const XML_CURSOR_POS *cursorPos,
	bool *doWrap
)
{
	xml::XmlText	*xmlText = nullptr;

	*doWrap = false;

	if( cursorPos->getViewerBox() )
	{
		XML_LINE		&theLine = m_theContent[cursorPos->getLine()];
		XML_LINE_CHUNK	&theChunk = theLine.m_theLine[cursorPos->getChunk()];
		xml::XmlText	*theElement = theChunk.getTextElement();
		size_t			insertPos = cursorPos->getInsertPos();

		if( theElement && insertPos < theChunk.startPos + theChunk.len )
		{
			Size		size;
			char		text[2];

			text[0] = theChunk.getText()[insertPos];
			text[1] = 0;

			theChunk.getTextPtr()->delChar( insertPos );
			theChunk.len--;
			xmlText = theElement;

			createFont( context, theElement, false );
			context.getTextExtent( text, 1, &size );

			// move the rest of the line one position left
			theLine.m_lineWidth -= size.width;
			theChunk.width -= size.width;

			// move other junks left
			bool otherElementsFound = false;
			for(
				size_t chunk = cursorPos->getChunk()+1;
				chunk < theLine.m_theLine.size();
				chunk++ )
			{
				XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
				theChunk.moveRight( -size.width );
				otherElementsFound = true;
			}

			for(
				size_t line=cursorPos->getLine()+1;
				!otherElementsFound && line < m_theContent.size();
				line++
			)
			{
				XML_LINE &theLine = m_theContent[line];
				for(
					size_t chunk = 0;
					!otherElementsFound && chunk < theLine.m_theLine.size();
					chunk++ )
				{
					XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
					if( theChunk.getTextElement() == theElement )
						theChunk.startPos--;
					else
						otherElementsFound = true;
				}
			}
			drawLine( context, theWindow, theLine, true );
			theElement->setValue( theChunk.getText() );
		}
	}

	return xmlText;
}

/*
--------------------------------------------------------------------------------
	cursor movement
--------------------------------------------------------------------------------
*/

xml::Element *XML_VIEWER_BOX::locateFirstElement(
	XML_CURSOR_POS *cursorPos
) const
{
	doEnterFunctionEx(gakLogging::llDetail, "xml::Element *XML_VIEWER_BOX::locateFirstElement(...)");

	xml::Element	*theElement = nullptr;

	if( m_theContent.size() )
	{
		const XML_LINE &theLine = m_theContent[0];
		if( theLine.m_theLine.size() )
		{
			const XML_LINE_CHUNK	&theChunk = theLine.m_theLine[0];

			theElement = theChunk.getTextElement();
			if( theElement )
			{
				cursorPos->setPosition( this, 0, 0, 0, 0 );
			}
			else
			{
				theElement = theChunk.getInlineBox()->locateFirstElement( cursorPos );
			}
		}
	}

	if( !theElement )
	{
		size_t	numSubBoxes = m_subBoxes.size();
		for( size_t i=0; i<numSubBoxes; i++ )
		{
			theElement = m_subBoxes[i]->locateFirstElement( cursorPos );
			if( theElement )
				break;
		}
	}

	if( !theElement )
	{
		size_t numChildBoxes = m_childElements.size();
		for( size_t i=0; i<numChildBoxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox )
			{
				theElement = theBox->locateFirstElement( cursorPos );
				if( theElement )
				{
					break;
				}
			}
		}
	}
	return theElement;
}

xml::Element *XML_VIEWER_BOX::locateLastElement(
	XML_CURSOR_POS *cursorPos
) const
{
	doEnterFunctionEx(gakLogging::llDetail, "xml::Element *XML_VIEWER_BOX::locateLastElement(...)");

	xml::Element	*theElement = nullptr;

	size_t	numLines = m_theContent.size();
	if( numLines )
	{
		size_t					line = numLines-1;
		const XML_LINE			&theLine = m_theContent[line];
		size_t					chunk = theLine.m_theLine.size()-1;

		const XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];

		theElement = theChunk.getTextElement();
		if( theElement )
		{
			cursorPos->setPosition(
				this, line, chunk,
				theChunk.startPos+theChunk.len,
				theChunk.width
			);
		}
		else
		{
			theElement = theChunk.getInlineBox()->locateLastElement( cursorPos );
		}
	}

	if( !theElement )
	{
		size_t	numSubBoxes = m_subBoxes.size();
		for( size_t i=numSubBoxes-1; i!=-1; i-- )
		{
			theElement = m_subBoxes[i]->locateLastElement( cursorPos );
			if( theElement )
			{
				break;
			}
		}
	}

	if( !theElement )
	{
		size_t numChildBoxes = m_childElements.size();
		for( size_t i=numChildBoxes; i!=-1; i-- )
		{
			const XML_VIEWER_ITEM	&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox )
			{
				theElement = theBox->locateLastElement( cursorPos );
				if( theElement )
				{
					break;
				}
			}
		}
	}
	return theElement;
}

bool XML_VIEWER_BOX::locateNextLine(
	Device &context,
	XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos,
	int oldPosition
)
{
	bool	found = false;

	size_t numLines = m_theContent.size();
	if( numLines > 1 )
	{
		numLines--;
		for( size_t	line=0; line < numLines && !found; line++ )
		{
			XML_LINE &curLine = m_theContent[line];
			size_t numChunks = curLine.m_theLine.size();
			for(
				size_t chunk=0;
				chunk < numChunks && !found;
				chunk++
			)
			{
				XML_LINE_CHUNK	&theChunk = curLine.m_theLine[chunk];
				if( theChunk.getInlineBox() == current )
				{
					// yeah we got it
					found = findCharPosInLine(
						context, line+1, oldPosition,
						cursorPos, CURSOR_DOWN
					);
				}
			}
		}
	}

	if( !found && bool(m_parentBox) )
	{
		found = m_parentBox->locateNextLine( context, this, cursorPos, oldPosition );
	}
	return found;
}

bool XML_VIEWER_BOX::locatePrevLine(
	Device &context,
	XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos,
	int oldPosition
)
{
	bool	found = false;

	size_t numLines = m_theContent.size();
	if( numLines > 1 )
	{
		for( size_t	line=numLines-1; line != -1 && !found; line-- )
		{
			XML_LINE &curLine = m_theContent[line];
			size_t numChunks = curLine.m_theLine.size();
			for(
				size_t chunk=numChunks-1;
				chunk != -1 && !found;
				chunk--
			)
			{
				XML_LINE_CHUNK	&theChunk = curLine.m_theLine[chunk];
				if( theChunk.getInlineBox() == current )
				{
					// yeah we got it
					if( line != 0 )
					{
						found = findCharPosInLine(
							context, line-1, oldPosition,
							cursorPos, CURSOR_UP
						);
					}
					break;
				}
			}
		}
	}

	if( !found && bool(m_parentBox) )
	{
		found = m_parentBox->locatePrevLine( context, this, cursorPos, oldPosition );
	}

	return found;
}

/// @TODO make const
bool XML_VIEWER_BOX::locateNextChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos )
{
	bool	found = false;

	size_t	numSubBoxes, numLines, numChunks;

	size_t	numElements = m_childElements.size();
	if( numElements > 1 )
	{
		numElements--;
		for( size_t i=0; i<numElements && !found; i++ )
		{
			XML_VIEWER_ITEM			&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox == current )
			{
				i++;
				XML_VIEWER_ITEM			&theItem = m_childElements[i];
				const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();
				if( theBox )
				{
					found = theBox->locateFirstElement( cursorPos ) != nullptr;
				}
				break;
			}
		}
	}

	if( !found )
	{
		numSubBoxes = m_subBoxes.size();
		if( numSubBoxes > 1 )
		{
			numSubBoxes--;
			for( size_t i=0; i<numSubBoxes; i++ )
			{
				XML_VIEWER_BOX *theBox = m_subBoxes[i];
				if( theBox == current )
				{
					i++;
					theBox = m_subBoxes[i];
					found = theBox->locateFirstElement( cursorPos ) != nullptr;
					break;
				}
			}
		}
	}
	
	// first we need the position of the current CHUNK
	if( !found )
	{
		numLines = m_theContent.size();
		for( size_t	line=0; line < numLines && !found; line++ )
		{
			XML_LINE &theLine = m_theContent[line];
			numChunks = theLine.m_theLine.size();
			for(
				size_t chunk=0;
				chunk < numChunks && !found;
				chunk++
			)
			{
				XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];
				if( theChunk.getInlineBox() == current )
				{
					// yeah we got it
					if( ++chunk < numChunks )
					{
						XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];

						if( theChunk.getTextElement() )
						{
							found = true;
							cursorPos->setPosition(
								this, line, chunk, theChunk.startPos, 0
							);
						}
						else
						{
							found = locateFirstElement( cursorPos ) != nullptr;
						}
					}
					else if( ++line < numLines )
					{
						XML_LINE		&theLine = m_theContent[line];
						XML_LINE_CHUNK	&theChunk = theLine.m_theLine[0];

						if( theChunk.getTextElement() )
						{
							found = true;
							cursorPos->setPosition(
								this, line, 0, theChunk.startPos, 0
							);
						}
						else
						{
							found = locateFirstElement( cursorPos ) != nullptr;
						}
					}
				}
			}
		}
	}
	if( !found && bool(m_parentBox) )
	{
		found = m_parentBox->locateNextChunk( this, cursorPos );
	}
	return found;
}

/// @TODO make const
bool XML_VIEWER_BOX::locatePrevChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos )
{
	bool	found = false;

	size_t	numSubBoxes, numLines, numChunks;

	size_t	numElements = m_childElements.size();
	if( numElements > 1 )
	{
		for( size_t i=numElements-1; i && !found; i-- )
		{
			XML_VIEWER_ITEM			&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox == current )
			{
				i--;
				XML_VIEWER_ITEM			&theItem = m_childElements[i];
				const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();
				if( theBox )
				{
					found = theBox->locateLastElement( cursorPos ) != nullptr;
				}
				break;
			}
		}
	}

	if( !found )
	{
		numSubBoxes = m_subBoxes.size();
		if( numSubBoxes > 1 )
		{
			for( size_t i=numSubBoxes-1; i; i-- )
			{
				XML_VIEWER_BOX *theBox = m_subBoxes[i];
				if( theBox == current )
				{
					i--;
					theBox = m_subBoxes[i];
					found = theBox->locateLastElement( cursorPos ) != nullptr;
					break;
				}
			}
		}
	}

	// first we need the position of the current CHUNK
	if( !found )
	{
		numLines = m_theContent.size();
		if( numLines > 1 )
		{
			for( size_t	line=numLines-1; line != -1 && !found; line-- )
			{
				XML_LINE &theLine = m_theContent[line];
				numChunks = theLine.m_theLine.size();
				for(
					size_t chunk=numChunks-1;
					chunk != -1 && !found;
					chunk--
				)
				{
					XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];
					if( theChunk.getInlineBox() == current )
					{
						// yeah we got it
						if( --chunk != -1 )
						{
							XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];

							if( theChunk.getTextElement() )
							{
								found = true;
								cursorPos->setPosition(
									this, line, chunk,
									theChunk.startPos + theChunk.len,
									theChunk.width
								);
							}
							else
								found = locateFirstElement( cursorPos ) != nullptr;
						}
						else if( --line != -1 )
						{
							XML_LINE		&theLine = m_theContent[line];
							size_t			numChunks = theLine.m_theLine.size();
							if( numChunks )
							{
								numChunks--;
								XML_LINE_CHUNK	&theChunk = theLine.m_theLine[numChunks];

								if( theChunk.getTextElement() )
								{
									found = true;
									cursorPos->setPosition(
										this, line, chunk,
										theChunk.startPos + theChunk.len,
										theChunk.width
									);

								}
								else
								{
									found = locateLastElement( cursorPos ) != nullptr;
								}
							}
						}
					}
				}
			}
		}
	}
	if( !found && bool(m_parentBox) )
	{
		found = m_parentBox->locatePrevChunk( this, cursorPos );
	}
	return found;
}

bool XML_VIEWER_BOX::moveCursorRight( Device &context, XML_CURSOR_POS *cursorPos )
{
	Size			size;
	char			text[2];
	char			c;
	bool			moved = true;
	size_t			line = cursorPos->getLine();
	size_t			chunk = cursorPos->getChunk();
	XML_LINE		&curLine = m_theContent[line];
	XML_LINE_CHUNK	&curChunk = curLine.m_theLine[chunk];

	if( cursorPos->getInsertPos() < curChunk.startPos + curChunk.len )
	{
		c = curChunk.getText()[cursorPos->getInsertPos()];
		text[0] = c;
		text[1] = 0;

		createFont( context, curChunk.getTextElement(), false );
		context.getTextExtent( text, 1, &size );

		cursorPos->movePosition( 1, size.width );
	}
	else if( ++chunk < curLine.m_theLine.size() )
	{
		XML_LINE_CHUNK	&nextChunk = curLine.m_theLine[chunk];

		if( nextChunk.getTextElement() )
			cursorPos->setPosition( chunk, nextChunk.startPos, 0 );
		else
			nextChunk.getInlineBox()->locateFirstElement( cursorPos );
	}
	else if( ++line < m_theContent.size() )
	{
		XML_LINE		&nextLine = m_theContent[line];
		XML_LINE_CHUNK	&nextChunk = nextLine.m_theLine[0];

		if( nextChunk.getTextElement() )
			cursorPos->setPosition( line, chunk, nextChunk.startPos, 0 );
		else
			nextChunk.getInlineBox()->locateFirstElement( cursorPos );
	}
	else if( m_parentBox )
		m_parentBox->locateNextChunk( this, cursorPos );
	else
		moved = false;

	return moved;
}

bool XML_VIEWER_BOX::moveCursorRightWord( Device &context, XML_CURSOR_POS *cursorPos )
{
	Size			size;
	STRING			text;
	char			c;
	bool			moved, spaceFound;
	size_t			line = cursorPos->getLine();
	XML_LINE		&theLine = m_theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.m_theLine[cursorPos->getChunk()];
	size_t			insertPos = cursorPos->getInsertPos();
	size_t			maxPos = theChunk.startPos + theChunk.len;


	if( insertPos < maxPos )
	{
		spaceFound = false;
		while( insertPos < maxPos )
		{
			c = theChunk.getText()[insertPos];
			if( isspace( c ) )
			{
				insertPos++;
				text += c;
				spaceFound = true;
			}
			else if( spaceFound )
				break;
			else
			{
				insertPos++;
				text += c;
			}
		}

		createFont( context, theChunk.getTextElement(), false );
		context.getTextExtent( text, text.strlen(), &size );


		cursorPos->movePosition( int(text.strlen()), size.width );
		moved = true;
	}
	else
		moved = moveCursorRight( context, cursorPos );

	return moved;
}

bool XML_VIEWER_BOX::moveCursorLeft( Device &context, XML_CURSOR_POS *cursorPos )
{
	Size			size;
	char			text[2];
	char			c;
	bool			found = true;
	size_t			line = cursorPos->getLine();
	size_t			chunk = cursorPos->getChunk();
	XML_LINE		&theLine = m_theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];

	if( cursorPos->getInsertPos() > theChunk.startPos )
	{
		c = theChunk.getText()[cursorPos->getInsertPos()-1];
		text[0] = c;
		text[1] = 0;

		createFont( context, theChunk.getTextElement(), false );
		context.getTextExtent( text, 1, &size );


		cursorPos->movePosition( -1, -size.width );
	}
	else if( chunk )
	{
		chunk--;
		XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];
		if( theChunk.getTextElement() )
		{
			cursorPos->setPosition(
				chunk,
				theChunk.startPos + theChunk.len,
				theChunk.width
			);
		}
		else
			theChunk.getInlineBox()->locateLastElement( cursorPos );
	}
	else if( line )
	{
		XML_LINE		&theLine = m_theContent[--line];

		chunk = theLine.m_theLine.size()-1;
		XML_LINE_CHUNK	&theChunk = theLine.m_theLine[cursorPos->getChunk()];

		if( theChunk.getTextElement() )
		{
			cursorPos->setPosition(
				line, chunk,
				theChunk.startPos + theChunk.len,
				theChunk.width
			);
		}
		else
			found = theChunk.getInlineBox()->locateLastElement( cursorPos );
	}
	else if( m_parentBox )
		found = m_parentBox->locatePrevChunk( this, cursorPos );
	else
		found = false;

	return found;
}

bool XML_VIEWER_BOX::moveCursorLeftWord( Device &context, XML_CURSOR_POS *cursorPos )
{
	Size			size;
	STRING			text;
	char			c;
	bool			moved, inWord;
	size_t			line = cursorPos->getLine();
	XML_LINE		&theLine = m_theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.m_theLine[cursorPos->getChunk()];
	size_t			insertPos = cursorPos->getInsertPos();

	if( insertPos > theChunk.startPos )
	{
		inWord = false;
		while( insertPos > theChunk.startPos )
		{
			insertPos--;
			c = theChunk.getText()[insertPos];
			if( !isspace( c ) )
			{
				text += c;
				inWord = true;
			}
			else if( !inWord )
				text += c;
			else
/*v*/			break;
		}

		createFont( context, theChunk.getTextElement(), false );
		context.getTextExtent( text, text.strlen(), &size );

		cursorPos->movePosition( -(int)text.strlen(), - size.width );
		moved = true;
	}
	else
		moved = moveCursorLeft( context, cursorPos );

	return moved;
}

void XML_VIEWER_BOX::moveCursorStart( XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&theLine = m_theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.m_theLine[0];

	cursorPos->setPosition( 0, theChunk.startPos, 0 );
}

void XML_VIEWER_BOX::moveCursorEnd( XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&theLine = m_theContent[line];
	size_t			chunk = theLine.m_theLine.size()-1;
	XML_LINE_CHUNK	&theChunk = theLine.m_theLine[chunk];

	cursorPos->setPosition(
		chunk,
		theChunk.startPos + theChunk.len,
		theChunk.width
	);
}

bool XML_VIEWER_BOX::moveCursorUp( Device &context, XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&oldLine = m_theContent[line];
	XML_LINE_CHUNK	&oldChunk = oldLine.m_theLine[cursorPos->getChunk()];
	int 			oldPosition = oldChunk.getPosition().x + cursorPos->getHorizOffset();
	bool			found = false;

	if( line )
	{
		line--;
		found = findCharPosInLine(
			context, line, oldPosition,
			cursorPos, CURSOR_UP
		);
	}
	else if( m_parentBox )
		found = m_parentBox->locatePrevLine( context, this, cursorPos, oldPosition );

	return found;
}

bool XML_VIEWER_BOX::moveCursorDown( Device &context, XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&oldLine = m_theContent[line];
	XML_LINE_CHUNK	&oldChunk = oldLine.m_theLine[cursorPos->getChunk()];
	int 			oldPosition = oldChunk.getPosition().x + cursorPos->getHorizOffset();
	bool			found = false;

	if( line+1 < m_theContent.size() )
	{
		line++;

		found = findCharPosInLine(
			context, line, oldPosition,
			cursorPos, CURSOR_DOWN
		);
	}
	else if( m_parentBox )
		found = m_parentBox->locateNextLine( context, this, cursorPos, oldPosition );

	return found;
}

bool XML_VIEWER_BOX::findLineNChunk(
	Device &context, xml::XmlText *xmlText, size_t insertPos,
	XML_CURSOR_POS *cursorPos
)
{
	bool	found = false;
	size_t	numChunks, numLines = m_theContent.size();
	for( size_t	line=0; !found && line<numLines; line++ )
	{
		XML_LINE	&theLine = m_theContent[line];

		numChunks = theLine.m_theLine.size();
		for( size_t chunk=0; !found && chunk<numChunks; chunk++ )
		{
			XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
			if( theChunk.getTextElement() == xmlText
			&& theChunk.startPos <= insertPos
			&& theChunk.startPos + theChunk.len > insertPos )
			{
				Size	size;

				createFont( context, xmlText, false );
				//createFont( context, theElement, false );
				context.getTextExtent(
					theChunk.getText(),
					theChunk.startPos, insertPos - theChunk.startPos,
					&size );

				cursorPos->setPosition( line, chunk, insertPos, size.width );
			}
		}
	}

	return found;
}

bool XML_VIEWER_BOX::findElementScreenPosition(
	xml::Element		*theElement,
	int				*x, int *y,
	XML_CURSOR_POS *cursorPos
) const
{
	doEnterFunctionEx(gakLogging::llDetail, "bool XML_VIEWER_BOX::findElement(xml::Element*,...)");
	bool	found = false;

	size_t line, chunk, numLines, numChunks, numSubboxes, numChildboxes;

	if( theElement == m_theElement )
	{
		// find the first text chunk
		numLines = m_theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = m_theContent[line];

			numChunks = theLine.m_theLine.size();
			for( chunk=0;  chunk<numChunks; chunk++ )
			{
				const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
				if( theChunk.isTextElement() )
				{
					cursorPos->setPosition(
						this, line, chunk, theChunk.startPos, 0
					);

					*x = theChunk.getPosition().x;
					*y = theChunk.getPosition().y;
					found = true;
					break;
				}
			}
		}
	}

	if( !found )
	{
		numLines = m_theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = m_theContent[line];
			numChunks = theLine.m_theLine.size();
			for( chunk=0;  chunk<numChunks; chunk++ )
			{
				const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
				if( theChunk.isTextElement()
				&& theChunk.getTextElement() == theElement)
				{
					cursorPos->setPosition(
						this, line, chunk, theChunk.startPos, 0
					);
					*x = theChunk.getPosition().x;
					*y = theChunk.getPosition().y;
					found = true;
/*v*/				break;
				}
			}
		}
	}

	if( !found )
	{
		numSubboxes = m_subBoxes.size();
		for( size_t i=0; i<numSubboxes; i++ )
		{
			found = m_subBoxes[i]->findElementScreenPosition(
				theElement,
				x, y, cursorPos
			);
			if( found )
			{
				break;
			}
		}
	}

	if( !found )
	{
		numChildboxes = m_childElements.size();
		for( size_t i=0; i<numChildboxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox )
			{
				found = theBox->findElementScreenPosition(
					theElement,
					x, y, cursorPos
				);
				if( found )
				{
					break;
				}
			}
		}
	}
	return found;
}


xml::Element *XML_VIEWER_BOX::findElementOnScreen(
	Device &context,
	int x, int y, int horizOffset, int vertOffset,
	XML_CURSOR_POS *cursorPos
) const
{
	doEnterFunctionEx(gakLogging::llDetail, "xml::Element *XML_VIEWER_BOX::findElementOnScreen(...)");

	size_t line, chunk, numLines, numChunks, numSubboxes, numChildboxes;

	xml::Element	*theElement = nullptr;

	if( !theElement )
	{
		numSubboxes = m_subBoxes.size();
		for( size_t i=0; i<numSubboxes; i++ )
		{
			theElement = m_subBoxes[i]->findElementOnScreen(
				context,
				x, y, horizOffset, vertOffset, cursorPos
			);
			if( theElement )
			{
				break;
			}
		}
	}

	if( !theElement )
	{
		numChildboxes = m_childElements.size();
		for( size_t i=0; i<numChildboxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = m_childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox )
			{
				theElement = theBox->findElementOnScreen(
					context,
					x, y, horizOffset, vertOffset, cursorPos
				);
				if( theElement )
				{
					break;
				}
			}
		}
	}

	if( !theElement )
	{
		if( m_position != css::POS_FIXED )
		{
			x += horizOffset;
			y += vertOffset;
		}

		numLines = m_theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = m_theContent[line];
			if( theLine.m_position.x <= x
			&& theLine.m_position.x + theLine.m_lineWidth >=x
			&&  theLine.m_position.y <= y
			&& theLine.m_position.y + theLine.m_lineHeight >=y )
			{
				numChunks = theLine.m_theLine.size();
				for( chunk=0; chunk<numChunks; chunk++ )
				{
					const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
					if( theChunk.getPosition().x <= x
					&& theChunk.getPosition().x + theChunk.width >= x
					&& theChunk.isTextElement() )
					{
						size_t	insertPos;
						int	horizOffset;

						theElement = theChunk.getTextElement();
						createFont( context, theElement, false );
						horizOffset = findCharPos(
							context, x-theChunk.getPosition().x,
							theChunk.getText(), theChunk.startPos,
							&insertPos
						);

						insertPos += theChunk.startPos;

						cursorPos->setPosition(
							this, line, chunk, insertPos, horizOffset
						);
/*v*/					break;
					}
				}
			}
		}
	}

	if( !theElement )
	{
		if( m_docPosition.left <= x
		&& m_docPosition.right >= x
		&& m_docPosition.top <= y
		&& m_docPosition.bottom >= y )
		{
			// find the last text chunk
			for(
				line=m_theContent.size()-1;
				line!=m_theContent.no_index && !theElement;
				line--
			)
			{
				const XML_LINE	&theLine = m_theContent[line];

				for( chunk = theLine.m_theLine.size()-1; chunk != -1; chunk-- )
				{
					const XML_LINE_CHUNK &theChunk = theLine.m_theLine[chunk];
					if( theChunk.isTextElement() )
					{
						theElement = theChunk.getTextElement();
						cursorPos->setPosition(
							this, line, chunk,
							theChunk.startPos+theChunk.len, theChunk.width
						);
						break;
					}
				}
			}
		}
	}
	return theElement;
}

/*
--------------------------------------------------------------------------------
	columns functions
	these functions calculate the columns sizes
--------------------------------------------------------------------------------
*/
void XML_TABLE_VIEWER_BOX::calcColumnWidth(
	Device &context, int windowWidth, int windowHeight
)
{
	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_TABLE_GROUP_VIEWER_BOX *rowBox = (XML_TABLE_GROUP_VIEWER_BOX *)(XML_VIEWER_BOX *)m_subBoxes[i];

		rowBox->calcColumnWidth(
			context, windowWidth, windowHeight
		);
	}
}

void XML_TABLE_GROUP_VIEWER_BOX::calcColumnWidth(
	Device &context, int windowWidth, int windowHeight
)
{
	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_TABLE_ROW_VIEWER_BOX *rowBox = (XML_TABLE_ROW_VIEWER_BOX *)(XML_VIEWER_BOX *)m_subBoxes[i];

		rowBox->calcColumnWidth(
			context, windowWidth, windowHeight
		);
	}
}

void XML_TABLE_ROW_VIEWER_BOX::calcColumnWidth(
	Device &context, int windowWidth, int windowHeight
)
{
	RectBorder	cellRect;

	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_VIEWER_BOX *cellBox = m_subBoxes[i];
		int innerWidth = cellBox->m_innerWidth;

		if( innerWidth > 0 )
			setStyleWidth( cellBox->getItemNumber(), innerWidth );

		cellBox->m_innerWidth = -1;

		try
		{
			cellBox->calcSize(
				context, 0, 0, windowWidth, windowWidth, windowHeight,
				&cellRect
			);
		}
		catch( ... )
		{
			cellBox->m_innerWidth = innerWidth;
/*@*/		throw;
		}
		cellBox->m_innerWidth = innerWidth;
		int columnWidth = cellRect.right - cellRect.left;
		setColumnWidth( cellBox->getItemNumber(), columnWidth );
	}
}

/*
--------------------------------------------------------------------------------
	calculation functions
	these functions calculate the box sizes
--------------------------------------------------------------------------------
*/
void XML_LINE::moveBy( int x, int y )
{
	m_position.x += x;
	m_position.y += y;

	for( size_t i=0; i<m_theLine.size(); i++ )
		m_theLine[i].moveBy( x, y );
}

void XML_VIEWER_BOX::moveBy( int x, int y )
{
	m_docPosition.left += x;
	m_docPosition.top += y;
	m_docPosition.right += x;
	m_docPosition.bottom += y;

	size_t numChildBoxes = m_childElements.size();
	for( size_t i=0; i<numChildBoxes; i++ )
	{
		XML_VIEWER_ITEM	&theItem = m_childElements[i];
		XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

		if( theBox
		&& theBox->getCssStyle()->getDisplay() != css::DSP_INLINE_BLOCK )
		{
			theBox->moveBy( x, y );
		}
	}

	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		m_subBoxes[i]->moveBy( x, y );
	}
	for( size_t i=0; i<m_theContent.size(); i++ )
	{
		m_theContent[i].moveBy( x, y );
	}
}
int XML_TABLE_VIEWER_BOX::calcSize(
	Device &context, int left, int top, int maxWidth,
	int windowWidth, int windowHeight, RectBorder *screen
)
{
	doEnterFunctionEx(gakLogging::llDetail, "XML_TABLE_VIEWER_BOX::calcSize");

	XML_VIEWER_BOX	*groupBox;
	RectBorder		groupRect;
	int 			groupHeight, boxHeight = 0;
	size_t 			numColumns = m_columnWidths.size();
	int 			totalWidth = 0;
	int 			totalStyleWidth = 0;
	double			widthPercent;

	m_docPosition.left = left;
	m_docPosition.top = top;
	m_docPosition.right = left+maxWidth;
	m_docPosition.bottom = top+windowHeight;

	calcColumnWidth( context, windowWidth, windowHeight );

	for( size_t i=0; i<numColumns; i++ )
	{
		if( m_columnWidths[i] < m_styleWidths[i]  )
			m_columnWidths[i] = m_styleWidths[i];

		totalStyleWidth += m_styleWidths[i];
		totalWidth += m_columnWidths[i];
	}

	if(	totalStyleWidth > maxWidth )
	{
		// table becomes too large -> we need more space
/*@*/	throw ResizeException( totalStyleWidth - maxWidth );
	}

	if( totalWidth > maxWidth )
	{
		totalWidth = 0;
		for( size_t i=0; i<numColumns; i++ )
		{
			totalWidth += m_columnWidths[i]-m_styleWidths[i];
		}

		for( size_t i=0; i<numColumns; i++ )
		{
			widthPercent = double(m_columnWidths[i]-m_styleWidths[i])/double(totalWidth);
			m_columnWidths[i] = int(m_styleWidths[i]+widthPercent*double(maxWidth-totalStyleWidth));
		}

		totalWidth = maxWidth;
	}

	while( true )
	{
		try
		{
			size_t	numSubBoxes = m_subBoxes.size();
			for( size_t i = 0; i<numSubBoxes; i++ )
			{
				groupBox = m_subBoxes[i];

				groupHeight = groupBox->calcSize(
					context, left, top, maxWidth, windowWidth, windowHeight,
					&groupRect
				);

				XML_LINE		&theLine = m_theContent[i];
				XML_LINE_CHUNK	&theChunk = theLine.m_theLine[0];
				theChunk.setBlockElement( groupBox );
				theChunk.setPosition( left, top );
				theChunk.width = totalWidth;
				theLine.m_lineWidth = totalWidth;
				theLine.m_lineHeight = groupHeight;
				theLine.m_position.x = left;
				theLine.m_position.y = top;


				top += groupHeight;
				boxHeight += groupHeight;
			}
			break;
		}
		catch( ... )
		{
/*@*/		throw;
		}
	}

	m_docPosition.right = left+totalWidth;
	m_docPosition.bottom = top+boxHeight;
	*screen = m_docPosition;

	return boxHeight;
}

int XML_TABLE_GROUP_VIEWER_BOX::calcSize(
	Device &context, int left, int top, int maxWidth,
	int windowWidth, int windowHeight, RectBorder *screen
)
{

	doEnterFunctionEx(gakLogging::llDetail, "XML_TABLE_GROUP_VIEWER_BOX::calcSize");
	XML_VIEWER_BOX	*rowBox;
	RectBorder		rowRect;
	int				rowHeight, boxHeight = 0;

	m_docPosition.left = left;
	m_docPosition.top = top;
	m_docPosition.right = left+maxWidth;
	m_docPosition.bottom = top+windowHeight;

	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		try
		{
			rowBox = m_subBoxes[i];

			rowHeight = rowBox->calcSize(
				context, left, top, maxWidth, windowWidth, windowHeight,
				&rowRect
			);

			XML_LINE		&theLine = m_theContent[i];
			XML_LINE_CHUNK	&theChunk = theLine.m_theLine[0];
			theChunk.setBlockElement( rowBox );
			theChunk.setPosition( left, top );
			theChunk.width = maxWidth;
			theLine.m_lineWidth = maxWidth;
			theLine.m_lineHeight = rowHeight;
			theLine.m_position.x = left;
			theLine.m_position.y = top;

			top += rowHeight;
			boxHeight += rowHeight;
		}
		catch( ... )
		{
/*@*/		throw;
		}
	}

	m_docPosition.bottom = top+boxHeight;
	*screen = m_docPosition;

	return boxHeight;
}

int XML_TABLE_ROW_VIEWER_BOX::calcSize(
	Device &context, int left, int top, int maxWidth,
	int windowWidth, int windowHeight, RectBorder *screen
)
{

	doEnterFunctionEx(gakLogging::llDetail, "XML_TABLE_ROW_VIEWER_BOX::calcSize");
	XML_VIEWER_BOX	*cellBox;
	RectBorder		cellRect;
	int				cellHeight, columnWidth, innerWidth;
	int				maxHeight = 0;

	m_docPosition.left = left;
	m_docPosition.top = top;
	m_docPosition.right = left+maxWidth;
	m_docPosition.bottom = top+windowHeight;

	size_t		numSubBoxes = m_subBoxes.size();
	XML_LINE	&theLine = m_theContent[0];
	theLine.m_position.x = left;
	theLine.m_position.y = top;
	theLine.m_lineWidth = maxWidth;

	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		try
		{
			cellBox = m_subBoxes[i];

			columnWidth = getColumnWidth(cellBox->getItemNumber());
			innerWidth = cellBox->m_innerWidth;
			cellBox->m_innerWidth = columnWidth;
			cellHeight = cellBox->calcSize(
				context, left, top, columnWidth, windowWidth, windowHeight,
				&cellRect
			);
			cellBox->m_innerWidth = innerWidth;

			XML_LINE_CHUNK	&theChunk = theLine.m_theLine[i];
			theChunk.setBlockElement( cellBox );
			theChunk.setPosition( left, top );
			theChunk.width = columnWidth;


			left += columnWidth;
			if( maxHeight < cellHeight  )
				maxHeight = cellHeight;
		}
		catch( ... )
		{
			cellBox->m_innerWidth = innerWidth;
/*@*/		throw;
		}
	}

	theLine.m_lineHeight = maxHeight;

	m_docPosition.bottom = top+maxHeight;
	*screen = m_docPosition;

	return maxHeight;
}

int XML_TABLE_CELL_VIEWER_BOX::calcSize(
	Device &context, int left, int top, int maxWidth, int windowWidth, int windowHeight, RectBorder *screen
)
{
	doEnterFunctionEx(gakLogging::llDetail, "XML_TABLE_CELL_VIEWER_BOX::calcSize");
	int boxHeight;
	try
	{

		boxHeight = XML_VIEWER_BOX::calcSize(
			context, left, top, maxWidth, windowWidth, windowHeight, screen
		);
	}
	catch( ... )
	{
/*@*/	throw;
	}
	return boxHeight;
}

int XML_VIEWER_BOX::calcSize(
	Device &context, int left, int top, int maxWidth, int windowWidth, int windowHeight, RectBorder *screen
)
{
	doEnterFunctionEx(gakLogging::llDetail, "XML_VIEWER_BOX::calcSize");

	RectBorder		childBoxRect;
	int 			childBoxHeight, childBoxWidth;
	css::Styles		*childBoxStyle;
	XML_VIEWER_BOX	*childBox, *previousBox;

	int	right = left+maxWidth;
	int bottom = m_parentBox ? m_parentBox->m_docPosition.bottom : windowHeight;

	css::Styles *style = getCssStyle();

	STRING cssLeft =  style->getLeft();
	STRING cssRight =  style->getRight();
	STRING cssTop =  style->getTop();
	STRING cssBottom =  style->getBottom();

	if( m_position <= css::POS_STATIC )
	{
		// do normal layout
	}
	else if( m_position == css::POS_RELATIVE )
	{
		if( !cssLeft.isEmpty() )
			left = left + cssSizeToPixel( context, cssLeft, m_theElement );
		if( !cssTop.isEmpty() )
			top = top + cssSizeToPixel( context, cssTop, m_theElement );

		if( !cssRight.isEmpty() )
		{
			if( m_parentBox )
				right = m_parentBox->m_docPosition.right -
					cssSizeToPixel( context, cssRight, m_theElement )
				;
			else
				right = left + maxWidth -
					cssSizeToPixel( context, cssRight, m_theElement )
				;
		}
		if( !cssBottom.isEmpty() )
		{
			if( m_parentBox )
				bottom = m_parentBox->m_docPosition.bottom -
					cssSizeToPixel( context, cssBottom, m_theElement )
				;
			else
				bottom = top + windowHeight -
					cssSizeToPixel( context, cssBottom, m_theElement )
				;
		}
	}
	else if( m_position >= css::POS_ABSOLUTE )
	{
		if( !cssLeft.isEmpty() )
			left = cssSizeToPixel( context, cssLeft, m_theElement );
		if( !cssTop.isEmpty() )
			top = cssSizeToPixel( context, cssTop, m_theElement );

		if( !cssRight.isEmpty() )
		{
			if( m_parentBox )
				right = m_parentBox->m_docPosition.right -
					cssSizeToPixel( context, cssRight, m_theElement )
				;
			else
				right = left + maxWidth -
					cssSizeToPixel( context, cssRight, m_theElement )
				;
		}
		if( !cssBottom.isEmpty() )
		{
			if( m_parentBox )
				bottom = m_parentBox->m_docPosition.bottom -
					cssSizeToPixel( context, cssBottom, m_theElement )
				;
			else
				bottom = top + windowHeight -
					cssSizeToPixel( context, cssBottom, m_theElement )
				;
		}
	}

	if( cssLeft.isEmpty() || cssRight.isEmpty() )
	{
		if( m_innerWidth > 0 )
		{
			maxWidth = m_innerWidth +
				m_padding_left + m_margin_left + m_padding_right + m_margin_right
			;
		}
		else if( m_innerWidthPercent > 0 )
		{
			maxWidth =
				((m_parentBox
					? m_parentBox->m_maxWidth
					: maxWidth
				) * m_innerWidthPercent) /100
			;
		}
		if( cssLeft.isEmpty() && !cssRight.isEmpty() )
			left = right-maxWidth;
		else
			right = left+maxWidth;
	}

	if( cssTop.isEmpty() || cssBottom.isEmpty() )
	{
		if( m_innerHeight > 0 )
		{
			m_maxHeight = m_innerHeight +
				m_padding_top + m_margin_top + m_padding_bottom + m_margin_bottom
			;
			if( m_border_style_top > css::BORD_NONE )
				m_maxHeight += m_border_width_top;
			if( m_border_style_bottom > css::BORD_NONE )
				m_maxHeight += m_border_width_bottom;
		}
		else if( m_innerHeightPercent > 0 )
		{
			m_maxHeight =
				((m_parentBox
					? m_parentBox->m_maxHeight
					: windowHeight
				) * m_innerHeightPercent) /100
			;
			if( m_border_style_top > css::BORD_NONE )
				m_maxHeight += m_border_width_top;
			if( m_border_style_bottom > css::BORD_NONE )
				m_maxHeight += m_border_width_bottom;
		}
		else
			m_maxHeight = windowHeight;

		if( cssTop.isEmpty() && !cssBottom.isEmpty() )
			top = bottom-m_maxHeight;
		else
			bottom = top+m_maxHeight;
	}

	m_docPosition.left = left;
	m_docPosition.top = top;
	m_docPosition.right = right;
	m_docPosition.bottom = bottom;

	Size	size;
	int		lineHeight;

	int innerLeft = left + m_padding_left + m_margin_left;
	if( m_border_style_left > css::BORD_NONE )
		innerLeft += m_border_width_left;

	int innerRight = right - m_padding_right - m_margin_right;
	if( m_border_style_right > css::BORD_NONE )
		innerRight -= m_border_width_right;

	int innerTop = top + m_padding_top + m_margin_top;
	if( m_border_style_top > css::BORD_NONE )
		innerTop += m_border_width_top;

	int innerWidth = maxWidth = m_maxWidth = innerRight - innerLeft;

	if( maxWidth < 0 )
	{
/*@*/	throw ResizeException( -maxWidth );
	}
	int x = innerLeft;
	int y = innerTop;
	int	maxRight = innerLeft;
	bool	autoWrap = true;
	bool	preserveBreaks = false;
	bool	preserveBlanks = false;
	size_t	newLinePos;

	css::TextAlign	text_align = style->getTextAlign();

	m_leftBoxes.clear();
	m_rightBoxes.clear();
	m_theContent.clear();

#if DEBUG_LOG
	STRING	tag = m_theElement->getTag();
	if( tag == "long" )
	{
		doLogValue( tag );
	}
#endif
	size_t	i=0, numElements = m_childElements.size();
	size_t	numLines = 0;
	size_t	chunkLen, wrapPos = 0, numChunks;

	/*
		=====================================================================
		walk through all child elements and calculate their size and position
		=====================================================================
	*/
	while( i<numElements )
	{
		// **************************
		// check inline text elements
		// **************************
		lineHeight = 0;
		while( i<numElements )
		{
			XML_VIEWER_ITEM		&theItem = m_childElements[i];
			xml::Element			*theElement = theItem.getXmlElement();

			if( theElement )
			{
				autoWrap = theItem.getAutoWrap();
				preserveBreaks = theItem.getPreserveBreaks();
				preserveBlanks = theItem.getPreserveBlanks();

				xml::XmlText *theText = dynamic_cast<xml::XmlText *>(theElement);
				if( theText )
				{
					XML_LINE	&currentLine = m_theContent[numLines];

					numChunks = currentLine.m_theLine.size();

					if( !numChunks )
					{
						if( !numLines )
						{
							int text_indent = cssSizeToPixel(
								context, style->getTextIndent(), theElement
							 );
							x += text_indent;
							maxWidth -= text_indent;
						}
						currentLine.m_position.x = x;
						currentLine.m_position.y = y;
						currentLine.m_maxWidth = maxWidth;
					}

					XML_LINE_CHUNK	&newChunk = currentLine.m_theLine[
						numChunks
					];

					newChunk.setPosition( x, y );
					newChunk.setText( theText, theItem.getXmlTextPtr() );

					if( !preserveBlanks
					&& !numChunks
					&& isspace( newChunk.getText()[wrapPos] ) )
						wrapPos++;

					newChunk.startPos = wrapPos;

					createFont( context, theText, false );

					lineHeight = getLineHeight( context, theText );
					if( lineHeight > currentLine.m_lineHeight )
						currentLine.m_lineHeight = lineHeight;
					else
						lineHeight = currentLine.m_lineHeight;

					const STRING &text = newChunk.getText();
					if( preserveBreaks )
					{
						newLinePos = text.searchChar( '\n', wrapPos );
						if( newLinePos != -1 )
						{
							chunkLen = newLinePos - wrapPos;
						}
						else
							chunkLen = text.strlen()-wrapPos;
					}
					else
						chunkLen = text.strlen()-wrapPos;

					context.getTextExtent(
						text,
						wrapPos, chunkLen,
						&size
					);
					if( currentLine.m_lineWidth + size.width > maxWidth )
					{
						if( !autoWrap )
						{
/*@*/						throw ResizeException(
								currentLine.m_lineWidth + size.width - maxWidth
							);
						}
						newChunk.width = findWrapPos(
							context, maxWidth-currentLine.m_lineWidth, !numChunks,
							newChunk.getText(), wrapPos, &chunkLen
						);

						// we did not get space for even one character
						if( !newChunk.width && !currentLine.m_lineWidth )
						{
							context.getTextExtent(
								text,
								wrapPos, 1,
								&size
							);

/*@*/						throw ResizeException( size.width-maxWidth );
						}
						newChunk.len = chunkLen;
						currentLine.m_lineWidth += newChunk.width;

						maxRight = innerRight;

						y += currentLine.m_lineHeight;
						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;

						wrapPos += chunkLen;
						if( isspace( newChunk.getText()[wrapPos] ) )
							wrapPos++;
						numLines++;
						lineHeight = 0;
					}
					else
					{
						x += size.width;
						if( maxRight<x )
							maxRight=x;

						newChunk.width = size.width;
						newChunk.len = chunkLen;
						currentLine.m_lineWidth += size.width;

						if( !preserveBreaks || newLinePos == -1 )
						{
							i++;
							wrapPos = 0;
						}
						else
						{
							y += currentLine.m_lineHeight;
							x = findLeft( y, innerLeft );
							maxWidth = findRight( y, innerRight ) - x;
							numLines++;
							lineHeight = 0;

							wrapPos += chunkLen+1;
							if( wrapPos >= text.strlen() )
							{
								i++;
								wrapPos = 0;
							}
						}
					}
				}
				else
					break;
			}
			else
				break;
		}

		// *****************************
		// check (inline) block elements
		// *****************************
		previousBox = nullptr;
		while( i<numElements )
		{
			XML_VIEWER_ITEM	&theItem = m_childElements[i];

			childBox = theItem.getViewerBox();

			if( childBox )
			{
				i++;
				childBoxStyle = childBox->getCssStyle();
				css::Display	display = childBoxStyle->getDisplay();
				if( display == css::DSP_BLOCK
				|| display == css::DSP_TABLE
				|| display == css::DSP_LIST_ITEM )
				{
					if( lineHeight )
					{
						// we need a new line
						numLines++;
						y += lineHeight;
						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;
						lineHeight = 0;
					}
					if( previousBox )
					{
						// merge the margins
						int maxMargin = math::max(
							previousBox->m_margin_bottom, childBox->m_margin_top
						);
						childBox->m_margin_top = maxMargin
							- previousBox->m_margin_bottom
						;
					}
					previousBox = childBox;
				}
				else
					previousBox = nullptr;

				childBoxHeight = childBox->calcSize(
					context, x, y, maxWidth, windowWidth, windowHeight,
					&childBoxRect
				);
				childBoxWidth = childBoxRect.right - childBoxRect.left;
				if( childBoxWidth > innerWidth )
				{
/*@*/				throw ResizeException( childBoxWidth - innerWidth );
				}
				if( display == css::DSP_INLINE_BLOCK
				|| display == css::DSP_INLINE_TABLE )
				{
					XML_LINE	&currentLine = m_theContent[numLines];

					if( childBoxWidth <= maxWidth-currentLine.m_lineWidth )
					{
						// add this block to the current line

						numChunks = currentLine.m_theLine.size();

						if( !numChunks )
						{
							currentLine.m_position.x = x;
							currentLine.m_position.y = y;
							currentLine.m_maxWidth = maxWidth;
						}
						XML_LINE_CHUNK	&newChunk = currentLine.m_theLine[
							numChunks
						];

						newChunk.setPosition( x, y );
						newChunk.width = childBoxWidth;
						newChunk.setBlockElement( childBox );
						if( currentLine.m_lineHeight < childBoxHeight )
							currentLine.m_lineHeight = childBoxHeight;
						lineHeight = currentLine.m_lineHeight;
						currentLine.m_lineWidth += childBoxWidth;
					}
					else
					{
						// the box is too large for this position move it down
						y += currentLine.m_lineHeight;
						y = findTop( y, childBoxWidth, innerLeft, innerRight );
						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;

						childBoxRect.left = x;
						childBoxRect.top = y;
						childBoxRect.right = x+childBoxWidth;
						childBoxRect.bottom = y + childBoxHeight;

						childBox->moveBy(
							childBoxRect.left - childBox->m_docPosition.left,
							childBoxRect.top - childBox->m_docPosition.top
						);

						// add this block to a new line
						XML_LINE	&currentLine = m_theContent[++numLines];
						currentLine.m_position.x = x;
						currentLine.m_position.y = y;
						currentLine.m_maxWidth = maxWidth;
						currentLine.m_lineHeight = childBoxHeight;
						currentLine.m_lineWidth = childBoxWidth;
						lineHeight = childBoxHeight;

						XML_LINE_CHUNK	&newChunk = currentLine.m_theLine[0];
						newChunk.setPosition( x, y );
						newChunk.setBlockElement( childBox );
						newChunk.width = childBoxWidth;
					}
					x += childBoxWidth;

				}
				else
				{
					if( childBoxWidth > maxWidth )
					{
						// the box is too large for this position move it down
						y = findTop( y, childBoxWidth, innerLeft, innerRight );
						childBoxRect.top = y;
						childBoxRect.bottom = y + childBoxHeight;
						childBox->moveDown(
							childBoxRect.top - childBox->m_docPosition.top
						);
					}

					css::Float cssFloat = childBoxStyle->getCssFloat();
					if( cssFloat > css::FLT_NONE )
					{
						if( cssFloat == css::FLT_LEFT )
						{
							x = findLeft( y, innerLeft );
							if( x != childBoxRect.left )
							{
								childBoxRect.left = x;
								childBoxRect.right = x+childBoxWidth;
								childBox->moveRight(
									childBoxRect.left - childBox->m_docPosition.left
								);
							}
							m_leftBoxes += childBoxRect;
						}
						else if( cssFloat == css::FLT_RIGHT )
						{
							x = findRight( y, innerRight );
							if( x != childBoxRect.right )
							{
								childBoxRect.right = x;
								childBoxRect.left = x - childBoxWidth;
								childBox->moveRight(
									childBoxRect.left - childBox->m_docPosition.left
								);
							}
							m_rightBoxes += childBoxRect;
						}


						if( childBoxRect.right > maxRight )
							maxRight = childBoxRect.right;

						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;
					}
					else
					{
						// add this block to a new line

						XML_LINE		&newLine = m_theContent[numLines++];
						XML_LINE_CHUNK	&newChunk = newLine.m_theLine[
							0
						];

						newLine.m_position.x = x;
						newLine.m_position.y = y;
						newLine.m_maxWidth = maxWidth;

						newChunk.setPosition( x, y );
						newChunk.setBlockElement( childBox );
						newLine.m_lineHeight = childBoxHeight;
						newChunk.width = newLine.m_lineWidth = childBoxWidth;

						if( childBoxRect.right > maxRight )
							maxRight = childBoxRect.right;

						y += childBoxHeight;
					}
				}
			}
			else
				break;
		}
	}

	/*
		do horizontal alignment for content lines
	*/
	numLines = m_theContent.size();
	if( text_align == css::ALIGN_CENTER || text_align == css::ALIGN_RIGHT )
	{
		for( size_t i=0; i<numLines; i++ )
		{
			XML_LINE	&currentLine = m_theContent[i];
			int 		offset = findRight( currentLine.m_position.y, innerRight );
			offset -= currentLine.m_position.x;
			offset -= currentLine.m_lineWidth;

			if( text_align == css::ALIGN_CENTER )
				offset /= 2;
			currentLine.moveRight( offset );
		}
	}

	/*
		do vertical alignment for inline boxes
	*/
	for( size_t i=0; i<numLines; i++ )
	{
		XML_LINE	&currentLine = m_theContent[i];
		for( size_t j=0; j<currentLine.m_theLine.size(); j++ )
		{
			XML_LINE_CHUNK	&theChunk = currentLine.m_theLine[j];
			if( theChunk.isBlockElement() )
			{
				childBoxHeight =
					theChunk.getInlineBox()->m_docPosition.bottom -
					theChunk.getInlineBox()->m_docPosition.top
				;
				if( childBoxHeight < currentLine.m_lineHeight )
				{
					theChunk.moveBy( 0, currentLine.m_lineHeight - childBoxHeight );
				}

			}
		}
	}

	int maxBottom = y;
	if( numLines )
	{
		for( size_t i = numLines-1; i!= -1; i-- )
		{
			XML_LINE	&currentLine = m_theContent[i];
			if( !currentLine.m_lineHeight
			|| !currentLine.m_theLine.size() )
			{
				m_theContent.removeElementAt( i );
				numLines--;
			}
		}
	}

	if( numLines )
	{
		XML_LINE	&currentLine = m_theContent[numLines-1];
		if( maxBottom < currentLine.m_position.y + currentLine.m_lineHeight )
		{
			maxBottom = currentLine.m_position.y + currentLine.m_lineHeight;
		}
	}
	for( size_t i=0; i<m_leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_leftBoxes[i];
		if( boxRect.bottom > maxBottom )
			maxBottom = boxRect.bottom;
	}
	for( size_t i=0; i<m_rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = m_rightBoxes[i];
		if( boxRect.bottom > maxBottom )
			maxBottom = boxRect.bottom;
	}
	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		XML_VIEWER_BOX	*subBox = m_subBoxes[i];
		subBox->calcSize(
			context,
			0, 0, windowWidth, windowWidth, windowHeight,
			&childBoxRect
		);
		if( (subBox->m_position == css::POS_ABSOLUTE || subBox->m_display >= css::DSP_TABLE)  
		&& childBoxRect.bottom > maxBottom )
			maxBottom = childBoxRect.bottom;
	}

	int boxHeight = maxBottom - innerTop;

	boxHeight += m_padding_top + m_margin_top + m_padding_bottom + m_margin_bottom;
	if( m_border_style_top > css::BORD_NONE )
		boxHeight += m_border_width_top;
	if( m_border_style_bottom > css::BORD_NONE )
		boxHeight += m_border_width_bottom;

	if( m_innerWidth <= 0
	&& m_innerWidthPercent <= 0
	&& cssRight.isEmpty()
	&& (style->getCssFloat() > css::FLT_NONE || m_display == css::DSP_CELL) )
		m_docPosition.right = maxRight + m_padding_right + m_margin_right;

	if( m_innerHeight <= 0 && m_innerHeightPercent <= 0 && cssBottom.isEmpty() )
		m_docPosition.bottom = m_docPosition.top + boxHeight;

	*screen = m_docPosition;

	return boxHeight;
}

/*
--------------------------------------------------------------------------------
	drawing functions
	these functions draw the content 
--------------------------------------------------------------------------------
*/
void XML_VIEWER_BOX::drawLine(
	Device &context,
	XMLeditorChild *xmlEditorWindow,
	XML_LINE &theLine,
	bool clearScreen
)
{
	Size	size;

	int vertOffset, horizOffset;

	if( m_position == css::POS_FIXED )
		vertOffset = horizOffset = 0;
	else
	{
		vertOffset = xmlEditorWindow->getVertOffset();
		horizOffset = xmlEditorWindow->getHorizOffset();
	}

	if( clearScreen )
	{
		css::Styles		*cssStyle;
		css::Color		color;
		bool			colorFound = false;
		xml::Element	*theElement = m_theElement;
		while( theElement )
		{
			cssStyle = theElement->getCssStyle();
			colorFound = cssStyle->getBackgroundColor( &color );
			if( colorFound )
				break;
			theElement = theElement->getParent();
		}
		if( colorFound )
			context.getBrush().create( color.red, color.green, color.blue );
		else
			context.getBrush().create( 255, 255, 255 );
		context.getPen().selectPen( Pen::spNull );
		context.rectangle(
			theLine.m_position.x-horizOffset,
			theLine.m_position.y-vertOffset,
			theLine.m_position.x-horizOffset+theLine.m_maxWidth+1,
			theLine.m_position.y-vertOffset+theLine.m_lineHeight+1
		);
	}

	for( size_t i=0; i<theLine.m_theLine.size(); i++ )
	{
		XML_LINE_CHUNK	&theChunk = theLine.m_theLine[i];
		xml::XmlText	*theText = theChunk.getTextElement();
		if( theText )
		{
			STRING text = theChunk.getText().subString(
				theChunk.startPos, theChunk.len
			);

			createFont( context, theText, true );
			context.getTextExtent( text, text.strlen(), &size );

			context.moveTo(
				theChunk.getPosition().x-horizOffset,
				theChunk.getPosition().y+theLine.m_lineHeight-size.height-vertOffset
			);
			context.printText( text );
		}
		else if( clearScreen )
		{
			theChunk.getInlineBox()->draw( context, xmlEditorWindow );
		}
	}
}

void XML_VIEWER_BOX::draw( Device &context, XMLeditorChild *xmlEditorWindow )
{
	doEnterFunctionEx(gakLogging::llDetail, "XML_VIEWER_BOX::draw");

	Size	size;
	STRING	text;

	STRING	wrapText;

	int vertOffset, horizOffset;

	if( m_position == css::POS_FIXED )
		vertOffset = horizOffset = 0;
	else
	{
		vertOffset = xmlEditorWindow->getVertOffset();
		horizOffset = xmlEditorWindow->getHorizOffset();
	}

	int left = m_docPosition.left;
	int right = m_docPosition.right;
	int top = m_docPosition.top;
	int bottom = m_docPosition.bottom;

	int borderLeft = left + m_margin_left;
	int borderRight = right - m_margin_right;
	int borderTop = top + m_margin_top;
	int borderBottom = bottom - m_margin_bottom;


	if( m_theElement )
	{
		css::Styles	*cssStyle = getCssStyle();

		css::Color	color;
		if( cssStyle->getBackgroundColor( &color ) )
		{
			context.getPen().selectPen( Pen::spNull );
			context.getBrush().create( color.red, color.green, color.blue );
			context.rectangle(
				borderLeft-horizOffset, borderTop-vertOffset,
				borderRight-horizOffset+1, borderBottom-vertOffset+1
			);
		}
		if( m_border_style_left > css::BORD_HIDDEN && m_border_width_left > 0 )
		{
			cssStyle->getBorderColorLeft( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( m_border_width_left, false ).setStyle( m_border_style_left, true )
			;
			context.verticalLine(
				borderLeft-horizOffset+m_border_width_left/2,
				borderTop-vertOffset,
				borderBottom-vertOffset
			);
		}
		if( m_border_style_right > css::BORD_HIDDEN && m_border_width_right > 0 )
		{
			cssStyle->getBorderColorRight( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( m_border_width_right, false )
				.setStyle( m_border_style_right, true )
			;
			context.verticalLine(
				borderRight-horizOffset-m_border_width_right/2,
				borderTop-vertOffset,
				borderBottom-vertOffset
			);
		}
		if( m_border_style_top > css::BORD_HIDDEN && m_border_width_top > 0 )
		{
			cssStyle->getBorderColorTop( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( m_border_width_top, false )
				.setStyle( m_border_style_top, true )
			;
			context.horizontalLine(
				borderLeft-horizOffset,
				borderRight-horizOffset,
				borderTop-vertOffset+m_border_width_top/2
			);
		}
		if( m_border_style_bottom > css::BORD_HIDDEN && m_border_width_bottom > 0 )
		{
			cssStyle->getBorderColorBottom( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( m_border_width_bottom, false )
				.setStyle( m_border_style_bottom, true )
			;
			context.horizontalLine(
				borderLeft-horizOffset,
				borderRight-horizOffset,
				borderBottom-vertOffset-m_border_width_bottom/2
			);
		}

		// draw the curcle for list items
		if( m_display == css::DSP_LIST_ITEM && m_listStyle > css::LIST_NONE )
		{
			if( m_listStyle <= css::LIST_SQUARE )
			{
				if( cssStyle->getColor( &color ) )
				{
					context.getBrush().create( color.red, color.green, color.blue );
					context.getPen().setColor( color.red, color.green, color.blue, false );
				}
				else
				{
					context.getBrush().create( 0, 0, 0 );
					context.getPen().setColor( color.red, color.green, color.blue, false );
				}
				if( m_listStyle == css::LIST_DISC )
				{
					context.getPen().selectPen( Pen::spNull );
					context.ellipse( left-16-horizOffset, top-vertOffset, left-8-horizOffset, top+8-vertOffset );
				}
				else if( m_listStyle == css::LIST_CIRCLE )
				{
					context.getPen().setWidth( 1, false );
					context.getPen().setStyle( Pen::psSolid, true );

					context.getBrush().selectBrush( Brush::sbNull );
					context.ellipse( left-16-horizOffset, top-vertOffset, left-8-horizOffset, top+8-vertOffset );
				}
				else if( m_listStyle == css::LIST_SQUARE )
				{
					context.getPen().setWidth( 1, false );
					context.getPen().setStyle( Pen::psSolid, true );

					context.getBrush().selectBrush( Brush::sbNull );
					context.rectangle( left-16-horizOffset, top-vertOffset, left-8-horizOffset, top+8-vertOffset );
				}
			}
			else
			{
				Size	size;
				char	tmpBuffer[16];
				int		listNumber = m_itemNumber+1;

				createFont( context, m_theElement, true );
				if( m_listStyle == css::LIST_ZERO_DECIMAL )
				{
					sprintf( tmpBuffer, "%02d. ", listNumber%100 );
				}
				else if( m_listStyle == css::LIST_UPPER )
				{
					sprintf( tmpBuffer, "%c. ", 'A'-1 + listNumber%26 );
				}
				else if( m_listStyle == css::LIST_LOWER )
				{
					sprintf( tmpBuffer, "%c. ", 'a'-1 + listNumber%26 );
				}
				else
				{
					sprintf( tmpBuffer, "%d. ", listNumber%100 );
				}
				context.getTextExtent( tmpBuffer, strlen(tmpBuffer), &size );
				context.moveTo( left-size.width-horizOffset, top-vertOffset );
				context.clrBackgroundColor();
				context.printText( tmpBuffer );
			}
		}
		for( size_t i=0; i<m_theContent.size(); i++ )
		{
			XML_LINE	&theLine = m_theContent[i];
			drawLine( context, xmlEditorWindow, theLine, false );
		}

	}

	size_t numChildBoxes = m_childElements.size();
	for( size_t i=0; i<numChildBoxes; i++ )
	{
		XML_VIEWER_ITEM	&theItem = m_childElements[i];
		XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

		if( theBox )  
		{
			RectBorder	screenPosition;
			theBox->getScreenPosition( vertOffset, horizOffset, &screenPosition );

			if( screenPosition.bottom > 0
			&& screenPosition.top < xmlEditorWindow->getWindowHeight()
			&& screenPosition.right > 0
			&& screenPosition.left < xmlEditorWindow->getWindowWidth() )
			{
				theBox->draw( context, xmlEditorWindow );
			}
		}
	}

	if( xmlEditorWindow->isXmlFrameEnabled() )
	{
		context.getBrush().selectBrush( Brush::sbNull );
		context.getPen().selectPen( Pen::spBlack );
		context.rectangle(
			left-horizOffset, top-vertOffset,
			right-horizOffset, bottom-vertOffset
		);
		context.setDefaultFont();
		context.setTextColor( 0, 0, 0 );
		context.setBackgroundColor( 255, 255, 200 );
		text = m_theElement ? m_theElement->getTag() : STRING("auto");
		context.getTextExtent( text, text.strlen(), &size );

		context.moveTo( left-horizOffset, top-vertOffset );
		context.printText( text );

		context.moveTo( left-horizOffset, bottom-size.height-vertOffset );
		context.printText( text );

		context.moveTo( right-size.width-horizOffset, bottom-size.height-vertOffset );
		context.printText( text );

		context.moveTo( right-size.width-horizOffset, top-vertOffset );
		context.printText( text );
	}

	size_t	numSubBoxes = m_subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		m_subBoxes[i]->draw( context, xmlEditorWindow );
	}
}

/*
--------------------------------------------------------------------------------
	BOX Tree functions
	these functions build the box tree
--------------------------------------------------------------------------------
*/
XML_VIEWER_BOX_PTR XML_VIEWER_BOX::createBox(
	css::Display childDisplay,
	int itemNumber
)
{
	return (childDisplay >= css::DSP_INLINE_TABLE)
		? new XML_TABLE_VIEWER_BOX(itemNumber)
		: new XML_VIEWER_BOX(itemNumber)
	;
}

XML_VIEWER_BOX_PTR XML_TABLE_VIEWER_BOX::createBox(
	css::Display ,
	int itemNumber
)
{
	return new XML_TABLE_GROUP_VIEWER_BOX(itemNumber);
}

XML_VIEWER_BOX_PTR XML_TABLE_GROUP_VIEWER_BOX::createBox(
	css::Display ,
	int itemNumber
)
{
	return new XML_TABLE_ROW_VIEWER_BOX(itemNumber);
}

XML_VIEWER_BOX_PTR XML_TABLE_ROW_VIEWER_BOX::createBox(
	css::Display ,
	int itemNumber
)
{
	return new XML_TABLE_CELL_VIEWER_BOX(itemNumber);
}

void XML_VIEWER_BOX::createBox(
	const Device &context, xml::Element *theRoot,
	css::Display childDisplay, css::Position childPosition,
	int itemNumber
)
{
	if( theRoot->getTag().isEmpty()
	&& theRoot->getValue( xml::PLAIN_MODE ).stripBlanks().isEmpty() )
/*@*/	return;

	XML_VIEWER_BOX_PTR	childBox = createBox( childDisplay, itemNumber );

	if( childPosition >= css::POS_ABSOLUTE
	||	childDisplay > css::DSP_TABLE
	||	(m_display >= css::DSP_INLINE_TABLE && m_display < css::DSP_CELL) )
	{
		m_subBoxes += childBox;
	}
	else
	{
		XML_VIEWER_ITEM	&theItem = m_childElements.createElement();
		theItem.setViewerBox( childBox );
	}

	childBox->buildBoxTree( context, theRoot, this );
}

void XML_VIEWER_BOX::findBoxes( const Device &context, xml::Element *theElement )
{
	xml::Element	*xmlChild;
	css::Styles		*childStyle;
	css::Display	childDisplay;
	css::Position	childPosition;

	int				itemNumber = 0;

	xml::XmlText	*theText = dynamic_cast<xml::XmlText *>(theElement);
	if( theText )
	{
		bool			autoWrap, preserveBlanks, preserveBreaks;
		STRING			value = theText->getValue( xml::PLAIN_MODE );
		css::WhiteSpace	whiteSpace = m_theElement->getCssStyle()->getWhiteSpace();

		switch( whiteSpace )
		{
			case css::WS_NORMAL:
				autoWrap = true;
				preserveBlanks = preserveBreaks = false;
				break;
			case css::WS_NOWRAP:
				autoWrap = false;
				preserveBlanks = preserveBreaks = false;
				break;
			case css::WS_PRE_WRAP:
				autoWrap = true;
				preserveBlanks = preserveBreaks = true;
				break;
			case css::WS_PRE:
				autoWrap = false;
				preserveBlanks = preserveBreaks = true;
				break;
			case css::WS_PRE_LINE:
				autoWrap = true;
				preserveBlanks = false;
				preserveBreaks = true;
				break;
		}

		value = prepareText( value, preserveBlanks, preserveBreaks );
		XML_VIEWER_ITEM	&theItem = m_childElements.createElement();
		theItem.setXmlElement(
			theText, value, autoWrap, preserveBlanks, preserveBreaks
		);
	}
	else
	{
		size_t	numElements = theElement->getNumObjects();
		for( size_t i=0; i<numElements; i++ )
		{
			xmlChild = theElement->getElement( i );
			if( xmlChild->isVisual() )
			{
				childStyle = xmlChild->getCssStyle();
				childPosition = childStyle->getPosition();
				childDisplay = childStyle->getDisplay();

				if( childPosition >= css::POS_ABSOLUTE
				|| childDisplay >= css::DSP_INLINE_BLOCK
				|| childStyle->getCssFloat() > css::FLT_NONE
				|| (m_display >= css::DSP_INLINE_TABLE && m_display < css::DSP_CELL)   )
				{
					createBox( context, xmlChild, childDisplay, childPosition, itemNumber );
					if( childDisplay >= css::DSP_LIST_ITEM )
						itemNumber++;
				}
				else
				{
					findBoxes( context, xmlChild );
				}
			}
		}
	}
}

void XML_TABLE_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	css::Styles		*theStyle = theRoot->getCssStyle();
	css::Display	display = theStyle->getDisplay();

	if( display > css::DSP_TABLE )
	{
		/*
			if the element is a part of a table but not a table itself
			we are an anonymous element - it's me
		*/

		m_theElement = nullptr;
		m_parentBox = container;
		m_display = css::DSP_TABLE;

		createBox( context, theRoot, display, theStyle->getPosition(), m_itemNumber );
	}
	else if( display >= css::DSP_INLINE_TABLE && display <= css::DSP_TABLE )
	{
		/*
			this element is either a table or an inline-table
		*/
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
		m_display = display;
	}
	else
		; // ignore this item (should never occure)
}

void XML_TABLE_GROUP_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	css::Styles		*theStyle = theRoot->getCssStyle();
	css::Display	display = theStyle->getDisplay();

	if( display > css::DSP_FOOTER_GROUP || display < css::DSP_HEADER_GROUP )
	{
		// we need an anonymous element - it's me
		m_theElement = nullptr;
		m_parentBox = container;
		m_display = css::DSP_ROW_GROUP;
		createBox( context, theRoot, display, theStyle->getPosition(), m_itemNumber );
	}
	else
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
}

void XML_TABLE_ROW_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	css::Styles		*theStyle = theRoot->getCssStyle();
	css::Display	display = theStyle->getDisplay();
	if( display != css::DSP_ROW )
	{
		// we need an anonymous element - it's me
		m_theElement = nullptr;
		m_parentBox = container;
		m_display = css::DSP_ROW;
		createBox( context, theRoot, display, theStyle->getPosition(), m_itemNumber );
	}
	else
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
}

void XML_TABLE_CELL_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	css::Styles		*theStyle = theRoot->getCssStyle();
	css::Display	display = theStyle->getDisplay();

	m_parentBox = container;
	initColumn();


	if( display >= css::DSP_INLINE_TABLE && display < css::DSP_CELL )
	{
		// we need an anonymous element - it's me
		m_theElement = nullptr;
		m_display = css::DSP_CELL;
		createBox( context, theRoot, display, theStyle->getPosition(), m_itemNumber );
	}
	else
	{
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
		m_display = css::DSP_CELL;
	}
}

void XML_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	m_theElement = theRoot;
	m_parentBox = container;

	css::Styles *cssStyle = theRoot->getCssStyle();

	m_margin_left = cssSizeToPixel( context, cssStyle->getMarginLeft(), theRoot );
	m_margin_right = cssSizeToPixel(
		context, cssStyle->getMarginRight(), theRoot
	);
	m_margin_top = cssSizeToPixel( context, cssStyle->getMarginTop(), theRoot );
	m_margin_bottom = cssSizeToPixel(
		context, cssStyle->getMarginBottom(), theRoot
	);

	m_padding_left = cssSizeToPixel(
		context, cssStyle->getPaddingLeft(), theRoot
	);
	m_padding_right = cssSizeToPixel(
		context, cssStyle->getPaddingRight(), theRoot
	);
	m_padding_top = cssSizeToPixel(
		context, cssStyle->getPaddingTop(), theRoot
	);
	m_padding_bottom = cssSizeToPixel(
		context, cssStyle->getPaddingBottom(), theRoot
	);

	STRING cssValue = cssStyle->getWidth();

	if( cssValue.isEmpty() )
	{
		m_innerWidth = -1;
		m_innerWidthPercent = -1;
	}
	else if( cssValue.endsWith( '%' ) )
	{
		m_innerWidth = -1;
		m_innerWidthPercent = atoi( cssValue );
	}
	else
	{
		m_innerWidthPercent = -1;
		m_innerWidth = cssSizeToPixel( context, cssValue, theRoot );
	}

	cssValue = cssStyle->getHeight();

	if( cssValue.isEmpty() )
	{
		m_innerHeight = -1;
		m_innerHeightPercent = -1;
	}
	else if( cssValue.endsWith( '%' ) )
	{
		m_innerHeight = -1;
		m_innerHeightPercent = atoi( cssValue );
	}
	else
	{
		m_innerHeightPercent = -1;
		m_innerHeight = cssSizeToPixel( context, cssValue, theRoot );
	}

	m_position = cssStyle->getPosition();
	m_display = cssStyle->getDisplay();
	m_listStyle = cssStyle->getListStyle();

	m_border_style_left = cssStyle->getBorderStyleLeft();
	m_border_style_right = cssStyle->getBorderStyleRight();
	m_border_style_top = cssStyle->getBorderStyleTop();
	m_border_style_bottom = cssStyle->getBorderStyleBottom();

	m_border_width_left = cssSizeToPixel(
		context, cssStyle->getBorderWidthLeft(), theRoot
	);
	m_border_width_right = cssSizeToPixel(
		context, cssStyle->getBorderWidthRight(), theRoot
	);
	m_border_width_top = cssSizeToPixel(
		context, cssStyle->getBorderWidthTop(), theRoot
	);
	m_border_width_bottom = cssSizeToPixel(
		context, cssStyle->getBorderWidthBottom(), theRoot
	);

	findBoxes( context, theRoot );

	// now we walk throup all items an remove blank lines
	bool	removeBlanks = true;
	bool	blankFound = false;
	size_t	blankIndex = 0;
	size_t 	i=0;
	size_t 	numElements = m_childElements.size();
	STRING	xmlText;

	while( i<numElements )
	{
		XML_VIEWER_ITEM		&theItem = m_childElements[i];
		xml::Element			*theElement = theItem.getXmlElement();

		if( theElement )
		{
			xmlText = theItem.getXmlText();
			xmlText.stripBlanks();
			if( xmlText.isEmpty() )
			{
				if( removeBlanks )
				{
					m_childElements.removeElementAt( i );
					numElements--;
					blankFound = false;
				}
				else
				{
					removeBlanks = true;
					blankFound = true;
					blankIndex = i++;
				}
			}
			else
			{
				/*
					the text is not empty -> preserve the next blank
				*/
				removeBlanks = false;
				blankFound = false;
				i++;
			}
		}
		else
		{
			if( blankFound )
			{
				m_childElements.removeElementAt( blankIndex );
				numElements--;
				blankFound = false;
				removeBlanks = true;
			}
			else
				i++;
		}
	}

}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace internal
}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

