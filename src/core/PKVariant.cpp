//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#include "PKVariant.h"
#include "PKStr.h"
#include "PKBase64.h"

//
// PKVariant
//

PKVariant::PKVariant()
{
	_type = PK_VARIANT_NULL; 
}

PKVariant::~PKVariant()
{

}

std::string PKVariant::toString()
{
	return "";
}


//
// PKVariantNULL
//

PKVariantNULL::PKVariantNULL()
{
}

PKVariantNULL::~PKVariantNULL()
{
}

//
// PKVariantBool
//

PKVariantBool::PKVariantBool(bool value)
{
	_type = PK_VARIANT_BOOL;
	_val  = value;
}

PKVariantBool::~PKVariantBool()
{
}

std::string PKVariantBool::toString()
{
	if(_val) return "true";
	return "false";
}

//
// PKVariantInt8
//

PKVariantInt8::PKVariantInt8(int8_t value)
{
	_type = PK_VARIANT_INT8;
	_val  = value;
}

PKVariantInt8::~PKVariantInt8()
{
}

std::string PKVariantInt8::toString()
{
	return PKStr::int32ToString(_val);
}

//
// PKVariantInt16
//

PKVariantInt16::PKVariantInt16(int16_t value)
{
	_type = PK_VARIANT_INT16;
	_val  = value;
}

PKVariantInt16::~PKVariantInt16()
{
}

std::string PKVariantInt16::toString()
{
	return PKStr::int32ToString(_val);
}

//
// PKVariantInt32
//

PKVariantInt32::PKVariantInt32(int32_t value)
{
	_type = PK_VARIANT_INT32;
	_val  = value;
}

PKVariantInt32::~PKVariantInt32()
{
}

std::string PKVariantInt32::toString()
{
	return PKStr::int32ToString(_val);
}

//
// PKVariantInt64
//

PKVariantInt64::PKVariantInt64(int64_t value)
{
	_type = PK_VARIANT_INT64;
	_val  = value;
}

PKVariantInt64::~PKVariantInt64()
{
}

std::string PKVariantInt64::toString()
{
	return PKStr::int64ToString(_val);
}

//
// PKVariantUInt8
//

PKVariantUInt8::PKVariantUInt8(uint8_t value)
{
	_type = PK_VARIANT_UINT8;
	_val  = value;
}

PKVariantUInt8::~PKVariantUInt8()
{
}

std::string PKVariantUInt8::toString()
{
	return PKStr::uint32ToString(_val);
}

//
// PKVariantInt16
//

PKVariantUInt16::PKVariantUInt16(uint16_t value)
{
	_type = PK_VARIANT_UINT16;
	_val  = value;
}

PKVariantUInt16::~PKVariantUInt16()
{
}

std::string PKVariantUInt16::toString()
{
	return PKStr::uint32ToString(_val);
}

//
// PKVariantInt32
//

PKVariantUInt32::PKVariantUInt32(uint32_t value)
{
	_type = PK_VARIANT_UINT32;
	_val  = value;
}

PKVariantUInt32::~PKVariantUInt32()
{
}

std::string PKVariantUInt32::toString()
{
	return PKStr::uint32ToString(_val);
}

//
// PKVariantInt64
//

PKVariantUInt64::PKVariantUInt64(uint64_t value)
{
	_type = PK_VARIANT_UINT64;
	_val  = value;
}

PKVariantUInt64::~PKVariantUInt64()
{
}

std::string PKVariantUInt64::toString()
{
	return PKStr::uint64ToString(_val);
}

//
// PKVariantDouble
//

PKVariantDouble::PKVariantDouble(double value)
{
	_type = PK_VARIANT_DOUBLE;
	_val  = value;
}

PKVariantDouble::~PKVariantDouble()
{
}

std::string PKVariantDouble::toString()
{
	return PKStr::doubleToString(_val);
}

//
// PKVariantVoidP
//

PKVariantVoidP::PKVariantVoidP(void *value)
{
	_type = PK_VARIANT_VOIDP;
	_val  = value;
}

PKVariantVoidP::~PKVariantVoidP()
{
}

std::string PKVariantVoidP::toString()
{
	return "";
}

//
// PKVariantString
//

