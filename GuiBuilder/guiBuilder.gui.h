/*
	this file was created from C:\CRESD\Source\WINLIB\GuiBuilder\guiBuilder.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/
#ifndef C__CRESD_SOURCE_WINLIB_GUIBUILDER_GUIBUILDER_GUI_H
#define C__CRESD_SOURCE_WINLIB_GUIBUILDER_GUIBUILDER_GUI_H
#include <winlib/popup.h>
#include <winlib/frame.h>
#include <winlib/scrollFrame.h>
#include <winlib/ControlW.h>
#include <winlib/xmlEditorChild.h>
#include <winlib/gridView.h>

namespace winlibGUI {

	const int ABOUT_id=128;
	const int attachment_id=228;
	const int attachmentLabel_id=227;
	const int BackgroundColor_id=240;
	const int BackgroundColorLabel_id=239;
	const int baseClass_id=246;
	const int baseClassLabel_id=245;
	const int BottomFrame_id=195;
	const int BUTTON_id=168;
	const int BUTTO_BAR_id=167;
	const int CHECKBOX_id=178;
	const int childCaption_id=198;
	const int childHeight_id=206;
	const int childHeightLabel_id=205;
	const int childWidth_id=204;
	const int childWidthLabel_id=203;
	const int COMBOBOX_id=170;
	const int controlName_id=197;
	const int controlStyle_id=244;
	const int controlStyleLabel_id=243;
	const int EDIT_id=171;
	const int EDIT_TRANSLATE_id=148;
	const int FILE_EXIT_id=146;
	const int FILE_NEW_id=137;
	const int FILE_NEW_FORM_id=138;
	const int FILE_NEW_FRAME_id=139;
	const int FILE_NEW_MENU_id=141;
	const int FILE_NEW_SCROLLER_id=140;
	const int FILE_NEW_STRINGLIST_id=142;
	const int FILE_OPEN_id=143;
	const int FILE_SAVE_id=144;
	const int FILE_SAVE_AS_id=145;
	const int FRAMECHILD_id=174;
	const int GRIDVIEWER_id=191;
	const int growHEIGHT_id=218;
	const int growHEIGHTLabel_id=217;
	const int growWIDTH_id=216;
	const int growWIDTHLabel_id=215;
	const int GuiDefinitions_id=259;
	const int HELLO_WORLD1_id=253;
	const int HELLO_WORLD2_id=254;
	const int HELLO_WORLD3_id=255;
	const int H_SCROLLBAR_id=182;
	const int H_TRACKBAR_id=175;
	const int itemList_id=248;
	const int itemListLabel_id=247;
	const int layoutHEIGHT_id=214;
	const int layoutHEIGHTLabel_id=213;
	const int layoutManager_id=230;
	const int layoutManagerLabel_id=229;
	const int layoutWIDTH_id=212;
	const int layoutWIDTHLabel_id=211;
	const int layout_xPOS_id=208;
	const int layout_xPOSLabel_id=207;
	const int layout_yPOS_id=210;
	const int layout_yPOSLabel_id=209;
	const int LISTBOX_id=169;
	const int MAIN_MENU_id=152;
	const int marginBOTTOM_id=238;
	const int marginBOTTOMLabel_id=237;
	const int marginLEFT_id=232;
	const int marginLEFTLabel_id=231;
	const int marginRIGHT_id=234;
	const int marginRIGHTLabel_id=233;
	const int marginTOP_id=236;
	const int marginTOPLabel_id=235;
	const int MEMOCONTROL_id=180;
	const int MENU_FILE_id=147;
	const int MENU_INFO_id=151;
	const int numCols_id=250;
	const int numColsLABEL_id=249;
	const int numRows_id=252;
	const int numRowsLABEL_id=251;
	const int OPEN_GUI_FILE_id=257;
	const int paddingBOTTOM_id=226;
	const int paddingBOTTOMLabel_id=225;
	const int paddingLEFT_id=220;
	const int paddingLEFTLabel_id=219;
	const int paddingRIGHT_id=222;
	const int paddingRIGHTLabel_id=221;
	const int paddingTOP_id=224;
	const int paddingTOPLabel_id=223;
	const int Properties_id=136;
	const int PUSHBUTTON_id=177;
	const int PUSHBUTTON1_id=155;
	const int PUSHBUTTON2_id=157;
	const int PUSHBUTTON3_id=156;
	const int PUSHBUTTON4_id=158;
	const int PUSHBUTTON5_id=159;
	const int PUSHBUTTONx1_id=161;
	const int PUSHBUTTONx2_id=162;
	const int PUSHBUTTONx3_id=163;
	const int PUSHBUTTONx4_id=164;
	const int PUSHBUTTONx5_id=165;
	const int PUSHBUTTONx6_id=166;
	const int PUSHBUTTON_Bottom_id=185;
	const int PUSHBUTTON_Full_id=189;
	const int PUSHBUTTON_Left_id=188;
	const int PUSHBUTTON_Right_id=187;
	const int PUSHBUTTON_Top_id=186;
	const int RADIOBUTTON_id=179;
	const int SAVE_GUI_FILE_id=258;
	const int SelectFontButton_id=242;
	const int SelectFontCheck_id=241;
	const int StringListEditor_id=134;
	const int stringListGrid_id=192;
	const int StyleDialog_id=130;
	const int StyleListBox_id=184;
	const int SysType_id=154;
	const int targetLanguageCombo_id=193;
	const int TestLabel_id=172;
	const int TEST_ATTACHMENT_id=131;
	const int TEST_FORM_id=129;
	const int TEST_LABEL_id=132;
	const int TEST_MENU_id=153;
	const int TEST_STRING_id=256;
	const int TEST_xmlGridFORM_id=133;
	const int TOOL_BAR_id=160;
	const int TopFrame_id=194;
	const int TranslationForm_id=135;
	const int translationsGrid_id=196;
	const int TREEVIEW_id=176;
	const int UPDOWNBUTTON_id=173;
	const int VIEW_ITEM_LIST_id=149;
	const int VIEW_ITEM_TREE_id=150;
	const int V_SCROLLBAR_id=183;
	const int V_TRACKBAR_id=181;
	const int XMLEDITORCHILD_id=190;
	const int xPOS_id=200;
	const int xPOSLabel_id=199;
	const int yPOS_id=202;
	const int yPOSLabel_id=201;

	class ABOUT_form : public winlib::ModalPopup {
		public:
		ABOUT_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"ABOUT");
		}

		winlib::PushButton *IDOK_PushButton;
		winlib::Label *SysType;
		private:
		virtual void getControls();
	};	// end of ABOUT

	class TEST_FORM_form : public winlib::ModalPopup {
		public:
		TEST_FORM_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"TEST_FORM");
		}

		winlib::PushButton *PUSHBUTTON1;
		winlib::PushButton *PUSHBUTTON3;
		winlib::PushButton *PUSHBUTTON2;
		winlib::PushButton *PUSHBUTTON4;
		winlib::PushButton *PUSHBUTTON5;
		winlib::FrameChild *TOOL_BAR;
		winlib::PushButton *PUSHBUTTONx1;
		winlib::PushButton *PUSHBUTTONx2;
		winlib::PushButton *PUSHBUTTONx3;
		winlib::PushButton *PUSHBUTTONx4;
		winlib::PushButton *PUSHBUTTONx5;
		winlib::PushButton *PUSHBUTTONx6;
		winlib::FrameChild *BUTTO_BAR;
		winlib::PushButton *BUTTON;
		winlib::ListBox *LISTBOX;
		winlib::ComboBox *COMBOBOX;
		winlib::EditControl *EDIT;
		winlib::Label *TestLabel;
		winlib::UpDownButton *UPDOWNBUTTON;
		winlib::FrameChild *FRAMECHILD;
		winlib::TrackBar *H_TRACKBAR;
		winlib::TreeView *TREEVIEW;
		winlib::PushButton *PUSHBUTTON;
		winlib::CheckBox *CHECKBOX;
		winlib::RadioButton *RADIOBUTTON;
		winlib::MemoControl *MEMOCONTROL;
		winlib::TrackBar *V_TRACKBAR;
		winlib::ScrollBar *H_SCROLLBAR;
		winlib::ScrollBar *V_SCROLLBAR;
		private:
		virtual void getControls();
	};	// end of TEST_FORM

	class StyleDialog_form : public winlib::ModalPopup {
		public:
		StyleDialog_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"StyleDialog");
		}

		winlib::ListBox *StyleListBox;
		winlib::PushButton *IDOK_PushButton;
		winlib::PushButton *IDCANCEL_PushButton;
		private:
		virtual void getControls();
	};	// end of StyleDialog

	class TEST_ATTACHMENT_form : public winlib::PopupWindow {
		public:
		TEST_ATTACHMENT_form(winlib::BasicWindow *owner) : PopupWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return PopupWindow::create(parent,"TEST_ATTACHMENT");
		}

		winlib::PushButton *PUSHBUTTON_Bottom;
		winlib::PushButton *PUSHBUTTON_Top;
		winlib::PushButton *PUSHBUTTON_Right;
		winlib::PushButton *PUSHBUTTON_Left;
		winlib::PushButton *PUSHBUTTON_Full;
		private:
		virtual void getControls();
	};	// end of TEST_ATTACHMENT

	class TEST_LABEL_form : public winlib::PopupWindow {
		public:
		TEST_LABEL_form(winlib::BasicWindow *owner) : PopupWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return PopupWindow::create(parent,"TEST_LABEL");
		}

		private:
		virtual void getControls();
	};	// end of TEST_LABEL

	class TEST_xmlGridFORM_form : public winlib::OverlappedWindow {
		public:
		TEST_xmlGridFORM_form(winlib::BasicWindow *owner) : OverlappedWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return OverlappedWindow::create(parent,"TEST_xmlGridFORM");
		}

		winlib::XMLeditorChild *XMLEDITORCHILD;
		winlib::GridViewer *GRIDVIEWER;
		private:
		virtual void getControls();
	};	// end of TEST_xmlGridFORM

	class StringListEditor_form : public winlib::OverlappedWindow {
		public:
		StringListEditor_form(winlib::BasicWindow *owner) : OverlappedWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return OverlappedWindow::create(parent,"StringListEditor");
		}

		winlib::GridViewer *stringListGrid;
		private:
		virtual void getControls();
	};	// end of StringListEditor

	class TranslationForm_form : public winlib::ModalPopup {
		public:
		TranslationForm_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"TranslationForm");
		}

		winlib::ComboBox *targetLanguageCombo;
		winlib::FrameChild *TopFrame;
		winlib::PushButton *IDOK_PushButton;
		winlib::PushButton *IDCANCEL_PushButton;
		winlib::FrameChild *BottomFrame;
		winlib::GridViewer *translationsGrid;
		private:
		virtual void getControls();
	};	// end of TranslationForm

	class Properties_frame : public winlib::FrameChild {
		public:
		Properties_frame(winlib::BasicWindow *owner) : FrameChild(owner) {}
		winlib::SuccessCode create(winlib::CallbackWindow*parent) {
			return FrameChild::create(parent,"Properties");
		}

		winlib::EditControl *controlName;
		winlib::EditControl *childCaption;
		winlib::Label *xPOSLabel;
		winlib::EditControl *xPOS;
		winlib::Label *yPOSLabel;
		winlib::EditControl *yPOS;
		winlib::Label *childWidthLabel;
		winlib::EditControl *childWidth;
		winlib::Label *childHeightLabel;
		winlib::EditControl *childHeight;
		winlib::Label *layout_xPOSLabel;
		winlib::EditControl *layout_xPOS;
		winlib::Label *layout_yPOSLabel;
		winlib::EditControl *layout_yPOS;
		winlib::Label *layoutWIDTHLabel;
		winlib::EditControl *layoutWIDTH;
		winlib::Label *layoutHEIGHTLabel;
		winlib::EditControl *layoutHEIGHT;
		winlib::Label *growWIDTHLabel;
		winlib::EditControl *growWIDTH;
		winlib::Label *growHEIGHTLabel;
		winlib::EditControl *growHEIGHT;
		winlib::Label *paddingLEFTLabel;
		winlib::EditControl *paddingLEFT;
		winlib::Label *paddingRIGHTLabel;
		winlib::EditControl *paddingRIGHT;
		winlib::Label *paddingTOPLabel;
		winlib::EditControl *paddingTOP;
		winlib::Label *paddingBOTTOMLabel;
		winlib::EditControl *paddingBOTTOM;
		winlib::Label *attachmentLabel;
		winlib::ComboBox *attachment;
		winlib::Label *layoutManagerLabel;
		winlib::ComboBox *layoutManager;
		winlib::Label *marginLEFTLabel;
		winlib::EditControl *marginLEFT;
		winlib::Label *marginRIGHTLabel;
		winlib::EditControl *marginRIGHT;
		winlib::Label *marginTOPLabel;
		winlib::EditControl *marginTOP;
		winlib::Label *marginBOTTOMLabel;
		winlib::EditControl *marginBOTTOM;
		winlib::Label *BackgroundColorLabel;
		winlib::ComboBox *BackgroundColor;
		winlib::CheckBox *SelectFontCheck;
		winlib::PushButton *SelectFontButton;
		winlib::Label *controlStyleLabel;
		winlib::PushButton *controlStyle;
		winlib::Label *baseClassLabel;
		winlib::ComboBox *baseClass;
		winlib::Label *itemListLabel;
		winlib::PushButton *itemList;
		winlib::Label *numColsLABEL;
		winlib::EditControl *numCols;
		winlib::Label *numRowsLABEL;
		winlib::EditControl *numRows;
		private:
		virtual void getControls();
	};	// end of Properties
	extern const int TEST_STRINGLIST_ids[];
	static const int TEST_STRINGLIST_count = 4;
	extern const int GuiBuilderStrings_ids[];
	static const int GuiBuilderStrings_count = 2;
	extern const int GuiFilterList_ids[];
	static const int GuiFilterList_count = 1;
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_WINLIB_GUIBUILDER_GUIBUILDER_GUI_H
