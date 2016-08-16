//
// PKVectorControlRoot
//

#include "PKVectorControlRoot.h"
#include "PKVectorEngine.h"
#include "PKVectorControl.h"
#include "PKLayout.h"
#include "PKDialog.h"
#include "PKTime.h"
#include "PKEditableVectorLabel.h"

//
// Properties
//

std::string PKVectorControlRoot::DOUBLE_BUFFER_BOOL_PROPERTY = "doubleBuffer";
std::string PKVectorControlRoot::BACK_COLOR_COLOR_PROPERTY   = "backColor";
std::string PKVectorControlRoot::ALPHA_DOUBLE_PROPERTY       = "alpha";

//
// PKVectorControlRoot
//

PKVectorControlRoot::PKVectorControlRoot()
: animThread(this)
{
	PKColor back = PKColorCreate(255, 255, 255, 255);

	PKOBJECT_ADD_BOOL_PROPERTY(doubleBuffer, false);
	PKOBJECT_ADD_COLOR_PROPERTY(backColor, back);
	PKOBJECT_ADD_DOUBLE_PROPERTY(alpha, 1.0);

#ifdef WIN32
	this->hwnd = NULL;
#endif

#ifdef WIN32
	this->mdc = NULL;
	this->bmp = NULL;
	this->invalidated = true;
#endif
	
	this->currentControl = NULL;
    this->focusControl = NULL;
	this->skipFocus = false;
	this->downControl = NULL;
    this->downHit = false;

    this->lastFocusControl = NULL;
    this->topLevelControl  = NULL;
}

PKVectorControlRoot::~PKVectorControlRoot()
{
#ifdef WIN32

	if(this->bmp != NULL)
	{
		DeleteObject(this->bmp);
		this->bmp = NULL;
	}

	if(this->mdc != NULL)
	{
		DeleteDC(this->mdc);
		this->mdc = NULL;
	}
#endif
}

void PKVectorControlRootNoWidgetWalk(PKObject *child, void *context)
{
    PKControl *ctrl = dynamic_cast<PKControl*>(child);
	PKVectorControlRoot *root = (PKVectorControlRoot *) context;

    if(ctrl != NULL)
    {
        ctrl->setNoWidget(true);
    }
	
	PKVectorControl *cvector = dynamic_cast<PKVectorControl*>(child);
	
	if(cvector != NULL)
	{
		cvector->setControlRoot(root);
	}
}

void PKVectorControlRoot::build()
{
    PKCustomControl::build();

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
            this->walkChildren(PKVectorControlRootNoWidgetWalk, this);

			root->build();
#ifdef LINUX
            uint32_t mw = root->getMinimumWidth();
            uint32_t mh = root->getMinimumHeight();

            root->resize(mw, mh);
            this->setDesiredWidgetSize(mw, mh);
            
            // Hook up signals
            
            g_signal_connect(G_OBJECT(this->widget), "size-allocate",
                             G_CALLBACK(PKVectorControlRoot::gtk_size_allocation_callback), this);  
                        
#endif	
		}
	}	
}

void PKVectorControlRoot::initialize(PKDialog *dialog)
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->initialize(dialog);
		}
	}
	
	PKCustomControl::initialize(dialog);
}

void PKVectorControlRoot::destroy()
{
	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->destroy();
            root = NULL;
		}
	}
    this->clearChildren();

    PKCustomControl::destroy();
}

void PKVectorControlRoot::setTopLevelControl(PKVectorControl *child)
{
    this->topLevelControl = child;
}

void PKVectorControlRoot::queueVectorControlAnimation(PKVectorAnimation *anim)
{
	this->animMutex.wait();

	this->anims.push_back(anim);

	if(!this->animThread.isRunning())
	{
		this->animThread.start();
	}

	this->animMutex.release();
}

void PKVectorControlRoot::cancelAllAnimationsForChild(PKVectorControl *ctrl)
{ 
    // Stop all animations first

    if(this->animThread.isRunning())
	{
		this->animThread.stop();
	}
   
    // Now cleanup

    this->animMutex.wait();

    std::vector<PKVectorAnimation *> new_anims;

    for(uint32_t i=0; i<this->anims.size(); i++)
    {
        bool remove = false;

        PKVectorAnimation *anim = this->anims[i];

        if(anim != NULL)
        {
            uint32_t count = anim->getNumberOfAnimationBlocks();

            for(uint32_t n=0; n<count; n++)
            {
                PKVectorAnimationBlock *b = anim->getAnimationBlockAt(n);

                if(b != NULL)
                {
                    if(b->getVectorControl() == ctrl)
                    {
                        remove = true;
                        break;
                    }
                }
            }
        }

        if(remove)
        {
            delete anim;
        }
        else
        {
            new_anims.push_back(anim);
        }
    }

    this->anims = new_anims;

    // Restart if necessary

    if(this->anims.size() > 0)
    {
        this->animThread.start();
    }
    
    this->animMutex.release();
}


void PKVectorControlRoot::clearInputState()
{
	this->currentControl = NULL;
}

