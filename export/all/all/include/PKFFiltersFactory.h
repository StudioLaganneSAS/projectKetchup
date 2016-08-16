//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKFFILTERS_FACTORY_H
#define PKFFILTERS_FACTORY_H

#include "IPKFFilterFactory.h"

//
// PKFFiltersFactory
//

class PKFFiltersFactory : public IPKFFilterFactory
{
public:

	PKFFiltersFactory();

private:
};

//
// Entry point
//

PKFReturnCode PKFFilters_FilterFactoryCreate(IPKFFilterFactory **factory);


#endif // PKFFILTERS_FACTORY_H
