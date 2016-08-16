//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKObject.h"
#include <typeinfo>

//
// Constructor / Destructor
//

PKObject::PKObject()
{
	// Initialize ref count to 1
	this->refCount = 1;

	// No Parent
	this->parent = NULL;

	// Create property bag
	this->properties = new PKPropertyBag();
	this->properties->setOwner(this);
}

PKObject::~PKObject()
{
	// clear out observers
	for(uint32_t i=0; i < this->observers.size(); i++)
	{
		// Release object
		this->observers[i]->release();
	}
	
	this->observers.clear();
	this->observedProperties.clear();
	this->observerContexts.clear();

	// clear out properties
	delete this->properties;
}

//
// Reference Counting
//
	
uint32_t PKObject::retain()
{
	// Increment and return new value
	return (++this->refCount);
}

uint32_t PKObject::release()
{
	// Decrement and return new value
	// Also, delete if we reach 0 
	// and still return a flat out 0

	this->refCount--;
	
	if(this->refCount == 0)
	{
		delete this;
		return 0;
	}
	
	return this->refCount;
}

//
// Properties API
//

PKPropertyBag *PKObject::getProperties()
{
	return this->properties;
}

bool PKObject::addProperty(std::string   name,
						   PKVariantType type)
{
	return this->properties->addProperty(name, type);
}

PKVariant *PKObject::get(std::string propertyName)
{
	return this->properties->get(propertyName);
}

PKVariant *PKObject::getDefault(std::string propertyName)
{
	return this->properties->getDefault(propertyName);
}

bool PKObject::privateSet(std::string propertyName,
						  PKVariant  *value,
						  PKObject   *fromObject,
						  std::string fromProperty,
						  bool defaultValue)
{
	// Get previous value & copy it

	PKVariant *old = this->get(propertyName);
	PKVariant *oldValue = PKVariantCopy(old);

	// Set the property first

	bool found = this->properties->setNoCallback(propertyName, value, defaultValue);

	// Let ourselves know it has changed

	PKProperty *prop = this->properties->getProperty(propertyName);

	if(prop != NULL)
	{
		this->selfPropertyChanged(prop, oldValue);
	}

	delete oldValue;

	if(found)
	{
		// Check if we have an observer
		// for this property and if so
		// let it know it has changed

		for(uint32_t n=0; n < this->observers.size(); n++)
		{
			if(this->observedProperties[n] == propertyName)
			{
				// Let the object know
				
				this->observers[n]->observeProperty(propertyName,
													this,
													this->observerContexts[n]);
			}
		}	
		
		// Check if we are bound to another
		// properties of another object and
		// set that property accordingly
		// But do not set it if it would go
		// back to the same object and prop
		// as it would loop indefinitely...
		
		for(uint32_t n=0; n < this->boundSelfProperties.size(); n++)
		{
			if(this->boundSelfProperties[n] == propertyName)
			{
				// Update the object
				// if needed
				
				if(fromObject != NULL && fromProperty != "")
				{
					if(fromObject   == this->boundObjects[n] &&
					   fromProperty == this->boundObjectProperties[n])
					{
						// Don't go back to the object
						// that called our method just now
						continue;
					}
				}
				
				PKVariant *prop = this->get(propertyName);
				
				if(prop)
				{
					this->boundObjects[n]->privateSet(this->boundObjectProperties[n], 
													  prop, this, propertyName);
				}
			}
		}	
	
		return true;
	}

	return false;
}

bool PKObject::set(std::string  propertyName,
				   PKVariant   *value,
				   bool defaultValue)
{
	// Use our internal method
	return this->privateSet(propertyName, value, NULL, "", defaultValue);
}

void PKObject::revertAllPropertiesToDefaultValues()
{
	for(uint32_t i=0; i<this->getProperties()->getNumberOfProperties(); i++)
	{
		std::string name = this->getProperties()->getPropertyAt(i)->getName();
		this->set(name, this->getDefault(name));
	}
}

//
// Observer API
//

bool PKObject::addObsverver(PKObject	  *observer,
						    std::string  propertyName,
							void        *context)
{
	// Check if we already
	// observe this property
	
	bool found = false;
	
	for(uint32_t i=0; i < this->observers.size(); i++)
	{
		if(this->observers[i] == observer)
		{
			if(this->observedProperties[i] == propertyName)
			{
				found = true;
				break;
			}
		}
	}
	
	if(found) 
	{
		// Don't add it
		return false;
	}
	
	// Otherwise let's go
	
	observer->retain();
	
	this->observers.push_back(observer);
	this->observedProperties.push_back(propertyName);
	this->observerContexts.push_back(context);
	
	return true;
}
						
