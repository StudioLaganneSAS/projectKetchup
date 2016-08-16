#ifndef about_XML_H
#define about_XML_H

//
// aboutXML
//

std::string aboutXML = 
"<PKVLayout id=\"ID_ROOT\" layoutx=\"2\" layouty=\"2\" margin=\"0\" spacing=\"0\">"
"    <PKHLayout id=\"ID_HEADER_LAYOUT\" layoutx=\"2\" margin=\"0\" spacing=\"0\">"
"        <PKPanelControl id=\"ID_HEADER_PANEL\" layoutx=\"2\" layouty=\"2\" minimumWidth=\"200\" minimumHeight=\"50\" borderSize=\"0\">"
"            <PKVLayout id=\"ID_HV_LAYOUT\" margin=\"15\" spacing=\"2\">"
"                <PKLabel id=\"ID_HEADER_TITLE\" layoutx=\"2\" text=\"Application Name         \" fontName=\"Arial\" color=\"30,30,30,255\" bold=\"true\" relativeFontSize=\"+10\"/>"
"                <PKLabel id=\"ID_HEADER_SUBTITLE\" layoutx=\"2\" text=\"Version 1.0.0 - Build 252\" fontName=\"Arial\" color=\"100,100,100,255\" relativeFontSize=\"+1\"/>"
"            </PKVLayout>"
"        </PKPanelControl>"
"        <PKImageView id=\"ID_LOGO\" image=\"[[REPLACE]]\"/>"
"    </PKHLayout>"
"    <PKLineControl id=\"ID_LINE\" layoutx=\"2\" color=\"170,170,170,255\"/>"
"    <PKVLayout id=\"ID_V_LAYOUT\" layoutx=\"2\">"
"        <PKLabel id=\"ID_COPYRIGHT\" layoutx=\"2\" text=\"Copyright (c) 2010 - Project Ketchup, Inc.\"/>"
"        <PKHLayout id=\"ID_H_LAYOUT\" layoutx=\"2\" margin=\"0\">"
"            <PKSpacer id=\"ID_SPACER\" layoutx=\"2\"/>"
"            <PKButton id=\"ID_OK\" caption=\"OK\" default=\"true\"/>"
"        </PKHLayout>"
"    </PKVLayout>"
"</PKVLayout>";

#endif // about_XML_H
