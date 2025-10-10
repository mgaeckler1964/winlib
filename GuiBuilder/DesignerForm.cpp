/*
		Project:		GUI Builder
		Module:			DesignerForm.cpp
		Description:	The form designer
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

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/numericString.h>

#include <WINLIB/BASICWIN.H>

#include "DesignerForm.h"
#include "GuiBuilderWindow.h"

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

void DesignerForm::internalSelect( BasicWindow *control )
{
	selectControl( control, false );
	m_theGuiBuilder->selectControl( control, FromDesignerWindow );
}

void DesignerForm::internalUnselect( BasicWindow *control )
{
	unselectControl( control );
	m_theGuiBuilder->unselectControl( control );
}

void DesignerForm::startSelect( const Point &position )
{
	resetClickStatus();
	BasicWindow	*child = findChild( position );
	if( child )
	{
		if( !isSelected( child ) )
		{
			internalClearSelected();
			internalSelect( child );
			invalidateWindow();
		}
		m_lastMousePos = position;
		m_startDraging = true;
		if( child->getLayoutData() )
		{
			m_currentFrame = dynamic_cast<const CallbackWindow *>( child->getParent() );
			if( m_currentFrame )
			{
				m_tableManager = dynamic_cast<TableManager *>( m_currentFrame->getLayoutManager() );
				m_lastChildClick = child;		// we can move this child, only
			}
		}
	}
	else
	{
		internalClearSelected();
		internalSelect( this );
		invalidateWindow();
	}
}
void DesignerForm::endSelect( WPARAM modifier, const Point &position )
{
	BasicWindow	*child = findChild( position );
	if( child )
	{
		if( modifier & MK_SHIFT )
		{
			if( isSelected( child ) )
			{
				internalUnselect( child );
			}
			else
			{
				internalSelect( child );
			}
			invalidateWindow();
		}
		else
		{
			m_theGuiBuilder->loadChildProperties( child );
		}
	}
}

bool DesignerForm::checkUnique( const PODmatrix<BasicWindow*>::ArrayType &line, BasicWindow *child )
{
	for( size_t i=0; i<line.size(); i++ )
	{
		BasicWindow *otherChild = line[i];
		if( otherChild && otherChild != child )
		{
			return false;
		}
	}

	return true;
}

/*
	check, whether the rectangle is big enough, to hold all children windows
	if not increase the rectangle
*/
bool DesignerForm::checkRectangle( const PODmatrix<BasicWindow*>::ArrayType &children, RectBorder *rectangle )
{
	bool changed = false;

	for( size_t i=0; i<children.size(); i++ )
	{
		BasicWindow *child;
		LayoutData *layoutData;
		if( (child = children[i]) != NULL && (layoutData = child->getLayoutData()) != NULL )
		{
			if( rectangle->left > layoutData->xPos )
			{
				changed = true;
				rectangle->left = layoutData->xPos;
			}
			if( rectangle->top > layoutData->yPos )
			{
				changed = true;
				rectangle->top = layoutData->yPos;
			}
			int right = layoutData->xPos + layoutData->width;
			if( rectangle->right < right )
			{
				changed = true;
				rectangle->right = right;
			}
			int bottom = layoutData->yPos + layoutData->height;
			if( rectangle->bottom < bottom )
			{
				changed = true;
				rectangle->bottom = bottom;
			}
		}
	}

	return changed;
}

void DesignerForm::findChildren( RectBorder *area, PODmatrix<BasicWindow*> *children )
{
	assert( m_tableManager );
	do
	{
		m_tableManager->extract( area->left, area->top, area->right, area->bottom, children );
	}
	while( checkRectangle( children->getArray(), area ) );
}