PKVariantString::PKVariantString(std::string value)
{
	_type = PK_VARIANT_STRING;
	_val  = value;
}

PKVariantString::~PKVariantString()
{
}

std::string PKVariantString::toString()
{
	return _val;
}

//
// PKVariantWString
//

PKVariantWString::PKVariantWString(std::wstring value)
{
	_type = PK_VARIANT_WSTRING;
	_val  = value;
}

PKVariantWString::~PKVariantWString()
{
}

std::string PKVariantWString::toString()
{
	return PKStr::wStringToUTF8string(_val);
}

//
// PKVariantWStringI18N
//

PKVariantWStringI18N::PKVariantWStringI18N(std::wstring value)
:PKVariantWString(value)
{
    this->_original = value;
}

PKVariantWStringI18N::~PKVariantWStringI18N()
{
    
}

//
// PKVariantPoint
//

PKVariantPoint::PKVariantPoint(int32_t x, int32_t y)
{
	_type = PK_VARIANT_POINT;
	_val  = PKPointCreate(x, y);
}

PKVariantPoint::~PKVariantPoint()
{
}

std::string PKVariantPoint::toString()
{
	return PKStr::int32ToString(_val.x) + "," + PKStr::int32ToString(_val.y);
}

//
// PKVariantRect
//

PKVariantRect::PKVariantRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	_type = PK_VARIANT_RECT;
	_val  = PKRectCreate(x, y, w, h);
}

PKVariantRect::~PKVariantRect()
{
}

std::string PKVariantRect::toString()
{
	return PKStr::int32ToString(_val.x)  + "," + 
		   PKStr::int32ToString(_val.y)  + "," +
		   PKStr::uint32ToString(_val.w) + "," +
		   PKStr::uint32ToString(_val.h);
}

//
// PKVariantColor
//

PKVariantColor::PKVariantColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	_type = PK_VARIANT_COLOR;
	_val  = PKColorCreate(r, g, b, a);
}

PKVariantColor::~PKVariantColor()
{
}

std::string PKVariantColor::toString()
{
	return PKStr::uint32ToString(_val.r) + "," + 
		   PKStr::uint32ToString(_val.g) + "," +
		   PKStr::uint32ToString(_val.b) + "," +
		   PKStr::uint32ToString(_val.a);
}

//
// PKVariantStringList
//

PKVariantStringList::PKVariantStringList(std::vector<std::string> value)
{
	_type = PK_VARIANT_STRINGLIST;
	_val  = value;
}

PKVariantStringList::~PKVariantStringList()
{
}

std::string PKVariantStringList::toString()
{
    std::string result = "";

    for(unsigned int i=0; i < _val.size(); i++)
    {
        result += _val[i];
        if(i != (_val.size()-1))
        {
            result += ",";
        }
    }

    return result;
}

//
// PKVariantWStringList
//

PKVariantWStringList::PKVariantWStringList(std::vector<std::wstring> value)
{
	_type = PK_VARIANT_WSTRINGLIST;
	_val  = value;
}

PKVariantWStringList::~PKVariantWStringList()
{
}

std::string PKVariantWStringList::toString()
{
    std::string result = "";

    for(unsigned int i=0; i < _val.size(); i++)
    {
        result += PKStr::wStringToUTF8string(_val[i]);

        if(i != (_val.size()-1))
        {
            result += ",";
        }
    }

    return result;
}

//
// PKVariantWStringListI18N
//

PKVariantWStringListI18N::PKVariantWStringListI18N(std::vector<std::wstring> value)
: PKVariantWStringList(value)
{
    this->_original = value;
}

PKVariantWStringListI18N::~PKVariantWStringListI18N()
{

}

//
// PKVariantInt32Range
//

PKVariantInt32Range::PKVariantInt32Range(int32_t min, int32_t max)
{
	_type = PK_VARIANT_INT32RANGE;
	_val  = PKInt32RangeCreate(min, max);
}

PKVariantInt32Range::~PKVariantInt32Range()
{
}

std::string PKVariantInt32Range::toString()
{
	return PKStr::int32ToString(_val.min) + "," + 
		   PKStr::int32ToString(_val.max);
}

//
// PKVariantInt64Range
//

