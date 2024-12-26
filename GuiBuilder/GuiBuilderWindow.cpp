/*
		Project:		GUI Builder
		Module:			GuiBuilderWindow.cpp
		Description:	The main window
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

#include <memory>

#include <gak/numericString.h>
#include <gak/directory.h>

#include "GuiBuilderWindow.h"
#include "StyleDialog.h"
#include "TranslateForm.h"

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

struct IdValues
{
	char		name[32];
	int			value;
};

class MyAboutForm : public ABOUT_form
{
	public:
	MyAboutForm() : ABOUT_form( NULL ) {}

	virtual winlib::ProcessStatus handleCreate()
	{
		winlib::ProcessStatus result = ABOUT_form::handleCreate();
		SysType->setText(gak::formatNumber(sizeof(void*)*8)+" bit");
		return result;
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static IdValues defaultIdentifiers[] =
{
	{ "IDOK", IDOK },
	{ "IDCANCEL", IDCANCEL },
	{ "IDABORT", IDABORT },
	{ "IDRETRY", IDRETRY },
	{ "IDIGNORE", IDIGNORE },
	{ "IDYES", IDYES },
	{ "IDNO", IDNO },
	{ "IDCLOSE", IDCLOSE },
	{ "IDHELP", IDHELP },
	{ "IDTRYAGAIN", IDTRYAGAIN },
	{ "IDCONTINUE", IDCONTINUE }
};

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

const STRING GuiBuilderWindow::WINDOW_TITLE = "MG GUI Builder";

const char GuiBuilderWindow::TRANSLATIONS[] = "translations";

const char GuiBuilderWindow::LANGUAGE_TAG[] = "language";
const char GuiBuilderWindow::LANG_ATTRIBUTE[] = "lang";
const char GuiBuilderWindow::TRANSLATION_ATTRIBUTE[] = "translation";

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

void GuiBuilderWindow::collectCaptions( const xml::Element *root, Captions *target ) const
{
	STRING		caption = root->getAttribute( CAPTION_ATTR );
	if( !caption.isEmpty() )
	{
		target->addElement( caption );
	}
	STRING	tag = root->getTag();
	if( tag == ITEM_TAG )
	{
		target->addElement( root->getValue( xml::PLAIN_MODE ) );
	}
	for( size_t i=0, max=root->getNumObjects(); i<max;	++i )
	{
		collectCaptions( root->getElement( i ), target );
	}
}

void GuiBuilderWindow::translateCaptions( xml::Element *root, const Dictionary &dict ) const
{
	STRING		caption = root->getAttribute( CAPTION_ATTR );
	if( !caption.isEmpty() && dict.hasElement( caption ) )
	{
		STRING translation = dict[caption];
		if( !translation.isEmpty() )
		{
			root->setStringAttribute( CAPTION_ATTR, translation );
		}
	}
	STRING	tag = root->getTag();
	if( tag == ITEM_TAG )
	{
		caption = root->getValue( xml::PLAIN_MODE );
		if( !caption.isEmpty() && dict.hasElement( caption ) )
		{
			STRING translation = dict[caption];
			if( !translation.isEmpty() )
			{
				root->removeObject( size_t(0) );
				root->addObject( new xml::PCData( translation ) );
			}
		}
	}
	for( size_t i=0, max=root->getNumObjects(); i<max;	++i )
	{
		translateCaptions( root->getElement( i ), dict );
	}
}

void GuiBuilderWindow::deleteGuiDoc()
{
	if( m_guiDoc )
	{
		delete m_guiDoc;
		m_guiDoc = NULL;
	}

	m_dictionarys.clear();

	topResourceSelect.clearEntries();
	m_topResources.clear();

	childSelect.clearEntries();
	m_childResources.clear();

	deleteDesignerForm();
}

void GuiBuilderWindow::handleChildSelect( void )
{
	assert( designerForm );

	designerForm->clearSelected();
	ArrayOfInts	selItems;
	childSelect.getSelectedItems( &selItems );

	for( 
		ArrayOfInts::const_iterator it = selItems.cbegin(), endIT = selItems.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow *child = getChildByResource( m_childResources[*it] );
		if( child )
		{
			designerForm->selectControl( child, false );
			selectControl( child, FromChildSelect );
		}
	}

	designerForm->invalidateWindow();
	enableDisaleProperties();
}

void GuiBuilderWindow::handleItemTreeSelect( void )
{
	assert( designerForm );

	designerForm->clearSelected();
	BasicWindow *child = getChildByTreeSelect();
	if( child )
	{
		selectControl( child, FromTreeSelect );
		designerForm->selectControl( child, true );
		loadChildProperties( child );
	}

	designerForm->invalidateWindow();
	enableDisaleProperties();
}

void GuiBuilderWindow::handleStringSelect( void )
{
	assert( stringListEditor );
	int	selected = childSelect.getSelection();
	if( selected >= 0 )
	{
		loadStringProperties( m_childResources[selected] );
	}

	enableDisaleProperties();
}

STRING GuiBuilderWindow::getUniqueType( void )
{
	STRING		uniqueType, currentType;
	ArrayOfInts	selected;

	childSelect.getSelectedItems( &selected );
	for( 
		ArrayOfInts::const_iterator it = selected.cbegin(), endIT = selected.cend();
		it != endIT;
		++it
	)
	{
		xml::Element *resource = m_childResources[*it];
		currentType = resource->getTag();
		if( currentType == CHILD_TAG )
		{
			currentType = resource->getAttribute( TYPE_ATTR );
		}

		if( uniqueType.isEmpty() )
		{
			uniqueType = currentType;
		}
		else if( uniqueType != currentType )
		{
			uniqueType = NULL_STRING;
			break;
		}
	}

	return uniqueType;
}


TreeNode *GuiBuilderWindow::addChildItem( xml::Element *resource, TreeNode *parent )
{
	STRING name = resource->getAttribute( NAME_ATTR );
	if( name.isEmpty() )
	{
		name = resource->getAttribute( TYPE_ATTR );
		STRING caption = resource->getAttribute( CAPTION_ATTR );
		if( !caption.isEmpty() )
		{
			name += STRING( " (" ) + caption + ')';
		}
	}

	childSelect.addEntry( name );
	m_childResources += resource;

	return m_editorMode != emSTRINGS ? treeSelect.appendItem( parent, name, resource ) : NULL;
}

void GuiBuilderWindow::fillMenuItemList( xml::Element *resource, TreeNode *parent )
{
	if( resource->getTag() == MENU_TAG )
	{
		STRING label = resource->getAttribute( CAPTION_ATTR );
		if( label.isEmpty() )
			label = resource->getAttribute( NAME_ATTR );
		if( label.isEmpty() )
			label = MENU_TAG;
		TreeNode *newNode = treeSelect.appendItem( parent, label, resource );
		for( size_t i=0; i<resource->getNumObjects(); i++ )
			fillMenuItemList( resource->getElement( i ), newNode );
	}
}

void GuiBuilderWindow::fillChildItemList( xml::Element *resource, TreeNode *parent )
{
	STRING	tag = resource->getTag();
	if( tag == CHILD_TAG 
	||  tag == FRAME_TAG 
	||  tag == SCROLLER_TAG 
	||  tag == FORM_TAG 
	||  tag == LIST_TAG
	||  tag == STRING_TAG )
	{
		TreeNode *newNode = addChildItem( resource, parent );
		for( size_t i=0; i<resource->getNumObjects(); i++ )
			fillChildItemList( resource->getElement( i ), newNode );
	}
	else if( tag != LayoutManager::className && tag != LayoutData::className  && tag != ITEMS_TAG )
		throw LibraryException( STRING( "Unkown tag " ) + tag );
}

void GuiBuilderWindow::refreshChildSelect( void )
{
	if( m_editorMode == emFORM )
	{
		xml::Element	*resource = getSelectedTopResource();
		if( resource )
		{
			childSelect.clearEntries();
			m_childResources.clear();

			fillChildItemList( resource, NULL );

			if( !childSelect.isVisible() )
			{
				treeSelect.hide();
				childSelect.show();
				mainArea.doLayout();
			}
		}
	}
}

void GuiBuilderWindow::loadResource( void )
{
	xml::Element	*resource = getSelectedTopResource();
	if( resource )
	{
		STRING		tag = resource->getTag();
		if( tag == FORM_TAG || tag == FRAME_TAG || tag == SCROLLER_TAG || tag == MENU_TAG || tag == LIST_TAG )
		{
			deleteDesignerForm();

			childSelect.clearEntries();
			m_childResources.clear();
			treeSelect.clear();

			if( tag == FORM_TAG || tag == FRAME_TAG || tag == SCROLLER_TAG )
			{
				m_editorMode = emFORM;

				designerForm = new DesignerForm( resource, this );

				createForm( m_docFileName, resource, designerForm, NULL, true );
				designerForm->catchControls();

				fillChildItemList( resource, NULL );

				if( !childSelect.isVisible() )
				{
					treeSelect.hide();
					childSelect.show();
					mainArea.doLayout();
				}
			}
			else if( tag == LIST_TAG )
			{
				m_editorMode = emSTRINGS;

				fillChildItemList( resource, NULL );

				stringListEditor = new StringListEditor();
				stringListEditor->create( this, m_childResources );

				if( !childSelect.isVisible() )
				{
					treeSelect.hide();
					childSelect.show();
					mainArea.doLayout();
				}
			}
			else
			{
				m_editorMode = emMENU;

				fillMenuItemList( resource, NULL );

				if( !treeSelect.isVisible() )
				{
					childSelect.hide();
					treeSelect.show();
					mainArea.doLayout();
				}
			}
		}
		else
		{
			throw gak::LibraryException( gak::STRING("Unknow resource type ") + resource->getTag() );
		}
	}
	enableDisaleProperties();
}

void GuiBuilderWindow::enableDisaleProperties( void )
{
	if( m_editorMode == emMENU )
	{
		enableDisaleMenuProperties();
	}
	else if( m_editorMode == emFORM )
	{
		enableDisaleFormProperties();
	}
	else if( m_editorMode == emSTRINGS )
	{
		enableDisaleStringsProperties();
	}

	properties.doLayout();
	invalidateWindow();
}

void GuiBuilderWindow::enableNameProperties( void )
{
	properties.enable();

	disableChild( properties.xPOSLabel );
	disableChild( properties.xPOS );
	disableChild( properties.yPOSLabel );
	disableChild( properties.yPOS );
	disableChild( properties.childWidthLabel );
	disableChild( properties.childWidth );
	disableChild( properties.childHeightLabel );
	disableChild( properties.childHeight );

	disableChild( properties.layout_xPOSLabel );
	disableChild( properties.layout_xPOS );
	disableChild( properties.layout_yPOSLabel );
	disableChild( properties.layout_yPOS );
	disableChild( properties.layoutWIDTHLabel );
	disableChild( properties.layoutWIDTH );
	disableChild( properties.layoutHEIGHTLabel );
	disableChild( properties.layoutHEIGHT );

	disableChild( properties.growWIDTHLabel );
	disableChild( properties.growWIDTH );
	disableChild( properties.growHEIGHTLabel );
	disableChild( properties.growHEIGHT );

	disableChild( properties.paddingLEFTLabel );
	disableChild( properties.paddingLEFT );
	disableChild( properties.paddingRIGHTLabel );
	disableChild( properties.paddingRIGHT );
	disableChild( properties.paddingTOPLabel );
	disableChild( properties.paddingTOP );
	disableChild( properties.paddingBOTTOMLabel );
	disableChild( properties.paddingBOTTOM );

	disableChild( properties.attachmentLabel );
	disableChild( properties.attachment );
	disableChild( properties.layoutManagerLabel );
	disableChild( properties.layoutManager );

	disableChild( properties.marginLEFTLabel );
	disableChild( properties.marginLEFT );
	disableChild( properties.marginRIGHTLabel );
	disableChild( properties.marginRIGHT );
	disableChild( properties.marginTOPLabel );
	disableChild( properties.marginTOP );
	disableChild( properties.marginBOTTOMLabel );
	disableChild( properties.marginBOTTOM );

	disableChild( properties.BackgroundColorLabel );
	disableChild( properties.BackgroundColor );

	disableChild( properties.SelectFontCheck );
	disableChild( properties.SelectFontButton );

	disableChild( properties.controlStyleLabel );
	disableChild( properties.controlStyle );

	disableChild( properties.baseClassLabel );
	disableChild( properties.baseClass );
	disableChild( properties.itemListLabel );
	disableChild( properties.itemList );
	disableChild( properties.numColsLABEL );
	disableChild( properties.numCols );
	disableChild( properties.numRowsLABEL );
	disableChild( properties.numRows );
}

void GuiBuilderWindow::enableDisaleMenuProperties( void )
{
	TreeNode *selected = treeSelect.getSelection();
	if( !selected )
	{
		properties.disable();
/***/	return;
	}
	enableNameProperties();
}

