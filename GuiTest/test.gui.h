/*
	this file was created from C:\CRESD\Source\WINLIB\GuiTest\test.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/
#ifndef C__CRESD_SOURCE_WINLIB_GUITEST_TEST_GUI_H
#define C__CRESD_SOURCE_WINLIB_GUITEST_TEST_GUI_H
#include <winlib/popup.h>
#include <winlib/frame.h>
#include <winlib/scrollFrame.h>
#include <winlib/ControlW.h>
#include <winlib/xmlEditorChild.h>
#include <winlib/chartWin.h>
#include <winlib/gridView.h>

#include <winlib/winApp.h>

namespace winlibGUI {

	const int COMBOBOX_id=136;
	const int LABEL_id=133;
	const int LABEL1_id=134;
	const int LABEL2_id=135;
	const int MyTab_id=132;
	const int MyTab_Tab0_id=129;
	const int MyTab_Tab1_id=130;
	const int SCROLLER_id=131;
	const int TestFORM_id=128;

	class GuiApplication : public winlib::Application {
		public:
		virtual gak::xml::Document *getGuiDoc();
		GuiApplication(int iconID=-1) : winlib::Application(iconID) {}
	};

	class TestFORM_form : public winlib::PopupWindow {
		public:
		TestFORM_form(winlib::BasicWindow *owner) : PopupWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return PopupWindow::create(parent,"TestFORM");
		}

		winlib::TabControl *MyTab;
		class MyTab_Tab0_frame *MyTab_Tab0;
		class MyTab_Tab1_frame *MyTab_Tab1;
		private:
		virtual void getControls();
	};	// end of TestFORM

	class MyTab_Tab0_frame : public winlib::FrameChild {
		public:
		MyTab_Tab0_frame(winlib::BasicWindow *owner) : FrameChild(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return FrameChild::create(parent,"MyTab_Tab0");
		}

		winlib::Label *LABEL;
		private:
		virtual void getControls();
	};	// end of MyTab_Tab0

	class MyTab_Tab1_frame : public winlib::FrameChild {
		public:
		MyTab_Tab1_frame(winlib::BasicWindow *owner) : FrameChild(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return FrameChild::create(parent,"MyTab_Tab1");
		}

		winlib::Label *LABEL1;
		winlib::Label *LABEL2;
		winlib::ComboBox *COMBOBOX;
		private:
		virtual void getControls();
	};	// end of MyTab_Tab1

	class SCROLLER_scroller : public winlib::ScrollFrame {
		public:
		SCROLLER_scroller(winlib::BasicWindow *owner) : ScrollFrame(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ScrollFrame::create(parent,"SCROLLER");
		}

		winlib::Label *LABEL;
		private:
		virtual void getControls();
	};	// end of SCROLLER
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_WINLIB_GUITEST_TEST_GUI_H
