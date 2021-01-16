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
	}							mouseState;
	size_t						sizerCol;
	STRING						*editCell;
	size_t						editCol, editRow;
	size_t						editPos, editOffset, selPos;
	int							caretOffsetPixel;

	gak::PODarray<RowAttribute>	rowAttributes;
	gak::PODarray<ColAttribute>	colAttributes;
	gak::Matrix<gak::STRING>	data;
	size_t						fixedCols, fixedRows;
	unsigned					rowHeight, colWidth;

	int							horizOffset, vertOffset;
	int							totalHeight, totalWidth;

	static void registerClass( void );

	int getRowHeight( void ) const
	{
		return getFont().getFontSizePixel()+2*cellPadding;
	}
	void calcDimensions( const Size &size );
	void calcDimensions( void )
	{
		assert( rowAttributes.size() == data.getNumRows() );
		assert( colAttributes.size() == data.getNumCols() );

		calcDimensions( getClientSize() );
	}
	void updateCaretPos( void );
	void moveCursor( size_t newEditCol, size_t newEditRow );
	void moveCursorNextCell( void );
	void moveCursorPrevCell( void );

	bool moveCursorLeftWord( Device &hDC, bool includeSelection );
	bool moveCursorLeft( Device &hDC, bool includeSelection )
	{
		assert( editCell );

		size_t	newPosition = editPos;
		if( newPosition )
			newPosition--;

		return moveCursor( hDC, newPosition, includeSelection );
	}
	bool moveCursor( Device &hDC, size_t newPosition, bool includeSelection );
	bool moveCursorRightWord( Device &hDC, bool includeSelection );
	bool moveCursorRight( Device &hDC, bool includeSelection )
	{
		assert( editCell );

		size_t	newPosition = editPos;
		if( newPosition < editCell->strlen() )
			newPosition++;

		return moveCursor( hDC, newPosition, includeSelection );
	}

	RectBorder getCellVirtualPosition( size_t col, size_t row );
	RectBorder getCellScreenPosition( size_t col, size_t row );
	void drawCell( Device &hDC, const RectBorder &rect, const STRING &cellData, size_t offset );
	void drawEditCell( Device &hDC, const RectBorder &rect );
	void drawEditCell( Device &hDC )
	{
		assert( editCell );

		RectBorder	rect = getCellScreenPosition( editCol, editRow );

		drawEditCell( hDC, rect );
	}
	void deleteSelection( void )
	{
		assert( editCell );
		size_t	start = gak::math::min( editPos, selPos );
		size_t	end = gak::math::max( editPos, selPos );

		editCell->delStr( start, end-start ); 
		editPos = selPos = start;
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
	: ChildWindow( owner ), fixedCols( 0 ), fixedRows( 0 ), rowHeight(0), colWidth(0), horizOffset( 0 ), vertOffset( 0 ), mouseState( msNORMAL ), editCell( NULL )
	{
		registerClass();
	}
	void createData( size_t newNumCols, size_t newNumRows )
	{
		editCell = NULL;
		data.create( newNumCols, newNumRows );
		colAttributes.setSize( newNumCols );
		rowAttributes.setSize( newNumRows );
		calcDimensions();
	}
	void setNumCols( size_t newNumCols )
	{
		editCell = NULL;
		data.setNumCols( newNumCols );
		colAttributes.setSize( newNumCols );
		calcDimensions();
	}
	size_t getNumCols( void ) const
	{
		return data.getNumCols();
	}
	void setNumRows( size_t newNumRows )
	{
		editCell = NULL;
		data.setNumRows( newNumRows );
		rowAttributes.setSize( newNumRows );
		calcDimensions();
	}
	void removeRow( size_t row )
	{
		editCell = NULL;

		data.removeRow( row );
		rowAttributes.removeElementAt( row );

		calcDimensions();
	}
	size_t getNumRows( void ) const
	{
		return data.getNumRows();
	}
	void setFixedCols( size_t newNumCols )
	{
		assert( newNumCols < data.getNumCols() );
		fixedCols = newNumCols;
	}
	void setFixedRows( size_t newNumRows )
	{
		assert( newNumRows < data.getNumRows() );
		fixedRows = newNumRows;
	}
	void setColWidth( unsigned colWidth )
	{
		this->colWidth = colWidth;
		calcDimensions();
	}
	void setColWidth( size_t col, unsigned colWidth );
	int getTrueColWidth( size_t colNum )
	{
		const ColAttribute	&col = colAttributes[colNum];
		return col.right - col.left;
	}
	int getVisibleTextWidth( size_t colNum )
	{
		Size clientSize = getClientSize();

		const ColAttribute	&col = colAttributes[colNum];
		int visibleWidth = gak::math::min( col.right - col.left, clientSize.width - col.left );

		return visibleWidth -2*cellPadding;
	}
	size_t getEditCol( void ) const
	{
		return editCol;
	}
	size_t getEditRow( void ) const
	{
		return editRow;
	}
	const STRING &getEditCell( void )
	{
		return getCell( editCol, editRow );
	}
	STRING &getCell( size_t col, size_t row )
	{
		return data( col, row );
	}
	const STRING &getCell( size_t col, size_t row ) const
	{
		return data( col, row );
	}
	void setCell( size_t col, size_t row, const STRING &cellData )
	{
		data( col, row ) = cellData;
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
