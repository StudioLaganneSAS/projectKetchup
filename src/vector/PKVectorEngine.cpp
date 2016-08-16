//
//
//

#include "PKVectorEngine.h"
#include "PKStr.h"

#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif

//
// Statics
//

#ifdef WIN32
ULONG_PTR PKVectorEngine::token = NULL;
#endif

void PKVectorEngine::initialize()
{
#ifdef WIN32

	Gdiplus::GdiplusStartupInput startupStruct;
	memset(&startupStruct, 0, sizeof(startupStruct));
		
	startupStruct.GdiplusVersion = 1;

	Gdiplus::Status status =
		Gdiplus::GdiplusStartup(&PKVectorEngine::token,
								&startupStruct,
								NULL);

	if(status != Gdiplus::Ok)
	{
		PKVectorEngine::token = NULL;
	}

#endif
}

void PKVectorEngine::cleanup()
{
#ifdef WIN32
	Gdiplus::GdiplusShutdown(PKVectorEngine::token);
#endif
}

//
// PKVectorEngine
//

PKVectorEngine::PKVectorEngine()
{
#ifdef WIN32
	this->graphics = NULL;
#endif
	
#ifdef MACOSX
	this->ctx = NULL;
	this->ownCtx = true;
#endif
	
#ifdef LINUX
    this->gc = NULL;
    this->cr = NULL;
    this->drawable = NULL;
#endif		
	
	this->width  = 0;
	this->height = 0;

	this->alpha = 1.0;
}

PKVectorEngine::~PKVectorEngine()
{
#ifdef WIN32
	
	if(this->graphics != NULL)
	{
		this->end();
	}
	
#endif
	
#ifdef LINUX

    if(this->cr != NULL)
	{
		this->end();
	}

#endif	

#ifdef MACOSX
	
	if(this->ctx != NULL)
	{
		this->end();
	}
	
#endif
}

bool PKVectorEngine::startWithWindow(PKVWindowHandle window)
{
#ifdef WIN32
	
	if(PKVectorEngine::token == NULL)
	{
		PKVectorEngine::initialize();
	}

	if(PKVectorEngine::token == NULL)
	{
		// Not good
		return false;
	}

	if(this->graphics != NULL)
	{
		return false;
	}

	this->graphics = Gdiplus::Graphics::FromHWND(window);

	if(this->graphics)
	{
		this->graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		this->graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		this->graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
		this->graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		return true;
	}
#endif

#ifdef LINUX

    if(this->cr != NULL || window == NULL)
	{
		return false;
	}	

    this->drawable = GDK_DRAWABLE(window->window);
    this->cr = gdk_cairo_create(this->drawable);
    
    return true;

#endif
	
#ifdef MACOSX
	
	if(this->ctx != NULL)
	{
		return false;
	}	
	
	OSStatus err = CreateCGContextForPort(window, &this->ctx);
	
	if(err == noErr)
	{
		this->ownCtx = true;
		return true;
	}
	
#endif
	
	return false;
}

bool PKVectorEngine::startWithDevice(PKVDeviceHandle device)
{
#ifdef WIN32
	
	if(PKVectorEngine::token == NULL)
	{
		PKVectorEngine::initialize();
	}

	if(PKVectorEngine::token == NULL)
	{
		// Not good
		return false;
	}

	if(this->graphics != NULL)
	{
		return false;
	}

	this->graphics = Gdiplus::Graphics::FromHDC(device);

	if(this->graphics)
	{
		this->graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		this->graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		this->graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
		this->graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

		return true;
	}
#endif

#ifdef LINUX

    if(this->cr != NULL || device == NULL)
	{
		return false;
	}	

    this->drawable = GDK_DRAWABLE(device);
    this->cr = gdk_cairo_create(this->drawable);
    
    return true;

#endif
	
#ifdef MACOSX
	
	if(this->ctx != NULL)
	{
		return false;
	}	
	
	OSStatus err = CreateCGContextForPort(device, &this->ctx);
	
	if(err == noErr)
	{
		this->ownCtx = true;
		return true;
	}
	
#endif

	return false;
}

#ifdef MACOSX

bool PKVectorEngine::startWithCGContext(CGContextRef c)
{
	if(this->ctx != NULL)
	{
		return false;
	}
	
	this->ctx = c;
	this->ownCtx = false;
	
	return true;
}

#endif


void PKVectorEngine::end()
{
#ifdef WIN32
	
	if(this->graphics == NULL)
	{
		return;
	}

	this->graphics->Flush();
	delete this->graphics;
	this->graphics = NULL;
#endif
	
#ifdef LINUX

    if(this->cr == NULL)
	{
		return;
	}

    cairo_destroy(this->cr);
    this->cr = NULL;    

#endif	
	
#ifdef MACOSX
	
	if(this->ctx)
	{
		if(this->ownCtx)
		{
			CGContextRelease(this->ctx);
		}

		this->ctx = NULL;
	}
	
#endif
}

//
// Setup
// 

void PKVectorEngine::setDimensions(uint32_t w, uint32_t h)
{
	this->width  = w;
	this->height = h;
}

void PKVectorEngine::setGlobalAlpha(double al)
{
	this->alpha = al;
}

void PKVectorEngine::clipToRect(PKRect rect)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	Gdiplus::Rect clip(rect.x, rect.y, rect.w, rect.h);
	this->graphics->IntersectClip(clip);

#endif

#ifdef MACOSX
	// TODO
#endif

#ifdef LINUX
	// TODO
#endif
}

//
// Drawing
//

void PKVectorEngine::drawLine(double x1, double y1,
							  double x2, double y2,
							  PKColor lineColor,
							  double lineWidth)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	BYTE al = (BYTE) ((double)lineColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 lineColor.r,
						 lineColor.g,
						 lineColor.b);

	Gdiplus::Pen *pen = new Gdiplus::Pen(color, (Gdiplus::REAL) lineWidth);

	if(pen)
	{
        this->graphics->DrawLine(pen, (Gdiplus::REAL) x1, (Gdiplus::REAL) y1, 
                                      (Gdiplus::REAL) x2, (Gdiplus::REAL) y2);
		delete pen;
	}

#endif
	
