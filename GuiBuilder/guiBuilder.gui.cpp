/*
	this file was created from C:\CRESD\Source\WINLIB\GuiBuilder\guiBuilder.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\WINLIB\GuiBuilder\guiBuilder.gui.h"

namespace winlibGUI {

	gak::xml::Document *GuiApplication::getGuiDoc() {
		gak::xml::Document *doc = winlib::Application::getGuiDoc();
		if(!doc) {
			gak::STRING xmlSrc = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n"
"<gui>\n"
"<menus>\n"
"<menu name=\"MAIN_MENU\" id=\"154\">\n"
"<menu name=\"MENU_FILE\" caption=\"&amp;File\" id=\"147\">\n"
"<menu name=\"FILE_NEW\" caption=\"&amp;New Document\" id=\"137\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"New &amp;Item\">\n"
"<menu name=\"FILE_NEW_FORM\" caption=\"&amp;Form\" id=\"138\" />\n"
"<menu caption=\"F&amp;rame\" name=\"FILE_NEW_FRAME\" id=\"139\" />\n"
"<menu caption=\"S&amp;croller\" name=\"FILE_NEW_SCROLLER\" id=\"140\" />\n"
"<menu caption=\"&amp;Menu\" name=\"FILE_NEW_MENU\" id=\"141\" />\n"
"<menu caption=\"&amp;Stringlist\" name=\"FILE_NEW_STRINGLIST\" id=\"142\" />\n"
"</menu>\n"
"<menu caption=\"-\" />\n"
"<menu name=\"FILE_OPEN\" caption=\"&amp;Open...\" id=\"143\" />\n"
"<menu caption=\"Save\" name=\"FILE_SAVE\" id=\"144\" />\n"
"<menu name=\"FILE_SAVE_AS\" caption=\"&amp;Save As...\" id=\"145\" />\n"
"<menu caption=\"-\" />\n"
"<menu name=\"FILE_EXIT\" caption=\"&amp;Exit\" id=\"146\" />\n"
"</menu>\n"
"<menu caption=\"&amp;Edit\">\n"
"<menu caption=\"&amp;Translate...\" name=\"EDIT_TRANSLATE\" id=\"148\" />\n"
"</menu>\n"
"<menu caption=\"&amp;View\">\n"
"<menu caption=\"Item &amp;List\" name=\"VIEW_ITEM_LIST\" id=\"149\" />\n"
"<menu caption=\"Item &amp;Tree\" name=\"VIEW_ITEM_TREE\" id=\"150\" />\n"
"</menu>\n"
"<menu caption=\"Tests\">\n"
"<menu caption=\"Test  Modal Popup\" name=\"MenuTest ModalPopup\" id=\"151\" />\n"
"<menu caption=\"Test Popup\" name=\"MenuTestPopup\" id=\"152\" />\n"
"</menu>\n"
"<menu name=\"MENU_INFO\" caption=\"&amp;Info!\" id=\"153\" />\n"
"</menu>\n"
"<menu name=\"TEST_MENU\" id=\"155\">\n"
"<menu caption=\"New Menu1\" />\n"
"<menu caption=\"New Menu2\" />\n"
"<menu caption=\"New Menu3\">\n"
"<menu caption=\"New Menu 4\" />\n"
"</menu>\n"
"</menu>\n"
"</menus>\n"
"<forms>\n"
"<form name=\"ABOUT\" id=\"128\" width=\"330\" height=\"185\" caption=\"About GUI Builder\" baseClass=\"ModalPopup\" style=\"2428502016\">\n"
"<child type=\"Label\" Caption=\"MG GUI Builder\" x=\"8\" y=\"9\" height=\"0\" width=\"0\" style=\"268435456\" />\n"
"<child type=\"PushButton\" Caption=\"OK\" name=\"IDOK\" x=\"96\" y=\"112\" height=\"20\" width=\"80\" id=\"1\" style=\"268500993\" />\n"
"<child type=\"Label\" name=\"\" caption=\"(c) 2017-2024 by Martin G\xE4""ckler all rights reserved.\" x=\"8\" y=\"83\" width=\"308\" height=\"19\" style=\"1342242827\" />\n"
"<child type=\"Label\" name=\"SysType\" caption=\"32/64\" x=\"256\" y=\"8\" width=\"53\" height=\"18\" style=\"1342242818\" id=\"156\" />\n"
"<child type=\"Label\" name=\"GakIcon\" caption=\"#2\" x=\"8\" y=\"32\" width=\"32\" height=\"32\" style=\"1342242819\" id=\"157\" />\n"
"<child type=\"Label\" name=\"GuiIcon\" caption=\"#1\" x=\"272\" y=\"32\" width=\"32\" height=\"32\" style=\"1342242819\" id=\"158\" />\n"
"</form>\n"
"<form name=\"TestModalPoup\" id=\"129\" width=\"1251\" height=\"780\" caption=\"Demonstration\" baseClass=\"ModalPopup\" style=\"2429026304\" bgColor=\"COLOR_APPWORKSPACE\" bg_red=\"0\" bg_green=\"0\" bg_blue=\"0\" font=\"MS Sans Serif;-13;0;0;0;400;255;0;0;0;1;2;1;34\">\n"
"<child type=\"FrameChild\" name=\"TOOL_BAR\" caption=\"\" x=\"162\" y=\"235\" width=\"80\" height=\"33\" style=\"1350565888\" id=\"164\">\n"
"<layout y=\"0\" x=\"1\" width=\"5\" growWidth=\"1\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON1\" caption=\"PushButton1\" x=\"34\" y=\"6\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"159\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON3\" caption=\"PushButton3\" x=\"266\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"160\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON2\" caption=\"PushButton2\" x=\"157\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"161\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON4\" caption=\"PushButton4\" x=\"407\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"162\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON5\" caption=\"PushButton5\" x=\"557\" y=\"7\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"163\" />\n"
"<layoutManager type=\"RowManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"</child>\n"
"<child type=\"FrameChild\" name=\"BUTTO_BAR\" caption=\"\" x=\"759\" y=\"388\" width=\"100\" height=\"25\" style=\"1350565888\" id=\"171\">\n"
"<layout x=\"0\" growHeight=\"1\" height=\"7\" y=\"0\" paddingLeft=\"0\" paddingTop=\"0\" paddingRight=\"0\" paddingBottom=\"0\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx1\" caption=\"PushButton1\" x=\"30\" y=\"39\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"165\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx2\" caption=\"PushButton2\" x=\"29\" y=\"138\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"166\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx3\" caption=\"PushButton3\" x=\"31\" y=\"284\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"167\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx4\" caption=\"PushButton4\" x=\"27\" y=\"381\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"168\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx5\" caption=\"PushButton5\" x=\"31\" y=\"504\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"169\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTONx6\" caption=\"PushButton6\" x=\"34\" y=\"637\" width=\"92\" height=\"25\" style=\"1342242816\" id=\"170\" />\n"
"<layoutManager type=\"ColManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"TableManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"PushButton\" name=\"BUTTON\" caption=\"PushButton\" x=\"63\" y=\"60\" width=\"100\" height=\"50\" id=\"172\">\n"
"<layout x=\"2\" y=\"2\" growWidth=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"ListBox\" name=\"LISTBOX\" caption=\"\" x=\"96\" y=\"32\" width=\"100\" height=\"50\" id=\"173\" style=\"276889601\">\n"
"<layout x=\"1\" growWidth=\"1\" growHeight=\"1\" height=\"3\" y=\"2\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"<items>\n"
"<item>This ist</item>\n"
"<item>a demo</item>\n"
"<item>list box</item>\n"
"<item>with some</item>\n"
"<item>example</item>\n"
"<item>items</item>\n"
"<item>that</item>\n"
"<item>you can</item>\n"
"<item>select</item>\n"
"<item>if you want</item>\n"
"<item>if you don\'t</item>\n"
"<item>forget</item>\n"
"<item>it</item>\n"
"</items>\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"COMBOBOX\" caption=\"\" x=\"93\" y=\"141\" width=\"100\" height=\"50\" id=\"174\">\n"
"<layout x=\"2\" y=\"3\" growWidth=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"EDIT\" caption=\"Edit\" x=\"105\" y=\"230\" width=\"100\" height=\"50\" id=\"175\" style=\"276889600\">\n"
"<layout y=\"4\" x=\"2\" growWidth=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"TestLabel\" caption=\"Label\" x=\"52\" y=\"391\" width=\"100\" height=\"50\" style=\"276824065\" id=\"176\">\n"
"<layout x=\"2\" y=\"1\" width=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Label\" x=\"100\" y=\"0\" width=\"100\" height=\"50\" style=\"276824065\">\n"
"<layout y=\"5\" x=\"2\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"FrameChild\" name=\"FRAMECHILD\" caption=\"\" x=\"96\" y=\"355\" width=\"100\" height=\"50\" id=\"178\" style=\"276824064\">\n"
"<layout x=\"3\" y=\"1\" growWidth=\"3\" growHeight=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Label\" x=\"122\" y=\"16\" width=\"100\" height=\"50\" style=\"276824065\">\n"
"<layout x=\"1\" y=\"0\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Label\" x=\"11\" y=\"14\" width=\"100\" height=\"50\" style=\"276824065\">\n"
"<layout x=\"1\" y=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Label\" x=\"70\" y=\"57\" width=\"100\" height=\"50\" style=\"276824065\">\n"
"<layout x=\"0\" y=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"TableManager\" />\n"
"<child type=\"UpDownButton\" name=\"UPDOWNBUTTON\" caption=\"\" x=\"38\" y=\"24\" width=\"80\" height=\"25\" style=\"1342242880\" id=\"177\">\n"
"<layout x=\"0\" y=\"0\" />\n"
"</child>\n"
"</child>\n"
"<child type=\"TrackBar\" name=\"H_TRACKBAR\" caption=\"\" x=\"112\" y=\"429\" width=\"100\" height=\"50\" id=\"179\" style=\"276893956\">\n"
"<layout x=\"0\" y=\"7\" growWidth=\"1\" height=\"1\" width=\"4\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"TreeView\" name=\"TREEVIEW\" caption=\"\" x=\"514\" y=\"238\" width=\"100\" height=\"50\" style=\"276889600\" id=\"180\" bgColor=\"Select\" bg_red=\"0\" bg_green=\"17\" bg_blue=\"221\">\n"
"<layout x=\"3\" y=\"2\" height=\"3\" growHeight=\"1\" growWidth=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON\" caption=\"PushButton\" x=\"4\" y=\"636\" width=\"100\" height=\"50\" style=\"277053441\" id=\"181\" font=\"Bauhaus 93;-12;0;0;0;700;255;0;0;0;3;2;1;82\">\n"
"<layout x=\"2\" y=\"6\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"CheckBox\" name=\"CHECKBOX\" caption=\"CheckBox\" x=\"155\" y=\"164\" width=\"100\" height=\"50\" style=\"1342242851\" id=\"182\">\n"
"<layout x=\"1\" y=\"6\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"RadioButton\" name=\"RADIOBUTTON\" caption=\"RadioButton\" x=\"204\" y=\"634\" width=\"100\" height=\"50\" style=\"1342242857\" id=\"183\">\n"
"<layout x=\"3\" y=\"6\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"MemoControl\" name=\"MEMOCONTROL\" caption=\"Memo\" x=\"217\" y=\"54\" width=\"100\" height=\"50\" style=\"1353781524\" id=\"184\">\n"
"<layout x=\"1\" y=\"1\" growWidth=\"1\" growHeight=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"TrackBar\" name=\"V_TRACKBAR\" caption=\"\" x=\"612\" y=\"300\" width=\"100\" height=\"50\" style=\"1342242818\" id=\"185\">\n"
"<layout x=\"4\" height=\"6\" growHeight=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" y=\"1\" />\n"
"</child>\n"
"<child type=\"ScrollBar\" name=\"H_SCROLLBAR\" caption=\"\" x=\"338\" y=\"699\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"186\">\n"
"<layout y=\"8\" x=\"0\" width=\"5\" growWidth=\"1\" />\n"
"</child>\n"
"<child type=\"ScrollBar\" name=\"V_SCROLLBAR\" caption=\"\" x=\"829\" y=\"603\" width=\"25\" height=\"25\" style=\"1342242823\" id=\"187\">\n"
"<layout x=\"5\" growHeight=\"1\" height=\"7\" y=\"1\" />\n"
"</child>\n"
"</form>\n"
"<form name=\"StyleDialog\" id=\"130\" width=\"410\" height=\"355\" caption=\"Control Styles\" baseClass=\"ModalPopup\" style=\"2425159680\">\n"
"<child type=\"Label\" caption=\"Select Styles for Selected Controls\" x=\"9\" y=\"10\" width=\"0\" height=\"0\" style=\"268435456\" name=\"\">\n"
"<layout x=\"0\" y=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"ListBox\" name=\"StyleListBox\" caption=\"\" x=\"20\" y=\"50\" width=\"500\" height=\"200\" id=\"188\" style=\"280035337\">\n"
"<layout y=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" growWidth=\"1\" width=\"2\" growHeight=\"1\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"Ok\" x=\"50\" y=\"313\" width=\"80\" height=\"25\" id=\"1\" style=\"268500993\">\n"
"<layout y=\"2\" growWidth=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"Cancel\" x=\"181\" y=\"321\" width=\"80\" height=\"25\" id=\"2\" style=\"268500992\">\n"
"<layout y=\"2\" x=\"1\" growWidth=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"TableManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"</form>\n"
"<form name=\"TEST_ATTACHMENT\" width=\"1331\" height=\"878\" id=\"131\">\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON_Bottom\" caption=\"Bottom\" x=\"289\" y=\"285\" width=\"80\" height=\"80\" style=\"1342242816\" id=\"189\">\n"
"<layout attachment=\"3\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON_Top\" caption=\"Top\" x=\"360\" y=\"155\" width=\"80\" height=\"80\" style=\"1342242816\" id=\"190\">\n"
"<layout attachment=\"1\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"AttachmentManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON_Right\" caption=\"Right\" x=\"459\" y=\"322\" width=\"80\" height=\"80\" style=\"1342242816\" id=\"191\">\n"
"<layout attachment=\"2\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON_Left\" caption=\"Left\" x=\"158\" y=\"170\" width=\"80\" height=\"80\" style=\"1342242816\" id=\"192\">\n"
"<layout attachment=\"0\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"PUSHBUTTON_Full\" caption=\"Full\" x=\"307\" y=\"279\" width=\"80\" height=\"80\" style=\"1342242816\" id=\"193\">\n"
"<layout attachment=\"4\" paddingLeft=\"4\" paddingTop=\"4\" paddingRight=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"</form>\n"
"<form name=\"TestPopup\" width=\"971\" height=\"250\" id=\"132\" baseClass=\"PopupWindow\" style=\"281542656\">\n"
"<child type=\"Label\" name=\"\" caption=\"Label Thequick Brown III\" x=\"21\" y=\"37\" width=\"0\" height=\"0\" style=\"1342242827\" font=\"Arimo;-96;0;0;0;700;255;0;0;0;3;2;1;34\" />\n"
"<child type=\"DateTimePicker\" name=\"DATETIMEPICKER\" caption=\"\" x=\"352\" y=\"165\" width=\"206\" height=\"27\" style=\"1342242820\" id=\"194\" />\n"
"<child type=\"ChartChild\" name=\"CHARTCHILD\" caption=\"\" x=\"608\" y=\"168\" width=\"167\" height=\"36\" style=\"1342177280\" id=\"195\" />\n"
"</form>\n"
"<form name=\"TEST_xmlGridFORM\" width=\"346\" height=\"361\" caption=\"xmlFORM\" baseClass=\"OverlappedWindow\" id=\"133\">\n"
"<child type=\"XMLeditorChild\" name=\"XMLEDITORCHILD\" caption=\"\" x=\"14\" y=\"27\" width=\"80\" height=\"25\" style=\"1342177280\" id=\"196\" />\n"
"<child type=\"GridViewer\" name=\"GRIDVIEWER\" caption=\"\" x=\"157\" y=\"182\" width=\"80\" height=\"25\" style=\"1342177280\" id=\"197\" />\n"
"</form>\n"
"<form name=\"StringListEditor\" width=\"391\" height=\"339\" caption=\"String List Editor\" baseClass=\"OverlappedWindow\" id=\"134\" style=\"277807104\">\n"
"<layoutManager type=\"SingleChildManager\" />\n"
"<child type=\"GridViewer\" name=\"stringListGrid\" caption=\"\" x=\"0\" y=\"0\" width=\"80\" height=\"25\" style=\"1342177323\" id=\"198\" numCols=\"0\" numRows=\"0\" />\n"
"</form>\n"
"<form name=\"TranslationForm\" width=\"485\" height=\"452\" caption=\"Translate\" id=\"135\" baseClass=\"ModalPopup\" style=\"269287424\">\n"
"<child type=\"FrameChild\" name=\"TopFrame\" caption=\"\" x=\"508\" y=\"54\" width=\"80\" height=\"33\" style=\"1342308352\" id=\"200\">\n"
"<layout attachment=\"1\" growWidth=\"1\" />\n"
"<layoutManager type=\"TableManager\" marginLeft=\"0\" marginRight=\"0\" marginTop=\"0\" marginBottom=\"0\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Target Language:\" x=\"330\" y=\"27\" width=\"0\" height=\"25\" style=\"1342177291\">\n"
"<layout x=\"0\" y=\"0\" width=\"1\" height=\"1\" paddingLeft=\"8\" paddingRight=\"8\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"targetLanguageCombo\" caption=\"\" x=\"351\" y=\"37\" width=\"80\" height=\"100\" style=\"1342243842\" id=\"199\">\n"
"<layout x=\"1\" y=\"0\" width=\"1\" height=\"1\" growWidth=\"1\" growHeight=\"0\" paddingLeft=\"8\" paddingRight=\"8\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"</child>\n"
"<layoutManager type=\"AttachmentManager\" />\n"
"<child type=\"FrameChild\" name=\"BottomFrame\" caption=\"\" x=\"51\" y=\"381\" width=\"80\" height=\"35\" style=\"1342308352\" id=\"201\">\n"
"<layout attachment=\"3\" growWidth=\"1\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"1\" y=\"5\" width=\"80\" height=\"25\" style=\"1342242817\" id=\"1\" font=\"\">\n"
"<layout x=\"0\" y=\"0\" width=\"1\" height=\"1\" growWidth=\"0\" growHeight=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"&amp;Cancel\" x=\"93\" y=\"8\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"2\">\n"
"<layout x=\"1\" y=\"0\" width=\"1\" height=\"1\" growWidth=\"0\" growHeight=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"TableManager\" />\n"
"</child>\n"
"<child type=\"GridViewer\" name=\"translationsGrid\" caption=\"\" x=\"279\" y=\"222\" width=\"80\" height=\"25\" style=\"1342177283\" numCols=\"2\" numRows=\"2\" id=\"202\">\n"
"<layout attachment=\"4\" growWidth=\"1\" growHeight=\"1\" />\n"
"</child>\n"
"</form>\n"
"</forms>\n"
"<frames>\n"
"<frame name=\"Properties\" id=\"136\" width=\"567\" height=\"898\" caption=\"\" style=\"276889600\">\n"
"<layoutManager type=\"TableManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"Label\" Caption=\"Name\" height=\"20\" style=\"268435467\">\n"
"<layout x=\"0\" y=\"0\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"controlName\" height=\"23\" id=\"203\" style=\"276889984\">\n"
"<layout x=\"1\" y=\"0\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Caption\" height=\"20\" style=\"268435467\">\n"
"<layout x=\"0\" y=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"childCaption\" height=\"23\" id=\"204\" style=\"276889984\">\n"
"<layout x=\"1\" y=\"1\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"x\" height=\"20\" style=\"268435467\" name=\"xPOSLabel\" id=\"205\">\n"
"<layout x=\"0\" y=\"2\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"xPOS\" height=\"23\" id=\"206\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"2\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"y\" height=\"20\" style=\"268435467\" name=\"yPOSLabel\" id=\"207\">\n"
"<layout x=\"0\" y=\"3\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"yPOS\" height=\"23\" id=\"208\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"3\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"width\" height=\"20\" style=\"268435467\" name=\"childWidthLabel\" id=\"209\">\n"
"<layout x=\"0\" y=\"4\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"childWidth\" height=\"23\" id=\"210\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"4\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"height\" height=\"20\" style=\"268435467\" name=\"childHeightLabel\" id=\"211\">\n"
"<layout x=\"0\" y=\"5\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"childHeight\" height=\"23\" id=\"212\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"5\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Layout x\" height=\"20\" style=\"268435467\" name=\"layout_xPOSLabel\" id=\"213\">\n"
"<layout x=\"0\" y=\"6\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"layout_xPOS\" height=\"23\" id=\"214\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"6\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Layout y\" height=\"20\" style=\"268435467\" name=\"layout_yPOSLabel\" id=\"215\">\n"
"<layout x=\"0\" y=\"7\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"layout_yPOS\" height=\"23\" id=\"216\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"7\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Layout Width\" height=\"20\" style=\"268435467\" name=\"layoutWIDTHLabel\" id=\"217\">\n"
"<layout x=\"0\" y=\"8\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"layoutWIDTH\" height=\"23\" id=\"218\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"8\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Layout Height\" height=\"20\" style=\"268435467\" name=\"layoutHEIGHTLabel\" id=\"219\">\n"
"<layout x=\"0\" y=\"9\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"layoutHEIGHT\" height=\"23\" id=\"220\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"9\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Grow Width\" height=\"20\" style=\"268435467\" name=\"growWIDTHLabel\" id=\"221\">\n"
"<layout x=\"0\" y=\"10\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"growWIDTH\" height=\"23\" id=\"222\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"10\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Grow Height\" height=\"20\" style=\"268435467\" name=\"growHEIGHTLabel\" id=\"223\">\n"
"<layout x=\"0\" y=\"11\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"growHEIGHT\" height=\"23\" id=\"224\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"11\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Padding Left\" height=\"20\" style=\"268435467\" name=\"paddingLEFTLabel\" id=\"225\">\n"
"<layout x=\"0\" y=\"12\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"paddingLEFT\" height=\"23\" id=\"226\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"12\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Padding Right\" height=\"20\" style=\"268435467\" name=\"paddingRIGHTLabel\" id=\"227\">\n"
"<layout x=\"0\" y=\"13\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"paddingRIGHT\" height=\"23\" id=\"228\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"13\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Padding Top\" height=\"20\" style=\"268435467\" name=\"paddingTOPLabel\" id=\"229\">\n"
"<layout x=\"0\" y=\"14\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"paddingTOP\" height=\"23\" id=\"230\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"14\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Padding Bottom\" height=\"20\" style=\"268435467\" name=\"paddingBOTTOMLabel\" id=\"231\">\n"
"<layout x=\"0\" y=\"15\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"paddingBOTTOM\" height=\"23\" id=\"232\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"15\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"attachmentLabel\" caption=\"Attachment\" x=\"49\" y=\"684\" width=\"0\" height=\"0\" style=\"1342177291\" id=\"233\">\n"
"<layout y=\"16\" x=\"0\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"attachment\" caption=\"\" x=\"262\" y=\"694\" width=\"80\" height=\"200\" style=\"1342242819\" id=\"234\">\n"
"<layout x=\"1\" y=\"16\" growWidth=\"1\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" />\n"
"<items>\n"
"<item>-</item>\n"
"<item>Left</item>\n"
"<item>Top</item>\n"
"<item>Right</item>\n"
"<item>Bottom</item>\n"
"<item>Full</item>\n"
"</items>\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Layout Manager\" style=\"268435467\" name=\"layoutManagerLabel\" id=\"235\">\n"
"<layout x=\"0\" y=\"17\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"ComboBox\" NAME=\"layoutManager\" height=\"200\" id=\"236\" style=\"268500995\" x=\"121\" y=\"35\" width=\"80\">\n"
"<layout x=\"1\" y=\"17\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"<items>\n"
"<item>None</item>\n"
"<item>TableManager</item>\n"
"<item>RowManager</item>\n"
"<item>ColManager</item>\n"
"<item>SingleChildManager</item>\n"
"<item>AttachmentManager</item>\n"
"</items>\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Margin Left\" height=\"20\" style=\"268435467\" name=\"marginLEFTLabel\" id=\"237\">\n"
"<layout x=\"0\" y=\"18\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"marginLEFT\" height=\"23\" id=\"238\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"18\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Margin Right\" height=\"20\" style=\"268435467\" name=\"marginRIGHTLabel\" id=\"239\">\n"
"<layout x=\"0\" y=\"19\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"marginRIGHT\" height=\"23\" id=\"240\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"19\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Margin Top\" height=\"20\" style=\"268435467\" name=\"marginTOPLabel\" id=\"241\">\n"
"<layout x=\"0\" y=\"20\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"marginTOP\" height=\"23\" id=\"242\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"20\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Margin Bottom\" height=\"20\" style=\"268435467\" name=\"marginBOTTOMLabel\" id=\"243\">\n"
"<layout x=\"0\" y=\"21\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"EditControl\" NAME=\"marginBOTTOM\" height=\"23\" id=\"244\" style=\"276889600\">\n"
"<layout x=\"1\" y=\"21\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"BackgroundColorLabel\" caption=\"Background Color\" x=\"0\" y=\"0\" width=\"0\" height=\"0\" style=\"1342177291\" id=\"245\">\n"
"<layout y=\"22\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" x=\"0\" />\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"BackgroundColor\" caption=\"\" x=\"201\" y=\"679\" width=\"80\" height=\"200\" style=\"1342242819\" id=\"246\">\n"
"<layout x=\"1\" y=\"22\" growWidth=\"1\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" />\n"
"<items>\n"
"<item>Default</item>\n"
"<item>White</item>\n"
"<item>Light Gray</item>\n"
"<item>Gray</item>\n"
"<item>Dark Gray</item>\n"
"<item>Black</item>\n"
"<item>Transprarent</item>\n"
"<item>COLOR_SCROLLBAR</item>\n"
"<item>COLOR_BACKGROUND</item>\n"
"<item>COLOR_ACTIVECAPTION</item>\n"
"<item>COLOR_INACTIVECAPTION</item>\n"
"<item>COLOR_MENU</item>\n"
"<item>COLOR_WINDOW</item>\n"
"<item>COLOR_WINDOWFRAME</item>\n"
"<item>COLOR_MENUTEXT</item>\n"
"<item>COLOR_WINDOWTEXT</item>\n"
"<item>COLOR_CAPTIONTEXT</item>\n"
"<item>COLOR_ACTIVEBORDER</item>\n"
"<item>COLOR_INACTIVEBORDER</item>\n"
"<item>COLOR_APPWORKSPACE</item>\n"
"<item>COLOR_HIGHLIGHT</item>\n"
"<item>COLOR_HIGHLIGHTTEXT</item>\n"
"<item>COLOR_BTNFACE</item>\n"
"<item>COLOR_BTNSHADOW</item>\n"
"<item>COLOR_GRAYTEXT</item>\n"
"<item>COLOR_BTNTEXT</item>\n"
"<item>COLOR_INACTIVECAPTIONTEXT</item>\n"
"<item>COLOR_BTNHIGHLIGHT</item>\n"
"<item>Select</item>\n"
"</items>\n"
"</child>\n"
"<child type=\"CheckBox\" name=\"SelectFontCheck\" caption=\"Font\" x=\"59\" y=\"711\" width=\"0\" height=\"25\" style=\"1342242851\" id=\"247\">\n"
"<layout x=\"0\" y=\"23\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"SelectFontButton\" caption=\"Select Font\" x=\"265\" y=\"685\" width=\"80\" height=\"23\" style=\"1342243584\" id=\"248\">\n"
"<layout x=\"1\" y=\"23\" growWidth=\"1\" paddingLeft=\"2\" paddingTop=\"2\" paddingRight=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"controlStyleLabel\" caption=\"Style\" x=\"0\" y=\"0\" width=\"0\" height=\"0\" style=\"268435467\" id=\"249\">\n"
"<layout y=\"24\" x=\"0\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"controlStyle\" caption=\"Styles\" x=\"365\" y=\"708\" width=\"80\" height=\"23\" id=\"250\" style=\"268501760\">\n"
"<layout x=\"1\" y=\"24\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"baseClassLabel\" caption=\"Base Class\" x=\"0\" y=\"0\" width=\"0\" height=\"0\" style=\"268435467\" id=\"251\">\n"
"<layout y=\"25\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" x=\"0\" />\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"baseClass\" caption=\"\" x=\"251\" y=\"685\" width=\"80\" height=\"200\" id=\"252\" style=\"276889603\">\n"
"<layout y=\"25\" x=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"<items>\n"
"<item>OverlappedWindow</item>\n"
"<item>PopupWindow</item>\n"
"<item>ModalPopup</item>\n"
"<item />\n"
"</items>\n"
"</child>\n"
"<child type=\"Label\" Caption=\"Items\" height=\"0\" style=\"268435467\" name=\"itemListLabel\" id=\"253\">\n"
"<layout x=\"0\" y=\"26\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"PushButton\" NAME=\"itemList\" caption=\"Items\" height=\"23\" id=\"254\" style=\"268501760\" x=\"116\" y=\"514\">\n"
"<layout x=\"1\" y=\"26\" width=\"1\" height=\"1\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"numColsLABEL\" caption=\"Num. Cols\" x=\"35\" y=\"711\" width=\"80\" height=\"25\" style=\"1342242827\" id=\"255\">\n"
"<layout x=\"0\" y=\"27\" width=\"1\" height=\"1\" />\n"
"</child>\n"
"<layout paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"<child type=\"EditControl\" name=\"numCols\" caption=\"\" x=\"150\" y=\"674\" width=\"80\" height=\"23\" style=\"1350631808\" id=\"256\">\n"
"<layout x=\"1\" y=\"27\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"numRowsLABEL\" caption=\"Num. Rows\" x=\"28\" y=\"719\" width=\"0\" height=\"0\" style=\"1342242827\" id=\"257\">\n"
"<layout x=\"0\" y=\"28\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"numRows\" caption=\"\" x=\"136\" y=\"701\" width=\"80\" height=\"23\" style=\"1350631808\" id=\"258\">\n"
"<layout x=\"1\" y=\"28\" growWidth=\"1\" paddingLeft=\"2\" paddingRight=\"2\" paddingTop=\"2\" paddingBottom=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"upLABEL\" caption=\"Up\" x=\"25\" y=\"796\" width=\"0\" height=\"0\" style=\"1342242827\" id=\"259\">\n"
"<layout x=\"0\" y=\"29\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"upBUTTON\" caption=\"^\" x=\"174\" y=\"809\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"260\">\n"
"<layout x=\"1\" y=\"29\" growWidth=\"1\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"downLABEL\" caption=\"Down\" x=\"26\" y=\"839\" width=\"0\" height=\"0\" style=\"1342242827\" id=\"261\">\n"
"<layout x=\"0\" y=\"30\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"downBUTTON\" caption=\"v\" x=\"152\" y=\"835\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"262\">\n"
"<layout x=\"1\" y=\"30\" growWidth=\"1\" />\n"
"</child>\n"
"</frame>\n"
"</frames>\n"
"<strings>\n"
"<list name=\"TEST_STRINGLIST\" caption=\"\">\n"
"<string name=\"HELLO_WORLD1\" caption=\"Hello World\" id=\"263\" />\n"
"<string name=\"HELLO_WORLD2\" caption=\"Hello 2World\" id=\"264\" />\n"
"<string name=\"HELLO_WORLD3\" caption=\"Hello World\" id=\"265\" />\n"
"<string name=\"TEST_STRING\" caption=\"SO So Das ist es\" id=\"266\" />\n"
"</list>\n"
"<list name=\"GuiBuilderStrings\" caption=\"\">\n"
"<string name=\"OPEN_GUI_FILE\" caption=\"Open a GUI definition\" id=\"267\" />\n"
"<string name=\"SAVE_GUI_FILE\" caption=\"Save a GUI definition\" id=\"268\" />\n"
"</list>\n"
"<list name=\"GuiFilterList\">\n"
"<string name=\"GuiDefinitions\" caption=\"Gui definitions (*.gui)|*.gui|\" id=\"269\" />\n"
"</list>\n"
"<list name=\"ToolHelpStrings\">\n"
"<string name=\"upBUTTON\" caption=\"Create Sooner\" id=\"260\" />\n"
"<string name=\"downBUTTON\" caption=\"Create Later\" id=\"262\" />\n"
"</list>\n"
"</strings>\n"
"</gui>";
			return Application::getGuiDoc( xmlSrc );
		}
		return doc;
	}


	void ABOUT_form::getControls() {
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		SysType=static_cast<winlib::Label*>(findChild(SysType_id));
		GakIcon=static_cast<winlib::Label*>(findChild(GakIcon_id));
		GuiIcon=static_cast<winlib::Label*>(findChild(GuiIcon_id));
	}	// ABOUT

	void TestModalPoup_form::getControls() {
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
	}	// TestModalPoup

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

	void TestPopup_form::getControls() {
		DATETIMEPICKER=static_cast<winlib::DateTimePicker*>(findChild(DATETIMEPICKER_id));
		CHARTCHILD=static_cast<winlib::ChartChild*>(findChild(CHARTCHILD_id));
	}	// TestPopup

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
		upLABEL=static_cast<winlib::Label*>(findChild(upLABEL_id));
		upBUTTON=static_cast<winlib::PushButton*>(findChild(upBUTTON_id));
		downLABEL=static_cast<winlib::Label*>(findChild(downLABEL_id));
		downBUTTON=static_cast<winlib::PushButton*>(findChild(downBUTTON_id));
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
	const int ToolHelpStrings_ids[] = {
		upBUTTON_id,
		downBUTTON_id,
	};
}	// namespace winlibGUI
