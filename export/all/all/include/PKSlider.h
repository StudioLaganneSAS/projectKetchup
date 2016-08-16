//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_SLIDER_H
#define PK_SLIDER_H

#include "PKControl.h"

#ifdef MACOSX

enum {
  kEventClassPKSlider  = FOUR_CHAR_CODE('PKSL'),
  kEventPKSliderTrack  = 1,
};

#endif

//
// PKSlider
//

class PKSlider : public PKControl
{
public:

    static std::string RANGE_INT32RANGE_PROPERTY;
    static std::string VALUE_INT32_PROPERTY;

public:
	PKSlider();
protected:
	~PKSlider();

public:
    void build();

	void setMinMax(int32_t min, 
				   int32_t max);
	
	void	setValue(int32_t value);
	int32_t getValue();

	uint32_t getMinimumWidth();
	uint32_t getMinimumHeight();

	// From PKObject
	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

private:

#ifdef LINUX
    
    static void gtk_slider_value_changed(GtkRange *range,
                                         gpointer  user_data);
                                         
    void linuxValueChanged();                                     
    
#endif

#ifdef MACOSX
	static pascal void SliderAction(ControlRef theControl, ControlPartCode partCode);
#endif
};

#endif // PK_SLIDER_H