#ifdef LINUX

    if(this->cr == NULL)
	{
		return;
	}
	
	cairo_set_line_width(this->cr, lineWidth);
	
	cairo_set_source_rgba(this->cr, 
	                      (double) lineColor.r/255.0, 
	                      (double) lineColor.g/255.0, 
	                      (double) lineColor.b/255.0,
	                      (double) lineColor.a/255.0);

    if((y1 == y2) && (x1 < x2)) x2 += lineWidth;
    if((x1 == x2) && (y1 < y2)) y2 += lineWidth;

    cairo_move_to(this->cr, x1, y1);
    cairo_line_to(this->cr, x2, y2);    
	cairo_stroke(this->cr);

#endif	
	
#ifdef MACOSX

	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();

	float fcomponents[4];
	fcomponents[0] = (float)lineColor.r/255.0;
	fcomponents[1] = (float)lineColor.g/255.0;
	fcomponents[2] = (float)lineColor.b/255.0;
	fcomponents[3] = (float)lineColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetStrokeColorWithColor(this->ctx, fgcolor);
	CGContextSetLineWidth(this->ctx, lineWidth);
	
	CGPoint points[2];
	
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y2;
	
	CGContextStrokeLineSegments(this->ctx, points, 2);
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);

#endif
}

void PKVectorEngine::drawLines(PKVPoint *points,
							   uint32_t  numPoints,
							   PKColor   lineColor,
							   double    lineWidth)
{
#ifdef WIN32

	if(this->graphics == NULL || numPoints < 2)
	{
		return;
	}

	BYTE al = (BYTE) ((double)lineColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 lineColor.r,
						 lineColor.g,
						 lineColor.b);

	Gdiplus::Pen *pen = new Gdiplus::Pen(color, (Gdiplus::REAL) lineWidth);

	if(pen)
	{
		Gdiplus::PointF *pts = new Gdiplus::PointF[numPoints];

		for(uint32_t i=0; i < numPoints; i++)
		{
			double x1 = points[i].x;
			double y1 = points[i].y;

			pts[i].X = (Gdiplus::REAL) x1;
			pts[i].Y = (Gdiplus::REAL) y1;
		}
	
		this->graphics->DrawLines(pen, pts, numPoints);

		delete [] pts;
		delete pen;
	}

#endif

#ifdef LINUX

    if(this->cr == NULL)
	{
		return;
	}
	
	cairo_set_line_width(this->cr, lineWidth);
	
	cairo_set_source_rgba(this->cr, 
	                      (double) lineColor.r/255.0, 
	                      (double) lineColor.g/255.0, 
	                      (double) lineColor.b/255.0,
	                      (double) lineColor.a/255.0);

    cairo_move_to(this->cr, points[0].x, points[0].y);

	for(uint32_t i=0; i < numPoints; i++)
	{
		double x1 = points[i].x;
		double y1 = points[i].y;

        cairo_line_to(this->cr, x1, y1);
	}    
        
	cairo_stroke(this->cr);

#endif	

#ifdef MACOSX
	
	if(this->ctx == NULL || numPoints < 2)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	
	float fcomponents[4];
	fcomponents[0] = (float)lineColor.r/255.0;
	fcomponents[1] = (float)lineColor.g/255.0;
	fcomponents[2] = (float)lineColor.b/255.0;
	fcomponents[3] = (float)lineColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetStrokeColorWithColor(this->ctx, fgcolor);
	CGContextSetLineWidth(this->ctx, lineWidth);
	
	uint32_t np = 2*(numPoints-2) + 2;
	CGPoint *pts = new CGPoint[np];
	uint32_t count = 0;
	
	for(uint32_t i=0; i < numPoints; i++)
	{
		if(i > 0 && i < numPoints - 1)
		{
			pts[count].x = points[i].x;
			pts[count].y = points[i].y;
			count++;
			
			pts[count].x = points[i].x;
			pts[count].y = points[i].y;
			count++;			
		}
		else
		{
			pts[count].x = points[i].x;
			pts[count].y = points[i].y;
			count++;
		}
	}
	
	CGContextStrokeLineSegments(this->ctx, pts, np);
	delete [] pts;
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);
	
#endif
	
}

//
// Rectangle
//

void PKVectorEngine::drawRect(double x, double y,
							  double w, double h,
							  PKColor lineColor,
							  double   lineWidth)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	BYTE al = (BYTE) ((double)lineColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 lineColor.r,
						 lineColor.g,
						 lineColor.b);

	Gdiplus::Pen *pen = new Gdiplus::Pen(color, (Gdiplus::REAL) lineWidth);

	if(pen)
	{
		graphics->DrawRectangle(pen, (Gdiplus::REAL) x, (Gdiplus::REAL) y, 
                               (Gdiplus::REAL) w, (Gdiplus::REAL) h);

		delete pen;
	}

#endif

#ifdef LINUX

    if(this->cr == NULL)
	{
		return;
	}
	
	cairo_set_line_width(this->cr, lineWidth);
	
	cairo_set_source_rgba(this->cr, 
	                      (double) lineColor.r/255.0, 
	                      (double) lineColor.g/255.0, 
	                      (double) lineColor.b/255.0,
	                      (double) lineColor.a/255.0);

    cairo_rectangle(this->cr, x, y, w, h);
	cairo_stroke(this->cr);

#endif	

#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);

	// Setup orientation
	
	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	
	float fcomponents[4];
	fcomponents[0] = (float)lineColor.r/255.0;
	fcomponents[1] = (float)lineColor.g/255.0;
	fcomponents[2] = (float)lineColor.b/255.0;
	fcomponents[3] = (float)lineColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetStrokeColorWithColor(this->ctx, fgcolor);
	CGContextSetLineWidth(this->ctx, lineWidth);

	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width   = w;
	rect.size.height   = h;
	
	CGContextStrokeRect(this->ctx, rect);
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);
	
#endif
}

//
// Fills
//

#ifdef WIN32

void PKVectorEngine::fillRect(double x, double y,
							  double w, double h,
							  HBRUSH br)
{
	if(this->graphics == NULL)
	{
		return;
	}

	LOGBRUSH logb;
	GetObject(br, sizeof(logb), &logb);

	BYTE r = GetRValue(logb.lbColor);
	BYTE g = GetGValue(logb.lbColor);
	BYTE b = GetBValue(logb.lbColor);

	Gdiplus::Color color(r, g, b);
	
	Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(color);

	if(brush)
	{
        graphics->FillRectangle(brush, 
							   (Gdiplus::REAL) x, 
							   (Gdiplus::REAL) y, 
							   (Gdiplus::REAL) w, 
							   (Gdiplus::REAL) h);

        delete brush;
	}
}

