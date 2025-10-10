/*
		Project:		Windows Class Library
		Module:			ressource.cpp
		Description:	Create windows from a ressource file
		Author:			Martin Gäckler
		Address:		HoFmannsthalweg 14, A-4030 Linz
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

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/exception.h>
#include <gak/numericString.h>
#include <gak/directory.h>

#include <WINLIB/POPUP.H>
#include <WINLIB/CONTROLW.h>
#include <WINLIB/frame.h>
#include <WINLIB/resource.h>
#include <WINLIB/DEVICE.H>
#include <WINLIB/gridView.h>
#include <WINLIB/xmlEditorChild.h>
#include <WINLIB/scrollFrame.h>

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

extern const char FORMS_TAG[] = "forms";
extern const char FORM_TAG[] = "form";
extern const char FRAMES_TAG[] = "frames";
extern const char FRAME_TAG[] = "frame";
extern const char SCROLLER_TAG[] = "scroller";
extern const char CHILD_TAG[] = "child";

extern const char MENUS_TAG[] = "menus";
extern const char MENU_TAG[] = "menu";

extern const char STRINGS_TAG[] = "strings";
extern const char LIST_TAG[] = "list";
extern const char STRING_TAG[] = "string";

extern const char ID_ATTR[] = "id";
extern const char NAME_ATTR[] = "name";
extern const char CAPTION_ATTR[] = "caption";
extern const char STYLE_ATTR[] = "style";
extern const char TYPE_ATTR[] = "type";

extern const char ITEMS_TAG[] = "items";
extern const char ITEM_TAG[] = "item";

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static char colorValues[][32] =
{
	"Default",

	"WHITE_BRUSH",
	"LTGRAY_BRUSH",
	"GRAY_BRUSH",
	"DKGRAY_BRUSH",
	"BLACK_BRUSH",
	"NULL_BRUSH",

	"COLOR_SCROLLBAR",
	"COLOR_BACKGROUND",
	"COLOR_ACTIVECAPTION",
	"COLOR_INACTIVECAPTION",
	"COLOR_MENU",
	"COLOR_WINDOW ",
	"COLOR_WINDOWFRAME",
	"COLOR_MENUTEXT",
	"COLOR_WINDOWTEXT",
	"COLOR_CAPTIONTEXT",
	"COLOR_ACTIVEBORDER",
	"COLOR_INACTIVEBORDER",
	"COLOR_APPWORKSPACE",
	"COLOR_HIGHLIGHT",
	"COLOR_HIGHLIGHTTEXT",
	"COLOR_BTNFACE",
	"COLOR_BTNSHADOW",
	"COLOR_GRAYTEXT",
	"COLOR_BTNTEXT",
	"COLOR_INACTIVECAPTIONTEXT",
	"COLOR_BTNHIGHLIGHT",

	"Select",
};

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

template <class CONTROL> 
static void addListItems( CONTROL *listBox, xml::Element *child )
{
	xml::Element	*items = child->getElement( ITEMS_TAG );
	if( items )
	{
		for( size_t i=0; i<items->getNumObjects(); i++ )
		{
			xml::Element	*item = items->getElement( i );
			if( item && item->getTag() == ITEM_TAG )
			{
				STRING	value = item->getValue( xml::PLAIN_MODE );
				listBox->addEntry( value );
			}
		}
	}
}

static void setGridViewer( GridViewer * gridViewer, xml::Element *child )
{
	unsigned numCols = child->getAttribute( "numCols" ).getValueN<unsigned>();
	gridViewer->setNumCols( numCols );
	unsigned numRows = child->getAttribute( "numRows" ).getValueN<unsigned>();
	gridViewer->setNumRows( numRows );
}

static void createChild( const F_STRING &resourceFileName, xml::Element *child, CallbackWindow *parent, bool designerMode )
{
	BasicWindow		*newBasicChild = NULL;
	ChildWindow		*newChild = NULL;
	Label			*staticText = NULL;
	Button			*button = NULL;
	PushButton		*pushButton = NULL;
	CheckBox		*checkBox = NULL;
	GroupBox		*groupBox = NULL;
	RadioButton		*radioButton = NULL;
	EditControl		*editControl = NULL;
	MemoControl		*memoControl = NULL;
	FrameChild		*frameChild = NULL;
	ScrollFrame		*scrollFrame = NULL;
	ComboBox		*comboBox = NULL;
	ListBox			*listBox = NULL;
	TrackBar		*trackBar = NULL;
	DateTimePicker	*dateTimePicker = NULL;
	ScrollBar		*scrollBar = NULL;
	UpDownButton	*upDownButton = NULL;
	TreeView		*treeView = NULL;
	GridViewer		*gridViewer = NULL;

	unsigned		x = child->getAttribute( LayoutData::xPosAttr ).getValueN<unsigned>();
	unsigned		y = child->getAttribute( LayoutData::yPosAttr ).getValueN<unsigned>();
	unsigned		width = child->getAttribute( LayoutData::widthAttr ).getValueN<unsigned>();
	unsigned		height = child->getAttribute( LayoutData::heightAttr ).getValueN<unsigned>();

	STRING			caption = child->getAttribute( CAPTION_ATTR );
	STRING			type = child->getAttribute( TYPE_ATTR );
	STRING			style = child->getAttribute( STYLE_ATTR );
	unsigned long	iStyle = style.getValueN<unsigned long>()|WS_CHILD;

	if( type == Label::className )
		newBasicChild = staticText = new Label( parent );
	else if( type == PushButton::className )
		newBasicChild = button = pushButton = new PushButton( parent );
	else if( type == CheckBox::className )
		newBasicChild = button = checkBox = new CheckBox( parent );
	else if( type == GroupBox::className )
		newBasicChild = groupBox = new GroupBox( parent );
	else if( type == RadioButton::className )
		newBasicChild = button = radioButton = new RadioButton( parent );
	else if( type == EditControl::className )
		newBasicChild = editControl = new EditControl( parent );
	else if( type == MemoControl::className )
		newBasicChild = memoControl = new MemoControl( parent );
	else if( type == FrameChild::className )
		newBasicChild = frameChild = new FrameChild( parent );
	else if( type == ScrollFrame::className )
		newBasicChild = scrollFrame = new ScrollFrame( parent );
	else if( type == ComboBox::className )
		newBasicChild = comboBox = new ComboBox( parent );
	else if( type == TreeView::className )
		newBasicChild = treeView = new TreeView( parent );
	else if( type == TrackBar::className )
		newBasicChild = trackBar = new TrackBar( parent );
	else if( type == DateTimePicker::className )
		newBasicChild = dateTimePicker = new DateTimePicker( parent );
	else if( type == ScrollBar::className )
		newBasicChild = scrollBar = new ScrollBar( parent );
	else if( type == UpDownButton::className )
		newBasicChild = upDownButton = new UpDownButton( parent );
	else if( type == ListBox::className )
		newBasicChild = listBox = new ListBox( parent );
	else if( type == XMLeditorChild::className )
		newBasicChild = newChild = new XMLeditorChild( parent );
	else if( type == GridViewer::className )
		newBasicChild = newChild = gridViewer = new GridViewer( parent );
	else
		throw gak::LibraryException( gak::STRING( "Unkown item type: " ) + type );

	if( newBasicChild )
	{
		if( designerMode && caption.isEmpty() )
		{
			caption = child->getAttribute( NAME_ATTR );
			if( caption.isEmpty() )
			{
				caption = type;
			}
		}
		newBasicChild->setResource( child );
		newBasicChild->move( x, y );
		newBasicChild->setText( caption );

		if( !style.isEmpty() )
		{
			if( designerMode )
			{
				iStyle |= WS_VISIBLE;
				if( frameChild || scrollFrame )
				{
					iStyle |= WS_BORDER;
				}
			}

			newBasicChild->setStyle( iStyle );
		}

		if( staticText )
			staticText->create( parent );
		else if( pushButton )
			pushButton->create( parent );
		else if( checkBox )
			checkBox->create( parent );
		else if( groupBox )
			groupBox->create( parent );
		else if( radioButton )
			radioButton->create( parent );
		else if( editControl )
			editControl->create( parent );
		else if( memoControl )
			memoControl->create( parent );
		else if( comboBox )
			comboBox->create( parent );
		else if( treeView )
			treeView->create( parent );
		else if( trackBar )
			trackBar->create( parent );
		else if( dateTimePicker )
			dateTimePicker->create( parent );
		else if( scrollBar )
			scrollBar->create( parent );
		else if( upDownButton )
			upDownButton->create( parent );
		else if( listBox )
			listBox->create( parent );
		else if( frameChild )
			frameChild->create( parent );
		else if( scrollFrame )
			scrollFrame->create( parent );
		else if( newChild )
			newChild->create( parent );
		else
			throw gak::LibraryException( gak::STRING( "Unkown child type: " ) + type );

		if( button )
		{
			if( iStyle & BS_BITMAP )
			{
				STRING	name = child->getAttribute( NAME_ATTR );
				if( !button->setBitmap( name ) )
				{
					F_STRING	bitmapFile = name + ".bmp";
					bitmapFile = makeFullPath( resourceFileName, bitmapFile );
					if( exists( bitmapFile ) )
					{
						button->setBitmap( bitmapFile );
					}
				}
			}
		}

		newBasicChild->resize( width, height );

		createChildWindows( resourceFileName, child, newBasicChild, designerMode );
	}
	if( listBox )
		addListItems( listBox, child );
	else if( comboBox )
		addListItems( comboBox, child );
	else if( gridViewer )
		setGridViewer( gridViewer, child );
}

static void createLayoutManager( xml::Element *resource, CallbackWindow *parent, bool designerMode )
{
	STRING			type = resource->getAttribute( TYPE_ATTR );
	LayoutManager	*layoutManager = createLayoutManager( type, designerMode );

	if( layoutManager )
	{
		layoutManager->m_margin.left = resource->getAttribute( LayoutManager::marginLeft ).getValueN<int>();
		layoutManager->m_margin.right = resource->getAttribute( LayoutManager::marginRight ).getValueN<int>();
		layoutManager->m_margin.top = resource->getAttribute( LayoutManager::marginTop ).getValueN<int>();
		layoutManager->m_margin.bottom = resource->getAttribute( LayoutManager::marginBottom ).getValueN<int>();

		parent->setLayoutManager( layoutManager );
	}
}

static void createLayoutData( xml::Element *resource, BasicWindow *child )
{
	LayoutData	*layoutData = new LayoutData;

	unsigned x = resource->getAttribute( LayoutData::xPosAttr ).getValueN<unsigned>();
	unsigned y = resource->getAttribute( LayoutData::yPosAttr ).getValueN<unsigned>();
	unsigned width = resource->getAttribute( LayoutData::widthAttr ).getValueN<unsigned>();
	unsigned height = resource->getAttribute( LayoutData::heightAttr ).getValueN<unsigned>();
	unsigned growWidth = resource->getAttribute( LayoutData::growWidthAttr ).getValueN<unsigned>();
	unsigned growHeight = resource->getAttribute( LayoutData::growHeightAttr ).getValueN<unsigned>();
	unsigned paddingLeft = resource->getAttribute( LayoutData::paddingLeft ).getValueN<unsigned>();
	unsigned paddingRight = resource->getAttribute( LayoutData::paddingRight ).getValueN<unsigned>();
	unsigned paddingTop = resource->getAttribute( LayoutData::paddingTop ).getValueN<unsigned>();
	unsigned paddingBottom = resource->getAttribute( LayoutData::paddingBottom ).getValueN<unsigned>();
	LayoutData::Attachment attach = LayoutData::Attachment(
		resource->getAttribute( LayoutData::attachment ).getValueN<signed>()
	);

	layoutData->setX( x )->setY( y )->setPaddingLeft( paddingLeft )->setPaddingRight( paddingRight )->setPaddingTop( paddingTop )->setPaddingBottom( paddingBottom )->setAttachment( attach );

	if( width )
		layoutData->setWidth( width );
	if( height )
		layoutData->setHeight( height );
	if( growWidth )
		layoutData->setGrowWidth( growWidth );
	if( growHeight )
		layoutData->setGrowHeight( growHeight );

	child->setLayoutData( layoutData );
}

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

void createChildWindows( const F_STRING &resourceFileName, xml::Element *resource, BasicWindow *parent, bool designerMode )
{
	CallbackWindow	*callParent = (parent->getWindowClass() & CallbackWindowFlag)
		? static_cast<CallbackWindow*>( parent )
		: NULL;

	for( size_t i=0; i<resource->getNumObjects(); i++ )
	{
		xml::Element *child = resource->getElement( i );
		if( callParent && child->getTag() == CHILD_TAG )
			createChild( resourceFileName, child, callParent, designerMode );
		else if( callParent && child->getTag() == LayoutManager::className )
			createLayoutManager( child, callParent, designerMode );
		else if( child->getTag() == LayoutData::className )
			createLayoutData( child, parent );
	}
}

SuccessCode createFrame( const F_STRING &resourceFileName, xml::Element *resource, FrameChild *frame, CallbackWindow *parent, bool designerMode )
{
	unsigned	x = resource->getAttribute( LayoutData::xPosAttr ).getValueN<unsigned>();
	unsigned	y = resource->getAttribute( LayoutData::yPosAttr ).getValueN<unsigned>();
	unsigned	width = resource->getAttribute( LayoutData::widthAttr ).getValueN<unsigned>();
	unsigned	height = resource->getAttribute( LayoutData::heightAttr ).getValueN<unsigned>();

	unsigned long style = resource->getAttribute( STYLE_ATTR ).getValueN<unsigned>();

	frame->setResource( resource );
	frame->setStyle( style|WS_CHILD );
	frame->sizeNmove( x, y, width, height );
	if( frame->create( parent ) == scSUCCESS )
	{
		frame->disallowNotifications();
		createChildWindows( resourceFileName, resource, frame, designerMode );
		frame->getControls();
		frame->allowDialogProcessing();
		frame->allowNotifications();

		return scSUCCESS;
	}
	return scERROR;
}

SuccessCode createFrame( const F_STRING &resourceFileName, xml::Element *root, const char *frameName, FrameChild *frameChild, CallbackWindow *parent, bool designerMode )
{
	SuccessCode	errorFlag = scERROR;
	if( root )
	{
		xml::Element	*frames = root->getElement( FRAMES_TAG );
		if( frames )
		{
			for( size_t i=0; i<frames->getNumObjects(); i++ )
			{
				xml::Element	*resource = frames->getElement( i );
				if( resource->getTag() == FRAME_TAG && resource->getAttribute( NAME_ATTR ) == frameName )
				{
					errorFlag = createFrame( resourceFileName, resource, frameChild, parent, designerMode );
					break;
				}
			}
		}
	}

	return errorFlag;
}

LayoutManager *createLayoutManager( const STRING &type, bool designerMode )
{
	LayoutManager *layoutManager;

	if( type == "SingleChildManager" )
		layoutManager = new SingleChildManager;
	else if( type == "TableManager" )
		layoutManager = new TableManager;
	else if( type == "RowManager" )
		layoutManager = new RowManager;
	else if( type == "ColManager" )
		layoutManager = new ColManager;
	else if( type == "AttachmentManager" )
		layoutManager = new AttachmentManager;
	else
		layoutManager = NULL;

	if( designerMode && layoutManager )
		layoutManager->setDesigerMode();

	return layoutManager;
}

STRING getColorName( size_t index )
{
	assert( index < arraySize( colorValues ) );
	return colorValues[index];
};

BasicWindow::BackgroundColor getColorValue( const STRING &name )
{
	for( size_t i=0; i<arraySize( colorValues ) ; i++ )
	{
		if( name == colorValues[i] )
			return BasicWindow::BackgroundColor( i );
	}

	return BasicWindow::bcDEFAULT;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

