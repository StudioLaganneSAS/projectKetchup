//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKOBJECT_H
#define PKOBJECT_H

#include "PKTypes.h"
#include "PKVariant.h"
#include "PKPropertyBag.h"
#include "PKPropertyCallback.h"
#include "PKi18n.h"

#include <string>
#include <vector>

class PKObject;

//
// PKSelector function type
// 

typedef void (*PKSelectorFunction)(PKVariant *context);

//
// PKChildrenWalk function type
//

typedef void (*PKChildrenWalk)(PKObject *child, void *context);

//
// PKParentWalk function type
//

typedef void (*PKParentsWalk)(PKObject *child, void *context);

// XML output property filter hook

typedef bool (*PKXMLPropertyHook)(PKObject *object, std::string propertyName);

//
// PKObject class
// --------------
//
// This class implements cocoa-like base object
// features such as reference counting and
// properties, as well as KVC/KVO and bindings
//
//

class PKObject : PKPropertyCallback {

public:

	PKObject();
	virtual ~PKObject();

	//
	// Reference Counting
	// ------------------
	//
	
	// Use retain() to increment the object's
	// reference count, i.e. when you want the
	// object to last in scope. Don't forget
	// to also call release() when you are done
	
	virtual uint32_t retain();
	virtual uint32_t release();

	//
	// Properties API
	// --------------
	//
	
	// Use addProperty to add a property to an
	// object, this should be done in the constructor
	// or your object and not later on
	
	virtual bool addProperty(std::string   name,
							 PKVariantType type);
	
	// Use get() and set() to retreive and
	// change property values of the object
	// ALways use set() instead of directly
	// altering an object property so that the
	// Observer API can work.
	
	virtual PKVariant *get(std::string propertyName);

	virtual PKVariant *getDefault(std::string propertyName);

	virtual bool set(std::string  propertyName,
					 PKVariant   *value,
					 bool defaultValue = false);

	virtual void revertAllPropertiesToDefaultValues();
	
	//
	// PropertyBag getter
	//

	PKPropertyBag *getProperties();

	//								
	// Observer Pattern (KVO)
	// ----------------------
	//
	
	// Use addObserver() to add another
	// object as an observer of one of 
	// this object perperties.
	// context is a void * pointer you
	// can pass to the method and it will
	// be passed back when the prop changes
	//
	
	virtual bool addObsverver(PKObject	  *observer,
							  std::string  propertyName,
							  void        *context);
							  
	virtual bool removeObserver(PKObject	*observer,
							    std::string  propertyName);
	
	// Once an observer is registered
	// the following method will be called
	// when a property that is being 
	// observes changes value over time
	// You need to override it and implement
	// but if you override it, be sure
	// to call the original class method 
	// as well so that bindings can work
	
	virtual void observeProperty(std::string  propertyName,
								 PKObject	 *ofObject,
								 void        *context);
								 
	//
	// Bindings API
	// ------------
	//
	
	// Bindings use KVC/KVO internally to bind
	// properties of two objects so that when
	// one changes, the other is altered as well
	// Bound properties must have the same variant
	// type underneath for it to work...
	// Bindings always work two ways
	
	virtual bool bindToObject(std::string ourPropertyName,
							  PKObject   *toObject,
							  std::string toPropertyName);
							  
	virtual bool unbindObject(std::string ourPropertyName,
							  PKObject   *toObject,
							  std::string toPropertyName);
	
	virtual bool isBoundToObject(std::string ourPropertyName,
							     PKObject   *toObject,
							     std::string toPropertyName);

	//
	// Children API
	// ------------
	//
	
	virtual PKObject *getParent();

	virtual void addChild(PKObject *child);
	virtual void prependChild(PKObject *child);
	virtual void removeChild(PKObject *child);
	virtual void insertChildAt(uint32_t i, PKObject *newChild);

    virtual void clearChildren();
	
	virtual uint32_t  getChildrenCount();
	virtual PKObject *getChildAt(uint32_t i);
	
	virtual void replaceChildAt(uint32_t i, PKObject *newChild);
	virtual void replaceChild(PKObject *oldChild, PKObject *newChild);

    virtual int32_t getIndexOfChild(PKObject *child);
	
