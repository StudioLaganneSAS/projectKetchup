//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKFFilterGraph.h"

//
// PKFFilterGraph
//

std::string PKFFilterGraph::GRAPHNAME_STRING_PROPERTY  = "name";
std::string PKFFilterGraph::LOGFILE_WSTRING_PROPERTY   = "logFile";


PKFFilterGraph::PKFFilterGraph(std::string graphName)
{
	this->logger = new PKLogger();

	PKOBJECT_ADD_STRING_PROPERTY(name, graphName);
	PKOBJECT_ADD_WSTRING_PROPERTY(logFile, L"");
}

PKFFilterGraph::~PKFFilterGraph()
{
	for(unsigned int i=0; i < this->filters.size(); i++)
	{
		this->disconnect(this->filters[i]);
	}

	this->filters.clear();

	delete this->logger;
}

PKFReturnCode PKFFilterGraph::getLogger(PKLogger **log)
{
	if(log == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*log = this->logger;

	return PKF_SUCCESS;
}

uint32_t PKFFilterGraph::getNumberOfFilters()
{
	return this->filters.size();
}

PKFReturnCode PKFFilterGraph::getFilter(uint32_t index, IPKFFilter  **filter)
{
	if(filter == NULL || index >= this->filters.size())
	{
		return PKF_ERR_INVALID_PARAM;
	}

	*filter = this->filters[index];

	return PKF_SUCCESS;
}

PKFReturnCode PKFFilterGraph::addFilter(IPKFFilter *filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(unsigned int i=0; i < this->filters.size(); i++)
	{
		if(this->filters[i] == filter)
		{
			return PKF_ERR_INVALID_PARAM;
		}
	}

	this->filters.push_back(filter);
	filter->setFilterGraph(this);

	this->addChild(filter);
	
	return PKF_SUCCESS;
}

PKFReturnCode PKFFilterGraph::removeFilter(IPKFFilter *filter)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	std::vector <IPKFFilter*> new_filters;

	for(unsigned int i=0; i < this->filters.size(); i++)
	{
		if(this->filters[i] != filter)
		{
			new_filters.push_back(this->filters[i]);
		}
		else
		{
			// disconnect filter 
			// before we remove it

			result = this->disconnect(this->filters[i]);

			if(result != PKF_SUCCESS && result != PKF_ERR_NOT_CONNECTED)
			{
				// keep the filter in since
				// we couldn't disconnect it

				new_filters.push_back(this->filters[i]);
			}
			else
			{
				// fully remove from graph
				this->filters[i]->setFilterGraph(NULL);
			}
		}
	}

	this->filters = new_filters;

	return result;
}