void DesignerForm::restoreChildren()
{
	bool restored;

	do
	{
		// perform a layout, so that the chnildren matrix is uptodate
		restored = false;
		m_currentFrame->doLayout();
		const ChildWindows	&children = m_currentFrame->getChildren();
		for(
			ChildWindows::const_iterator it = children.cbegin(),  endIT = children.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow *child = *it;
			if( child != m_lastChildClick )	// do never restore the selected child
			{
				LayoutData *layoutData = child->getLayoutData();
				if( layoutData )
				{
					xml::Element *resource = child->getResource();
					xml::Element	*layout = resource->getElement( LayoutData::className );
					int	xPos = layout->getAttribute( LayoutData::xPosAttr ).getValueN<unsigned>();
					int	yPos = layout->getAttribute( LayoutData::yPosAttr ).getValueN<unsigned>();
					if( layoutData->xPos != xPos || layoutData->yPos != yPos )
					{
						assert( m_tableManager );
						// this item was temprary moved -> try to restore position
						PODmatrix<BasicWindow*>	area;
						m_tableManager->extract( 
							xPos, yPos, xPos + layoutData->width, yPos + layoutData->height, 
							&area 
						);
						if( checkUnique( area.getArray(), child ) )
						{
							// yes, we can restore that child
							layoutData->xPos = xPos;
							layoutData->yPos = yPos;
							restored = true;
						}
					}
				}
			}
		}
	} while( restored );
}

void DesignerForm::performTableDrag( const Point &position )
{
	assert( m_tableManager );
	assert( m_lastChildClick );

	Point	childPos;
	mouseToChildPosition( position, m_lastChildClick->getParent(), &childPos );

	int	col, row;
	int colLeft, rowTop;
	int colRight, rowBottom;
	size_t	numColums = m_tableManager->getNumColumns();
	size_t	numRows = m_tableManager->getNumRows();
	for( col=0; size_t(col)<numColums; col++ )
	{
		colLeft = m_tableManager->getColLeft( col );
		colRight = m_tableManager->getColLeft( col ) + m_tableManager->getColWidth( col );
		if( childPos.x < colLeft )
		{
			col--;
			break;
		}
		if( childPos.x >= colLeft && childPos.x < colRight )
			break;
	}
	for( row=0; size_t(row)<numRows; row++ )
	{
		rowTop = m_tableManager->getRowTop( row );
		rowBottom = m_tableManager->getRowTop( row ) + m_tableManager->getRowHeight( row );
		if( childPos.y < rowTop )
		{
			row--;
			break;
		}
		if( childPos.y >= rowTop && childPos.y < rowBottom )
			break;
	}

	LayoutData	*layoutData = m_lastChildClick->getLayoutData();
	if( layoutData->xPos != col || layoutData->yPos != row )
	{
		m_tableMoved = true;
		if( col < 0							// move left outside current frame
		|| col >= int(numColums) )			// move right outside current frame
		 
		{
			PODarray<BasicWindow*>	colData;
			m_tableManager->getColumn( layoutData->xPos, &colData );
			// check if selected control is on edge of window
			if( checkUnique( colData, m_lastChildClick ) )
/***/			return;
		}
		if( row < 0 					// move top outside current frame
		|| row >= int(numRows) )		// move bottom outside current frame
		{
			PODarray<BasicWindow*>	rowData;
			m_tableManager->getRow( layoutData->yPos, &rowData );
			// check if selected control is on edge of window
			if( checkUnique( rowData, m_lastChildClick ) )
			{
/***/			return;
			}
		}

		bool	swap = true;

		if( col >= 0 )
			layoutData->xPos = col;
		else
		{
			/*
				we are outside the left frame
				move all children right
			*/
			const ChildWindows &children = m_currentFrame->getChildren();
			for(
				ChildWindows::const_iterator it = children.cbegin(),  endIT = children.cend();
				it != endIT;
				++it
			)
			{
				BasicWindow *child = *it;
		
				LayoutData		*layoutData = child->getLayoutData();
				if( layoutData )
				{
					layoutData->xPos -= col;
				}
			}
			swap = false;
			layoutData->xPos = 0;
		}
		if( row >= 0 )
			layoutData->yPos = row;
		else
		{
			/*
				we are outside the top frame
				move all children down
			*/
			const ChildWindows	&children = m_currentFrame->getChildren();
			for(
				ChildWindows::const_iterator it = children.cbegin(),  endIT = children.cend();
				it != endIT;
				++it
			)
			{
				BasicWindow *child = *it;
		
				LayoutData		*layoutData = child->getLayoutData();
				if( layoutData )
				{
					layoutData->yPos -= row;
				}
			}
			swap = false;
			layoutData->yPos = 0;
		}

		if( swap )
		{
			/*
				check whether the target area is empty
			*/
			PODmatrix<BasicWindow*>		oldChildren;
			m_tableManager->extract( 
				layoutData->xPos, 
				layoutData->yPos, 
				layoutData->xPos+layoutData->width,
				layoutData->yPos + layoutData->height, 
				&oldChildren 
			);
			if( !checkUnique( oldChildren.getArray(), m_lastChildClick ) )
			{
				RectBorder	area;

				area.left = layoutData->xPos;
				area.top = layoutData->yPos;
				area.right = layoutData->xPos + int(numColums);
				area.bottom = layoutData->yPos + layoutData->height;

				// no -> move them right
				PODarray<BasicWindow*>		changed;
				findChildren( &area, &oldChildren );
				const Array<BasicWindow*,PODallocator<BasicWindow*> >	&oldArray = oldChildren.getArray();
				for( size_t i=0; i<oldArray.size(); i++ )
				{
					BasicWindow *child = oldArray[i];
					if( child && child != m_lastChildClick && !changed.hasElement( child ) )
					{
						child->getLayoutData()->xPos += layoutData->width + layoutData->xPos - area.left;
						changed += child;
					}
				}
				restoreChildren();
			}
		}
		m_currentFrame->doLayout();
	}
}

