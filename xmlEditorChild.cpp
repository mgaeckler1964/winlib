/*
		Project:		Windows Class Library
		Module:			xmlEditorChild.cpp
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

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
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

class ResizeException
{
	int incWidth;

	public:
	ResizeException( int incWidth )
	{
		this->incWidth = incWidth;
	}
	int getIncWidth( void ) const
	{
		return incWidth;
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

const char XMLeditorChild::className[] = "XMLeditorChild";

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
	position.x += x;
	position.y += y;
	if( theInlineBlockBox )
		theInlineBlockBox->moveBy( x, y );
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void XMLeditorChild::registerClass( void )
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		fillDefaultClass( &wc );
		wc.style			|= CS_DBLCLKS;
		wc.lpszClassName	 = className;

		registered = ChildWindow::registerClass( &wc );
	}
}

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

	const XML_LINE	&theLine = theContent[line];
	size_t			numChunks = theLine.theLine.size();

	for( size_t chunk = 0; chunk <numChunks; chunk++ )
	{
		const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];

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

	if( !found && screenPosition < theLine.position.x )
	{
		// set the cursor to the first text chunk we found
		for( size_t chunk = 0; chunk <numChunks; chunk++ )
		{
			const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];

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
	if( !found && screenPosition > theLine.position.x+theLine.lineWidth )
	{
		// set the cursor to the last text chunk we found
		for( size_t chunk = numChunks-1; chunk != -1; chunk-- )
		{
			const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];

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

	while( leftRectIdx < leftBoxes.size()
	|| rightRectIdx < rightBoxes.size() )
	{
		leftRectIdx = findLeftBox( y );
		rightRectIdx = findRightBox( y );


		if( leftRectIdx < leftBoxes.size() )
		{
			const RectBorder &leftRect = leftBoxes[leftRectIdx];
			leftBorder = leftRect.right;
			nextY = leftRect.bottom;
		}

		if( rightRectIdx < rightBoxes.size() )
		{
			const RectBorder &rightRect = rightBoxes[rightRectIdx];
			rightBorder = rightRect.left;
			if( rightRect.bottom < nextY
			|| leftRectIdx >= leftBoxes.size())
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

	for( size_t i=0; i<leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = leftBoxes[i];
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
	for( size_t i=0; i<leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = leftBoxes[i];
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
	int		right = docPosition.right;

	for( size_t i=0; i<rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = rightBoxes[i];
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
	for( size_t i=0; i<rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = rightBoxes[i];
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

STRING XMLeditorChild::getWindowClassName( void ) const
{
	return className;
}

ProcessStatus XMLeditorChild::handleVertScroll( VertScrollCode scrollCode, int nPos, HWND )
{
	switch( scrollCode )
	{
		case vscTOP:
			nPos = 0;
			break;
		case vscLINE_UP:
			nPos = vertOffset - 20;
			break;
		case vscPAGE_UP:
			nPos = vertOffset - size.height;
			break;
		case vscLINE_DOWN:
			nPos = vertOffset + 20;
			break;
		case vscPAGE_DOWN:
			nPos = vertOffset + size.height;
			break;
		case vscBOTTOM:
			nPos = boxSize.bottom - size.height;
			break;

		case vscTHUMB_POSITION:
		case vscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	if( nPos < 0 )
		nPos = 0;
	else if( nPos > boxSize.bottom - size.height )
		nPos = boxSize.bottom - size.height;

	setVertScrollPos( nPos );
	vertOffset = nPos;

	invalidateWindow();

	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND )
{
	switch( scrollCode )
	{
		case hscLEFT:
			nPos = 0;
			break;
		case hscLINE_LEFT:
			nPos = horizOffset - 20;
			break;
		case hscPAGE_LEFT:
			nPos = horizOffset - size.width;
			break;
		case hscLINE_RIGHT:
			nPos = horizOffset + 20;
			break;
		case hscPAGE_RIGHT:
			nPos = horizOffset + size.width;
			break;
		case hscRIGHT:
			nPos = boxSize.right - size.width;
			break;

		case hscTHUMB_POSITION:
		case hscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	if( nPos < 0 )
		nPos = 0;
	else if( nPos > boxSize.right - size.width )
		nPos = boxSize.right - size.width;

	setHorizScrollPos( nPos );
	horizOffset = nPos;

	invalidateWindow();

	return psPROCESSED;
}

void XMLeditorChild::drawCursor( Device &context )
{
	if( cursorPos.getViewerBox() && cursorPos.getLine() != -1 && cursorPos.getChunk() != -1 )
	{
		cursorVisible = !cursorVisible;

		const XML_LINE			&theLine = cursorPos.getViewerBox()->getLine(cursorPos.getLine());
		const XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos.getChunk()];

		context.getPen().selectPen( Pen::spBlack );
		int old = context.setROP2( R2_NOT );

		context.verticalLine(
			theChunk.getPosition().x-horizOffset+cursorPos.getHorizOffset(),
			theChunk.getPosition().y-vertOffset,
			theChunk.getPosition().y+theLine.lineHeight-vertOffset
		);
		context.setROP2( old );
	}
}

void XMLeditorChild::handleTimer( void )
{
	DrawDevice	context( this );

	drawCursor( context );
}

ProcessStatus XMLeditorChild::handleRepaint( Device &hDC )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleRepaint");

	if( theViewerBox )
	{
		cursorVisible = false;

		theViewerBox->draw( hDC, this );

		drawCursor( hDC );
	}
	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleResize( const Size &size )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleResize");

	this->size = size;
	if( theViewerBox )
	{
		DrawDevice	context( this );

		hideCursor( context );
		/*
			don't clear cursor -> otherwise handleCharacterInput may crash
		*/
//		cursorPos.clear();

		int maxWidth = size.width;
		while( true )
		{
			try
			{
				theViewerBox->calcSize(
					context, 0, 0, maxWidth, maxWidth, size.height, &boxSize
				);
				break;
			}
			catch( ResizeException &e )
			{
				maxWidth += e.getIncWidth();
			}
			catch( ... )
			{
/*@*/			throw;
			}
		}

		if( boxSize.bottom > size.height )
		{
			showVertScrollBar( 0, boxSize.bottom - size.height );
		}
		else
		{
			vertOffset = 0;
			hideVertScrollBar();
		}

		if( boxSize.right > size.width )
		{
			showHorizScrollBar( 0, boxSize.right - size.width );
		}
		else
		{
			horizOffset = 0;
			hideHorizScrollBar();
		}

		invalidateWindow();
#ifdef _DEBUG
		dump();
#endif
	}
	return psDO_DEFAULT;
}

void XMLeditorChild::handleFocus( void )
{
	if( !cursorPos.getViewerBox() )
	{
		theViewerBox->locateFirstElement( &cursorPos );
	}

	enableCursor();
}

void XMLeditorChild::handleKillFocus( void )
{
	DrawDevice	context( this );

	disableCursor( context );
}