PKVariantInt64Range::PKVariantInt64Range(int64_t min, int64_t max)
{
	_type = PK_VARIANT_INT64RANGE;
	_val  = PKInt64RangeCreate(min, max);
}

PKVariantInt64Range::~PKVariantInt64Range()
{
}

std::string PKVariantInt64Range::toString()
{
	return PKStr::int64ToString(_val.min) + "," + 
		   PKStr::int64ToString(_val.max);
}

//
// PKVariantDoubleRange
//

PKVariantDoubleRange::PKVariantDoubleRange(double min, double max)
{
	_type = PK_VARIANT_DOUBLERANGE;
	_val  = PKDoubleRangeCreate(min, max);
}

PKVariantDoubleRange::~PKVariantDoubleRange()
{
}

std::string PKVariantDoubleRange::toString()
{
	return PKStr::doubleToString(_val.min) + "," + 
		   PKStr::doubleToString(_val.max);
}

//
// PKVariantBinary
//

PKVariantBinary::PKVariantBinary(uint32_t size, void *buffer)
{
	_type   = PK_VARIANT_BINARY;
	_size   = size;
	_buffer = NULL; 

	if(_size == 0)
	{
		return;
	}

	_buffer = new char[_size];

	if(_buffer)
	{
		memcpy(_buffer, buffer, _size);
	}
}

PKVariantBinary::~PKVariantBinary()
{
	if(_buffer != NULL)
	{
		delete [] _buffer;
		_buffer = NULL;
	}
}

std::string PKVariantBinary::toString()
{
	// Use base64 to convert the buffer
	// to a readable ascii string

	if(_size == 0 || _buffer == NULL)
	{
		return "";
	}

	return PKBase64::encode((char *) _buffer, _size);
}

//
// Creators
//

// Generic

PKVariant *PKVariant_Create(PKVariantType type)
{
	switch(type)
	{
	case PK_VARIANT_NULL:
		return NULL;
		break;
	
	case PK_VARIANT_BOOL:
		return PKVariantBool_Create(false);
		break;
	
	case PK_VARIANT_INT64:
		return PKVariantInt64_Create(0);
		break;
	
	case PK_VARIANT_INT32:
		return PKVariantInt32_Create(0);
		break;
	
	case PK_VARIANT_INT16:
		return PKVariantInt16_Create(0);
		break;
	
	case PK_VARIANT_INT8:
		return PKVariantInt8_Create(0);
		break;
	
	case PK_VARIANT_UINT64:
		return PKVariantUInt64_Create(0);
		break;
	
	case PK_VARIANT_UINT32:
		return PKVariantUInt32_Create(0);
		break;
	
	case PK_VARIANT_UINT16:
		return PKVariantUInt16_Create(0);
		break;
	
	case PK_VARIANT_UINT8:
		return PKVariantUInt8_Create(0);
		break;
	
	case PK_VARIANT_DOUBLE:
		return PKVariantDouble_Create(0);
		break;
	
	case PK_VARIANT_VOIDP:
		return PKVariantVoidP_Create(NULL);
		break;
	
	case PK_VARIANT_STRING:
		return PKVariantString_Create("");
		break;
	
	case PK_VARIANT_WSTRING:
		return PKVariantWString_Create(L"");
		break;
	
	case PK_VARIANT_POINT:
		return PKVariantPoint_Create(PKPointCreate(0, 0));
		break;

	case PK_VARIANT_RECT:
		return PKVariantRect_Create(PKRectCreate(0, 0, 0, 0));
		break;

	case PK_VARIANT_COLOR:
		return PKVariantColor_Create(PKColorCreate(0, 0, 0, 0));
		break;

    case PK_VARIANT_STRINGLIST:
        {
            std::vector<std::string> empty;
            return PKVariantStringList_Create(empty);
        }
        break;

    case PK_VARIANT_WSTRINGLIST:
        {
            std::vector<std::wstring> empty;
            return PKVariantWStringList_Create(empty);
        }
        break;

    case PK_VARIANT_INT32RANGE:
        return PKVariantInt32Range_Create(PKInt32RangeCreate(0, 0));
        break;

    case PK_VARIANT_INT64RANGE:
        return PKVariantInt64Range_Create(PKInt64RangeCreate(0, 0));
        break;

    case PK_VARIANT_DOUBLERANGE:
        return PKVariantDoubleRange_Create(PKDoubleRangeCreate(0, 0));
        break;

	case PK_VARIANT_BINARY:
		return PKVariantBinary_Create(0, NULL);
    }

	return NULL;
}

