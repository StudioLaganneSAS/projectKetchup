//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKImageView.h"

#include "PKImage.h"
#include "PKImageLoader.h"
#include "PKLayout.h"
#include "PKDialog.h"
#include "PKComboBox.h"
#include "PKGroupBox.h"
#include "PKButton.h"
#include "PKCheckBox.h"
#include "PKSlider.h"
#include "PKLink.h"
#include "PKPanelControl.h"
#include "PKLabel.h"
#include "PKGradientPanel.h"
#include "PKScrollView.h"

#ifdef WIN32

#define PKIMG_FOWARD_MOUSE_EVENTS

LRESULT CALLBACK PKImageView::ImageWinProc(HWND hWnd, 
										   UINT msg, 
										   WPARAM wParam, 
										   LPARAM lParam)
{
	PKImageView *img = (PKImageView *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(img == NULL)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	PKDialog *dialog = img->dialog;

	if(dialog == NULL)
	{
        return img->lpOldProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
	case WM_COMMAND:
		{
			// Find which control is sending this
			// and fire the appropriate callback

			uint32_t msg = HIWORD(wParam);
			uint32_t id  = LOWORD(wParam);

			PKControl *obj = PKControl::getControlWithId(id);

			if(obj != NULL)
			{
                if(msg == STN_CLICKED)
                {
                    PKLink *link = dynamic_cast<PKLink*>(obj);

                    if(link)
                    {
                        // Handle the click
                        link->clicked();
                    }
                }

				if(msg == CBN_SELCHANGE)
				{
                    PKComboBox *combo = dynamic_cast<PKComboBox*>(obj);
					
                    if(combo)
					{
						int32_t oldValue = PKVALUE_INT32(obj->get(PKComboBox::CURRENTITEM_INT32_PROPERTY));
                        dialog->comboBoxChanged(combo->getControlId(), oldValue);
					}
				}

                PKButton *button = dynamic_cast<PKButton*>(obj);

				if(button)
				{
                    dialog->buttonClicked(button->getControlId());
				}

				if(dynamic_cast<PKGroupBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKGroupBox::CHECKED_BOOL_PROPERTY));
                    dialog->checkBoxAction(obj->getControlId(), oldValue);
				}

				if(dynamic_cast<PKCheckBox*>(obj))
				{
					bool oldValue = PKVALUE_BOOL(obj->get(PKCheckBox::CHECKED_BOOL_PROPERTY));
                    dialog->checkBoxAction(obj->getControlId(), oldValue);
				}

				return 0;
			}
		}
		break;

    case WM_HSCROLL:
		{
			HWND controlHWND = (HWND) lParam;

			if(img->getChildrenCount() > 0)
			{
				PKControl *root = dynamic_cast<PKControl*>(img->getChildAt(0));

				if(root)
				{
    				PKView *obj = root->findObjectByWindow(controlHWND);

				    if(obj != NULL)
				    {
                        PKSlider *slider = dynamic_cast<PKSlider*>(obj);

                        if(slider)
					    {
							int32_t previousValue = PKVALUE_INT32(slider->get(PKSlider::VALUE_INT32_PROPERTY));
							int32_t newValue = (int32_t) SendMessage(slider->getWindowHandle(), TBM_GETPOS, 0, 0);

							PKVariantInt32 newV(newValue);
							slider->set(PKSlider::VALUE_INT32_PROPERTY, &newV);

					    	dialog->sliderChanged(slider->getControlId(), previousValue);
					    }
				    }
			    }
            }
		}
		break;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		{
			HWND control = (HWND) lParam;
			HDC  hdc     = (HDC)  wParam;

            // Handle links

			if(img->getChildrenCount() > 0)
    		{
	    		PKControl *root = dynamic_cast<PKControl*>(img->getChildAt(0));

		    	if(root)
			    {
				    PKView *obj = root->findObjectByWindow(control);

                    if(obj != NULL)
	    	        {
                        // Check if it's a link

                        PKLink *link = dynamic_cast<PKLink*>(obj);

                        if(link)
                        {
                            PKVariant *color = link->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        // Check if it's a label

                        PKLabel *label = dynamic_cast<PKLabel*>(obj);

                        if(label)
                        {
                            PKVariant *color = label->get(PKLink::COLOR_COLOR_PROPERTY);
                            PKColor c = PKVALUE_COLOR(color);

                            SetTextColor(hdc, RGB(c.r, c.g, c.b));
                        }

                        SetBkMode(hdc, TRANSPARENT);
                        return (LRESULT) GetStockObject(HOLLOW_BRUSH);
                    }
                }
            }
		}
		break;

	case WM_DRAWITEM:
		{
			// This is called for owner drawn
			// static controls such as the 
			// PKImageView class when it needs to
			// have an alpha channel...

			DRAWITEMSTRUCT *drawStruct = (DRAWITEMSTRUCT *) lParam;

			if(drawStruct != NULL)
			{
				if(img->getChildrenCount() >0)
				{
                    PKControl *root = dynamic_cast<PKControl*>(img->getChildAt(0));

					if(root)
					{
						PKView *obj = root->findObjectByWindow(drawStruct->hwndItem);

						if(obj != NULL)
						{
							if(dynamic_cast<PKImageView*>(obj))
							{
								PKImageView *img = dynamic_cast<PKImageView*>(obj);

                                HBRUSH brush = NULL;

								// Draw the image

								img->alphaDraw(drawStruct->hDC, 
											   drawStruct->rcItem, 
											   brush);
								
								return TRUE;
							}
						}
					}
				}
			}
		}
		break;

#ifdef PKIMG_FOWARD_MOUSE_EVENTS

    case WM_MOUSEMOVE:
        {
			int x;
			int y;
			unsigned int mod;

			dialog->getMouseParams(wParam, lParam, &x, &y, &mod);
            dialog->mouseMove(x+img->getX(), y+img->getY(), mod);
        }
        break;

    case WM_LBUTTONDOWN:
        {
			int x;
			int y;
			unsigned int mod;

            SetCapture(hWnd);

			dialog->getMouseParams(wParam, lParam, &x, &y, &mod);
            dialog->mouseDown(PK_BUTTON_LEFT, x+img->getX(), y+img->getY(), mod);
        }
        break;

    case WM_LBUTTONUP:
        {
			int x;
			int y;
			unsigned int mod;

			dialog->getMouseParams(wParam, lParam, &x, &y, &mod);
            dialog->mouseUp(PK_BUTTON_LEFT, x+img->getX(), y+img->getY(), mod);

            ReleaseCapture();
        }
        break;

    case WM_RBUTTONDOWN:
        {
			int x;
			int y;
			unsigned int mod;

            SetCapture(hWnd);

			dialog->getMouseParams(wParam, lParam, &x, &y, &mod);
            dialog->mouseDown(PK_BUTTON_RIGHT, x+img->getX(), y+img->getY(), mod);
        }
        break;

    case WM_RBUTTONUP:
        {
			int x;
			int y;
			unsigned int mod;

			dialog->getMouseParams(wParam, lParam, &x, &y, &mod);
            dialog->mouseUp(PK_BUTTON_RIGHT, x+img->getX(), y+img->getY(), mod);

            ReleaseCapture();
        }
        break;

#endif // PKIMG_FOWARD_MOUSE_EVENTS
	}

	return img->lpOldProc(hWnd, msg, wParam, lParam);
}

