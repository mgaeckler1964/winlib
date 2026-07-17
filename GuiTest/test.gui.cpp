/*
	this file was created from C:\CRESD\Source\WINLIB\GuiTest\test.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\WINLIB\GuiTest\test.gui.h"

namespace winlibGUI {

	gak::xml::Document *GuiApplication::getGuiDoc() {
		gak::xml::Document *doc = winlib::Application::getGuiDoc();
		if(!doc) {
			gak::STRING xmlSrc = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n"
"<gui>\n"
"<forms>\n"
"<form name=\"TestFORM\" style=\"281018368\" width=\"640\" height=\"480\" caption=\"Test\" id=\"128\">\n"
"<layoutManager type=\"SingleChildManager\" />\n"
"<child type=\"TabControl\" name=\"MyTab\" caption=\"\" x=\"175\" y=\"153\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"132\">\n"
"<items>\n"
"<item>Tab0</item>\n"
"<item>Tab1</item>\n"
"</items>\n"
"</child>\n"
"</form>\n"
"</forms>\n"
"<frames>\n"
"<frame name=\"MyTab_Tab0\" width=\"300\" height=\"300\" id=\"129\" style=\"276824064\">\n"
"<child type=\"Label\" name=\"LABEL\" caption=\"MyTab0\" x=\"28\" y=\"49\" width=\"0\" height=\"0\" style=\"1342242827\" id=\"133\" />\n"
"</frame>\n"
"<frame name=\"MyTab_Tab1\" width=\"300\" height=\"300\" id=\"130\" style=\"276824064\">\n"
"<child type=\"Label\" name=\"LABEL\" caption=\"MyTab1\" x=\"89\" y=\"121\" width=\"80\" height=\"25\" style=\"1342242827\" id=\"133\" />\n"
"</frame>\n"
"<scroller name=\"SCROLLER\" width=\"200\" height=\"200\" id=\"131\" style=\"268435456\">\n"
"<child type=\"Label\" name=\"LABEL\" caption=\"The Scroller\" x=\"0\" y=\"0\" width=\"0\" height=\"0\" style=\"1342242827\" id=\"133\" />\n"
"</scroller>\n"
"</frames>\n"
"</gui>";
			return Application::getGuiDoc( xmlSrc );
		}
		return doc;
	}


	void TestFORM_form::getControls() {
		MyTab=static_cast<winlib::TabControl*>(findChild(MyTab_id));
	}	// TestFORM

	void MyTab_Tab0_frame::getControls() {
		LABEL=static_cast<winlib::Label*>(findChild(LABEL_id));
	}	// MyTab_Tab0

	void MyTab_Tab1_frame::getControls() {
		LABEL=static_cast<winlib::Label*>(findChild(LABEL_id));
	}	// MyTab_Tab1

	void SCROLLER_scroller::getControls() {
		LABEL=static_cast<winlib::Label*>(findChild(LABEL_id));
	}	// SCROLLER
}	// namespace winlibGUI