void PKVectorControlRoot::runThread(PKThread *thread)
{
	// Animation thread

	while(this->anims.size() > 0)
	{
		// Loop over the first animation 
		// in our vector queue variable

		bool     keepGoing  = true;
		double   startTime  = (double) PKTime::getTime() / (double) PKTime::getTimeScale();
		double   frame      = 0;
		uint32_t numBlocks  = this->anims[0]->getNumberOfAnimationBlocks();
		double   lastUpdate = 0;

		do
		{
			double nowTime = (double) PKTime::getTime() / (double) PKTime::getTimeScale();
			double current = nowTime - startTime;

			bool allDone = true;

			std::vector<PKRect> updateRects;

			for(uint32_t i=0; i<numBlocks; i++)
			{
				PKVectorAnimationBlock *block = this->anims[0]->getAnimationBlockAt(i);

				if(block != NULL)
				{
					double blockStart = block->getStartTime();
					double duration   = block->getEndTime() - block->getStartTime();

					if(current >= block->getStartTime())
					{
						// This block has started

						if(current < block->getEndTime())
						{
							// Still running, update
							allDone = false;

							if(current >= (lastUpdate+0.04)) // 25 img/sec
							{
								if(block->getAnimationDescriptor() && block->getVectorControl())
								{
									PKVectorAnimationDescriptor *desc = block->getAnimationDescriptor();

									if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_FADE)
									{
										PKVectorAnimationDescriptorFade *fade = dynamic_cast<PKVectorAnimationDescriptorFade *>(desc);

										// FADE
        
                                        PKRect rect;

										double total = fade->getAlphaEndValue() - fade->getAlphaStartValue();
										double alpha = fade->getAlphaStartValue() + (current-blockStart)*total/duration;

										PKVariantDouble newAlpha(alpha);
										block->getVectorControl()->set(PKVectorControl::ALPHA_DOUBLE_PROPERTY, &newAlpha);

										rect.x = (block->getVectorControl()->getX() - this->getX());
										rect.y = (block->getVectorControl()->getY() - this->getY());
										rect.w =  block->getVectorControl()->getW();
										rect.h =  block->getVectorControl()->getH();

                                        updateRects.push_back(rect);
									}

									if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_FADE_TO)
									{
										PKVectorAnimationDescriptorFadeTo *fade = dynamic_cast<PKVectorAnimationDescriptorFadeTo *>(desc);

										// FADE TO
        
                                        PKRect rect;

										double total = fade->getAlphaEndValue() - fade->getAlphaStartValue();
										double alpha = fade->getAlphaStartValue() + (current-blockStart)*total/duration;

										PKVariantDouble newAlpha(alpha);
										block->getVectorControl()->set(PKVectorControl::ALPHA_DOUBLE_PROPERTY, &newAlpha);

										rect.x = (block->getVectorControl()->getX() - this->getX());
										rect.y = (block->getVectorControl()->getY() - this->getY());
										rect.w =  block->getVectorControl()->getW();
										rect.h =  block->getVectorControl()->getH();

                                        updateRects.push_back(rect);
									}

                                    if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_SLIDE)
									{
										PKVectorAnimationDescriptorSlide *slide = dynamic_cast<PKVectorAnimationDescriptorSlide *>(desc);

										// FADE
										
										double xSize = slide->getXEnd() - slide->getXStart();
										double ySize = slide->getYEnd() - slide->getYStart();

										double newX = slide->getXStart() + (current-blockStart)*xSize/duration;
										double newY = slide->getYStart() + (current-blockStart)*ySize/duration;

        								PKRect rect;
										PKRect frame = PKVALUE_RECT(block->getVectorControl()->get(PKControl::FRAME_RECT_PROPERTY));

										int old_x = frame.x;
										int old_y = frame.y;

										frame.x = newX;
										frame.y = newY;

										PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
										block->getVectorControl()->set(PKControl::FRAME_RECT_PROPERTY, &newFrame);
									
										rect.x = pk_min(old_x, newX);
										rect.y = pk_min(old_y, newY);
										rect.w = block->getVectorControl()->getW() + abs(old_x - newX) + 1;
										rect.h = block->getVectorControl()->getH() + abs(old_y - newY) + 1;

										updateRects.push_back(rect);
									}
								}								
							}
						}
						else
						{
							// Over, make sure it's closed
						
							if(!block->getDone())
							{
								if(block->getAnimationDescriptor() && block->getVectorControl())
								{
									PKVectorAnimationDescriptor *desc = block->getAnimationDescriptor();

									if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_FADE)
									{
										PKVectorAnimationDescriptorFade *fade = dynamic_cast<PKVectorAnimationDescriptorFade *>(desc);

        								PKRect rect;
										double alpha = fade->getAlphaEndValue();

										PKVariantDouble newAlpha(alpha);
										block->getVectorControl()->set(PKVectorControl::ALPHA_DOUBLE_PROPERTY, &newAlpha);

										rect.x = (block->getVectorControl()->getX() - this->getX());
										rect.y = (block->getVectorControl()->getY() - this->getY());
										rect.w =  block->getVectorControl()->getW();
										rect.h =  block->getVectorControl()->getH();

                                        updateRects.push_back(rect);
									}

									if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_FADE_TO)
									{
										PKVectorAnimationDescriptorFadeTo *fade = dynamic_cast<PKVectorAnimationDescriptorFadeTo *>(desc);

        								PKRect rect;
										double alpha = fade->getAlphaEndValue();

										PKVariantDouble newAlpha(alpha);
										block->getVectorControl()->set(PKVectorControl::ALPHA_DOUBLE_PROPERTY, &newAlpha);

										rect.x = (block->getVectorControl()->getX() - this->getX());
										rect.y = (block->getVectorControl()->getY() - this->getY());
										rect.w =  block->getVectorControl()->getW();
										rect.h =  block->getVectorControl()->getH();

                                        updateRects.push_back(rect);
									}

                                    if(desc->getAnimationType() == PK_VECTOR_ANIMATION_TYPE_SLIDE)
									{
										PKVectorAnimationDescriptorSlide *slide = dynamic_cast<PKVectorAnimationDescriptorSlide *>(desc);

										// FADE
										
										double newX = slide->getXEnd();
										double newY = slide->getYEnd();

        								PKRect rect;
										PKRect frame = PKVALUE_RECT(block->getVectorControl()->get(PKControl::FRAME_RECT_PROPERTY));

										int old_x = frame.x;
										int old_y = frame.y;

										frame.x = newX;
										frame.y = newY;

										PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
										block->getVectorControl()->set(PKControl::FRAME_RECT_PROPERTY, &newFrame);
									
										rect.x = pk_min(old_x, newX);
										rect.y = pk_min(old_y, newY);
										rect.w = block->getVectorControl()->getW() + abs(old_x - newX) + 1;
										rect.h = block->getVectorControl()->getH() + abs(old_y - newY) + 1;

										updateRects.push_back(rect);
									}

									block->setDone(true);
								}
								
								lastUpdate += 0.04;
							}
						}
					}
					else
					{
						// Not started yet
						allDone = false;
					}
				}
			}

			if(updateRects.size())
			{
				PKRect combined;

				uint32_t left   = this->getW();
				uint32_t top    = this->getH();
				uint32_t right  = 0;
				uint32_t bottom = 0;

				for(uint32_t n=0; n<updateRects.size(); n++)
				{	
					if(updateRects[n].x < left)
					{
						left = updateRects[n].x;
					}

					if(updateRects[n].y < top)
					{
						top = updateRects[n].y;
					}

					if((updateRects[n].x + updateRects[n].w) > right)
					{
						right = (updateRects[n].x + updateRects[n].w);
					}

					if((updateRects[n].y + updateRects[n].h) > bottom)
					{
						bottom = (updateRects[n].y + updateRects[n].h);
					}
				}

#ifdef MACOSX 
				combined.x = left - this->getScrollOriginX();
				combined.y = top  - this->getScrollOriginY();
#else
				combined.x = left;
				combined.y = top;
#endif
				combined.w = right - left;
				combined.h = bottom - top;

				this->invalidateRect(combined);
				lastUpdate += 0.04;
			}

			if(allDone)
			{
				keepGoing = false;
			}

			// Should be short enough
			PKTime::sleep(5);
		}
		while(keepGoing);

		// See if we have more animations to run

		this->animMutex.wait();

		PKVectorAnimation *anim = this->anims[0];

		// Signal that this animation is over
		// if it has a completion callback

		if(anim->getCompletionHandler())
		{
			PKSafeEvents::postSafeEvent(anim->getCompletionHandler(), 
										anim->getCompletionEventName(), 
										anim->getCompletionContext());
		}

		this->anims.erase(this->anims.begin());
		delete anim;

		this->animMutex.release();
	}
}

