//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKGIFLoader.h"
#include "PKStr.h"
#include "PKPath.h"

//
// PKGIFLoader
//

PKGIFLoader::PKGIFLoader()
{
	this->pixels   = NULL;
	this->width    = 0;
	this->height   = 0;
	
#ifdef MACOSX
	this->cgImageRef = NULL;
#endif
}

PKGIFLoader::~PKGIFLoader()
{
	this->unload();
}

bool PKGIFLoader::load(std::wstring filename)
{
#ifdef WIN32

    LPPICTURE pPic = NULL;

    OleLoadPicturePath((LPOLESTR) filename.c_str(), NULL, 
                       0, 0, IID_IPicture, (LPVOID*) &pPic);

    if (pPic == NULL)
    {
        // Not a GIF
        return false;
    }

    HBITMAP hPic = NULL;
    pPic->get_Handle((UINT*)&hPic);

    if(hPic == NULL)
    {
        pPic->Release();
        return false;
    }

    BITMAP bitmapObject;
    GetObject(hPic, sizeof(BITMAP), &bitmapObject);

    HDC memDC = CreateCompatibleDC(NULL);

    BITMAPINFO *bmi = (BITMAPINFO *) malloc(sizeof(BITMAPINFO));

    memset(&bmi->bmiHeader, 0, sizeof(BITMAPINFOHEADER));
    bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    bmi->bmiHeader.biBitCount    = 32;
    bmi->bmiHeader.biCompression = BI_RGB;
    bmi->bmiHeader.biWidth       = bitmapObject.bmWidth;
    bmi->bmiHeader.biHeight      = bitmapObject.bmHeight;
    bmi->bmiHeader.biPlanes      = 1;
    bmi->bmiHeader.biSizeImage   = bitmapObject.bmWidth*bitmapObject.bmHeight*4;

    void *pix = NULL;

    HBITMAP dib = CreateDIBSection(memDC,
                                   bmi,
                                   DIB_RGB_COLORS,
                                   &pix,
                                   NULL, 0);

    if(dib == NULL)
    {
        return false;
    }

    DWORD attrs;
    pPic->get_Attributes(&attrs);

    if((attrs & PICTURE_TRANSPARENT) == PICTURE_TRANSPARENT)
    {
        unsigned char *dst = (unsigned char *) pix;

        for(unsigned int j=0; j < this->height; j++)
        {
            for(unsigned int i=0; i < this->width; i++)
            {
                dst[0] = 0xFF;
                dst[1] = 0x00;
                dst[2] = 0xFF;
            }
        }
    }

    SelectObject(memDC, dib);

    OLE_XSIZE_HIMETRIC w;
    OLE_YSIZE_HIMETRIC h;

    pPic->get_Width(&w);
    pPic->get_Height(&h);

    pPic->Render(memDC, 
                 0, 0, 
                 bitmapObject.bmWidth, 
                 bitmapObject.bmHeight, 
                 0, 0, w, h,
                 NULL);

    this->height    = abs(bmi->bmiHeader.biHeight);
    this->width     = bmi->bmiHeader.biWidth;
    this->bit_depth = bmi->bmiHeader.biBitCount;

    this->pixels = new char[bmi->bmiHeader.biSizeImage];
    memcpy(this->pixels, pix, bmi->bmiHeader.biSizeImage);

    DeleteObject(dib);
    DeleteDC(memDC);

    pPic->Release();
    free(bmi);

    // Just mark the whole image as
    // being opaque (alpha == 0xFF)

    if((attrs & PICTURE_TRANSPARENT) == PICTURE_TRANSPARENT)
    {
        // Asign transparent pixels
        // Based on the color we filled
        // in previously.

        unsigned char *dst = (unsigned char *) this->pixels;

        for(unsigned int j=0; j < this->height; j++)
        {
            for(unsigned int i=0; i < this->width; i++)
            {
                if(dst[0] == 0xFF &&
                   dst[1] == 0x00 &&
                   dst[2] == 0xFF)
                {
                    dst[3] = 0x00;
                }
                else
                {
                    dst[3] = 0xFF;
                }

                dst += 4;
            }
        }
    }
    else
    {
        // Mark all pixels opaque

        unsigned char *dst = (unsigned char *) this->pixels;

        for(unsigned int j=0; j < this->height; j++)
        {
            for(unsigned int i=0; i < this->width; i++)
            {
                dst[3] = 0xFF;
                dst += 4;
            }
        }
    }

    return true;

#endif

#ifdef MACOSX

	CFStringRef path = PKStr::wStringToCFString(filename);

    CGImageRef                  imageRef = NULL;
    Handle                      dataRef = NULL;
    OSType                      dataRefType;
    GraphicsImportComponent     gi;
    ComponentResult             result;
   
    result = QTNewDataReferenceFromFullPathCFString(path, kQTNativeDefaultPathStyle, 
													0, &dataRef,  &dataRefType);

	CFRelease(path);

    if(NULL != dataRef) 
	{
       GetGraphicsImporterForDataRefWithFlags(dataRef, dataRefType, &gi, 0);
	   
       result = GraphicsImportCreateCGImage(gi, &imageRef, 0);
       
	   DisposeHandle(dataRef);
       CloseComponent(gi);

	   if(result == noErr)
	   {
		   CGContextRef bitmapContext;
		   
		   this->width  = CGImageGetWidth(imageRef);
		   this->height = CGImageGetHeight(imageRef); 
		   
		   this->pixels = (char*) malloc(width*height*4);
		   
		   if(this->pixels == NULL)
		   {
			   CGImageRelease(imageRef);
			   return false;
		   }
		   
		   memset(this->pixels, 0, width*height*4);
		   
		   CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
		   
		   bitmapContext = CGBitmapContextCreate(this->pixels, this->width, this->height,
												 8, this->width*4, colorspace, kCGImageAlphaPremultipliedFirst);
		   
		   if(bitmapContext != NULL)
		   {
			   // Draw the image on the context
			   
			   CGRect imgRect;
			   
			   imgRect.origin.x = 0;
			   imgRect.origin.y = 0;
			   imgRect.size.width  = this->width;
			   imgRect.size.height = this->height;
			   
			   CGContextDrawImage(bitmapContext, imgRect, imageRef);
			   
			   CGContextRelease(bitmapContext);
			   
			   this->cgImageRef = imageRef;
			   CGImageRetain(this->cgImageRef);			   
			   
			   CGImageRelease(imageRef);
			   
			   return true;
		   }
		   
		   CGImageRelease(imageRef);
	   }	   
    }

	return false;
	
#endif
}