PKVariant *PKVariantBool_Create(bool value)
{
	return (PKVariant *) (new PKVariantBool(value));
}

PKVariant *PKVariantInt8_Create(int8_t value)
{
	return (PKVariant *) (new PKVariantInt8(value));
}

PKVariant *PKVariantInt16_Create(int16_t value)
{
	return (PKVariant *) (new PKVariantInt16(value));
}

PKVariant *PKVariantInt32_Create(int32_t value)
{
	return (PKVariant *) (new PKVariantInt32(value));
}

PKVariant *PKVariantInt64_Create(int64_t value)
{
	return (PKVariant *) (new PKVariantInt64(value));
}

PKVariant *PKVariantUInt8_Create(uint8_t value)
{
	return (PKVariant *) (new PKVariantUInt8(value));
}

PKVariant *PKVariantUInt16_Create(uint16_t value)
{
	return (PKVariant *) (new PKVariantUInt16(value));
}

PKVariant *PKVariantUInt32_Create(uint32_t value)
{
	return (PKVariant *) (new PKVariantUInt32(value));
}

PKVariant *PKVariantUInt64_Create(uint64_t value)
{
	return (PKVariant *) (new PKVariantUInt64(value));
}

PKVariant *PKVariantDouble_Create(double value)
{
	return (PKVariant *) (new PKVariantDouble(value));
}

PKVariant *PKVariantVoidP_Create(void *value)
{
	return (PKVariant *) (new PKVariantVoidP(value));
}

PKVariant *PKVariantString_Create(std::string value)
{
	return (PKVariant *) (new PKVariantString(value));
}

PKVariant *PKVariantWString_Create(std::wstring value)
{
	return (PKVariant *) (new PKVariantWString(value));
}

PKVariant *PKVariantPoint_Create(PKPoint value)
{
	return (PKVariant *) (new PKVariantPoint(value.x, value.y));
}

PKVariant *PKVariantRect_Create(PKRect value)
{
	return (PKVariant *) (new PKVariantRect(value.x, value.y, value.w, value.h));
}

PKVariant *PKVariantColor_Create(PKColor value)
{
	return (PKVariant *) (new PKVariantColor(value.r, value.g, value.b, value.a));
}

PKVariant *PKVariantStringList_Create(std::vector<std::string> value)
{
    return (PKVariant *) (new PKVariantStringList(value));
}

PKVariant *PKVariantWStringList_Create(std::vector<std::wstring> value)
{
    return (PKVariant *) (new PKVariantWStringList(value));
}

PKVariant *PKVariantInt32Range_Create(PKInt32Range r)
{
    return (PKVariant *) (new PKVariantInt32Range(r.min, r.max));
}

PKVariant *PKVariantInt64Range_Create(PKInt64Range r)
{
    return (PKVariant *) (new PKVariantInt64Range(r.min, r.max));
}

PKVariant *PKVariantDoubleRange_Create(PKDoubleRange r)
{
    return (PKVariant *) (new PKVariantDoubleRange(r.min, r.max));
}

PKVariant *PKVariantBinary_Create(uint32_t size, void *buffer)
{
	return (PKVariant *) (new PKVariantBinary(size, buffer));
}

//
// Utils
//