void GuiBuilderWindow::enableDisaleStringsProperties( void )
{
	ArrayOfInts	selItems;
	size_t		numSelected = childSelect.getSelectedItems( &selItems );
	if( !numSelected )
	{
		properties.disable();
/***/	return;
	}
	enableNameProperties();
}

void GuiBuilderWindow::enableDisaleFormProperties( void )
{
	ArrayOfInts	selItems;
	size_t		numSelected = childSelect.getSelectedItems( &selItems );
	if( !numSelected )
	{
		properties.disable();
/***/	return;
	}

	properties.enable();

	enableChild( properties.xPOSLabel );
	enableChild( properties.xPOS );
	enableChild( properties.yPOSLabel );
	enableChild( properties.yPOS );
	enableChild( properties.childWidthLabel );
	enableChild( properties.childWidth );
	enableChild( properties.childHeightLabel );
	enableChild( properties.childHeight );

	enableChild( properties.layout_xPOSLabel );
	enableChild( properties.layout_xPOS );
	enableChild( properties.layout_yPOSLabel );
	enableChild( properties.layout_yPOS );
	enableChild( properties.layoutWIDTHLabel );
	enableChild( properties.layoutWIDTH );
	enableChild( properties.layoutHEIGHTLabel );
	enableChild( properties.layoutHEIGHT );

	enableChild( properties.growWIDTHLabel );
	enableChild( properties.growWIDTH );
	enableChild( properties.growHEIGHTLabel );
	enableChild( properties.growHEIGHT );

	enableChild( properties.paddingLEFTLabel );
	enableChild( properties.paddingLEFT );
	enableChild( properties.paddingRIGHTLabel );
	enableChild( properties.paddingRIGHT );
	enableChild( properties.paddingTOPLabel );
	enableChild( properties.paddingTOP );
	enableChild( properties.paddingBOTTOMLabel );
	enableChild( properties.paddingBOTTOM );

	enableChild( properties.attachmentLabel );
	enableChild( properties.attachment );
	enableChild( properties.layoutManagerLabel );
	enableChild( properties.layoutManager );

	if( properties.layoutManager->getSelection() <= 0 )
	{
		disableChild( properties.marginLEFTLabel );
		disableChild( properties.marginLEFT );
		disableChild( properties.marginRIGHTLabel );
		disableChild( properties.marginRIGHT );
		disableChild( properties.marginTOPLabel );
		disableChild( properties.marginTOP );
		disableChild( properties.marginBOTTOMLabel );
		disableChild( properties.marginBOTTOM );
	}
	else
	{
		enableChild( properties.marginLEFTLabel );
		enableChild( properties.marginLEFT );
		enableChild( properties.marginRIGHTLabel );
		enableChild( properties.marginRIGHT );
		enableChild( properties.marginTOPLabel );
		enableChild( properties.marginTOP );
		enableChild( properties.marginBOTTOMLabel );
		enableChild( properties.marginBOTTOM );
	}

	enableChild( properties.BackgroundColorLabel );
	enableChild( properties.BackgroundColor );

	enableChild( properties.SelectFontCheck );
	enableChild( properties.SelectFontButton );

	enableChild( properties.controlStyleLabel );
	enableChild( properties.controlStyle );

	bool	listbox = false;
	bool	combobox = false;
	bool	form = false;
	bool	grid = false;

	for( size_t i=0; i<numSelected; i++ )
	{
		xml::Element	*childResource = m_childResources[selItems[i]];
		STRING			type = childResource->getAttribute( TYPE_ATTR );
		if( type == ListBox::className )
		{
			listbox = true;
		}
		else if( type == ComboBox::className )
		{
			combobox = true;
		}
		else if( type == GridViewer::className )
		{
			grid = true;
		}
		else if( childResource->getTag() == FORM_TAG )
		{
			form = true;
		}
	}

	if( form )
	{
		enableChild( properties.baseClassLabel );
		enableChild( properties.baseClass );
	}
	else
	{
		disableChild( properties.baseClassLabel );
		disableChild( properties.baseClass );
	}

	if( combobox || listbox )
	{
		enableChild( properties.itemListLabel );
		enableChild( properties.itemList );
	}
	else
	{
		disableChild( properties.itemListLabel );
		disableChild( properties.itemList );
	}

	if( grid )
	{
		enableChild( properties.numColsLABEL );
		enableChild( properties.numCols );
		enableChild( properties.numRowsLABEL );
		enableChild( properties.numRows );
	}
	else
	{
		disableChild( properties.numColsLABEL );
		disableChild( properties.numCols );
		disableChild( properties.numRowsLABEL );
		disableChild( properties.numRows );
	}
}

void GuiBuilderWindow::newMenu()
{
	xml::Element *gui = m_guiDoc->getRoot();
	if( gui )
	{
		xml::Element	*menus = gui->getElement( MENUS_TAG );
		if( !menus )
		{
			menus = gui->addObject( new xml::Any( MENUS_TAG ) );
		}

		xml::Element *newMenu = menus->addObject( new xml::Any( MENU_TAG ) );
		STRING name = "MENU";
		newMenu->setStringAttribute( NAME_ATTR, name );
		addSelectedTopResource( newMenu, name );
		loadResource();

		setChangedFlag();
	}
}
void GuiBuilderWindow::newStringList()
{
	xml::Element	*gui = m_guiDoc->getRoot();
	if( gui )
	{
		xml::Element	*strings = gui->getElement( STRINGS_TAG );
		if( !strings )
			strings= gui->addObject( new xml::Any( STRINGS_TAG ) );

		xml::Element *list = strings->addObject( new xml::Any( LIST_TAG ) );
		STRING name = "STRINGLIST";
		list->setStringAttribute( NAME_ATTR, name );
		addSelectedTopResource( list, name );
		loadResource();

		setChangedFlag();
	}
}
void GuiBuilderWindow::newFrame()
{
	xml::Element	*gui = m_guiDoc->getRoot();
	if( gui )
	{
		xml::Element	*frames = gui->getElement( FRAMES_TAG );
		if( !frames )
			frames = gui->addObject( new xml::Any( FRAMES_TAG ) );

		xml::Element *newFrame = frames->addObject( new xml::Any( FRAME_TAG ) );
		STRING name = "FRAME";
		newFrame->setStringAttribute( NAME_ATTR, name );
		addSelectedTopResource( newFrame, name );
		loadResource();

		setChangedFlag();
	}
}