#endif


//
// Properties
//

std::string PKImageView::ALPHA_UINT8_PROPERTY   = "alpha";
std::string PKImageView::IMAGE_WSTRING_PROPERTY = "image";

//
// PKImageView
//

PKImageView::PKImageView()
{
	PKOBJECT_ADD_WSTRING_PROPERTY(image, L"IMG_NOT_SET");
    PKOBJECT_ADD_UINT8_PROPERTY(alpha, 255);

	this->width     = 0;
	this->height    = 0;
	this->bitmap    = NULL;

	this->built = false;

#ifdef WIN32
    this->hbitmap   = NULL;
    this->lpOldProc = NULL;
#endif

#ifdef LINUX
    this->pixbuf = NULL;
#endif

}

PKImageView::~PKImageView()
{
#ifdef WIN32

    if(this->built && this->hwnd)
    {
    	SubclassWindow(this->hwnd, (WNDPROC) this->lpOldProc);
    }

    if(this->hbitmap != NULL)
	{
		SendMessage(this->hwnd, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) (HANDLE) NULL);
		DeleteObject(this->hbitmap);
		this->hbitmap = NULL;
	}

	if(this->hwnd != NULL)
	{
		DestroyWindow(this->hwnd);
	}

#endif

#ifdef LINUX
    if(this->pixbuf != NULL)
	{
	    gdk_pixbuf_unref(this->pixbuf);
	    this->pixbuf = NULL;
	}    