void DesignerForm::performStandardDrag( const Point &position )
{
	doEnterFunction("DesignerForm::performStandardDrag");

	int xOffset = position.x - m_lastMousePos.x;
	int yOffset = position.y - m_lastMousePos.y;
	m_lastMousePos = position;
	for( 
		ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow *control = *it;
		Point		pos;
		if( m_relativePositions.hasElement( control ) )
		{
			pos = m_relativePositions[control];
		}
		else
		{
			pos = control->getRelativePosition();
			if( !m_oldPositions.hasElement( control ) )
			{
				m_oldPositions[control] = pos;
			}
		}

		pos.x += xOffset;
		pos.y += yOffset;
		m_relativePositions[control] = pos;
		if( s_gridSize )
		{
			pos.x += s_gridSize/2;
			pos.y += s_gridSize/2;
			pos.x -= pos.x % s_gridSize;
			pos.y -= pos.y % s_gridSize;
		}
		control->move( pos );
		xml::Element *resource = control->getResource();
		resource->setIntegerAttribute( LayoutData::xPosAttr, pos.x );
		resource->setIntegerAttribute( LayoutData::yPosAttr, pos.y );
		if( m_oldPositions[control] != pos )
		{
			m_theGuiBuilder->setChangedFlag();
		}
	}
}

void DesignerForm::performResize( const Point &position )
{
	doEnterFunction("DesignerForm::performResize");
	int xOffset = position.x - m_lastMousePos.x;
	int yOffset = position.y - m_lastMousePos.y;
	m_lastMousePos = position;
	for( 
		ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow *control = *it;
		RectBorder	rect = control->getRelativeRectangle();
		if( m_sizeMode == smLOWERRIGHT )
		{
			rect.right += xOffset;
			rect.bottom += yOffset;
		}
		else if( m_sizeMode == smUPPERRIGHT )
		{
			rect.right += xOffset;
			rect.top += yOffset;
		}
		else if( m_sizeMode == smUPPERLEFT )
		{
			rect.left += xOffset;
			rect.top += yOffset;
		}
		else if( m_sizeMode == smLOWERLEFT )
		{
			rect.left += xOffset;
			rect.bottom += yOffset;
		}
		control->sizeNmove( rect );

		xml::Element *resource = control->getResource();
		resource->setIntegerAttribute( LayoutData::xPosAttr, rect.left );
		resource->setIntegerAttribute( LayoutData::yPosAttr, rect.top );
		resource->setIntegerAttribute( LayoutData::widthAttr, rect.getWidth() );
		resource->setIntegerAttribute( LayoutData::heightAttr, rect.getHeight() );
		m_theGuiBuilder->setChangedFlag();
	}
}

