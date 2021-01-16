/*
		Project:		Windows Class Library
		Module:			gridView.cpp
		Description:	A control diosplaying data in a grid
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
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <WINLIB/gridView.h>
#include <WINLIB\DEVICE.H>
#include <WINLIB/RAW_DATA.H>

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

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

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

const char GridViewer::className[] = "GridViewer";

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void GridViewer::registerClass( void )
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		fillDefaultClass( &wc );
//			wc.style			|= CS_DBLCLKS;
		wc.lpszClassName	 = className;
		wc.hCursor			 = NULL;

		registered = ChildWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void GridViewer::calcDimensions( const Size &size )
{
	assert( rowAttributes.size() == data.getNumRows() );
	assert( colAttributes.size() == data.getNumCols() );

	if( data.getNumRows() && data.getNumCols() )
	{
		double	colWidth = this->colWidth ? this->colWidth : double(size.width)/double(data.getNumCols());
		double	rowHeight = getRowHeight();

		double	left = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = colAttributes.begin(), endIT = colAttributes.end();
			it != endIT;
			++it
		)
		{
			it->left = unsigned(left+0.5);
			if( it->width )
				left += it->width;
			else
				left += colWidth;
			it->right = unsigned(left+0.5);
		}
		totalWidth = int(left+0.5);
		if( totalWidth > size.width )
		{
			showHorizScrollBar( 0, int(left-size.width) );
			if( horizOffset  > totalWidth - size.width )
				horizOffset = totalWidth - size.width;
			setHorizScrollPos( horizOffset );
		}
		else
		{
			horizOffset = 0;
			hideHorizScrollBar();
		}

		double	top = 0;
		for(
			gak::PODarray<RowAttribute>::iterator it = rowAttributes.begin(), endIT = rowAttributes.end();
			it != endIT;
			++it
		)
		{
			it->top = unsigned(top+0.5);
			if( it->height )
				top += it->height;
			else
				top += rowHeight;
			it->bottom = unsigned(top+0.5);
		}
		totalHeight = int(top+0.5);
		if( totalHeight > size.height )
		{
			showVertScrollBar( 0, totalHeight-size.height );
			if( vertOffset > totalHeight - size.height )
				vertOffset = totalHeight - size.height;
			setVertScrollPos( vertOffset );
		}
		else
		{
			vertOffset = 0;
			hideVertScrollBar();
		}
	}
	updateCaretPos();
}

RectBorder GridViewer::getCellVirtualPosition( size_t col, size_t row )
{
	RectBorder	rect;

	const ColAttribute	&colAttribute = colAttributes[col];

	rect.left = colAttribute.left;
	rect.right = colAttribute.right;

	const RowAttribute	&rowAttribute = rowAttributes[row];
	rect.top = rowAttribute.top;
	rect.bottom = rowAttribute.bottom;

	return rect;
}

RectBorder GridViewer::getCellScreenPosition( size_t col, size_t row )
{
	RectBorder	rect = getCellVirtualPosition( col, row );

	if( col >= fixedCols )
	{
		rect.left -= horizOffset;
		rect.right -= horizOffset;
	}
	if( row >= fixedRows )
	{
		rect.top -= vertOffset;
		rect.bottom -= vertOffset;
	}

	return rect;
}

void GridViewer::drawCell( Device &hDC, const RectBorder &rect, const STRING &cellData, size_t offset )
{
	doEnterFunction("GridViewer::drawCell");

	RectBorder	textRect = rect;
	textRect.left += cellPadding;
	textRect.top += cellPadding-1;
	textRect.right -= cellPadding;
	textRect.bottom -= cellPadding;
	if( offset )
		hDC.drawText( cellData+offset, cellData.strlen()-offset, textRect, DT_NOPREFIX|DT_LEFT|DT_TOP );
	else
		hDC.drawText( cellData, textRect, DT_NOPREFIX|DT_LEFT|DT_TOP );

	if( &cellData == editCell && editPos != selPos )
	{
		Size	size;

		hDC.getTextExtent( cellData+offset, selPos-offset, &size );
		if( editPos < selPos )
		{
			textRect.right = textRect.left+size.width;
			textRect.left += caretOffsetPixel;
		}
		else
		{
			textRect.right = textRect.left + caretOffsetPixel;
			textRect.left += size.width;
		}
		hDC.setROP2( R2_NOT );
		hDC.getBrush().createSyscolor( scSELECTION );
		hDC.getPen().setStyle( Pen::psNull );
		hDC.rectangle( textRect );
		hDC.setROP2( R2_COPYPEN );
	}
}

void GridViewer::drawEditCell( Device &hDC, const RectBorder &rect )
{
	assert( editCell );

	hDC.getBrush().createSyscolor( scWINDOW_FACE );
	hDC.getPen().setStyle( Pen::psSolid ).setWidth( 1 );
	hDC.rectangle( rect.left, rect.top, rect.right, rect.bottom );

	hideCaret();
	drawCell( hDC, rect, *editCell, editOffset );
	showCaret();
}

void GridViewer::updateCaretPos( void )
{
	if( editCell )
	{
		int left = colAttributes[editCol].left+1;
		if( editCol >= fixedCols )
			left -= horizOffset;
		int top = rowAttributes[editRow].top;
		if( editRow >= fixedRows )
		{
			top -= vertOffset;
		}

		moveCaret( left+caretOffsetPixel, top );
	}
}

void GridViewer::moveCursor( size_t newEditCol, size_t newEditRow )
{
	assert( newEditCol < data.getNumCols() );
	assert( newEditRow < data.getNumRows() );

	if( newEditRow != editRow  || newEditCol != editCol )
	{
		selPos = editPos = 0;
		editOffset = 0;
		caretOffsetPixel = 0;
		editCell = &data( newEditCol, newEditRow );
		editRow = newEditRow;
		editCol = newEditCol;

		if( newEditCol >= fixedCols && newEditRow >= fixedRows )
		{
			Size		size= getClientSize();
			RectBorder rect = getCellScreenPosition( editCol, editRow );

			int	minLeft = colAttributes[fixedCols].left;
			if( rect.left < minLeft )
			{
				horizOffset -= minLeft-rect.left;
				setHorizScrollPos( horizOffset );
			}
			else if( rect.right > size.width )
			{
				int visibleWidth = math::min( rect.getWidth(), size.width-minLeft );
				horizOffset = rect.left+horizOffset-size.width+visibleWidth; 
				setHorizScrollPos( horizOffset );
			}
			int	minTop = rowAttributes[fixedRows].top;
			if( rect.top < minTop )
			{
				vertOffset -= minTop-rect.top;
				setVertScrollPos( vertOffset );
			}
			else if( rect.bottom > size.height )
			{
				int visibleHeight = math::min( rect.getHeight(), size.height-minTop );
				vertOffset = rect.top+vertOffset-size.height+visibleHeight; 
				setVertScrollPos( vertOffset );
			}
		}

		updateCaretPos();
		invalidateWindow();
	}
}

void GridViewer::moveCursorNextCell( void )
{
	size_t	newEditRow = editRow;
	size_t	newEditCol = editCol+1;
	if( newEditCol >= data.getNumCols() )
	{
		if( getStyle() & gvCOL_CREATE )
			setNumCols( newEditCol+1 );
		else
		{
			newEditRow++;
			newEditCol = fixedCols;
			if( newEditRow >= data.getNumRows() )
			{
				if( getStyle() & gvROW_CREATE )
					setNumRows( newEditRow+1 );
				else
				{
					newEditRow = editRow;
					newEditCol = editCol;
				}
			}
		}
	}
	moveCursor( newEditCol, newEditRow );
}

void GridViewer::moveCursorPrevCell( void )
{
	if( editCol > fixedCols )
	{
		moveCursor( editCol-1, editRow );
	}
	else if( editRow > fixedRows )
	{
		moveCursor( data.getNumCols()-1, editRow-1 );
	}
}

bool GridViewer::moveCursorLeftWord( Device &hDC, bool includeSelection )
{
	assert( editCell );

	size_t newEditPos = editPos;
	while( newEditPos )
	{
		if( !isspace( (*editCell)[--newEditPos] ) )
/*v*/		break;
	}
	while( newEditPos )
	{
		if( isspace( (*editCell)[--newEditPos] ) )
/*v*/		break;
	}
	if( isspace( (*editCell)[newEditPos] ) )
		newEditPos++;

	return moveCursor( hDC, newEditPos, includeSelection );
}