#endif 


void PKVectorEngine::fillRect(double x, double y,
							  double w, double h,
							  PKColor fillColor)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	BYTE al = (BYTE) ((double)fillColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 fillColor.r,
						 fillColor.g,
						 fillColor.b);

	Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(color);

	if(brush)
	{
        graphics->FillRectangle(brush, 
							   (Gdiplus::REAL) x, 
							   (Gdiplus::REAL) y, 
							   (Gdiplus::REAL) w, 
							   (Gdiplus::REAL) h);

        delete brush;
	}

#endif

#ifdef LINUX
	
    if(this->cr == NULL)
	{
		return;
	}
		
	cairo_set_source_rgba(this->cr, 
	                      (double) fillColor.r/255.0, 
	                      (double) fillColor.g/255.0, 
	                      (double) fillColor.b/255.0,
	                      (double) fillColor.a/255.0);

    cairo_rectangle(this->cr, x, y, w, h);
	cairo_fill(this->cr);

#endif	

#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);

	// Setup orientation
	
	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
	
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	
	float fcomponents[4];
	fcomponents[0] = (float)fillColor.r/255.0;
	fcomponents[1] = (float)fillColor.g/255.0;
	fcomponents[2] = (float)fillColor.b/255.0;
	fcomponents[3] = (float)fillColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetFillColorWithColor(this->ctx, fgcolor);
	
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width  = w;
	rect.size.height = h;
	
	CGContextFillRect(this->ctx, rect);
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);
	
#endif
	
}

void PKVectorEngine::fillTriangle(double x1, double y1,
                                  double x2, double y2,
                                  double x3, double y3,
                                  PKColor fillColor)
{
#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
    
	// Setup orientation
	
	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
        
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	
	float fcomponents[4];
	fcomponents[0] = (float)fillColor.r/255.0;
	fcomponents[1] = (float)fillColor.g/255.0;
	fcomponents[2] = (float)fillColor.b/255.0;
	fcomponents[3] = (float)fillColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetFillColorWithColor(this->ctx, fgcolor);

    CGContextBeginPath(this->ctx);
	CGContextMoveToPoint(this->ctx, x1, y1);
    CGContextAddLineToPoint(this->ctx, x2, y2);
    CGContextAddLineToPoint(this->ctx, x3, y3);
    CGContextAddLineToPoint(this->ctx, x1, y1);
    CGContextClosePath(this->ctx);
    
    CGContextFillPath(this->ctx);
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);
	
#endif    
    
#ifdef WIN32

#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	BYTE al = (BYTE) ((double)fillColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 fillColor.r,
						 fillColor.g,
						 fillColor.b);

	Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(color);

	if(brush)
	{
        Gdiplus::GraphicsPath *path = new Gdiplus::GraphicsPath();

        path->AddLine((Gdiplus::REAL)x1, (Gdiplus::REAL)y1, (Gdiplus::REAL)x2, (Gdiplus::REAL)y2);
        path->AddLine((Gdiplus::REAL)x2, (Gdiplus::REAL)y2, (Gdiplus::REAL)x3, (Gdiplus::REAL)y3);
        path->AddLine((Gdiplus::REAL)x3, (Gdiplus::REAL)y3, (Gdiplus::REAL)x1, (Gdiplus::REAL)y1);
        path->CloseAllFigures();

        if(path)
        {
            graphics->FillPath(brush, 
	    					   path);

            delete path;
        }

        delete brush;
	}

#endif

#endif

#ifdef LINUX
    // TODO
#endif
}

void PKVectorEngine::fillCircle(double x, double y,
								double  radius,
								PKColor fillColor)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	BYTE al = (BYTE) ((double)fillColor.a*this->alpha);

	Gdiplus::Color color(al, 
						 fillColor.r,
						 fillColor.g,
						 fillColor.b);

	Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(color);

	if(brush)
	{
		graphics->FillEllipse(brush, 
							 (Gdiplus::REAL) (x-radius), 
							 (Gdiplus::REAL) (y-radius), 
							 (Gdiplus::REAL) (2*radius), 
							 (Gdiplus::REAL) (2*radius));

		delete brush;
	}

#endif
	
#ifdef LINUX

    if(this->cr == NULL)
	{
		return;
	}
		
	cairo_set_source_rgba(this->cr, 
	                      (double) fillColor.r/255.0, 
	                      (double) fillColor.g/255.0, 
	                      (double) fillColor.b/255.0,
	                      (double) fillColor.a/255.0);

    cairo_arc(this->cr, x, y, radius, 0, 2*M_PI);
	cairo_fill(this->cr);

#endif		
	
#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);

	// Setup orientation

	if(this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
	
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	// Setup colors
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	
	float fcomponents[4];
	fcomponents[0] = (float)fillColor.r/255.0;
	fcomponents[1] = (float)fillColor.g/255.0;
	fcomponents[2] = (float)fillColor.b/255.0;
	fcomponents[3] = (float)fillColor.a/255.0;
	
	CGColorRef fgcolor = CGColorCreate(rgb, fcomponents);
	
	CGContextSetFillColorWithColor(this->ctx, fgcolor);
	
	CGRect rect;
	rect.origin.x = x-radius;
	rect.origin.y = y-radius;
	rect.size.width   = 2*radius;
	rect.size.height   = 2*radius;
	
	CGContextFillEllipseInRect(this->ctx, rect);
	
	CGColorSpaceRelease(rgb);
	CGColorRelease(fgcolor);
	
	CGContextRestoreGState(this->ctx);
	
#endif
	
}