// From PKControl

void PKVectorControlRoot::move(int32_t x, 
					           int32_t y)
{
	PKVariant *fr = this->get(FRAME_RECT_PROPERTY);
	PKRect frame  = PKVALUE_RECT(fr);

	int32_t diffX = x - frame.x;
	int32_t diffY = y - frame.y;

	frame.x = x;
	frame.y = y;

	PKVariantRect newFrame(frame.x, frame.y, frame.w, frame.h);
	this->set(FRAME_RECT_PROPERTY, &newFrame);

#ifdef LINUX
    return;
#endif

    // Move children by the same offset

    for(uint32_t i=0; i < this->getChildrenCount(); i++)
    {
        PKControl *child = dynamic_cast<PKControl*>(this->getChildAt(i));

        if(child)
        {
            child->moveBy(diffX, diffY);
        }
    }
}

void PKVectorControlRoot::relayout()
{
    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
            uint32_t w = root->getW();
            uint32_t h = root->getH();

			root->resize(w, h);
		}
	}
}

void PKVectorControlRoot::resize(uint32_t w, 
					             uint32_t h)
{
	PKCustomControl::resize(w, h);

#ifdef WIN32

	if(this->bmp != NULL)
	{
		DeleteObject(this->bmp);
		this->bmp = NULL;
	}

	if(this->mdc != NULL)
	{
		DeleteDC(this->mdc);
		this->mdc = NULL;
	}

	if(this->hwnd)
	{
		HDC dc = GetDC(this->hwnd);

		this->mdc = CreateCompatibleDC(dc);
		this->bmp = CreateCompatibleBitmap(dc, w, h);
	
		this->invalidated = true;
	}

#endif

#ifndef LINUX

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->resize(w, h);
		}
	}