    virtual PKObject *findChildWithPropertyValue(std::string propertyName,
                                                 PKVariant  *propertyValue);

    virtual void walkChildren(PKChildrenWalk wFunc, void *context);
    virtual void walkChildrenBackwards(PKChildrenWalk wFunc, void *context);
    virtual void walkParents(PKParentsWalk wFunc, void *context);

	//
	// XML Output
	// ----------
	//

	std::string getClassName();
	
	virtual std::string toXML(uint32_t indent = 0);
	
	virtual std::string toXMLWithPropertyHook(PKXMLPropertyHook hook,
											  uint32_t indent = 0);

	// Internal callback for when
	// one of our properties is set
	// either internally or externally
	// Override it if you want
	// to update a visual element or
	// something like that

	virtual void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

protected:

	uint32_t  refCount;
	PKObject *parent;

private:

	PKPropertyBag *properties;
	
	std::vector <PKObject *> children;
	
	std::vector <PKObject *>  observers;
	std::vector <std::string> observedProperties;
	std::vector <void *>      observerContexts;
	
	// Bindings API helper
	// this is to avoid loops
	
	bool privateSet(std::string propertyName,
					PKVariant  *value,
					PKObject   *fromObject,
					std::string fromProperty,
					bool defaultValue = false);
	
	std::vector <PKObject *>  boundObjects;
	std::vector <std::string> boundSelfProperties;
	std::vector <std::string> boundObjectProperties;

	// PKPropertyCallback

	void onPropertyChanged(PKProperty *prop, PKVariant *oldValue);
};

//
// Properties helper macros
// ------------------------
// 
// You can use these macros to easily
// add and initialize properties
// from your PKObject subclass constructor
//

// bool

