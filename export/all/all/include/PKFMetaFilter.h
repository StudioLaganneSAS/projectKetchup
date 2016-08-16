//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
// 

#ifndef PKFMETA_FILTER_H
#define PKFMETA_FILTER_H

#include "IPKFFilter.h"

//
// PKFMetaFilter
//

class PKFMetaFilter : public IPKFFilter {
public:

    PKFMetaFilter();
    virtual ~PKFMetaFilter();

	// Input

	PKFReturnCode registerInputFilter(IPKFFilter *filter);

	uint32_t   getNumberOfInputFilters();

	PKFReturnCode getInputFilter(uint32_t index, IPKFFilter **filter);
	PKFReturnCode unregisterInputFilter(uint32_t index);
	PKFReturnCode unregisterInputFilter(IPKFFilter *filter);
	
	PKFReturnCode clearInputFilters();

	// Output

	PKFReturnCode registerOutputFilter(IPKFFilter *filter);

	uint32_t   getNumberOfOutputFilters();

	PKFReturnCode getOutputFilter(uint32_t index, IPKFFilter **filter);
	PKFReturnCode unregisterOutputFilter(uint32_t index);
	PKFReturnCode unregisterOutputFilter(IPKFFilter *filter);
	
	PKFReturnCode clearOutputFilters();


	// Reimplemented from IPKFFilter to
	// delegate to sub filters

    uint32_t      getNumberOfConnectors();
    
	PKFReturnCode getConnector(uint32_t index, 
							   IPKFConnector **connector);

private:
	std::vector <IPKFFilter *> inputFilters;
    std::vector <IPKFFilter *> outputFilters;
};


#endif // PKFMETA_FILTER_H