void DesignerForm::endTableDrag()
{
	const ChildWindows &children = m_currentFrame->getChildren();
	for(
		ChildWindows::const_iterator it = children.cbegin(),  endIT = children.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow *child = *it;
		
		LayoutData		*layoutData = child->getLayoutData();
		if( layoutData )
		{
			xml::Element		*resource = child->getResource();
			xml::Element		*layout = resource->getElement( LayoutData::className );
			layout->setIntegerAttribute( LayoutData::xPosAttr, layoutData->xPos );
			layout->setIntegerAttribute( LayoutData::yPosAttr, layoutData->yPos );
		}
	}
}

void DesignerForm::endDragging()
{
	if( s_gridSize && !m_tableManager )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow *control = *it;
			RectBorder	rect = control->getRelativeRectangle();

			rect.left += s_gridSize / 2;
			rect.top += s_gridSize / 2;
			rect.right += s_gridSize / 2;
			rect.bottom += s_gridSize / 2;

			rect.left -= rect.left % s_gridSize;
			rect.top -= rect.top % s_gridSize;
			rect.right -= rect.right % s_gridSize;
			rect.bottom -= rect.bottom % s_gridSize;
			control->sizeNmove( rect );

			xml::Element *resource = control->getResource();
			resource->setIntegerAttribute( LayoutData::xPosAttr, rect.left );
			resource->setIntegerAttribute( LayoutData::yPosAttr, rect.top );
			resource->setIntegerAttribute( LayoutData::widthAttr, rect.getWidth() );
			if( resource->getAttribute( TYPE_ATTR ) != ComboBox::className )
			{
				resource->setIntegerAttribute( LayoutData::heightAttr, rect.getHeight() );
			}
		}
	}
}

