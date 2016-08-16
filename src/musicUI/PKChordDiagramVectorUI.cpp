//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKChordDiagramVectorUI.h"
#include "PKVectorEngine.h"
#include "PKStr.h"
#include "PKDialog.h"

//
// PROPERTIES
//

std::string PKChordDiagramVectorUI::BACKCOLOR_COLOR_PROPERTY     = "backColor";
std::string PKChordDiagramVectorUI::FORECOLOR_COLOR_PROPERTY     = "foreColor";
std::string PKChordDiagramVectorUI::MINHEIGTH_UINT32_PROPERTY    = "minHeight"; 
std::string PKChordDiagramVectorUI::ALWAYSSHOWFRET_BOOL_PROPERTY = "alwaysShowFret";
std::string PKChordDiagramVectorUI::SELECTED_BACKCOLOR_COLOR_PROPERTY = "selectedBackColor";
std::string PKChordDiagramVectorUI::SELECTED_FORECOLOR_COLOR_PROPERTY = "selectedForeColor";
std::string PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY            = "selected";


//
// PKChordDiagramVectorUI
//

PKChordDiagramVectorUI::PKChordDiagramVectorUI()
{
    PKColor back  = PKColorCreate(255, 255, 255, 255);
    PKColor fore  = PKColorCreate(  0,   0,   0, 255);
    PKColor sback = PKColorCreate(200, 210, 230, 255);
    PKColor sfore = PKColorCreate(  0,   0,   0, 255);

    PKOBJECT_ADD_COLOR_PROPERTY(backColor, back);
    PKOBJECT_ADD_COLOR_PROPERTY(foreColor, fore);
    PKOBJECT_ADD_UINT32_PROPERTY(minHeight, 0);
	PKOBJECT_ADD_BOOL_PROPERTY(alwaysShowFret, false);
	PKOBJECT_ADD_COLOR_PROPERTY(selectedBackColor, sback);
    PKOBJECT_ADD_COLOR_PROPERTY(selectedForeColor, sfore);
    PKOBJECT_ADD_BOOL_PROPERTY(selected, false);	

    this->diagram  = NULL;
	this->callback = NULL;
    this->hit = false;
}

PKChordDiagramVectorUI::~PKChordDiagramVectorUI()
{
}

void PKChordDiagramVectorUI::build()
{
	PKVectorControl::build();

#ifdef LINUX
    this->setDesiredWidgetSize(this->getMinimumWidth(),
                               this->getMinimumHeight());
#endif	
}

void PKChordDiagramVectorUI::setChordDiagram(PKChordDiagram *d)
{
    this->diagram = d;
}

PKChordDiagram *PKChordDiagramVectorUI::getChordDiagram()
{
	return this->diagram;
}

void PKChordDiagramVectorUI::setUICallback(PKChordDiagramUICallback *call)
{
	this->callback = call;
}

uint32_t PKChordDiagramVectorUI::getMinimumWidth()
{
    PKVariant *m = this->get(PKChordDiagramVectorUI::MINHEIGTH_UINT32_PROPERTY);
    uint32_t min = PKVALUE_UINT32(m)-3;

    if(min != 0)
    {
        int h = min;
        return h +(h/8);
    }

    int h = 60;
    return h +(h/8);
}

uint32_t PKChordDiagramVectorUI::getMinimumHeight()
{
    PKVariant *m = this->get(PKChordDiagramVectorUI::MINHEIGTH_UINT32_PROPERTY);
    uint32_t min = PKVALUE_UINT32(m)-3;

    if(min != 0)
    {
        return min;
    }
    else
    {
        int h = 60;
        return h;
    }
}