bool PKVariantEquals(PKVariant *v1, PKVariant *v2)
{
    if(v1 == NULL || v2 == NULL)
    {
        return false;
    }

    if(v1->_type != v2->_type)
    {
        return false;
    }

	switch(v1->_type)
	{
	case PK_VARIANT_NULL:
		return true;
		break;
	
	case PK_VARIANT_BOOL:
        return (PKVALUE_BOOL(v1) == PKVALUE_BOOL(v2));
		break;
	
	case PK_VARIANT_INT64:
		return (PKVALUE_INT64(v1) == PKVALUE_INT64(v2));
		break;
	
	case PK_VARIANT_INT32:
		return (PKVALUE_INT32(v1) == PKVALUE_INT32(v2));
		break;
	
	case PK_VARIANT_INT16:
		return (PKVALUE_INT16(v1) == PKVALUE_INT16(v2));
		break;
	
	case PK_VARIANT_INT8:
		return (PKVALUE_INT8(v1) == PKVALUE_INT8(v2));
		break;
	
	case PK_VARIANT_UINT64:
		return (PKVALUE_UINT64(v1) == PKVALUE_UINT64(v2));
		break;
	
	case PK_VARIANT_UINT32:
		return (PKVALUE_UINT32(v1) == PKVALUE_UINT32(v2));
		break;
	
	case PK_VARIANT_UINT16:
		return (PKVALUE_UINT16(v1) == PKVALUE_UINT16(v2));
		break;
	
	case PK_VARIANT_UINT8:
		return (PKVALUE_UINT8(v1) == PKVALUE_UINT8(v2));
		break;
	
	case PK_VARIANT_DOUBLE:
		return (PKVALUE_DOUBLE(v1) == PKVALUE_DOUBLE(v2));
		break;
	
	case PK_VARIANT_VOIDP:
		return (PKVALUE_VOIDP(v1) == PKVALUE_VOIDP(v2));
		break;
	
	case PK_VARIANT_STRING:
		return (PKVALUE_STRING(v1) == PKVALUE_STRING(v2));
		break;
	
	case PK_VARIANT_WSTRING:
		return (PKVALUE_WSTRING(v1) == PKVALUE_WSTRING(v2));
		break;
	
	case PK_VARIANT_POINT:
        {
            PKPoint p1 = PKVALUE_POINT(v1);
            PKPoint p2 = PKVALUE_POINT(v2);

            return ((p1.x == p2.x) && (p1.y == p2.y));
        }
        break;

	case PK_VARIANT_RECT:
        {
            PKRect r1 = PKVALUE_RECT(v1);
            PKRect r2 = PKVALUE_RECT(v2);

            return ((r1.x == r2.x) && (r1.y == r2.y) &&
                    (r1.w == r2.w) && (r1.h == r2.h));
        }
		break;

	case PK_VARIANT_COLOR:
        {
            PKColor c1 = PKVALUE_COLOR(v1);
            PKColor c2 = PKVALUE_COLOR(v2);

            return ((c1.r == c2.r) && (c1.g == c2.g) &&
                    (c1.b == c2.b) && (c1.a == c2.a));
        }
		break;

    case PK_VARIANT_STRINGLIST:
        {
            std::vector<std::string> list1 = PKVALUE_STRINGLIST(v1);
            std::vector<std::string> list2 = PKVALUE_STRINGLIST(v2);

            if(list1.size() != list2.size())
            {
                return false;
            }

            for(uint32_t i=0; i < list1.size(); i++)
            {
                if(list1[i] != list2[i])
                {
                    return false;
                }
            }

            return true;
        }
        break;

    case PK_VARIANT_WSTRINGLIST:
        {
            std::vector<std::wstring> list1 = PKVALUE_WSTRINGLIST(v1);
            std::vector<std::wstring> list2 = PKVALUE_WSTRINGLIST(v2);

            if(list1.size() != list2.size())
            {
                return false;
            }

            for(uint32_t i=0; i < list1.size(); i++)
            {
                if(list1[i] != list2[i])
                {
                    return false;
                }
            }

            return true;
        }
        break;

    case PK_VARIANT_INT32RANGE:
        {
            PKInt32Range r1 = PKVALUE_INT32RANGE(v1);
            PKInt32Range r2 = PKVALUE_INT32RANGE(v2);

            return ((r1.min == r2.min) && (r1.max == r2.max));
        }
        break;

    case PK_VARIANT_INT64RANGE:
        {
            PKInt64Range r1 = PKVALUE_INT64RANGE(v1);
            PKInt64Range r2 = PKVALUE_INT64RANGE(v2);

            return ((r1.min == r2.min) && (r1.max == r2.max));
        }
        break;

    case PK_VARIANT_DOUBLERANGE:
        {
            PKDoubleRange r1 = PKVALUE_DOUBLERANGE(v1);
            PKDoubleRange r2 = PKVALUE_DOUBLERANGE(v2);

            return ((r1.min == r2.min) && (r1.max == r2.max));
        }
        break;

	case PK_VARIANT_BINARY:
		{
			uint32_t s1 = PKVALUE_BINARY_SIZE(v1);
			uint32_t s2 = PKVALUE_BINARY_SIZE(v2);

			if(s1 != s2)
			{
				return false;
			}

			void *b1 = PKVALUE_BINARY_BUFFER(v1);
			void *b2 = PKVALUE_BINARY_BUFFER(v2);

			return (memcmp(b1, b2, s1) == 0);
		}
		break;
    }

    return false;
}