void GuiBuilderWindow::newScroller()
{
	xml::Element	*gui = m_guiDoc->getRoot();
	if( gui )
	{
		xml::Element	*frames = gui->getElement( FRAMES_TAG );
		if( !frames )
			frames = gui->addObject( new xml::Any( FRAMES_TAG ) );

		xml::Element *newFrame = frames->addObject( new xml::Any( SCROLLER_TAG ) );
		STRING name = "SCROLLER";
		newFrame->setStringAttribute( NAME_ATTR, name );
		addSelectedTopResource( newFrame, name );
		loadResource();

		setChangedFlag();
	}
}

void GuiBuilderWindow::newForm()
{
	xml::Element	*gui = m_guiDoc->getRoot();
	if( gui )
	{
		xml::Element	*forms = gui->getElement( FORMS_TAG );
		if( !forms )
			forms = gui->addObject( new xml::Any( FORMS_TAG ) );

		xml::Element *newForm = forms->addObject( new xml::Any( FORM_TAG ) );
		STRING name = "FORM";
		newForm->setStringAttribute( NAME_ATTR, name );
		newForm->setIntegerAttribute( STYLE_ATTR, WS_VISIBLE|WS_BORDER|WS_DLGFRAME );
		newForm->setIntegerAttribute( LayoutData::widthAttr, 640 );
		newForm->setIntegerAttribute( LayoutData::heightAttr, 480 );
		addSelectedTopResource( newForm, name );
		loadResource();

		setChangedFlag();
	}
}

void GuiBuilderWindow::newDocument()
{
	deleteGuiDoc();

	m_guiDoc = new xml::Document( "" );
	m_guiDoc->addObject( new xml::Declaration );
	m_guiDoc->addObject( new xml::Any( "gui" ) );

	setDocument( NULL_STRING );
	clrChangedFlag();
}

void GuiBuilderWindow::newStyle( unsigned long newStyle, unsigned long styleMask )
{
	styleMask = ~styleMask;

	ArrayOfInts	selected;

	size_t	numSelected = childSelect.getSelectedItems( &selected );
	for( size_t i=0; i<numSelected; i++ )
	{
		xml::Element *resource = m_childResources[selected[i]];
		unsigned long currentStyle = resource->getAttribute( STYLE_ATTR ).getValueN<unsigned long>();
		currentStyle &= styleMask;
		currentStyle |= newStyle;
		resource->setIntegerAttribute( STYLE_ATTR, currentStyle );
	}
}

bool GuiBuilderWindow::checkChangedFlag( void )
{
	if( m_changedFlag )
	{
		int buttonID = messageBox( "Document has changed. Do you want to save?", "MG GUI Builder", MB_ICONQUESTION|MB_YESNOCANCEL );
		if( buttonID == IDCANCEL )
		{
			return false;
		}
		else if( buttonID == IDYES )
		{
			return saveDocument();
		}
	}

	return true;
}

void GuiBuilderWindow::loadTranslations( xml::Document *doc )
{
	xml::Element	*translations = doc->getRoot();
	if( translations )
	{
		xml::XmlArray	languages;
		translations->getChildElements( &languages, LANGUAGE_TAG );
		for( 
			xml::XmlArray::const_iterator it = languages.cbegin(), endIT = languages.cend();
			it != endIT;
			++it
		)
		{
			xml::Element *language = *it;
			STRING	lang = language->getAttribute( LANG_ATTRIBUTE );
			Dictionary	&currentDict = m_dictionarys[lang];

			xml::XmlArray	items;
			language->getChildElements( &items, ITEM_TAG );
			for( 
				xml::XmlArray::const_iterator it = items.cbegin(), endIT = items.cend();
				it != endIT;
				++it
			)
			{
				const xml::Element *item = *it;
				STRING caption = item->getAttribute( CAPTION_ATTR );
				STRING translation = item->getAttribute( TRANSLATION_ATTRIBUTE );

				currentDict[caption] = translation;
			}
		}
	}
}

void GuiBuilderWindow::loadGUI( xml::Document *doc )
{
	xml::Element	*gui = doc->getRoot();
	if( gui )
	{
		if( doc != m_guiDoc )
		{
			deleteGuiDoc();
			m_guiDoc = doc;
		}

		/*
			need special processing for menus, because they may contain menus, too
		*/
		xml::Element	*menus = gui->getElement( MENUS_TAG );
		if( menus )
		{
			for( size_t i=0; i<menus->getNumObjects(); i++ )
			{
				xml::Element *menu = menus->getElement( i );
				STRING		name = menu->getAttribute( NAME_ATTR );
				addTopResource( menu, name );
			}
		}

		/*
			all other resource types
		*/
		xml::XmlArray	items;
		const char	*frameTypes[] = { FORM_TAG, FRAME_TAG, SCROLLER_TAG, LIST_TAG };
		for( size_t i=0; i<arraySize(frameTypes); i++ )
		{
			gui->getAllElements( &items, frameTypes[i] );
			for( 
				xml::XmlArray::const_iterator it = items.cbegin(), endIT = items.cend();
				it != endIT;
				++it
			)
			{
				xml::Element *item = *it;
				STRING		name = item->getAttribute( NAME_ATTR );
				addSelectedTopResource( item, name );
			}
		}
	}
}

void GuiBuilderWindow::loadDocument( void )
{
	OpenFileDialog	openFile;
	if( openFile.create( this, OPEN_GUI_FILE_id, GuiFilterList_ids, GuiFilterList_count ) == IDOK )
	{
		F_STRING		guiFileName = openFile.getFilename();
		xml::Parser		theGuiParser( guiFileName );
		xml::Document	*newDoc = theGuiParser.readFile( false );
		if( newDoc )
		{
			loadGUI( newDoc );
		}

		F_STRING	translationFileName = guiFileName + '.';
		translationFileName += TRANSLATIONS;
		if( gak::exists( translationFileName ) )
		{
			xml::Parser		theTranslationParser( translationFileName );
			std::auto_ptr<xml::Document> translationDoc( theTranslationParser.readFile( false ) );
			if( translationDoc.get() )
			{
				loadTranslations( translationDoc.get() );
			}
		}

		setDocument( guiFileName );
		clrChangedFlag();
	}
}

GuiBuilderWindow::IdentifiersMap GuiBuilderWindow::createIDs( void )
{
	IdentifiersMap	identifiers;
	xml::XmlArray	items;
	int				id;

	const char *childTypes[] = { FORM_TAG, FRAME_TAG, SCROLLER_TAG, MENU_TAG, CHILD_TAG, STRING_TAG };
	STRING	name;
	for( size_t i=0; i<arraySize(childTypes); i++ )
	{
		m_guiDoc->getAllElements( &items, childTypes[i] );
		for( size_t i=0; i<items.size(); i++ )
		{
			xml::Element	*item = items[i];
			name = item->getAttribute( NAME_ATTR );
			if( !name.isEmpty() )
			{
				id = checkDefaultIdentifier( name );
				if( !id )
				{
					if( identifiers.hasElement( name ) )
					{
						id = identifiers[name];
					}
					else
					{
						id = int(identifiers.size()) + 128;
						identifiers[name] = id;
					}
				}
				item->setIntegerAttribute( ID_ATTR, id );
			}
			else
			{
				item->deleteAttribute( ID_ATTR );
			}
		}
	}

	return identifiers;
}

STRING GuiBuilderWindow::saveGui( const F_STRING &fileName )
{
	STRING	xmlCode = m_guiDoc->generate( xml::XML_MODE );
	std::ofstream	stream( fileName );
	stream << xmlCode;

	return xmlCode;
}

