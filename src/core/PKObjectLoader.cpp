//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKObjectLoader.h"
#include "PKStr.h"
#include "PKBase64.h"

//
// PKObjectLoader
//

PKObjectLoader::PKObjectLoader()
{
	this->error = false;
}

PKObjectLoader::~PKObjectLoader()
{
}

//
// Setup
//

void PKObjectLoader::addObjectFactory(IPKObjectFactory *factory)
{
	this->factories.push_back(factory);
}

//
// Loading
//

bool PKObjectLoader::getError()
{
	return this->error;
}

PKObject *PKObjectLoader::loadFromXML(std::string xml)
{
	std::string current;
	
	this->error = false;
	
	current = this->skipWhitespace(xml);		

	if(current[0] != '<')
	{
		// Not valid XML
		this->error = true;
		return NULL;
	}

	// Find the first whitespace or / or >

	int32_t spaceIndex = current.find_first_of(" ");
	int32_t slashIndex = current.find_first_of("/");
	int32_t superIndex = current.find_first_of(">");

	if(spaceIndex == std::string::npos &&
	   slashIndex == std::string::npos &&
       superIndex == std::string::npos)
	{
		// Not valid XML
		this->error = true;
		return NULL;
	}

    if(spaceIndex == std::string::npos) spaceIndex = 32768;
    if(slashIndex == std::string::npos) slashIndex = 32768;
    if(superIndex == std::string::npos) superIndex = 32768;

    int32_t minIndex = pk_min(pk_min(spaceIndex, slashIndex), superIndex);

	std::string className = current.substr(1, minIndex-1);
	current = current.substr(minIndex);

	PKObject *obj = NULL;

	for(unsigned int i=0; i<this->factories.size(); i++)
	{
		obj = this->factories[i]->createObject(className);

		if(obj != NULL)
		{
			break;
		}
	}

	if(obj == NULL)
	{
		this->error = true;
		return NULL;
	}

	this->loadObject(obj, className, current);

	return obj;
}

//
// Local stuff
//

std::string PKObjectLoader::skipWhitespace(std::string src)
{
	unsigned int index=0; 

	while((src[index] == ' '   ||
		   src[index] == '\t'  ||
		   src[index] == '\n'  ||
		   src[index] == '\r') &&
		   index < src.size())
	{
		index++;
	}

	return src.substr(index);
}

std::string PKObjectLoader::loadObject(PKObject *object,
									   std::string cName,
									   std::string xml)
{
	std::string current = this->skipWhitespace(xml);

	if(current.size() == 0)
	{
		return "";
	}

	if(current[0] != '/' &&
	   current[0] != '>')
	{
		do
		{
			current = this->skipWhitespace(current);

			if(current.size() == 0)
			{
				return "";
			}

			if(current[0] == '/' ||
			   current[0] == '>')
			{
				break;
			}

			int32_t equalPos = current.find_first_of("=");

			if(equalPos == std::string::npos)
			{
				// Not valid XML
				this->error = true;
				return "";
			}

			std::string propertyName = current.substr(0, equalPos);
			propertyName = PKStr::stripBeginingAndEndChar(propertyName, ' ');

			// Find the property value

			current = current.substr(equalPos+1);
			current = this->skipWhitespace(current);

			if(current.size() == 0)
			{
				return "";
			}

			if(current[0] != '\"')
			{
				return "";
			}

			current = current.substr(1);

			int32_t quotePos = current.find_first_of("\"");

			if(quotePos == std::string::npos)
			{
				return "";
			}

			std::string value = current.substr(0, quotePos);

			// Find this property in the object
			// and load the value into it

			this->setObjectProperty(object, propertyName, value);

			current = current.substr(quotePos+1);
		}
		while(true);
	}

	// Now check how this ends

	current = this->skipWhitespace(current);

	if(current.size() == 0)
	{
		return "";
	}

	if(current[0] == '/')
	{
		// End of this object
		// No children

		current = current.substr(2);
		return current;
	}

	if(current[0] != '>')
	{
		// Not valid XML
		this->error = true;
		return "";
	}

	current = current.substr(1);

	// Now load children

	do
	{
		current = this->skipWhitespace(current);

		// Check what we have

		if(current.size() < 2)
		{
			return "";
		}

		if(current[0] != '<')
		{
			// Invalid XML
			this->error = true;
			return "";
		}

		if(current[1] == '/')
		{
			// Break (this is the end of this object)
			current = current.substr(2);
			int32_t superIndex = current.find_first_of(">");
			current  = current.substr(superIndex+1);
			return current;
		}

		int32_t spaceIndex = current.find_first_of(" ");
		int32_t slashIndex = current.find_first_of("/");
		int32_t superIndex = current.find_first_of(">");

		if(spaceIndex == std::string::npos &&
		   slashIndex == std::string::npos &&
		   superIndex == std::string::npos)
		{
			// Not valid XML
			this->error = true;
			return NULL;
		}

        if(spaceIndex == std::string::npos) spaceIndex = 32768;
        if(slashIndex == std::string::npos) slashIndex = 32768;
        if(superIndex == std::string::npos) superIndex = 32768;

		int32_t minIndex = pk_min(pk_min(spaceIndex, slashIndex), superIndex);

		std::string className = current.substr(1, minIndex-1);
		current = current.substr(minIndex);

		PKObject *obj = NULL;

		for(unsigned int i=0; i<this->factories.size(); i++)
		{
			obj = this->factories[i]->createObject(className);

			if(obj != NULL)
			{
				break;
			}
		}

		if(obj == NULL)
		{
			return "";
		}

		object->addChild(obj);

		current = this->loadObject(obj, className, current);
	}
	while(true);

	return current;
}

