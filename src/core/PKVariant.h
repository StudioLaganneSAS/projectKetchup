//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//
//

#ifndef PKVARIANT_H
#define PKVARIANT_H

#include "PKTypes.h"
#include <string>
#include <vector>

//
// PKVariant value subtype
//

typedef enum {

	PK_VARIANT_NULL,
	
	PK_VARIANT_BOOL,

	PK_VARIANT_INT64,
	PK_VARIANT_INT32,
	PK_VARIANT_INT16,
	PK_VARIANT_INT8,
	
	PK_VARIANT_UINT64,
	PK_VARIANT_UINT32,
	PK_VARIANT_UINT16,
	PK_VARIANT_UINT8,
	
	PK_VARIANT_DOUBLE,
	PK_VARIANT_VOIDP,
	
	PK_VARIANT_STRING,
	PK_VARIANT_WSTRING,

	PK_VARIANT_POINT,
	PK_VARIANT_RECT,
	PK_VARIANT_COLOR,

    PK_VARIANT_STRINGLIST,
    PK_VARIANT_WSTRINGLIST,
    
    PK_VARIANT_INT32RANGE,
    PK_VARIANT_INT64RANGE,
    PK_VARIANT_DOUBLERANGE,

	PK_VARIANT_BINARY,

} PKVariantType;

//
// PKVariant Class
//

class PKVariant 
{
public:

	PKVariantType _type;
	virtual ~PKVariant();

	virtual std::string toString();
	
protected:

	PKVariant();
};

//
// Specific Variant classes
//

class PKVariantNULL : public PKVariant
{
public:

	PKVariantNULL();
	~PKVariantNULL();
};

class PKVariantBool : public PKVariant
{
public:

	PKVariantBool(bool value);
	~PKVariantBool();

	std::string toString();
	
	bool _val;
};

class PKVariantInt8 : public PKVariant
{
public:

	PKVariantInt8(int8_t value);
	~PKVariantInt8();

	std::string toString();

	int8_t _val;
};

class PKVariantInt16 : public PKVariant
{
public:

	PKVariantInt16(int16_t value);
	~PKVariantInt16();

	std::string toString();

	int16_t _val;
};

class PKVariantInt32 : public PKVariant
{
public:

	PKVariantInt32(int32_t value);
	~PKVariantInt32();

	std::string toString();

	int32_t _val;
};

class PKVariantInt64 : public PKVariant
{
public:

	PKVariantInt64(int64_t value);
	~PKVariantInt64();

	std::string toString();

	int64_t _val;
};

class PKVariantUInt8 : public PKVariant
{
public:

	PKVariantUInt8(uint8_t value);
	~PKVariantUInt8();

	std::string toString();

	uint8_t _val;
};

class PKVariantUInt16 : public PKVariant
{
public:

	PKVariantUInt16(uint16_t value);
	~PKVariantUInt16();

	std::string toString();

	uint16_t _val;
};

class PKVariantUInt32 : public PKVariant
{
public:

	PKVariantUInt32(uint32_t value);
	~PKVariantUInt32();

	std::string toString();

	uint32_t _val;
};

class PKVariantUInt64 : public PKVariant
{
public:

	PKVariantUInt64(uint64_t value);
	~PKVariantUInt64();

	std::string toString();

	uint64_t _val;
};

class PKVariantDouble : public PKVariant
{
public:

	PKVariantDouble(double value);
	~PKVariantDouble();

	std::string toString();

	double _val;
};

class PKVariantVoidP : public PKVariant
{
public:

	PKVariantVoidP(void *value);
	~PKVariantVoidP();

	std::string toString();

	void *_val;
};

class PKVariantString : public PKVariant
{
public:

	PKVariantString(std::string value);
	~PKVariantString();

	std::string toString();

	std::string _val;
};

class PKVariantWString : public PKVariant
{
public:

	PKVariantWString(std::wstring value);
	~PKVariantWString();

	std::string toString();

	std::wstring _val;
};

class PKVariantWStringI18N : public PKVariantWString
{
public:

	PKVariantWStringI18N(std::wstring value);
	~PKVariantWStringI18N();

	std::wstring _original;
};

class PKVariantPoint : public PKVariant
{
public:

	PKVariantPoint(int32_t x, int32_t y);
	~PKVariantPoint();

	std::string toString();

	PKPoint _val;
};