void GuiBuilderWindow::saveHeader( const F_STRING &fileName, const IdentifiersMap &identifiers )
{
	F_STRING		hFile = fileName + ".h";
	std::ofstream	stream( hFile );

	STRING macro = hFile;
	macro.replaceChar( '.', '_' );
	macro.replaceChar( ':', '_' );
	macro.replaceChar( DIRECTORY_DELIMITER, '_' );
	macro.upperCase();

	stream <<	"/*\n"
				"\tthis file was created from " << fileName << " with MG Gui Builder\n"
				"\t+++++++++++++++++++++++\n"
				"\t!!!!! Do not Edit !!!!!\n"
				"\t+++++++++++++++++++++++\n"
				"*/\n"
				"#ifndef " << macro << "\n"
				"#define " << macro << "\n"
				"#include <winlib/popup.h>\n"
				"#include <winlib/frame.h>\n"
				"#include <winlib/scrollFrame.h>\n"
				"#include <winlib/ControlW.h>\n"
				"#include <winlib/xmlEditorChild.h>\n"
				"#include <winlib/gridView.h>\n\n"
				"#include <winlib/winApp.h>\n\n"
				"namespace winlibGUI {\n\n";
	for( 
		IdentifiersMap::const_iterator	it = identifiers.cbegin(), endIT = identifiers.cend();
		it != endIT;
		++it
	) 
	{
		const STRING &name = it->getKey();
		if( !checkDefaultIdentifier( name ) )
		{
			stream << "\tconst int " << name << "_id=" << it->getValue() << ";\n";
		}
	}

	stream << "\n\tclass GuiApplication : public winlib::Application {\n"
			<< "\t\tpublic:\n"
			<< "\t\tvirtual gak::xml::Document *getGuiDoc( void );\n"
			"\t\tGuiApplication(int iconID=-1) : winlib::Application(iconID) {}\n"
			"\t};\n";

	for( 
		xml::XmlArray::iterator	it = m_topResources.begin(), endIT = m_topResources.end();
		it != endIT;
		++it
	)
	{
		xml::Element		*resource = *it;
		STRING			tag = resource->getTag();
		STRING			name = resource->getAttribute( NAME_ATTR );
		if( (tag == FORM_TAG || tag==FRAME_TAG || tag==SCROLLER_TAG) && !name.isEmpty() )
		{
			STRING	parentClass, baseClass;
			if( tag == FORM_TAG )
			{
				baseClass = resource->getAttribute( "baseClass" );
				if( baseClass.isEmpty() )
					baseClass = "PopupWindow";
				parentClass = "BasicWindow";
			}
			else if( tag == SCROLLER_TAG )
			{
				baseClass = ScrollFrame::className;
				parentClass = "CallbackWindow";
			}
			else
			{
				baseClass = FrameChild::className;
				parentClass = "CallbackWindow";
			}
			stream << "\n\tclass " << name << '_' << tag << " : public winlib::" << baseClass << " {\n"
				"\t\tpublic:\n"
				<< "\t\t" << name << '_' << tag << "(winlib::BasicWindow *owner) : " << baseClass << "(owner) {}\n"
				<< "\t\twinlib::SuccessCode create(winlib::" << parentClass << "*parent) {\n"
				"\t\t\treturn " << baseClass << "::create(parent,\"" << name << "\");\n"
				"\t\t}\n\n"
			;

			xml::XmlArray	children;
			resource->getAllElements( &children, CHILD_TAG );
			for( 
				xml::XmlArray::iterator	it = children.begin(), endIT = children.end();
				it != endIT;
				++it
			)
			{
				xml::Element	*child = *it;
				STRING		type = child->getAttribute( TYPE_ATTR );
				STRING		name = child->getAttribute( NAME_ATTR );
				if( !type.isEmpty() && !name.isEmpty() )
				{
					if( checkDefaultIdentifier( name ) )
					{
						name += '_';
						name += type;
					}
					stream << "\t\twinlib::" << type << " *" << name << ";\n";
				}
			}

			stream << "\t\tprivate:\n"
				"\t\tvirtual void getControls();\n"
				"\t};\t// end of " << name << '\n';
		}
		else if( tag == LIST_TAG )
		{
			if( !name.isEmpty() )
			{
				stream << "\textern const int " << name << "_ids[];\n";
				stream << "\tstatic const int " << name << "_count = " << resource->getNumObjects() << ";\n";
			}
		}
	}
	stream << "}\t// namespace winlibGUI\n\n"
		"#endif // " << macro << '\n'
	;
}

void GuiBuilderWindow::saveCpp( const F_STRING &fileName, const STRING &xmlGuiSrc )
{
	F_STRING		hFile = fileName + ".h";
	F_STRING		cppFile = fileName + ".cpp";
	std::ofstream	stream( cppFile );

	stream <<	"/*\n"
				"\tthis file was created from " << fileName << " with MG Gui Builder\n"
				"\t+++++++++++++++++++++++\n"
				"\t!!!!! Do not Edit !!!!!\n"
				"\t+++++++++++++++++++++++\n"
				"*/\n\n"
				"#include \"" << hFile << "\"\n\n"
				"namespace winlibGUI {\n\n";

	stream <<	"\tgak::xml::Document *GuiApplication::getGuiDoc() {\n"
					"\t\tgak::xml::Document *doc = winlib::Application::getGuiDoc();\n"
					"\t\tif(!doc) {\n"
						"\t\t\tgak::STRING xmlSrc = " << xmlGuiSrc.cString() << ";\n"
						"\t\t\treturn Application::getGuiDoc( xmlSrc );\n"
					"\t\t}\n"
					"\t\treturn NULL;\n"
				"\t}\n\n";
	for( 
		xml::XmlArray::iterator	it = m_topResources.begin(), endIT = m_topResources.end();
		it != endIT;
		++it
	)
	{
		xml::Element		*resource = *it;
		STRING			tag = resource->getTag();
		STRING			name = resource->getAttribute( NAME_ATTR );
		if( (tag == FORM_TAG || tag==FRAME_TAG || tag==SCROLLER_TAG) && !name.isEmpty() )
		{
			stream << "\n\tvoid " << name << '_' << tag << "::getControls() {\n";
			xml::XmlArray	children;
			resource->getAllElements( &children, CHILD_TAG );
			for( 
				xml::XmlArray::iterator	it = children.begin(), endIT = children.end();
				it != endIT;
				++it
			)
			{
				xml::Element	*child = *it;
				STRING		type = child->getAttribute( TYPE_ATTR );
				STRING		name = child->getAttribute( NAME_ATTR );
				STRING		idName = name;
				if( !type.isEmpty() && !name.isEmpty() )
				{
					if( checkDefaultIdentifier( name ) )
					{
						name += '_';
						name += type;
					}
					else
						idName += "_id";

					stream << "\t\t" << name << "=static_cast<winlib::" << type << "*>(findChild(" << idName << "));\n";
				}
			}

			stream << "\t}\t// " << name << '\n';
		}
		else if( tag == LIST_TAG )
		{
			xml::XmlArray	children;
			resource->getAllElements( &children, STRING_TAG );
			if( children.size() && !name.isEmpty() )
			{
				stream << "\tconst int " << name << "_ids[] = {\n";
				for( 
					xml::XmlArray::const_iterator	it = children.cbegin(), endIT = children.cend();
					it != endIT;
					++it
				)
				{
					xml::Element	*string = *it;
					STRING			name = string->getAttribute( NAME_ATTR );
					if( !name.isEmpty() )
					{
						stream << "\t\t" << name << "_id,\n";
					}
				}
				stream << "\t};\n";
			}
		}
	}
	stream << "}\t// namespace winlibGUI\n";
}

void GuiBuilderWindow::saveDictionaries( const STRING &fileName )
{
	if( m_dictionarys.size() )
	{
		xml::Any	translations( TRANSLATIONS );
		for(
			Dictionarys::const_iterator it = m_dictionarys.cbegin(), endIT = m_dictionarys.cend();
			it != endIT;
			++it
		)
		{
			const Dictionary &dictionary = it->getValue();

			if( dictionary.size() )
			{
				const STRING &lang = it->getKey();

				xml::Element *language = translations.addObject( new xml::Any( LANGUAGE_TAG ) );
				language->setStringAttribute( LANG_ATTRIBUTE, lang );
				for(
					Dictionary::const_iterator it = dictionary.cbegin(), endIT = dictionary.cend();
					it != endIT;
					++it
				)
				{
					const STRING &caption = it->getKey();
					const STRING &translation = it->getValue();
					xml::Element *item = language->addObject( new xml::Any( ITEM_TAG ) );
	
					item->setStringAttribute( CAPTION_ATTR, caption );
					item->setStringAttribute( TRANSLATION_ATTRIBUTE, translation );
				}
			}
		}
		STRING			translationFileName = fileName + '.';
		translationFileName += TRANSLATIONS;

		STRING	xmlCode = translations.generateDoc();
		std::ofstream	stream( translationFileName );
		stream << xmlCode;
	}
}

void GuiBuilderWindow::saveTranslations( const F_STRING &fileName )
{
	F_STRING	plainName = removeExtension( fileName );
	for(
		Dictionarys::const_iterator it = m_dictionarys.cbegin(), endIT = m_dictionarys.cend();
		it != endIT;
		++it
	)
	{
		F_STRING newName = plainName + '.' + it->getKey() + ".gui";
		const Dictionary &dict = it->getValue();

		std::auto_ptr<xml::Element> newDocument( m_guiDoc->copy() );

		translateCaptions( newDocument.get(), dict );

		STRING	xmlCode = newDocument->generateDoc();
		std::ofstream	stream( newName );
		stream << xmlCode;
	}
}

void GuiBuilderWindow::saveDocumentAs( const F_STRING &fileName )
{
	IdentifiersMap	identifiers = createIDs();

	STRING xmlGuiSrc = saveGui( fileName );
	saveHeader( fileName, identifiers );
	saveCpp( fileName, xmlGuiSrc );
	saveDictionaries( fileName );
	saveTranslations( fileName );

	clrChangedFlag();
}

