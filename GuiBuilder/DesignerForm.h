/*
		Project:		GUI Builder
		Module:			DesignerForm.h
		Description:	The form designer
		Author:			Martin Gäckler
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

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
#include <WINLIB/colors.h>

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

	void internalClearSelected();
	void internalSelect( BasicWindow *control );
	void internalUnselect( BasicWindow *control );
	bool isSelected( BasicWindow *control )
	{
		return m_selected.hasElement( control );
	}

	void startSelect( const Point &position );
	void endSelect( WPARAM modifier, const Point &position );

	template <typename IteratorT>
	bool checkOther( const IteratorT &begin, const IteratorT &end, BasicWindow *child );

	template <typename ContainerT>
	bool checkOther( const ContainerT &line, BasicWindow *child );

	bool checkRectangle( const gak::PODmatrix<BasicWindow*>::ArrayType &children, RectBorder *rectangle );
	void findChildren( RectBorder *area, gak::PODmatrix<BasicWindow*> *children );
	void restoreChildren();
	void performTableDrag( const Point &position );
	void endTableDrag();
	void endDragging();
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
	void checkSizeStart( const Point &position );

	void resetClickStatus()
	{
		m_lastChildClick = nullptr;
		m_tableMoved = m_startDraging = false;
		m_relativePositions.clear();
		m_oldPositions.clear();
	}
	public:
	DesignerForm( gak::xml::Element *resource, GuiBuilderWindow	*theGuiBuilder ) : OverlappedWindow( nullptr ) 
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

	void moveHorizontal( int xPos );
	void moveVertical( int yPos );
	void setWidth( int width );
	void setHeight( int height );
	void setCaption( const STRING &caption );
	
	void setLayoutHorizontal( int xPos );
	void setLayoutVertical( int yPos );
	void setLayoutSizeHorizontal( unsigned width );
	void setLayoutSizeVertical( unsigned height );

	void setLayoutGrowHorizontal( unsigned growWidth );
	void setLayoutGrowVertical( unsigned growHeight );
	void setPaddingLeft( int paddingLeft );
	void setPaddingRight( int paddingRight );
	void setPaddingTop( int paddingTop );
	void setPaddingBottom( int paddingBottom );
	void setAttachment( int attachment );
	void setLayoutManager( const STRING &type );
	void setMargin( int marginLeft, int marginRight, int marginTop, int marginBottom );

	void setFont( const STRING &font );
	void setBaseClass( const STRING &baseClass );
	void setNumCols( unsigned numCols );
	void setNumRows( unsigned numRows );

	bool moveUp();
	bool moveDown();

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
	COLORREF getBackgroundColor() const
	{
		if( m_selected.size() )
		{
			return m_selected[0]->getBackgroundBrush().getBackgroundColor();
		}
		return colors::WHITE;
	}

	private:
	class LayoutRemover
	{
		public:
		static void perform( BasicWindow *child )
		{
			child->setLayoutData( nullptr );
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
		return m_selected.size() ? m_selected[0] : nullptr;
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