#ifndef PKStdDialogXML_H
#define PKStdDialogXML_H

//
// PKStdDialogXML
//

std::string PKStdDialogXML = 
"<PKVLayout id=\"ID_ROOT\" layoutx=\"2\" layouty=\"2\" margin=\"0\" spacing=\"0\">"
"    <PKPanelControl id=\"ID_PANEL\" layoutx=\"2\" layouty=\"2\" minimumWidth=\"150\" minimumHeight=\"40\" borderSize=\"0\">"
#ifdef MACOSX
"        <PKHLayout id=\"ID_OBJ_1\" layoutx=\"2\" margin=\"25\" spacing=\"20\">"
#else
"        <PKHLayout id=\"ID_OBJ_1\" layoutx=\"2\" margin=\"15\" spacing=\"20\">"
#endif
"            <PKImageView id=\"ID_IMAGE\" image=\"[[REPLACE_IMG]]\"/>"
"            <PKVLayout id=\"ID_OBJ_3\" layoutx=\"2\" margin=\"0\" spacing=\"6\">"
#ifdef WIN32
"                <PKLabel id=\"ID_HEADER\" layoutx=\"2\" text=\"[[REPLACE_HEADER]]\" color=\"0,30,120,255\" bold=\"true\" relativeFontSize=\"+2\"/>"
"                <PKLabel id=\"ID_TEXT\" layoutx=\"2\" text=\"[[REPLACE_TEXT]][[REPLACE_TEXT]][[REPLACE_TEXT]]\" multiline=\"true\" maxWidth=\"250\" />"
#endif
#ifdef MACOSX
"                <PKLabel id=\"ID_HEADER\" layoutx=\"2\" text=\"[[REPLACE_HEADER]]\" color=\"0,25,110,255\" bold=\"true\"/>"
"                <PKLabel id=\"ID_TEXT\" layoutx=\"2\" text=\"[[REPLACE_TEXT]]\" multiline=\"true\" maxWidth=\"350\" />"
#endif
#ifdef LINUX
"                <PKLabel id=\"ID_HEADER\" layoutx=\"2\" text=\"[[REPLACE_HEADER]]\" color=\"0,25,110,255\" bold=\"true\"/>"
"                <PKLabel id=\"ID_TEXT\" layoutx=\"2\" text=\"[[REPLACE_TEXT]]\" multiline=\"true\" maxWidth=\"350\" />"
#endif
"            </PKVLayout>"
"        </PKHLayout>"
"    </PKPanelControl>"
"    <PKLineControl id=\"ID_LINE\" layoutx=\"2\" color=\"200,200,200,255\"/>"
"    <PKHLayout id=\"ID_OBJ_7\" layoutx=\"2\" margin=\"10\" spacing=\"6\">"
"        <PKButton id=\"ID_BUTTON1\" caption=\"Button1\"/>"
"        <PKSpacer id=\"ID_SPACER\" layoutx=\"2\"/>"
"        <PKButton id=\"ID_BUTTON2\" caption=\"Button2\"/>"
"        <PKButton id=\"ID_BUTTON3\" caption=\"Button3\" default=\"true\"/>"
"    </PKHLayout>"
"</PKVLayout>";

#endif // PKStdDialogXML_H