void PKChordDiagramVectorUI::drawVector(PKVectorEngine *engine,
		                                int32_t x, int32_t y)
{
	if(this->diagram == NULL || engine == NULL)
    {
        return;
    }

	uint32_t start = this->diagram->getStartingFret(); 
	if(start == 0) start = 1; // Needed to calculations work always

	int numStrings = this->diagram->getNumberOfStrings();
	int numFrets   = this->diagram->getNumberOfFrets();

	if(numStrings < 2 ||
	   numFrets  < 1)
	{
		return;
	}

	int hoff = 2;

    int w = this->getW();
    int h = this->getH()-hoff;

    PKVariant *b  = this->get(PKChordDiagramVectorUI::BACKCOLOR_COLOR_PROPERTY);
    PKColor    bc = PKVALUE_COLOR(b);

    PKVariant *f  = this->get(PKChordDiagramVectorUI::FORECOLOR_COLOR_PROPERTY);
    PKColor    fc = PKVALUE_COLOR(f);

	PKVariant *s  = this->get(PKChordDiagramVectorUI::SELECTED_BOOL_PROPERTY);
    bool selected = PKVALUE_BOOL(s);

	if(selected)
	{
		bc = PKVALUE_COLOR(this->get(PKChordDiagramVectorUI::SELECTED_BACKCOLOR_COLOR_PROPERTY));
		fc = PKVALUE_COLOR(this->get(PKChordDiagramVectorUI::SELECTED_FORECOLOR_COLOR_PROPERTY));
	}

	// Compute sizes

    double thickness = 1 + (h>=70 ? ((double) h) / 200.0 : 0);

    int topMargin    = (int)((double)h / 6 + (double)h/25);
    int bottomMargin = (int)((double)h / 6 - (double)h/25);

    int rightMargin  = (int)((1*((double)h / 6)));
    int leftMargin   = (int)((1.5*((double)h / 6)));

	// Draw background

	engine->fillRect(x-1, y-1, w+1, h+hoff+1, bc);

	if(selected)
	{
		PKColor border;
		border.a = bc.a;
		border.r = (bc.r > 50 ? bc.r - 50 : 0);
		border.g = (bc.g > 50 ? bc.g - 50 : 0);
		border.b = (bc.b > 50 ? bc.b - 50 : 0);	
		
		engine->drawRect(x, y,
					 w,
					 h+hoff,
					 border, 1);
	}

	// Draw foreground

    double lineLength = (double) w - leftMargin - rightMargin;
    double lineHeight = (double) h - topMargin - bottomMargin;

	int stringSize = (int)(lineLength/(numStrings - 1));
    int fretSize   = (int)(lineHeight/numFrets);

	lineLength = stringSize*(numStrings-1);
	lineHeight = fretSize*numFrets;

	engine->drawRect(x+(int)leftMargin, 
					 y+(int)topMargin+hoff,
					 lineLength,
					 lineHeight,
					 fc, thickness);

	for(int i=0; i < numStrings - 2; i++)
	{
		engine->drawLine(x+(int)(leftMargin + stringSize*(i+1)), y+topMargin+hoff,
						 x+(int)(leftMargin + stringSize*(i+1)), y+topMargin+hoff+lineHeight,
						 fc, thickness);
	}

	for(int i=0; i < numFrets - 1; i++)
	{
		engine->drawLine(x+leftMargin, y+(int)(topMargin + fretSize*(i+1))+hoff,
						 x+leftMargin+lineLength, y+(int)(topMargin + fretSize*(i+1))+hoff,
						 fc, thickness);
	}

	// Draw neck if needed

    if(this->diagram->getStartingFret() == 0)
    {
		engine->drawRect(x+(int)leftMargin, y+(int)topMargin - thickness+1,
						 lineLength, 2*thickness,
						 fc, 1);
		engine->fillRect(x+(int)leftMargin, y+(int)topMargin - thickness+1,
						 lineLength, 2*thickness,
						 fc);
	}

	// Draw notes

    for(uint32_t k=0; k < this->diagram->getNumberOfNotes(); k++)
    {
        uint8_t string = this->diagram->getStringForNoteAt(k);
        int8_t  fret   = this->diagram->getFretForNoteAt(k);

        if(fret > 0)
        {
			// Normal note
    		double radius = pk_min(stringSize/2 - thickness, 
	                		fretSize/2 - thickness) - (h < 100 ? 0 : thickness);

			engine->fillCircle(x+leftMargin + stringSize*string,
							   y+topMargin + fretSize*(fret-start) + fretSize/2+hoff,
							   radius, fc);
		}
		else
		{
			if(fret == 0)
			{
				// Open string

                double fontSize = topMargin - h/20 - (h > 70 ? h/20 : 0);
				int wStr = engine->getStringWidth(L"Arial", fontSize, PKV_FONT_STYLE_BOLD, L"O");

				engine->drawString(L"Arial", fontSize, PKV_FONT_STYLE_BOLD,
								   fc, x+leftMargin + stringSize*string - wStr/2,  
								   y+(topMargin - fontSize)/2-2+hoff, L"O");
			}
			else
			{
				// Negative = muted string

				double fontSize = topMargin - h/20 - (h > 70 ? h/20 : 0);
				int wStr = engine->getStringWidth(L"Arial", fontSize, PKV_FONT_STYLE_BOLD, L"X");

				engine->drawString(L"Arial", fontSize, PKV_FONT_STYLE_BOLD,
								   fc, x+leftMargin + stringSize*string - wStr/2, 
								   y+(topMargin - fontSize)/2-2+hoff, L"X");
			}
		}
	}

	// Draw bars

    for(uint32_t k=0; k < this->diagram->getNumberOfBars(); k++)
    {
        PKChordBar bar = this->diagram->getBarAt(k);

        uint8_t s = bar.startString;
        uint8_t e = bar.endString;
        uint8_t  fret = bar.fret;

		double radius = pk_min(stringSize/2 - thickness, 
			            fretSize/2 - thickness) - (h < 100 ? 0 : thickness);

		engine->fillCircle(x+leftMargin + stringSize*s,
						   y+topMargin + fretSize*(fret-start) + fretSize/2+hoff,
						   radius,
						   fc);

		engine->fillCircle(x+leftMargin + stringSize*e,
						   y+topMargin + fretSize*(fret-start) + fretSize/2+hoff,
						   radius,
						   fc);

		engine->fillRect(x+leftMargin + stringSize*s,
						 y+topMargin + fretSize*(fret-start) + fretSize/2 - radius + hoff,
						 stringSize*(e-s), 2*radius, fc);
	}

	// Draw text for starting fret

	PKVariant *a = this->get(PKChordDiagramVectorUI::ALWAYSSHOWFRET_BOOL_PROPERTY);
	bool alwaysShow = PKVALUE_BOOL(a);

    if((this->diagram->getStartingFret() > 0) || alwaysShow)
    {
		double fontSize = topMargin - h/20 - (h > 70 ? h/20 : 0);
        std::wstring t = PKStr::uint32ToWstring(start);
		uint32_t wStr  = engine->getStringWidth(L"Arial", fontSize, PKV_FONT_STYLE_BOLD, t);

		PKColor shade;

		shade.a = 220;
		shade.b = fc.b;
		shade.g = fc.g;
		shade.r = fc.r;

		engine->drawString(L"Arial", fontSize, PKV_FONT_STYLE_BOLD, shade, 
						  x+(leftMargin-wStr)/2-1, y+topMargin+(fretSize-fontSize)/2+hoff, t);
    }

}

// From PKVectorControl

void PKChordDiagramVectorUI::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	this->invalidate();
	PKVectorControl::selfPropertyChanged(prop, oldValue);
}

// From PKCustomControl

void PKChordDiagramVectorUI::mouseDown(PKButtonType button,
			                           int x,
			                           int y,
			                           unsigned int mod)
{
    if(button == PK_BUTTON_LEFT)
    {
        this->hit = true;
    }
}

void PKChordDiagramVectorUI::mouseUp(PKButtonType button,
									 int x,
									 int y,
									 unsigned int mod)
{
    if(button == PK_BUTTON_LEFT)
    {
        if(this->hit)
        {
            if(this->getDialog())
            {
                std::string sid = PKVALUE_STRING(this->get(PKControl::ID_STRING_PROPERTY));
                this->getDialog()->customControlClicked(button, sid, this, "");
            }
        }
    }

    this->hit = false;
}
