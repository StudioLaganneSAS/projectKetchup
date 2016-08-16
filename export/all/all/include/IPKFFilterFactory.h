//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef IPKFFILTERFACTORY_H
#define IPKFFILTERFACTORY_H

#include "IPKFFilter.h"

//
// PKFFilterFactory record
//

typedef struct PKFFilterFactoryRecord_s {
	
	PKF_IFilterCreate        *createFunction;
	PKF_FilterPropertiesFill *fillPropertiesFunction;

} PKFFilterFactoryRecord;

//
// IPKFFilterFactory
//

class IPKFFilterFactory {

protected:
	IPKFFilterFactory();

public:
	virtual ~IPKFFilterFactory();

	//
	// Use these in your class to register
	// the filters you want to export...
	//

	PKFReturnCode addFilter(PKF_IFilterCreate        *filterCreateFunction,
							PKF_FilterPropertiesFill *filterPropertiesFillFunction);

    //
    // Use those three functions to
    // create the filters you want...
    //

    virtual uint32_t   getNumberOfFilters();

	virtual PKFReturnCode getFilterProperties(uint32_t index, PKFFilterProperties **prop);
    virtual PKFReturnCode createFilter(uint32_t index, IPKFFilter **filter);
	virtual PKFReturnCode createFilter(std::string filterId, IPKFFilter **filter);

private:

	std::vector <PKFFilterFactoryRecord *> records;
};

//
//
//

typedef PKFReturnCode (PKF_IFilterFactoryCreate)(IPKFFilterFactory **factory);

//
//
//

#endif //IPKFFILTERFACTORY_H
