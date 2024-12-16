/*
		Project:		GUI Builder
		Module:			DesignerForm.h
		Description:	The form designer
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

#ifndef DESIGNER_FORM_H
#define DESIGNER_FORM_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/numericString.h>

#include <WINLIB/POPUP.H>
#include <WINLIB/resource.h>
#include <WINLIB/frame.h>
#include <WINLIB/gridView.h>

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

using namespace winlib;

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

class GuiBuilderWindow; 

class DesignerForm : public OverlappedWindow
{
	static const int s_handleSize = 5;
	static const int s_gridSize = 8;

	gak::PairMap<void*,Point>	m_relativePositions;
	gak::PairMap<void*,Point>	m_oldPositions;

	BasicWindow					*m_lastChildClick;
	bool						m_tableMoved;
	const CallbackWindow		*m_currentFrame;

	TableManager				*m_tableManager;
	ChildWindows				m_selected;
	GuiBuilderWindow			*m_theGuiBuilder;
	enum SizeMode
	{
		smNONE, smUPPERLEFT, smUPPERRIGHT, smLOWERLEFT, smLOWERRIGHT
	}							m_sizeMode;
	bool						m_startDraging;
	Point						m_lastMousePos;

	void internalClearSelected()
	{
		for( size_t i=m_selected.size()-1; i!=-1; i-- )
		{
			internalUnselect( m_selected[i] );
		}

		m_selected.clear();
	}
	void internalSelect( BasicWindow *control );
	void internalUnselect( BasicWindow *control );
	bool isSelected( BasicWindow *control )
	{
		return m_selected.hasElement( control );
	}

	void startSelect( const Point &position );
	void endSelect( WPARAM modifier, const Point &position );
	bool checkUnique( const gak::PODmatrix<BasicWindow*>::ArrayType &line, BasicWindow *child );
	bool checkRectangle( const gak::PODmatrix<BasicWindow*>::ArrayType &children, RectBorder *rectangle );
	void findChildren( RectBorder *area, gak::PODmatrix<BasicWindow*> *children );
	void restoreChildren( void );
	void performTableDrag( const Point &position );
	void endTableDrag( void );
	void endDragging( void );
	void performStandardDrag( const Point &position );
	void performResize( const Point &position );
	void createControl( const Point *position );

	public:
	void selectControl( BasicWindow *control, bool fromTree )
	{
		if( fromTree )
		{
			m_selected.clear();
		}
		m_selected.addElement( control );
	}
	void unselectControl( BasicWindow *control )
	{
		m_selected.removeElement( control );
	}
	void clearSelected()
	{
		m_selected.clear();
	}

	private:
	void checkSizeStart( const Point &position )
	{
		doEnterFunction("checkSizeStart");

		BasicWindow	*child = findChild( position );
		if( child && isSelected( child ) )
		{
			RectBorder	childRect = child->getRelativeRectangle();
			if( position.x >= childRect.left 
			&& position.y >= childRect.top
			&& position.x < childRect.left + s_handleSize
			&& position.y < childRect.top + s_handleSize )
			{
				// it's the upper left corner
				SetCursor( LoadCursor( NULL, IDC_SIZENWSE ) );
				m_sizeMode = smUPPERLEFT;
			}
			else if( position.x <= childRect.right 
			&& position.y >= childRect.top
			&& position.x > childRect.right - s_handleSize
			&& position.y < childRect.top + s_handleSize )
			{
				// it's the upper right corner
				SetCursor( LoadCursor( NULL, IDC_SIZENESW ) );
				m_sizeMode = smUPPERRIGHT;
			}
			else if( position.x >= childRect.left 
			&& position.y <= childRect.bottom
			&& position.x < childRect.left + s_handleSize
			&& position.y > childRect.bottom - s_handleSize )
			{
				// it's the lower left corner
				SetCursor( LoadCursor( NULL, IDC_SIZENESW ) );
				m_sizeMode = smLOWERLEFT;
			}
			else if( position.x <= childRect.right 
			&& position.y <= childRect.bottom
			&& position.x > childRect.right - s_handleSize
			&& position.y > childRect.bottom - s_handleSize )
			{
				// it's the lower right corner
				SetCursor( LoadCursor( NULL, IDC_SIZENWSE ) );
				m_sizeMode = smLOWERRIGHT;
			}
			else
			{
				SetCursor( LoadCursor( NULL, IDC_HAND ) );
				if( m_sizeMode != smNONE )
				{
					m_sizeMode = smNONE;
				}
			}
		}
		else
		{
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			if( m_sizeMode != smNONE )
			{
				m_sizeMode = smNONE;
			}
		}
	}

	void resetClickStatus()
	{
		m_lastChildClick = NULL;
		m_tableMoved = m_startDraging = false;
		m_relativePositions.clear();
		m_oldPositions.clear();
	}
	public:
	DesignerForm( gak::xml::Element *resource, GuiBuilderWindow	*theGuiBuilder ) : OverlappedWindow( NULL ) 
	{
		m_theGuiBuilder = theGuiBuilder;
		this->setResource( resource );

		resetClickStatus();
	}
	virtual ProcessStatus handleKeyDown( int key );
	virtual ProcessStatus handleResize( const Size &newSize );
	virtual ProcessStatus handleLeftButton( LeftButton leftButton, WPARAM modifier, const Point &position );
	virtual ProcessStatus handleMouseMove( WPARAM modifier, const Point &position );
	virtual void postControlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam );

	void moveHorizontal( int xPos )
	{
		int	dummy, yPos;
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow *child = *it;
			child->getRelativePosition( &dummy, &yPos );
			child->move( xPos, yPos );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				resource->setIntegerAttribute( LayoutData::xPosAttr, xPos );
			}
		}
	}
	void moveVertical( int yPos )
	{
		int	dummy, xPos;
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow *child = *it;
			child->getRelativePosition( &xPos, &dummy );
			child->move( xPos, yPos );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				resource->setIntegerAttribute( LayoutData::yPosAttr, yPos );
			}
		}
	}
	void setWidth( int width )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			Size		size = child->getSize();
			size.width = width;
			child->resize( size );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				resource->setIntegerAttribute( LayoutData::widthAttr, width );
			}
		}
	}
	void setHeight( int height )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			Size		size = child->getSize();
			size.height = height;
			child->resize( size );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				resource->setIntegerAttribute( LayoutData::heightAttr, height );
			}
		}
	}
	void setCaption( const STRING &caption )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			child->setTitle( caption );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
				resource->setStringAttribute( CAPTION_ATTR, caption );
		}
	}
	
	void setLayoutHorizontal( int xPos )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->xPos = xPos;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::xPosAttr, xPos );
			}
		}
	}
	void setLayoutVertical( int yPos )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->yPos = yPos;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::yPosAttr, yPos );
			}
		}
	}
	void setLayoutSizeHorizontal( unsigned width )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->width = width;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::widthAttr, width );
			}
		}
	}
	void setLayoutSizeVertical( unsigned height )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->height = height;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::heightAttr, height );
			}
		}
	}

	void setLayoutGrowHorizontal( unsigned growWidth )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->growWidth = growWidth;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::growWidthAttr, growWidth );
			}
		}
	}
	void setLayoutGrowVertical( unsigned growHeight )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->growHeight = growHeight;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::growHeightAttr, growHeight );
			}
		}
	}
	void setPaddingLeft( int paddingLeft )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->padding.left = paddingLeft;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::paddingLeft, paddingLeft );
			}
		}
	}
	void setPaddingRight( int paddingRight )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->padding.right = paddingRight;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::paddingRight, paddingRight );
			}
		}
	}
	void setPaddingTop( int paddingTop )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->padding.top = paddingTop;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::paddingTop, paddingTop );
			}
		}
	}
	void setPaddingBottom( int paddingBottom )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->padding.bottom = paddingBottom;

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );

				layout->setIntegerAttribute( LayoutData::paddingBottom, paddingBottom );
			}
		}
	}
	void setAttachment( int attachment )
	{
		if( attachment < 0 )
		{
			return;		// do not change
		}
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow	*child = *it;
			LayoutData	*layoutData = child->getLayoutData();
			if( !layoutData )
				child->setLayoutData( layoutData = new LayoutData );
			layoutData->attach = LayoutData::Attachment( attachment );

			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( !layout )
				{
					layout = resource->addObject( new gak::xml::Any( LayoutData::className ) );
				}

				layout->setIntegerAttribute( LayoutData::attachment, attachment );
			}
		}
	}
	void setLayoutManager( const STRING &type )
	{
		// reset size and position from resource
		const ChildWindows &children = getChildren();

		for( size_t i=0; i<children.size(); i++ )
		{
			BasicWindow	*control = children[i];
			gak::xml::Element	*resource = control->getResource();
			if( resource )
			{
				int x = resource->getAttribute( LayoutData::xPosAttr ).getValueN<int>();
				int y = resource->getAttribute( LayoutData::yPosAttr ).getValueN<int>();
				int width = resource->getAttribute( LayoutData::widthAttr ).getValueN<int>();
				int height = resource->getAttribute( LayoutData::heightAttr ).getValueN<int>();
				control->sizeNmove( x, y, width, height );
			}
		}
	
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			CallbackWindow *child = dynamic_cast<CallbackWindow*>( *it );
			if( child )
			{
				LayoutManager *layoutManager = createLayoutManager( type, true );

				child->setLayoutManager( layoutManager );

				gak::xml::Element	*resource = child->getResource();
				if( resource )
				{
					gak::xml::Element	*layoutManagerResource = resource->getElement( LayoutManager::className );
					if( layoutManagerResource && !layoutManager )
					{
						resource->removeObject( layoutManagerResource );
						delete layoutManagerResource;
					}
					else if( !layoutManagerResource && layoutManager )
					{
						gak::xml::Element	*layoutManagerResource = resource->addObject( new gak::xml::Any( LayoutManager::className ) );
						layoutManagerResource->setStringAttribute( TYPE_ATTR, type );
					}
					else if( layoutManagerResource && layoutManager )
					{
						layoutManagerResource->setStringAttribute( TYPE_ATTR, type );
					}
				}
			}
		}
	}
	void setMargin( int marginLeft, int marginRight, int marginTop, int marginBottom )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			CallbackWindow	*child = dynamic_cast<CallbackWindow*>(*it);
			if( child )
			{
				LayoutManager	*layoutManager = child->getLayoutManager();
				if( layoutManager )
				{
					layoutManager->margin.left = marginLeft;
					layoutManager->margin.right = marginRight;
					layoutManager->margin.top = marginTop;
					layoutManager->margin.bottom = marginBottom;
				}
				gak::xml::Element		*resource = child->getResource();
				if( resource )
				{
					gak::xml::Element *layoutManagerResource = resource->getElement( LayoutManager::className );
					if( !layoutManagerResource && layoutManager )
						layoutManagerResource = resource->addObject( new gak::xml::Any( LayoutManager::className ) );
					else if( layoutManagerResource && !layoutManager )
					{
						resource->removeObject( layoutManagerResource );
						delete layoutManagerResource;
						layoutManagerResource = NULL;
					}
					if( layoutManagerResource )
					{
						layoutManagerResource->setIntegerAttribute( LayoutManager::marginLeft, marginLeft );
						layoutManagerResource->setIntegerAttribute( LayoutManager::marginRight, marginRight );
						layoutManagerResource->setIntegerAttribute( LayoutManager::marginTop, marginTop );
						layoutManagerResource->setIntegerAttribute( LayoutManager::marginBottom, marginBottom );
					}
				}
			}
		}
	}

	void setFont( const STRING &font )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow			*child = *it;
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				resource->setStringAttribute( "font", font );
				child->getFont().fromString( font, true );
			}
		}
	}
	void setBaseClass( const STRING &baseClass )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			BasicWindow			*child = *it;
			gak::xml::Element	*resource = child->getResource();
			if( resource && resource->getTag() == FORM_TAG )
			{
				resource->setStringAttribute( "baseClass", baseClass );
			}
		}
	}
	void setNumCols( unsigned numCols )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			GridViewer	*child = dynamic_cast<GridViewer*>(*it);
			if( child )
			{
				child->setNumCols( numCols );
				gak::xml::Element	*resource = child->getResource();
				if( resource )
				{
					resource->setIntegerAttribute( "numCols", numCols );
				}
			}
		}
	}
	void setNumRows( unsigned numRows )
	{
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{
			GridViewer	*child = dynamic_cast<GridViewer*>(*it);
			if( child )
			{
				child->setNumRows( numRows );
				gak::xml::Element	*resource = child->getResource();
				if( resource )
				{
					resource->setIntegerAttribute( "numRows", numRows );
				}
			}
		}
	}

	bool moveUp()
	{
		bool hasChanged = false;
		for( 
			ChildWindows::const_iterator it = m_selected.cbegin(), endIT = m_selected.cend();
			it != endIT;
			++it
		)
		{

			BasicWindow *child = *it;
			gak::xml::Element *element = child->getResource();
			if( element )
			{
				long index = element->getIndex();
				if( index )
				{
					element->moveTo(index-1);
					hasChanged = true;
				}
				else
				{
/*v*/				break;
				}
			}
			else
			{
/*v*/			break;
			}
		}
		
		return hasChanged;
	}

	bool moveDown()
	{
		bool hasChanged = false;
		for( 
			ChildWindows::const_reverse_iterator it = m_selected.crbegin(), endIT = m_selected.crend();
			it != endIT;
			++it
		)
		{

			BasicWindow *child = *it;
			gak::xml::Element *element = child->getResource();
			if( element )
			{
				long index = element->getIndex();
				long numSiblings = element->getParent()->getNumObjects()-1;
				if( index < numSiblings )
				{
					element->moveTo(index+1);
					hasChanged = true;
				}
				else
				{
/*v*/				break;
				}
			}
			else
			{
/*v*/			break;
			}
		}
		
		return hasChanged;
	}

	void refreshSelection();

	private:
	class BackgroundChanger
	{
		const STRING	&colorName;
		COLORREF		color;

		BasicWindow::BackgroundColor colorType;

		public:
		BackgroundChanger( const STRING &colorName, COLORREF color ) 
		: colorName(colorName), color(color)
		{
			colorType = getColorValue( colorName );
		}

		void operator () ( BasicWindow *child ) const
		{
			child->setBackgroundColor( colorType, color );
			gak::xml::Element		*resource = child->getResource();
			resource->setStringAttribute( "bgColor", colorName );
			resource->setIntegerAttribute( "bg_red", GetRValue( color ) );
			resource->setIntegerAttribute( "bg_green", GetGValue( color ) );
			resource->setIntegerAttribute( "bg_blue", GetBValue( color ) );
		}
	};
	public:
	void setBackgroundColor( const STRING &colorName, COLORREF color )
	{
		BackgroundChanger	theChanger( colorName, color );
		m_selected.forEach( theChanger );
	}
	using OverlappedWindow::setBackgroundColor;

	private:
	class LayoutRemover
	{
		public:
		static void perform( BasicWindow *child )
		{
			child->setLayoutData( NULL );
			gak::xml::Element	*resource = child->getResource();
			if( resource )
			{
				gak::xml::Element *layout = resource->getElement( LayoutData::className );
				if( layout )
				{
					resource->removeObject( layout );
					delete layout;
				}
			}
		}
	};
	public:
	void removeLayoutData()
	{
		m_selected.forEach<LayoutRemover>();
	}

	private:
	class ControlCatcher
	{
		DesignerForm	*theDesignerWindow;
		public:
		ControlCatcher( DesignerForm	*theDesignerWindow ) 
		: theDesignerWindow(theDesignerWindow) {}

		void operator () ( BasicWindow *child ) const
		{
			theDesignerWindow->catchControl( child );
			FrameChild	*frame = dynamic_cast<FrameChild*>(child);
			if( frame )
			{
				theDesignerWindow->catchControls( frame->getChildren() );
			}
		}

	};
	public:
	void catchControls( const ChildWindows &children )
	{
		ControlCatcher catcher( this );
		children.forEach( catcher );
	}
	void catchControls()
	{
		catchControls( getChildren() );
		catchControl( this );
	}

	BasicWindow *getSelected() const
	{
		return m_selected.size() ? m_selected[0] : NULL;
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

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif //  DESIGNER_FORM_H