#endif
}

void PKVectorControlRoot::show()
{
	PKControl::show();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->show();
		}
	}
}

void PKVectorControlRoot::hide()
{
	PKControl::hide();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->hide();			
		}
	}
}

void PKVectorControlRoot::enable()
{
	PKControl::enable();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->enable();
		}
	}
}

void PKVectorControlRoot::disable()
{
	PKControl::disable();

#ifdef LINUX
    return;
#endif

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			root->disable();
		}
	}
}



uint32_t PKVectorControlRoot::getMinimumWidth()
{
    if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
    {
        return this->getW();
    }

    if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumWidth());
		}
	}

	return 0;
}

uint32_t PKVectorControlRoot::getMinimumHeight()
{
    if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
    {
        return this->getH();
    }

	if(this->getChildrenCount() == 1)
	{
		PKControl *root = dynamic_cast<PKControl*>(this->getChildAt(0));

		if(root)
		{
			return (root->getMinimumHeight());
		}
	}

	return 0;
}

void PKVectorControlRoot::invalidate(bool recurse)
{
#ifdef WIN32
	this->invalidated = true;
#endif

	PKCustomControl::invalidate(false);
}

void PKVectorControlRoot::invalidateRect(PKRect client)
{
#ifdef WIN32
	this->invalidated = true;
#endif

	PKCustomControl::invalidateRect(client);
}

//
// Main draw method (Internal)
//

void PKVectorControlRootDrawChild(PKVectorControlRoot *root, PKVectorControl *ctrl)
{
    PKVariant *h = ctrl->get(PKControl::HIDDEN_BOOL_PROPERTY);
    bool hidden  = PKVALUE_BOOL(h);

    if(!hidden)
    {
		PKVariant *a = ctrl->get(PKVectorControl::ALPHA_DOUBLE_PROPERTY);
		double alpha = PKVALUE_DOUBLE(a);

		if(alpha == 1.0)
		{
    		ctrl->drawVectorInternal(root->currentEngine, 
	    							 ctrl->getX() - root->getX(), 
		    						 ctrl->getY() - root->getY());
		}
		else
		{
            // TODO: this isn't very optimized, we shouldn't
            // create a DIB for each frame but keep the DIB
            // and graphic DC around for as long as needed

			// We must double buffer & alpha blit
			int w = ctrl->getW();
			int h = ctrl->getH();

#ifdef WIN32
			HDC alphaDC  = CreateCompatibleDC(root->currentDevice);

			HBITMAP bmp  = NULL;
			void   *bits = NULL;

			BITMAPINFO dibInfo;
			memset(&dibInfo, 0, sizeof(BITMAPINFO));

			dibInfo.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
			dibInfo.bmiHeader.biWidth         =  w;
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

			memset(bits, 0, w*h*4);

			HANDLE  aold = SelectObject(alphaDC, bmp);

			PKVectorEngine engine2;

			engine2.startWithDevice(alphaDC);
			engine2.setDimensions(w, h);

    		ctrl->drawVectorInternal(&engine2, 0, 0);

			engine2.end();

			root->currentEngine->drawImage(bits, w, h, 
										   ctrl->getX() - root->getX(), 
										   ctrl->getY() - root->getY(), alpha);

			DeleteObject(bmp);
			DeleteDC(alphaDC);
#endif

#ifdef MACOSX

			CGColorSpaceRef   colorspace;

			char *buffer = new char[w*h*4];
			memset(buffer, 0, w*h*4);

			colorspace = CGColorSpaceCreateDeviceRGB();

			CGContextRef imgContext = CGBitmapContextCreate(buffer, w, h, 8, w*4, 
															colorspace, kCGImageAlphaPremultipliedFirst);
			
			PKVectorEngine engine2;
			
			engine2.startWithCGContext(imgContext);
			engine2.setDimensions(w, h);
			
			CGContextTranslateCTM(imgContext, 0, h-1);
			CGContextScaleCTM(imgContext, 1.0, -1.0);
			
			ctrl->drawVectorInternal(&engine2, 0, 0);
			
			engine2.end();
			
			CGImageRef img = CGBitmapContextCreateImage(imgContext);
			
			root->currentEngine->drawCGImage(img, ctrl->getX() - root->getX(), 
										     ctrl->getY() - root->getY()+1, alpha);
			
			CGImageRelease(img);
			CGContextRelease(imgContext);
			
			delete [] buffer;

#endif

#ifdef LINUX
				// TODO
#endif
        }
    }
}

void PKVectorControlRootChildrenWalk(PKObject *child, void *context)
{
	PKVectorControlRoot *root = (PKVectorControlRoot*) context;

	if(root == NULL)
	{
		return;
	}

	PKVectorControl *ctrl = dynamic_cast<PKVectorControl*>(child);

	if(ctrl)
	{
        if(ctrl == root->topLevelControl)
        {
            // Skip this control, it'll be drawn last
            return;
        }

        PKVectorControlRootDrawChild(root, ctrl);
	}
}