ProcessStatus XMLeditorChild::handleLeftButton(
	LeftButton leftButton, WPARAM modifier, const Point &position
)
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleLeftButton");

	if( leftButton == lbUP && !modifier && bool(theViewerBox) )
	{
		DrawDevice	context( this );

		hideCursor( context );

		xml::Element *theElement = theViewerBox->findElementOnScreen(
			context,
			position.x, position.y, horizOffset, vertOffset,
			&cursorPos
		);
		if( theElement )
		{
			getParent()->postMessage( WM_XML_ITEM_CLICK, 0, (LPARAM)theElement );
			enableCursor();
		}
		else
		{
			disableCursor( context );
		}
	}
	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleKeyDown( int key )
{
	ProcessStatus	handled = psDO_DEFAULT;

	if( cursorPos.getViewerBox() )
	{
		DrawDevice	context( this );
		xml::Element *oldElement = cursorPos.getElement();

		handled = psPROCESSED;
		switch( key )
		{
			case VK_DOWN:
				hideCursor( context );
				cursorPos.getViewerBox()->moveCursorDown( context, &cursorPos );
				break;
			case VK_UP:
				hideCursor( context );
				cursorPos.getViewerBox()->moveCursorUp( context, &cursorPos );
				break;

			case VK_RIGHT:
				hideCursor( context );
				if( !isControlKey() )
					cursorPos.getViewerBox()->moveCursorRight( context, &cursorPos );
				else
					cursorPos.getViewerBox()->moveCursorRightWord( context, &cursorPos );
				break;

			case VK_LEFT:
				hideCursor( context );
				if( !isControlKey() )
					cursorPos.getViewerBox()->moveCursorLeft( context, &cursorPos );
				else
					cursorPos.getViewerBox()->moveCursorLeftWord( context, &cursorPos );
				break;

			case VK_HOME:
				hideCursor( context );
				if( !isControlKey() )
					cursorPos.getViewerBox()->moveCursorStart( &cursorPos );
				else
					theViewerBox->locateFirstElement( &cursorPos );
				break;

			case VK_END:
				hideCursor( context );
				if( !isControlKey() )
					cursorPos.getViewerBox()->moveCursorEnd( &cursorPos );
				else
					theViewerBox->locateLastElement( &cursorPos );
				break;


			case VK_DELETE:
			{
				bool	doWrap;

				hideCursor( context );
				xml::XmlText *xmlText = cursorPos.getViewerBox()->deleteCharacter(
					context, this, &cursorPos, &doWrap
				);
				if( xmlText )
				{
					if( doWrap )
					{
						handleResize( size );
						cursorPos.getViewerBox()->findLineNChunk(
							context, xmlText, cursorPos.getInsertPos(),
							&cursorPos
						);
					}
					getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
				}
				break;
			}

			case VK_BACK:
			{
				bool	doWrap;

				hideCursor( context );
				if( cursorPos.getViewerBox()->moveCursorLeft( context, &cursorPos ) )
				{
					xml::XmlText *xmlText = cursorPos.getViewerBox()->deleteCharacter(
						context, this, &cursorPos, &doWrap
					);
					if( xmlText )
					{
						if( doWrap )
						{
							handleResize( size );
							cursorPos.getViewerBox()->findLineNChunk(
								context, xmlText, cursorPos.getInsertPos(),
								&cursorPos
							);
						}
						getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
					}
				}
				break;
			}

			default:
				handled = psDO_DEFAULT;
		}

		if( cursorPos.getViewerBox() )
		{
			const XML_LINE			&theLine = cursorPos.getViewerBox()->getLine(cursorPos.getLine());
			const XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos.getChunk()];

			int x = theChunk.getPosition().x-horizOffset+cursorPos.getHorizOffset();
			int yTop = theChunk.getPosition().y-vertOffset;
			int yBot = theChunk.getPosition().y+theLine.lineHeight-vertOffset;

			if( yTop<0 )
				scrollVertical(  vertOffset+yTop );
			else if( yBot > size.height )
				scrollVertical(  vertOffset+(yBot-size.height) );
			if( x<0 )
				scrollHorizontal( horizOffset+x );
			else if( x>size.width )
				scrollHorizontal( horizOffset+(x-size.width) );
		}

		xml::Element *newElement = cursorPos.getElement();
		if( newElement != oldElement )
			getParent()->postMessage( WM_XML_ITEM_CLICK, 0, (LPARAM)newElement );

	}
	return handled;
}