#define PKOBJECT_ADD_BOOL_PROPERTY(propertyName, defaultValue) \
															   \
	PKVariantBool val##propertyName(defaultValue);			   \
															   \
	this->addProperty(#propertyName, PK_VARIANT_BOOL);		   \
	this->set(#propertyName, &val##propertyName, true);

// int8_t

#define PKOBJECT_ADD_INT8_PROPERTY(propertyName, defaultValue) \
															   \
	PKVariantInt8 val##propertyName(defaultValue);			   \
															   \
	this->addProperty(#propertyName, PK_VARIANT_INT8);		   \
	this->set(#propertyName, &val##propertyName, true);
	
// int16_t

#define PKOBJECT_ADD_INT16_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantInt16 val##propertyName(defaultValue);			    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_INT16);		    \
	this->set(#propertyName, &val##propertyName, true);

// int32_t

#define PKOBJECT_ADD_INT32_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantInt32 val##propertyName(defaultValue);			    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_INT32);		    \
	this->set(#propertyName, &val##propertyName, true);

// int64_t

#define PKOBJECT_ADD_INT64_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantInt64 val##propertyName(defaultValue);			    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_INT64);		    \
	this->set(#propertyName, &val##propertyName, true);

// uint8_t

#define PKOBJECT_ADD_UINT8_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantUInt8 val##propertyName(defaultValue);			    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_UINT8);		    \
	this->set(#propertyName, &val##propertyName, true);

// uint16_t

#define PKOBJECT_ADD_UINT16_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantUInt16 val##propertyName(defaultValue);		    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_UINT16);	    \
	this->set(#propertyName, &val##propertyName, true);

// uint32_t

#define PKOBJECT_ADD_UINT32_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantUInt32 val##propertyName(defaultValue);		    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_UINT32);	    \
	this->set(#propertyName, &val##propertyName, true);

// uint64_t

#define PKOBJECT_ADD_UINT64_PROPERTY(propertyName, defaultValue) \
															     \
	PKVariantUInt64 val##propertyName(defaultValue);  		     \
															     \
	this->addProperty(#propertyName, PK_VARIANT_UINT64);	     \
	this->set(#propertyName, &val##propertyName, true);

// double

#define PKOBJECT_ADD_DOUBLE_PROPERTY(propertyName, defaultValue) \
															     \
	PKVariantDouble val##propertyName(defaultValue);   		     \
															     \
	this->addProperty(#propertyName, PK_VARIANT_DOUBLE);	     \
	this->set(#propertyName, &val##propertyName, true);

// void *

#define PKOBJECT_ADD_VOIDP_PROPERTY(propertyName, defaultValue) \
															    \
	PKVariantVoidP val##propertyName(defaultValue);			    \
															    \
	this->addProperty(#propertyName, PK_VARIANT_VOIDP);		    \
	this->set(#propertyName, &val##propertyName, true);

// std::string

#define PKOBJECT_ADD_STRING_PROPERTY(propertyName, defaultValue) \
															     \
	PKVariantString val##propertyName(defaultValue);		     \
															     \
	this->addProperty(#propertyName, PK_VARIANT_STRING);	     \
	this->set(#propertyName, &val##propertyName, true);

// std::wstring

#define PKOBJECT_ADD_WSTRING_PROPERTY(propertyName, defaultValue) \
															      \
	PKVariantWString val##propertyName(defaultValue);			  \
															      \
	this->addProperty(#propertyName, PK_VARIANT_WSTRING);	      \
	this->set(#propertyName, &val##propertyName, true);

// PKPoint

#define PKOBJECT_ADD_POINT_PROPERTY(propertyName, v) \
															      \
	PKVariantPoint val##propertyName(v.x, v.y);	                  \
															      \
	this->addProperty(#propertyName, PK_VARIANT_POINT);	          \
	this->set(#propertyName, &val##propertyName, true);

// PKRect

#define PKOBJECT_ADD_RECT_PROPERTY(propertyName, v) \
															      \
	PKVariantRect val##propertyName(v.x, v.y, v.w, v.h);	      \
															      \
	this->addProperty(#propertyName, PK_VARIANT_RECT);	          \
	this->set(#propertyName, &val##propertyName, true);

// PKColor

#define PKOBJECT_ADD_COLOR_PROPERTY(propertyName, v) \
															      \
	PKVariantColor val##propertyName(v.r, v.g, v.b, v.a);	      \
															      \
	this->addProperty(#propertyName, PK_VARIANT_COLOR);	          \
	this->set(#propertyName, &val##propertyName, true);

// std::string list

#define PKOBJECT_ADD_STRINGLIST_PROPERTY(propertyName, defaultValue) \
															         \
	PKVariantStringList val##propertyName(defaultValue);		     \
															         \
	this->addProperty(#propertyName, PK_VARIANT_STRINGLIST);	     \
	this->set(#propertyName, &val##propertyName, true);

// std::wstring list

#define PKOBJECT_ADD_WSTRINGLIST_PROPERTY(propertyName, defaultValue) \
															          \
	PKVariantWStringList val##propertyName(defaultValue);			  \
															          \
	this->addProperty(#propertyName, PK_VARIANT_WSTRINGLIST);	      \
	this->set(#propertyName, &val##propertyName, true);

// PKInt32Range

#define PKOBJECT_ADD_INT32RANGE_PROPERTY(propertyName, v)         \
														          \
	PKVariantInt32Range val##propertyName(v.min, v.max);          \
														          \
	this->addProperty(#propertyName, PK_VARIANT_INT32RANGE);	  \
	this->set(#propertyName, &val##propertyName, true);

// PKInt64Range

#define PKOBJECT_ADD_INT64RANGE_PROPERTY(propertyName, v)         \
														          \
	PKVariantInt64Range val##propertyName(v.min, v.max);          \
														          \
	this->addProperty(#propertyName, PK_VARIANT_INT64RANGE);	  \
	this->set(#propertyName, &val##propertyName, true);

// PKDoubleRange

#define PKOBJECT_ADD_DOUBLERANGE_PROPERTY(propertyName, v)        \
														          \
	PKVariantDoubleRange val##propertyName(v.min, v.max);         \
														          \
	this->addProperty(#propertyName, PK_VARIANT_DOUBLERANGE);	  \
	this->set(#propertyName, &val##propertyName, true);

// Binary buffer

#define PKOBJECT_ADD_BINARY_PROPERTY(propertyName, size, buffer)  \
														          \
	PKVariantBinary val##propertyName(size, buffer);              \
														          \
	this->addProperty(#propertyName, PK_VARIANT_BINARY);	      \
	this->set(#propertyName, &val##propertyName, true);

#endif // PKOBJECT_H