PKVariant *PKVariantCopy(PKVariant *src)
{
	if(src == NULL)
	{
		return NULL;
	}

	switch(src->_type)
	{
	case PK_VARIANT_NULL:
		return new PKVariantNULL();
		break;
	
	case PK_VARIANT_BOOL:
        return PKVariantBool_Create(PKVALUE_BOOL(src));
		break;
	
	case PK_VARIANT_INT64:
		return PKVariantInt64_Create(PKVALUE_INT64(src));
		break;
	
	case PK_VARIANT_INT32:
		return PKVariantInt32_Create(PKVALUE_INT32(src));
		break;
	
	case PK_VARIANT_INT16:
		return PKVariantInt16_Create(PKVALUE_INT16(src));
		break;
	
	case PK_VARIANT_INT8:
		return PKVariantInt8_Create(PKVALUE_INT8(src));
		break;
	
	case PK_VARIANT_UINT64:
		return PKVariantUInt64_Create(PKVALUE_UINT64(src));
		break;
	
	case PK_VARIANT_UINT32:
		return PKVariantUInt32_Create(PKVALUE_UINT32(src));
		break;
	
	case PK_VARIANT_UINT16:
		return PKVariantUInt16_Create(PKVALUE_UINT16(src));
		break;
	
	case PK_VARIANT_UINT8:
		return PKVariantUInt8_Create(PKVALUE_UINT8(src));
		break;
	
	case PK_VARIANT_DOUBLE:
		return PKVariantDouble_Create(PKVALUE_DOUBLE(src));
		break;
	
	case PK_VARIANT_VOIDP:
		return PKVariantVoidP_Create(PKVALUE_VOIDP(src));
		break;
	
	case PK_VARIANT_STRING:
		return PKVariantString_Create(PKVALUE_STRING(src));
		break;
	
	case PK_VARIANT_WSTRING:
		return PKVariantWString_Create(PKVALUE_WSTRING(src));
		break;
	
	case PK_VARIANT_POINT:
        {
            PKPoint p = PKVALUE_POINT(src);
            return PKVariantPoint_Create(p);
        }
        break;

	case PK_VARIANT_RECT:
        {
            PKRect r = PKVALUE_RECT(src);
            return PKVariantRect_Create(r);
        }
		break;

	case PK_VARIANT_COLOR:
        {
            PKColor c = PKVALUE_COLOR(src);
            return PKVariantColor_Create(c);
        }
		break;

    case PK_VARIANT_STRINGLIST:
        {
            std::vector<std::string> list = PKVALUE_STRINGLIST(src);
            return PKVariantStringList_Create(list);
        }
        break;

    case PK_VARIANT_WSTRINGLIST:
        {
            std::vector<std::wstring> list = PKVALUE_WSTRINGLIST(src);
            return PKVariantWStringList_Create(list);
        }
        break;

    case PK_VARIANT_INT32RANGE:
        {
            PKInt32Range r = PKVALUE_INT32RANGE(src);
            return PKVariantInt32Range_Create(r);
        }
        break;

    case PK_VARIANT_INT64RANGE:
        {
            PKInt64Range r = PKVALUE_INT64RANGE(src);
            return PKVariantInt64Range_Create(r);
        }
        break;

    case PK_VARIANT_DOUBLERANGE:
        {
            PKDoubleRange r = PKVALUE_DOUBLERANGE(src);
            return PKVariantDoubleRange_Create(r);
        }
        break;

	case PK_VARIANT_BINARY:
		{
			uint32_t s = PKVALUE_BINARY_SIZE(src);
			void *b = PKVALUE_BINARY_BUFFER(src);

			return PKVariantBinary_Create(s, b);
		}
		break;
    }

    return NULL;
}
