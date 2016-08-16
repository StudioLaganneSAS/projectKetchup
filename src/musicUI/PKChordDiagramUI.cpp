//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKChordDiagramUI.h"
#include "PKBitmapOps.h"
#include "PKFont.h"
#include "PKStr.h"

//
// PROPERTIES
//

std::string PKChordDiagramUI::BACKCOLOR_COLOR_PROPERTY  = "backColor";
std::string PKChordDiagramUI::FORECOLOR_COLOR_PROPERTY  = "foreColor";
std::string PKChordDiagramUI::MINHEIGTH_UINT32_PROPERTY = "minHeight"; 

//
// PKChordDiagramUI
//

PKChordDiagramUI::PKChordDiagramUI()
{
    PKColor back = PKColorCreate(255, 255, 255, 255);
    PKColor fore = PKColorCreate(  0,   0,   0, 255);

    PKOBJECT_ADD_COLOR_PROPERTY(backColor, back);
    PKOBJECT_ADD_COLOR_PROPERTY(foreColor, fore);
    PKOBJECT_ADD_UINT32_PROPERTY(minHeight, 0);

    this->diagram = NULL;
    this->cache   = NULL;
}

PKChordDiagramUI::~PKChordDiagramUI()
{
    if(this->cache != NULL)
    {
        delete this->cache;
    }
}

void PKChordDiagramUI::build()
{
	PKDoubleBufferedControl::build();
}

void PKChordDiagramUI::setChordDiagram(PKChordDiagram *d)
{
    this->diagram = d;
}

uint32_t PKChordDiagramUI::getMinimumWidth()
{
    PKVariant *m = this->get(PKChordDiagramUI::MINHEIGTH_UINT32_PROPERTY);
    uint32_t min = PKVALUE_UINT32(m);

    if(min != 0)
    {
        int h = min;
        return h +(h/8);
    }

    int h = 60;
    return h +(h/8);
}

