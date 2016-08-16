//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKVLayout.h"

//
// PKHLayout
// 

PKVLayout::PKVLayout()
{
	PKVariantInt32 type(PK_LAYOUT_VERTICAL);
	this->set(PKLayout::TYPE_INT32_PROPERTY, &type);
}

PKVLayout::~PKVLayout()
{
}