void PKVectorControlRoot::drawVectorInternal(PKVectorEngine *engine)
{
	this->currentEngine = engine;

	// Draw children
	this->walkChildren(PKVectorControlRootChildrenWalk, (void*) this);

    // Now draw top-level if any
    if(this->topLevelControl)
    {
        PKVectorControlRootDrawChild(this, this->topLevelControl);
    }
}

void PKVectorControlRoot::drawIntoVectorEngine(PKVectorEngine *engine)
{
	this->drawVectorInternal(engine);
}

// 
// From PKCustomControl
//

#ifdef WIN32

void PKVectorControlRoot::drawWin32(PKDrawingDevice device, 
								    PKRect          rect)
{
	PKVectorEngine engine;

	this->currentDevice = device;

	// Check if we are double buffer

	PKVariant *d = this->get(PKVectorControlRoot::DOUBLE_BUFFER_BOOL_PROPERTY);
	bool db = PKVALUE_BOOL(d);

	PKVariant *al = this->get(PKVectorControlRoot::ALPHA_DOUBLE_PROPERTY);
	double alpha  = PKVALUE_DOUBLE(al);

	PKVariant *back = this->get(PKVectorControlRoot::BACK_COLOR_COLOR_PROPERTY);
	PKColor b = PKVALUE_COLOR(back);

	if(db)
	{
		uint32_t w = this->getW();
		uint32_t h = this->getH();

		if(alpha == 1)
		{
			HANDLE hold = SelectObject(mdc, bmp);

			if(this->invalidated)
			{
				engine.startWithDevice(mdc);
				engine.setDimensions(w, h);
				engine.clipToRect(rect);

				engine.fillRect(-1, -1, w+1, h+1, b);

				this->currentBuffer = mdc;
				this->drawVectorInternal(&engine);

				engine.end();
				this->invalidated = false;
			}

			BitBlt(device, rect.x, rect.y, rect.w, rect.h, 
				   mdc, rect.x, rect.y, SRCCOPY);

			SelectObject(mdc, hold);
		}
		else
		{
			HDC alphaDC  = CreateCompatibleDC(device);
			HBITMAP abmp = CreateCompatibleBitmap(device, w, h);
			HANDLE  aold = SelectObject(alphaDC, abmp);

			if(this->invalidated)
			{
				engine.startWithDevice(alphaDC);
				engine.setDimensions(w, h);
				engine.clipToRect(rect);

				engine.fillRect(-1, -1, w+1, h+1, b);

				this->currentBuffer = alphaDC;
				this->drawVectorInternal(&engine);

				engine.end();
				this->invalidated = false;
			}

			HANDLE hold = SelectObject(mdc, bmp);

			BYTE al = (BYTE) pk_min(255, (int)(alpha*255.0));
			BLENDFUNCTION blend = { AC_SRC_OVER, 0, al, AC_SRC_ALPHA };

			HBRUSH brush = CreateSolidBrush(RGB(b.r, b.g, b.b));
			RECT r; r.left = r.top = 0;
			r.right = w; r.bottom = h;
			FillRect(mdc, &r, brush);
			DeleteObject(brush);

			AlphaBlend(mdc, 0, 0, w, h, alphaDC, 0, 0, w, h, blend);

			DeleteObject(abmp);
			DeleteDC(alphaDC);

			BitBlt(device, rect.x, rect.y, rect.w, rect.h, 
				   mdc, rect.x, rect.y, SRCCOPY);

			SelectObject(mdc, hold);
		}
	}
	else
	{
		engine.startWithDevice(device);
		engine.setDimensions(this->getW(), this->getH());
		engine.clipToRect(rect);
		this->drawVectorInternal(&engine);
		engine.end();
	}
}

#endif

#ifdef LINUX

void PKVectorControlRoot::drawLinux(GtkWidget   *widget,
                                    GdkDrawable *drawable,
                                    GdkGC       *gc,
                                    PKRect       rect)
{
	PKVariant *back = this->get(PKVectorControlRoot::BACK_COLOR_COLOR_PROPERTY);
	PKColor b = PKVALUE_COLOR(back);

    GtkAllocation all;
    gtk_widget_get_allocation(widget, &all);

	PKVectorEngine engine;
	engine.startWithDevice(drawable);
	engine.setDimensions(all.width, all.height);
	engine.clipToRect(rect);
	engine.fillRect(0, 0, all.width, all.height, b);
	this->drawVectorInternal(&engine);
	engine.end();
}

#endif


#ifdef MACOSX

