//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef PKFFILTERGRAPH_H
#define PKFFILTERGRAPH_H

#include "PKObject.h"
#include "PKFTypes.h"
#include "IPKFFilter.h"
#include "IPKFConnector.h"
#include "PKFInputConnector.h"
#include "PKFOutputConnector.h"
#include "PKFNullSourceFilter.h"
#include "PKFNullSinkFilter.h"
#include "PKLogger.h"

//
// PKFFilterGraph
//

class PKFFilterGraph : public PKObject
{
public:

	PKFFilterGraph(std::string graphName);

    PKFReturnCode getLogger(PKLogger **log);

    uint32_t   getNumberOfFilters();
    PKFReturnCode getFilter(uint32_t index, IPKFFilter  **filter);

    PKFReturnCode addFilter(IPKFFilter *filter);
    PKFReturnCode removeFilter(IPKFFilter *filter);

    PKFReturnCode connect(IPKFFilter *source, IPKFFilter *dest);
    PKFReturnCode disconnect(IPKFFilter *source, IPKFFilter *dest);
    PKFReturnCode disconnect(IPKFFilter *filter);

    PKFReturnCode connect(IPKFConnector *source, IPKFConnector *dest);
    PKFReturnCode disconnect(IPKFConnector *conn);

	PKFReturnCode closeGraph();

	PKFReturnCode runGraphFromFilter(IPKFFilter *filter);
	PKFReturnCode runAll();

	PKFReturnCode interruptAll();

protected:
    
    virtual ~PKFFilterGraph();

	// From PKObject
	void selfPropertyChanged(PKProperty *prop, PKVariant *oldValue);

public:

	static std::string GRAPHNAME_STRING_PROPERTY;
	static std::string LOGFILE_WSTRING_PROPERTY;

private:
	PKLogger *logger;
	std::vector <IPKFFilter *> filters;
};

#endif // PKFFILTERGRAPH_H
