//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKUIFactory.h"

// Objects

#include "PKButton.h"
#include "PKLayout.h"
#include "PKHLayout.h"
#include "PKVLayout.h"
#include "PKSpacer.h"
#include "PKCheckBox.h"
#include "PKGroupBox.h"
#include "PKImageView.h"
#include "PKEdit.h"
#include "PKComboBox.h"
#include "PKLabel.h"
#include "PKProgressBar.h"
#include "PKSlider.h"
#include "PKTabControl.h"
#include "PKCustomControl.h"
#include "PKLink.h"
#include "PKDoubleBufferedControl.h"
#include "PKAnimationControl.h"
#include "PKPanelControl.h"
#include "PKBitmapButton.h"
#include "PKMenu.h"
#include "PKMenuItem.h"
#include "PKSeparator.h"
#include "PKToolbar.h"
#include "PKToolbarItem.h"
#include "PKToolbarSegmentedItem.h"
#include "PKToolbarSeparator.h"
#include "PKToolbarFixedSpace.h"
#include "PKToolbarFlexibleSpace.h"
#include "PKStretchButton.h"
#include "PKListView.h"
#include "PKGradientPanel.h"
#include "PKLineControl.h"
#include "PKBitmapToggleButton.h"
#include "PKStretchToggleButton.h"
#include "PKListBox.h"
#include "PKStretchEdit.h"
#include "PKFixedLayout.h"
#include "PKScrollView.h"
#include "PKSwitchControl.h"
#include "PKTreeView.h"
#include "PKGridLayout.h"
#include "PKVectorControlRoot.h"
#include "PKIconTabControl.h"
#include "PKBitmapBorderVectorPanel.h"
#include "PKEditableVectorLabel.h"
#include "PKImageVectorControl.h"
#include "PKSelectorControl.h"
#include "PKBitmapProgressBar.h"
#include "PKLittleArrows.h"
#include "PKArrowTabControl.h"

//
// PKUIFactory
//

// Comment out except for special builds
//#define PK_LITE

PKUIFactory::PKUIFactory()
{

    this->objectsAvailable.push_back("PKLayout");
    this->objectsAvailable.push_back("PKButton");
	this->objectsAvailable.push_back("PKVLayout");
	this->objectsAvailable.push_back("PKHLayout");
	this->objectsAvailable.push_back("PKImageView");
	this->objectsAvailable.push_back("PKBitmapButton");
	this->objectsAvailable.push_back("PKCheckBox");
	this->objectsAvailable.push_back("PKFixedLayout");
	this->objectsAvailable.push_back("PKBitmapToggleButton");
	this->objectsAvailable.push_back("PKBitmapProgressBar");

#ifndef PK_LITE
    this->objectsAvailable.push_back("PKSpacer");
	this->objectsAvailable.push_back("PKGroupBox");
	this->objectsAvailable.push_back("PKEdit");
	this->objectsAvailable.push_back("PKComboBox");
	this->objectsAvailable.push_back("PKLabel");
	this->objectsAvailable.push_back("PKProgressBar");
	this->objectsAvailable.push_back("PKSlider");
	this->objectsAvailable.push_back("PKTabControl");
	this->objectsAvailable.push_back("PKCustomControl");
	this->objectsAvailable.push_back("PKLink");
	this->objectsAvailable.push_back("PKDoubleBufferedControl");
	this->objectsAvailable.push_back("PKAnimationControl");
	this->objectsAvailable.push_back("PKPanelControl");
	this->objectsAvailable.push_back("PKStretchButton");
	this->objectsAvailable.push_back("PKListView");
	this->objectsAvailable.push_back("PKGradientPanel");
	this->objectsAvailable.push_back("PKListBox");
	this->objectsAvailable.push_back("PKLineControl");
	this->objectsAvailable.push_back("PKStretchToggleButton");
	this->objectsAvailable.push_back("PKStretchEdit");
	this->objectsAvailable.push_back("PKScrollView");
	this->objectsAvailable.push_back("PKSwitchControl");
	this->objectsAvailable.push_back("PKTreeView");
	this->objectsAvailable.push_back("PKGridLayout");
	this->objectsAvailable.push_back("PKVectorControlRoot");
    this->objectsAvailable.push_back("PKIconTabControl");
	this->objectsAvailable.push_back("PKBitmapBorderVectorPanel");
	this->objectsAvailable.push_back("PKEditableVectorLabel");
    this->objectsAvailable.push_back("PKImageVectorControl");
    this->objectsAvailable.push_back("PKSelectorControl");
	this->objectsAvailable.push_back("PKLittleArrows");
	this->objectsAvailable.push_back("PKArrowTabControl");
#endif // PK_LITE
}


PKUIFactory::~PKUIFactory()
{
}