bool PKObject::removeObserver(PKObject	  *observer,
						      std::string  propertyName)
{
	std::vector <PKObject *>  newObservers;
	std::vector <std::string> newObservedProperties;
	std::vector <void *>      newObserverContexts;

	// Traverse the list

	bool found = false;
	
	for(uint32_t i=0; i < this->observers.size(); i++)
	{
		if(this->observers[i]		   == observer && 
	       this->observedProperties[i] == propertyName)
		{
			// Release object
			observer->release();

			found = true;
		}
		else
		{
			newObservers.push_back(this->observers[i]);
			newObservedProperties.push_back(this->observedProperties[i]);
			newObserverContexts.push_back(this->observerContexts[i]);
		}
	}
	
	if(found) 
	{
		// Swap out vectors
		
		this->observers = newObservers;
		this->observedProperties = newObservedProperties;
		this->observerContexts = newObserverContexts;
	
		return true;
	}
	
	return false;
}
	
void PKObject::observeProperty(std::string  propertyName,
							   PKObject	 *ofObject,
							   void        *context)
{
	// The default implementation of this
	// method does not do anything ...	
	// Subclasses will implement
}
								 
//
// Bindings API
//
	
bool PKObject::bindToObject(std::string ourPropertyName,
						    PKObject   *toObject,
						    std::string toPropertyName)
{
	// Check if we already
	// bind to this property
	
	if(toObject == NULL)
	{
		return false;
	}
	
	bool found = false;
	
	for(uint32_t i=0; i < this->boundObjects.size(); i++)
	{
		if(this->boundObjects[i] == toObject &&
		   this->boundSelfProperties[i] == ourPropertyName &&
		   this->boundObjectProperties[i] == toPropertyName)
		{
			found = true;
			break;
		}
	}
	
	if(found) 
	{
		// Don't add it
		return false;
	}

	// Else add it if properties 
	// really are the same type
	
	PKVariant *ours   = this->get(ourPropertyName);
	PKVariant *theirs = toObject->get(toPropertyName);
	
	if(ours == NULL)
	{
		return false;
	}
	
	if(theirs == NULL)
	{
		return false;
	}
	
	if(ours->_type != theirs->_type)
	{
		return false;
	}
	
	// If all it good then add it
	
	toObject->retain();
	
	this->boundObjects.push_back(toObject);
	this->boundSelfProperties.push_back(ourPropertyName);
	this->boundObjectProperties.push_back(toPropertyName);	

	// Add ourselve to the other
	// object so that it works two ways

	if(!toObject->isBoundToObject(toPropertyName,
								  this,
								  ourPropertyName))
	{
		// Add it
		toObject->bindToObject(toPropertyName,
							   this,
							   ourPropertyName);
	}

	return true;
}

bool PKObject::isBoundToObject(std::string ourPropertyName,
							   PKObject   *toObject,
							   std::string toPropertyName)
{
	if(toObject == NULL)
	{
		return false;
	}
	
	for(uint32_t i=0; i < this->boundObjects.size(); i++)
	{
		if(this->boundObjects[i] == toObject &&
		   this->boundSelfProperties[i] == ourPropertyName &&
		   this->boundObjectProperties[i] == toPropertyName)
		{
			return true;
		}
	}

	return false;
}

bool PKObject::unbindObject(std::string ourPropertyName,
						    PKObject   *toObject,
						    std::string toPropertyName)
{
	std::vector <PKObject *>  newBoundObjects;
	std::vector <std::string> newBoundSelfProperties;
	std::vector <std::string> newBoundObjectProperties;

	bool found = false;
	
	for(uint32_t i=0; i < this->boundObjects.size(); i++)
	{
		if(this->boundObjects[i] == toObject &&
		   this->boundSelfProperties[i] == ourPropertyName &&
		   this->boundObjectProperties[i] == toPropertyName)
		{
			found = true;
			toObject->release();
		}
		else
		{	
			newBoundObjects.push_back(this->boundObjects[i]);
			newBoundSelfProperties.push_back(this->boundSelfProperties[i]);
			newBoundObjectProperties.push_back(this->boundObjectProperties[i]);
		}
	}
	
	if(found) 
	{
		this->boundObjects = newBoundObjects;
		this->boundSelfProperties = newBoundSelfProperties;
		this->boundObjectProperties = newBoundObjectProperties;

		// Remove ourselves from the other

		if(toObject->isBoundToObject(toPropertyName,
									 this,
									 ourPropertyName))
		{
			// Remove it
			toObject->unbindObject(toPropertyName,
								   this,
								   ourPropertyName);
		}
	
		return true;
	}

	return false;
}