void DesignerForm::createControl( const Point *position )
{
	STRING			newCaption, type, newName;
	BasicWindow		*newWindow = NULL;
	ControlWindow	*newControl = NULL;
	ChildWindow		*newChild = NULL;
	CallbackWindow	*parent = findFrame( *position );
	Point			newChildPos;

	if( !parent )
	{
		parent = this;
	}
	else
	{
		// mouse position is relative to me
		// we must calculate the position relaitve to the frame
		mouseToChildPosition( *position, parent, &newChildPos );
		position = &newChildPos;
	}

	xml::Element	*resource = parent->getResource();
	xml::Element	*newResource = resource->addObject( new xml::Any( CHILD_TAG ) );
	if(m_theGuiBuilder->getCurrentId() == BUTTON_PUSH )
	{
		newWindow = newControl = new PushButton( parent );
		newCaption = PushButton::className;
		type = PushButton::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == CHECKBOX_PUSH )
	{
		newWindow = newControl = new CheckBox( parent );
		newCaption = CheckBox::className;
		type = CheckBox::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == RADIOBUTTON_PUSH )
	{
		newWindow = newControl = new RadioButton( parent );
		newCaption = RadioButton::className;
		type = RadioButton::className;
	}
	else if( m_theGuiBuilder->getCurrentId() == GROUPBOX_PUSH )
	{
		newWindow = newControl = new GroupBox( parent );
		newCaption = GroupBox::className;
		type = GroupBox::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == LABEL_PUSH )
	{
		newWindow = newControl = new Label( parent );
		newCaption = Label::className;
		type = Label::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == LIST_PUSH )
	{
		newWindow = newControl = new ListBox( parent );
		newCaption = "";
		type = ListBox::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == TREEVIEW_PUSH )
	{
		newWindow = newControl = new TreeView( parent );
		newCaption = "";
		type = TreeView::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == TRACKBAR_PUSH )
	{
		newWindow = newControl = new TrackBar( parent );
		newCaption = "";
		type = TrackBar::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == DATETIMEPICKER_PUSH )
	{
		newWindow = newControl = new DateTimePicker( parent );
		newCaption = "";
		type = DateTimePicker::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == SCROLLBAR_PUSH )
	{
		newWindow = newControl = new ScrollBar( parent );
		newCaption = "";
		type = ScrollBar::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == UPDPOWN_PUSH )
	{
		newWindow = newControl = new UpDownButton( parent );
		newCaption = "";
		type = UpDownButton::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == COMBO_PUSH )
	{
		newWindow = newControl = new ComboBox( parent );
		newCaption = "";
		type = ComboBox::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == EDIT_PUSH )
	{
		newWindow = newControl = new EditControl( parent );
		newCaption = "Edit";
		type = EditControl::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == MEMO_PUSH )
	{
		newWindow = newControl = new MemoControl( parent );
		newCaption = "Memo";
		type = MemoControl::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == FRAME_PUSH )
	{
		newWindow = newChild = new FrameChild( parent );
		newCaption = "";
		type = FrameChild::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == SCROLLER_PUSH )
	{
		newWindow = newChild = new ScrollFrame( parent );
		newCaption = "";
		type = ScrollFrame::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == XMLVIEW_PUSH )
	{
		newWindow = newChild = new XMLeditorChild( parent );
		newCaption = "";
		type = XMLeditorChild::className;
	}
	else if(m_theGuiBuilder->getCurrentId() == GRIDVIEW_PUSH )
	{
		newWindow = newChild = new GridViewer( parent );
		newCaption = "";
		type = GridViewer::className;
	}

	newName = type.uperCaseCopy();

	newWindow->sizeNmove( position->x, position->y, 80, 25 );
	newWindow->setText( newCaption );

	newResource->setStringAttribute( TYPE_ATTR, type );
	newResource->setStringAttribute( NAME_ATTR, newName );
	newResource->setStringAttribute( CAPTION_ATTR, newCaption );
	newResource->setIntegerAttribute( LayoutData::xPosAttr, position->x );
	newResource->setIntegerAttribute( LayoutData::yPosAttr, position->y );
	newResource->setIntegerAttribute( LayoutData::widthAttr, 80 );
	newResource->setIntegerAttribute( LayoutData::heightAttr, 25 );
	newResource->setIntegerAttribute( STYLE_ATTR, newWindow->getStyle() );
	newWindow->setResource( newResource );
	if( newChild )
	{
		newChild->create( parent );
	}
	else if( newControl )
	{
		newControl->create( parent );
	}
	catchControl( newWindow );
	m_theGuiBuilder->addChildItem( newResource );
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
ProcessStatus DesignerForm::handleKeyDown( int key )
{
	if( key == VK_DELETE )
	{
		bool	itsMe = false;
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow		*child = *it;
			if( child == this )
			{
				itsMe = true;
			}
			xml::Element	*resource = child->getResource();

			resource->remove();
			delete resource;
			child->close();
		}
		m_selected.clear();
		
		m_theGuiBuilder->removeSelected( itsMe );

		return psPROCESSED;
	}
	return OverlappedWindow::handleKeyDown( key );
}

ProcessStatus DesignerForm::handleResize( const Size &clientSize )
{
	Size			windowSize = getSize();
	xml::Element	*resource = getResource();

	resource->setIntegerAttribute( LayoutData::widthAttr, windowSize.width );
	resource->setIntegerAttribute( LayoutData::heightAttr, windowSize.height );

	return OverlappedWindow::handleResize( clientSize );
}

ProcessStatus DesignerForm::handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position )
{
	doEnterFunction("DesignerForm::handleLeftButton");
	focus();
	if( leftButton == lbDOWN && m_theGuiBuilder->getCurrentId() == SELECT_PUSH && !(modifier& MK_SHIFT) )
	{
		m_startDraging = false;
		startSelect( position );
	}
	else if( leftButton == lbUP )
	{
		if( m_tableMoved )
		{
			endTableDrag();
		}
		else if( m_startDraging )
		{
			endDragging();
		}
		else if( m_theGuiBuilder->getCurrentId() == SELECT_PUSH )
		{
			endSelect( modifier, position );
		}
		else
		{
			createControl( &position );
		}
		resetClickStatus();
	}

	return psPROCESSED;
}