void PKVectorControlRoot::drawMacOSX(RgnHandle	 inLimitRgn,
								     CGContextRef inContext)
{
	PKVariant *al = this->get(PKVectorControlRoot::ALPHA_DOUBLE_PROPERTY);
	double alpha  = PKVALUE_DOUBLE(al);

	PKVariant *back = this->get(PKVectorControlRoot::BACK_COLOR_COLOR_PROPERTY);
	PKColor b = PKVALUE_COLOR(back);

	// Set Alpha
	CGContextSetAlpha(inContext, alpha);
	
	// Draw background
	CGRect  rect;
	float   bkg[4];
	
	bkg[0] = b.r/255.0; bkg[1] = b.g/255.0; bkg[2] = b.b/255.0; bkg[3] = 1.0;
	
    CGColorSpaceRef   colorspace;
    colorspace = CGColorSpaceCreateDeviceRGB();

	CGContextSaveGState(inContext);	
	CGContextSetFillColorSpace(inContext, colorspace);
	
	rect.origin.x = 0;
	rect.origin.y = 0;
	rect.size.width  = this->getW();
	rect.size.height = this->getH();	
	CGContextSetFillColor(inContext, bkg);
	CGContextFillRect(inContext, rect);
	
	// Draw vector
	
	PKVectorEngine engine;
	engine.startWithCGContext(inContext);
	engine.setDimensions(this->getW(), this->getH());
	this->drawVectorInternal(&engine);
	engine.end();
	
	CGContextRestoreGState(inContext);	
    CGColorSpaceRelease(colorspace);
}

#endif

void PKVectorControlRootSetFocusWalk(PKObject *child, void *context)
{
	PKVectorControlRoot *root = (PKVectorControlRoot *) context;
	
	if(root == NULL)
	{
		return;
	}
	
	if(child == root)
	{
	    return;
	}
	
	if(root->focusSet)
	{
		return;
	}
	
	PKVectorControl *vectorControl = dynamic_cast<PKVectorControl *>(child);
	
	if(vectorControl)
	{
		PKVariant *h = vectorControl->get(PKControl::HIDDEN_BOOL_PROPERTY);
		bool hidden  = PKVALUE_BOOL(h);

		if(!hidden && vectorControl->wantsKeyboardFocus())
		{
			vectorControl->focusAcquired(root->focusDirection);
			root->focusControl = vectorControl;
			root->focusSet = true;
		}
	}
}

void PKVectorControlRoot::doCut()
{
    if(this->focusControl)
    {
        this->focusControl->doCut();
    }
}

void PKVectorControlRoot::doCopy()
{
    if(this->focusControl)
    {
        this->focusControl->doCopy();
    }
}

void PKVectorControlRoot::doPaste()
{
    if(this->focusControl)
    {
        this->focusControl->doPaste();
    }
}

void PKVectorControlRoot::doSelectAll()
{
    if(this->focusControl)
    {
        this->focusControl->doSelectAll();
    }
}

bool PKVectorControlRoot::hasSelection()
{
    if(this->focusControl)
    {
        return this->focusControl->hasSelection();
    }

    return false;
}

void PKVectorControlRoot::commitChanges()
{
    if(this->focusControl != NULL)
    {
        PKEditableVectorLabel *l = dynamic_cast<PKEditableVectorLabel*>(this->focusControl);

        if(l != NULL)
        {
            l->commitChanges();
        }
    }
}


void PKVectorControlRoot::focusAcquired(PKDirection direction)
{
	PKCustomControl::focusAcquired(direction);

    if(direction == PK_DIR_NONE)
    {
        // This happens in LINUX when
        // a menu or popup menu is brought
        // up. We should remember which control
        // had the focus and reset to it
        // quietly
        
        if(this->lastFocusControl)
        {
            this->focusControl = this->lastFocusControl;
            this->focusControl->focusAcquired(direction);
            return;
        }        
    }


	if(direction == PK_DIR_FORWARD)
	{
	    // give the focus to the first 
	    // of our child control that okays
	
	    if(this->skipFocus)
	    {
		    this->skipFocus = false;
		    return;
	    }
	
	    this->focusSet = false;
	    this->walkChildren(PKVectorControlRootSetFocusWalk, this);	
	}
	
	if(direction == PK_DIR_BACKWARD)
	{
	    // give the focus to the last 
	    // of our child control that okays
	    
	    if(this->skipFocus)
	    {
		    this->skipFocus = false;
		    return;
	    }
	
	    this->focusSet = false;
	    this->walkChildrenBackwards(PKVectorControlRootSetFocusWalk, this);		    
    }	
}

void PKVectorControlRoot::focusLost()
{
	PKCustomControl::focusLost();

	// See if we need to update our own
	
    if(this->focusControl)
    {
		this->focusControl->focusLost();
		
		this->lastFocusControl = focusControl;
		this->focusControl = NULL;
	}	
}

void PKVectorControlRootSetNextFocusWalk(PKObject *child, void *context)
{
	PKVectorControlRoot *root = (PKVectorControlRoot *) context;
	
	if(root == NULL)
	{
		return;
	}

	if(child == root)
	{
	    return;
	}

	if(root->focusSet)
	{
		return;
	}
	
	PKVectorControl *vCtrl = dynamic_cast<PKVectorControl *>(child);
	
	if(vCtrl)
	{
		if(root->focusControl == vCtrl)
		{
			root->nextOne = true;
		}
		else
		{
			if(root->nextOne)
			{
				PKVariant *h = vCtrl->get(PKControl::HIDDEN_BOOL_PROPERTY);
				bool hidden  = PKVALUE_BOOL(h);
				
				if(!hidden && vCtrl->wantsKeyboardFocus())
				{
					root->nextFocusControl = vCtrl;
					root->focusSet = true;
				}
			}
		}
	}
}