//
// Children API (for XML)
// ----------------------
//

PKObject *PKObject::getParent()
{
	return this->parent;
}

void PKObject::addChild(PKObject *child)
{
	if(child == NULL)
	{
		return;
	}
	
	bool found = false;
	
	for(unsigned int i=0; i < this->children.size(); i++)
	{
		if(this->children[i] == child)
		{
			found = true;
			break;
		}
	}
	
	if(!found)
	{
		this->children.push_back(child);
		child->parent = this;
	}
}

void PKObject::prependChild(PKObject *child)
{
	if(child == NULL)
	{
		return;
	}
	
	bool found = false;
	
	for(unsigned int i=0; i < this->children.size(); i++)
	{
		if(this->children[i] == child)
		{
			found = true;
			break;
		}
	}
	
	if(!found)
	{
    	std::vector<PKObject *> new_children;

        new_children.push_back(child);
        child->parent = this;
        
	    for(unsigned int i=0; i < this->children.size(); i++)
	    {
    	    new_children.push_back(this->children[i]);
        }

        this->children = new_children;        
	}
}

void PKObject::insertChildAt(uint32_t index, PKObject *newChild)
{
	std::vector<PKObject *> new_children;
	
	for(unsigned int i=0; i < this->children.size(); i++)
	{
		if(i != index)
		{
			new_children.push_back(this->children[i]);
		}
		else
		{
			new_children.push_back(newChild);
			new_children.push_back(this->children[i]);
		}
	}

	this->children = new_children;
}

void PKObject::removeChild(PKObject *child)
{
	std::vector<PKObject *> new_children;
	
	for(unsigned int i=0; i < this->children.size(); i++)
	{
		if(this->children[i] != child)
		{
			new_children.push_back(this->children[i]);
		}
		else
		{
			// Un-parent it
			this->children[i]->parent = NULL;
		}
	}

	this->children = new_children;
}

void PKObject::clearChildren()
{
    this->children.clear();
}

uint32_t PKObject::getChildrenCount()
{
	return this->children.size();
}

PKObject *PKObject::getChildAt(uint32_t i)
{
	if(i >= this->children.size())
	{
		return NULL;
	}
	
	return this->children[i];
}

int32_t PKObject::getIndexOfChild(PKObject *child)
{
	for(unsigned int i=0; i < this->children.size(); i++)
	{
        if(this->children[i] == child)
        {
            return i;
        }
    }

    return -1;
}

void PKObject::replaceChildAt(uint32_t i, PKObject *newChild)
{
	if(i >= this->children.size())
	{
		return;
	}
	
	this->children[i] = newChild;	
}

void PKObject::replaceChild(PKObject *oldChild, PKObject *newChild)
{
	for(unsigned int i=0; i < this->children.size(); i++)
	{
		if(this->children[i] == oldChild)
		{
			newChild->parent = this;
			this->children[i]->parent = NULL;
			this->children[i] = newChild;
		}
	}
}

PKObject *PKObject::findChildWithPropertyValue(std::string propertyName,
                                               PKVariant  *propertyValue)
{
    if(propertyName  == "" ||
       propertyValue == NULL)
    {
        return NULL;
    }

    // See if it matches us

    PKVariant *prop = this->get(propertyName);

    if(prop)
    {
        if(PKVariantEquals(prop, propertyValue))
        {
            return this;
        }
    }

    // Check out children

    uint32_t count = this->getChildrenCount();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);

        PKObject *result = child->findChildWithPropertyValue(propertyName,
                                                             propertyValue);

        if(result)
        {
            return result;
        }
    }

    return NULL;
}

void PKObject::walkChildren(PKChildrenWalk wFunc, void *context)
{
    if(wFunc == NULL)
    {
        return;
    }

    // Call it for ourselves

    wFunc(this, context);

    // Walk our children

    uint32_t count = this->getChildrenCount();

    for(uint32_t i=0; i < count; i++)
    {
        PKObject *child = this->getChildAt(i);
        
        if(child)
        {
            child->walkChildren(wFunc, context);
        }
    }
}

void PKObject::walkChildrenBackwards(PKChildrenWalk wFunc, void *context)
{
    if(wFunc == NULL)
    {
        return;
    }

    // Walk our children backwards

    uint32_t count = this->getChildrenCount();

    if(count > 0)
    {
        for(int32_t i=count-1; i >= 0; i--)
        {
            PKObject *child = this->getChildAt(i);
        
            if(child)
            {
                child->walkChildrenBackwards(wFunc, context);
            }
        }
    }
    
    // Call it for ourselves

    wFunc(this, context);
}