bool GuiBuilderWindow::saveDocumentAs( void )
{
	SaveFileAsDialog	saveFile;
	if( saveFile.create( this, SAVE_GUI_FILE_id, GuiFilterList_ids, GuiFilterList_count ) == IDOK )
	{
		F_STRING	fileName = saveFile.getFilename();
		saveDocumentAs( fileName );
		setDocument( fileName );

		return true;
	}
	else
	{
		return false;
	}
}

bool GuiBuilderWindow::saveDocument( void )
{
	if( !getDocument().isEmpty() )
	{
		saveDocumentAs( getDocument() );
		return true;
	}
	else
	{
		return saveDocumentAs();
	}
}

void GuiBuilderWindow::handleChildNameChange( void )
{
	ArrayOfInts	selected;
	size_t	numSelected = childSelect.getSelectedItems( &selected );
	if( numSelected )
	{
		xml::Element *resource = m_childResources[selected[0]];
		STRING	oldName = resource->getAttribute( NAME_ATTR );
		STRING	newName = properties.controlName->getText();
		STRING	resourceTag = resource->getTag();

		resource->setStringAttribute( NAME_ATTR, newName );
		if( newName.isEmpty() )
		{
			resource->deleteAttribute( ID_ATTR );
			if( resourceTag == CHILD_TAG )
			{
				newName = resource->getAttribute( TYPE_ATTR );
			}

			if( newName.isEmpty() )
			{
				newName = resourceTag;
			}
		}

		childSelect.replaceEntry( selected[0], newName );
		childSelect.selectEntry( selected[0] );

		if( resourceTag == FORM_TAG )
		{
			renameTopSelected( newName );
		}
	}
}

void GuiBuilderWindow::changeStringResource( xml::Element *string, const STRING &name, const STRING &value, const BasicWindow *source )
{
	assert( stringListEditor );

	STRING	tag = string->getTag();
	string->setStringAttribute( NAME_ATTR, name );

	int	selIdx = int(m_childResources.findElement( string ));
	if( selIdx != m_childResources.no_index  )
	{
		childSelect.replaceEntry( selIdx, name );
		childSelect.selectEntry( selIdx );
	}

	if( tag == STRING_TAG )
		string->setStringAttribute( CAPTION_ATTR, value );
	else if( tag == LIST_TAG )
		renameTopSelected( name );

	if( source == this )
	{
		if( selIdx > 0 )
		{
			stringListEditor->setString( selIdx, name, value );
		}
	}
	else
	{
		m_loading = true;
		properties.controlName->setText( name );
		properties.childCaption->setText( value );
		m_loading = false;
	}
	setChangedFlag();
}

void GuiBuilderWindow::changeStringResource( size_t idx, const STRING &name, const STRING &value, const BasicWindow *source )
{
	xml::Element	*stringList = getSelectedTopResource();
	if( stringList )
	{
		while( idx >= m_childResources.size() )
		{
			addChildItem( stringList->addObject( new xml::Any( STRING_TAG ) )->setStringAttribute( NAME_ATTR, "NEW_STRNG" ), false );
		}
		xml::Element	*string = m_childResources[idx];
		changeStringResource( string, name, value, source );
	}
}

bool GuiBuilderWindow::checkLayoutData( void )
{
	STRING	value = properties.layout_xPOS->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.layout_yPOS->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.layoutWIDTH->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.layoutHEIGHT->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.growWIDTH->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.growHEIGHT->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.paddingLEFT->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.paddingRIGHT->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.paddingTOP->getText();
	if( !value.isEmpty() )
		return false;

	value = properties.paddingBOTTOM->getText();
	if( !value.isEmpty() )
		return false;

	if( properties.attachment->getSelection() > 0 )
		return false;

	designerForm->removeLayoutData();
	designerForm->doLayout();

	return true;
}