bool PKGIFLoader::load(char *buffer, int size)
{
    // We have to write the buffer to a 
    // temporary file and read from disk

    std::wstring tempPath = PKPath::getTempPath();
    std::wstring tempFile = PKPath::getTempFileName(tempPath);

    PKFile *file = new PKFile();

    if(file->open(tempFile, PKFILE_MODE_WRITE) == PKFILE_OK)
    {
        bool success = false;

        file->write(buffer, size);
        file->close();

        delete file;
		
        success = this->load(tempFile);

        PKPath::deleteFile(tempFile);

        return success;
    }

    return false;
}

bool PKGIFLoader::load(PKByteReader *reader)
{
#ifdef WIN32

    // Not used
    return false;

#endif

#ifdef MACOSX

	// Not used
	return false;
	
#endif

	return false;
}

bool PKGIFLoader::unload()
{
	if(this->pixels)
	{
		free(this->pixels);
		this->pixels = NULL;
	}	

	this->channels = 0;
	this->width    = 0;
	this->height   = 0;
	
#ifdef MACOSX
	
	if(this->cgImageRef)
	{
		CGImageRelease(this->cgImageRef);
		this->cgImageRef = NULL;
	}
	
#endif
	
	return true;
}

char *PKGIFLoader::getPixels()
{
	return this->pixels;
}

unsigned int PKGIFLoader::getChannels()
{
	return this->channels;
}

unsigned int PKGIFLoader::getWidth()
{
	return this->width;
}

unsigned int PKGIFLoader::getHeight()
{
	return this->height;
}

#ifdef MACOSX
CGImageRef PKGIFLoader::getCGImageRef()
{
	return this->cgImageRef;
}
#endif

