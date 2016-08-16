//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKBase64.h"

//
// Gobals
//

static char _base64Chars[]= {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
						     'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
					         '0','1','2','3','4','5','6','7','8','9',
							 '-','_' };

//
// PKBase64
//

std::string PKBase64::encode(char *str, int length)
{
    std::string result;

    int i;
    int blocks = length / 3;
    int left   = length % 3;

    for(i=0; i < blocks; i++)
    {
        result += PKBase64::encodeBlock(str + i*3, 3);
    }

    if(left > 0)
    {
        result += PKBase64::encodeBlock(str + blocks*3, left);
    }

    return result;
}

unsigned char* PKBase64::decode(std::string str, int* size)
{
    if(((str.size() % 4) > 0) || (str.size() == 0))
    {
		*size = 0;
        return NULL;
    }

    int blocks   =  str.size() / 4;
    int outSize  = blocks*3;
    int realSize = outSize;

    if(str[str.size() - 1] == '=')
    {
        realSize--;

        if(str[str.size() - 2] == '=')
        {
            realSize--;
        }
    }

    char *out = new char[outSize];
	memset(out, 0, outSize);

    for(int i=0; i < blocks; i++)
    {
        PKBase64::decodeBlock(str.substr(i*4, 4), out + i*3);
    }

	*size = realSize;
    return (unsigned char*)out;
}

std::string PKBase64::encodeBlock(char *block, int size)
{
    std::string result(4, '=');

    switch(size)
    {
    case 3:
        {
            int b1 = ((block[0] & 0xFC) >> 2);
            int b2 = ((block[0] & 0x03) << 4) | ((block[1] & 0xF0) >> 4);
            int b3 = ((block[1] & 0x0F) << 2) | ((block[2] & 0xC0) >> 6);
            int b4 = ((block[2] & 0x3F));

            result[0] = _base64Chars[b1]; 
            result[1] = _base64Chars[b2]; 
            result[2] = _base64Chars[b3]; 
            result[3] = _base64Chars[b4]; 

        }
        break;
    
    case 2:
        {
            int b1 = ((block[0] & 0xFC) >> 2);
            int b2 = ((block[0] & 0x03) << 4) | ((block[1] & 0xF0) >> 4);
            int b3 = ((block[1] & 0x0F) << 2);

            result[0] = _base64Chars[b1]; 
            result[1] = _base64Chars[b2]; 
            result[2] = _base64Chars[b3]; 
            
        }
        break;
    
    case 1:
        {
            int b1 = ((block[0] & 0xFC) >> 2);
            int b2 = ((block[0] & 0x03) << 4);
            
            result[0] = _base64Chars[b1]; 
            result[1] = _base64Chars[b2]; 
            
        }
        break;
    }

    return result;
}


void PKBase64::decodeBlock(std::string block, char *out)
{
    if((block[3] == '=') && (block[2] == '='))
    {
        int c1 = PKBase64::charValue(block[0]);
        int c2 = PKBase64::charValue(block[1]);

        if((c1 == -1) || (c2 == -1))
        {
            return;
        }

        int v1 = (c1 << 2) | (c2 >> 4); 
        out[0] = (char) v1;
        out[1] = '\0';

    }
    else
    {
        if(block[3] == '=')
        {
            int c1 = PKBase64::charValue(block[0]);
            int c2 = PKBase64::charValue(block[1]);
            int c3 = PKBase64::charValue(block[2]);

            if((c1 == -1) || 
               (c2 == -1) ||
               (c3 == -1))
            {
                return;
            }

            int v1 = (c1 << 2) | ((c2 & 0x30) >> 4); 
            int v2 = ((c2 & 0x0F)<< 4) | ((c3 & 0x3C) >> 2); 
            
            out[0] = (char) v1;
            out[1] = (char) v2;
            out[2] = '\0';
        }
        else
        {
            int c1 = PKBase64::charValue(block[0]);
            int c2 = PKBase64::charValue(block[1]);
            int c3 = PKBase64::charValue(block[2]);
            int c4 = PKBase64::charValue(block[3]);

            if((c1 == -1) || 
               (c2 == -1) ||
               (c3 == -1) ||
               (c4 == -1))
            {
                return;
            }

            int v1 = (c1 << 2) | ((c2 & 0x30) >> 4); 
            int v2 = ((c2 & 0x0F)<< 4) | ((c3 & 0x3C) >> 2); 
            int v3 = ((c3 & 0x03)<< 6) | ((c4 & 0x3F)); 

            out[0] = (char) v1;
            out[1] = (char) v2;
            out[2] = (char) v3;
        }
    }

}

int PKBase64::charValue(char c)
{
    int i;

    for(i=0; i < 64; i++)
    {
        if(_base64Chars[i] == c)
        {
            return i;
        }
    }

    return -1;
}