void GuiBuilderWindow::editItemList( void )
{
	/*
		find first item with items tag
	*/
	STRING			oldListItems;
	ArrayOfInts		selItems;
	size_t			numSelected = childSelect.getSelectedItems( &selItems );

	for( size_t i=0; i<numSelected; i++ )
	{
		xml::Element		*resource = m_childResources[selItems[i]];
		xml::Element		*items = resource->getElement( ITEMS_TAG );
		if( items )
		{
			for( size_t i=0; i<items->getNumObjects(); i++ )
			{
				xml::Element	*item = items->getElement( i );
				if( i )
				{
					oldListItems += "\r\n";
				}
				oldListItems += item->getValue( xml::PLAIN_MODE );
			}

			break;
		}
	}

	/*
		open the editor
	*/
	StringEditor	stringEditor;
	STRING newListItems = stringEditor.create( this, "Edit Items", oldListItems, false );
	if( oldListItems != newListItems )
	{
		ArrayOfStrings		itemList;
		itemList.createElements( newListItems, "\n" );

		/*
			create the XML tree
		*/
		xml::Element	*newItems = new xml::Any( ITEMS_TAG );
		for( 
			ArrayOfStrings::iterator it = itemList.begin(), endIT = itemList.end();
			it != endIT;
			++it
		)
		{
			xml::Element	*item = newItems->addObject( new xml::Any( ITEM_TAG ) );
			item->addObject( new xml::PCData( it->stripBlanks() ) );
		}

		/*
			replace all selected items lists with this new list
		*/
		for( size_t i=0; i<numSelected; i++ )
		{
			xml::Element		*resource = m_childResources[selItems[i]];
			xml::Element		*items = resource->getElement( ITEMS_TAG );
			if( items )
			{
				resource->removeObject( items );
				delete items;
			}
			if( itemList.size() )
			{
				STRING	type = resource->getAttribute( TYPE_ATTR );
				if( type==ListBox::className || type==ComboBox::className )
				{
					resource->addObject( newItems->copy() );

					if( type==ComboBox::className )
					{
						int oldHeight = resource->getAttribute( LayoutData::heightAttr ).getValueN<int>();
						int minHeight = int(itemList.size())*13+21+2;
						if( oldHeight < minHeight )
						{
							resource->setIntegerAttribute( LayoutData::heightAttr, minHeight );
						}
					}
				}
			}
		}

		/*
			since we have added a copy to the items, we can delete the newItems tree
		*/
		delete newItems;

		setChangedFlag();
	}
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
ProcessStatus GuiBuilderWindow::handleEditChange( int cmd )
{
	if( m_loading )
	{
		return psPROCESSED;
	}

	switch( cmd )
	{
		case controlName_id:
		{
			if( m_editorMode == emFORM )
			{
				handleChildNameChange();
			}
			else if( m_editorMode == emMENU )
			{
				TreeNode	*selected = treeSelect.getSelection();
				if( selected )
				{
					xml::Element	*resource = (xml::Element*)selected->getData();
					resource->setStringAttribute( NAME_ATTR, properties.controlName->getText() );
				}
			}
			else if( m_editorMode == emSTRINGS )
			{
				xml::Element	*stringResource = getSelectedChildResource();
				if( stringResource )
				{
					STRING newName = properties.controlName->getText();
					STRING caption = properties.childCaption->getText();
					changeStringResource( stringResource, newName, caption, this );
				}
			}
			break;
		}
		case childCaption_id:
		{
			STRING caption = properties.childCaption->getText();
			if( m_editorMode == emFORM )
			{
				designerForm->setCaption( caption );
				designerForm->invalidateWindow();
			}
			else if( m_editorMode == emMENU )
			{
				TreeNode	*selected = treeSelect.getSelection();
				if( selected )
				{
					xml::Element	*resource = (xml::Element*)selected->getData();
					resource->setStringAttribute( CAPTION_ATTR, caption );
					treeSelect.changeLabel( selected, caption );
				}
			}
			else if( m_editorMode == emSTRINGS )
			{
				xml::Element	*stringResource = getSelectedChildResource();
				if( stringResource )
				{
					STRING newName = properties.controlName->getText();
					STRING caption = properties.childCaption->getText();
					changeStringResource( stringResource, newName, caption, this );
				}
			}
			break;
		}
		case xPOS_id:
		{
			int xPos = properties.xPOS->getText().getValueN<int>();
			designerForm->moveHorizontal( xPos );
			designerForm->invalidateWindow();
			break;
		}
		case yPOS_id:
		{
			int yPos = properties.yPOS->getText().getValueN<int>();
			designerForm->moveVertical( yPos );
			designerForm->invalidateWindow();
			break;
		}
		case childWidth_id:
		{
			int width = properties.childWidth->getText().getValueN<int>();
			designerForm->setWidth( width );
			designerForm->invalidateWindow();
			break;
		}
		case childHeight_id:
		{
			int height = properties.childHeight->getText().getValueN<int>();
			designerForm->setHeight( height );
			designerForm->invalidateWindow();
			break;
		}
		case layout_xPOS_id:
		{
			if( !checkLayoutData() )
			{
				int xPos = properties.layout_xPOS->getText().getValueN<int>();
				designerForm->setLayoutHorizontal( xPos );
				designerForm->doLayout();
			}
			break;
		}
		case layout_yPOS_id:
		{
			if( !checkLayoutData() )
			{
				int yPos = properties.layout_yPOS->getText().getValueN<int>();
				designerForm->setLayoutVertical( yPos );
				designerForm->doLayout();
			}
			break;
		}
		case layoutWIDTH_id:
		{
			if( !checkLayoutData() )
			{
				unsigned width = properties.layoutWIDTH->getText().getValueN<unsigned>();
				designerForm->setLayoutSizeHorizontal( width ? width : 1u );
				designerForm->doLayout();
			}
			break;
		}
		case layoutHEIGHT_id:
		{
			if( !checkLayoutData() )
			{
				unsigned height = properties.layoutHEIGHT->getText().getValueN<unsigned>();
				designerForm->setLayoutSizeVertical( height ? height : 1u );
				designerForm->doLayout();
			}
			break;
		}
		case growWIDTH_id:
		{
			if( !checkLayoutData() )
			{
				int growWidth = properties.growWIDTH->getText().getValueN<int>();
				designerForm->setLayoutGrowHorizontal( growWidth );
				designerForm->doLayout();
			}
			break;
		}
		case growHEIGHT_id:
		{
			if( !checkLayoutData() )
			{
				int growHeight = properties.growHEIGHT->getText().getValueN<int>();
				designerForm->setLayoutGrowVertical( growHeight );
				designerForm->doLayout();
			}
			break;
		}
		case paddingLEFT_id:
		{
			if( !checkLayoutData() )
			{
				int paddingLeft = properties.paddingLEFT->getText().getValueN<int>();
				designerForm->setPaddingLeft( paddingLeft );
				designerForm->doLayout();
			}
			break;
		}
		case paddingRIGHT_id:
		{
			if( !checkLayoutData() )
			{
				int paddingRight = properties.paddingRIGHT->getText().getValueN<int>();
				designerForm->setPaddingRight( paddingRight );
				designerForm->doLayout();
			}
			break;
		}
		case paddingTOP_id:
		{
			if( !checkLayoutData() )
			{
				int paddingTop = properties.paddingTOP->getText().getValueN<int>();
				designerForm->setPaddingTop( paddingTop );
				designerForm->doLayout();
			}
			break;
		}
		case paddingBOTTOM_id:
		{
			if( !checkLayoutData() )
			{
				int paddingBottom = properties.paddingBOTTOM->getText().getValueN<int>();
				designerForm->setPaddingBottom( paddingBottom );
				designerForm->doLayout();
			}
			break;
		}
		case marginLEFT_id:
		case marginRIGHT_id:
		case marginTOP_id:
		case marginBOTTOM_id:
		{
			int marginLeft = properties.marginLEFT->getText().getValueN<int>();
			int marginRight = properties.marginRIGHT->getText().getValueN<int>();
			int marginTop = properties.marginTOP->getText().getValueN<int>();
			int marginBottom = properties.marginBOTTOM->getText().getValueN<int>();
			designerForm->setMargin( marginLeft, marginRight, marginTop, marginBottom );
			designerForm->doLayout();
			break;
		}
		case numCols_id:
		{
			unsigned numCols = properties.numCols->getText().getValueN<unsigned>();
			designerForm->setNumCols( numCols );
			designerForm->invalidateWindow();
			break;
		}
		case numRows_id:
		{
			unsigned numRows = properties.numRows->getText().getValueN<unsigned>();
			designerForm->setNumRows( numRows );
			designerForm->invalidateWindow();
			break;
		}

		default:
			return OverlappedWindow::handleEditChange( cmd );
	}

	setChangedFlag();
	return psPROCESSED;
}

void GuiBuilderWindow::handleMenuEditor( WPARAM wParam )
{
	if( wParam == VK_INSERT )
	{
		TreeNode *selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		xml::Element	*newMenu = resource->addObject( new xml::Any( MENU_TAG ) );
		treeSelect.appendItem( selected, "New Menu", newMenu );
		newMenu->setStringAttribute( CAPTION_ATTR, "New Menu" );
	}
	else if( wParam == VK_DELETE )
	{
		TreeNode *selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		treeSelect.deleteItem( selected );
		resource->remove();
		delete resource;
	}
	else if( wParam == VK_UP && isControlKey() )
	{
		TreeNode *selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		TreeNode *parent = selected->getParent();
		size_t	index = selected->getIndex();
		if( index )
		{
			index--;
			resource->moveTo( index );
			treeSelect.selectItem(
				treeSelect.moveItem( selected, parent, index )
			);
		}
		else if( parent )
		{
			TreeNode *grandParent = parent->getParent();
			if( grandParent )
			{
				index = grandParent->getIndex( parent );
				resource->moveTo( (xml::Element*)grandParent->getData(), index );
				treeSelect.selectItem(
					treeSelect.moveItem( selected, grandParent, index )
				);
			}
		}
	}
	else if( wParam == VK_DOWN && isControlKey() )
	{
		TreeNode *selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		TreeNode *parent = selected->getParent();
		if( parent )
		{
			size_t	index = selected->getIndex();
			if( index < parent->getNumChildren() - 1 )
			{
				index++;
				resource->moveTo( index );
				treeSelect.selectItem(
					treeSelect.moveItem( selected, parent, index )
				);
			}
			else
			{
				TreeNode *grandParent = parent->getParent();
				if( grandParent )
				{
					index = grandParent->getIndex( parent )+1;
					resource->moveTo( (xml::Element*)grandParent->getData(), index );
					treeSelect.selectItem(
						treeSelect.moveItem( selected, grandParent, index )
					);
				}
			}
		}
	}

	setChangedFlag();
}

void GuiBuilderWindow::handleFormEditor( WPARAM wParam )
{
	if( !isControlKey() )
		return;

	if( wParam == VK_UP )
	{
		TreeNode		*selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		xml::Element	*layoutManager = resource->getParent()->getElement( LayoutManager::className );
		xml::Element	*layoutData = resource->getParent()->getElement( LayoutData::className );
		TreeNode		*parent = selected->getParent();
		size_t			index = selected->getIndex();
		if( index )
		{
			index--;
			treeSelect.selectItem(
				treeSelect.moveItem( selected, parent, index )
			);
			BasicWindow *child = getChildByResource( resource );
			if( child )
			{
				child->move( index );
				CallbackWindow *parent = dynamic_cast<CallbackWindow *>( 
					const_cast<BasicWindow *>( child->getParent() ) 
				);
				if( parent )
					parent->doLayout();
			}

			if( layoutManager && layoutManager->getIndex() <= index )
				index++;
			if( layoutData && layoutData->getIndex() <= index )
				index++;
			resource->moveTo( index );
		}
	}
	else if( wParam == VK_DOWN )
	{
		TreeNode		*selected = treeSelect.getSelection();
		xml::Element	*resource = (xml::Element *)selected->getData();
		xml::Element	*layoutManager = resource->getParent()->getElement( LayoutManager::className );
		xml::Element	*layoutData = resource->getParent()->getElement( LayoutData::className );
		TreeNode		*parent = selected->getParent();
		if( parent )
		{
			size_t	index = selected->getIndex();
			if( index < parent->getNumChildren() - 1 )
			{
				index++;
				treeSelect.selectItem(
					treeSelect.moveItem( selected, parent, index )
				);

				BasicWindow *child = getChildByResource( resource );
				if( child )
				{
					child->move( index );
					CallbackWindow *parent = dynamic_cast<CallbackWindow *>( 
						const_cast<BasicWindow *>( child->getParent() ) 
					);
					if( parent )
						parent->doLayout();
				}

				if( layoutManager && layoutManager->getIndex() <= index )
					index++;
				if( layoutData && layoutData->getIndex() <= index )
					index++;
				resource->moveTo( index );
			}
		}
	}

	setChangedFlag();
}

void GuiBuilderWindow::handleStringEditor( WPARAM wParam )
{
	assert( stringListEditor );

	if( wParam == VK_DELETE )
	{
		int selected = childSelect.getSelection();
		if( selected > 0 )
		{
			xml::Element	*resource = m_childResources[selected];
			childSelect.deleteEntry( selected );
			m_childResources.removeElementAt( selected );
			resource->remove();
			delete resource;

			stringListEditor->removeRow( selected );
		}

		setChangedFlag();
	}
}

void GuiBuilderWindow::postControlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM  )
{
	if( uMsg == WM_KEYUP )
	{
		if( control == &treeSelect )
		{
			if( m_editorMode == emMENU )
				handleMenuEditor( wParam );
			else if( m_editorMode == emFORM )
				handleFormEditor( wParam );
		}
		else if( control == &childSelect && m_editorMode == emSTRINGS )
		{
			handleStringEditor( wParam );
		}
	}
}

bool GuiBuilderWindow::handleTreeViewDrag( TreeView *, TreeNode *dragItem, TreeNode *dragOver )
{
	bool	allow = dragItem->getParent() != dragOver;

	if( allow )
	{
		for( 
			TreeNode *parent = dragOver;
			parent;
			parent = parent->getParent()
		)
		{
			if( parent == dragItem )
				allow = false;
		}

		if( allow && m_editorMode == emFORM )
		{
			xml::Element	*dropResource = (xml::Element *)dragOver->getData();
			CallbackWindow *dropChild = dynamic_cast<CallbackWindow *>( 
				const_cast<BasicWindow *>( getChildByResource( dropResource ) ) 
			);
			allow = (dropChild != NULL);
		}
	}

	SetCursor( LoadCursor( NULL, allow ? IDC_ARROW : IDC_NO ) );

	return allow;
}