bool PKVectorControlRoot::moveFocusToNextControlPart()
{
	if(this->focusControl)
	{
		this->focusSet = false;
		this->nextOne  = false;
		
		this->nextFocusControl = NULL;
		this->walkChildren(PKVectorControlRootSetNextFocusWalk, this);
		
		if(this->nextFocusControl != NULL)
		{
			this->focusControl->focusLost();
			this->focusControl = this->nextFocusControl;
			this->focusControl->focusAcquired(this->focusDirection);
			
			return true;
		}
	}
	
	return false;
}

void PKVectorControlRootSetPreviousFocusWalk(PKObject *child, void *context)
{
	PKVectorControlRoot *root = (PKVectorControlRoot *) context;
	
	if(root == NULL)
	{
		return;
	}

	if(child == root)
	{
	    return;
	}

	if(root->focusSet)
	{
		return;
	}
	
	PKVectorControl *vCtrl = dynamic_cast<PKVectorControl *>(child);
	
	if(vCtrl)
	{
		if(root->focusControl == vCtrl)
		{
			root->nextOne = true;
		}
		else
		{
			if(root->nextOne)
			{
				PKVariant *h = vCtrl->get(PKControl::HIDDEN_BOOL_PROPERTY);
				bool hidden  = PKVALUE_BOOL(h);
				
				if(!hidden && vCtrl->wantsKeyboardFocus())
				{
					root->nextFocusControl = vCtrl;
					root->focusSet = true;
				}
			}
		}
	}
}


bool PKVectorControlRoot::moveFocusToPreviousControlPart()
{
	if(this->focusControl)
	{
		this->focusSet = false;
		this->nextOne  = false;
		
		this->nextFocusControl = NULL;
		this->walkChildrenBackwards(PKVectorControlRootSetPreviousFocusWalk, this);
		
		if(this->nextFocusControl != NULL)
		{
			this->focusControl->focusLost();
			this->focusControl = this->nextFocusControl;
			this->focusControl->focusAcquired(this->focusDirection);
			
			return true;
		}
	}
	
	return false;
}

void PKVectorControlRoot::mouseDown(PKButtonType button,
									int x,
									int y,
									unsigned int mod)

{
	PKCustomControl::mouseDown(button, x, y, mod);
	
	if(this->dialog == NULL)
	{
		return;
	}
	
	// Check if it hit one of our children
	
	PKControl *child = this->findChildControlAt(x, y);
	
	if(child && (child != this))
	{
		PKVectorControl *custom = dynamic_cast<PKVectorControl *> (child);
		
		if(custom)
		{
            if(!custom->hasFocus() && custom->wantsKeyboardFocus())
            {
                if(this->focusControl)
                {
                    this->focusControl->focusLost();
                    this->focusControl = NULL;
                }

				// Set focus to new control
				
                custom->focusAcquired(PK_DIR_FORWARD);
                this->focusControl = custom;
				
				// We need also to set the real
				// system focus to ourselves
				
				if(!this->hasFocus())
				{
					PKVariant   *i = this->get(PKControl::ID_STRING_PROPERTY);
					std::string id = PKVALUE_STRING(i);
					this->skipFocus = true;
					this->dialog->setFocusToControl(id);
				}
            }

			int cX = custom->getX();
			int cY = custom->getY();
			
			int tX = this->getX();
			int tY = this->getX();
			
			custom->mouseDown(button, x-(cX-tX), y-(cY-tY), mod);
			this->downControl = custom;
		}
        else
        {
            if(this->focusControl)
            {
                this->focusControl->focusLost();
                this->focusControl = NULL;
            }

            // Signal we're being clicked
            this->downHit = true;
        }
	}
    else
    {
        if(this->focusControl)
        {
            this->focusControl->focusLost();
            this->focusControl = NULL;
        }

        // Signal we're being clicked
        this->downHit = true;
    }
}

void PKVectorControlRoot::mouseUp(PKButtonType button,
								  int x,
								  int y,
								  unsigned int mod)
{
	PKCustomControl::mouseUp(button, x, y, mod);

	if(this->downControl != NULL)
	{
		int cX = this->downControl->getX();
		int cY = this->downControl->getY();
		
		int tX = this->getX();

		int tY = this->getX();

		this->downControl->mouseUp(button, x-(cX-tX), y-(cY-tY), mod);
		this->downControl = NULL;
	}
	
	// Check if it hit one of our children

	PKControl *child = this->findChildControlAt(x, y);
	
	if(child && (child != this))
	{
		PKCustomControl *custom = dynamic_cast<PKCustomControl *> (child);
		
		if(custom)
		{
			int cX = custom->getX();
			int cY = custom->getY();
			
			int tX = this->getX();
			int tY = this->getX();

			custom->mouseUp(button, x-(cX-tX), y-(cY-tY), mod);
		}
        else // Assume it's us being clicked
        {
            if(this->downHit && this->getDialog())
            {
                PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
                std::string sid = PKVALUE_STRING(id);

                this->getDialog()->customControlClicked(PK_BUTTON_LEFT, sid, this, "");
            }
        }
	}
    else
    {
        if(this->downHit && this->getDialog())
        {
            PKVariant *id = this->get(PKControl::ID_STRING_PROPERTY);
            std::string sid = PKVALUE_STRING(id);

            this->getDialog()->customControlClicked(PK_BUTTON_LEFT, sid, this, "");
        }
    }

    this->downHit = false;
}

