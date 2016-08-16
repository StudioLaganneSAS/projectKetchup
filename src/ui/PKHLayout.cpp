//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKHLayout.h"

//
// PKHLayout
// 

PKHLayout::PKHLayout()
{
	PKVariantInt32 type(PK_LAYOUT_HORIZONTAL);
	this->set(PKLayout::TYPE_INT32_PROPERTY, &type);
}

PKHLayout::~PKHLayout()
{
}

