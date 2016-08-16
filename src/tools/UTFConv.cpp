//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "UTFConv.h"

// UTF-8 -> UTF-16
int UTF8toUTF16(const unsigned char *utf08, unsigned int length, unsigned short *utf16, unsigned int maxout, bool &hasBOM)
{
    unsigned int cur = 0;
    unsigned int v=0;
    unsigned int o=0;
    unsigned int r=0;
    unsigned int tail;

    // it's da BOM
    if(length >= 3 && (utf08[0] == 0xEF && utf08[1] == 0xBB && utf08[2] == 0xBF))
    {
        v = 3;
        hasBOM = true;
    }
    else
    {
        hasBOM = false;
    }

    while(v<length)
    {
        if(utf08[v] < 0x80)
        {
            cur = utf08[v];
            tail = 0;
        }
        else if(utf08[v] < 0xC0)
        {
            return -1;
        }
        else if(utf08[v] < 0xE0)
        {
            cur = utf08[v] & 0x1F;
            tail = 1;
        }
        else if(utf08[v] < 0xF0)
        {
            cur = utf08[v] & 0x0F;
            tail = 2;
        }
        else if(utf08[v] < 0xF8)
        {
            cur = utf08[v] & 0x07;
            tail = 3;
        }
        else
        {
            return -1;
        }

        ++v;

        for(r=0;r<tail;++r)
        {
            if(v >= length || ((utf08[v] & 0xC0) != 0x80))
                return -1;

            cur = (cur << 6) | (utf08[v] & 0x3F);

            ++v;
        }

        if(cur < 0x10000)
        {
            if(o >= maxout)
                return -1;

            utf16[o++] = cur;
        }
        else if(cur < 0x110000)
        {
            if(o+1 >= maxout)
                return -1;

            cur -= 0x10000;

            utf16[o++] = 0xD800 | (cur >> 10);
            utf16[o++] = 0xDC00 | (cur & 0x03FF);
        }
        else
        {
            return -1;
        }
    }

    return o;
}

// UTF-16 -> UTF-8
int UTF16toUTF8(const unsigned short *utf16, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM)
{
    unsigned int cur = 0;
    unsigned int v=0;
    unsigned int o=0;
    signed int r=0;
    signed int bits = 0;

    // it's da BOM
    if(useBOM)
    {
        utf08[o++] = 0xEF;
        utf08[o++] = 0xBB;
        utf08[o++] = 0xBF;
    }

    while(v<length)
    {
        cur = utf16[v++];

        if( (cur & 0xFC00) == 0xD800)
        {
            if( (v < length) && ((utf16[v] & 0xFC00) == 0xDC00))
            {
                cur  = (cur & 0x03FF) << 10;
                cur |= utf16[v] & 0x03FF;
                cur += 0x10000;
            }
            else
            {
                return -1;
            }

            ++v;
        }

        if(o >= maxout)
            return -1;

        if(cur < 0x80)
        {
            utf08[o++] = cur;
            bits = 0;
        }
        else if(cur < 0x800)
        {
            utf08[o++] = (cur >> 6) | 0xC0;
            bits = 6;
        }
        else if(cur < 0x10000)
        {
            utf08[o++] = (cur >> 12) | 0xE0;
            bits = 12;
        }
        else
        {
            utf08[o++] = (cur >> 18) | 0xF0;
            bits = 18;
        }

        for(r=bits;r>0;r-=6)
        {
            if(o >= maxout)
                return -1;

            utf08[o++] = 0x80 | ((cur >> (r-6)) & 0x3F);
        }
    }

    return o;
}

int LATIN1toUTF8(const unsigned char *latin1, unsigned int length, unsigned char **utf8, bool useBOM)
{
    unsigned char testChar;

    int outSize = 0;
    unsigned int index   = 0;

    // Calculate the size of the utf8 buffer.
    for(index = 0 ; index < length ; ++ index)
    {
        testChar = latin1[index];
        if(testChar > 127)
        {
            outSize += 2;
        }
        else
        {
            outSize += 1;
        }
    }
    *utf8 = new unsigned char[outSize + 1]; // leave room for a terminating null
    outSize = 0;
    for(index = 0 ; index < length ; ++ index)
    {
        testChar = latin1[index];
        if(testChar > 127)
        {
            (*utf8)[outSize] = (unsigned char)(0xC0 | ((int)testChar >> 6));    // 110xxxxx
            ++ outSize;
            (*utf8)[outSize] = 0x80 | (0x3F & (int)testChar);   // 10xxxxxx
            ++ outSize;
        }
        else
        {
            (*utf8)[outSize] = testChar;
            outSize += 1;
        }
    }
    (*utf8)[outSize] = 0;
    return outSize;
}

int UTF8ToWchart(const unsigned char *utf08, unsigned int length, wchar_t *wchart, unsigned int maxout, bool &hasBOM)
{
    unsigned short *buffer= new unsigned short[maxout];
    int result = UTF8toUTF16(utf08, length, buffer,  maxout, hasBOM);

    for(int index = 0; index < result; ++ index)
    {
        wchart[index] = buffer[index];
    }

    delete [] buffer;

    return result;
}

int WchartToUTF8(const wchar_t *wchart, unsigned int length, unsigned char *utf08, unsigned int maxout, bool useBOM)
{
    unsigned short *buffer = new unsigned short[maxout + 1];

    for(unsigned int index = 0; index < length; ++ index)
    {
        buffer[index] = wchart[index];
    }

    int result = UTF16toUTF8(buffer, length, utf08, maxout, useBOM);

    delete [] buffer;

    return result;
}