uint32_t PKVectorEngine::getStringWidth(std::wstring family,
										double fontSize,
										PKVFontStyle style,
										std::wstring text)
{
#ifdef WIN32

	if(this->graphics == NULL)
	{
		return 0;
	}

	UINT styleMask = 0;

	switch(style)
	{
	case PKV_FONT_STYLE_BOLD:
		{
			styleMask = Gdiplus::FontStyleBold;
		}
		break;

	case PKV_FONT_STYLE_ITALICS:
		{
			styleMask = Gdiplus::FontStyleItalic;
		}
		break;

	case PKV_FONT_STYLE_BOLD_ITALICS:
		{
			styleMask = Gdiplus::FontStyleBoldItalic;
		}
		break;

	case PKV_FONT_STYLE_UNDERLINE:
		{
			styleMask = Gdiplus::FontStyleUnderline;
		}
		break;

	case PKV_FONT_STYLE_STRIKEOUT:
		{
			styleMask = Gdiplus::FontStyleStrikeout;
		}
		break;

	case PKV_FONT_STYLE_NORMAL:
	default:
		{
			styleMask = Gdiplus::FontStyleRegular;
		}
		break;
	}

	Gdiplus::Font *font = new Gdiplus::Font(family.c_str(),
										   (Gdiplus::REAL) fontSize,
										    styleMask,
										    Gdiplus::UnitPixel,
										    NULL); // FontCollection

	if(font != NULL)
	{
		Gdiplus::RectF layoutRect(0, 0, 0, 0);
	    Gdiplus::RectF boundRect;
		
        Gdiplus::StringFormat format;
        format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        format.SetTrimming(Gdiplus::StringTrimmingNone);

		graphics->MeasureString(text.c_str(), text.size(),
								font, layoutRect, &format, &boundRect);

        delete font;

		return (uint32_t) (boundRect.Width);
	}

	return 0;

#endif

#ifdef LINUX

    if(this->cr == NULL)
	{
	    return 0;
	}

    cairo_text_extents_t extents;
    
    std::string textUTF8 = PKStr::wStringToUTF8string(text);
    std::string familyUTF8 = PKStr::wStringToUTF8string(family);

    cairo_font_weight_t font_weight = CAIRO_FONT_WEIGHT_NORMAL;
    cairo_font_slant_t  font_slant  = CAIRO_FONT_SLANT_NORMAL;

	switch(style)
	{
	case PKV_FONT_STYLE_BOLD:
		{
		    font_weight = CAIRO_FONT_WEIGHT_BOLD;
		}
		break;

	case PKV_FONT_STYLE_ITALICS:
		{
		    font_slant  = CAIRO_FONT_SLANT_ITALIC;
		}
		break;

	case PKV_FONT_STYLE_BOLD_ITALICS:
		{
		    font_weight = CAIRO_FONT_WEIGHT_BOLD;
		    font_slant  = CAIRO_FONT_SLANT_ITALIC;
		}
		break;

	case PKV_FONT_STYLE_UNDERLINE:
		{
		    // Does not affect size
		}
		break;

	case PKV_FONT_STYLE_STRIKEOUT:
		{
		    // Does not affect size
		}
		break;

	case PKV_FONT_STYLE_NORMAL:
	default:
		{
		}
		break;
	}
    
    cairo_select_font_face(this->cr, familyUTF8.c_str(), 
                           font_slant, font_weight);
                           
    cairo_set_font_size(this->cr, fontSize);
    
    cairo_text_extents(this->cr, textUTF8.c_str(), &extents);

    if(extents.x_advance > ((fontSize/10)))
    {
        return extents.x_advance - (fontSize/10);
    }

    return extents.x_advance;

#endif	
	
#ifdef MACOSX
	
    ATSUFontID     fontID;
    ATSUStyle      defaultStyle;
	
    OSStatus status = noErr;
	
	// See how many spaces we got at the end
	
	uint32_t numSpaces = 0;
	uint32_t text_size = text.size();
	int32_t  index_sp  = text_size - 1;
	
	while(index_sp > 0)
	{
		if(text[index_sp] == L' ')
		{
			numSpaces++;
		}
		else
		{
			break;
		}
		
		index_sp--;
	}
    
	index_sp = 0;
	
	while(index_sp < text_size)
	{
		if(text[index_sp] == L' ')
		{
			numSpaces++;
		}
		else
		{
			break;
		}
		
		index_sp++;
	}

    // Locate font
    
	std::string familyName = PKStr::wStringToString(family);
    
    status = ATSUFindFontFromName(familyName.c_str(), familyName.size(),
                                  kFontFullName, kFontNoPlatformCode, 
                                  kFontUnicodePlatform, kFontNoLanguageCode,
                                  &fontID);
	
	if(status == kATSUInvalidFontErr)
	{
		return 0;
	}
    
    // Create style
    
    status = ATSUCreateStyle(&defaultStyle);
    
    if(status == noErr)
    {
        ATSUAttributeTag  theTags[] = {kATSUSizeTag, 
									   kATSUFontTag, 
									   kATSUStyleRenderingOptionsTag};
		
        ByteCount        theSizes[] = {sizeof(Fixed), 
									   sizeof(ATSUFontID), 
									   sizeof(ATSStyleRenderingOptions)};
				
        Fixed             atsuSize = IntToFixed(fontSize);
        ATSStyleRenderingOptions o = kATSStyleApplyAntiAliasing;
		
        ATSUAttributeValuePtr theValues[] = {&atsuSize, &fontID, &o};
        
        status = ATSUSetAttributes(defaultStyle, 3, 
                                   theTags, 
                                   theSizes, 
                                   theValues);                
        
        if(style == PKV_FONT_STYLE_BOLD)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
        if(style == PKV_FONT_STYLE_ITALICS)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDItalicTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
        if(style == PKV_FONT_STYLE_BOLD_ITALICS)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag, kATSUQDItalicTag};
            ByteCount        sizes[] = {sizeof(Boolean), sizeof(Boolean)};
			
            Boolean b = true;
            Boolean c = true;
            
            ATSUAttributeValuePtr values[] = {&b, &c};
            
            status = ATSUSetAttributes(defaultStyle, 2, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
        if(style == PKV_FONT_STYLE_UNDERLINE)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDUnderlineTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
        if(style == PKV_FONT_STYLE_STRIKEOUT)
        {   
            ATSUAttributeTag  tags[] = {kATSUStyleStrikeThroughTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
		// Setup the string
		
		ATSUTextLayout  myTextLayout;
		
		UniChar *textU = NULL;
		uint32_t ss = 0;
		
		PKStr::wStringToUniChar(text, &ss, &textU);
		UniCharCount    length = ss;
		
		if(textU == NULL)
		{
			ATSUDisposeStyle(defaultStyle);
			return 0;
		}
		
		status = ATSUCreateTextLayoutWithTextPtr ((UniChar*) textU,
												  kATSUFromTextBeginning, // offset from beginning
												  kATSUToTextEnd,         // length of text range
												  length,                 // length of text buffer
												  1,                      // number of style runs
												  &length,                // length of the style run
												  &defaultStyle, 
												  &myTextLayout); 
		
		ATSUAttributeTag  ltheTags[] =  {kATSUCGContextTag, kATSULineTruncationTag};
		ByteCount   ltheSizes[] = {sizeof(CGContextRef), sizeof(ATSULineTruncation)};
		
		ATSULineTruncation truncate  = kATSUTruncateNone;
		
		ATSUAttributeValuePtr ltheValues[] = {&this->ctx, &truncate};
		
		ATSUSetLayoutControls(myTextLayout, 2, 
							  ltheTags, 
							  ltheSizes, 
							  ltheValues);

		// And measure

		Rect bounds;
		
		ATSUMeasureTextImage(myTextLayout,
							 0, length,
							 0, 0, &bounds);
		
		
		// And cleanup
		
		delete [] textU;
		ATSUDisposeTextLayout(myTextLayout);		
		
		// See if we had spaces
		
		if(numSpaces > 0)
		{
			// Get the glyph width of a space
			
			ATSUTextLayout  myTextLayout2;
			UniCharCount    length2 = 1;
			
			UniChar c = L' ';
			
			status = ATSUCreateTextLayoutWithTextPtr ((UniChar*) &c,
													  kATSUFromTextBeginning, // offset from beginning
													  kATSUToTextEnd,         // length of text range
													  1,                      // length of text buffer
													  1,                      // number of style runs
													  &length2,                // length of the style run
													  &defaultStyle, 
													  &myTextLayout2); 
			
			ItemCount count = 0;
			ATSTrapezoid value;
			
			ATSUGetGlyphBounds(myTextLayout2, 0, 0,
							   kATSUFromTextBeginning,
							   1, kATSUseDeviceOrigins,
							   1, &value, &count);
			
			int x1 = FixedToInt(value.lowerLeft.x);
			int x2 = FixedToInt(value.lowerRight.x);
			
			ATSUDisposeTextLayout(myTextLayout2);
			ATSUDisposeStyle(defaultStyle);
			
			return (bounds.right - bounds.left) + numSpaces*(x2-x1);
		}
		
		ATSUDisposeStyle(defaultStyle);

		return (bounds.right - bounds.left);
	}
	
#endif
	
	return 0;
}


void PKVectorEngine::drawString(std::wstring family,
								double fontSize,
								PKVFontStyle style,
								PKColor color,
								double x, double y,
								std::wstring text)
{
#ifdef WIN32
	
	if(this->graphics == NULL)
	{
		return;
	}

	UINT styleMask = 0;

	switch(style)
	{
	case PKV_FONT_STYLE_BOLD:
		{
			styleMask = Gdiplus::FontStyleBold;
		}
		break;

	case PKV_FONT_STYLE_ITALICS:
		{
			styleMask = Gdiplus::FontStyleItalic;
		}
		break;

	case PKV_FONT_STYLE_BOLD_ITALICS:
		{
			styleMask = Gdiplus::FontStyleBoldItalic;
		}
		break;

	case PKV_FONT_STYLE_UNDERLINE:
		{
			styleMask = Gdiplus::FontStyleUnderline;
		}
		break;

	case PKV_FONT_STYLE_STRIKEOUT:
		{
			styleMask = Gdiplus::FontStyleStrikeout;
		}
		break;

	case PKV_FONT_STYLE_NORMAL:
	default:
		{
			styleMask = Gdiplus::FontStyleRegular;
		}
		break;
	}

	Gdiplus::Font *font = new Gdiplus::Font(family.c_str(),
										   (Gdiplus::REAL) fontSize,
										    styleMask,
										    Gdiplus::UnitPixel,
										    NULL); // FontCollection

	BYTE al = (BYTE) ((double)color.a*this->alpha);

	Gdiplus::Color gcolor(al, 
						  color.r,
						  color.g,
						  color.b);

	Gdiplus::SolidBrush *brush = new Gdiplus::SolidBrush(gcolor);

	if(font != NULL && brush != NULL)
	{
		Gdiplus::PointF origin;

		origin.X = (Gdiplus::REAL) x;
		origin.Y = (Gdiplus::REAL) y;

		graphics->DrawString(text.c_str(),
							 text.size(),
							 font, origin,
							 brush);
		
		delete font;
		font = NULL;
		delete brush;
		brush = NULL;
	}

	if(brush != NULL)
	{
		delete brush;
	}

	if(font != NULL)
	{
		delete font;
	}

#endif
	
#ifdef LINUX

    if(this->cr == NULL)
	{
	    return;
	}

    cairo_text_extents_t extents;
    
    std::string textUTF8 = PKStr::wStringToUTF8string(text);
    std::string familyUTF8 = PKStr::wStringToUTF8string(family);
    
    cairo_font_weight_t font_weight = CAIRO_FONT_WEIGHT_NORMAL;
    cairo_font_slant_t  font_slant  = CAIRO_FONT_SLANT_NORMAL;

	switch(style)
	{
	case PKV_FONT_STYLE_BOLD:
		{
		    font_weight = CAIRO_FONT_WEIGHT_BOLD;
		}
		break;

	case PKV_FONT_STYLE_ITALICS:
		{
		    font_slant  = CAIRO_FONT_SLANT_ITALIC;
		}
		break;

	case PKV_FONT_STYLE_BOLD_ITALICS:
		{
		    font_weight = CAIRO_FONT_WEIGHT_BOLD;
		    font_slant  = CAIRO_FONT_SLANT_ITALIC;
		}
		break;

	case PKV_FONT_STYLE_UNDERLINE:
		{
		    // Does not affect size
		}
		break;

	case PKV_FONT_STYLE_STRIKEOUT:
		{
		    // Does not affect size
		}
		break;

	case PKV_FONT_STYLE_NORMAL:
	default:
		{
		}
		break;
	}
    
    cairo_select_font_face(this->cr, familyUTF8.c_str(), 
                           font_slant, font_weight);
                           
    cairo_set_font_size(this->cr, fontSize);
    
	cairo_set_source_rgba(this->cr, 
	                      (double) color.r/255.0, 
	                      (double) color.g/255.0, 
	                      (double) color.b/255.0,
	                      (double) color.a/255.0);

    cairo_move_to(this->cr, x, y+fontSize);
    cairo_show_text(this->cr, textUTF8.c_str());

#endif		
	
#ifdef MACOSX
	
    ATSUFontID     fontID;
    ATSUStyle      defaultStyle;

    OSStatus status = noErr;
    
    // Locate font
    
	std::string familyName = PKStr::wStringToString(family);
    
    status = ATSUFindFontFromName(familyName.c_str(), familyName.size(),
                                  kFontFullName, kFontNoPlatformCode, 
                                  kFontUnicodePlatform, kFontNoLanguageCode,
                                  &fontID);
	
	if(status == kATSUInvalidFontErr)
	{
		return;
	}
    
    // Create style
    
    status = ATSUCreateStyle(&defaultStyle);
    
    if(status == noErr)
    {
        ATSUAttributeTag  theTags[] = {kATSUSizeTag, 
									   kATSURGBAlphaColorTag, 
									   kATSUFontTag, 
									   kATSUStyleRenderingOptionsTag};
		
        ByteCount        theSizes[] = {sizeof(Fixed), 
									   sizeof(ATSURGBAlphaColor), 
									   sizeof(ATSUFontID), 
									   sizeof(ATSStyleRenderingOptions)};
        
        Fixed             atsuSize = IntToFixed(fontSize);
        ATSURGBAlphaColor fcolor   = {(float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0, (float)color.a/255.0};
        ATSStyleRenderingOptions o = kATSStyleApplyAntiAliasing;
		
        ATSUAttributeValuePtr theValues[] = {&atsuSize, &fcolor, &fontID, &o};
        
        status = ATSUSetAttributes(defaultStyle, 4, 
                                   theTags, 
                                   theSizes, 
                                   theValues);                
        
        if(style == PKV_FONT_STYLE_BOLD)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }

        if(style == PKV_FONT_STYLE_ITALICS)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDItalicTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
        if(style == PKV_FONT_STYLE_BOLD_ITALICS)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDBoldfaceTag, kATSUQDItalicTag};
            ByteCount        sizes[] = {sizeof(Boolean), sizeof(Boolean)};
			
            Boolean b = true;
            Boolean c = true;
            
            ATSUAttributeValuePtr values[] = {&b, &c};
            
            status = ATSUSetAttributes(defaultStyle, 2, 
                                       tags, 
                                       sizes, 
                                       values);                
        }

        if(style == PKV_FONT_STYLE_UNDERLINE)
        {   
            ATSUAttributeTag  tags[] = {kATSUQDUnderlineTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }

        if(style == PKV_FONT_STYLE_STRIKEOUT)
        {   
            ATSUAttributeTag  tags[] = {kATSUStyleStrikeThroughTag};
            ByteCount        sizes[] = {sizeof(Boolean)};
			
            Boolean b = true;
            
            ATSUAttributeValuePtr values[] = {&b};
            
            status = ATSUSetAttributes(defaultStyle, 1, 
                                       tags, 
                                       sizes, 
                                       values);                
        }
		
		// Draw the string
		
		ATSUTextLayout  myTextLayout;
		
		UniChar *textU = NULL;
		uint32_t ss = 0;
		
		PKStr::wStringToUniChar(text, &ss, &textU);
		UniCharCount    length = ss;
		
		if(textU == NULL)
		{
			ATSUDisposeStyle(defaultStyle);
			return;
		}
		
		status = ATSUCreateTextLayoutWithTextPtr ((UniChar*) textU,
												  kATSUFromTextBeginning, // offset from beginning
												  kATSUToTextEnd,         // length of text range
												  length,                 // length of text buffer
												  1,                      // number of style runs
												  &length,                // length of the style run
												  &defaultStyle, 
												  &myTextLayout); 
		
		ATSUAttributeTag  ltheTags[] =  {kATSUCGContextTag};
		ByteCount   ltheSizes[] = {sizeof(CGContextRef)};
		
		ATSUAttributeValuePtr ltheValues[] = {&this->ctx};
		
		ATSUSetLayoutControls(myTextLayout, 1, 
							  ltheTags, 
							  ltheSizes, 
							  ltheValues);
		
		CGContextSaveGState(this->ctx);

        CGContextSetAlpha(this->ctx, (double) this->alpha);

		// Setup orientation
		
		if(!this->ownCtx)
		{
			CGContextTranslateCTM(this->ctx, 0, 
								  this->height-1);
			
			CGContextScaleCTM(this->ctx, 1.0, -1.0);
		}

		ATSUDrawText (myTextLayout, 
					  kATSUFromTextBeginning, 
					  kATSUToTextEnd, 
					  IntToFixed(x), 
					  IntToFixed(this->height - y - fontSize));
		
		CGContextRestoreGState(this->ctx);
		
		// And cleanup
		
		delete [] textU;
		ATSUDisposeTextLayout(myTextLayout);		
		ATSUDisposeStyle(defaultStyle);
	}
#endif
}

void PKVectorEngine::drawImage(PKImage *image,
			                   double x, double y)
{
	if(image == NULL)
	{
		return;
	}
	
	int w = image->getWidth();
	int h = image->getHeight();
	
	if(w == 0 || 
	   h == 0)
	{
		return;
	}

#ifdef LINUX

    if(this->cr == NULL)
	{
	    return;
	}

    cairo_surface_t *img = cairo_image_surface_create_for_data((unsigned char *)image->getPixels(),
                                                               CAIRO_FORMAT_ARGB32,
                                                               w, h, w*4);

    cairo_save(this->cr);

    cairo_new_path(this->cr);
    cairo_move_to(this->cr, x, y);
    cairo_line_to(this->cr, x+w,y);
    cairo_line_to(this->cr, x+w,y+h);
    cairo_line_to(this->cr, x,y+h);
    cairo_close_path(this->cr);
    cairo_clip(this->cr);    

    cairo_set_source_surface(this->cr, img, x, y);

    cairo_paint(this->cr);
    cairo_restore(this->cr);

    cairo_surface_destroy (img);

#endif

#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(image->getWidth(),
												  image->getHeight(),
												  image->getWidth()*4,
												  PixelFormat32bppARGB,
												  (BYTE*)image->getPixels());

	if(bitmap)
	{
		this->graphics->DrawImage(bitmap, 
                                 (Gdiplus::REAL) x,
                                 (Gdiplus::REAL) y);
	
        delete bitmap;
    }

#endif

#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(!this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}

	CGImageRef img = image->getCGImageRef();
		
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = this->height - y - h;
	rect.size.width  = w;
	rect.size.height = h;
	
	CGContextSetInterpolationQuality(this->ctx, kCGInterpolationHigh);
	CGContextDrawImage(this->ctx, rect, img);
	
	CGContextRestoreGState(this->ctx);
#endif
}			