PKFReturnCode PKFFilterGraph::connect(IPKFFilter *source, IPKFFilter *dest)
{
	if(source == NULL || dest == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	bool found = false;

	for(unsigned int i=0; i < source->getNumberOfConnectors(); i++)
	{
		IPKFConnector *s = NULL;
		
		PKFReturnCode code = source->getConnector(i, &s);

		if(code != PKF_SUCCESS)
		{
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		if(s->getType() == PKF_CONNECTOR_TYPE_INPUT)
		{
			continue;
		}

		for(unsigned int j=0; j < dest->getNumberOfConnectors(); j++)
		{
			IPKFConnector *d = NULL;
		
			PKFReturnCode code = dest->getConnector(j, &d);

			if(code != PKF_SUCCESS)
			{
				return PKF_ERR_OBJECT_NOT_VALID;
			}

			if(d->getType() == PKF_CONNECTOR_TYPE_OUTPUT)
			{
				continue;
			}
			
			PKFReturnCode result = s->connect(d);

			if(result == PKF_SUCCESS)
			{
				found = true;
				break;
			}
		}

		if(found == true)
		{
			break;
		}
	}

	if(found)
	{
		return PKF_SUCCESS;
	}

	return PKF_ERR_NO_MATCHING_TYPES;
}

PKFReturnCode PKFFilterGraph::disconnect(IPKFFilter *source, IPKFFilter *dest)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(source == NULL || dest == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(unsigned int i=0; i < source->getNumberOfConnectors(); i++)
	{
		IPKFConnector *s = NULL;
		
		PKFReturnCode code = source->getConnector(i, &s);

		if(code != PKF_SUCCESS)
		{
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		if(s->getType() == PKF_CONNECTOR_TYPE_INPUT)
		{
			continue;
		}

		IPKFConnector *d = NULL;

		if(s->getConnectedTo(&d) == PKF_SUCCESS)
		{
			PKFInputConnector *in = (PKFInputConnector *) d;
			IPKFFilter        *f  = NULL;

			if(in->getParentFilter(&f) == PKF_SUCCESS)
			{
				if(f == dest)
				{
					result = s->disconnect();

					if(result != PKF_SUCCESS)
					{
						break;
					}
				}
			}
		}
	}

	return result;
}

PKFReturnCode PKFFilterGraph::disconnect(IPKFFilter *filter)
{
	PKFReturnCode result = PKF_SUCCESS;

	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	for(unsigned int i=0; i < filter->getNumberOfConnectors(); i++)
	{
		IPKFConnector *s = NULL;
		
		PKFReturnCode code = filter->getConnector(i, &s);

		if(code != PKF_SUCCESS)
		{
			return PKF_ERR_OBJECT_NOT_VALID;
		}

		result = s->disconnect();

		if(result != PKF_SUCCESS)
		{
			break;
		}
	}

	return result;
}

PKFReturnCode PKFFilterGraph::connect(IPKFConnector *source, IPKFConnector *dest)
{
	if(source == NULL || dest == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKFReturnCode result = source->connect(dest);

	return result;
}

PKFReturnCode PKFFilterGraph::disconnect(IPKFConnector *conn)
{
	if(conn == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKFReturnCode result = conn->disconnect();

	return result;
}

PKFReturnCode PKFFilterGraph::closeGraph()
{
	for(uint32_t i=0; i < this->filters.size(); i++)
	{
		if(this->filters[i] == NULL)
		{
			continue;
		}

		for(uint32_t j=0; j < this->filters[i]->getNumberOfConnectors(); j++)
		{
			IPKFConnector *conn = NULL;

			if(this->filters[i]->getConnector(j, &conn) == PKF_SUCCESS)
			{
				IPKFFilter  *null = NULL;
				IPKFConnector *to = NULL;

				if(conn->getConnectedTo(&to) == PKF_SUCCESS)
				{
					continue;
				}

				switch(conn->getType())
				{
				case PKF_CONNECTOR_TYPE_INPUT:
					{
						null = new PKFNullSourceFilter(conn->getTransferType());
					}
					break;

				case PKF_CONNECTOR_TYPE_OUTPUT:
					{
						null = new PKFNullSinkFilter(conn->getTransferType());
					}
					break;

				default:
					break;
				}

				if(NULL == null)
				{
					return PKF_ERR_OUT_OF_MEMORY;
				}

				if(null->getConnector(0, &to) == PKF_SUCCESS)
				{
					PKFReturnCode result = this->connect(conn, to);

					if(result != PKF_SUCCESS)
					{
						delete null;
						return result;	
					}
					else
					{
						this->addFilter(null);
					}
				}
				else
				{
					delete null;
				}
			}
		}
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFFilterGraph::runGraphFromFilter(IPKFFilter *filter)
{
	if(filter == NULL)
	{
		return PKF_ERR_INVALID_PARAM;
	}

	PKVariant *ar = filter->get(IPKFFilter::AUTORUN_BOOL_PROPERTY);

	if(PKVALUE_BOOL(ar) == false)
	{
		// Do not run filter manually
		// it will have been called automatically...

		PKFReturnCode result = filter->run();
	
		if(result != PKF_SUCCESS)
		{
			return result;
		}
	}

	// now iterate output connectors

	for(uint32_t i=0; i < filter->getNumberOfConnectors(); i++)
	{
		IPKFConnector *conn = NULL;

		if(filter->getConnector(i, &conn) == PKF_SUCCESS)
		{
			if(conn->getType() == PKF_CONNECTOR_TYPE_INPUT)
			{
				continue;
			}

			// find filter connected to it

			IPKFConnector *to = NULL;

			if(conn->getConnectedTo(&to) == PKF_SUCCESS)
			{
				if(to->getType() == PKF_CONNECTOR_TYPE_OUTPUT)
				{
					return PKF_ERR_OBJECT_NOT_VALID;
				}

				PKFInputConnector *input = (PKFInputConnector *) to;
				IPKFFilter *next = NULL;

				if(input->getParentFilter(&next) == PKF_SUCCESS)
				{
					PKFReturnCode result = this->runGraphFromFilter(next);

					if(result != PKF_SUCCESS)
					{
						return result;
					}
				}
				else
				{
					return PKF_ERR_NOT_READY;
				}
			}
			else
			{
				// graph not complete

				return PKF_ERR_NOT_READY;
			}
		}
	}

	return PKF_SUCCESS;
}

PKFReturnCode PKFFilterGraph::runAll()
{
	PKFReturnCode result = PKF_ERR_NOT_READY;
	std::vector <IPKFFilter *> sources;

	// First find all filters with no input

	for(uint32_t i=0; i < this->filters.size(); i++)
	{
		if(this->filters[i] == NULL)
		{
			continue;
		}

		bool addit = true;

		for(uint32_t j=0; j < this->filters[i]->getNumberOfConnectors(); j++)
		{
			IPKFConnector *conn = NULL;

			if(this->filters[i]->getConnector(j, &conn) == PKF_SUCCESS)
			{
				if(conn->getType() == PKF_CONNECTOR_TYPE_INPUT)
				{
					addit = false;
				}
			}
		}

		if(addit)
		{
			sources.push_back(this->filters[i]);
		}
	}

	// Then run them in series

	for(uint32_t n=0; n < sources.size(); n++)
	{
		result = this->runGraphFromFilter(sources[n]);
		
		if(result != PKF_SUCCESS)
		{
			return result;
		}
	}
	
	return result;
}

PKFReturnCode PKFFilterGraph::interruptAll()
{
	PKFReturnCode result = PKF_SUCCESS;

	for(uint32_t i=0; i < this->filters.size(); i++)
	{
		result = this->filters[i]->interrupt();

		if(result != PKF_SUCCESS)
		{
			return result;
		}
	}

	return result;
}

void PKFFilterGraph::selfPropertyChanged(PKProperty *prop, PKVariant *oldValue)
{
	if(prop->getName() == PKFFilterGraph::LOGFILE_WSTRING_PROPERTY)
	{
		PKVariant *v = prop->getValue();

		if(this->logger->open(PKVALUE_WSTRING(v)))
		{
			PKVariant *nv = this->get(PKFFilterGraph::GRAPHNAME_STRING_PROPERTY);
			this->logger->log("MODimage log started for graph: " + PKVALUE_STRING(nv));
		}
	}
}

