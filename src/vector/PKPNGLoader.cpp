//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKPNGLoader.h"

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

//
// PKPNGLoader
//

PKPNGLoader::PKPNGLoader()
{
	this->pixels   = NULL;
	this->png_ptr  = NULL;
	this->info_ptr = NULL;
	this->width    = 0;
	this->height   = 0;

#ifdef MACOSX
	this->cgImage  = NULL;
#endif
}

PKPNGLoader::~PKPNGLoader()
{
	this->unload();
}

bool PKPNGLoader::load(std::wstring filename)
{
	PKByteReader reader;

	if(reader.open(filename))
	{
		return this->load(&reader);
	}
	else
	{
		return false;
	}
}

bool PKPNGLoader::load(char *buffer, int size)
{
	PKByteReader reader;

	if(reader.open(buffer, size))
	{
		return this->load(&reader);
	}
	else
	{
		return false;
	}
}

// Data provider callback for libpng

void ui_user_read_data(png_structp png_ptr, 
					   png_bytep data, 
					   png_size_t length)

{
	PKByteReader *reader = (PKByteReader *) png_get_io_ptr(png_ptr);

	reader->read((char *) data, (int) length);
}


bool PKPNGLoader::load(PKByteReader *reader)
{
    uch           sig[8];
	unsigned long width;
	unsigned long height;
    png_uint_32   i, rowbytes;
	char         *tmp;
	int           bitCount;			
	int           bpp;

    png_bytepp  row_pointers = NULL;

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

	reader->read((char *)sig, 8);

    if (!png_check_sig(sig, 8))
	{
        return false;   /* bad signature */
	}

    /* could pass pointers to user-defined error handlers instead of NULLs: */

    this->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!this->png_ptr)
	{
        return false;   /* out of memory */
	}

    this->info_ptr = png_create_info_struct(png_ptr);

    if (!this->info_ptr) 
	{
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;   /* out of memory */
    }


    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_ptr->jmpbuf)) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

	png_set_read_fn(png_ptr, (voidp) reader, ui_user_read_data);
	
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */


    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

    png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth, &color_type, NULL, NULL, NULL);

    /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);


    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    this->channels = (int)png_get_channels(png_ptr, info_ptr);

    if ((tmp = (char *)malloc(rowbytes*height)) == NULL) 
	{        
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(tmp);

        return false;
    }

    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0;  i < height;  ++i)
	{
        row_pointers[i] = ((uch *) tmp) + (height - i - 1)*rowbytes;
	}

    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);

    free(row_pointers);
    row_pointers = NULL;

    png_read_end(png_ptr, NULL);

	/*
	 * now, convert the data
	 *
	 */

	this->width  = width;
	this->height = height;

	switch(this->channels) 
	{
		case 4:
		    bitCount    = 32;
			break;
		case 3:
		    bitCount    = 24;
			break;
		default:
			return false;
			break;
	}

	bpp = bitCount/8;

	this->pixels = (char*) malloc(width*height*4);

	switch(bpp) {

		case 3:
			{
				for(unsigned int j=0; j<height; j++) {

					for(unsigned int i=0; i<width; i++) {
	
						memcpy((char *) pixels + j*width*4 + i*4, 
							   (char *) tmp    + (height - j - 1)*width*bpp + i*bpp, bpp);
					}
				}
			}
			break;

		case 4:
			{
				for(unsigned int i=0; i<height; i++) {

					memcpy((char *) pixels + i*width*bpp, 
						   (char *) tmp + (height - i - 1)*width*bpp,
						   width*bpp);
				}
			}
			break;
	}

#if(defined(WIN32) || defined (LINUX))
	
	// invert only R & B
    
	for (unsigned int yIndex = 0; yIndex < height; yIndex++)
	{
	    unsigned char * pPixel = (unsigned char *) this->pixels + width * 4 * yIndex;

        for (unsigned int xIndex = 0; xIndex < width; xIndex++)
		{
  		   unsigned char temp;

		   temp = pPixel[2];

	       pPixel[2] = pPixel[0]; 
		   pPixel[0] = temp;

		   if(this->channels == 3) 
		   {
			   pPixel[3] = 255; 
		   }

		   pPixel += 4;
		}
	}
	
#endif

#if(defined(MACOSX))
	
	// convert from RGBA to ARGB
    
	for (unsigned int yIndex = 0; yIndex < height; yIndex++)
	{
	    unsigned char * pPixel = (unsigned char *) this->pixels + width * 4 * yIndex;
		
        for (unsigned int xIndex = 0; xIndex < width; xIndex++)
		{
			unsigned char temp;
			
			temp = pPixel[3];
			
			pPixel[3] = pPixel[2]; 
			pPixel[2] = pPixel[1]; 
			pPixel[1] = pPixel[0]; 
			pPixel[0] = temp;
			
			if(this->channels == 3) 
			{
				pPixel[0] = 255; 
			}
			
			pPixel += 4;
		}
	}
	
#endif

#ifdef MACOSX
	
	int w = width;
	int h = height;
	
	// Create a CGimage 
	
    CGDataProviderRef provider;
    CGColorSpaceRef   colorspace;
    
    colorspace = CGColorSpaceCreateDeviceRGB();
    provider   = CGDataProviderCreateWithData(NULL, this->pixels, w*h*4, NULL);
    
    this->cgImage = CGImageCreate(w, h, 8, 32, w*4, 
                          colorspace,
                          kCGImageAlphaFirst,
                          provider,
                          NULL, false,
                          kCGRenderingIntentDefault);
    
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorspace);	
	
	
#endif
	
	free(tmp);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	this->png_ptr = NULL;

	return true;
}

bool PKPNGLoader::unload()
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
	
	if(this->cgImage)
	{
		CGImageRelease(this->cgImage);
		this->cgImage = NULL;
	}
	
#endif
	
	return true;
}

char *PKPNGLoader::getPixels()
{
	return this->pixels;
}

unsigned int PKPNGLoader::getChannels()
{
	return this->channels;
}

unsigned int PKPNGLoader::getWidth()
{
	return this->width;
}

unsigned int PKPNGLoader::getHeight()
{
	return this->height;
}

#ifdef MACOSX

CGImageRef PKPNGLoader::getCGImageRef()
{
	return this->cgImage;
}

#endif

