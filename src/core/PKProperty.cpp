//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKProperty.h"

//
// PKProperty
//

PKProperty::PKProperty(std::string name, PKVariantType type)
{
	this->name  = name;
	this->value = PKVariant_Create(type);
	this->defaultValue = PKVariant_Create(type);
	this->callback = NULL;
}

PKProperty::~PKProperty()
{
	if(this->value != NULL)
	{
		delete this->value;
	}

	if(this->defaultValue != NULL)
	{
		delete this->defaultValue;
	}
}

void PKProperty::setPropertyCallback(PKPropertyCallback *callback)
{
	this->callback = callback;
}

std::string PKProperty::getName()
{
	return this->name;
}

void PKProperty::setName(std::string n)
{
	this->name = n;
}

PKVariant *PKProperty::getValue()
{
	return this->value;
}

PKVariant *PKProperty::getDefaultValue()
{
	return this->defaultValue;
}

void PKProperty::setValueNoCallback(PKVariant *val,		
			 					    bool defaultValue)
{
	if(this->value == NULL)
	{
		return;
	}

	if(val->_type != this->value->_type)
	{
		// Incompatible types
		return;
	}

	// Else, let's go

	switch(val->_type)
	{
	case PK_VARIANT_NULL:
		break;
	
	case PK_VARIANT_BOOL:
		PKVALUE_BOOL_SET(this->value, PKVALUE_BOOL(val));
		if(defaultValue)
			PKVALUE_BOOL_SET(this->defaultValue, PKVALUE_BOOL(val));
		break;

	case PK_VARIANT_INT64:
		PKVALUE_INT64_SET(this->value, PKVALUE_INT64(val));
		if(defaultValue)
			PKVALUE_INT64_SET(this->defaultValue, PKVALUE_INT64(val));
		break;

	case PK_VARIANT_INT32:
		PKVALUE_INT32_SET(this->value, PKVALUE_INT32(val));
		if(defaultValue)
			PKVALUE_INT32_SET(this->defaultValue, PKVALUE_INT32(val));
		break;

	case PK_VARIANT_INT16:
		PKVALUE_INT16_SET(this->value, PKVALUE_INT16(val));
		if(defaultValue)
			PKVALUE_INT16_SET(this->defaultValue, PKVALUE_INT16(val));
		break;

	case PK_VARIANT_INT8:
		PKVALUE_INT8_SET(this->value, PKVALUE_INT8(val));
		if(defaultValue)
			PKVALUE_INT8_SET(this->defaultValue, PKVALUE_INT8(val));
		break;

	case PK_VARIANT_UINT64:
		PKVALUE_UINT64_SET(this->value, PKVALUE_UINT64(val));
		if(defaultValue)
			PKVALUE_UINT64_SET(this->defaultValue, PKVALUE_UINT64(val));
		break;

	case PK_VARIANT_UINT32:
		PKVALUE_UINT32_SET(this->value, PKVALUE_UINT32(val));
		if(defaultValue)
			PKVALUE_UINT32_SET(this->defaultValue, PKVALUE_UINT32(val));
		break;

	case PK_VARIANT_UINT16:
		PKVALUE_UINT16_SET(this->value, PKVALUE_UINT16(val));
		if(defaultValue)
			PKVALUE_UINT16_SET(this->defaultValue, PKVALUE_UINT16(val));
		break;

	case PK_VARIANT_UINT8:
		PKVALUE_UINT8_SET(this->value, PKVALUE_UINT8(val));
		if(defaultValue)
			PKVALUE_UINT8_SET(this->defaultValue, PKVALUE_UINT8(val));
		break;

	case PK_VARIANT_DOUBLE:
		PKVALUE_DOUBLE_SET(this->value, PKVALUE_DOUBLE(val));
		if(defaultValue)
			PKVALUE_DOUBLE_SET(this->defaultValue, PKVALUE_DOUBLE(val));
		break;

	case PK_VARIANT_VOIDP:
		PKVALUE_VOIDP_SET(this->value, PKVALUE_VOIDP(val));
		if(defaultValue)
			PKVALUE_VOIDP_SET(this->defaultValue, PKVALUE_VOIDP(val));
		break;

	case PK_VARIANT_STRING:
		PKVALUE_STRING_SET(this->value, PKVALUE_STRING(val));
		if(defaultValue)
			PKVALUE_STRING_SET(this->defaultValue, PKVALUE_STRING(val));
		break;

	case PK_VARIANT_WSTRING:
        {
            PKVariantWStringI18N *i18n = dynamic_cast<PKVariantWStringI18N*>(val);

            if(i18n)
            {
				std::wstring newVal = PKVALUE_WSTRING(val);

				if(newVal.size() > 7 && newVal.substr(0, 7) == L"PKI18N:")
				{
					newVal = newVal.substr(7);
				}				

                std::wstring original = ((PKVariantWString *)this->value)->_val;

                if(original == L"NOT_SET")
                {
                    original = newVal;
                }

                PKVariantWStringI18N *current = dynamic_cast<PKVariantWStringI18N*>(this->value);

                if(current)
                {
                    original = current->_original;
                }
                
                delete this->value;
                this->value = new PKVariantWStringI18N(original);
                
                PKVALUE_WSTRING_SET(this->value, newVal);
				if(defaultValue)
	                PKVALUE_WSTRING_SET(this->defaultValue, newVal);
            }
            else
            {
        		PKVALUE_WSTRING_SET(this->value, PKVALUE_WSTRING(val));
				if(defaultValue)
	        		PKVALUE_WSTRING_SET(this->defaultValue, PKVALUE_WSTRING(val));
            }           
        }
        break;

	case PK_VARIANT_RECT:
		PKVALUE_RECT_SET(this->value, PKVALUE_RECT(val));
		if(defaultValue)
			PKVALUE_RECT_SET(this->defaultValue, PKVALUE_RECT(val));
		break;

	case PK_VARIANT_POINT:
		PKVALUE_POINT_SET(this->value, PKVALUE_POINT(val));
		if(defaultValue)
			PKVALUE_POINT_SET(this->defaultValue, PKVALUE_POINT(val));
		break;

	case PK_VARIANT_COLOR:
		PKVALUE_COLOR_SET(this->value, PKVALUE_COLOR(val));
		if(defaultValue)
			PKVALUE_COLOR_SET(this->defaultValue, PKVALUE_COLOR(val));
		break;

   	case PK_VARIANT_STRINGLIST:
		PKVALUE_STRINGLIST_SET(this->value, PKVALUE_STRINGLIST(val));
		if(defaultValue)
			PKVALUE_STRINGLIST_SET(this->defaultValue, PKVALUE_STRINGLIST(val));
		break;

	case PK_VARIANT_WSTRINGLIST:
        {
            PKVariantWStringListI18N *i18n = dynamic_cast<PKVariantWStringListI18N*>(val);

            if(i18n)
            {
                std::vector<std::wstring> original = ((PKVariantWStringList *)this->value)->_val;

                if(original.size() == 0)
                {
                    original = PKVALUE_WSTRINGLIST(val);
                }

                PKVariantWStringListI18N *current = dynamic_cast<PKVariantWStringListI18N*>(this->value);

                if(current)
                {
                    original = current->_original;
                }

                delete this->value;
                this->value = new PKVariantWStringListI18N(original);

                PKVALUE_WSTRINGLIST_SET(this->value, PKVALUE_WSTRINGLIST(val));
				if(defaultValue)
	                PKVALUE_WSTRINGLIST_SET(this->defaultValue, PKVALUE_WSTRINGLIST(val));
            }
            else
            {
        		PKVALUE_WSTRINGLIST_SET(this->value, PKVALUE_WSTRINGLIST(val));
				if(defaultValue)
	        		PKVALUE_WSTRINGLIST_SET(this->defaultValue, PKVALUE_WSTRINGLIST(val));
            }
        }
        break;

	case PK_VARIANT_INT32RANGE:
		PKVALUE_INT32RANGE_SET(this->value, PKVALUE_INT32RANGE(val));
		if(defaultValue)
			PKVALUE_INT32RANGE_SET(this->defaultValue, PKVALUE_INT32RANGE(val));
		break;

	case PK_VARIANT_INT64RANGE:
		PKVALUE_INT64RANGE_SET(this->value, PKVALUE_INT64RANGE(val));
		if(defaultValue)
			PKVALUE_INT64RANGE_SET(this->defaultValue, PKVALUE_INT64RANGE(val));
		break;

	case PK_VARIANT_DOUBLERANGE:
		PKVALUE_DOUBLERANGE_SET(this->value, PKVALUE_DOUBLERANGE(val));
		if(defaultValue)
			PKVALUE_DOUBLERANGE_SET(this->defaultValue, PKVALUE_DOUBLERANGE(val));
		break;

	case PK_VARIANT_BINARY:
		{
			uint32_t size = PKVALUE_BINARY_SIZE(val);
			void    *buff = PKVALUE_BINARY_BUFFER(val);

			PKVariantBinary *_ourvalue = (PKVariantBinary *) this->value;
			_ourvalue->_size = size;

			if(_ourvalue->_buffer != NULL)
			{
				delete [] _ourvalue->_buffer;
				_ourvalue->_buffer = NULL;
			}

			if(size > 0)
			{
				_ourvalue->_buffer = new char[size];

				if(_ourvalue->_buffer)
				{
					memcpy(_ourvalue->_buffer, buff, size);
				}
			}

			if(defaultValue)
			{
				PKVariantBinary *_defvalue = (PKVariantBinary *) this->defaultValue;
				_defvalue->_size = size;

				if(_defvalue->_buffer != NULL)
				{
					delete [] _defvalue->_buffer;
					_defvalue->_buffer = NULL;
				}

				if(size > 0)
				{
					_defvalue->_buffer = new char[size];

					if(_defvalue->_buffer)
					{
						memcpy(_defvalue->_buffer, buff, size);
					}
				}
			}
		}
		break;
    }
}

std::string PKProperty::toXML()
{
	std::string result = "";
	
	result += this->name;
	result += "=\"";
	result += this->value->toString();
	result += "\"";
	
	return result;
}

void PKProperty::setValue(PKVariant *val, 
  					      bool defaultValue)
{ 
	// Copy current value
	PKVariant *oldValue = PKVariantCopy(this->getValue());

	// Set new value

	this->setValueNoCallback(val, defaultValue);

	// Callback

	if(this->callback != NULL)
	{
		this->callback->onPropertyChanged(this, oldValue);
	}

	delete oldValue;
}
