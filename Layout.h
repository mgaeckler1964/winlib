/*
		Project:		Windows Class Library
		Module:			Layout.h
		Description:	Layout managers
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


#ifndef WINLIB_LAYOUT_H
#define WINLIB_LAYOUT_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/set.h>
#include <gak/matrix.h>

#include <WINLIB/rectangle.h>

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

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

class BasicWindow;
typedef gak::UnorderedSet<BasicWindow*>	ChildWindows;

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

struct LayoutData
{
	enum Attachment { ATTACH_LEFT, ATTACH_TOP, ATTACH_RIGHT, ATTACH_BOTTOM, ATTACH_CLIENT } attach; 
	int			xPos, yPos;
	unsigned	width, height;
	unsigned	growWidth, growHeight;
	RectBorder	padding;

	static const char className[];

	static const char xPosAttr[];
	static const char yPosAttr[];
	static const char widthAttr[];
	static const char heightAttr[];

	static const char growWidthAttr[];
	static const char growHeightAttr[];

	static const char paddingLeft[];
	static const char paddingTop[];
	static const char paddingRight[];
	static const char paddingBottom[];

	static const char attachment[];

	LayoutData()
	{
		xPos = yPos = 0;
		width = height = 1;
		growWidth = growHeight =
		padding.left = padding.right =
		padding.top = padding.bottom = 0;
		attach = ATTACH_LEFT;
	}
	LayoutData *setX( int newX )
	{
		xPos = newX;
		return this;
	}
	LayoutData *setY( int newY )
	{
		yPos = newY;
		return this;
	}
	LayoutData *setWidth( int newWidth )
	{
		width = newWidth > 1 ? newWidth : 1;
		return this;
	}
	LayoutData *setHeight( int newHeight )
	{
		height = newHeight > 1 ? newHeight : 1;
		return this;
	}
	LayoutData *setGrowWidth( int newGrowWidth )
	{
		growWidth = newGrowWidth;
		return this;
	}
	LayoutData *setGrowHeight( int newGrowHeight )
	{
		growHeight = newGrowHeight;
		return this;
	}

	LayoutData *setPaddingLeft( int newPadding )
	{
		padding.left = newPadding;
		return this;
	}
	LayoutData *setPaddingRight( int newPadding )
	{
		padding.right = newPadding;
		return this;
	}
	LayoutData *setPaddingTop( int newPadding )
	{
		padding.top = newPadding;
		return this;
	}
	LayoutData *setPaddingBottom( int newPadding )
	{
		padding.bottom = newPadding;
		return this;
	}
	LayoutData *setPaddingX( int newPadding )
	{
		padding.left = padding.right = newPadding;
		return this;
	}
	LayoutData *setPaddingY( int newPadding )
	{
		padding.top = padding.bottom = newPadding;
		return this;
	}
	LayoutData *setPadding( int newPadding )
	{
		padding.left = padding.right = padding.top = padding.bottom = newPadding;
		return this;
	}
	LayoutData *setAttachment( Attachment newAttach )
	{
		if( newAttach < 0 )
		{
			newAttach = ATTACH_LEFT;
		}
		attach = newAttach;
		return this;
	}
};

struct LayoutManager
{
	RectBorder	margin;

	protected:
	bool	designerMode;

	public:
	static const char className[];
	static const char marginLeft[];
	static const char marginTop[];
	static const char marginRight[];
	static const char marginBottom[];

	LayoutManager( int marginLeft, int marginTop, int marginRight, int marginBottom )
	{
		designerMode = false;

		margin.left = marginLeft;
		margin.top = marginTop;
		margin.right = marginRight;
		margin.bottom = marginBottom;
	}
	LayoutManager( int marginVert, int marginHoriz )
	{
		designerMode = false;

		margin.left = margin.right = marginHoriz;
		margin.top = margin.bottom = marginVert;
	}
	LayoutManager( int margin=0 )
	{
		designerMode = false;

		this->margin.left = this->margin.right =
		this->margin.top = this->margin.bottom = margin;
	}
	void setDesigerMode( void )
	{
		designerMode = true;
	}
	virtual Size calcSize( const ChildWindows &children, const Size &newSize );
	virtual void doLayout( const ChildWindows &children, const Size &newSize ) = 0;

	virtual ~LayoutManager();
};

class SingleChildManager : public LayoutManager
{
	public:
	SingleChildManager( int marginLeft, int marginTop, int marginRight, int marginBottom ) 
	: LayoutManager( marginLeft, marginTop, marginRight, marginBottom )
	{
	}
	SingleChildManager( int marginVert, int marginHoriz )
	: LayoutManager( marginVert, marginHoriz )
	{
	}
	SingleChildManager( int margin=0 )
	: LayoutManager( margin )
	{
	}
	virtual void doLayout( const ChildWindows &children, const Size &newSize );
};

class TableManager : public LayoutManager
{
	struct Dimension
	{
		int			pos;
		unsigned	length, grow;

		Dimension()
		{
			pos = length = grow = 0;
		}
	};

	gak::PODarray<Dimension>		colDimensions, rowDimensions;
	gak::PODmatrix<BasicWindow*>	childrenMatrix;

	Size doLayout( const ChildWindows &children, const Size &newSize, bool resize );

	public:
	TableManager( int marginLeft, int marginTop, int marginRight, int marginBottom ) 
	: LayoutManager( marginLeft, marginTop, marginRight, marginBottom )
	{
	}
	TableManager( int marginVert, int marginHoriz )
	: LayoutManager( marginVert, marginHoriz )
	{
	}
	TableManager( int margin=0 )
	: LayoutManager( margin )
	{
	}

	virtual Size calcSize( const ChildWindows &children, const Size &newSize );
	virtual void doLayout( const ChildWindows &children, const Size &newSize );

	size_t getNumColumns( void ) const
	{
		return colDimensions.size();
	}
	int getColWidth( size_t col ) const
	{
		return colDimensions[col].length;
	}
	int getColLeft( size_t col ) const
	{
		return colDimensions[col].pos;
	}

	size_t getNumRows( void ) const
	{
		return rowDimensions.size();
	}
	int getRowHeight( size_t row ) const
	{
		return rowDimensions[row].length;
	}
	int getRowTop( size_t row ) const
	{
		return rowDimensions[row].pos;
	}
	BasicWindow	*getChildAt( size_t col, size_t row )
	{
		if( col < childrenMatrix.getNumCols() && row < childrenMatrix.getNumRows() )
			return childrenMatrix( col, row );
		return NULL;
	}
	void extract( 
		size_t leftCol, size_t upperRow, size_t rightCol, size_t lowerRow, 
		gak::PODmatrix<BasicWindow*> *result 
	)
	{
		if( leftCol < childrenMatrix.getNumCols() && upperRow < childrenMatrix.getNumRows() )
		{
			if( rightCol > childrenMatrix.getNumCols() )
				rightCol = childrenMatrix.getNumCols();
			if( lowerRow > childrenMatrix.getNumRows() )
				lowerRow = childrenMatrix.getNumRows();

			childrenMatrix.extract( leftCol, upperRow, rightCol, lowerRow, result );
		}
	}

	void getRow( size_t row, gak::PODarray<BasicWindow*> *rowData )
	{
		childrenMatrix.getRow(row, rowData );
	}
	void getColumn( size_t col, gak::PODarray<BasicWindow*> *colData )
	{
		childrenMatrix.getColumn( col, colData );
	}

};

class RowManager : public LayoutManager
{
	public:
	RowManager( int marginLeft, int marginTop, int marginRight, int marginBottom ) 
	: LayoutManager( marginLeft, marginTop, marginRight, marginBottom )
	{
	}
	RowManager( int marginVert, int marginHoriz )
	: LayoutManager( marginVert, marginHoriz )
	{
	}
	RowManager( int margin=0 )
	: LayoutManager( margin )
	{
	}

	virtual void doLayout( const ChildWindows &children, const Size &newSize );
};

class ColManager : public LayoutManager
{
	public:
	ColManager( int marginLeft, int marginTop, int marginRight, int marginBottom ) 
	: LayoutManager( marginLeft, marginTop, marginRight, marginBottom )
	{
	}
	ColManager( int marginVert, int marginHoriz )
	: LayoutManager( marginVert, marginHoriz )
	{
	}
	ColManager( int margin=0 )
	: LayoutManager( margin )
	{
	}

	virtual void doLayout( const ChildWindows &children, const Size &newSize );
};

class AttachmentManager : public LayoutManager
{
	public:
	AttachmentManager( int marginLeft, int marginTop, int marginRight, int marginBottom ) 
	: LayoutManager( marginLeft, marginTop, marginRight, marginBottom )
	{
	}
	AttachmentManager( int marginVert, int marginHoriz )
	: LayoutManager( marginVert, marginHoriz )
	{
	}
	AttachmentManager( int margin=0 )
	: LayoutManager( margin )
	{
	}

	virtual void doLayout( const ChildWindows &children, const Size &newSize );
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

#endif