#ifdef MACOSX	

void PKVectorEngine::drawCGImage(CGImageRef image,
								 double x, double y, double alpha)
{
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) alpha);
	
	// Setup orientation
	
	if(!this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	CGImageRef img = image;
	
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = this->height - y - CGImageGetHeight(img);
	rect.size.width  = CGImageGetWidth(img);
	rect.size.height = CGImageGetHeight(img);
	
	CGContextSetInterpolationQuality(this->ctx, kCGInterpolationHigh);
	CGContextDrawImage(this->ctx, rect, img);
	
	CGContextRestoreGState(this->ctx);	
}

#endif

void PKVectorEngine::fillStretchWidth(int x, int y, int w, PKImage *source,
									  std::vector<unsigned int> offsets)
{
	int sizeDiff = (w - source->getWidth());
	
	if(sizeDiff <= 0)
	{
		this->drawImageSubImageWithOffset(source, x, y, 0, 0, w);
		return;
	}
	
	unsigned int srcOffset = 0;
	unsigned int dstOffset = x;
	
	int count = offsets.size();
	
	if(count < 1)
	{
		return;
	}
	
	for(int i=0; i < count; i++)
	{
		int areaSize = 0;
		
		if(i == (count - 1))
		{
			areaSize = (sizeDiff / count) + (sizeDiff % count);
		}
		else
		{
			areaSize = (sizeDiff / count);
		}
		
		this->drawImageSubImageWithOffset(source, dstOffset, y, srcOffset, 0, offsets[i] - srcOffset);
		
		dstOffset += (offsets[i] - srcOffset);
		
		for(int j=0; j < areaSize; j++)
		{
			this->drawImageSubImageWithOffset(source, dstOffset+j, y, offsets[i], 0, 1);
		}
		
		srcOffset  = offsets[i];
		dstOffset += areaSize;
	}

	this->drawImageSubImageWithOffset(source, dstOffset, y, srcOffset, 0, source->getWidth() - offsets[offsets.size() - 1]);	
}

