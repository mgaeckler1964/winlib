/*
		Project:		GUI Builder
		Module:			GuiBuilderWindow.h
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

#ifndef GUI_BUILDER_WINDOW_H
#define GUI_BUILDER_WINDOW_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/numericString.h>

#include <WINLIB/POPUP.H>
#include <WINLIB/frame.h>
#include <WINLIB/scrollFrame.h>
#include <WINLIB/CONTROLW.H>
#include <WINLIB/STREDIT.H>
#include <WINLIB/STDDLG.H>

#include "guiBuilder.gui.h"
#include "guiBuilder_rc.h"

#include "DesignerForm.h"
#include "StringListEditor.h"
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

using namespace winlib;
using namespace winlibGUI;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

enum SelectionSource { FromDesignerWindow, FromChildSelect, FromTreeSelect };

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class GuiBuilderWindow : public OverlappedWindow
{
	static const STRING WINDOW_TITLE;

	static const char TRANSLATIONS[];

	static const char LANGUAGE_TAG[];
	static const char LANG_ATTRIBUTE[];
	static const char TRANSLATION_ATTRIBUTE[];

	class MySelectButton : public RadioButton
	{
		public:
		MySelectButton()
		{
			addStyle( BS_PUSHLIKE|BS_ICON );
		}
	};

	typedef PairMap<CI_STRING, int>	IdentifiersMap;

	Dictionarys			m_dictionarys;
	xml::Document		*m_guiDoc;
	F_STRING			docFileName;
	bool				changedFlag;

	FrameChild			toolbar, mainArea;
	ScrollFrame			propertiesScroller;

	MySelectButton		btnSelectControl, btnCreateLabel, 
						btnCreatePushButton, btnCreateCheckBox, btnCreateRadioButton, btnCreateGroupBox, 
						btnCreateListBox, btnCreateComboBox, 
						btnCreateEditControl, btnCreateMemoControl, 
						btnCreateTrackBar, btnCreateScrollBar, btnCreateUpDownButton,
						btnCreateTreeView, btnCreateXmlViewer, btnCreateGridViewer, 
						btnCreateFrame, btnCreateScroller;
	int					currentId;

	ListBox				topResourceSelect;
	xml::XmlArray		topResources;


	enum
	{
		emFORM, emMENU, emSTRINGS
	}					editorMode;

	TreeView			treeSelect;
	ListBox				childSelect;
	xml::XmlArray		childResources;

	Properties_frame	properties;
	DesignerForm		*designerForm;
	StringListEditor	*stringListEditor;

	bool				m_loading;

	int					currentStyle;

	void setDocument( const F_STRING &fileName )
	{
		docFileName = fileName;
	}
	const F_STRING &getDocument( void ) const
	{
		return docFileName;
	}
	void clrChangedFlag( void )
	{
		if( changedFlag )
		{
			setTitle( WINDOW_TITLE );
			changedFlag = false;
		}
	}
	public:
	void setChangedFlag( void )
	{
		if( !changedFlag )
		{
			setTitle( STRING(WINDOW_TITLE) + " *" );
			changedFlag = true;
		}
	}
	bool getChangedFlag( void ) const
	{
		return changedFlag;
	}

	public:
	GuiBuilderWindow() : properties( NULL ), OverlappedWindow( NULL )
	{
		m_loading = false;
		designerForm = NULL;
		stringListEditor = NULL;
		m_guiDoc = NULL;
		changedFlag = false;
		currentId = SELECT_PUSH;
		editorMode = emFORM;
		setTitle( WINDOW_TITLE );
	}
	~GuiBuilderWindow()
	{
		if( m_guiDoc )
		{
			delete m_guiDoc;
		}
	}

	SuccessCode create( void );
	private:
	void deleteGuiDoc();

	public:
	TreeNode *addChildItem( xml::Element *resource )
	{
		return addChildItem( resource, editorMode != emSTRINGS ? treeSelect.findItem( resource->getParent() ) : NULL );
	}

	private:
	TreeNode *addChildItem( xml::Element *resource, TreeNode *parent );
	void fillMenuItemList( xml::Element *resource, TreeNode *parent );
	void fillChildItemList( xml::Element *resource, TreeNode *parent );
	xml::Element *getSelectedChildResource( void ) const
	{
		size_t	selIdx = size_t(childSelect.getSelection());
		if( selIdx < childResources.size() )
			return childResources[selIdx];
		else
			return NULL;
	}
	void deleteDesignerForm( void )
	{
		if( designerForm )
		{
			designerForm->close();
			delete designerForm;
			designerForm = NULL;
		}
		if( stringListEditor )
		{
			stringListEditor->close();
			delete stringListEditor;
			stringListEditor = NULL;
		}
	}
	void renameTopSelected( const STRING &newName )
	{
		int	idx = topResourceSelect.getSelection();
		topResourceSelect.replaceEntry( idx, newName );
		topResourceSelect.selectEntry( idx );
	}
	xml::Element *getSelectedTopResource( void ) const
	{
		size_t	selResource = size_t(topResourceSelect.getSelection());
		if( selResource < topResources.size() )
		{
			return topResources[selResource];
		}
		return NULL;
	}
	void addTopResource( xml::Element *resource, const STRING &name )
	{
		topResourceSelect.addEntry( name );
		topResources += resource;
	}
	void addSelectedTopResource( xml::Element *resource, const STRING &name )
	{
		addTopResource( resource, name );
		topResourceSelect.selectEntry( name );
	}
	void loadResource( void );

	static void enableChild( BasicWindow *child )
	{
		child->enable();
		child->show();
	}
	static void disableChild( BasicWindow *child )
	{
		child->hide();
		child->disable();
	}
	void enableDisaleProperties( void );
	void enableNameProperties( void );
	void enableDisaleMenuProperties( void );
	void enableDisaleStringsProperties( void );
	void enableDisaleFormProperties( void );

	BasicWindow *getChildByResource( xml::Element *resource )
	{
		assert( designerForm );

		return designerForm->findChild( resource );
	}
	BasicWindow *getChildByTreeSelect( void )
	{
		assert( designerForm );

		BasicWindow *child;
		TreeNode	*treeNode = treeSelect.getSelection();
		if( treeNode )
			child = getChildByResource( (xml::Element*)treeNode->getData() );
		else
			child = NULL;

		return child;
	}
	void handleChildSelect( void );
	void handleItemTreeSelect( void );
	void handleStringSelect( void );

	public:
	void removeSelected( bool withDesigner );

	private:
	STRING getUniqueType( void );

	void handleChildNameChange();
	public:
	void changeStringResource( xml::Element *string, const STRING &name, const STRING &value, const BasicWindow *source );
	void changeStringResource( size_t idx, const STRING &name, const STRING &value, const BasicWindow *source );

	private:
	virtual ProcessStatus handleEditChange( int control );
	virtual bool handleTreeViewDrag( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dragOver );
	virtual void handleTreeViewDrop( TreeView *dragTreeView, TreeNode *dragItem, TreeNode *dropTarget );
	virtual ProcessStatus handleSelectionChange( int control );
	virtual ProcessStatus handleButtonClick( int control );
	virtual ProcessStatus handleCommand( int cmd );
	virtual bool canClose( void );
	void handleMenuEditor( WPARAM wParam );
	void handleFormEditor( WPARAM wParam );
	void handleStringEditor( WPARAM wParam );
	virtual void postControlCallback( BasicWindow *control, unsigned uMsg, WPARAM wParam, LPARAM lParam );

	public:
	int getCurrentId( void ) const
	{
		return currentId;
	}

	private:
	int checkDefaultIdentifier( const STRING &name );

	public:
	void selectControl( BasicWindow *child, SelectionSource source )
	{
		if( source == FromTreeSelect )
			childSelect.clearSelection();
		else
			treeSelect.selectItem( treeSelect.findItem( child->getResource() ) );

		if( source != FromChildSelect )
		{
			xml::Element	*resource = child->getResource();
			size_t		pos = childResources.findElement( resource );
			if( pos != childResources.no_index )
			{
				childSelect.selectEntry( int(pos) );
			}
		}
		loadChildProperties( child );
	}
	void unselectControl( BasicWindow *child )
	{
		xml::Element	*resource = child->getResource();
		size_t		pos = childResources.findElement( resource );
		if( pos != -1 )
			childSelect.unselectEntry( int(pos) );
		enableDisaleProperties();
	}
	private:
	void loadMenuProperties( xml::Element *resource )
	{
		m_loading = true;
		properties.controlName->setText( resource->getAttribute( NAME_ATTR ) );
		properties.childCaption->setText( resource->getAttribute( CAPTION_ATTR ) );
		enableDisaleProperties();
		m_loading = false;
	}

	void loadStringProperties( xml::Element *resource )
	{
		m_loading = true;
		properties.controlName->setText( resource->getAttribute( NAME_ATTR ) );
		properties.childCaption->setText( resource->getAttribute( CAPTION_ATTR ) );
		enableDisaleProperties();
		m_loading = false;
	}

	public:
	void loadChildProperties( BasicWindow *child );

	private:
	void setLayoutManager( void )
	{
		if( designerForm )
		{
			STRING	type = properties.layoutManager->getSelectedText();
			designerForm->setLayoutManager( type );
			designerForm->doLayout();
		}
		enableDisaleProperties();
	}
	bool checkLayoutData( void );
	void editItemList( void );
	void newMenu();
	void newStringList();
	void newFrame();
	void newScroller();
	void newForm();

	void collectCaptions( const xml::Element *root, Captions *target ) const;
	void translateCaptions( xml::Element *root, const Dictionary &dict ) const;
	bool checkChangedFlag( void );
	void newDocument();
	void loadTranslations( xml::Document *doc );
	void loadGUI( xml::Document *doc );
	void loadDocument( void );
	IdentifiersMap createIDs( void );
	void saveGui( const F_STRING &fileName );
	void saveHeader( const F_STRING &fileName, const IdentifiersMap &identifiers );
	void saveCpp( const F_STRING &fileName );
	void saveDictionaries( const STRING &fileName );
	void saveDocumentAs( const F_STRING &newName );
	void saveTranslations( const F_STRING &fileName );
	bool saveDocumentAs( void );
	bool saveDocument( void );

	void newStyle( unsigned long newStyle, unsigned long styleMask );
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

#endif //  GUI_BUILDER_WINDOW_H