ProcessStatus DesignerForm::handleMouseMove( WPARAM , const Point &position )
{
	doEnterFunction("DesignerForm::handleMouseMove");
	Size	clientSize = getClientSize();
	if( position.x < 0 || position.y < 0 || position.x > clientSize.width || position.y > clientSize.height )
	{
		if( !m_startDraging )
		{
			releaseMouse();
		}
	}
	else
	{
		captureMouse();
		if( m_startDraging )
		{
			if( m_lastChildClick )
			{
				if( m_tableManager )
				{
					performTableDrag( position );
				}
			}
			else if( m_sizeMode == smNONE )
			{
				performStandardDrag( position );
			}
			else
			{
				performResize( position );
			}
			invalidateWindow();
		}
		else
		{
			checkSizeStart( position );
		}
	}

	return psPROCESSED;
}

void DesignerForm::postControlCallback( BasicWindow *control, unsigned uMsg, WPARAM , LPARAM lParam )
{
	if( uMsg == WM_MOUSEMOVE )
	{
		if( control != this )
		{
			Point point = MAKEPOINT( DWORD(lParam) );

			if( point.x >= 0 && point.y >= 0 )
			{
				Size size = control->getClientSize();

				if( point.x < size.width && point.y < size.height )
					captureMouse();
			}
		}
	}
	else if( uMsg == WM_PAINT )
	{
		DrawDevice	hDC( control );

		Size	childRect = control->getClientSize();

		if( m_selected.hasElement( control ) )
		{
			hDC.getPen().selectPen( Pen::spBlack );
			hDC.getBrush().selectBrush( Brush::sbNull );
			hDC.rectangle( 0, 0, childRect.width, childRect.height );
		
			hDC.getBrush().selectBrush( Brush::sbBlack );
			hDC.rectangle( 0, 0, s_handleSize, s_handleSize );
			hDC.rectangle( childRect.width-s_handleSize, 0, childRect.width, s_handleSize );
			hDC.rectangle( childRect.width-s_handleSize, childRect.height-s_handleSize, childRect.width, childRect.height );
			hDC.rectangle( 0, childRect.height-s_handleSize, s_handleSize, childRect.height );
		}

		CallbackWindow *callbackWindow = dynamic_cast<CallbackWindow *>(control);
		if( callbackWindow )
		{
			const LayoutManager *layoutManager = callbackWindow->getLayoutManager();
			if( !layoutManager )
			{
				if( s_gridSize )
				{
					Size	size = control->getSize();
					for( int x=s_gridSize; x<size.width; x += s_gridSize )
					{
						for( int y=s_gridSize; y<size.height; y += s_gridSize )
						{
							hDC.setPixel( x, y, 0, 0, 255 );
						}
					}
				}
			}
			else
			{
				const TableManager *tableManager = dynamic_cast<const TableManager *>( layoutManager );
				if( tableManager )
				{
					hDC.getPen().setColor( 0, 0, 255, false )
						.setWidth( 1, false )
						.setStyle( Pen::psDash, true )
					;
					// draw the grid of the table layout manager
					for( size_t i=0; i<tableManager->getNumColumns(); i++ )
					{
						int pos = tableManager->getColLeft( i );
						hDC.line( pos, 0, pos, childRect.height );
					}
					for( size_t i=0; i<tableManager->getNumRows(); i++ )
					{
						int pos = tableManager->getRowTop( i );
						hDC.line( 0, pos, childRect.width, pos );
					}
				}
			}
		}
	}
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void DesignerForm::refreshSelection()
{
	for( 
		ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
		it != endIT;
		++it
	)
	{

		BasicWindow *child = *it;
		m_theGuiBuilder->selectControl( child, FromRefresh );

	}
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