#endif

	if(this->bitmap != NULL)
	{
		delete this->bitmap;
	}
}

void PKImageView::build()
{
	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return;
	}

    if(this->built)
    {
        return;
    }

#ifdef WIN32

	this->hbitmap = NULL;

	MEMORY_BASIC_INFORMATION mbi;
    static int dummy = 0;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

	HMODULE hInst = (HMODULE)(mbi.AllocationBase);

	this->hwnd = CreateWindow(L"STATIC", L"",
							  WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_CENTERIMAGE | SS_NOTIFY, 0, 0, 100, 80,
							  parentView->getWindowHandle(), 
							  (HMENU) this->controlId, 
							  hInst, 0);

    if(this->hwnd)
    {
        this->lpOldProc = SubclassWindow(this->hwnd, (WNDPROC) ImageWinProc);
    	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)(LPVOID) (PKImageView *) this);
    }

#endif

#ifdef LINUX

    this->widget = gtk_image_new();
    
#endif // LINUX

#ifdef MACOSX
	// Do nothing
	this->controlRef = NULL;
#endif
	
	PKVariant *img = this->get(PKImageView::IMAGE_WSTRING_PROPERTY);
	std::wstring URI = PKVALUE_WSTRING(img);
	
	PKImageURIType type;
	std::wstring   image;
	
	PKImage::processURI(URI, &type, &image);

	if(type == PKIMAGE_URI_RESOURCE)
	{
		this->loadImageFromResource(image);
	}

	if(type == PKIMAGE_URI_FILENAME)
	{
		this->loadImageFromFilename(image);
	}

#ifdef LINUX
    
    if(this->widget != NULL && this->bitmap != NULL)
    {
        uint32_t mmw = this->bitmap->getWidth();
        uint32_t mmh = this->bitmap->getHeight();
    
        gtk_widget_set_size_request(this->widget, mmw, mmh);
    }

#endif // LINUX


    // Build children

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->build();
		}
	}

	this->built = true;
}

void PKImageView::initialize(PKDialog *dialog)
{
    PKControl::initialize(dialog);

	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);
		}
	}
}

void PKImageView::destroy()
{
	if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}
    this->clearChildren();

    PKControl::destroy();
}

PKWindowHandle PKImageView::getWindowHandle()
{
    return PKControl::getWindowHandle();

	// We forward to our parent

	PKObject *parent = this->getParent();

	if(parent)
	{
		PKView *parentView = dynamic_cast<PKView*>(parent);

		if(parentView)
		{
			return parentView->getWindowHandle();
		}
	}

	return NULL;
}

#ifdef WIN32
HBRUSH PKImageView::getBackgroundBrush()
{
    // So that our children
    // get drawn in true alpha
    // fashion over uour content
    return (HBRUSH) NULL;
}

void PKImageView::fillDeviceWithBackground(PKDrawingDevice device,
                                           int x, int y, int w, int h)
{
    if(this->hbitmap != NULL)
    {
	    HDC cdc = CreateCompatibleDC(device);
	    SelectObject(cdc, this->hbitmap);

        BitBlt(device, 0, 0, w, h, cdc, x, y, SRCCOPY);
        DeleteObject(cdc);
    }
}

#endif

void PKImageView::move(int32_t x, 
					   int32_t y)
{
	PKControl::move(x, y);

	if(this->getChildrenCount() == 1)
	{
		PKLayout *rootLayout = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(rootLayout)
		{
			rootLayout->move(x, y);
		}
	}
}

void PKImageView::resize(uint32_t w, 
				         uint32_t h)
{
	PKControl::resize(w, h);

    if(this->getChildrenCount() == 1)
	{
		PKLayout *root = dynamic_cast<PKLayout*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w, h);
		}
	}
}


