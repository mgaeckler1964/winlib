/*
		Project:		Windows Class Library
		Module:			gridView.h
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

#ifndef WINLIB_GRIDVIEW_H
#define WINLIB_GRIDVIEW_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/matrix.h>

#include <winlib/ChildWin.h>
#include <WINLIB/DEVICE.H>

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

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

const int mouseSizeWidth = 4;
const int cellPadding = 2;

// styles
const int gvGRID				= 0x0001;
const int gvEDITABLE			= 0x0002;
const int gvCAPTION_EDITABLE	= 0x0004;
const int gvCOL_SIZABLE			= 0x0008;
const int gvCOL_CREATE			= 0x0010;
const int gvROW_CREATE			= 0x0020;

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class GridViewer : public ChildWindow
{
	struct CellAttribute
	{
		long backgroundColor;
		CellAttribute() : backgroundColor(-1) {}
	};
	struct TheCell
	{
		CellAttribute	attribute;
		gak::STRING		text;
	};

	struct RowAttribute
	{
		unsigned	height;
		unsigned	top, bottom;
	};

	struct ColAttribute
	{
		unsigned	width;
		unsigned	left, right;
	};

	enum MouseState
	{
		msNORMAL, msSTART_COL_SIZER, msDO_COL_SIZER
	}							m_mouseState;
	size_t						m_sizerCol;
	TheCell						*m_editCell;
	size_t						m_editCol, m_editRow;
	size_t						m_editPos, m_editOffset, m_selPos;
	int							m_caretOffsetPixel;

	gak::PODarray<RowAttribute>	m_rowAttributes;
	gak::PODarray<ColAttribute>	m_colAttributes;
	gak::Matrix<TheCell>		m_data;
	size_t						m_fixedCols, m_fixedRows;
	unsigned					m_rowHeight, m_colWidth;

	int							m_horizOffset, m_vertOffset;
	int							m_totalHeight, m_totalWidth;

	static void registerClass( void );

	int getRowHeight( void ) const
	{
		return getFont().getFontSizePixel()+2*cellPadding;
	}
	void calcDimensions( const Size &size );
	void calcDimensions( void )
	{
		assert( m_rowAttributes.size() == m_data.getNumRows() );
		assert( m_colAttributes.size() == m_data.getNumCols() );

		calcDimensions( getClientSize() );
	}
	void updateCaretPos( void );
	void moveCursor( size_t newEditCol, size_t newEditRow );
	void moveCursorNextCell( void );
	void moveCursorPrevCell( void );

	bool moveCursorLeftWord( Device &hDC, bool includeSelection );
	bool moveCursorLeft( Device &hDC, bool includeSelection )
	{
		assert( m_editCell );

		size_t	newPosition = m_editPos;
		if( newPosition )
			newPosition--;

		return moveCursor( hDC, newPosition, includeSelection );
	}
	bool moveCursor( Device &hDC, size_t newPosition, bool includeSelection );
	bool moveCursorRightWord( Device &hDC, bool includeSelection );
	bool moveCursorRight( Device &hDC, bool includeSelection )
	{
		assert( m_editCell );

		size_t	newPosition = m_editPos;
		if( newPosition < m_editCell->text.strlen() )
			newPosition++;

		return moveCursor( hDC, newPosition, includeSelection );
	}

	RectBorder getCellVirtualPosition( size_t col, size_t row );
	RectBorder getCellScreenPosition( size_t col, size_t row );
	void drawCell( Device &hDC, const RectBorder &rect, const TheCell &cellData, size_t offset );
	void drawEditCell( Device &hDC, const RectBorder &rect );
	void drawEditCell( Device &hDC )
	{
		assert( m_editCell );

		RectBorder	rect = getCellScreenPosition( m_editCol, m_editRow );

		drawEditCell( hDC, rect );
	}
	void deleteSelection( void )
	{
		assert( m_editCell );
		size_t	start = gak::math::min( m_editPos, m_selPos );
		size_t	end = gak::math::max( m_editPos, m_selPos );

		m_editCell->text.delStr( start, end-start ); 
		m_editPos = m_selPos = start;
	}

	virtual STRING getWindowClassName( void ) const;

	virtual ProcessStatus handleVertScroll( VertScrollCode scrollCode, int nPos, HWND scrollBar );
	virtual ProcessStatus handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND scrollBar );
	virtual ProcessStatus handleRepaint( Device &hDC );
	virtual ProcessStatus handleResize( const Size &newSize );

	virtual ProcessStatus handleMouseMove( WPARAM modifier, const Point &position );
	virtual ProcessStatus handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position );

	virtual ProcessStatus handleKeyDown( int key );
	virtual ProcessStatus handleCharacterInput( int c );

	public:
	static const char className[];

	GridViewer( BasicWindow *owner ) 
	: ChildWindow( owner ), m_fixedCols( 0 ), m_fixedRows( 0 ), m_rowHeight(0), 
	m_colWidth(0), m_horizOffset( 0 ), m_vertOffset( 0 ), m_mouseState( msNORMAL ), 
	m_editCell( NULL )
	{
		registerClass();
	}
	void createData( size_t newNumCols, size_t newNumRows )
	{
		m_editCell = NULL;
		m_data.create( newNumCols, newNumRows );
		m_colAttributes.setSize( newNumCols );
		m_rowAttributes.setSize( newNumRows );
		calcDimensions();
	}
	void setNumCols( size_t newNumCols )
	{
		m_editCell = NULL;
		m_data.setNumCols( newNumCols );
		m_colAttributes.setSize( newNumCols );
		calcDimensions();
	}
	size_t getNumCols( void ) const
	{
		return m_data.getNumCols();
	}
	void setNumRows( size_t newNumRows )
	{
		m_editCell = NULL;
		m_data.setNumRows( newNumRows );
		m_rowAttributes.setSize( newNumRows );
		calcDimensions();
	}
	void removeRow( size_t row )
	{
		m_editCell = NULL;

		m_data.removeRow( row );
		m_rowAttributes.removeElementAt( row );

		calcDimensions();
	}
	size_t getNumRows( void ) const
	{
		return m_data.getNumRows();
	}
	void setFixedCols( size_t newNumCols )
	{
		assert( newNumCols < m_data.getNumCols() );
		m_fixedCols = newNumCols;
	}
	void setFixedRows( size_t newNumRows )
	{
		assert( newNumRows < m_data.getNumRows() );
		m_fixedRows = newNumRows;
	}
	void setColWidth( unsigned colWidth )
	{
		m_colWidth = colWidth;
		calcDimensions();
	}
	void setColWidth( size_t col, unsigned colWidth );
	int getTrueColWidth( size_t colNum )
	{
		const ColAttribute	&col = m_colAttributes[colNum];
		return col.right - col.left;
	}
	int getVisibleTextWidth( size_t colNum )
	{
		Size clientSize = getClientSize();

		const ColAttribute	&col = m_colAttributes[colNum];
		int visibleWidth = gak::math::min( col.right - col.left, clientSize.width - col.left );

		return visibleWidth -2*cellPadding;
	}
	size_t getEditCol( void ) const
	{
		return m_editCol;
	}
	size_t getEditRow( void ) const
	{
		return m_editRow;
	}
	const STRING &getEditCell( void )
	{
		return getCell( m_editCol, m_editRow );
	}
	TheCell &getCellData( size_t col, size_t row )
	{
		return m_data( col, row );
	}
	STRING &getCell( size_t col, size_t row )
	{
		return m_data( col, row ).text;
	}
	const STRING &getCell( size_t col, size_t row ) const
	{
		return m_data( col, row ).text;
	}
	void setCell( size_t col, size_t row, const STRING &cellData )
	{
		m_data( col, row ).text = cellData;
	}
	void setCellColor( size_t col, size_t row, int color )
	{
		m_data( col, row ).attribute.backgroundColor = color;
	}

	void clear( void );
	void copy ( void );
	void cut( void )
	{
		copy();
		clear();
	}
	void paste( void );
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

#endif //  WINLIB_GRIDVIEW_H
