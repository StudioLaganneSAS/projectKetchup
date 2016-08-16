#ifndef PKTextInputDialogXML_H
#define PKTextInputDialogXML_H

//
// PKTextInputDialogXML
//

std::string PKTextInputDialogXML = 
"<PKVLayout id=\"ID_ROOT\" layoutx=\"2\" layouty=\"2\" margin=\"0\" spacing=\"0\">"
//"    <PKPanelControl id=\"ID_PANEL\" layoutx=\"2\" layouty=\"2\" minimumWidth=\"150\" minimumHeight=\"40\" borderSize=\"0\">"
"        <PKHLayout id=\"ID_OBJ_1\" layoutx=\"2\" margin=\"10\" spacing=\"15\">"
"            <PKImageView id=\"ID_IMAGE\" image=\"[[REPLACE_IMG]]\"/>"
"            <PKVLayout id=\"ID_OBJ_3\" layoutx=\"2\" margin=\"0\" spacing=\"6\">"
"                <PKLabel id=\"ID_TEXT\" layoutx=\"2\" text=\"[[REPLACE_TEXT]]\" multiline=\"true\" maxWidth=\"250\" />"
"                <PKEdit id=\"ID_EDIT\" layoutx=\"1\" frame=\"0,0,220,0\" text=\"[[REPLACE_EDIT]]\"/>"
"            </PKVLayout>"
"        </PKHLayout>"
//"    </PKPanelControl>"
"    <PKLineControl id=\"ID_LINE\" layoutx=\"2\" color=\"200,200,200,255\"/>"
"    <PKHLayout id=\"ID_OBJ_7\" layoutx=\"2\" margin=\"10\" spacing=\"6\">"
"        <PKSpacer id=\"ID_SPACER\" layoutx=\"2\"/>"
"        <PKButton id=\"ID_BUTTON1\" caption=\"Button2\"/>"
"        <PKButton id=\"ID_BUTTON2\" caption=\"Button3\" default=\"true\"/>"
"    </PKHLayout>"
"</PKVLayout>";

#endif // PKTextInputDialogXML_H
