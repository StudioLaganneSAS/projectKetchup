//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
// 

#include "IPKFSample.h"

//
// IPKFSample
//

IPKFSample::IPKFSample()
{
	PKOBJECT_ADD_BOOL_PROPERTY(locked, false);
}

IPKFSample::~IPKFSample()
{
}

bool IPKFSample::isLocked()
{
	bool result;

	this->mutex.wait();
	PKVariant *locked = this->get("locked");
	result = PKVALUE_BOOL(locked);
	this->mutex.release();

	return result;
}

PKFReturnCode IPKFSample::lock()
{
	this->mutex.wait();
	PKVariantBool v(true);
	this->set("locked", &v);
	this->mutex.release();

	return PKF_SUCCESS;
}

PKFReturnCode IPKFSample::unlock()
{
	this->mutex.wait();
	PKVariantBool v(false);
	this->set("locked", &v);
	this->mutex.release();

	return PKF_SUCCESS;
}