void PKVectorEngine::drawImage(void *argbBits, int w, int h,
							   double x, double y, double alpha)
{
	if(argbBits == NULL)
	{
		return;
	}
	
	if(w == 0 || 
	   h == 0)
	{
		return;
	}

#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(w, h, w*4,
												  PixelFormat32bppARGB,
												  (BYTE*)argbBits);

	if(bitmap)
	{
		if(alpha == 1)
		{
			this->graphics->DrawImage(bitmap, 
		                             (Gdiplus::REAL) x,
			                         (Gdiplus::REAL) y);
		}
		else
		{
			Gdiplus::ColorMatrix clrMatrix =         { 
						1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, (Gdiplus::REAL) alpha, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			};			

			Gdiplus::ImageAttributes imgAttr;
			imgAttr.SetColorMatrix(&clrMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

			Gdiplus::RectF destination(x, y, w, h);

			this->graphics->DrawImage(bitmap, destination, 
									  0, 0, w, h, Gdiplus::UnitPixel, &imgAttr);
		}

        delete bitmap;
    }

#endif


#ifdef MACOSX
	// TODO
#endif

#ifdef LINUX
	// TODO
#endif
}

void PKVectorEngine::drawImageSubImage(PKImage *image,
                 			           double x, double y,
			                           double w, double h)
{
	if(image == NULL)
	{
		return;
	}
	
	if(image->getWidth() == 0 || 
	   image->getHeight() == 0)
	{
		return;
	}

	if(w == 0) w = image->getWidth();
	if(h == 0) h = image->getHeight();

#ifdef LINUX

    if(this->cr == NULL)
	{
	    return;
	}

    cairo_surface_t *img = cairo_image_surface_create_for_data((unsigned char *)image->getPixels(),
                                                               CAIRO_FORMAT_ARGB32,
                                                               image->getWidth(),
                                                               image->getHeight(),
                                                               image->getWidth()*4);

    cairo_save(this->cr);

    cairo_new_path(this->cr);
    cairo_move_to(this->cr, x, y);
    cairo_line_to(this->cr, x+w,y);
    cairo_line_to(this->cr, x+w,y+h);
    cairo_line_to(this->cr, x,y+h);
    cairo_close_path(this->cr);
    cairo_clip(this->cr);    

    cairo_set_source_surface(this->cr, img, x, y);

    cairo_paint(this->cr);
    cairo_restore(this->cr);

    cairo_surface_destroy (img);

#endif

#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(image->getWidth(),
												  image->getHeight(),
												  image->getWidth()*4,
												  PixelFormat32bppARGB,
												  (BYTE*)image->getPixels());

	if(bitmap)
	{
        Gdiplus::Rect rect;
        rect.X = (INT) x;
        rect.Y = (INT) y;
        rect.Width  = (INT) w;
        rect.Height = (INT) h;

		this->graphics->DrawImage(bitmap, 
							      rect, 
								  0, 0, 
								  (INT)w, (INT)h, 
								  Gdiplus::UnitPixel,
                                  NULL, NULL, NULL);
	
        delete bitmap;
    }