void GuiBuilderWindow::handleTreeViewDrop( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dropTarget )
{
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

	if( !handleTreeViewDrag( dragTreeView, dragItem, dropTarget ) )
		return;

	/*
		move the xml resource to the new parent
	*/
	xml::Element	*dragResource = (xml::Element *)dragItem->getData();
	xml::Element	*dropResource = (xml::Element *)dropTarget->getData();

	dragResource->remove();
	dropResource->addObject( dragResource );

	// move the tree view item
	dragTreeView->moveItem( dragItem, dropTarget );

	/*
		move the child control
	*/
	if( m_editorMode == emFORM )
	{
		BasicWindow	*dragChild = getChildByResource( dragResource );
		CallbackWindow *dropChild = static_cast<CallbackWindow *>( 
			const_cast<BasicWindow *>( getChildByResource( dropResource ) ) 
		);

		dragChild->move( dropChild );
		designerForm->invalidateWindow();
	}

	setChangedFlag();
}

ProcessStatus GuiBuilderWindow::handleSelectionChange( int cmd )
{
	switch( cmd )
	{
		case RESOURCE_SELECT:
			loadResource();
			break;

		case ITEM_SELECT:
			assert( m_editorMode == emSTRINGS || m_editorMode == emFORM );
			if( m_editorMode == emFORM )
			{
				handleChildSelect();
			}
			else
			{
				handleStringSelect();
			}
			break;

		case TREE_SELECT:
		{
			assert( m_editorMode == emMENU || m_editorMode == emFORM );
			if( m_editorMode == emMENU )
			{
				TreeNode *myNode = treeSelect.getSelection();
				if( myNode )
				{
					loadMenuProperties( (xml::Element*)myNode->getData());
				}
			}
			else
			{
				handleItemTreeSelect();
			}
			break;
		}
		case layoutManager_id:
			setLayoutManager();
			break;

		case baseClass_id:
		{
			if( designerForm )
			{
				STRING	baseClass = properties.baseClass->getSelectedText();
				designerForm->setBaseClass( baseClass );
			}
			break;
		}

		case BackgroundColor_id:
		{
			if( designerForm )
			{
				size_t selected = properties.BackgroundColor->getSelection();
				STRING colorName = getColorName( selected );
				if( properties.BackgroundColor->getNumItems()-1 == selected )
				{
					ColorDialog	colorDialog;
					COLORREF	backgroundColor;
					if( colorDialog.create( this, &backgroundColor ) )
						designerForm->setBackgroundColor( colorName, backgroundColor );
				}
				else
					designerForm->setBackgroundColor( colorName, 0 );
			}
			break;
		}
		case attachment_id:
		{
			if( !checkLayoutData() )
			{
				int attachment = properties.attachment->getSelection()-1;
				if( attachment >= 0 )
				{
					designerForm->setAttachment( attachment );
					designerForm->doLayout();
				}
			}
			break;
		}

		default:
			return OverlappedWindow::handleSelectionChange( cmd );
	}

	return psPROCESSED;
}

ProcessStatus GuiBuilderWindow::handleButtonClick( int control )
{
	switch( control )
	{
		/*
			tool bar
		*/
		case SELECT_PUSH:
		case LABEL_PUSH:
		case BUTTON_PUSH:
		case CHECKBOX_PUSH:
		case RADIOBUTTON_PUSH:
		case GROUPBOX_PUSH:
		case LIST_PUSH:
		case TREEVIEW_PUSH:
		case TRACKBAR_PUSH:
		case SCROLLBAR_PUSH:
		case UPDPOWN_PUSH:
		case COMBO_PUSH:
		case EDIT_PUSH:
		case MEMO_PUSH:
		case FRAME_PUSH:
		case SCROLLER_PUSH:
		case XMLVIEW_PUSH:
		case GRIDVIEW_PUSH:
			m_currentId = control;
			break;

		/*
			properties
		*/
		case itemList_id:
			editItemList();
			break;

		case controlStyle_id:
		{
			StyleDialog	styleDialog( m_currentStyle, getUniqueType() );
			styleDialog.create( this );
			if( styleDialog.getModalResult() == IDOK )
			{
				m_currentStyle = styleDialog.getStyle();
				unsigned long styleMask = styleDialog.getStyleMask();
				newStyle( m_currentStyle, styleMask );
				setChangedFlag();
			}
			break;
		}
		case SelectFontCheck_id:
			assert( designerForm );

			if( !properties.SelectFontCheck->isActive() )
			{
				designerForm->setFont( "" );
				setChangedFlag();
				break;
			}

		case SelectFontButton_id:
		{
			assert( designerForm );

			BasicWindow *child = designerForm->getSelected();
			if( child )
			{
				Font	&font = child->getFont();
				if( !font.isAssigned() )
				{
					for( 
						const BasicWindow *parent = child->getParent();
						parent;
						parent = parent->getParent()
					)
					{
						font = parent->getFont();
						if( font.isAssigned() )
							break;
					}
				}
				if( font.selectFont( this, false ) )
				{
					designerForm->setFont( font.toString() );
					properties.SelectFontCheck->setActive();
					setChangedFlag();
				}
			}
		}
		case upBUTTON_id:
		{
			assert( designerForm );
			if( designerForm->moveUp() )
			{
				refreshChildSelect();
				designerForm->refreshSelection();
				setChangedFlag();
			}
			break;
		}
		case downBUTTON_id:
		{
			assert( designerForm );
			if( designerForm->moveDown() )
			{
				refreshChildSelect();
				designerForm->refreshSelection();
				setChangedFlag();
			}
			break;
		}
		default:
			return OverlappedWindow::handleButtonClick( control );
	}

	return psPROCESSED;
}

ProcessStatus GuiBuilderWindow::handleCommand( int cmd )
{
	switch( cmd )
	{
		/*
			menu items
		*/
		case FILE_NEW_id:
			if( checkChangedFlag() )
			{
				newDocument();
			}
			break;

		case FILE_NEW_FORM_id:
			newForm();
			break;

		case FILE_NEW_FRAME_id:
			newFrame();
			break;

		case FILE_NEW_SCROLLER_id:
			newScroller();
			break;

		case FILE_NEW_MENU_id:
			newMenu();
			break;

		case FILE_NEW_STRINGLIST_id:
			newStringList();
			break;

		case FILE_OPEN_id:
			if( checkChangedFlag() )
			{
				loadDocument();
			}
			break;

		case FILE_SAVE_id:
			saveDocument();
			break;

		case FILE_SAVE_AS_id:
			saveDocumentAs();
			break;

		case FILE_EXIT_id:
			if( checkChangedFlag() )
			{
				close();
			}
			break;

		case EDIT_TRANSLATE_id:
		{
			Captions		captions;
			collectCaptions( m_guiDoc, &captions );
			TranslateForm	theForm( captions, 	m_dictionarys );

			theForm.create( this );
			if( theForm.getModalResult() == IDOK )
			{
				m_dictionarys.moveFrom( theForm.getDictionarys() );
				setChangedFlag();
			}
			break;
		}

		case VIEW_ITEM_LIST_id:
			if( m_editorMode == emFORM )
			{
				treeSelect.hide();
				childSelect.show();
				mainArea.doLayout();
			}
			break;

		case VIEW_ITEM_TREE_id:
			if( m_editorMode == emFORM )
			{
				childSelect.hide();
				treeSelect.show();
				mainArea.doLayout();
			}
			break;

		case MENU_INFO_id:
		{
			MyAboutForm	theForm;

			theForm.create( this );
			break;
		}

		default:
			return OverlappedWindow::handleCommand( cmd );
	}

	return psPROCESSED;
}