void PKObjectLoader::setObjectProperty(PKObject *object,
									   std::string name,
									   std::string value)
{
	if(object == NULL)
	{
		this->error = true;
		return;
	}

	PKVariant *prop = object->get(name);

	if(prop == NULL)
	{
		this->error = true;
		return;
	}

	PKVariantType type = prop->_type;

	switch(type)
	{
	case PK_VARIANT_NULL:
		{
			// Nothing to do
		}
		break;

	case PK_VARIANT_BOOL:
		{
			bool v = false;
			if(PKStr::toLower(value) == "true")
			{
				v = true;
			}
			PKVariantBool vb(v);
			object->set(name, &vb);
		}
		break;

	case PK_VARIANT_INT64:
		{
			int64_t v = PKStr::stringToInt64(value);
			PKVariantInt64 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_INT32:
		{
			int32_t v = PKStr::stringToInt32(value);
			PKVariantInt32 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_INT16:
		{
			int16_t v = (int16_t) PKStr::stringToInt32(value);
			PKVariantInt16 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_INT8:
		{
			int8_t v = (int8_t) PKStr::stringToInt32(value);
			PKVariantInt8 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_UINT64:
		{
			uint64_t v = PKStr::stringToUint64(value);
			PKVariantUInt64 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_UINT32:
		{
			uint32_t v = PKStr::stringToUInt32(value);
			PKVariantUInt32 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_UINT16:
		{
			uint16_t v = (uint16_t) PKStr::stringToUInt32(value);
			PKVariantUInt16 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_UINT8:
		{
			uint8_t v = (uint8_t) PKStr::stringToUInt32(value);
			PKVariantUInt8 vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_DOUBLE:
		{
			double v = PKStr::stringToDouble(value);
			PKVariantDouble vv(v);
			object->set(name, &vv);
		}
		break;

	case PK_VARIANT_VOIDP:
		{
			// Not possible
		}
		break;

	case PK_VARIANT_STRING:
		{
			PKVariantString vs(value);
			object->set(name, &vs);
		}
		break;

	case PK_VARIANT_WSTRING:
		{
            std::wstring v;

            if(PKStr::findFirstOf(value, "PKI18N:") == 0)
            {
                // This is a i18n wstring
                value = value.substr(7);
    
                v = PKStr::stringToWString(value);

                PKVariantWStringI18N vv(v);
			    object->set(name, &vv);
            }
            else
            {
    			v = PKStr::utf8StringToWstring(value);

                PKVariantWString vv(v);
    			object->set(name, &vv);
            }
		}
		break;

	case PK_VARIANT_POINT:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 2)
			{
				PKVariantPoint pt(PKStr::stringToInt32(values[0]),
								  PKStr::stringToInt32(values[1]));
			
				object->set(name, &pt);
			}
		}
		break;

	case PK_VARIANT_RECT:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 4)
			{
				PKVariantRect  rc(PKStr::stringToInt32(values[0]),
								  PKStr::stringToInt32(values[1]),
								  PKStr::stringToInt32(values[2]),
								  PKStr::stringToInt32(values[3]));
			
				object->set(name, &rc);
			}
		}
		break;

	case PK_VARIANT_COLOR:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 4)
			{
				PKVariantColor c(PKStr::stringToInt32(values[0]),
								 PKStr::stringToInt32(values[1]),
								 PKStr::stringToInt32(values[2]),
								 PKStr::stringToInt32(values[3]));
			
				object->set(name, &c);
			}
		}
		break;

	case PK_VARIANT_INT32RANGE:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 2)
			{
				PKVariantInt32Range r(PKStr::stringToInt32(values[0]),
								      PKStr::stringToInt32(values[1]));
			
				object->set(name, &r);
			}
		}
		break;

	case PK_VARIANT_INT64RANGE:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 2)
			{
				PKVariantInt64Range r(PKStr::stringToInt64(values[0]),
								      PKStr::stringToInt64(values[1]));
			
				object->set(name, &r);
			}
		}
		break;

	case PK_VARIANT_DOUBLERANGE:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			if(values.size() == 2)
			{
				PKVariantDoubleRange r(PKStr::stringToDouble(values[0]),
								       PKStr::stringToDouble(values[1]));
			
				object->set(name, &r);
			}
		}
		break;

	case PK_VARIANT_STRINGLIST:
		{
			std::vector<std::string> values;
			PKStr::explode(value, ',', &values);

			PKVariantStringList l(values);
			object->set(name, &l);
		}
		break;

	case PK_VARIANT_WSTRINGLIST:
		{
            bool i18n = false;

            if(PKStr::findFirstOf(value, "PKI18N:") == 0)
            {
                i18n = true;
                value = value.substr(7);
            }

			std::vector<std::wstring> wvalues;

            if(i18n)
            {
                std::wstring wvalue = PK_i18n(PKStr::stringToWString(value));
		        PKStr::explode(wvalue, L',', &wvalues);

                PKVariantWStringListI18N l(wvalues);
		        object->set(name, &l);
            }
            else
            {
                std::vector<std::string> values;
		        PKStr::explode(value, ',', &values);

                uint32_t count = values.size();

                for(unsigned int i=0; i < count; i++)
                {
                    wvalues.push_back(PKStr::utf8StringToWstring(values[i]));
                }

                PKVariantWStringList l(wvalues);
		        object->set(name, &l);
            }
		}
		break;

	case PK_VARIANT_BINARY:
		{
			int size = 0;
			unsigned char *buffer = PKBase64::decode(value, &size);

			if(size > 0 && buffer != NULL)
			{
				PKVariantBinary b(size, buffer);
				object->set(name, &b);

				delete [] buffer;
			}
		}
		break;
    }
}
