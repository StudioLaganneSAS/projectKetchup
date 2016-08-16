//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKJPGLoader.h"

#ifdef LINUX

typedef struct _RGBTRIPLE {

    uint8_t rgbtBlue;
    uint8_t rgbtGreen;
    uint8_t rgbtRed;
    
} RGBTRIPLE;

#endif

/*
 *
 *
 */

PKJPGLoader::PKJPGLoader()
{
	this->pixels   = NULL;
	this->width    = 0;
	this->height   = 0;
	
#ifdef MACOSX
	this->cgImageRef = NULL;
#endif
}

PKJPGLoader::~PKJPGLoader()
{
	this->unload();
}

bool PKJPGLoader::load(std::wstring filename)
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

bool PKJPGLoader::load(char *buffer, int size)
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

#if(defined(WIN32) || defined(LINUX))

// Data provider callbacks for libjpeg

static PKByteReader    *static_reader = NULL;
static unsigned char   *static_buffer = NULL;
static bool			    static_fail   = false;

static void init_source(j_decompress_ptr cinfo)
{
	cinfo->src->next_input_byte = NULL;
	cinfo->src->bytes_in_buffer = 0;
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	if(static_reader != NULL && 
	   static_buffer != NULL)
	{
		int read = static_reader->read((char *) static_buffer, 4096);
		
		if(read > 0)
		{
			cinfo->src->next_input_byte = (const unsigned char *) static_buffer;
			cinfo->src->bytes_in_buffer = read;
		}
		else
		{
			// Insert fake EOI

			static_buffer[0] = (JOCTET) 0xFF;
			static_buffer[1] = (JOCTET) JPEG_EOI;
			
			cinfo->src->next_input_byte = (const unsigned char *) static_buffer;
			cinfo->src->bytes_in_buffer = 2;
		}
	}

    return TRUE;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    if((size_t)num_bytes > cinfo->src->bytes_in_buffer) {
        
		cinfo->src->next_input_byte = NULL;
        cinfo->src->bytes_in_buffer = 0;
		
		if(static_reader != NULL)
		{
			static_reader->skip(num_bytes - cinfo->src->bytes_in_buffer);
		}
    }
	else 
	{
        cinfo->src->next_input_byte += (size_t) num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

static void term_source(j_decompress_ptr cinfo)
{
}

void jpeg_memory_src(j_decompress_ptr cinfo)
{
    jpeg_source_mgr *src;

    src = cinfo->src = (jpeg_source_mgr *) malloc(sizeof(jpeg_source_mgr));

    src->init_source       = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data   = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart;
    src->term_source       = term_source;
    src->next_input_byte   = NULL;
    src->bytes_in_buffer   = 0;

	static_buffer = new unsigned char[4096];
}

METHODDEF(void) friendly_error_exit (j_common_ptr cinfo)
{
	static_fail = true;
}

#endif

#ifdef MACOSX

// Data provider callbacks for CGImageCreateWithJPEGDataProvider

size_t juiDataProviderGetBytes(void *info,    
							  void *buffer,    
							  size_t count)
{
	PKByteReader *reader = (PKByteReader *) info;
	
	return reader->read((char *) buffer, count);
}							  

void juiDataProviderSkipBytes(void *info, size_t count)
{
	PKByteReader *reader = (PKByteReader *) info;
	reader->skip(count);
}

void juiDataProviderRewind(void *info)
{
	PKByteReader *reader = (PKByteReader *) info;
	reader->seek(0);	
}

void juiDataProviderRelease(void *info)
{
	// Nothing to do
}

#endif

bool PKJPGLoader::load(PKByteReader *reader)
{
#if(defined(WIN32) || defined(LINUX))

    jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

    uint8_t *buffer = 0, *bufcur = 0;

    uint32_t row_stride = 0, y;

    cinfo.err = jpeg_std_error(&jerr);
  	jerr.error_exit = friendly_error_exit;

    // initialize jpeg decompression object

    jpeg_create_decompress(&cinfo);

    // specify data source
    
	jpeg_memory_src(&cinfo);
	static_reader = reader;
	static_fail   = false;

    // read jpeg header
    jpeg_read_header(&cinfo, TRUE);

    // begin decompression
    jpeg_start_decompress(&cinfo);

    // update row_stride
    row_stride = cinfo.output_width * cinfo.output_components;

	if(static_fail == true)
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		static_reader = NULL;

		if(static_buffer != NULL)
		{
			delete [] static_buffer;
			static_buffer = NULL;
		}
	
		return false;
	}

    // save these for caller
    this->width  = cinfo.output_width;
    this->height = cinfo.output_height;

    // calculate bitmap size
    int bmpSize = row_stride*cinfo.output_height;

    // allocate bitmap data
    uint8_t *temp = (uint8_t*) malloc(bmpSize);

	if(temp == NULL)
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		static_reader = NULL;

		if(static_buffer != NULL)
		{
			delete [] static_buffer;
			static_buffer = NULL;
		}
		return false;
	}

    bufcur = temp;

    for(y=0;y<cinfo.output_height;y++)
    {
        jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&bufcur, 1);
        bufcur += row_stride;
    }

	if(static_fail == true)
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		free(temp);
		static_reader = NULL;

		if(static_buffer != NULL)
		{
			delete [] static_buffer;
			static_buffer = NULL;
		}
	
		return false;
	}

    // now copy color data

	this->pixels = new char[this->width*this->height*4];

	if(this->pixels == NULL)
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		static_reader = NULL;
		free(temp);

		if(static_buffer != NULL)
		{
			delete [] static_buffer;
			static_buffer = NULL;
		}
		return false;
	}

    {

        RGBTRIPLE *rgbData = (RGBTRIPLE*) temp;

        uint32_t v=0;
		uint32_t d=0;

        for(v=0;v<cinfo.output_width*cinfo.output_height;v++)
        {
            uint8_t r = rgbData[v].rgbtRed;
            uint8_t g = rgbData[v].rgbtGreen;
            uint8_t b = rgbData[v].rgbtBlue;
 
			this->pixels[d]   = r;
			this->pixels[d+1] = g;
			this->pixels[d+2] = b;
			this->pixels[d+3] = (char) 0xFF;

			d += 4;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

	free(temp);
	static_reader = NULL;

	if(static_buffer != NULL)
	{
		delete [] static_buffer;
		static_buffer = NULL;
	}

	return true;

#endif

#ifdef MACOSX

	CGDataProviderCallbacks dataProviderCallbacks;
	
	dataProviderCallbacks.getBytes        = juiDataProviderGetBytes;
	dataProviderCallbacks.skipBytes       = juiDataProviderSkipBytes;
	dataProviderCallbacks.rewind          = juiDataProviderRewind;
	dataProviderCallbacks.releaseProvider = juiDataProviderRelease;

	CGDataProviderRef uiReaderProvider = CGDataProviderCreate((void *) reader,
															  &dataProviderCallbacks);

	CGImageRef jpgImage = CGImageCreateWithJPEGDataProvider(uiReaderProvider, NULL,
														    false, kCGRenderingIntentDefault);
														   
	if(jpgImage != NULL)
	{
		CGContextRef bitmapContext;
		
		this->width  = CGImageGetWidth(jpgImage);
		this->height = CGImageGetHeight(jpgImage); 
		
		this->pixels = (char*) malloc(width*height*4);
		
		if(this->pixels == NULL)
		{
			CGImageRelease(jpgImage);
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
			
			CGContextDrawImage(bitmapContext, imgRect, jpgImage);
			
			CGContextRelease(bitmapContext);

			this->cgImageRef = jpgImage;
			CGImageRetain(this->cgImageRef);
			
			CGImageRelease(jpgImage);
			
			return true;
		}
		
		CGImageRelease(jpgImage);
	} 

#endif

	return false;
}

bool PKJPGLoader::unload()
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

char *PKJPGLoader::getPixels()
{
	return this->pixels;
}

unsigned int PKJPGLoader::getChannels()
{
	return this->channels;
}

unsigned int PKJPGLoader::getWidth()
{
	return this->width;
}

unsigned int PKJPGLoader::getHeight()
{
	return this->height;
}

#ifdef MACOSX
CGImageRef PKJPGLoader::getCGImageRef()
{
	return this->cgImageRef;
}
#endif