#endif

#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(!this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}

	CGImageRef img = image->getCGImageRef();
	
	CGRect sub_rect;
	sub_rect.origin.x = 0;
	sub_rect.origin.y = 0;
	sub_rect.size.width  = w;
	sub_rect.size.height = h;

	CGImageRef sub_img = CGImageCreateWithImageInRect(img, sub_rect);
		
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = this->height - y - h;
	rect.size.width  = w;
	rect.size.height = h;
	
	CGContextSetInterpolationQuality(this->ctx, kCGInterpolationHigh);
	CGContextDrawImage(this->ctx, rect, sub_img);
	
	CFRelease(sub_img);
	CGContextRestoreGState(this->ctx);
#endif
}			                   

void PKVectorEngine::drawImageSubImageWithOffset(PKImage *image,
												 double x, double y,
												 double fromX, double fromY,
												 double w, double h)
{
	if(image == NULL)
	{
		return;
	}
	
	if(image->getWidth() == 0 || 
	   image->getHeight() == 0)
	{
		return;
	}
	
	if(w == 0) w = image->getWidth();
	if(h == 0) h = image->getHeight();
	
#ifdef LINUX
	
    if(this->cr == NULL)
	{
	    return;
	}
	
    cairo_surface_t *img = cairo_image_surface_create_for_data((unsigned char *)image->getPixels(),
                                                               CAIRO_FORMAT_ARGB32,
                                                               image->getWidth(),
                                                               image->getHeight(),
                                                               image->getWidth()*4);
	
    cairo_save(this->cr);
	
    cairo_new_path(this->cr);
    cairo_move_to(this->cr, x, y);
    cairo_line_to(this->cr, x+w,y);
    cairo_line_to(this->cr, x+w,y+h);
    cairo_line_to(this->cr, x,y+h);
    cairo_close_path(this->cr);
    cairo_clip(this->cr);    
	
	// TODO: check that
    cairo_set_source_surface(this->cr, img, x+fromX, y+fromY);
	
    cairo_paint(this->cr);
    cairo_restore(this->cr);
	
    cairo_surface_destroy (img);
	
#endif
	
#ifdef WIN32
	
	if(this->graphics == NULL)
	{
		return;
	}
	
	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(image->getWidth(),
												  image->getHeight(),
												  image->getWidth()*4,
												  PixelFormat32bppARGB,
												  (BYTE*)image->getPixels());
	
	if(bitmap)
	{
        Gdiplus::Rect rect;
        rect.X = (INT) x;
        rect.Y = (INT) y;
        rect.Width  = (INT) w;
        rect.Height = (INT) h;
		
		this->graphics->DrawImage(bitmap, 
							      rect, 
								  (INT)fromX, (INT)fromY, 
								  (INT)w, (INT)h, 
								  Gdiplus::UnitPixel,
                                  NULL, NULL, NULL);
		
        delete bitmap;
    }
	
#endif
	
#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(!this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}
	
	CGImageRef img = image->getCGImageRef();
	
	CGRect sub_rect;
	sub_rect.origin.x = fromX;
	sub_rect.origin.y = fromY;
	sub_rect.size.width  = w;
	sub_rect.size.height = h;
	
	CGImageRef sub_img = CGImageCreateWithImageInRect(img, sub_rect);
	
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = this->height - y - h;
	rect.size.width  = w;
	rect.size.height = h;
	
	CGContextSetInterpolationQuality(this->ctx, kCGInterpolationHigh);
	CGContextDrawImage(this->ctx, rect, sub_img);
	
	CFRelease(sub_img);
	CGContextRestoreGState(this->ctx);
#endif
}

