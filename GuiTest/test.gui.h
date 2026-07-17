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

	const int LABEL_id=132;
	const int MyTab_id=131;
	const int MyTab_Tab1_id=129;
	const int MyTab_Tab2_id=130;
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
		private:
		virtual void getControls();
	};	// end of TestFORM

	class MyTab_Tab1_frame : public winlib::FrameChild {
		public:
		MyTab_Tab1_frame(winlib::BasicWindow *owner) : FrameChild(owner) {}
		winlib::SuccessCode create(winlib::CallbackWindow*parent) {
			return FrameChild::create(parent,"MyTab_Tab1");
		}

		winlib::Label *LABEL;
		private:
		virtual void getControls();
	};	// end of MyTab_Tab1

	class MyTab_Tab2_frame : public winlib::FrameChild {
		public:
		MyTab_Tab2_frame(winlib::BasicWindow *owner) : FrameChild(owner) {}
		winlib::SuccessCode create(winlib::CallbackWindow*parent) {
			return FrameChild::create(parent,"MyTab_Tab2");
		}

		winlib::Label *LABEL;
		private:
		virtual void getControls();
	};	// end of MyTab_Tab2
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_WINLIB_GUITEST_TEST_GUI_H
