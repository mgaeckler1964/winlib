/*
		Project:		Windows Class Library
		Module:			gridView.cpp
		Description:	A control diosplaying data in a grid
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

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <WINLIB/gridView.h>
#include <WINLIB\DEVICE.H>
#include <WINLIB/RAW_DATA.H>
#include <WINLIB/messages.h>

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
	assert( m_rowAttributes.size() == m_data.getNumRows() );
	assert( m_colAttributes.size() == m_data.getNumCols() );

	if( m_data.getNumRows() && m_data.getNumCols() )
	{
		double	colWidth = m_colWidth ? m_colWidth : double(size.width)/double(m_data.getNumCols());
		double	rowHeight = getRowHeight();

		double	left = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = m_colAttributes.begin(), endIT = m_colAttributes.end();
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
		m_totalWidth = int(left+0.5);
		if( m_totalWidth > size.width )
		{
			showHorizScrollBar( 0, int(left-size.width) );
			if( m_horizOffset  > m_totalWidth - size.width )
				m_horizOffset = m_totalWidth - size.width;
			setHorizScrollPos( m_horizOffset );
		}
		else
		{
			m_horizOffset = 0;
			hideHorizScrollBar();
		}

		double	top = 0;
		for(
			gak::PODarray<RowAttribute>::iterator it = m_rowAttributes.begin(), endIT = m_rowAttributes.end();
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
		m_totalHeight = int(top+0.5);
		if( m_totalHeight > size.height )
		{
			showVertScrollBar( 0, m_totalHeight-size.height );
			if( m_vertOffset > m_totalHeight - size.height )
				m_vertOffset = m_totalHeight - size.height;
			setVertScrollPos( m_vertOffset );
		}
		else
		{
			m_vertOffset = 0;
			hideVertScrollBar();
		}
	}
	updateCaretPos();
}

RectBorder GridViewer::getCellVirtualPosition( size_t col, size_t row )
{
	RectBorder	rect;

	const ColAttribute	&colAttribute = m_colAttributes[col];

	rect.left = colAttribute.left;
	rect.right = colAttribute.right;

	const RowAttribute	&rowAttribute = m_rowAttributes[row];
	rect.top = rowAttribute.top;
	rect.bottom = rowAttribute.bottom;

	return rect;
}

RectBorder GridViewer::getCellScreenPosition( size_t col, size_t row )
{
	RectBorder	rect = getCellVirtualPosition( col, row );

	if( col >= m_fixedCols )
	{
		rect.left -= m_horizOffset;
		rect.right -= m_horizOffset;
	}
	if( row >= m_fixedRows )
	{
		rect.top -= m_vertOffset;
		rect.bottom -= m_vertOffset;
	}

	return rect;
}

void GridViewer::drawCell( Device &hDC, const RectBorder &rect, const TheCell &cellData, size_t offset )
{
	doEnterFunction("GridViewer::drawCell");

	RectBorder	textRect = rect;
	textRect.left += cellPadding;
	textRect.top += cellPadding-1;
	textRect.right -= cellPadding;
	textRect.bottom -= cellPadding;
	if( offset )
		hDC.drawText( cellData.text+offset, cellData.text.strlen()-offset, textRect, DT_NOPREFIX|DT_LEFT|DT_TOP );
	else
		hDC.drawText( cellData.text, textRect, DT_NOPREFIX|DT_LEFT|DT_TOP );

	if( &cellData == m_editCell && m_editPos != m_selPos )
	{
		Size	size;

		hDC.getTextExtent( cellData.text+offset, m_selPos-offset, &size );
		if( m_editPos < m_selPos )
		{
			textRect.right = textRect.left+size.width;
			textRect.left += m_caretOffsetPixel;
		}
		else
		{
			textRect.right = textRect.left + m_caretOffsetPixel;
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
	assert( m_editCell );

	hDC.getBrush().createSyscolor( scWINDOW_FACE );
	hDC.getPen().setStyle( Pen::psSolid ).setWidth( 1 );
	hDC.rectangle( rect.left, rect.top, rect.right, rect.bottom );

	hideCaret();
	drawCell( hDC, rect, *m_editCell, m_editOffset );
	showCaret();
}

void GridViewer::updateCaretPos( void )
{
	if( m_editCell )
	{
		int left = m_colAttributes[m_editCol].left+1;
		if( m_editCol >= m_fixedCols )
			left -= m_horizOffset;
		int top = m_rowAttributes[m_editRow].top;
		if( m_editRow >= m_fixedRows )
		{
			top -= m_vertOffset;
		}

		moveCaret( left+m_caretOffsetPixel, top );
	}
}

void GridViewer::moveCursor( size_t newEditCol, size_t newEditRow )
{
	assert( newEditCol < m_data.getNumCols() );
	assert( newEditRow < m_data.getNumRows() );

	if( !m_editCell || newEditRow != m_editRow  || newEditCol != m_editCol )
	{
		if( m_editCell )
		{
			getParent()->postMessage(WM_GRID_ITEM_EXIT, getId(), MAKELPARAM(m_editCol, m_editRow) );
		}
		m_selPos = m_editPos = 0;
		m_editOffset = 0;
		m_caretOffsetPixel = 0;
		m_editCell = &m_data( newEditCol, newEditRow );
		m_editRow = newEditRow;
		m_editCol = newEditCol;
		if( m_editCell )
		{
			getParent()->postMessage(WM_GRID_ITEM_ENTER, getId(), MAKELPARAM(m_editCol, m_editRow) );
		}

		if( newEditCol >= m_fixedCols && newEditRow >= m_fixedRows )
		{
			Size		size= getClientSize();
			RectBorder rect = getCellScreenPosition( m_editCol, m_editRow );

			int	minLeft = m_colAttributes[m_fixedCols].left;
			if( rect.left < minLeft )
			{
				m_horizOffset -= minLeft-rect.left;
				setHorizScrollPos( m_horizOffset );
			}
			else if( rect.right > size.width )
			{
				int visibleWidth = math::min( rect.getWidth(), size.width-minLeft );
				m_horizOffset = rect.left+m_horizOffset-size.width+visibleWidth; 
				setHorizScrollPos( m_horizOffset );
			}
			int	minTop = m_rowAttributes[m_fixedRows].top;
			if( rect.top < minTop )
			{
				m_vertOffset -= minTop-rect.top;
				setVertScrollPos( m_vertOffset );
			}
			else if( rect.bottom > size.height )
			{
				int visibleHeight = math::min( rect.getHeight(), size.height-minTop );
				m_vertOffset = rect.top+m_vertOffset-size.height+visibleHeight; 
				setVertScrollPos( m_vertOffset );
			}
		}

		updateCaretPos();
		invalidateWindow();
	}
}

void GridViewer::moveCursorNextCell( void )
{
	size_t	newEditRow = m_editRow;
	size_t	newEditCol = m_editCol+1;
	if( newEditCol >= m_data.getNumCols() )
	{
		if( getStyle() & gvCOL_CREATE )
			setNumCols( newEditCol+1 );
		else
		{
			newEditRow++;
			newEditCol = m_fixedCols;
			if( newEditRow >= m_data.getNumRows() )
			{
				if( getStyle() & gvROW_CREATE )
					setNumRows( newEditRow+1 );
				else
				{
					newEditRow = m_editRow;
					newEditCol = m_editCol;
				}
			}
		}
	}
	moveCursor( newEditCol, newEditRow );
}

void GridViewer::moveCursorPrevCell( void )
{
	if( m_editCol > m_fixedCols )
	{
		moveCursor( m_editCol-1, m_editRow );
	}
	else if( m_editRow > m_fixedRows )
	{
		moveCursor( m_data.getNumCols()-1, m_editRow-1 );
	}
}

bool GridViewer::moveCursorLeftWord( Device &hDC, bool includeSelection )
{
	assert( m_editCell );

	size_t newEditPos = m_editPos;
	while( newEditPos )
	{
		if( !isspace( m_editCell->text[--newEditPos] ) )
/*v*/		break;
	}
	while( newEditPos )
	{
		if( isspace( m_editCell->text[--newEditPos] ) )
/*v*/		break;
	}
	if( isspace( m_editCell->text[newEditPos] ) )
		newEditPos++;

	return moveCursor( hDC, newEditPos, includeSelection );
}