void PKVectorEngine::drawImageStretched(PKImage *image,
			                            double x, double y,
			                            double w, double h)
{
	if(image == NULL)
	{
		return;
	}
	
	if(image->getWidth() == 0 || 
	   image->getHeight() == 0)
	{
		return;
	}

	if(w == 0) return;
	if(h == 0) return;

#ifdef LINUX

    if(this->cr == NULL)
	{
	    return;
	}

    cairo_surface_t *img = cairo_image_surface_create_for_data((unsigned char *)image->getPixels(),
                                                               CAIRO_FORMAT_ARGB32,
                                                               image->getWidth(),
                                                               image->getHeight(),
                                                               image->getWidth()*4);

    cairo_save(this->cr);

    cairo_new_path(this->cr);
    cairo_move_to(this->cr, x, y);
    cairo_line_to(this->cr, x+w,y);
    cairo_line_to(this->cr, x+w,y+h);
    cairo_line_to(this->cr, x,y+h);
    cairo_close_path(this->cr);
    cairo_clip(this->cr);    

    cairo_matrix_t   matrix;
    cairo_pattern_t *pattern;
    
    pattern = cairo_pattern_create_for_surface(img);
    
    float cx = image->getWidth()  / w;
    float cy = image->getHeight() / h;
    
    cairo_matrix_init_translate(&matrix, -cx*x, -cy*y);
    cairo_matrix_scale(&matrix, cx, cy);
    
    cairo_pattern_set_matrix(pattern, &matrix);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_BEST);

    cairo_set_source(this->cr, pattern);
    cairo_paint(this->cr);

    cairo_restore(this->cr);
    cairo_surface_destroy (img);

#endif

#ifdef WIN32

	if(this->graphics == NULL)
	{
		return;
	}

	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(image->getWidth(),
												  image->getHeight(),
												  image->getWidth()*4,
												  PixelFormat32bppARGB,
												  (BYTE*)image->getPixels());

	if(bitmap)
	{
        Gdiplus::Rect rect;
        rect.X = (INT) x;
        rect.Y = (INT) y;
        rect.Width  = (INT) w;
        rect.Height = (INT) h;

		this->graphics->DrawImage(bitmap, 
							      rect, 
								  0, 0, 
								  image->getWidth(), 
                                  image->getHeight(), 
								  Gdiplus::UnitPixel,
                                  NULL, NULL, NULL);
	
        delete bitmap;
    }

#endif

#ifdef MACOSX
	
	if(this->ctx == NULL)
	{
		return;
	}
	
	CGContextSaveGState(this->ctx);
	CGContextSetAlpha(this->ctx, (double) this->alpha);
	
	// Setup orientation
	
	if(!this->ownCtx)
	{
		CGContextTranslateCTM(this->ctx, 0, 
							  this->height-1);
		
		CGContextScaleCTM(this->ctx, 1.0, -1.0);
	}

	CGImageRef img = image->getCGImageRef();
		
	CGRect rect;
	rect.origin.x = x;
	rect.origin.y = this->height - y - h;
	rect.size.width  = w;
	rect.size.height = h;
	
	CGContextSetInterpolationQuality(this->ctx, kCGInterpolationHigh);
	CGContextDrawImage(this->ctx, rect, img);
	
	CGContextRestoreGState(this->ctx);
#endif
}			                   


