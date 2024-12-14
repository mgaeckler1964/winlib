/*
	this file was created from C:\CRESD\Source\WINLIB\GuiBuilder\guiBuilder.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\WINLIB\GuiBuilder\guiBuilder.gui.h"

namespace winlibGUI {

	void ABOUT_form::getControls() {
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		SysType=static_cast<winlib::Label*>(findChild(SysType_id));
	}	// ABOUT

	void TEST_FORM_form::getControls() {
		PUSHBUTTON1=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON1_id));
		PUSHBUTTON3=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON3_id));
		PUSHBUTTON2=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON2_id));
		PUSHBUTTON4=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON4_id));
		PUSHBUTTON5=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON5_id));
		TOOL_BAR=static_cast<winlib::FrameChild*>(findChild(TOOL_BAR_id));
		PUSHBUTTONx1=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx1_id));
		PUSHBUTTONx2=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx2_id));
		PUSHBUTTONx3=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx3_id));
		PUSHBUTTONx4=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx4_id));
		PUSHBUTTONx5=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx5_id));
		PUSHBUTTONx6=static_cast<winlib::PushButton*>(findChild(PUSHBUTTONx6_id));
		BUTTO_BAR=static_cast<winlib::FrameChild*>(findChild(BUTTO_BAR_id));
		BUTTON=static_cast<winlib::PushButton*>(findChild(BUTTON_id));
		LISTBOX=static_cast<winlib::ListBox*>(findChild(LISTBOX_id));
		COMBOBOX=static_cast<winlib::ComboBox*>(findChild(COMBOBOX_id));
		EDIT=static_cast<winlib::EditControl*>(findChild(EDIT_id));
		TestLabel=static_cast<winlib::Label*>(findChild(TestLabel_id));
		UPDOWNBUTTON=static_cast<winlib::UpDownButton*>(findChild(UPDOWNBUTTON_id));
		FRAMECHILD=static_cast<winlib::FrameChild*>(findChild(FRAMECHILD_id));
		H_TRACKBAR=static_cast<winlib::TrackBar*>(findChild(H_TRACKBAR_id));
		TREEVIEW=static_cast<winlib::TreeView*>(findChild(TREEVIEW_id));
		PUSHBUTTON=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_id));
		CHECKBOX=static_cast<winlib::CheckBox*>(findChild(CHECKBOX_id));
		RADIOBUTTON=static_cast<winlib::RadioButton*>(findChild(RADIOBUTTON_id));
		MEMOCONTROL=static_cast<winlib::MemoControl*>(findChild(MEMOCONTROL_id));
		V_TRACKBAR=static_cast<winlib::TrackBar*>(findChild(V_TRACKBAR_id));
		H_SCROLLBAR=static_cast<winlib::ScrollBar*>(findChild(H_SCROLLBAR_id));
		V_SCROLLBAR=static_cast<winlib::ScrollBar*>(findChild(V_SCROLLBAR_id));
	}	// TEST_FORM

	void StyleDialog_form::getControls() {
		StyleListBox=static_cast<winlib::ListBox*>(findChild(StyleListBox_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		IDCANCEL_PushButton=static_cast<winlib::PushButton*>(findChild(IDCANCEL));
	}	// StyleDialog

	void TEST_ATTACHMENT_form::getControls() {
		PUSHBUTTON_Bottom=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_Bottom_id));
		PUSHBUTTON_Top=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_Top_id));
		PUSHBUTTON_Right=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_Right_id));
		PUSHBUTTON_Left=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_Left_id));
		PUSHBUTTON_Full=static_cast<winlib::PushButton*>(findChild(PUSHBUTTON_Full_id));
	}	// TEST_ATTACHMENT

	void TEST_LABEL_form::getControls() {
	}	// TEST_LABEL

	void TEST_xmlGridFORM_form::getControls() {
		XMLEDITORCHILD=static_cast<winlib::XMLeditorChild*>(findChild(XMLEDITORCHILD_id));
		GRIDVIEWER=static_cast<winlib::GridViewer*>(findChild(GRIDVIEWER_id));
	}	// TEST_xmlGridFORM

	void StringListEditor_form::getControls() {
		stringListGrid=static_cast<winlib::GridViewer*>(findChild(stringListGrid_id));
	}	// StringListEditor

	void TranslationForm_form::getControls() {
		targetLanguageCombo=static_cast<winlib::ComboBox*>(findChild(targetLanguageCombo_id));
		TopFrame=static_cast<winlib::FrameChild*>(findChild(TopFrame_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		IDCANCEL_PushButton=static_cast<winlib::PushButton*>(findChild(IDCANCEL));
		BottomFrame=static_cast<winlib::FrameChild*>(findChild(BottomFrame_id));
		translationsGrid=static_cast<winlib::GridViewer*>(findChild(translationsGrid_id));
	}	// TranslationForm

	void Properties_frame::getControls() {
		controlName=static_cast<winlib::EditControl*>(findChild(controlName_id));
		childCaption=static_cast<winlib::EditControl*>(findChild(childCaption_id));
		xPOSLabel=static_cast<winlib::Label*>(findChild(xPOSLabel_id));
		xPOS=static_cast<winlib::EditControl*>(findChild(xPOS_id));
		yPOSLabel=static_cast<winlib::Label*>(findChild(yPOSLabel_id));
		yPOS=static_cast<winlib::EditControl*>(findChild(yPOS_id));
		childWidthLabel=static_cast<winlib::Label*>(findChild(childWidthLabel_id));
		childWidth=static_cast<winlib::EditControl*>(findChild(childWidth_id));
		childHeightLabel=static_cast<winlib::Label*>(findChild(childHeightLabel_id));
		childHeight=static_cast<winlib::EditControl*>(findChild(childHeight_id));
		layout_xPOSLabel=static_cast<winlib::Label*>(findChild(layout_xPOSLabel_id));
		layout_xPOS=static_cast<winlib::EditControl*>(findChild(layout_xPOS_id));
		layout_yPOSLabel=static_cast<winlib::Label*>(findChild(layout_yPOSLabel_id));
		layout_yPOS=static_cast<winlib::EditControl*>(findChild(layout_yPOS_id));
		layoutWIDTHLabel=static_cast<winlib::Label*>(findChild(layoutWIDTHLabel_id));
		layoutWIDTH=static_cast<winlib::EditControl*>(findChild(layoutWIDTH_id));
		layoutHEIGHTLabel=static_cast<winlib::Label*>(findChild(layoutHEIGHTLabel_id));
		layoutHEIGHT=static_cast<winlib::EditControl*>(findChild(layoutHEIGHT_id));
		growWIDTHLabel=static_cast<winlib::Label*>(findChild(growWIDTHLabel_id));
		growWIDTH=static_cast<winlib::EditControl*>(findChild(growWIDTH_id));
		growHEIGHTLabel=static_cast<winlib::Label*>(findChild(growHEIGHTLabel_id));
		growHEIGHT=static_cast<winlib::EditControl*>(findChild(growHEIGHT_id));
		paddingLEFTLabel=static_cast<winlib::Label*>(findChild(paddingLEFTLabel_id));
		paddingLEFT=static_cast<winlib::EditControl*>(findChild(paddingLEFT_id));
		paddingRIGHTLabel=static_cast<winlib::Label*>(findChild(paddingRIGHTLabel_id));
		paddingRIGHT=static_cast<winlib::EditControl*>(findChild(paddingRIGHT_id));
		paddingTOPLabel=static_cast<winlib::Label*>(findChild(paddingTOPLabel_id));
		paddingTOP=static_cast<winlib::EditControl*>(findChild(paddingTOP_id));
		paddingBOTTOMLabel=static_cast<winlib::Label*>(findChild(paddingBOTTOMLabel_id));
		paddingBOTTOM=static_cast<winlib::EditControl*>(findChild(paddingBOTTOM_id));
		attachmentLabel=static_cast<winlib::Label*>(findChild(attachmentLabel_id));
		attachment=static_cast<winlib::ComboBox*>(findChild(attachment_id));
		layoutManagerLabel=static_cast<winlib::Label*>(findChild(layoutManagerLabel_id));
		layoutManager=static_cast<winlib::ComboBox*>(findChild(layoutManager_id));
		marginLEFTLabel=static_cast<winlib::Label*>(findChild(marginLEFTLabel_id));
		marginLEFT=static_cast<winlib::EditControl*>(findChild(marginLEFT_id));
		marginRIGHTLabel=static_cast<winlib::Label*>(findChild(marginRIGHTLabel_id));
		marginRIGHT=static_cast<winlib::EditControl*>(findChild(marginRIGHT_id));
		marginTOPLabel=static_cast<winlib::Label*>(findChild(marginTOPLabel_id));
		marginTOP=static_cast<winlib::EditControl*>(findChild(marginTOP_id));
		marginBOTTOMLabel=static_cast<winlib::Label*>(findChild(marginBOTTOMLabel_id));
		marginBOTTOM=static_cast<winlib::EditControl*>(findChild(marginBOTTOM_id));
		BackgroundColorLabel=static_cast<winlib::Label*>(findChild(BackgroundColorLabel_id));
		BackgroundColor=static_cast<winlib::ComboBox*>(findChild(BackgroundColor_id));
		SelectFontCheck=static_cast<winlib::CheckBox*>(findChild(SelectFontCheck_id));
		SelectFontButton=static_cast<winlib::PushButton*>(findChild(SelectFontButton_id));
		controlStyleLabel=static_cast<winlib::Label*>(findChild(controlStyleLabel_id));
		controlStyle=static_cast<winlib::PushButton*>(findChild(controlStyle_id));
		baseClassLabel=static_cast<winlib::Label*>(findChild(baseClassLabel_id));
		baseClass=static_cast<winlib::ComboBox*>(findChild(baseClass_id));
		itemListLabel=static_cast<winlib::Label*>(findChild(itemListLabel_id));
		itemList=static_cast<winlib::PushButton*>(findChild(itemList_id));
		numColsLABEL=static_cast<winlib::Label*>(findChild(numColsLABEL_id));
		numCols=static_cast<winlib::EditControl*>(findChild(numCols_id));
		numRowsLABEL=static_cast<winlib::Label*>(findChild(numRowsLABEL_id));
		numRows=static_cast<winlib::EditControl*>(findChild(numRows_id));
	}	// Properties
	const int TEST_STRINGLIST_ids[] = {
		HELLO_WORLD1_id,
		HELLO_WORLD2_id,
		HELLO_WORLD3_id,
		TEST_STRING_id,
	};
	const int GuiBuilderStrings_ids[] = {
		OPEN_GUI_FILE_id,
		SAVE_GUI_FILE_id,
	};
	const int GuiFilterList_ids[] = {
		GuiDefinitions_id,
	};
}	// namespace winlibGUI
