/*
		Project:		Windows Class Library
		Module:			Layout.cpp
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


// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <WINLIB/BASICWIN.H>
#include <WINLIB/Layout.h>

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

const char LayoutData::className[] = "layout";

const char LayoutData::xPosAttr[] = "x";
const char LayoutData::yPosAttr[] = "y";
const char LayoutData::widthAttr[] = "width";
const char LayoutData::heightAttr[] = "height";

const char LayoutData::growWidthAttr[] = "growWidth";
const char LayoutData::growHeightAttr[] = "growHeight";

const char LayoutData::paddingLeft[] = "paddingLeft";
const char LayoutData::paddingTop[] = "paddingTop";
const char LayoutData::paddingRight[] = "paddingRight";
const char LayoutData::paddingBottom[] = "paddingBottom";

const char LayoutData::attachment[] = "attachment";

const char LayoutManager::className[] = "layoutManager";
const char LayoutManager::marginLeft[] = "marginLeft";
const char LayoutManager::marginTop[] = "marginTop";
const char LayoutManager::marginRight[] = "marginRight";
const char LayoutManager::marginBottom[] = "marginBottom";

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

LayoutManager::~LayoutManager()
{
}

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

Size TableManager::doLayout( const ChildWindows &children, const Size &newSize, bool resize )
{
	Size wantedSize = newSize;

	colDimensions.clear();
	rowDimensions.clear();

	/*
		determine layout data for each row and col
	*/
	for( size_t i=0; i<children.size(); i++ )
	{
		BasicWindow			*child = children[i];
		const LayoutData	*childLayout = child->getLayoutData();
		if( childLayout && child->isVisible() )
		{
//			int		row = childLayout->yPos;
			Size	childSize = child->getSize();

			unsigned width = (childSize.width + childLayout->padding.left + childLayout->padding.right) / childLayout->width;
			unsigned height = (childSize.height + childLayout->padding.top + childLayout->padding.bottom) / childLayout->height;

			for( unsigned j=0; j<childLayout->width; j++ )
			{
				size_t		col = childLayout->xPos+j;
				Dimension	&colDim = colDimensions[col];

				if( !childLayout->growWidth )
				{
					if( colDim.length < width )
						colDim.length = width;
				}
				else
				{
					if( colDim.grow < childLayout->growWidth )
						colDim.grow = childLayout->growWidth;
				}
			}

			for( unsigned j=0; j<childLayout->height; j++ )
			{
				size_t		row = childLayout->yPos+j;
				Dimension	&rowDim = rowDimensions[row];

				if( !childLayout->growHeight )
				{
					if( rowDim.length < height )
						rowDim.length = height;
				}
				else
				{
					if( rowDim.grow < childLayout->growHeight )
						rowDim.grow = childLayout->growHeight;
				}
			}
		}
	}

	// calculate the col widths that can grow all positions
	int colGrowSum = 0;
	int	width = newSize.width - margin.left - margin.right;
	for( size_t i=0; i<colDimensions.size(); i++ )
	{
		Dimension &colDim = colDimensions[i];
		if( colDim.grow )
			colGrowSum += colDim.grow;
		else
		{
			if( designerMode && !colDim.length )
				colDim.length = 20;
			width -= colDim.length;
		}
	}

	double colFactor = colGrowSum ? width/colGrowSum : 0;
	int colPos = margin.left;
	for( size_t i=0; i<colDimensions.size(); i++ )
	{
		Dimension &colDim = colDimensions[i];
		colDim.pos = colPos;
		if( colDim.grow )
			colDim.length = int(colDim.grow * colFactor);
		colPos += colDim.length;
	}

	// calculate the row heights that can grow and all positions
	int rowGrowSum = 0;
	int	height = newSize.height - margin.top - margin.bottom;
	for( size_t i=0; i<rowDimensions.size(); i++ )
	{
		Dimension &rowDim = rowDimensions[i];
		if( rowDim.grow )
			rowGrowSum += rowDim.grow;
		else
		{
			if( designerMode && !rowDim.length )
				rowDim.length = 20;
			height -= rowDim.length;
		}
	}

	double rowFactor = rowGrowSum ? height/rowGrowSum : 0;
	int rowPos = margin.top;
	for( size_t i=0; i<rowDimensions.size(); i++ )
	{
		Dimension &rowDim = rowDimensions[i];
		rowDim.pos = rowPos;
		if( rowDim.grow )
			rowDim.length = int(rowDim.grow * rowFactor);
		rowPos += rowDim.length;
	}


	// now we can move & resize the child windows and setup the matrix
	childrenMatrix.create( 
		colDimensions.size(),
		rowDimensions.size()
	);
	for( size_t i=0; i<children.size(); i++ )
	{
		BasicWindow			*child = children[i];
		const LayoutData	*childLayout = child->getLayoutData();
		if( childLayout && child->isVisible() )
		{
			int					col = childLayout->xPos;
			int					row = childLayout->yPos;
			const Dimension		&colDim = colDimensions[col];
			const Dimension		&rowDim = rowDimensions[row];

			Size				childSize = child->getSize();

			if( childLayout->width > 1 || childLayout->height > 1 )
				childrenMatrix.fill( col, row, col+childLayout->width, row+childLayout->height, child );
			else
				childrenMatrix( col, row ) = child;

			int xPos = colDim.pos + childLayout->padding.left;
			int yPos = rowDim.pos + childLayout->padding.top;
			int width, height;

			if( childLayout->growWidth )
			{
				width = 0;
				for( unsigned j=0; j<childLayout->width; j++ )
				{
					int					col = childLayout->xPos+j;
					const Dimension		&colDim = colDimensions[col];
					width += colDim.length;
				}
				
				width -= childLayout->padding.left + childLayout->padding.right;
			}
			else
				width = childSize.width;
			if( childLayout->growHeight )
			{
				height = 0;
				for( unsigned j=0; j<childLayout->height; j++ )
				{
					int					row = childLayout->yPos+j;
					const Dimension		&rowDim = rowDimensions[row];
					height += rowDim.length;
				}
				height -= childLayout->padding.top + childLayout->padding.bottom;
			}
			else
				height = childSize.height;
			if( resize )
			{
				if( width > 0 && height > 0 )
				{
					child->sizeNmove( xPos, yPos, width, height );
				}
			}
			else
			{
				int newWidth, newHeight;
				if( width > 0 && (newWidth = xPos + width) > wantedSize.width )
				{
					wantedSize.width = newWidth;
				} 
				if( height > 0 && (newHeight = yPos + height) > wantedSize.height )
				{
					wantedSize.height = newHeight;
				} 
			}
		}
	}

	return wantedSize;
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