void PKVectorControlRoot::mouseDblClick(PKButtonType button,
						   int x,
						   int y,
						   unsigned int mod)
{
	PKCustomControl::mouseDblClick(button, x, y, mod);
	
	// Check if it hit one of our children
	
	PKControl *child = this->findChildControlAt(x, y);
	
	if(child && (child != this))
	{
		PKCustomControl *custom = dynamic_cast<PKCustomControl *> (child);
		
		if(custom)
		{
			int cX = custom->getX();
			int cY = custom->getY();
			
			int tX = this->getX();
			int tY = this->getX();

			custom->mouseDblClick(button, x-(cX-tX), y-(cY-tY), mod);
		}
	}
}

void PKVectorControlRoot::mouseMove(int x,
									int y,
									unsigned int mod)
{
	PKCustomControl::mouseMove(x, y, mod);
	
    if(this->dialog)
    {
        std::string id = PKVALUE_STRING(this->get(PKControl::ID_STRING_PROPERTY));
        this->dialog->vectorControlRootMouseMove(id, x, y);
    }
    
	PKControl *child = this->findChildControlAt(x, y);
	
	if(child && (child != this))
	{
		PKCustomControl *custom = dynamic_cast<PKCustomControl *> (child);
		
		if(custom)
		{			
			if(custom != this->currentControl)
			{
				if(this->currentControl != NULL)
				{
					this->currentControl->mouseLeave();
				}
				
				this->currentControl = custom;
			}
			
			if(this->currentControl)
			{
				int cX = this->currentControl->getX();
				int cY = this->currentControl->getY();

				int tX = this->getX();
				int tY = this->getX();
				
				this->currentControl->mouseMove(x-(cX-tX), y-(cY-tY), mod);
			}
		}
        else
        {
		    if(this->currentControl != NULL)
		    {
			    this->currentControl->mouseLeave();
			    this->currentControl = NULL;
		    }
        }
	}
    else
    {
	    if(this->currentControl != NULL)
	    {
		    this->currentControl->mouseLeave();
		    this->currentControl = NULL;
	    }
    }
}


void PKVectorControlRoot::mouseLeave()
{
	PKCustomControl::mouseLeave();
	
    if(this->dialog)
    {
        std::string id = PKVALUE_STRING(this->get(PKControl::ID_STRING_PROPERTY));
        this->dialog->vectorControlRootMouseLeave(id);
    }

	if(this->currentControl != NULL)
	{
		this->currentControl->mouseLeave();
		this->currentControl = NULL;
	}
}


void PKVectorControlRoot::mouseWheel(PKButtonType button,
									 int x,
									 int y,
									 unsigned int mod)
{
	PKCustomControl::mouseWheel(button, x, y, mod);
	
	// Check if it hit one of our children
	
	PKControl *child = this->findChildControlAt(x, y);
	
	if(child && (child != this))
	{
		PKCustomControl *custom = dynamic_cast<PKCustomControl *> (child);
		
		if(custom)
		{
			custom->mouseWheel(button, x, y, mod);
		}
	}	
}

bool PKVectorControlRoot::keyDown(unsigned int modifier,
					              PKKey        keyCode,
					              unsigned int virtualCode)
{
	bool result = PKCustomControl::keyDown(modifier, keyCode, virtualCode);
	
	// Check if it hit one of our children

    if(this->focusControl)
    {
        result = this->focusControl->keyDown(modifier, keyCode, virtualCode);
    }

    return result;
}

bool PKVectorControlRoot::keyUp(unsigned int modifier,
            				    PKKey        keyCode,
			            	    unsigned int virtualCode)
{
	bool result = PKCustomControl::keyUp(modifier, keyCode, virtualCode);
	
	// Check if it hit one of our children

    if(this->focusControl)
    {
        result = this->focusControl->keyUp(modifier, keyCode, virtualCode);
    }

    return result;
}

void PKVectorControlRoot::characterInput(std::wstring wstr)
{
	PKCustomControl::characterInput(wstr);
	
	// Check if it hit one of our children

    if(this->focusControl)
    {
        this->focusControl->characterInput(wstr);
    }
}

#ifdef LINUX

void PKVectorControlRoot::gtk_size_allocation_callback(GtkWidget     *widget,
                                                       GtkAllocation *allocation,
                                                       gpointer       user_data)
{
    PKVectorControlRoot *croot = (PKVectorControlRoot *) user_data;
    
    if(croot)
    {
	    if(croot->getChildrenCount() == 1)
	    {
		    PKControl *root = dynamic_cast<PKControl*>(croot->getChildAt(0));

		    if(root)
		    {
			    root->resize(allocation->width, 
			                 allocation->height);
		    }
	    }
    }    
}
                                                       
#endif