// From PKObject
void PKImageView::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKImageView::IMAGE_WSTRING_PROPERTY)
	{
		PKVariant *img = this->get(PKImageView::IMAGE_WSTRING_PROPERTY);
		std::wstring URI = PKVALUE_WSTRING(img);
		
		PKImageURIType type;
		std::wstring   image;
		
		PKImage::processURI(URI, &type, &image);
		
		if(type == PKIMAGE_URI_RESOURCE)
		{
			this->loadImageFromResource(image);
		}
		
		if(type == PKIMAGE_URI_FILENAME)
		{
			this->loadImageFromFilename(image);
		}
	
        this->hide();
        this->show();
    }

	PKControl::selfPropertyChanged(prop, oldValue);
}

// RGBA input
void PKImageView::setImageBuffer(void *pixelData,
					 			 uint32_t width,
								 uint32_t height)
{
	if(pixelData == NULL ||
	   width	 == 0    ||
	   height    == 0)
	{
		return;
	}

	PKObject *parent = this->getParent();

	if(parent == NULL)
	{
		return;
	}

	PKView *parentView = dynamic_cast<PKView*>(parent);

	if(parentView == NULL)
	{
		return;
	}

#ifdef LINUX

    if(this->pixbuf != NULL)
	{
	    gdk_pixbuf_unref(this->pixbuf);
	    this->pixbuf = NULL;
	}    
	
	// Invert R & B
	
	uint8_t *pdata = (uint8_t *) pixelData;

	for(unsigned int j=0; j < height; j++)
	{
		for(unsigned int i=0; i < width; i++)
		{
		    uint8_t temp = pdata[0];
		     
			pdata[0] = pdata[2];
			pdata[2] = temp;

			pdata +=4;
		}
	}

    this->pixbuf = gdk_pixbuf_new_from_data((guchar*) pixelData,
                                            GDK_COLORSPACE_RGB,
                                            TRUE, 8, width, height, 
                                            width*4, 
                                            NULL, NULL);

    if(this->widget != NULL)
	{
	    gtk_image_set_from_pixbuf(GTK_IMAGE(this->widget), this->pixbuf);
	}    

#endif

#ifdef WIN32

	// alpha premultiply

	uint8_t *pdata = (uint8_t *) pixelData;

	for(unsigned int j=0; j < height; j++)
	{
		for(unsigned int i=0; i < width; i++)
		{
			if(pdata[3] != 255)
			{
				pdata[0] = (uint8_t) ((int)pdata[0]*(int)pdata[3]/255);
				pdata[1] = (uint8_t) ((int)pdata[1]*(int)pdata[3]/255);
				pdata[2] = (uint8_t) ((int)pdata[2]*(int)pdata[3]/255);
			}
		
			pdata +=4;
		}
	}

	if(this->hbitmap != NULL)
	{
		SendMessage(this->hwnd, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) (HANDLE) NULL);
		DeleteObject(this->hbitmap);
		this->hbitmap = NULL;
	}

	this->width  = width;
	this->height = height;

	int w = width;
	int h = height;

	// Create a Bitmap and assign it to the static

	HBITMAP bmp  = NULL;
    void   *bits = NULL;

	BITMAPINFO dibInfo;
	memset(&dibInfo, 0, sizeof(BITMAPINFO));

	dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
	dibInfo.bmiHeader.biWidth         =  width;
	dibInfo.bmiHeader.biHeight        =  -h;
	dibInfo.bmiHeader.biBitCount      =  32;
	dibInfo.bmiHeader.biClrUsed       =  0;
	dibInfo.bmiHeader.biClrImportant  =  0;
	dibInfo.bmiHeader.biCompression   =  BI_RGB;
	dibInfo.bmiHeader.biPlanes        =  1;
	dibInfo.bmiHeader.biSizeImage     =  w*h*4;
	dibInfo.bmiHeader.biXPelsPerMeter =  0;
	dibInfo.bmiHeader.biYPelsPerMeter =  0;

	bmp = CreateDIBSection(NULL, &dibInfo, DIB_RGB_COLORS, &bits, NULL, 0);

    if(bmp == NULL)
    {
        return;
    }

    // Copy pixels

    memcpy(bits, pixelData, width*height*4);
	SendMessage(this->hwnd, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) (HANDLE) bmp);

	this->hbitmap = bmp;