uint32_t PKChordDiagramUI::getMinimumHeight()
{
    PKVariant *m = this->get(PKChordDiagramUI::MINHEIGTH_UINT32_PROPERTY);
    uint32_t min = PKVALUE_UINT32(m);

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

void PKChordDiagramUI::resize(uint32_t w, 
                  			  uint32_t h)
{
    PKDoubleBufferedControl::resize(w, h);

    if(this->cache != NULL)
    {
        delete this->cache;
        this->cache = NULL;
    }
}

void PKChordDiagramUI::draw32bpp(uint8_t *buffer)
{
    if(this->diagram == NULL)
    {
        return;
    }

    int w = this->getW();
    int h = this->getH();

    if(this->cache == NULL)
    {
        this->cache = new PKImage();

        if(this->cache == NULL)
        {
            return;
        }

        this->cache->create(w, h);

        PKVariant *b  = this->get(PKChordDiagramUI::BACKCOLOR_COLOR_PROPERTY);
        PKColor    bc = PKVALUE_COLOR(b);

        PKVariant *f  = this->get(PKChordDiagramUI::FORECOLOR_COLOR_PROPERTY);
        PKColor    fc = PKVALUE_COLOR(f);

        int thickness = (h >= 200 ? 2 : 1);

        int topMargin    = h / 8 + h/50;
        int bottomMargin = h / 8 - h/50;

        int rightMargin  = (int) (1.5*(h / 8));
        int leftMargin   = (int) (1.5*(h / 8));

        PKFont *font = new PKFont("Arial", topMargin/2+1, true);

        // Fill background
        PKBitmapOps::fillRect(this->cache, bc);

        // Draw the grid

        int lineLength = w-leftMargin-rightMargin;
        int lineHeight = h-topMargin-bottomMargin;

        uint32_t *fullLine = new uint32_t[lineLength+1];
        PKBitmapOps::fillRect(fullLine, lineLength+1, 1, fc);

        uint32_t *gridLine = new uint32_t[lineLength+1];

        int stringSize = lineLength/(this->diagram->getNumberOfStrings() - 1);
        int newLineLength = stringSize*(this->diagram->getNumberOfStrings() - 1);
        int offset = (lineLength - newLineLength) / 2;
        lineLength = newLineLength + 1;

        for(int n = 0; n < lineLength+1; n++)
        {
            uint32_t *dst = (uint32_t *) gridLine + n;

            int modulo = n % stringSize;

            if(thickness == 1)
            {
                if(modulo != 0)
                {
                    PKBitmapOps::setPixel(dst, bc);
                }
                else
                {
                    PKBitmapOps::setPixel(dst, fc);
                }
            }
            else
            {
                if(modulo != 0 && modulo != (stringSize-1) && n!=1)
                {
                    PKBitmapOps::setPixel(dst, bc);
                }
                else
                {
                    PKBitmapOps::setPixel(dst, fc);
                }
            }
        }

        int fretSize = lineHeight/this->diagram->getNumberOfFrets();
        lineHeight = fretSize*this->diagram->getNumberOfFrets();

        char *dstPix = this->cache->getPixels();

        for(int32_t j=0; j < lineHeight+1; j++)
        {
            uint32_t *dst = (uint32_t *) dstPix + w*(j+topMargin) + leftMargin + offset;

            int mod = j % fretSize;

            if(thickness == 1)
            {
                if(mod != 0)
                {
                    memcpy(dst, gridLine, lineLength*4);
                }
                else
                {
                    memcpy(dst, fullLine, lineLength*4);
                }
            }
            else
            {
                if(mod != 0 && mod != (fretSize-1) && j!=1)
                {
                    memcpy(dst, gridLine, lineLength*4);
                }
                else
                {
                    memcpy(dst, fullLine, lineLength*4);
                }
            }
        }

        if(this->diagram->getStartingFret() == 0)
        {
            int size = h/50;
            PKBitmapOps::fillRect(this->cache, fc, 
                                  leftMargin+offset, 
                                  topMargin-(size)+(size==1?0:1), 
                                  lineLength, (h/50));
        }

        // Draw notes

        for(uint32_t k=0; k < this->diagram->getNumberOfNotes(); k++)
        {
            uint8_t string = this->diagram->getStringForNoteAt(k);
            int8_t  fret   = this->diagram->getFretForNoteAt(k);

            if(fret > 0)
            {
                // Normal note
                int wn = stringSize-2;
                int hn = fretSize-3;
                int r = pk_min(wn, hn);

                int xn = leftMargin + offset + string*stringSize;
                int yn = topMargin + (fret-1)*fretSize + fretSize/2;

                PKBitmapOps::fillCircle(this->cache->getPixels(), w, h, fc, xn, yn, pk_max(3, r/2));
            }
            else
            {
                if(fret == 0)
                {
                    // Open string
                    int xx = leftMargin + offset + string*stringSize - font->getCharWidth(L'O')/2 + 1;
                    int yy = (topMargin - font->getCharHeight())/2;
                    font->drawText(L"O", this->cache, xx, yy);
                }
                else
                {
                    // Muted string
                    int xx = leftMargin + offset + string*stringSize - font->getCharWidth(L'X')/2 + 1;
                    int yy = (topMargin - font->getCharHeight())/2;
                    font->drawText(L"X", this->cache, xx, yy);
                }
            }
        }

        // Draw bars

        char *pix = this->cache->getPixels();

        for(uint32_t k=0; k < this->diagram->getNumberOfBars(); k++)
        {
            PKChordBar bar = this->diagram->getBarAt(k);

            uint8_t s = bar.startString;
            uint8_t e = bar.endString;
            uint8_t  fret = bar.fret;

            int wn = stringSize-2;
            int hn = fretSize-3;
            int r = pk_min(wn, hn);

            int xn1 = leftMargin + offset + s*stringSize;
            int yn1 = topMargin + (fret-1)*fretSize + fretSize/2;

            int xn2 = leftMargin + offset + e*stringSize;
            int yn2 = topMargin + (fret-1)*fretSize + fretSize/2;

            PKBitmapOps::fillCircle(pix, w, h, fc, xn1, yn1, pk_max(3, r/2));
            PKBitmapOps::fillCircle(pix, w, h, fc, xn2, yn2, pk_max(3, r/2));

            PKBitmapOps::fillRect(pix, w, h, fc, xn1, yn1-r/2+1, (e-s)*stringSize, r-1);
        }
       
        // Draw start fret

        uint32_t start = this->diagram->getStartingFret(); 

        if(start > 0)
        {
            std::wstring t = PKStr::uint32ToWstring(start);
            int xx = pk_max(0, (leftMargin - font->getTextWidth(t))/2 - (stringSize/4)+2);
            int yy = topMargin + (fretSize - font->getCharHeight())/2 - 1;
            font->drawText(t, this->cache, xx, yy, 150);
        }

        delete font;
        delete [] fullLine;

    }

    this->cache->copy(0, 0, (char *)buffer, 0, 0, w, h);
}