bool GuiBuilderWindow::canClose( void )
{
	return checkChangedFlag();
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

SuccessCode GuiBuilderWindow::create( void )
{
	if( !setMenu( MAIN_MENU_id ) )
	{
		messageBox( "Unable to load Menu", "Error", MB_ICONERROR );
/*@*/	return scERROR;
	}

	SuccessCode error = OverlappedWindow::create( NULL, 640, 480 );
	if( error == scSUCCESS )
	{
		setBackgroundColorByIndex( scDIALOG_FACE );

		OverlappedWindow::setLayoutManager( new AttachmentManager );
		toolbar.resize( 0, 50 );
		toolbar.create( this );
		toolbar.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_TOP )->setGrowWidth( 1 ) );
		toolbar.setLayoutManager( new RowManager );

		btnSelectControl.setText( "Select" );
		btnSelectControl.setId( SELECT_PUSH );
		btnSelectControl.create( &toolbar );
		btnSelectControl.setActive();
		m_currentId = SELECT_PUSH;

		btnCreateLabel.setText( Label::className );
		btnCreateLabel.setId( LABEL_PUSH );
		btnCreateLabel.create( &toolbar );

		btnCreatePushButton.setText( "Button" );
		btnCreatePushButton.setId( BUTTON_PUSH );
		btnCreatePushButton.create( &toolbar );

		btnCreateCheckBox.setText( CheckBox::className );
		btnCreateCheckBox.setId( CHECKBOX_PUSH );
		btnCreateCheckBox.create( &toolbar );

		btnCreateRadioButton.setText( RadioButton::className );
		btnCreateRadioButton.setId( RADIOBUTTON_PUSH );
		btnCreateRadioButton.create( &toolbar );

		btnCreateGroupBox.setText( RadioButton::className );
		btnCreateGroupBox.setId( GROUPBOX_PUSH );
		btnCreateGroupBox.create( &toolbar );

		btnCreateListBox.setText( ListBox::className );
		btnCreateListBox.setId( LIST_PUSH );
		btnCreateListBox.create( &toolbar );

		btnCreateComboBox.setText( ComboBox::className );
		btnCreateComboBox.setId( COMBO_PUSH );
		btnCreateComboBox.create( &toolbar );

		btnCreateEditControl.setText( "Edit" );
		btnCreateEditControl.setId( EDIT_PUSH );
		btnCreateEditControl.create( &toolbar );

		btnCreateMemoControl.setText( "Memo" );
		btnCreateMemoControl.setId( MEMO_PUSH );
		btnCreateMemoControl.create( &toolbar );

		btnCreateTrackBar.setText( TrackBar::className );
		btnCreateTrackBar.setId( TRACKBAR_PUSH );
		btnCreateTrackBar.create( &toolbar );

		btnCreateScrollBar.setText( ScrollBar::className );
		btnCreateScrollBar.setId( SCROLLBAR_PUSH );
		btnCreateScrollBar.create( &toolbar );

		btnCreateUpDownButton.setText( UpDownButton::className );
		btnCreateUpDownButton.setId( UPDPOWN_PUSH );
		btnCreateUpDownButton.create( &toolbar );

		btnCreateTreeView.setText( "TreeView." );
		btnCreateTreeView.setId( TREEVIEW_PUSH );
		btnCreateTreeView.create( &toolbar );

		btnCreateXmlViewer.setText( "XmlViewer." );
		btnCreateXmlViewer.setId( XMLVIEW_PUSH );
		btnCreateXmlViewer.create( &toolbar );

		btnCreateGridViewer.setText( "GridViewer." );
		btnCreateGridViewer.setId( GRIDVIEW_PUSH );
		btnCreateGridViewer.create( &toolbar );

		btnCreateFrame.setText( "Frame" );
		btnCreateFrame.setId( FRAME_PUSH );
		btnCreateFrame.create( &toolbar );

		btnCreateScroller.setText( "Scroller" );
		btnCreateScroller.setId( SCROLLER_PUSH );
		btnCreateScroller.create( &toolbar );

		mainArea.create( this );
		mainArea.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_CLIENT )->setGrowHeight( 1 )->setGrowWidth( 1 ) );
		mainArea.setLayoutManager( new AttachmentManager );

		topResourceSelect.setId( RESOURCE_SELECT );
		topResourceSelect.resize( 200, 10 );
		topResourceSelect.create( &mainArea );
		topResourceSelect.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_LEFT )->setGrowHeight( 1 )  );

		childSelect.setId( ITEM_SELECT );
		childSelect.enableExtendedSelection();
		childSelect.resize( 200, 10 );
		childSelect.create( &mainArea );
		childSelect.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_LEFT )->setGrowHeight( 1 ) );
		catchControl( &childSelect );

		treeSelect.setId( TREE_SELECT );
		treeSelect.hide();
		treeSelect.resize( 200, 10 );
		treeSelect.enableDragDrop();
		treeSelect.create( &mainArea );
		treeSelect.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_LEFT )->setGrowHeight( 1 ) );
		catchControl( &treeSelect );

		if( (error=propertiesScroller.create( &mainArea )) == scSUCCESS )
		{
			propertiesScroller.setLayoutData( (new LayoutData)->setAttachment( LayoutData::ATTACH_CLIENT )->setGrowHeight( 1 )->setGrowWidth( 1 ) );
		}
		else
		{
			messageBox( "Unable to create scroller frame", "Error", MB_ICONERROR );
		}

		if( (error=properties.create( &propertiesScroller )) == scSUCCESS )
		{
			properties.disable();

			doLayout();
			newDocument();
		}
		else
		{
			messageBox( "Unable to create properties frame", "Error", MB_ICONERROR );
		}
	}
	else
	{
		messageBox( "Unable to create main window", "Error", MB_ICONERROR );
	}

	return error;
}

int GuiBuilderWindow::checkDefaultIdentifier( const STRING &name )
{
	for( size_t i=0; i<arraySize(defaultIdentifiers); i++ )
	{
		if( name == defaultIdentifiers[i].name )
			return defaultIdentifiers[i].value;
	}

	return 0;
}

void GuiBuilderWindow::loadChildProperties( BasicWindow *child )
{
	m_loading = true;

	xml::Element	*resource = child->getResource();
	xml::Element	*layoutResource = resource->getElement( LayoutManager::className );

	properties.controlName->setText( resource->getAttribute( NAME_ATTR ) );
	if( layoutResource )
	{
		properties.layoutManager->selectEntry( layoutResource->getAttribute( TYPE_ATTR ) );

		properties.marginLEFT->setText( layoutResource->getAttribute( LayoutManager::marginLeft ) );
		properties.marginRIGHT->setText( layoutResource->getAttribute( LayoutManager::marginRight ) );
		properties.marginTOP->setText( layoutResource->getAttribute( LayoutManager::marginTop ) );
		properties.marginBOTTOM->setText( layoutResource->getAttribute( LayoutManager::marginBottom ) );
	}
	else
	{
		properties.layoutManager->selectEntry( 0 );

		properties.marginLEFT->setText( "" );
		properties.marginRIGHT->setText( "" );
		properties.marginTOP->setText( "" );
		properties.marginBOTTOM->setText( "" );
	}

	properties.xPOS->setText( resource->getAttribute( LayoutData::xPosAttr ) );
	properties.yPOS->setText( resource->getAttribute( LayoutData::yPosAttr ) );

	properties.childWidth->setText( resource->getAttribute( LayoutData::widthAttr ) );
	properties.childHeight->setText( resource->getAttribute( LayoutData::heightAttr ) );

	properties.childCaption->setText( resource->getAttribute( CAPTION_ATTR ) );

	if( resource->getTag() == FORM_TAG )
	{
		STRING	baseClass = resource->getAttribute( "baseClass" );
		properties.baseClass->selectEntry( baseClass );
	}
	LayoutData	*layoutData = child->getLayoutData();
	if( layoutData )
	{
		int xPos = layoutData->xPos;
		properties.layout_xPOS->setText( formatNumber( xPos ) );
		int yPos = layoutData->yPos;
		properties.layout_yPOS->setText( formatNumber( yPos ) );

		int width = layoutData->width;
		properties.layoutWIDTH->setText( formatNumber( width ) );
		int height = layoutData->height;
		properties.layoutHEIGHT->setText( formatNumber( height ) );

		int growWidth = layoutData->growWidth;
		properties.growWIDTH->setText( formatNumber( growWidth ) );
		int growHeight = layoutData->growHeight;
		properties.growHEIGHT->setText( formatNumber( growHeight ) );

		int paddingLeft = layoutData->padding.left;
		properties.paddingLEFT->setText( formatNumber( paddingLeft ) );

		int paddingRight = layoutData->padding.right;
		properties.paddingRIGHT->setText( formatNumber( paddingRight ) );

		int paddingTop = layoutData->padding.top;
		properties.paddingTOP->setText( formatNumber( paddingTop ) );

		int paddingBottom = layoutData->padding.bottom;
		properties.paddingBOTTOM->setText( formatNumber( paddingBottom ) );

		properties.attachment->selectEntry( layoutData->attach+1 );
	}
	else
	{
		properties.layout_xPOS->setText( "" );
		properties.layout_yPOS->setText( "" );
		properties.layoutWIDTH->setText( "" );
		properties.layoutHEIGHT->setText( "" );
		properties.growWIDTH->setText( "" );
		properties.growHEIGHT->setText( "" );

		properties.paddingLEFT->setText( "" );
		properties.paddingRIGHT->setText( "" );
		properties.paddingTOP->setText( "" );
		properties.paddingBOTTOM->setText( "" );

		properties.attachment->selectEntry( 0 );
	}

	properties.BackgroundColor->selectEntry( getColorValue( resource->getAttribute( "bgColor" ) ) );
	m_currentStyle = resource->getAttribute( STYLE_ATTR ).getValueN<unsigned>();

	STRING font = resource->getAttribute("font");
	if( font.isEmpty() )
	{
		properties.SelectFontCheck->clrActive();
	}
	else
	{
		properties.SelectFontCheck->setActive();
	}

	properties.numCols->setText( resource->getAttribute( "numCols" ) );
	properties.numRows->setText( resource->getAttribute( "numRows" ) );

	m_loading = false;
	enableDisaleProperties();
}

void GuiBuilderWindow::removeSelected( bool withDesigner )
{
	assert( designerForm );

	childSelect.clearEntries();
	m_childResources.clear();
	treeSelect.clear();
	if( !withDesigner )
	{
		fillChildItemList( designerForm->getResource(), NULL );
	}
	else
	{
		topResourceSelect.clearEntries();
		m_topResources.clear();
		loadGUI( m_guiDoc );
	}

	setChangedFlag();
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