bool GridViewer::moveCursorRightWord( Device &hDC, bool includeSelection )
{
	assert( m_editCell );
	size_t	len = m_editCell->text.strlen();

	size_t newEditPos = m_editPos;
	while( newEditPos < len )
	{
		if( isspace( m_editCell->text[newEditPos] ) )
/*v*/		break;

		newEditPos++;
	}
	while( newEditPos < len )
	{
		if( !isspace( m_editCell->text[newEditPos] ) )
/*v*/		break;

		newEditPos++;
	}

	return moveCursor( hDC, newEditPos, includeSelection );
}

bool GridViewer::moveCursor( Device &hDC, size_t newPosition, bool includeSelection )
{
	assert( m_editCell );

	bool	scrolled = m_editPos != m_selPos;

	if( newPosition <= m_editOffset )
	{
		scrolled = scrolled || newPosition < m_editOffset;

		m_editOffset = newPosition;
		m_caretOffsetPixel = 0;
	}
	else
	{
		if( newPosition > m_editCell->text.strlen() )
			newPosition = m_editCell->text.strlen();

		Size	size;
		hDC.getTextExtent( m_editCell->text, m_editOffset, newPosition-m_editOffset, &size );
		m_caretOffsetPixel = size.width;

		int	width = getVisibleTextWidth( m_editCol );

		while( width < m_caretOffsetPixel )
		{
			hDC.getTextExtent( m_editCell->text[m_editOffset++], &size );
			m_caretOffsetPixel -= size.width;
			scrolled = true;
		}
	}

	m_editPos = newPosition;
	if( includeSelection )
		m_selPos = newPosition;
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
			nPos = m_vertOffset - rowHeight;
			break;
		case vscPAGE_UP:
			nPos = m_vertOffset - size.height;
			break;
		case vscLINE_DOWN:
			nPos = m_vertOffset + rowHeight;
			break;
		case vscPAGE_DOWN:
			nPos = m_vertOffset + size.height;
			break;
		case vscBOTTOM:
			nPos = m_totalHeight - size.height;
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
	else if( nPos > (maxNpos = m_totalHeight - size.height) )
		nPos = maxNpos;

	setVertScrollPos( nPos );
	m_vertOffset = nPos;
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
			nPos = m_horizOffset - rowHeight;
			break;
		case hscPAGE_LEFT:
			nPos = m_horizOffset - size.width;
			break;
		case hscLINE_RIGHT:
			nPos = m_horizOffset + rowHeight;
			break;
		case hscPAGE_RIGHT:
			nPos = m_horizOffset + size.width;
			break;
		case hscRIGHT:
			nPos = m_totalWidth - size.width;
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
	else if( nPos > (maxNpos = m_totalWidth - size.width) )
		nPos = maxNpos;

	setHorizScrollPos( nPos );
	m_horizOffset = nPos;
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
	assert( m_rowAttributes.size() == m_data.getNumRows() );
	assert( m_colAttributes.size() == m_data.getNumCols() );

	Size	size = getSize();

	int	fixedHeight = m_fixedRows ? m_rowAttributes[m_fixedRows-1].bottom	: 0;
	int	fixedWidth  = m_fixedCols ? m_colAttributes[m_fixedCols-1].right	: 0;

	if( getStyle() & gvGRID )
	{
		doEnterFunction("GridViewer::drawGrid");
		hDC.getPen().setStyle( Pen::psDot ).setWidth( 1 );

		unsigned col=0;
		for(
			gak::PODarray<ColAttribute>::iterator it = m_colAttributes.begin(), endIT = m_colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int	pos = it->left;
			if( col >= m_fixedCols )
				pos -= m_horizOffset;
			if( pos >= size.width )
				break;
			if( pos > 0 )
			{
				hDC.verticalLine( pos, 0, size.height );
			}
		}

		unsigned row=0;
		for(
			gak::PODarray<RowAttribute>::iterator it = m_rowAttributes.begin(), endIT = m_rowAttributes.end();
			it != endIT;
			++it, ++row
		)
		{
			int pos = it->top;
			if( row >= m_fixedRows )
				pos -= m_vertOffset;
			if( pos >= size.height )
				break;
			if( pos > 0  )
				hDC.horizontalLine( 0, size.width, pos );
		}
	}

	if( m_data.getNumRows() && m_data.getNumCols() )
	{
		doEnterFunction("showText");
		size_t col=0;
		hDC.selectFont( getFont() );
		hDC.clrBackgroundColor();
		for(
			gak::PODarray<ColAttribute>::iterator it = m_colAttributes.begin(), endIT = m_colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			RectBorder	rect;
			rect.left = it->left;
			rect.right = it->right;
			if( col >= m_fixedCols )
			{
				rect.left -= m_horizOffset;
				rect.right -= m_horizOffset;
			}
			if( rect.left > size.width )
/*v*/			break;

			if( rect.right > 0 )
			{
				size_t row=0;
				for(
					gak::PODarray<RowAttribute>::iterator it = m_rowAttributes.begin(), endIT = m_rowAttributes.end();
					it != endIT;
					++it, ++row
				)
				{
					rect.top = it->top;
					rect.bottom = it->bottom;
					if( m_vertOffset && row >= m_fixedRows )
					{
						rect.top -= m_vertOffset;
						rect.bottom -= m_vertOffset;
					}

					if( rect.top > size.height )
/*v*/					break;
					if( rect.bottom > 0 )
					{
						const TheCell &cellData = getCellData( col, row );

						if( row < m_fixedRows || col < m_fixedCols )
						{
							doEnterFunction("prepare Fixed Cells");
							if( row < m_fixedRows && col < m_fixedCols )
								hDC.removeClipping();
							else if( row < m_fixedRows )
								hDC.setClipping( fixedWidth, 0, size.width, size.height );
							else if( col < m_fixedCols )
								hDC.setClipping( 0, fixedHeight, size.width, size.height );


							hDC.getBrush().createSyscolor( scDIALOG_FACE );
							hDC.getPen().setStyle( Pen::psNull );
							hDC.rectangle( rect.left, rect.top, rect.right, rect.bottom );
						}
						else if( cellData.attribute.backgroundColor > 0 )
						{
							hDC.getBrush().create( cellData.attribute.backgroundColor );
							hDC.getPen().setStyle( Pen::psNull );
							hDC.rectangle( rect.left, rect.top, rect.right, rect.bottom );
						}
						else
						{
							hDC.setClipping( fixedWidth, fixedHeight, size.width, size.height );
						}


						if( m_editCell == &cellData )
						{
							drawEditCell( hDC, rect );
						}
						else if( !cellData.text.isEmpty() )
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
	MouseState		newState = m_mouseState;
	unsigned long	style = getStyle();

	if( (m_mouseState == msNORMAL || m_mouseState == msSTART_COL_SIZER) && (style & gvCOL_SIZABLE) )
	{
		newState = msNORMAL;
		size_t	col = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = m_colAttributes.begin(), endIT = m_colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int right = it->right;
			if( col >= m_fixedCols )
				right -= m_horizOffset;

			if( abs(int(right - position.x)) <= mouseSizeWidth )
			{
				newState = msSTART_COL_SIZER;
				m_sizerCol = col;
				break;
			}
		}
	}
	else if( m_mouseState == msDO_COL_SIZER )
	{
		int left = m_colAttributes[m_sizerCol].left;
		if( m_sizerCol >= m_fixedCols )
			left -= m_horizOffset;
		int newWidth = position.x - left;
		if( newWidth > mouseSizeWidth*2 )
		{
			setColWidth( m_sizerCol, newWidth );
			invalidateWindow();
		}
	}
	else if( !(style & gvCOL_SIZABLE) )
	{
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	if( newState != m_mouseState )
	{
		m_mouseState = newState;
		if( m_mouseState == msSTART_COL_SIZER || m_mouseState == msDO_COL_SIZER )
			SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );
		else
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	return psPROCESSED;
}

ProcessStatus GridViewer::handleLeftButton( LeftButton leftButton, WPARAM /*modifier*/, const Point &position )
{
	unsigned long	style = getStyle();

	if( leftButton == lbDOWN && m_mouseState == msSTART_COL_SIZER )
	{
		m_mouseState = msDO_COL_SIZER;
		captureMouse();
		return psPROCESSED;
	}
	else if( leftButton == lbDOWN && m_mouseState == msNORMAL && (style & (gvEDITABLE|gvCAPTION_EDITABLE)) )
	{
		TheCell	*lastEditCell = m_editCell;
		size_t lastCol = m_editCol;
		size_t lastRow = m_editRow;

		m_editCell = NULL;
		int		caretLeft;
		size_t	col = 0;
		for(
			gak::PODarray<ColAttribute>::iterator it = m_colAttributes.begin(), endIT = m_colAttributes.end();
			it != endIT;
			++it, ++col
		)
		{
			int left = it->left;
			int right = it->right;
			if( m_horizOffset && col >= m_fixedCols )
			{
				left -= m_horizOffset;
				right -= m_horizOffset;
			}

			if( left < position.x && position.x < right )
			{
				caretLeft = left;
/*v*/			break;
			}
		}
		if( col < m_data.getNumCols() )
		{
			size_t row=0;
			for(
				gak::PODarray<RowAttribute>::iterator it = m_rowAttributes.begin(), endIT = m_rowAttributes.end();
				it != endIT;
				++it, ++row
			)
			{
				bool	isCaption = (col < m_fixedCols || row < m_fixedRows);
				int	top = it->top;
				int	bottom = it->bottom;
				if( m_vertOffset && row >= m_fixedRows )
				{
					top -= m_vertOffset;
					bottom -= m_vertOffset;
				}
				if( top < position.y && position.y < bottom )
				{
					if( isCaption && !(style&gvCAPTION_EDITABLE) )
						break;
					if( !isCaption && !(style&gvEDITABLE) )
						break;

					focus();
					createCaret( caretLeft+1, top, bottom-top );
					m_editCol = col;
					m_editRow = row;
					m_editCell = &getCellData( col, row );
					m_selPos = m_editPos = 0;
					m_editOffset = 0;
					m_caretOffsetPixel = 0;
/*v*/				break;
				}
			}
		}
		if( lastEditCell != m_editCell )
		{
			if( lastEditCell )
			{
				getParent()->postMessage(WM_GRID_ITEM_EXIT, getId(), MAKELPARAM(lastCol, lastRow) );
			}

			if( m_editCell )
			{
				getParent()->postMessage(WM_GRID_ITEM_ENTER, getId(), MAKELPARAM(m_editCol, m_editRow) );
			}
			else
			{
				destroyCaret();
			}

			invalidateWindow();
		}

	}
	else if( leftButton == lbUP && m_mouseState == msDO_COL_SIZER )
	{
		m_mouseState = msSTART_COL_SIZER;
		releaseMouse();
		return psPROCESSED;
	}

	return psDO_DEFAULT;
}

ProcessStatus GridViewer::handleKeyDown( int key )
{
	if( m_editCell )
	{
		DrawDevice hDC( this );

		switch( key )
		{
			case VK_HOME:
				if( moveCursor( hDC, 0, !isShiftKey() ) )
					drawEditCell( hDC );
				break;
			case VK_END:
				if( moveCursor( hDC, m_editCell->text.strlen(), !isShiftKey() ) )
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
				if( m_editPos < m_editCell->text.strlen() || m_selPos < m_editCell->text.strlen() )
				{
					if( m_editPos != m_selPos )
					{
						if( isShiftKey() )
							copy();
						deleteSelection();
					}
					else
						m_editCell->text.delChar( m_editPos );
					drawEditCell( hDC );
					notifyParent();
				}
				break;
			case VK_BACK:
				if( m_editPos || m_selPos )
				{
					if( m_editPos != m_selPos )
						deleteSelection();
					else
					{
						moveCursorLeft( hDC, true );
						m_editCell->text.delChar( m_editPos );
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
				size_t	newEditRow = m_editRow+1;

				if( newEditRow >= m_data.getNumRows() )
				{
					if( getStyle() & gvROW_CREATE )
						setNumRows( newEditRow+1 );
					else
					{
						newEditRow = m_editRow;
					}
				}

				moveCursor( m_editCol, newEditRow );
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
	if( m_editCell )
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

			if( m_editPos != m_selPos )
				deleteSelection();

			m_editCell->text.insChar( m_editPos, char(c) );
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
	if( col < m_data.getNumCols() )
	{
		m_colAttributes[col].width = colWidth;
	}
	if( col == m_editCol && m_editCell )
	{
		DrawDevice dc( this );
		moveCursor( dc, m_editPos, false );
	}
	calcDimensions();
}

void GridViewer::clear( void )
{
	if( m_editCell && m_editPos != m_selPos )
	{
		size_t	start = math::min( m_editPos, m_selPos );
		size_t	end = math::max( m_editPos, m_selPos );

		m_editCell->text.delStr( start, end-start );
		m_selPos = m_editPos;
		DrawDevice dc( this );
		drawEditCell( dc );
		notifyParent();
	}
}

void GridViewer::copy ( void )
{
	if( m_editCell && m_editPos != m_selPos )
	{
		Clipboard	clip( this );
		if( clip )
		{
			size_t	start = math::min( m_editPos, m_selPos );
			size_t	end = math::max( m_editPos, m_selPos );

			STRING	text = m_editCell->text.subString( start, end-start );
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
	if( m_editCell )
	{
		Clipboard	clip( this, CF_TEXT );
		if( clip )
		{
			ClipboardMemory	clipData( CF_TEXT );
			if( clipData.is_open() )
			{
				size_t	start = math::min( m_editPos, m_selPos );
				size_t	end = math::max( m_editPos, m_selPos );
				size_t	len = end - start;

				if( len )
					m_editCell->text.delStr( start, end-start );

				STRING				text;
				iClipboardStream	clipStream( clipData );
				clipStream >> text;

				m_editCell->text.insStr( m_editPos, text );
				DrawDevice	hDC( this );
				moveCursor( hDC, m_editPos + text.strlen(), true ); 
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