void PKObject::walkParents(PKParentsWalk wFunc, void *context)
{
    if(wFunc == NULL)
    {
        return;
    }

    // Call it for ourselves

    wFunc(this, context);

    // Walk our children

	PKObject *parent = this->getParent();

	if(parent)
	{
		parent->walkParents(wFunc, context);
	}
}

//
// XML Output
// ----------
//

std::string PKObject::getClassName()
{
	// Get our class name
	std::string cname = typeid(*this).name();

#ifdef MACOSX
	// Remove the prefix (this assumes a name < 100 chars)
	if(cname.size() <= 10) cname = cname.substr(1);
	else cname = cname.substr(2);
#endif

#ifdef WIN32
	// Remove the "class "
	cname = cname.substr(6);
#endif

	return cname;
}


std::string PKObject::toXML(uint32_t indent)
{
	return this->toXMLWithPropertyHook(NULL, indent);
}

std::string PKObject::toXMLWithPropertyHook(PKXMLPropertyHook hook,
								  uint32_t indent)
{
	std::string result = "";
	unsigned int i;
	
	for(unsigned int k=0; k<indent; k++)
	{
		result += "    ";
	}

	std::string cname = this->getClassName();

	if(this->getChildrenCount() == 0)
	{
		result += "<";
		result += cname;
		
		for(i=0; i < this->properties->getNumberOfProperties(); i++)
		{
			PKVariant *cval = this->properties->getPropertyAt(i)->getValue();
			PKVariant *dval = this->properties->getPropertyAt(i)->getDefaultValue();

			if(!PKVariantEquals(cval, dval)) // Only save if != to default value
			{
				bool addIt = true;
				
				if(hook != NULL)
				{
					addIt = hook(this, this->properties->getPropertyAt(i)->getName());
				}

				if(addIt)
				{
					result += " ";
					result += this->properties->getPropertyAt(i)->toXML();
				}
			}
		}
		
		result += "/>";
	}
	else
	{
		result += "<";
		result += cname;
		
		for(i=0; i < this->properties->getNumberOfProperties(); i++)
		{
			PKVariant *cval = this->properties->getPropertyAt(i)->getValue();
			PKVariant *dval = this->properties->getPropertyAt(i)->getDefaultValue();

			if(!PKVariantEquals(cval, dval)) // Only save if != to default value
			{
				bool addIt = true;
				
				if(hook != NULL)
				{
					addIt = hook(this, this->properties->getPropertyAt(i)->getName());
				}

				if(addIt)
				{
					result += " ";
					result += this->properties->getPropertyAt(i)->toXML();
				}
			}
		}
		
		result += ">";
#ifdef WIN32
		result += "\r";
#endif
		result += "\n";
		
		for(unsigned int n=0; n < this->children.size(); n++)
		{
			result += this->children[n]->toXMLWithPropertyHook(hook, indent+1);
		}

		for(unsigned int k=0; k<indent; k++)
		{
			result += "    ";
		}

		result += "</";
		result += cname;
		result += ">";
	}

#ifdef WIN32
	result += "\r";
#endif
	result += "\n";
	
	return result;
}


// Internal callback for when
// one of our properties is set
// either internally or externally
// Override it if you want
// to update a visual element or
// something like that


void PKObject::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	// The default implementation of this
	// method does not do anything ...	
	// Subclasses will implement
}


// PKPropertyCallback

void PKObject::onPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	// This is called when one of our 
	// properties was changed through the
	// normal setValue() method, but outside of
	// this object, so this is
	// where we look at our bound properties

	// First call our internal handler
	// to reflect any changes

	this->selfPropertyChanged(prop, oldValue);

	// Then see about bound objects

	std::string propertyName = prop->getName();

	// Check if we have an observer
	// for this property and if so
	// let it know it has changed

	for(uint32_t n=0; n < this->observers.size(); n++)
	{
		if(this->observedProperties[n] == propertyName)
		{
			// Let the object know
			
			this->observers[n]->observeProperty(propertyName,
												this,
												this->observerContexts[n]);
		}
	}	
	
	// Check if we are bound to another
	// properties of another object and
	// set that property accordingly
	
	for(uint32_t n=0; n < this->boundSelfProperties.size(); n++)
	{
		if(this->boundSelfProperties[n] == propertyName)
		{
			// Update the object
			// if needed
			
			PKVariant *p = prop->getValue();
			
			if(p)
			{
				this->boundObjects[n]->privateSet(this->boundObjectProperties[n], 
												  p, this, propertyName);
			}
		}
	}	
}
