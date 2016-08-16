//
//
//

#ifndef _PK_VECTOR_ENGINE
#define _PK_VECTOR_ENGINE

#include "PKTypes.h"
#include "PKImage.h"
#include <string>
#include <vector>

#ifdef WIN32

#include <windows.h>
#include <GdiPlus.h>

typedef HWND PKVWindowHandle;
typedef HDC  PKVDeviceHandle;

#endif

#ifdef LINUX

#include <gtk/gtk.h>
#include <cairo.h>

typedef GtkWidget* PKVWindowHandle;
typedef GdkDrawable* PKVDeviceHandle;

#endif

#ifdef MACOSX

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

typedef CGrafPtr PKVWindowHandle;
typedef CGrafPtr PKVDeviceHandle;

#endif

//
// DEFINES
//

typedef enum _PKVFontStyle
{
	
	PKV_FONT_STYLE_NORMAL,
	PKV_FONT_STYLE_BOLD,
	PKV_FONT_STYLE_ITALICS,
	PKV_FONT_STYLE_BOLD_ITALICS,
	PKV_FONT_STYLE_UNDERLINE,
	PKV_FONT_STYLE_STRIKEOUT,

} PKVFontStyle;

//
// TYPES
//

typedef struct _PKVPoint
{
	double x;
	double y;

} PKVPoint;

//
// PKVectorEngine
//

class PKVectorEngine
{
public:

	static void initialize();
	static void cleanup();

public:

	PKVectorEngine();
	~PKVectorEngine();

	// Start drawing, end drawing

	bool startWithWindow(PKVWindowHandle window);
	bool startWithDevice(PKVDeviceHandle device);
	
#ifdef MACOSX
	bool startWithCGContext(CGContextRef c);
#endif
	
	void end();

	// Setup

	void setDimensions(uint32_t w, uint32_t h);
	void setGlobalAlpha(double alpha);

	void clipToRect(PKRect rect);

	// Lines

	void drawLine(double x1, double y1,
				  double x2, double y2,
				  PKColor lineColor,
				  double  lineWidth);

	void drawLines(PKVPoint *points,
				   uint32_t  numPoints,
				   PKColor   lineColor,
				   double    lineWidth);

	// Rectangles

	void drawRect(double x, double y,
				  double w, double h,
				  PKColor lineColor,
				  double  lineWidth);

	// Fills

	void fillRect(double x, double y,
				  double w, double h,
				  PKColor fillColor);

#ifdef WIN32

	void fillRect(double x, double y,
				  double w, double h,
				  HBRUSH brush);

#endif 

	void fillCircle(double x, double y,
					double  radius,
					PKColor fillColor);

    void fillTriangle(double x1, double y1,
                      double x2, double y2,
                      double x3, double y3,
                      PKColor fillColor);

	// Strings

	uint32_t getStringWidth(std::wstring family,
							double fontSize,
							PKVFontStyle style,
							std::wstring text);
					

	void drawString(std::wstring family,
					double fontSize,
					PKVFontStyle style,
					PKColor color,
					double x, double y,
					std::wstring text);
	
	// Images

	void drawImage(PKImage *image,
				   double x, double y);

	void drawImage(void *argbBits, int w, int h,
				   double x, double y, double alpha);

#ifdef MACOSX	
	void drawCGImage(CGImageRef image,
				     double x, double y, double alpha);
#endif
	
	void drawImageSubImage(PKImage *image,
				           double x, double y,
				           double w = 0, double h = 0);
	
	void drawImageSubImageWithOffset(PKImage *image,
									 double x, double y,
									 double fromX = 0, double fromY = 0,
									 double w = 0, double h = 0);

	void drawImageStretched(PKImage *image,
				            double x, double y,
				            double w, double h);
	
	void fillStretchWidth(int x, int y, int w, PKImage *image,
						  std::vector<unsigned int> offsets);

private:

#ifdef WIN32
	static ULONG_PTR token;
	Gdiplus::Graphics *graphics;
#endif

#ifdef MACOSX
	CGContextRef ctx;
	bool ownCtx;
#endif
	
#ifdef LINUX
    GdkDrawable *drawable;
    cairo_t *cr;
    GdkGC *gc;
#endif	
	
	uint32_t width;
	uint32_t height;

	double alpha;
};

#endif // _PK_VECTOR_ENGINE