bool GridViewer::moveCursorRightWord( Device &hDC, bool includeSelection )
{
	assert( editCell );
	size_t	len = editCell->strlen();

	size_t newEditPos = editPos;
	while( newEditPos < len )
	{
		if( isspace( (*editCell)[newEditPos] ) )
/*v*/		break;

		newEditPos++;
	}
	while( newEditPos < len )
	{
		if( !isspace( (*editCell)[newEditPos] ) )
/*v*/		break;

		newEditPos++;
	}

	return moveCursor( hDC, newEditPos, includeSelection );
}

bool GridViewer::moveCursor( Device &hDC, size_t newPosition, bool includeSelection )
{
	assert( editCell );

	bool	scrolled = editPos != selPos;

	if( newPosition <= editOffset )
	{
		scrolled = scrolled || newPosition < editOffset;

		editOffset = newPosition;
		caretOffsetPixel = 0;
	}
	else
	{
		if( newPosition > editCell->strlen() )
			newPosition = editCell->strlen();

		Size	size;
		hDC.getTextExtent( *editCell, editOffset, newPosition-editOffset, &size );
		caretOffsetPixel = size.width;

		int	width = getVisibleTextWidth( editCol );

		while( width < caretOffsetPixel )
		{
			hDC.getTextExtent( (*editCell)[editOffset++], &size );
			caretOffsetPixel -= size.width;
			scrolled = true;
		}
	}

	editPos = newPosition;
	if( includeSelection )
		selPos = newPosition;
	else
		scrolled = true;

	updateCaretPos();

	return scrolled;
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING GridViewer::getWindowClassName( void ) const
{
	return className;
}
   
ProcessStatus GridViewer::handleVertScroll( VertScrollCode scrollCode, int nPos, HWND /*scrollBar*/ )
{
	int		rowHeight = getRowHeight();
	Size	size = getClientSize();

	switch( scrollCode )
	{
		case vscTOP:
			nPos = 0;
			break;
		case vscLINE_UP:
			nPos = vertOffset - rowHeight;
			break;
		case vscPAGE_UP:
			nPos = vertOffset - size.height;
			break;
		case vscLINE_DOWN:
			nPos = vertOffset + rowHeight;
			break;
		case vscPAGE_DOWN:
			nPos = vertOffset + size.height;
			break;
		case vscBOTTOM:
			nPos = totalHeight - size.height;
			break;

		case vscTHUMB_POSITION:
		case vscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	int maxNpos;
	if( nPos < 0 )
		nPos = 0;
	else if( nPos > (maxNpos = totalHeight - size.height) )
		nPos = maxNpos;

	setVertScrollPos( nPos );
	vertOffset = nPos;
	updateCaretPos();

	invalidateWindow();

	return psPROCESSED;
}

ProcessStatus GridViewer::handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND )
{
	int		rowHeight = getRowHeight();
	Size	size = getClientSize();
	switch( scrollCode )
	{
		case hscLEFT:
			nPos = 0;
			break;
		case hscLINE_LEFT:
			nPos = horizOffset - rowHeight;
			break;
		case hscPAGE_LEFT:
			nPos = horizOffset - size.width;
			break;
		case hscLINE_RIGHT:
			nPos = horizOffset + rowHeight;
			break;
		case hscPAGE_RIGHT:
			nPos = horizOffset + size.width;
			break;
		case hscRIGHT:
			nPos = totalWidth - size.width;
			break;

		case hscTHUMB_POSITION:
		case hscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	int maxNpos;
	if( nPos < 0 )
		nPos = 0;
	else if( nPos > (maxNpos = totalWidth - size.width) )
		nPos = maxNpos;

	setHorizScrollPos( nPos );
	horizOffset = nPos;
	updateCaretPos();

	invalidateWindow();

	return psPROCESSED;
}

ProcessStatus GridViewer::handleResize( const Size &newSize )
{
	calcDimensions( newSize );

	return psDO_DEFAULT;
}

ProcessStatus GridViewer::handleRepaint( Device &hDC )
{
	doEnterFunction("GridViewer::handleRepaint");
	assert( rowAttributes.size() == data.getNumRows() );
	assert( colAttributes.size() == data.getNumCols() );

	Size	size = getSize();

	int	fixedHeight = fixedRows ? rowAttributes[fixedRows-1].bottom	: 0;
	int	fixedWidth  = fixedCols ? colAttributes[fixedCols-1].right	: 0;

	if( getStyle() & gvGRID )
	{
		doEnterFunction("GridViewer::drawGrid");
		hDC.getPen().setStyle( Pen::psDot ).setWidth( 1 );

		unsigned col=0;
		for(
			gak::PODarray<ColAttribute>::iterator it = colAttributes.begin(), endIT = colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int	pos = it->left;
			if( col >= fixedCols )
				pos -= horizOffset;
			if( pos >= size.width )
				break;
			if( pos > 0 )
			{
				hDC.verticalLine( pos, 0, size.height );
			}
		}

		unsigned row=0;
		for(
			gak::PODarray<RowAttribute>::iterator it = rowAttributes.begin(), endIT = rowAttributes.end();
			it != endIT;
			++it, ++row
		)
		{
			int pos = it->top;
			if( row >= fixedRows )
				pos -= vertOffset;
			if( pos >= size.height )
				break;
			if( pos > 0  )
				hDC.horizontalLine( 0, size.width, pos );
		}
	}

	if( data.getNumRows() && data.getNumCols() )
	{
		doEnterFunction("showText");
		size_t col=0;
		hDC.selectFont( getFont() );
		hDC.clrBackgroundColor();
		for(
			gak::PODarray<ColAttribute>::iterator it = colAttributes.begin(), endIT = colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			RectBorder	rect;
			rect.left = it->left;
			rect.right = it->right;
			if( col >= fixedCols )
			{
				rect.left -= horizOffset;
				rect.right -= horizOffset;
			}
			if( rect.left > size.width )
/*v*/			break;

			if( rect.right > 0 )
			{
				size_t row=0;
				for(
					gak::PODarray<RowAttribute>::iterator it = rowAttributes.begin(), endIT = rowAttributes.end();
					it != endIT;
					++it, ++row
				)
				{
					rect.top = it->top;
					rect.bottom = it->bottom;
					if( vertOffset && row >= fixedRows )
					{
						rect.top -= vertOffset;
						rect.bottom -= vertOffset;
					}

					if( rect.top > size.height )
/*v*/					break;
					if( rect.bottom > 0 )
					{
						if( row < fixedRows || col < fixedCols )
						{
							doEnterFunction("prepare Fixed Cells");
							if( row < fixedRows && col < fixedCols )
								hDC.removeClipping();
							else if( row < fixedRows )
								hDC.setClipping( fixedWidth, 0, size.width, size.height );
							else if( col < fixedCols )
								hDC.setClipping( 0, fixedHeight, size.width, size.height );


							hDC.getBrush().createSyscolor( scDIALOG_FACE );
							hDC.getPen().setStyle( Pen::psNull );
							hDC.rectangle( rect.left, rect.top, rect.right, rect.bottom );
						}
						else
						{
							hDC.setClipping( fixedWidth, fixedHeight, size.width, size.height );
						}

						const STRING &cellData = getCell( col, row );
						if( editCell == &cellData )
						{
							drawEditCell( hDC, rect );
						}
						else if( !cellData.isEmpty() )
						{
							drawCell( hDC, rect, cellData, 0 );
						}
					}
				}
			}
		}
	}

	return psPROCESSED;
}

ProcessStatus GridViewer::handleMouseMove( WPARAM /*modifier*/, const Point &position )
{
	MouseState		newState = mouseState;
	unsigned long	style = getStyle();

	if( (mouseState == msNORMAL || mouseState == msSTART_COL_SIZER) && (style & gvCOL_SIZABLE) )
	{
		newState = msNORMAL;
		size_t	col = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = colAttributes.begin(), endIT = colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int right = it->right;
			if( col >= fixedCols )
				right -= horizOffset;

			if( abs(int(right - position.x)) <= mouseSizeWidth )
			{
				newState = msSTART_COL_SIZER;
				sizerCol = col;
				break;
			}
		}
	}
	else if( mouseState == msDO_COL_SIZER )
	{
		int left = colAttributes[sizerCol].left;
		if( sizerCol >= fixedCols )
			left -= horizOffset;
		int newWidth = position.x - left;
		if( newWidth > mouseSizeWidth*2 )
		{
			setColWidth( sizerCol, newWidth );
			invalidateWindow();
		}
	}
	else if( !(style & gvCOL_SIZABLE) )
	{
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	if( newState != mouseState )
	{
		mouseState = newState;
		if( mouseState == msSTART_COL_SIZER || mouseState == msDO_COL_SIZER )
			SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );
		else
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	return psPROCESSED;
}

ProcessStatus GridViewer::handleLeftButton( LeftButton leftButton, WPARAM /*modifier*/, const Point &position )
{
	unsigned long	style = getStyle();

	if( leftButton == lbDOWN && mouseState == msSTART_COL_SIZER )
	{
		mouseState = msDO_COL_SIZER;
		captureMouse();
		return psPROCESSED;
	}
	else if( leftButton == lbDOWN && mouseState == msNORMAL && (style & (gvEDITABLE|gvCAPTION_EDITABLE)) )
	{
		STRING	*lastEditCell = editCell;
		editCell = NULL;
		int		caretLeft;
		size_t	col = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = colAttributes.begin(), endIT = colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int left = it->left;
			int right = it->right;
			if( horizOffset && col >= fixedCols )
			{
				left -= horizOffset;
				right -= horizOffset;
			}

			if( left < position.x && position.x < right )
			{
				caretLeft = left;
/*v*/			break;
			}
		}
		if( col < data.getNumCols() )
		{
			size_t row=0;
			for(
				gak::PODarray<RowAttribute>::iterator it = rowAttributes.begin(), endIT = rowAttributes.end();
				it != endIT;
				++it, ++row
			)
			{
				bool	isCaption = (col < fixedCols || row < fixedRows);
				int	top = it->top;
				int	bottom = it->bottom;
				if( vertOffset && row >= fixedRows )
				{
					top -= vertOffset;
					bottom -= vertOffset;
				}
				if( top < position.y && position.y < bottom )
				{
					if( isCaption && !(style&gvCAPTION_EDITABLE) )
						break;
					if( !isCaption && !(style&gvEDITABLE) )
						break;

					focus();
					createCaret( caretLeft+1, top, bottom-top );
					editCol = col;
					editRow = row;
					editCell = &getCell( col, row );
					selPos = editPos = 0;
					editOffset = 0;
					caretOffsetPixel = 0;
/*v*/				break;
				}
			}
		}
		if( lastEditCell != editCell )
		{
			if( !editCell )
				destroyCaret();
			invalidateWindow();
		}

	}
	else if( leftButton == lbUP && mouseState == msDO_COL_SIZER )
	{
		mouseState = msSTART_COL_SIZER;
		releaseMouse();
		return psPROCESSED;
	}

	return psDO_DEFAULT;
}

ProcessStatus GridViewer::handleKeyDown( int key )
{
	if( editCell )
	{
		DrawDevice hDC( this );

		switch( key )
		{
			case VK_HOME:
				if( moveCursor( hDC, 0, !isShiftKey() ) )
					drawEditCell( hDC );
				break;
			case VK_END:
				if( moveCursor( hDC, editCell->strlen(), !isShiftKey() ) )
					drawEditCell( hDC );
				break;
			case VK_RIGHT:
				if( (isControlKey() ? moveCursorRightWord( hDC, !isShiftKey() ) : moveCursorRight( hDC, !isShiftKey() )) )
					drawEditCell( hDC );
				break;
			case VK_LEFT:
				if( (isControlKey() ? moveCursorLeftWord( hDC, !isShiftKey() ) : moveCursorLeft( hDC, !isShiftKey() )) )
					drawEditCell( hDC );
				break;
			case VK_INSERT:
				if( isControlKey() )
					copy();
				else if( isShiftKey() )
					paste();
				break;
			case VK_DELETE:
				if( editPos < editCell->strlen() || selPos < editCell->strlen() )
				{
					if( editPos != selPos )
					{
						if( isShiftKey() )
							copy();
						deleteSelection();
					}
					else
						editCell->delChar( editPos );
					drawEditCell( hDC );
					notifyParent();
				}
				break;
			case VK_BACK:
				if( editPos || selPos )
				{
					if( editPos != selPos )
						deleteSelection();
					else
					{
						moveCursorLeft( hDC, true );
						editCell->delChar( editPos );
					}
					drawEditCell( hDC );
					notifyParent();
				}
				break;
			case VK_TAB:
				if( isShiftKey() )
					moveCursorPrevCell();
				else
					moveCursorNextCell();
				break;
			case VK_RETURN:
			{
				size_t	newEditRow = editRow+1;

				if( newEditRow >= data.getNumRows() )
				{
					if( getStyle() & gvROW_CREATE )
						setNumRows( newEditRow+1 );
					else
					{
						newEditRow = editRow;
					}
				}

				moveCursor( editCol, newEditRow );
				break;
			}
			default:
/***/			return psDO_DEFAULT;
		}
/***/	return psPROCESSED;
	}

	return psDO_DEFAULT;
}

ProcessStatus GridViewer::handleCharacterInput( int c )
{
	if( editCell )
	{
		if( c == 'C' - 'A' + 1 )
		{
			copy();
			return psPROCESSED;
		}
		else if( c == 'V' - 'A' + 1 )
		{
			paste();
			return psPROCESSED;
		}
		else if( c == 'X' - 'A' + 1 )
		{
			cut();
			return psPROCESSED;
		}
		else if( c >= ' ' )
		{
			DrawDevice	context( this );

			hideCaret();

			if( editPos != selPos )
				deleteSelection();

			editCell->insChar( editPos, char(c) );
			moveCursorRight( context, true );
			drawEditCell( context );

			showCaret();

			notifyParent();

			return psPROCESSED;
		}
	}
	
	return psDO_DEFAULT;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void GridViewer::setColWidth( size_t col, unsigned colWidth )
{
	if( col < data.getNumCols() )
	{
		colAttributes[col].width = colWidth;
	}
	if( col == editCol && editCell )
	{
		DrawDevice dc( this );
		moveCursor( dc, editPos, false );
	}
	calcDimensions();
}

void GridViewer::clear( void )
{
	if( editCell && editPos != selPos )
	{
		size_t	start = math::min( editPos, selPos );
		size_t	end = math::max( editPos, selPos );

		editCell->delStr( start, end-start );
		selPos = editPos;
		DrawDevice dc( this );
		drawEditCell( dc );
		notifyParent();
	}
}

void GridViewer::copy ( void )
{
	if( editCell && editPos != selPos )
	{
		Clipboard	clip( this );
		if( clip )
		{
			size_t	start = math::min( editPos, selPos );
			size_t	end = math::max( editPos, selPos );

			STRING	text = editCell->subString( start, end-start );
			WindowsBuffer	clipBuff;
			oWindowsStream	winStream( clipBuff );
			winStream << text << '\0';
			winStream.flush();
			clip.clear();
			clip.putToClipBoard( clipBuff, CF_TEXT );
		}
	}
}

void GridViewer::paste( void )
{
	if( editCell )
	{
		Clipboard	clip( this, CF_TEXT );
		if( clip )
		{
			ClipboardMemory	clipData( CF_TEXT );
			if( clipData.is_open() )
			{
				size_t	start = math::min( editPos, selPos );
				size_t	end = math::max( editPos, selPos );
				size_t	len = end - start;

				if( len )
					editCell->delStr( start, end-start );

				STRING				text;
				iClipboardStream	clipStream( clipData );
				clipStream >> text;

				editCell->insStr( editPos, text );
				DrawDevice	hDC( this );
				moveCursor( hDC, editPos + text.strlen(), true ); 
				drawEditCell( hDC );
				notifyParent();
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