Size LayoutManager::calcSize( const ChildWindows &/*children*/, const Size &newSize )
{
	return newSize;
}
   
void SingleChildManager::doLayout( const ChildWindows &children, const Size &newSize )
{
	RectSize	freeArea( newSize );
	freeArea.substractBorder( margin );

	if( children.size() == 1 )
	{
		BasicWindow			*child = children[0];
		LayoutData	*layoutData = child->getLayoutData();
		if( layoutData )
			freeArea.substractBorder( layoutData->padding );
		child->sizeNmove( freeArea );
	}
}

Size TableManager::calcSize( const ChildWindows &children, const Size &newSize )
{
	return doLayout( children, newSize, false );
}

void TableManager::doLayout( const ChildWindows &children, const Size &newSize )
{
	doLayout( children, newSize, true );
}

void RowManager::doLayout( const ChildWindows &children, const Size &newSize )
{
	size_t	numVisible = 0;
	for( size_t i=0; i<children.size(); i++ )
	{
		if( children[i]->isVisible() )
		{
			numVisible++;
		}
	}

	if( numVisible )
	{
		RectSize	freeArea( newSize );
		freeArea.substractBorder( margin );
		freeArea.width /= int(numVisible);

		for( size_t i=0; i<children.size(); i++ )
		{
			BasicWindow *child = children[i];
			if( child->isVisible() )
			{
				RectSize	childDim = freeArea;
				LayoutData	*layoutData = child->getLayoutData();
				if( layoutData )
					childDim.substractBorder( layoutData->padding );

				child->sizeNmove( childDim );
				freeArea.x += freeArea.width;
			}
		}
	}
}