#endif

#ifdef MACOSX

	int w = width;
	int h = height;

	this->width  = width;
	this->height = height;

	// Create a CGimage and use HIImageViewCreate
	
    CGDataProviderRef provider;
    CGColorSpaceRef   colorspace;
    CGImageRef        image;
    
    colorspace = CGColorSpaceCreateDeviceRGB();
    provider   = CGDataProviderCreateWithData(NULL, pixelData, w*h*4, NULL);
    
    image = CGImageCreate(w, h, 8, 32, w*4, 
                          colorspace,
                          kCGImageAlphaFirst,
                          provider,
                          NULL, false,
                          kCGRenderingIntentDefault);
    
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorspace);	
	
	if(this->controlRef == NULL)
	{
		HIImageViewCreate(image, &this->controlRef);
	
		HIViewRef mContent;
	
		HIViewFindByID(HIViewGetRoot(GetWindowFromPort(parentView->getWindowHandle())),
					kHIViewWindowContentID, &mContent);
	
		HIViewAddSubview(mContent, this->controlRef);
		HIViewSetVisible(this->controlRef, true);
	}
	else
	{
		HIImageViewSetImage((HIViewRef) this->controlRef, image);
	}

	if(this->controlRef != nil)
	{
		PKVariant *a  = this->get(PKImageView::ALPHA_UINT8_PROPERTY);
		uint8_t alpha = PKVALUE_UINT8(a);
		
#ifdef OSX_10_5
		CGFloat alph = (CGFloat) alpha / 255.0;
#else
		float alph = (float) alpha / 255.0;
#endif
		
		HIImageViewSetAlpha(this->controlRef, alph);
	}
	
	CGImageRelease(image);

#endif
}

// Image resource input
void PKImageView::loadImageFromResource(std::wstring id)
{
	if(this->bitmap != NULL)
	{
		delete this->bitmap;
		this->bitmap = NULL;
	}

	this->bitmap = PKImageLoader::loadBitmapFromResource(id);

	if(this->bitmap != NULL)
	{
		this->setImageBuffer((void *) bitmap->getPixels(),
							 bitmap->getWidth(),
							 bitmap->getHeight());
	}
}

// Image file input
void PKImageView::loadImageFromFilename(std::wstring fn)
{
	if(this->bitmap != NULL)
	{
		delete this->bitmap;
		this->bitmap = NULL;
	}

	this->bitmap = PKImageLoader::loadBitmapFromFilename(fn);

	if(bitmap != NULL)
	{
		this->setImageBuffer((void *) bitmap->getPixels(),
							 bitmap->getWidth(),
							 bitmap->getHeight());
	}
}


#ifdef WIN32

void PKImageView::alphaDraw(HDC dc, RECT bounds, HBRUSH brush)
{
    PKVariant *a = this->get(PKImageView::ALPHA_UINT8_PROPERTY);
    uint8_t alpha = PKVALUE_UINT8(a);

	HDC cdc = CreateCompatibleDC(dc);

    if(cdc == NULL)
    {
        return;
    }

	SetBkMode(dc,  TRANSPARENT);
	SetBkMode(cdc, TRANSPARENT);

	HBITMAP old = (HBITMAP) SelectObject(cdc, this->hbitmap);

	if(brush != NULL)
	{
		FillRect(dc, &bounds, brush);
	}

	BLENDFUNCTION blendfunction = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

	BOOL result = AlphaBlend(dc, 
			                 bounds.left, 
			                 bounds.top, 
			                 this->width, 
			                 this->height, 
			                 cdc, 0, 0, this->width, this->height,
			                 blendfunction);

    if(!result)
    {
        // ERROR ?
    }

    SelectObject(cdc, old);
	DeleteDC(cdc);
}

#endif

uint32_t PKImageView::getMinimumWidth()
{
    if(!this->built)
    {
        this->build();
    }

	return this->width;
}

uint32_t PKImageView::getMinimumHeight()
{
    if(!this->built)
    {
        this->build();
    }

    return this->height;
}

