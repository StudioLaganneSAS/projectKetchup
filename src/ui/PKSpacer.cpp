//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKSpacer.h"

//
// Properties
//

std::string PKSpacer::TYPE_INT32_PROPERTY = "type";

//
// PKSpacer
//

PKSpacer::PKSpacer()
{
	PKOBJECT_ADD_INT32_PROPERTY(type, PK_SPACER_HORIZONTAL);
}

PKSpacer::~PKSpacer()
{

}

void PKSpacer::build()
{
    PKControl::build();
    
#ifdef LINUX

    // Create a dummy widget
    this->widget = gtk_drawing_area_new();

#endif
}

void PKSpacer::setSpacerType(PKSpacerType type)
{
	PKVariantInt32 t(type);
	this->set(TYPE_INT32_PROPERTY, &t);
}

PKSpacerType PKSpacer::getSpacerType()
{
	PKVariant *type = this->get(TYPE_INT32_PROPERTY);
	return (PKSpacerType) PKVALUE_INT32(type);
}

uint32_t PKSpacer::getMinimumWidth()
{
	if(this->getLayoutPolicyX() == PK_LAYOUT_FIXED)
	{
		return this->getW();
	}

	return 0;
}

uint32_t PKSpacer::getMinimumHeight()
{
	if(this->getLayoutPolicyY() == PK_LAYOUT_FIXED)
	{
		return this->getH();
	}

	return 0;
}