class PKVariantRect : public PKVariant
{
public:

	PKVariantRect(int32_t x, int32_t y, uint32_t w, uint32_t h);
	~PKVariantRect();

	std::string toString();

	PKRect _val;
};

class PKVariantColor : public PKVariant
{
public:

	PKVariantColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	~PKVariantColor();

	std::string toString();

	PKColor _val;
};

class PKVariantStringList : public PKVariant
{
public:

    PKVariantStringList(std::vector<std::string>);
	~PKVariantStringList();

	std::string toString();

	std::vector<std::string> _val;
};

class PKVariantWStringList : public PKVariant
{
public:

    PKVariantWStringList(std::vector<std::wstring>);
	~PKVariantWStringList();

	std::string toString();

	std::vector<std::wstring> _val;
};

class PKVariantWStringListI18N : public PKVariantWStringList
{
public:

    PKVariantWStringListI18N(std::vector<std::wstring>);
	~PKVariantWStringListI18N();

	std::vector<std::wstring> _original;
};

class PKVariantInt32Range : public PKVariant
{
public:

    PKVariantInt32Range(int32_t min, int32_t max);
	~PKVariantInt32Range();

	std::string toString();

	PKInt32Range _val;
};

class PKVariantInt64Range : public PKVariant
{
public:

    PKVariantInt64Range(int64_t min, int64_t max);
	~PKVariantInt64Range();

	std::string toString();

	PKInt64Range _val;
};

class PKVariantDoubleRange : public PKVariant
{
public:

    PKVariantDoubleRange(double min, double max);
	~PKVariantDoubleRange();

	std::string toString();

	PKDoubleRange _val;
};

class PKVariantBinary : public PKVariant
{
public:

    PKVariantBinary(uint32_t size, void *buffer);
	~PKVariantBinary();

	std::string toString();

	uint32_t      _size;
	void         *_buffer;
};

//
// Macros
// ------
//

// Generic Creator

PKVariant *PKVariant_Create(PKVariantType type);

// Creators (pointers)

PKVariant *PKVariantBool_Create(bool value);

PKVariant *PKVariantInt8_Create(int8_t value);
PKVariant *PKVariantInt16_Create(int16_t value);
PKVariant *PKVariantInt32_Create(int32_t value);
PKVariant *PKVariantInt64_Create(int64_t value);

PKVariant *PKVariantUInt8_Create(uint8_t value);
PKVariant *PKVariantUInt16_Create(uint16_t value);
PKVariant *PKVariantUInt32_Create(uint32_t value);
PKVariant *PKVariantUInt64_Create(uint64_t value);

PKVariant *PKVariantDouble_Create(double value);
PKVariant *PKVariantVoidP_Create(void *value);

PKVariant *PKVariantString_Create(std::string value);
PKVariant *PKVariantWString_Create(std::wstring value);

PKVariant *PKVariantPoint_Create(PKPoint p);
PKVariant *PKVariantRect_Create(PKRect r);
PKVariant *PKVariantColor_Create(PKColor c);

PKVariant *PKVariantStringList_Create(std::vector<std::string> value);
PKVariant *PKVariantWStringList_Create(std::vector<std::wstring> value);

PKVariant *PKVariantInt32Range_Create(PKInt32Range r);
PKVariant *PKVariantInt64Range_Create(PKInt64Range r);
PKVariant *PKVariantDoubleRange_Create(PKDoubleRange r);

PKVariant *PKVariantBinary_Create(uint32_t size, void *buffer);

// Getters

#define PKVALUE_BOOL(variant) \
		((PKVariantBool *) variant)->_val

#define PKVALUE_INT8(variant) \
		((PKVariantInt8 *) variant)->_val

#define PKVALUE_INT16(variant) \
		((PKVariantInt16 *) variant)->_val

#define PKVALUE_INT32(variant) \
		((PKVariantInt32 *) variant)->_val

#define PKVALUE_INT64(variant) \
		((PKVariantInt64 *) variant)->_val

#define PKVALUE_UINT8(variant) \
		((PKVariantUInt8 *) variant)->_val

#define PKVALUE_UINT16(variant) \
		((PKVariantUInt16 *) variant)->_val

#define PKVALUE_UINT32(variant) \
		((PKVariantUInt32 *) variant)->_val