ProcessStatus XMLeditorChild::handleCharacterInput( int c )
{
	if( cursorPos.getViewerBox() && c >= ' ' )
	{
		bool		doWrap;
		DrawDevice	context( this );

		hideCursor( context );

		xml::XmlText *xmlText = cursorPos.getViewerBox()->insertCharacter(
			context, this, &cursorPos, c, &doWrap
		);
		if( xmlText )
		{
			if( doWrap )
			{
				handleResize( size );
				cursorPos.getViewerBox()->findLineNChunk(
					context, xmlText, cursorPos.getInsertPos(),
					&cursorPos
				);
			}
			getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
		}
	}

	return psPROCESSED;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

css::Styles *XML_VIEWER_BOX::getCssStyle( void )
{
	css::Styles		*theStyles = NULL;
	XML_VIEWER_BOX	*theBox = this;
	xml::Element	*theElement = theBox->theElement;

	while( theBox && !theStyles )
	{
		if( theElement )
		{
			theStyles = theElement->getCssStyle();
			break;
		}

		theBox = theBox->parentBox;
		theElement = theBox->theElement;
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
	xml::XmlText	*xmlText = NULL;

	*doWrap = false;

	if( cursorPos->getViewerBox() )
	{
		XML_LINE &theLine = theContent[cursorPos->getLine()];
		XML_LINE_CHUNK &theChunk = theLine.theLine[cursorPos->getChunk()];
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

			if( theLine.lineWidth + size.width <= theLine.maxWidth )
			{
				// yeah we do not need an eintire refresh
				// move the cursor right
				theLine.lineWidth += size.width;
				theChunk.width += size.width;

				// move other junks right
				bool otherElementsFound = false;
				for(
					size_t chunk = cursorPos->getChunk()+1;
					chunk < theLine.theLine.size();
					chunk++ )
				{
					XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
					theChunk.moveRight( size.width );
					otherElementsFound = true;
				}

				for(
					size_t line=cursorPos->getLine()+1;
					!otherElementsFound && line < theContent.size();
					line++
				)
				{
					XML_LINE &theLine = theContent[line];
					for(
						size_t chunk = 0;
						!otherElementsFound && chunk < theLine.theLine.size();
						chunk++ )
					{
						XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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
	xml::XmlText	*xmlText = NULL;

	*doWrap = false;

	if( cursorPos->getViewerBox() )
	{
		XML_LINE		&theLine = theContent[cursorPos->getLine()];
		XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos->getChunk()];
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
			theLine.lineWidth -= size.width;
			theChunk.width -= size.width;

			// move other junks left
			bool otherElementsFound = false;
			for(
				size_t chunk = cursorPos->getChunk()+1;
				chunk < theLine.theLine.size();
				chunk++ )
			{
				XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
				theChunk.moveRight( -size.width );
				otherElementsFound = true;
			}

			for(
				size_t line=cursorPos->getLine()+1;
				!otherElementsFound && line < theContent.size();
				line++
			)
			{
				XML_LINE &theLine = theContent[line];
				for(
					size_t chunk = 0;
					!otherElementsFound && chunk < theLine.theLine.size();
					chunk++ )
				{
					XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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

	xml::Element	*theElement = NULL;

	if( theContent.size() )
	{
		const XML_LINE &theLine = theContent[0];
		if( theLine.theLine.size() )
		{
			const XML_LINE_CHUNK	&theChunk = theLine.theLine[0];

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
		size_t	numSubBoxes = subBoxes.size();
		for( size_t i=0; i<numSubBoxes; i++ )
		{
			theElement = (*subBoxes[i]).locateFirstElement( cursorPos );
			if( theElement )
				break;
		}
	}

	if( !theElement )
	{
		size_t numChildBoxes = childElements.size();
		for( size_t i=0; i<numChildBoxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = childElements[i];
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

	xml::Element	*theElement = NULL;

	size_t	numLines = theContent.size();
	if( numLines )
	{
		size_t					line = numLines-1;
		const XML_LINE			&theLine = theContent[line];
		size_t					chunk = theLine.theLine.size()-1;

		const XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];

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
		size_t	numSubBoxes = subBoxes.size();
		for( size_t i=numSubBoxes-1; i!=-1; i-- )
		{
			theElement = subBoxes[i]->locateLastElement( cursorPos );
			if( theElement )
			{
				break;
			}
		}
	}

	if( !theElement )
	{
		size_t numChildBoxes = childElements.size();
		for( size_t i=numChildBoxes; i!=-1; i-- )
		{
			const XML_VIEWER_ITEM	&theItem = childElements[i];
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

	size_t numLines = theContent.size();
	if( numLines > 1 )
	{
		numLines--;
		for( size_t	line=0; line < numLines && !found; line++ )
		{
			XML_LINE &curLine = theContent[line];
			size_t numChunks = curLine.theLine.size();
			for(
				size_t chunk=0;
				chunk < numChunks && !found;
				chunk++
			)
			{
				XML_LINE_CHUNK	&theChunk = curLine.theLine[chunk];
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

	if( !found && bool(parentBox) )
	{
		found = parentBox->locateNextLine( context, this, cursorPos, oldPosition );
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

	size_t numLines = theContent.size();
	if( numLines > 1 )
	{
		for( size_t	line=numLines-1; line != -1 && !found; line-- )
		{
			XML_LINE &curLine = theContent[line];
			size_t numChunks = curLine.theLine.size();
			for(
				size_t chunk=numChunks-1;
				chunk != -1 && !found;
				chunk--
			)
			{
				XML_LINE_CHUNK	&theChunk = curLine.theLine[chunk];
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

	if( !found && bool(parentBox) )
	{
		found = parentBox->locatePrevLine( context, this, cursorPos, oldPosition );
	}

	return found;
}

/// @TODO make const
bool XML_VIEWER_BOX::locateNextChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos )
{
	bool	found = false;

	size_t	numSubBoxes, numLines, numChunks;

	size_t	numElements = childElements.size();
	if( numElements > 1 )
	{
		numElements--;
		for( size_t i=0; i<numElements && !found; i++ )
		{
			XML_VIEWER_ITEM			&theItem = childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox == current )
			{
				i++;
				XML_VIEWER_ITEM			&theItem = childElements[i];
				const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();
				if( theBox )
				{
					found = theBox->locateFirstElement( cursorPos ) != NULL;
				}
				break;
			}
		}
	}

	if( !found )
	{
		numSubBoxes = subBoxes.size();
		if( numSubBoxes > 1 )
		{
			numSubBoxes--;
			for( size_t i=0; i<numSubBoxes; i++ )
			{
				XML_VIEWER_BOX *theBox = subBoxes[i];
				if( theBox == current )
				{
					i++;
					theBox = subBoxes[i];
					found = theBox->locateFirstElement( cursorPos ) != NULL;
					break;
				}
			}
		}
	}
	
	// first we need the position of the current CHUNK
	if( !found )
	{
		numLines = theContent.size();
		for( size_t	line=0; line < numLines && !found; line++ )
		{
			XML_LINE &theLine = theContent[line];
			numChunks = theLine.theLine.size();
			for(
				size_t chunk=0;
				chunk < numChunks && !found;
				chunk++
			)
			{
				XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];
				if( theChunk.getInlineBox() == current )
				{
					// yeah we got it
					if( ++chunk < numChunks )
					{
						XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];

						if( theChunk.getTextElement() )
						{
							found = true;
							cursorPos->setPosition(
								this, line, chunk, theChunk.startPos, 0
							);
						}
						else
						{
							found = locateFirstElement( cursorPos ) != NULL;
						}
					}
					else if( ++line < numLines )
					{
						XML_LINE		&theLine = theContent[line];
						XML_LINE_CHUNK	&theChunk = theLine.theLine[0];

						if( theChunk.getTextElement() )
						{
							found = true;
							cursorPos->setPosition(
								this, line, 0, theChunk.startPos, 0
							);
						}
						else
						{
							found = locateFirstElement( cursorPos ) != NULL;
						}
					}
				}
			}
		}
	}
	if( !found && bool(parentBox) )
	{
		found = parentBox->locateNextChunk( this, cursorPos );
	}
	return found;
}

/// @TODO make const
bool XML_VIEWER_BOX::locatePrevChunk( XML_VIEWER_BOX *current, XML_CURSOR_POS *cursorPos )
{
	bool	found = false;

	size_t	numSubBoxes, numLines, numChunks;

	size_t	numElements = childElements.size();
	if( numElements > 1 )
	{
		for( size_t i=numElements-1; i && !found; i-- )
		{
			XML_VIEWER_ITEM			&theItem = childElements[i];
			const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

			if( theBox == current )
			{
				i--;
				XML_VIEWER_ITEM			&theItem = childElements[i];
				const XML_VIEWER_BOX	*theBox = theItem.getViewerBox();
				if( theBox )
				{
					found = theBox->locateLastElement( cursorPos ) != NULL;
				}
				break;
			}
		}
	}

	if( !found )
	{
		numSubBoxes = subBoxes.size();
		if( numSubBoxes > 1 )
		{
			for( size_t i=numSubBoxes-1; i; i-- )
			{
				XML_VIEWER_BOX *theBox = subBoxes[i];
				if( theBox == current )
				{
					i--;
					theBox = subBoxes[i];
					found = theBox->locateLastElement( cursorPos ) != NULL;
					break;
				}
			}
		}
	}

	// first we need the position of the current CHUNK
	if( !found )
	{
		numLines = theContent.size();
		if( numLines > 1 )
		{
			for( size_t	line=numLines-1; line != -1 && !found; line-- )
			{
				XML_LINE &theLine = theContent[line];
				numChunks = theLine.theLine.size();
				for(
					size_t chunk=numChunks-1;
					chunk != -1 && !found;
					chunk--
				)
				{
					XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];
					if( theChunk.getInlineBox() == current )
					{
						// yeah we got it
						if( --chunk != -1 )
						{
							XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];

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
								found = locateFirstElement( cursorPos ) != NULL;
						}
						else if( --line != -1 )
						{
							XML_LINE		&theLine = theContent[line];
							size_t			numChunks = theLine.theLine.size();
							if( numChunks )
							{
								numChunks--;
								XML_LINE_CHUNK	&theChunk = theLine.theLine[numChunks];

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
									found = locateLastElement( cursorPos ) != NULL;
								}
							}
						}
					}
				}
			}
		}
	}
	if( !found && bool(parentBox) )
	{
		found = parentBox->locatePrevChunk( this, cursorPos );
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
	XML_LINE		&curLine = theContent[line];
	XML_LINE_CHUNK	&curChunk = curLine.theLine[chunk];

	if( cursorPos->getInsertPos() < curChunk.startPos + curChunk.len )
	{
		c = curChunk.getText()[cursorPos->getInsertPos()];
		text[0] = c;
		text[1] = 0;

		createFont( context, curChunk.getTextElement(), false );
		context.getTextExtent( text, 1, &size );

		cursorPos->movePosition( 1, size.width );
	}
	else if( ++chunk < curLine.theLine.size() )
	{
		XML_LINE_CHUNK	&nextChunk = curLine.theLine[chunk];

		if( nextChunk.getTextElement() )
			cursorPos->setPosition( chunk, nextChunk.startPos, 0 );
		else
			nextChunk.getInlineBox()->locateFirstElement( cursorPos );
	}
	else if( ++line < theContent.size() )
	{
		XML_LINE		&nextLine = theContent[line];
		XML_LINE_CHUNK	&nextChunk = nextLine.theLine[0];

		if( nextChunk.getTextElement() )
			cursorPos->setPosition( line, chunk, nextChunk.startPos, 0 );
		else
			nextChunk.getInlineBox()->locateFirstElement( cursorPos );
	}
	else if( parentBox )
		parentBox->locateNextChunk( this, cursorPos );
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
	XML_LINE		&theLine = theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos->getChunk()];
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
	XML_LINE		&theLine = theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];

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
		XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];
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
		XML_LINE		&theLine = theContent[--line];

		chunk = theLine.theLine.size()-1;
		XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos->getChunk()];

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
	else if( parentBox )
		found = (*parentBox).locatePrevChunk( this, cursorPos );
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
	XML_LINE		&theLine = theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.theLine[cursorPos->getChunk()];
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
	XML_LINE		&theLine = theContent[line];
	XML_LINE_CHUNK	&theChunk = theLine.theLine[0];

	cursorPos->setPosition( 0, theChunk.startPos, 0 );
}

void XML_VIEWER_BOX::moveCursorEnd( XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&theLine = theContent[line];
	size_t			chunk = theLine.theLine.size()-1;
	XML_LINE_CHUNK	&theChunk = theLine.theLine[chunk];

	cursorPos->setPosition(
		chunk,
		theChunk.startPos + theChunk.len,
		theChunk.width
	);
}

bool XML_VIEWER_BOX::moveCursorUp( Device &context, XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&oldLine = theContent[line];
	XML_LINE_CHUNK	&oldChunk = oldLine.theLine[cursorPos->getChunk()];
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
	else if( parentBox )
		found = (*parentBox).locatePrevLine( context, this, cursorPos, oldPosition );

	return found;
}

bool XML_VIEWER_BOX::moveCursorDown( Device &context, XML_CURSOR_POS *cursorPos )
{
	size_t			line = cursorPos->getLine();
	XML_LINE		&oldLine = theContent[line];
	XML_LINE_CHUNK	&oldChunk = oldLine.theLine[cursorPos->getChunk()];
	int 			oldPosition = oldChunk.getPosition().x + cursorPos->getHorizOffset();
	bool			found = false;

	if( line+1 < theContent.size() )
	{
		line++;

		found = findCharPosInLine(
			context, line, oldPosition,
			cursorPos, CURSOR_DOWN
		);
	}
	else if( parentBox )
		found = (*parentBox).locateNextLine( context, this, cursorPos, oldPosition );

	return found;
}

bool XML_VIEWER_BOX::findLineNChunk(
	Device &context, xml::XmlText *xmlText, size_t insertPos,
	XML_CURSOR_POS *cursorPos
)
{
	bool	found = false;
	size_t	numChunks, numLines = theContent.size();
	for( size_t	line=0; !found && line<numLines; line++ )
	{
		XML_LINE	&theLine = theContent[line];

		numChunks = theLine.theLine.size();
		for( size_t chunk=0; !found && chunk<numChunks; chunk++ )
		{
			XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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

	if( theElement == this->theElement )
	{
		// find the first text chunk
		numLines = theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = theContent[line];

			numChunks = theLine.theLine.size();
			for( chunk=0;  chunk<numChunks; chunk++ )
			{
				const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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
		numLines = theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = theContent[line];
			numChunks = theLine.theLine.size();
			for( chunk=0;  chunk<numChunks; chunk++ )
			{
				const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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
		numSubboxes = subBoxes.size();
		for( size_t i=0; i<numSubboxes; i++ )
		{
			found = subBoxes[i]->findElementScreenPosition(
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
		numChildboxes = childElements.size();
		for( size_t i=0; i<numChildboxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = childElements[i];
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

	xml::Element	*theElement = NULL;

	if( !theElement )
	{
		numSubboxes = subBoxes.size();
		for( size_t i=0; i<numSubboxes; i++ )
		{
			theElement = subBoxes[i]->findElementOnScreen(
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
		numChildboxes = childElements.size();
		for( size_t i=0; i<numChildboxes; i++ )
		{
			const XML_VIEWER_ITEM	&theItem = childElements[i];
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
		if( position != css::POS_FIXED )
		{
			x += horizOffset;
			y += vertOffset;
		}

		numLines = theContent.size();
		for( line=0; line<numLines; line++ )
		{
			const XML_LINE	&theLine = theContent[line];
			if( theLine.position.x <= x
			&& theLine.position.x + theLine.lineWidth >=x
			&&  theLine.position.y <= y
			&& theLine.position.y + theLine.lineHeight >=y )
			{
				numChunks = theLine.theLine.size();
				for( chunk=0; chunk<numChunks; chunk++ )
				{
					const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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
		if( docPosition.left <= x
		&& docPosition.right >= x
		&& docPosition.top <= y
		&& docPosition.bottom >= y )
		{
			// find the last text chunk
			for(
				line=theContent.size()-1;
				line!=theContent.no_index && !theElement;
				line--
			)
			{
				const XML_LINE	&theLine = theContent[line];

				for( chunk = theLine.theLine.size()-1; chunk != -1; chunk-- )
				{
					const XML_LINE_CHUNK &theChunk = theLine.theLine[chunk];
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
	size_t	numSubBoxes = subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_TABLE_GROUP_VIEWER_BOX *rowBox = (XML_TABLE_GROUP_VIEWER_BOX *)(XML_VIEWER_BOX *)subBoxes[i];

		rowBox->calcColumnWidth(
			context, windowWidth, windowHeight
		);
	}
}

void XML_TABLE_GROUP_VIEWER_BOX::calcColumnWidth(
	Device &context, int windowWidth, int windowHeight
)
{
	size_t	numSubBoxes = subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_TABLE_ROW_VIEWER_BOX *rowBox = (XML_TABLE_ROW_VIEWER_BOX *)(XML_VIEWER_BOX *)subBoxes[i];

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

	size_t	numSubBoxes = subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		XML_VIEWER_BOX *cellBox = subBoxes[i];
		int innerWidth = cellBox->innerWidth;

		if( innerWidth > 0 )
			setStyleWidth( cellBox->getItemNumber(), innerWidth );

		cellBox->innerWidth = -1;

		try
		{
			cellBox->calcSize(
				context, 0, 0, windowWidth, windowWidth, windowHeight,
				&cellRect
			);
		}
		catch( ... )
		{
			cellBox->innerWidth = innerWidth;
/*@*/		throw;
		}
		cellBox->innerWidth = innerWidth;
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
	position.x += x;
	position.y += y;

	for( size_t i=0; i<theLine.size(); i++ )
		theLine[i].moveBy( x, y );
}

void XML_VIEWER_BOX::moveBy( int x, int y )
{
	docPosition.left += x;
	docPosition.top += y;
	docPosition.right += x;
	docPosition.bottom += y;

	size_t numChildBoxes = childElements.size();
	for( size_t i=0; i<numChildBoxes; i++ )
	{
		XML_VIEWER_ITEM	&theItem = childElements[i];
		XML_VIEWER_BOX	*theBox = theItem.getViewerBox();

		if( theBox
		&& theBox->getCssStyle()->getDisplay() != css::DSP_INLINE_BLOCK )
		{
			theBox->moveBy( x, y );
		}
	}

	size_t	numSubBoxes = subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		subBoxes[i]->moveBy( x, y );
	}
	for( size_t i=0; i<theContent.size(); i++ )
	{
		theContent[i].moveBy( x, y );
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
	size_t 			numColumns = columnWidths.size();
	int 			totalWidth = 0;
	int 			totalStyleWidth = 0;
	double			widthPercent;

	docPosition.left = left;
	docPosition.top = top;
	docPosition.right = left+maxWidth;
	docPosition.bottom = top+windowHeight;

	calcColumnWidth( context, windowWidth, windowHeight );

	for( size_t i=0; i<numColumns; i++ )
	{
		if( columnWidths[i] < styleWidths[i]  )
			columnWidths[i] = styleWidths[i];

		totalStyleWidth += styleWidths[i];
		totalWidth += columnWidths[i];
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
			totalWidth += columnWidths[i]-styleWidths[i];
		}

		for( size_t i=0; i<numColumns; i++ )
		{
			widthPercent = (double)(columnWidths[i]-styleWidths[i])/(double)totalWidth;
			columnWidths[i] = int(styleWidths[i]+widthPercent*double(maxWidth-totalStyleWidth));
		}

		totalWidth = maxWidth;
	}

	while( true )
	{
		try
		{
			size_t	numSubBoxes = subBoxes.size();
			for( size_t i = 0; i<numSubBoxes; i++ )
			{
				groupBox = subBoxes[i];

				groupHeight = groupBox->calcSize(
					context, left, top, maxWidth, windowWidth, windowHeight,
					&groupRect
				);

				XML_LINE		&theLine = theContent[i];
				XML_LINE_CHUNK	&theChunk = theLine.theLine[0];
				theChunk.setBlockElement( groupBox );
				theChunk.setPosition( left, top );
				theChunk.width = totalWidth;
				theLine.lineWidth = totalWidth;
				theLine.lineHeight = groupHeight;
				theLine.position.x = left;
				theLine.position.y = top;


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

	docPosition.right = left+totalWidth;
	docPosition.bottom = top+boxHeight;
	*screen = docPosition;

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

	docPosition.left = left;
	docPosition.top = top;
	docPosition.right = left+maxWidth;
	docPosition.bottom = top+windowHeight;

	size_t	numSubBoxes = subBoxes.size();
	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		try
		{
			rowBox = subBoxes[i];

			rowHeight = rowBox->calcSize(
				context, left, top, maxWidth, windowWidth, windowHeight,
				&rowRect
			);

			XML_LINE		&theLine = theContent[i];
			XML_LINE_CHUNK	&theChunk = theLine.theLine[0];
			theChunk.setBlockElement( rowBox );
			theChunk.setPosition( left, top );
			theChunk.width = maxWidth;
			theLine.lineWidth = maxWidth;
			theLine.lineHeight = rowHeight;
			theLine.position.x = left;
			theLine.position.y = top;

			top += rowHeight;
			boxHeight += rowHeight;
		}
		catch( ... )
		{
/*@*/		throw;
		}
	}

	docPosition.bottom = top+boxHeight;
	*screen = docPosition;

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

	docPosition.left = left;
	docPosition.top = top;
	docPosition.right = left+maxWidth;
	docPosition.bottom = top+windowHeight;

	size_t		numSubBoxes = subBoxes.size();
	XML_LINE	&theLine = theContent[0];
	theLine.position.x = left;
	theLine.position.y = top;
	theLine.lineWidth = maxWidth;

	for( size_t i = 0; i<numSubBoxes; i++ )
	{
		try
		{
			cellBox = subBoxes[i];

			columnWidth = getColumnWidth(cellBox->getItemNumber());
			innerWidth = cellBox->innerWidth;
			cellBox->innerWidth = columnWidth;
			cellHeight = cellBox->calcSize(
				context, left, top, columnWidth, windowWidth, windowHeight,
				&cellRect
			);
			cellBox->innerWidth = innerWidth;

			XML_LINE_CHUNK	&theChunk = theLine.theLine[i];
			theChunk.setBlockElement( cellBox );
			theChunk.setPosition( left, top );
			theChunk.width = columnWidth;


			left += columnWidth;
			if( maxHeight < cellHeight  )
				maxHeight = cellHeight;
		}
		catch( ... )
		{
			cellBox->innerWidth = innerWidth;
/*@*/		throw;
		}
	}

	theLine.lineHeight = maxHeight;

	docPosition.bottom = top+maxHeight;
	*screen = docPosition;

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
	int bottom = parentBox ? (*parentBox).docPosition.bottom : windowHeight;

	css::Styles *style = getCssStyle();

	STRING cssLeft =  style->getLeft();
	STRING cssRight =  style->getRight();
	STRING cssTop =  style->getTop();
	STRING cssBottom =  style->getBottom();

	if( position <= css::POS_STATIC )
	{
		// do normal layout
	}
	else if( position == css::POS_RELATIVE )
	{
		if( !cssLeft.isEmpty() )
			left = left + cssSizeToPixel( context, cssLeft, theElement );
		if( !cssTop.isEmpty() )
			top = top + cssSizeToPixel( context, cssTop, theElement );

		if( !cssRight.isEmpty() )
		{
			if( parentBox )
				right = (*parentBox).docPosition.right -
					cssSizeToPixel( context, cssRight, theElement )
				;
			else
				right = left + maxWidth -
					cssSizeToPixel( context, cssRight, theElement )
				;
		}
		if( !cssBottom.isEmpty() )
		{
			if( parentBox )
				bottom = (*parentBox).docPosition.bottom -
					cssSizeToPixel( context, cssBottom, theElement )
				;
			else
				bottom = top + windowHeight -
					cssSizeToPixel( context, cssBottom, theElement )
				;
		}
	}
	else if( position >= css::POS_ABSOLUTE )
	{
		if( !cssLeft.isEmpty() )
			left = cssSizeToPixel( context, cssLeft, theElement );
		if( !cssTop.isEmpty() )
			top = cssSizeToPixel( context, cssTop, theElement );

		if( !cssRight.isEmpty() )
		{
			if( parentBox )
				right = (*parentBox).docPosition.right -
					cssSizeToPixel( context, cssRight, theElement )
				;
			else
				right = left + maxWidth -
					cssSizeToPixel( context, cssRight, theElement )
				;
		}
		if( !cssBottom.isEmpty() )
		{
			if( parentBox )
				bottom = (*parentBox).docPosition.bottom -
					cssSizeToPixel( context, cssBottom, theElement )
				;
			else
				bottom = top + windowHeight -
					cssSizeToPixel( context, cssBottom, theElement )
				;
		}
	}

	if( cssLeft.isEmpty() || cssRight.isEmpty() )
	{
		if( this->innerWidth > 0 )
		{
			maxWidth = this->innerWidth +
				padding_left + margin_left + padding_right + margin_right
			;
		}
		else if( this->innerWidthPercent > 0 )
		{
			maxWidth =
				((parentBox
					? (*parentBox).maxWidth
					: maxWidth
				) * this->innerWidthPercent) /100
			;
		}
		if( cssLeft.isEmpty() && !cssRight.isEmpty() )
			left = right-maxWidth;
		else
			right = left+maxWidth;
	}

	if( cssTop.isEmpty() || cssBottom.isEmpty() )
	{
		if( this->innerHeight > 0 )
		{
			maxHeight = this->innerHeight +
				padding_top + margin_top + padding_bottom + margin_bottom
			;
			if( border_style_top > css::BORD_NONE )
				maxHeight += border_width_top;
			if( border_style_bottom > css::BORD_NONE )
				maxHeight += border_width_bottom;
		}
		else if( this->innerHeightPercent > 0 )
		{
			maxHeight =
				((parentBox
					? (*parentBox).maxHeight
					: windowHeight
				) * this->innerHeightPercent) /100
			;
			if( border_style_top > css::BORD_NONE )
				maxHeight += border_width_top;
			if( border_style_bottom > css::BORD_NONE )
				maxHeight += border_width_bottom;
		}
		else
			maxHeight = windowHeight;

		if( cssTop.isEmpty() && !cssBottom.isEmpty() )
			top = bottom-maxHeight;
		else
			bottom = top+maxHeight;
	}

	docPosition.left = left;
	docPosition.top = top;
	docPosition.right = right;
	docPosition.bottom = bottom;

	Size	size;
	int		lineHeight;

	int innerLeft = left + padding_left + margin_left;
	if( border_style_left > css::BORD_NONE )
		innerLeft += border_width_left;

	int innerRight = right - padding_right - margin_right;
	if( border_style_right > css::BORD_NONE )
		innerRight -= border_width_right;

	int innerTop = top + padding_top + margin_top;
	if( border_style_top > css::BORD_NONE )
		innerTop += border_width_top;

	int innerWidth = maxWidth = this->maxWidth = innerRight - innerLeft;

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

	leftBoxes.clear();
	rightBoxes.clear();
	theContent.clear();

#if DEBUG_LOG
	STRING	tag = theElement->getTag();
	if( tag == "long" )
	{
		doLogValue( tag );
	}
#endif
	size_t	i=0, numElements = childElements.size();
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
			XML_VIEWER_ITEM		&theItem = childElements[i];
			xml::Element			*theElement = theItem.getXmlElement();

			if( theElement )
			{
				autoWrap = theItem.getAutoWrap();
				preserveBreaks = theItem.getPreserveBreaks();
				preserveBlanks = theItem.getPreserveBlanks();

				xml::XmlText *theText = dynamic_cast<xml::XmlText *>(theElement);
				if( theText )
				{
					XML_LINE	&currentLine = theContent[numLines];

					numChunks = currentLine.theLine.size();

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
						currentLine.position.x = x;
						currentLine.position.y = y;
						currentLine.maxWidth = maxWidth;
					}

					XML_LINE_CHUNK	&newChunk = currentLine.theLine[
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
					if( lineHeight > currentLine.lineHeight )
						currentLine.lineHeight = lineHeight;
					else
						lineHeight = currentLine.lineHeight;

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
					if( currentLine.lineWidth + size.width > maxWidth )
					{
						if( !autoWrap )
						{
/*@*/						throw ResizeException(
								currentLine.lineWidth + size.width - maxWidth
							);
						}
						newChunk.width = findWrapPos(
							context, maxWidth-currentLine.lineWidth, !numChunks,
							newChunk.getText(), wrapPos, &chunkLen
						);

						// we did not get space for even one character
						if( !newChunk.width && !currentLine.lineWidth )
						{
							context.getTextExtent(
								text,
								wrapPos, 1,
								&size
							);

/*@*/						throw ResizeException( size.width-maxWidth );
						}
						newChunk.len = chunkLen;
						currentLine.lineWidth += newChunk.width;

						maxRight = innerRight;

						y += currentLine.lineHeight;
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
						currentLine.lineWidth += size.width;

						if( !preserveBreaks || newLinePos == -1 )
						{
							i++;
							wrapPos = 0;
						}
						else
						{
							y += currentLine.lineHeight;
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
		previousBox = NULL;
		while( i<numElements )
		{
			XML_VIEWER_ITEM	&theItem = childElements[i];

			childBox = theItem.getViewerBox();

			if( childBox )
			{
				i++;
				childBoxStyle = (*childBox).getCssStyle();
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
							previousBox->margin_bottom, childBox->margin_top
						);
						childBox->margin_top = maxMargin
							- previousBox->margin_bottom
						;
					}
					previousBox = childBox;
				}
				else
					previousBox = NULL;

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
					XML_LINE	&currentLine = theContent[numLines];

					if( childBoxWidth <= maxWidth-currentLine.lineWidth )
					{
						// add this block to the current line

						numChunks = currentLine.theLine.size();

						if( !numChunks )
						{
							currentLine.position.x = x;
							currentLine.position.y = y;
							currentLine.maxWidth = maxWidth;
						}
						XML_LINE_CHUNK	&newChunk = currentLine.theLine[
							numChunks
						];

						newChunk.setPosition( x, y );
						newChunk.width = childBoxWidth;
						newChunk.setBlockElement( childBox );
						if( currentLine.lineHeight < childBoxHeight )
							currentLine.lineHeight = childBoxHeight;
						lineHeight = currentLine.lineHeight;
						currentLine.lineWidth += childBoxWidth;
					}
					else
					{
						// the box is too large for this position move it down
						y += currentLine.lineHeight;
						y = findTop( y, childBoxWidth, innerLeft, innerRight );
						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;

						childBoxRect.left = x;
						childBoxRect.top = y;
						childBoxRect.right = x+childBoxWidth;
						childBoxRect.bottom = y + childBoxHeight;

						(*childBox).moveBy(
							childBoxRect.left - (*childBox).docPosition.left,
							childBoxRect.top - (*childBox).docPosition.top
						);

						// add this block to a new line
						XML_LINE	&currentLine = theContent[++numLines];
						currentLine.position.x = x;
						currentLine.position.y = y;
						currentLine.maxWidth = maxWidth;
						currentLine.lineHeight = childBoxHeight;
						currentLine.lineWidth = childBoxWidth;
						lineHeight = childBoxHeight;

						XML_LINE_CHUNK	&newChunk = currentLine.theLine[0];
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
						(*childBox).moveDown(
							childBoxRect.top - (*childBox).docPosition.top
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
								(*childBox).moveRight(
									childBoxRect.left - (*childBox).docPosition.left
								);
							}
							leftBoxes += childBoxRect;
						}
						else if( cssFloat == css::FLT_RIGHT )
						{
							x = findRight( y, innerRight );
							if( x != childBoxRect.right )
							{
								childBoxRect.right = x;
								childBoxRect.left = x - childBoxWidth;
								(*childBox).moveRight(
									childBoxRect.left - (*childBox).docPosition.left
								);
							}
							rightBoxes += childBoxRect;
						}


						if( childBoxRect.right > maxRight )
							maxRight = childBoxRect.right;

						x = findLeft( y, innerLeft );
						maxWidth = findRight( y, innerRight ) - x;
					}
					else
					{
						// add this block to a new line

						XML_LINE		&newLine = theContent[numLines++];
						XML_LINE_CHUNK	&newChunk = newLine.theLine[
							0
						];

						newLine.position.x = x;
						newLine.position.y = y;
						newLine.maxWidth = maxWidth;

						newChunk.setPosition( x, y );
						newChunk.setBlockElement( childBox );
						newLine.lineHeight = childBoxHeight;
						newChunk.width = newLine.lineWidth = childBoxWidth;

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
	numLines = theContent.size();
	if( text_align == css::ALIGN_CENTER || text_align == css::ALIGN_RIGHT )
	{
		for( size_t i=0; i<numLines; i++ )
		{
			XML_LINE	&currentLine = theContent[i];
			int 		offset = findRight( currentLine.position.y, innerRight );
			offset -= currentLine.position.x;
			offset -= currentLine.lineWidth;

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
		XML_LINE	&currentLine = theContent[i];
		for( size_t j=0; j<currentLine.theLine.size(); j++ )
		{
			XML_LINE_CHUNK	&theChunk = currentLine.theLine[j];
			if( theChunk.isBlockElement() )
			{
				childBoxHeight =
					(*theChunk.getInlineBox()).docPosition.bottom -
					(*theChunk.getInlineBox()).docPosition.top
				;
				if( childBoxHeight < currentLine.lineHeight )
				{
					theChunk.moveBy( 0, currentLine.lineHeight - childBoxHeight );
				}

			}
		}
	}

	int maxBottom = y;
	if( numLines )
	{
		for( size_t i = numLines-1; i!= -1; i-- )
		{
			XML_LINE	&currentLine = theContent[i];
			if( !currentLine.lineHeight
			|| !currentLine.theLine.size() )
			{
				theContent.removeElementAt( i );
				numLines--;
			}
		}
	}

	if( numLines )
	{
		XML_LINE	&currentLine = theContent[numLines-1];
		if( maxBottom < currentLine.position.y + currentLine.lineHeight )
		{
			maxBottom = currentLine.position.y + currentLine.lineHeight;
		}
	}
	for( size_t i=0; i<leftBoxes.size(); i++ )
	{
		const RectBorder &boxRect = leftBoxes[i];
		if( boxRect.bottom > maxBottom )
			maxBottom = boxRect.bottom;
	}
	for( size_t i=0; i<rightBoxes.size(); i++ )
	{
		const RectBorder &boxRect = rightBoxes[i];
		if( boxRect.bottom > maxBottom )
			maxBottom = boxRect.bottom;
	}
	size_t	numSubBoxes = subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		XML_VIEWER_BOX	*subBox = subBoxes[i];
		(*subBox).calcSize(
			context,
			0, 0, windowWidth, windowWidth, windowHeight,
			&childBoxRect
		);
		if( ((*subBox).position == css::POS_ABSOLUTE || (*subBox).display >= css::DSP_TABLE)  
		&& childBoxRect.bottom > maxBottom )
			maxBottom = childBoxRect.bottom;
	}

	int boxHeight = maxBottom - innerTop;

	boxHeight += padding_top + margin_top + padding_bottom + margin_bottom;
	if( border_style_top > css::BORD_NONE )
		boxHeight += border_width_top;
	if( border_style_bottom > css::BORD_NONE )
		boxHeight += border_width_bottom;

	if( this->innerWidth <= 0
	&& this->innerWidthPercent <= 0
	&& cssRight.isEmpty()
	&& (style->getCssFloat() > css::FLT_NONE || this->display == css::DSP_CELL) )
		docPosition.right = maxRight + padding_right + margin_right;

	if( this->innerHeight <= 0 && this->innerHeightPercent <= 0 && cssBottom.isEmpty() )
		docPosition.bottom = docPosition.top + boxHeight;

	*screen = docPosition;

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

	if( position == css::POS_FIXED )
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
		xml::Element	*theElement = this->theElement;
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
			theLine.position.x-horizOffset,
			theLine.position.y-vertOffset,
			theLine.position.x-horizOffset+theLine.maxWidth+1,
			theLine.position.y-vertOffset+theLine.lineHeight+1
		);
	}

	for( size_t i=0; i<theLine.theLine.size(); i++ )
	{
		XML_LINE_CHUNK	&theChunk = theLine.theLine[i];
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
				theChunk.getPosition().y+theLine.lineHeight-size.height-vertOffset
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

	if( position == css::POS_FIXED )
		vertOffset = horizOffset = 0;
	else
	{
		vertOffset = xmlEditorWindow->getVertOffset();
		horizOffset = xmlEditorWindow->getHorizOffset();
	}

	int left = docPosition.left;
	int right = docPosition.right;
	int top = docPosition.top;
	int bottom = docPosition.bottom;

	int borderLeft = left + margin_left;
	int borderRight = right - margin_right;
	int borderTop = top + margin_top;
	int borderBottom = bottom - margin_bottom;


	if( theElement )
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
		if( border_style_left > css::BORD_HIDDEN && border_width_left > 0 )
		{
			cssStyle->getBorderColorLeft( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( border_width_left, false ).setStyle( border_style_left, true )
			;
			context.verticalLine(
				borderLeft-horizOffset+border_width_left/2,
				borderTop-vertOffset,
				borderBottom-vertOffset
			);
		}
		if( border_style_right > css::BORD_HIDDEN && border_width_right > 0 )
		{
			cssStyle->getBorderColorRight( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( border_width_right, false )
				.setStyle( border_style_right, true )
			;
			context.verticalLine(
				borderRight-horizOffset-border_width_right/2,
				borderTop-vertOffset,
				borderBottom-vertOffset
			);
		}
		if( border_style_top > css::BORD_HIDDEN && border_width_top > 0 )
		{
			cssStyle->getBorderColorTop( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( border_width_top, false )
				.setStyle( border_style_top, true )
			;
			context.horizontalLine(
				borderLeft-horizOffset,
				borderRight-horizOffset,
				borderTop-vertOffset+border_width_top/2
			);
		}
		if( border_style_bottom > css::BORD_HIDDEN && border_width_bottom > 0 )
		{
			cssStyle->getBorderColorBottom( &color );
			context.getPen().setColor( color.red, color.green, color.blue, false )
				.setWidth( border_width_bottom, false )
				.setStyle( border_style_bottom, true )
			;
			context.horizontalLine(
				borderLeft-horizOffset,
				borderRight-horizOffset,
				borderBottom-vertOffset-border_width_bottom/2
			);
		}

		// draw the curcle for list items
		if( display == css::DSP_LIST_ITEM && listStyle > css::LIST_NONE )
		{
			if( listStyle <= css::LIST_SQUARE )
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
				if( listStyle == css::LIST_DISC )
				{
					context.getPen().selectPen( Pen::spNull );
					context.ellipse( left-16-horizOffset, top-vertOffset, left-8-horizOffset, top+8-vertOffset );
				}
				else if( listStyle == css::LIST_CIRCLE )
				{
					context.getPen().setWidth( 1, false );
					context.getPen().setStyle( Pen::psSolid, true );

					context.getBrush().selectBrush( Brush::sbNull );
					context.ellipse( left-16-horizOffset, top-vertOffset, left-8-horizOffset, top+8-vertOffset );
				}
				else if( listStyle == css::LIST_SQUARE )
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
				int		listNumber = itemNumber+1;

				createFont( context, theElement, true );
				if( listStyle == css::LIST_ZERO_DECIMAL )
				{
					sprintf( tmpBuffer, "%02d. ", listNumber%100 );
				}
				else if( listStyle == css::LIST_UPPER )
				{
					sprintf( tmpBuffer, "%c. ", 'A'-1 + listNumber%26 );
				}
				else if( listStyle == css::LIST_LOWER )
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
		for( size_t i=0; i<theContent.size(); i++ )
		{
			XML_LINE	&theLine = theContent[i];
			drawLine( context, xmlEditorWindow, theLine, false );
		}

	}

	size_t numChildBoxes = childElements.size();
	for( size_t i=0; i<numChildBoxes; i++ )
	{
		XML_VIEWER_ITEM	&theItem = childElements[i];
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
		text = theElement ? theElement->getTag() : STRING("auto");
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

	size_t	numSubBoxes = subBoxes.size();
	for( size_t i=0; i<numSubBoxes; i++ )
	{
		(*subBoxes[i]).draw( context, xmlEditorWindow );
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
	||	(display >= css::DSP_INLINE_TABLE && display < css::DSP_CELL) )
	{
		subBoxes += childBox;
	}
	else
	{
		XML_VIEWER_ITEM	&theItem = childElements.createElement();
		theItem.setViewerBox( childBox );
	}

	(*childBox).buildBoxTree( context, theRoot, this );
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
		css::WhiteSpace	whiteSpace = this->theElement->getCssStyle()->getWhiteSpace();

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
		XML_VIEWER_ITEM	&theItem = childElements.createElement();
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
				|| (display >= css::DSP_INLINE_TABLE && display < css::DSP_CELL)   )
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

		theElement = NULL;
		parentBox = container;
		this->display = css::DSP_TABLE;

		createBox( context, theRoot, display, theStyle->getPosition(), itemNumber );
	}
	else if( display >= css::DSP_INLINE_TABLE && display <= css::DSP_TABLE )
	{
		/*
			this element is either a table or an inline-table
		*/
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
		this->display = display;
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
		theElement = NULL;
		parentBox = container;
		this->display = css::DSP_ROW_GROUP;
		createBox( context, theRoot, display, theStyle->getPosition(), itemNumber );
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
		theElement = NULL;
		parentBox = container;
		this->display = css::DSP_ROW;
		createBox( context, theRoot, display, theStyle->getPosition(), itemNumber );
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

	parentBox = container;
	initColumn();


	if( display >= css::DSP_INLINE_TABLE && display < css::DSP_CELL )
	{
		// we need an anonymous element - it's me
		theElement = NULL;
		this->display = css::DSP_CELL;
		createBox( context, theRoot, display, theStyle->getPosition(), itemNumber );
	}
	else
	{
		XML_VIEWER_BOX::buildBoxTree( context, theRoot, container );
		this->display = css::DSP_CELL;
	}
}

void XML_VIEWER_BOX::buildBoxTree(
	const Device &context, xml::Element *theRoot, XML_VIEWER_BOX *container
)
{
	theElement = theRoot;
	parentBox = container;

	css::Styles *cssStyle = theRoot->getCssStyle();

	margin_left = cssSizeToPixel( context, cssStyle->getMarginLeft(), theRoot );
	margin_right = cssSizeToPixel(
		context, cssStyle->getMarginRight(), theRoot
	);
	margin_top = cssSizeToPixel( context, cssStyle->getMarginTop(), theRoot );
	margin_bottom = cssSizeToPixel(
		context, cssStyle->getMarginBottom(), theRoot
	);

	padding_left = cssSizeToPixel(
		context, cssStyle->getPaddingLeft(), theRoot
	);
	padding_right = cssSizeToPixel(
		context, cssStyle->getPaddingRight(), theRoot
	);
	padding_top = cssSizeToPixel(
		context, cssStyle->getPaddingTop(), theRoot
	);
	padding_bottom = cssSizeToPixel(
		context, cssStyle->getPaddingBottom(), theRoot
	);

	STRING cssValue = cssStyle->getWidth();

	if( cssValue.isEmpty() )
	{
		innerWidth = -1;
		innerWidthPercent = -1;
	}
	else if( cssValue.endsWith( '%' ) )
	{
		innerWidth = -1;
		innerWidthPercent = atoi( cssValue );
	}
	else
	{
		innerWidthPercent = -1;
		innerWidth = cssSizeToPixel( context, cssValue, theRoot );
	}

	cssValue = cssStyle->getHeight();

	if( cssValue.isEmpty() )
	{
		innerHeight = -1;
		innerHeightPercent = -1;
	}
	else if( cssValue.endsWith( '%' ) )
	{
		innerHeight = -1;
		innerHeightPercent = atoi( cssValue );
	}
	else
	{
		innerHeightPercent = -1;
		innerHeight = cssSizeToPixel( context, cssValue, theRoot );
	}

	position = cssStyle->getPosition();
	display = cssStyle->getDisplay();
	listStyle = cssStyle->getListStyle();

	border_style_left = cssStyle->getBorderStyleLeft();
	border_style_right = cssStyle->getBorderStyleRight();
	border_style_top = cssStyle->getBorderStyleTop();
	border_style_bottom = cssStyle->getBorderStyleBottom();

	border_width_left = cssSizeToPixel(
		context, cssStyle->getBorderWidthLeft(), theRoot
	);
	border_width_right = cssSizeToPixel(
		context, cssStyle->getBorderWidthRight(), theRoot
	);
	border_width_top = cssSizeToPixel(
		context, cssStyle->getBorderWidthTop(), theRoot
	);
	border_width_bottom = cssSizeToPixel(
		context, cssStyle->getBorderWidthBottom(), theRoot
	);

	findBoxes( context, theRoot );

	// now we walk throup all items an remove blank lines
	bool	removeBlanks = true;
	bool	blankFound = false;
	size_t	blankIndex = 0;
	size_t 	i=0;
	size_t 	numElements = childElements.size();
	STRING	xmlText;

	while( i<numElements )
	{
		XML_VIEWER_ITEM		&theItem = childElements[i];
		xml::Element			*theElement = theItem.getXmlElement();

		if( theElement )
		{
			xmlText = theItem.getXmlText();
			xmlText.stripBlanks();
			if( xmlText.isEmpty() )
			{
				if( removeBlanks )
				{
					childElements.removeElementAt( i );
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
				childElements.removeElementAt( blankIndex );
				numElements--;
				blankFound = false;
				removeBlanks = true;
			}
			else
				i++;
		}
	}

}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
void XMLeditorChild::setDocument( xml::Document *newDocument )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::setDocument");

	if( theViewerBox )
	{
		theViewerBox = NULL;
	}

	if( newDocument )
	{
		DrawDevice	context( this );

		theViewerBox = new XML_VIEWER_BOX;
		theViewerBox->buildBoxTree( context, newDocument->getRoot(), NULL );

		horizOffset = vertOffset = 0;

		if( size.width && size.height )
			handleResize( size );
#ifdef _DEBUG
		dump();
#endif
	}
}

void XMLeditorChild::refresh( void )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::refresh");

	if( theViewerBox )
	{
		xml::Element	*theRoot = (*theViewerBox).getElement();

		if( theRoot )
		{
			DrawDevice	context( this );

			disableCursor( context );
			theViewerBox = new XML_VIEWER_BOX;
			theViewerBox->buildBoxTree( context, theRoot, NULL );

			handleResize( size );
		}
	}
}

void XMLeditorChild::showElement( xml::Element *theElement )
{
	if( theViewerBox )
	{
		int			x, y;
		DrawDevice	context( this );

		hideCursor( context );
		bool found = (*theViewerBox).findElementScreenPosition(
			theElement, &x, &y, &cursorPos
		);
		if( found )
		{
			if( x<horizOffset || x>horizOffset+size.width )
			{
				horizOffset = x;
				setHorizScrollPos( x );
				invalidateWindow();
			}
			if( y<vertOffset || y>vertOffset+size.height )
			{
				vertOffset = y;
				setVertScrollPos( y );
				invalidateWindow();
			}
		}
	}
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

