//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_IMAGE_VIEW_H
#define PK_IMAGE_VIEW_H

#include "PKControl.h"

class PKImage;

//
// PKImageView
//

class PKImageView : public PKControl
{
public:

	static std::string IMAGE_WSTRING_PROPERTY;
    static std::string ALPHA_UINT8_PROPERTY;

public:

	PKImageView();
protected:
	~PKImageView();

public:
    virtual void build();
    virtual void initialize(PKDialog *dialog);
    virtual void destroy();

#ifdef WIN32
    virtual HBRUSH getBackgroundBrush();

    virtual void fillDeviceWithBackground(PKDrawingDevice device,
                                          int x, int y, int w, int h);

public:

	WNDPROC lpOldProc;

	static LRESULT CALLBACK ImageWinProc(HWND hwnd,
							 		     UINT uMsg,
								 	     WPARAM wParam,
                                         LPARAM lParam);
#endif

    PKWindowHandle getWindowHandle();

	// RGBA input
	void setImageBuffer(void *pixelData,
						uint32_t width,
						uint32_t height);

	// Image resource input
	void loadImageFromResource(std::wstring id);

	// Image file input
	void loadImageFromFilename(std::wstring fn);

#ifdef WIN32
	// Alpha Draw method
	void alphaDraw(HDC dc, RECT bounds, HBRUSH brush);
#endif

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

    // From PKControl

	virtual void move(int32_t x, 
					  int32_t y);

	virtual void resize(uint32_t w, 
						uint32_t h);

    // From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

	uint32_t width;
	uint32_t height;
	PKImage *bitmap;
	bool built;

#ifdef WIN32
	HBITMAP hbitmap;
#endif

#ifdef LINUX
    GdkPixbuf *pixbuf;
#endif
};

#endif // PK_IMAGE_VIEW_H