#define PKVALUE_UINT64(variant) \
		((PKVariantUInt64 *) variant)->_val

#define PKVALUE_DOUBLE(variant) \
		((PKVariantDouble *) variant)->_val

#define PKVALUE_VOIDP(variant) \
		((PKVariantVoidP *) variant)->_val

#define PKVALUE_STRING(variant) \
		((PKVariantString *) variant)->_val

#define PKVALUE_WSTRING(variant) \
		((PKVariantWString *) variant)->_val

#define PKVALUE_POINT(variant) \
		((PKVariantPoint *) variant)->_val

#define PKVALUE_RECT(variant) \
		((PKVariantRect *) variant)->_val

#define PKVALUE_COLOR(variant) \
		((PKVariantColor *) variant)->_val

#define PKVALUE_STRINGLIST(variant) \
		((PKVariantStringList *) variant)->_val

#define PKVALUE_WSTRINGLIST(variant) \
		((PKVariantWStringList *) variant)->_val

#define PKVALUE_INT32RANGE(variant) \
		((PKVariantInt32Range *) variant)->_val

#define PKVALUE_INT64RANGE(variant) \
		((PKVariantInt64Range *) variant)->_val

#define PKVALUE_DOUBLERANGE(variant) \
		((PKVariantDoubleRange *) variant)->_val

#define PKVALUE_BINARY_SIZE(variant) \
		((PKVariantBinary *) variant)->_size

#define PKVALUE_BINARY_BUFFER(variant) \
		((PKVariantBinary *) variant)->_buffer

// SETTERS

#define PKVALUE_BOOL_SET(variant, value) \
		((PKVariantBool *) variant)->_val = value

#define PKVALUE_INT8_SET(variant, value) \
		((PKVariantInt8 *) variant)->_val = value

#define PKVALUE_INT16_SET(variant, value) \
		((PKVariantInt16 *) variant)->_val = value

#define PKVALUE_INT32_SET(variant, value) \
		((PKVariantInt32 *) variant)->_val = value

#define PKVALUE_INT64_SET(variant, value) \
		((PKVariantInt64 *) variant)->_val = value

#define PKVALUE_UINT8_SET(variant, value) \
		((PKVariantUInt8 *) variant)->_val = value

#define PKVALUE_UINT16_SET(variant, value) \
		((PKVariantUInt16 *) variant)->_val = value

#define PKVALUE_UINT32_SET(variant, value) \
		((PKVariantUInt32 *) variant)->_val = value

#define PKVALUE_UINT64_SET(variant, value) \
		((PKVariantUInt64 *) variant)->_val = value

#define PKVALUE_DOUBLE_SET(variant, value) \
		((PKVariantDouble *) variant)->_val = value

#define PKVALUE_VOIDP_SET(variant, value) \
		((PKVariantVoidP *) variant)->_val = value

#define PKVALUE_STRING_SET(variant, value) \
		((PKVariantString *) variant)->_val = value

#define PKVALUE_WSTRING_SET(variant, value) \
		((PKVariantWString *) variant)->_val = value

#define PKVALUE_POINT_SET(variant, value) \
		((PKVariantPoint *) variant)->_val = value

#define PKVALUE_RECT_SET(variant, value) \
		((PKVariantRect *) variant)->_val = value

#define PKVALUE_COLOR_SET(variant, value) \
		((PKVariantColor *) variant)->_val = value

#define PKVALUE_STRINGLIST_SET(variant, value) \
		((PKVariantStringList *) variant)->_val = value

#define PKVALUE_WSTRINGLIST_SET(variant, value) \
		((PKVariantWStringList *) variant)->_val = value

#define PKVALUE_INT32RANGE_SET(variant, value) \
		((PKVariantInt32Range *) variant)->_val = value

#define PKVALUE_INT64RANGE_SET(variant, value) \
		((PKVariantInt64Range *) variant)->_val = value

#define PKVALUE_DOUBLERANGE_SET(variant, value) \
		((PKVariantDoubleRange *) variant)->_val = value

#define PKVALUE_BINARY_SET(variant, size, buffer)      \
		((PKVariantBinary *) variant)->_size   = size; \
		((PKVariantBinary *) variant)->_buffer = size

// Util

bool PKVariantEquals(PKVariant *v1, PKVariant *v2);

PKVariant *PKVariantCopy(PKVariant *src);

#endif // PKVARIANT_H