void ColManager::doLayout( const ChildWindows &children, const Size &newSize )
{
	size_t	numVisible = 0;
	for( size_t i=0; i<children.size(); i++ )
		if( children[i]->isVisible() )
			numVisible++;

	RectSize	freeArea( newSize );
	freeArea.substractBorder( margin );
	freeArea.height /= int(numVisible);

	for( size_t i=0; i<children.size(); i++ )
	{
		BasicWindow *child = children[i];
		if( child->isVisible() )
		{
			RectSize	childDim = freeArea;
			LayoutData	*layoutData = child->getLayoutData();
			if( layoutData )
				childDim.substractBorder( layoutData->padding );

			child->sizeNmove( childDim );

			freeArea.y += freeArea.height;
		}
	}
}

void AttachmentManager::doLayout( const ChildWindows &children, const Size &newSize )
{
	int	left = margin.left;
	int top = margin.top;
	int childHeight = newSize.height-margin.top-margin.bottom;
	int childWidth = newSize.width - margin.left - margin.right;

	int x, y, width, height;

	for( size_t i=0; childHeight > 0 && childWidth > 0 && i<children.size(); i++ )
	{
		BasicWindow *child = children[i];
		LayoutData *layoutData = child->getLayoutData();
		if( layoutData && child->isVisible() )
		{
			const Size	size = child->getSize();
			switch( layoutData->attach )
			{
				case LayoutData::ATTACH_TOP:
				{
					x = left + layoutData->padding.left;
					y = top + layoutData->padding.top;
					width = layoutData->growWidth
						? childWidth - layoutData->padding.left - layoutData->padding.right
						: size.width
					;
					height = size.height;
					int tmp = size.height + layoutData->padding.top + layoutData->padding.bottom;
					top += tmp;
					childHeight -= tmp;
					break;
				}
				case LayoutData::ATTACH_LEFT:
				{
					x = left + layoutData->padding.left;
					y = top + layoutData->padding.top;
					width = size.width; 
					height = layoutData->growHeight 
						? childHeight - layoutData->padding.top - layoutData->padding.bottom 
						: size.height
					;
					int	tmp = size.width + layoutData->padding.left + layoutData->padding.right;
					left += tmp;
					childWidth -= tmp;
					break;
				}
				case LayoutData::ATTACH_RIGHT:
					x = left + childWidth - layoutData->padding.right - size.width;
					y = top + layoutData->padding.top;
					width = size.width; 
					height = layoutData->growHeight 
						? childHeight - layoutData->padding.top - layoutData->padding.bottom 
						: size.height
					;

					childWidth -= layoutData->padding.left + layoutData->padding.right + width;
					break;

				case LayoutData::ATTACH_BOTTOM:
					x = left + layoutData->padding.left;
					y = top + childHeight - layoutData->padding.bottom - size.height;
					width = layoutData->growWidth
						? childWidth - layoutData->padding.left - layoutData->padding.right
						: size.width
					;
					height = size.height;
					childHeight -= layoutData->padding.top + layoutData->padding.bottom + height;
					break;

				case LayoutData::ATTACH_CLIENT:
				default:
					assert( layoutData->attach == LayoutData::ATTACH_CLIENT );
					x = left + layoutData->padding.left;
					y = top + layoutData->padding.top;
					width = layoutData->growWidth
						? childWidth - layoutData->padding.left - layoutData->padding.right
						: size.width
					;
					height = layoutData->growHeight 
						? childHeight - layoutData->padding.top - layoutData->padding.bottom 
						: size.height
					;
					break;
			}
			child->sizeNmove( x, y, width, height );
		}
	}
}


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

