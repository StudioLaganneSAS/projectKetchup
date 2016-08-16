//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//  

#ifndef IPKFFILTER_H
#define IPKFFILTER_H

#include "PKObject.h"
#include "PKFTypes.h"
#include "IPKFConnector.h"
#include "PKFFilterProperties.h"

class PKFFilterGraph;

//
// IPKFFilter
//

class IPKFFilter : public PKObject
{
	friend class PKFFilterGraph;

public:

    IPKFFilter();

    //
    // Call run() to have the filter operate
    // on its input. Call interrupt() to finish
    //  a run() operation early.
    //

    virtual PKFReturnCode run();
    virtual PKFReturnCode interrupt();

    //
    // Override this in the GUI class to get
    // feedback or interaction during the run()
    // operation.
    //

	virtual PKFReturnCode feedback(std::string info, void *parameter);

	//
	// These functions get called when
	// one input or output pin is connected
	// or disconnected so you can perform operations
	// if you feel like it
	//

	virtual PKFReturnCode onConnecting(IPKFConnector *connector, IPKFConnector *to);
	virtual PKFReturnCode onConnected(IPKFConnector *connector, IPKFConnector *to);
	virtual PKFReturnCode onDisconnected(IPKFConnector *connector);

    //
    // This is the main function that you will
    // override to create a filter. It implements
    // the operation of the filter.
    //

    virtual PKFReturnCode process() = 0;

    PKFReturnCode   addConnector(IPKFConnector *connector);
    PKFReturnCode   removeConnector(uint32_t index);

    virtual uint32_t      getNumberOfConnectors();
    virtual PKFReturnCode getConnector(uint32_t index, IPKFConnector **connector);

    PKFFilterProperties *getFilterProperties();
    void                 setFilterProperties(PKFFilterProperties *properties);

public:

	static std::string AUTORUN_BOOL_PROPERTY;

protected:

    virtual ~IPKFFilter();

    void          setFilterGraph(PKFFilterGraph  *graph);
	PKFReturnCode getFilterGraph(PKFFilterGraph **graph);

	void            setStatus(PKFFilterStatus status);
    PKFFilterStatus getStatus();

private:
	std::vector <IPKFConnector *> connectors;
	PKFFilterStatus          status;
    PKFFilterProperties     *properties;
	PKFFilterGraph          *parentGraph;
};

//
// 
//

typedef PKFReturnCode (PKF_IFilterCreate)(IPKFFilter **filter);

//
//
//

typedef PKFReturnCode (PKF_FilterPropertiesFill)(PKFFilterProperties *filterProperties);

//
//
//

#endif // IPKFFILTER_H