std::vector<std::string> PKUIFactory::getAvailableObjectsList()
{
	return this->objectsAvailable;
}
		
PKObject *PKUIFactory::createObject(std::string className)
{
	if(className == "PKButton")
	{
		return new PKButton();
	}

	if(className == "PKLayout")
	{
		return new PKLayout();
	}

	if(className == "PKHLayout")
	{
		return new PKHLayout();
	}

	if(className == "PKVLayout")
	{
		return new PKVLayout();
	}

    if(className == "PKCheckBox")
	{
		return new PKCheckBox();
	}

	if(className == "PKImageView")
	{
		return new PKImageView();
	}

    if(className == "PKFixedLayout")
    {
        return new PKFixedLayout();
    }

    if(className == "PKBitmapButton")
    {
        return new PKBitmapButton();
    }

    if(className == "PKBitmapToggleButton")
    {
        return new PKBitmapToggleButton();
    }

	if(className == "PKBitmapProgressBar")
	{
		return new PKBitmapProgressBar();
	}

#ifndef PK_LITE

    if(className == "PKSpacer")
	{
		return new PKSpacer();
	}

	if(className == "PKGroupBox")
	{
		return new PKGroupBox();
	}

    if(className == "PKEdit")
    {
        return new PKEdit();
    }

    if(className == "PKComboBox")
    {
        return new PKComboBox();
    }

    if(className == "PKLabel")
    {
        return new PKLabel();
    }

    if(className == "PKProgressBar")
    {
        return new PKProgressBar();
    }

    if(className == "PKSlider")
    {
        return new PKSlider();
    }

    if(className == "PKTabControl")
    {
        return new PKTabControl();
    }

    if(className == "PKCustomControl")
    {
        return new PKCustomControl();
    }

    if(className == "PKLink")
    {
        return new PKLink();
    }

    if(className == "PKDoubleBufferedControl")
    {
        return new PKDoubleBufferedControl();
    }

    if(className == "PKAnimationControl")
    {
        return new PKAnimationControl();
    }

    if(className == "PKPanelControl")
    {
        return new PKPanelControl();
    }

    if(className == "PKMenu")
    {
        return new PKMenu();
    }

    if(className == "PKMenuItem")
    {
        return new PKMenuItem();
    }

    if(className == "PKSeparator")
    {
        return new PKSeparator();
    }

	if(className == "PKToolbar")
	{
		return new PKToolbar();
	}

	if(className == "PKToolbarItem")
	{
		return new PKToolbarItem();
	}

	if(className == "PKToolbarSegmentedItem")
	{
		return new PKToolbarSegmentedItem();
	}
	
	if(className == "PKToolbarSeparator")
	{
		return new PKToolbarSeparator();
	}
	
	if(className == "PKToolbarFixedSpace")
	{
		return new PKToolbarFixedSpace();
	}

	if(className == "PKToolbarFlexibleSpace")
	{
		return new PKToolbarFlexibleSpace();
	}

    if(className == "PKStretchButton")
    {
        return new PKStretchButton();
    }

    if(className == "PKListView")
    {
        return new PKListView();
    }

    if(className == "PKGradientPanel")
    {
        return new PKGradientPanel();
    }

    if(className == "PKLineControl")
    {
        return new PKLineControl();
    }

    if(className == "PKStretchToggleButton")
    {
        return new PKStretchToggleButton();
    }

    if(className == "PKListBox")
    {
        return new PKListBox();
    }
	
    if(className == "PKStretchEdit")
    {
        return new PKStretchEdit();
    }

    if(className == "PKScrollView")
    {
        return new PKScrollView();
    }

    if(className == "PKSwitchControl")
    {
        return new PKSwitchControl();
    }

    if(className == "PKTreeView")
    {
        return new PKTreeView();
    }

	if(className == "PKGridLayout")
	{
		return new PKGridLayout();
	}

	if(className == "PKVectorControlRoot")
	{
		return new PKVectorControlRoot();
	}

    if(className == "PKIconTabControl")
    {
        return new PKIconTabControl();
    }
	
	if(className == "PKBitmapBorderVectorPanel")
	{
		return new PKBitmapBorderVectorPanel();
	}

	if(className == "PKEditableVectorLabel")
	{
		return new PKEditableVectorLabel();
	}

    if(className == "PKImageVectorControl")
    {
		return new PKImageVectorControl();
    }
	
    if(className == "PKSelectorControl")
    {
        return new PKSelectorControl();
    }

	if(className == "PKLittleArrows")
	{
		return new PKLittleArrows();
	}

	if(className == "PKArrowTabControl")
	{
		return new PKArrowTabControl();
	}
	
#endif // PK_LITE

	return NULL;
}
		
