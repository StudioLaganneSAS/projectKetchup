//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKDownload.h"
#include "PKMemBuffer.h"
#include "PKStr.h"
#include "PKTime.h"

#ifdef WIN32
#include <Wininet.h>
#endif

#ifdef MACOSX
#include <errno.h>
#endif

//
// Properties
// 

std::string PKDownload::ID_UINT32_PROPERTY         = "id";
std::string PKDownload::URL_STRING_PROPERTY        = "url";
std::string PKDownload::USERAGENT_STRING_PROPERTY  = "userAgent";
std::string PKDownload::REFERER_STRING_PROPERTY    = "referer";
std::string PKDownload::TIMEOUT_UINT32_PROPERTY    = "timeout";
std::string PKDownload::FILENAME_WSTRING_PROPERTY  = "filename";
std::string PKDownload::MEMBUFFER_VOIDP_PROPERTY   = "membuffer";
std::string PKDownload::STATUS_UINT32_PROPERTY     = "status";
std::string PKDownload::RETURNCODE_UINT32_PROPERTY = "returnCode";
std::string PKDownload::ERRORCODE_UINT32_PROPERTY  = "errorCode";
std::string PKDownload::FILESIZE_UINT64_PROPERTY   = "fileSize";
std::string PKDownload::DOWNLOADED_UINT64_PROPERTY = "downloaded";
std::string PKDownload::PROGRESS_UINT32_PROPERTY   = "progress";
std::string PKDownload::DATASTARTED_BOOL_PROPERTY  = "dataStarted";
std::string PKDownload::DATAFINISHED_BOOL_PROPERTY = "dataFinished";
std::string PKDownload::DATASTALLED_BOOL_PROPERTY  = "dataSalled";

//
// MACOSX stuff
//

#ifdef MACOSX

static const CFOptionFlags kNetworkEvents = kCFStreamEventOpenCompleted |
											kCFStreamEventHasBytesAvailable |
											kCFStreamEventEndEncountered |
											kCFStreamEventErrorOccurred;


void PKDownload::ReadStreamClientCallBack(CFReadStreamRef stream, 
										  CFStreamEventType type, 
										  void *clientCallBackInfo)
{
	PKDownload *dl = (PKDownload *) clientCallBackInfo;
	
	if(dl == NULL)
	{
		return;
	}

    PKVariant *i = dl->get(PKDownload::ID_UINT32_PROPERTY);
    uint32_t id  = PKVALUE_UINT32(i);
	
	UInt8	buffer[16 * 1024];
	CFIndex	bytesRead;
	
	switch(type)
	{
		case kCFStreamEventOpenCompleted:
		{
			// Flag the status
			
			PKVariantUInt32 s(PKDL_DOWNLOADING);
            dl->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
			
            if(dl->callback != NULL)
            {
                dl->callback->dlStatus(id, PKDL_DOWNLOADING);
            }			
		}
		break;
			
		case kCFStreamEventHasBytesAvailable:
		{
			if(!dl->sizeSet)
			{
				CFHTTPMessageRef response = (CFHTTPMessageRef) CFReadStreamCopyProperty(dl->stream, kCFStreamPropertyHTTPResponseHeader);

				if(response != NULL)
				{
					// Get content length
					
					CFStringRef length = CFHTTPMessageCopyHeaderFieldValue(response, CFSTR("Content-Length"));
					
					if(length != NULL)
					{
						std::wstring lStr = PKStr::cfStringToWString(length);
						uint64_t fsize = PKStr::wstringToUint64(lStr);
						
						PKVariantUInt64 size(fsize);
						dl->set(PKDownload::FILESIZE_UINT64_PROPERTY, &size);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlFileSize(id, fsize);
						}

						dl->dlSize  = fsize;
						dl->sizeSet = true;						
						
						CFRelease(length);
					}
					
					// Get HTML return code
					
					CFIndex code = CFHTTPMessageGetResponseStatusCode(response);

					PKVariantUInt32 rcode(code);
					dl->set(PKDownload::RETURNCODE_UINT32_PROPERTY, &rcode);			
					
					// Check the return code
					
					if(code == 400)
					{
						PKVariantUInt32 err(PKDL_ERR_BAD_REQUEST);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_BAD_REQUEST);
						}
						
						dl->stop();
						return;
					}
					
					if(code == 401)
					{
						PKVariantUInt32 err(PKDL_ERR_UNAUTHORIZED);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_UNAUTHORIZED);
						}
						
						dl->stop();
						return;
					}
					
					if(code == 403)
					{
						PKVariantUInt32 err(PKDL_ERR_FORBIDDEN);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_FORBIDDEN);
						}
						
						dl->stop();
						return;
					}
					
					if(code == 404)
					{
						PKVariantUInt32 err(PKDL_ERR_NOT_FOUND);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_NOT_FOUND);
						}
						
						dl->stop();
						return;
					}
					
					if(code >= 500)
					{
						PKVariantUInt32 err(PKDL_ERR_SERVER_ERROR);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_SERVER_ERROR);
						}
						
						dl->stop();
						return;
					}
				}
			}
			
			// Now read the data
			
			bytesRead = CFReadStreamRead(stream, buffer, sizeof(buffer));
			
			if(bytesRead > 0)
			{
				// Check how much was downloaded
				
				dl->dlSoFar += bytesRead;
				
				// Update internal vars
				
				PKVariantUInt64 downloaded(dl->dlSoFar);
				dl->set(PKDownload::DOWNLOADED_UINT64_PROPERTY, &downloaded);
				
				if(dl->dlSize > 0)
				{
					uint32_t progress = (uint32_t) dl->dlSoFar * 100 / (uint32_t) dl->dlSize;
				
					PKVariantUInt32 p(progress);
					dl->set(PKDownload::PROGRESS_UINT32_PROPERTY, &p);
				
					// Update progress
				
					if(dl->callback != NULL)
					{
						dl->callback->dlProgress(id,  dl->dlSoFar, dl->dlSize);
					}
				}

				// We need to write that data
				// to file or to the mem buffer
				
				if(dl->output != NULL)
				{
					// To file
					
					bool dataStart = false;
					
					// Check if file is open
					
					if(!dl->output->isOpen())
					{
						PKVariant *f = dl->get(PKDownload::FILENAME_WSTRING_PROPERTY);
						std::wstring filename = PKVALUE_WSTRING(f);
						
						// Open the output file
						// and see if we succeed
						
						PKFileError error = dl->output->open(filename,
															 PKFILE_MODE_WRITE);
						
						if(error == PKFILE_OK)
						{
							dataStart = true;
						}
						else
						{
							PKVariantUInt32 err(PKDL_ERR_CANTWRITE);
							dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
							
							if(dl->callback != NULL)
							{
								dl->callback->dlError(id, PKDL_ERR_CANTWRITE);
							}
							
							PKVariantUInt32 s(PKDL_ERROR);
							dl->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
							
							if(dl->callback != NULL)
							{
								dl->callback->dlStatus(id, PKDL_ERROR);
							}

							dl->stop();
							return;
						}
					}
					
					// Write to file
					
					if(dl->output->isOpen())
					{
						dl->output->write(buffer, (uint32_t)bytesRead);
					}
					
					if(dataStart)
					{
						// Signal we started writing
						
						PKVariantBool d(true);
						dl->set(PKDownload::DATASTARTED_BOOL_PROPERTY , &d);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlDataStarted(id);
						}
					}
				}
				else
				{
					// To memory buffer
					
					if(dl->memBuffer == NULL)
					{
						PKVariantUInt32 err(PKDL_ERR_NOBUFFER);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_NOBUFFER);
						}
						
						PKVariantUInt32 s(PKDL_ERROR);
						dl->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlStatus(id, PKDL_ERROR);
						}

						dl->stop();
						return;
					}
					
					bool dataStart = false;
					
					// Check if file is open
					
					if(dl->memBuffer->getDataSize() == 0)
					{
						dataStart = true;
					}
					
					// Check buffer size
					
					uint32_t bsize   = dl->memBuffer->getSize();
					uint32_t current = dl->memBuffer->getDataSize();
					
					if(current + (uint32_t)bytesRead > bsize)
					{
						// Reallocate buffer
						dl->memBuffer->reAlloc(current + (uint32_t)bytesRead + 32768);
					}
					
					// Write to buffer
					
					char *dbuffer = NULL;
					dl->memBuffer->lock((void **) &dbuffer);
					
					if(dbuffer)
					{
						memcpy(dbuffer + current, buffer, (uint32_t)bytesRead);
						dl->memBuffer->setDataSize(current+(uint32_t)bytesRead);
						dl->memBuffer->unlock();
					}
					else
					{
						PKVariantUInt32 err(PKDL_ERR_NOBUFFER);
						dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlError(id, PKDL_ERR_NOBUFFER);
						}
						
						PKVariantUInt32 s(PKDL_ERROR);
						dl->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlStatus(id, PKDL_ERROR);
						}
						
						dl->stop();
						return;
					}
					
					// Signal
					
					if(dataStart)
					{
						// Signal we started writing
						
						PKVariantBool d(true);
						dl->set(PKDownload::DATASTARTED_BOOL_PROPERTY , &d);
						
						if(dl->callback != NULL)
						{
							dl->callback->dlDataStarted(id);
						}
					}
				}
			}
			else if ( bytesRead < 0 )
			{
				PKVariantUInt32 err(PKDL_ERR_UNKNOWN);
				dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
				
				if(dl->callback != NULL)
				{
					dl->callback->dlError(id, PKDL_ERR_UNKNOWN);
				}

				dl->stop();
			}
		}
		break;
			
		case kCFStreamEventEndEncountered:
		{
			if(dl->output != NULL)
			{
				dl->output->close();
			}
			
			// Flag it
			
            PKVariantBool f(true);
            dl->set(PKDownload::DATAFINISHED_BOOL_PROPERTY , &f);
			
            if(dl->callback != NULL)
            {
                dl->callback->dlDataFinished(id);
            }
			
            PKVariantUInt32 s(PKDL_FINISHED);
            dl->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
			
            if(dl->callback != NULL)
            {
                dl->callback->dlStatus(id, PKDL_FINISHED);
            }

			dl->stop();
		}
		break;
			
		case kCFStreamEventErrorOccurred:
		{
			// Check the error code
			
			CFStreamError cferr = CFReadStreamGetError(dl->stream);

			if(cferr.domain == 12 || // kCFStreamErrorDomainNetDB
			   cferr.domain == 13)   // kCFStreamErrorDomainSystemConfiguration
			{
				PKVariantUInt32 err(PKDL_ERR_NOT_CONNECTED);
				dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
				
				if(dl->callback != NULL)
				{
					dl->callback->dlError(id, PKDL_ERR_NOT_CONNECTED);
				}
			
				dl->stop();
				return;
			}
			
			if(cferr.domain == 4) // kCFStreamErrorDomainHTTP
			{
				if(cferr.error == -4) // kCFStreamErrorHTTPConnectionLost
				{
					PKVariantUInt32 err(PKDL_ERR_INTERRUPTED);
					dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
					
					if(dl->callback != NULL)
					{
						dl->callback->dlError(id, PKDL_ERR_INTERRUPTED);
					}
					
					dl->stop();
					return;					
				}
			}
			
			if(cferr.domain == kCFStreamErrorDomainPOSIX)
			{
				// Error code is from errno.h
				
				if(cferr.error == ENETDOWN)
				{
					PKVariantUInt32 err(PKDL_ERR_NOT_CONNECTED);
					dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
					
					if(dl->callback != NULL)
					{
						dl->callback->dlError(id, PKDL_ERR_NOT_CONNECTED);
					}
					
					dl->stop();
					return;
				}
				
				if(cferr.error == ENETUNREACH || cferr.error == EHOSTUNREACH)
				{
					PKVariantUInt32 err(PKDL_ERR_CANTREACH);
					dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
					
					if(dl->callback != NULL)
					{
						dl->callback->dlError(id, PKDL_ERR_CANTREACH);
					}
					
					dl->stop();
					return;
				}
				
				if(cferr.error == ENOTCONN || cferr.error == ECONNRESET)
				{
					PKVariantUInt32 err(PKDL_ERR_INTERRUPTED);
					dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
					
					if(dl->callback != NULL)
					{
						dl->callback->dlError(id, PKDL_ERR_INTERRUPTED);
					}
					
					dl->stop();
					return;					
				}
				
				
			}
			
			// Flag it
			
			PKVariantUInt32 err(PKDL_ERR_UNKNOWN);
			dl->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
			
			if(dl->callback != NULL)
			{
				dl->callback->dlError(id, PKDL_ERR_UNKNOWN);
			}
			
			dl->stop();
		}
		break;
			
		default:
			break;
	}	
}

#endif

//
// PKDownload
//

uint32_t PKDownload::lastId = 0;

PKDownload::PKDownload()
{
    PKOBJECT_ADD_UINT32_PROPERTY(id, PKDownload::lastId++);
    PKOBJECT_ADD_STRING_PROPERTY(url, "");
    PKOBJECT_ADD_STRING_PROPERTY(userAgent, "");
    PKOBJECT_ADD_STRING_PROPERTY(referer, "");
    PKOBJECT_ADD_UINT32_PROPERTY(timeout, 30);
    PKOBJECT_ADD_WSTRING_PROPERTY(filename, L"");
    PKOBJECT_ADD_VOIDP_PROPERTY(membuffer, NULL);
    PKOBJECT_ADD_UINT32_PROPERTY(status, PKDL_IDLE);
    PKOBJECT_ADD_UINT32_PROPERTY(returnCode, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(errorCode, 0);
    PKOBJECT_ADD_UINT64_PROPERTY(fileSize, 0);
    PKOBJECT_ADD_UINT64_PROPERTY(downloaded, 0);
    PKOBJECT_ADD_UINT32_PROPERTY(progress, 0);
    PKOBJECT_ADD_BOOL_PROPERTY(dataStarted, false);
    PKOBJECT_ADD_BOOL_PROPERTY(dataFinished, false);
    PKOBJECT_ADD_BOOL_PROPERTY(dataSalled, false);

    this->callback     = NULL;
    
    this->dlThread     = new PKThread(this);
    this->watchThread  = new PKThread(this);

    this->resumeFrom   = 0;
    this->cancelFlag   = false;
    this->finished     = false;
    this->sizeSet      = false;

    this->output    = NULL;
    this->memBuffer = NULL;

#ifdef WIN32
    this->m_ulObjRefCount = 1;
#endif

#ifdef MACOSX
	this->stream  = NULL;
	this->message = NULL;
	this->dlSoFar = 0;
	this->dlSize  = 0;
#endif
	
    PKSafeEvents::initialize();
}

PKDownload::~PKDownload()
{
	this->stop();
	
    delete this->dlThread;
    delete this->watchThread;
}

void PKDownload::setDownloadCallback(IPKDownloadCallback *c)
{
    this->callback = c;
}

bool PKDownload::start()
{
    // Check we have everything 
    // we need to start the dl...

    PKVariant *u = this->get(PKDownload::URL_STRING_PROPERTY);
    std::string url = PKVALUE_STRING(u);

    if(url == "")
    {
        // No URL ??
        return false;
    }

    if(url.length() < 10)
    {
        // needs to be at least http://xxxx
        return false;
    }

    if(PKStr::toLower(url).substr(0, 7) != "http://")
    {
        // We only handle http
        return false;
    }

    PKVariant *f = this->get(PKDownload::FILENAME_WSTRING_PROPERTY);
    std::wstring filename = PKVALUE_WSTRING(f);

    PKVariant *m = this->get(PKDownload::MEMBUFFER_VOIDP_PROPERTY);
    void *membuf = PKVALUE_VOIDP(m);

    if(filename == L"" && membuf == NULL)
    {
        // No filename or mem buffer
        return false;
    }

#ifdef WIN32
    if(this->dlThread->isRunning())
    {
        // Already running
        return false;
    }
#endif
	
#ifdef MACOSX

	if(this->stream != NULL)
	{
        // Already running
		return false;
	}
	
	if(this->watchThread->isRunning())
	{
		this->watchThread->stop();
	}
	
#endif 
	
    // All good - GO!

    if(this->output != NULL)
    {
        this->output->close();
        delete this->output;
        this->output = NULL;
    }

    if(this->memBuffer != NULL)
    {
        this->memBuffer = NULL;
    }

    if(filename != L"")
    {
        this->output = new PKFile();
    }
    else
    {
        if(membuf != NULL)
        {
            this->memBuffer = (PKMemBuffer *) membuf;
            this->memBuffer->setDataSize(0);
        }
    }

    this->cancelFlag = false;
    this->finished   = false;
    this->sizeSet    = false;

	PKVariantUInt64 downloaded(0);
	this->set(PKDownload::DOWNLOADED_UINT64_PROPERTY, &downloaded);

    PKVariantUInt32 s(PKDL_IDLE);
    this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

#ifdef WIN32
    this->dlThread->start();
    this->watchThread->start();
#endif
	
#ifdef MACOSX
	this->dlSoFar = 0;
	this->dlSize  = 0;

	PKSafeEvents::postSafeEvent(this, 
								"START",
								NULL);	

    this->watchThread->start();
#endif
	
    return true;
}

bool PKDownload::stop()
{
#ifdef WIN32
    if(this->dlThread->isRunning())
    {
        // Stop the download
        this->cancelFlag = true;

        int64_t time = PKTime::getTime();
        int64_t base = PKTime::getTimeScale();

        double startTime = 0;

        if(base != 0)
        {
            startTime = (double) (time) / (double) base;
        }

        while(this->dlThread->isRunning())
        {
            PKTime::sleep(10);

            int64_t elapsed = PKTime::getTime();
            double now = 30;

            if(base != 0)
            {
                now = (double) elapsed / (double) base;
            }

            // Give it 10 seconds max
            // to cancel the connection
            
            if(now >= (startTime + 10))
            {
                this->dlThread->forceImmediateStop();
                break;
            }
        }

        if(this->output != NULL)
        {
            if(this->output->isOpen())
            {
                this->output->close();
            }
        }
    }

    if(this->watchThread->isRunning())
    {
        this->watchThread->stop();
    }
#endif
	
#ifdef MACOSX
	
	if(this->stream != NULL)
	{
		CFReadStreamSetClient(stream, kCFStreamEventNone, NULL, NULL);
		
		CFReadStreamUnscheduleFromRunLoop(stream, 
										  CFRunLoopGetCurrent(), 
										  kCFRunLoopCommonModes);
		
		CFReadStreamClose(stream);
		CFRelease(stream);			
	}
	
	if(this->message != NULL)
	{
		CFRelease(this->message);
	}
	
	this->stream  = NULL;
	this->message = NULL;
	
#endif
	
    return true;
}

bool PKDownload::immediateStop()
{
#ifdef MACOSX
	this->stop();
#endif
	
#ifdef WIN32
    if(this->dlThread->isRunning())
    {
        this->dlThread->forceImmediateStop();
        
        if(this->output != NULL)
        {
            if(this->output->isOpen())
            {
                this->output->close();
            }
        }
    }
	
    if(this->watchThread->isRunning())
    {
        this->watchThread->forceImmediateStop();
    }
#endif

    return true;
}

// PKThread

void PKDownload::runThread(PKThread *thread)
{
    // This is where we start the download

    if(thread == this->dlThread)
    {
        PKVariant *urlV = this->get(PKDownload::URL_STRING_PROPERTY);
        std::string url = PKVALUE_STRING(urlV);

        PKVariant *f = this->get(PKDownload::FILENAME_WSTRING_PROPERTY);
        std::wstring filename = PKVALUE_WSTRING(f);

#ifdef WIN32

        std::wstring wurl = PKStr::stringToWString(url);

        DeleteUrlCacheEntry(wurl.c_str());

        HRESULT hr = URLOpenStream(NULL, 
							       wurl.c_str(), 
							       0, this);

        if(this->output != NULL)
        {
            this->output->close();
        }
#endif
		
        return;
    }

    if(thread == this->watchThread)
    {
        PKVariant *t = this->get(PKDownload::TIMEOUT_UINT32_PROPERTY);
        uint32_t timeout = PKVALUE_UINT32(t);
        
        int64_t start = PKTime::getTime();
        int64_t scale = PKTime::getTimeScale();

        double startTime = (double) ((double) start / (double) scale);

        if(scale != 0)
        {
            while(true)
            {
                if(thread->getExitFlag())
                {
                    break;
                }

                PKTime::sleep(500);

                int64_t now64 = PKTime::getTime();
                double now = (double) ((double) now64 / (double) scale);

                if(now >= (startTime + timeout))
                {
                    PKVariant *s = this->get(PKDownload::STATUS_UINT32_PROPERTY);
                    uint32_t status = PKVALUE_UINT32(s);

                    if(status == PKDL_CONNECTING || 
                       status == PKDL_REDIRECTING)
                    {
                        // Still conecting, it's not
                        // right so let's flag it...
                        
                        PKSafeEvents::postSafeEvent(this, 
                                                    "TIMEOUT",
                                                    NULL);

                        break;
                    }

                    // Otherwise break anyway
                    break;
                }
            }
        }
    }
}

// Safe Events

void PKDownload::processSafeEvent(std::string  eventName,
					              void        *payload)
{
    PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
    uint32_t id  = PKVALUE_UINT32(i);

    if(eventName == "TIMEOUT")
    {
        // STOP

        this->immediateStop();

        // Flag it!

        PKVariantUInt32 err(PKDL_ERR_TIMEOUT);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_TIMEOUT);
        }
    }
	
#ifdef MACOSX	
	if(eventName == "START")
	{
		PKVariantUInt32 s(PKDL_CONNECTING);

		CFStringRef	rawCFString;
		CFStringRef	normalizedCFString;
		CFStringRef	escapedCFString;
		CFURLRef	urlRef;
		
		CFStreamClientContext ctxt = { 0, (void*)NULL, NULL, NULL, NULL };
		
		PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
		uint32_t id  = PKVALUE_UINT32(i);
		
        PKVariant *urlV = this->get(PKDownload::URL_STRING_PROPERTY);
        std::string url = PKVALUE_STRING(urlV);

		PKVariant *u = this->get(PKDownload::USERAGENT_STRING_PROPERTY);
		std::string ua = PKVALUE_STRING(u);
		
		PKVariant *r = this->get(PKDownload::REFERER_STRING_PROPERTY);
		std::string referer = PKVALUE_STRING(r);
		
		// Prepare URL
		
		rawCFString = CFStringCreateWithCString(NULL, url.c_str(), CFStringGetSystemEncoding());
		
		if(rawCFString == NULL)
		{
			goto MAC_Bail;
		}
		
		normalizedCFString	= CFURLCreateStringByReplacingPercentEscapes(NULL, rawCFString, CFSTR(""));
		
		if(normalizedCFString == NULL)
		{
			CFRelease( rawCFString );
			goto MAC_Bail;
		}
		
		escapedCFString	= CFURLCreateStringByAddingPercentEscapes(NULL, normalizedCFString, NULL, NULL, kCFStringEncodingUTF8);
		
		if(escapedCFString == NULL)
		{
			CFRelease( rawCFString );
			CFRelease( normalizedCFString );
			goto MAC_Bail;
		}
		
		urlRef= CFURLCreateWithString( kCFAllocatorDefault, escapedCFString, NULL );
		
		CFRelease(rawCFString);
		CFRelease(normalizedCFString);
		CFRelease(escapedCFString);
		
		if(urlRef == NULL)
		{
			goto MAC_Bail;
		}
		
		// Create HTTP message
		
		this->message = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("GET"), urlRef, kCFHTTPVersion1_1);
		
		if(this->message == NULL) goto MAC_Bail;
		
		// Create stream object
		
		this->stream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, this->message);
		
		if (this->stream == NULL) goto MAC_Bail;
		
		 // Set user-agent and referer if needed
		 
		 if(referer != "")
		 {
			 CFStringRef refCFSTR = CFStringCreateWithCString(NULL, referer.c_str(), CFStringGetSystemEncoding());
		 
			 if(refCFSTR != NULL)
			 {
				 CFHTTPMessageSetHeaderFieldValue(this->message, 
												  CFSTR("Referer"), 
												  refCFSTR); 			
				 CFRelease(refCFSTR);
			 }
		 }
		 
		 // User-Agent
		 
		 if(ua != "")
		 {
			 CFStringRef uaCFSTR = CFStringCreateWithCString(NULL, ua.c_str(), CFStringGetSystemEncoding());
		 
			 if(uaCFSTR != NULL)
			 {
				CFHTTPMessageSetHeaderFieldValue(this->message, 
												 CFSTR("User-Agent"), 
												 uaCFSTR); 			
				 CFRelease(uaCFSTR);
			 }
		 }
		 
		// Set preferences
		
		CFReadStreamSetProperty(this->stream, kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue);
		
		// Set the client notifier
		
		ctxt.info = (void *) this;
		
		if(CFReadStreamSetClient(this->stream, kNetworkEvents, PKDownload::ReadStreamClientCallBack, &ctxt ) == false)
		{
			goto MAC_Bail;
		}		
		
		// And start the transfer
		
		CFReadStreamScheduleWithRunLoop(this->stream, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
		
		if(CFReadStreamOpen(this->stream) == false)
		{
			goto MAC_Bail;
		}

		this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);
		
		if(this->callback != NULL)
		{
			this->callback->dlStatus(id, PKDL_CONNECTING);
		}

		return;
		
	MAC_Bail:
		
		PKVariantUInt32 err(PKDL_ERR_UNKNOWN);
		this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);
		
		if(this->callback != NULL)
		{
			this->callback->dlError(id, PKDL_ERR_UNKNOWN);
		}
		
		this->stop();
		
		return;
	}
#endif
}

#ifdef WIN32

// IBindStatusCallback

HRESULT STDMETHODCALLTYPE PKDownload::OnStartBinding(DWORD dwReserved,
										             IBinding __RPC_FAR *pib)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE PKDownload::GetPriority(LONG __RPC_FAR *pnPriority)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnLowResource(DWORD reserved)
{
    // running out of space ?
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnDataAvailable(DWORD grfBSCF,
										              DWORD dwSize,
										              FORMATETC __RPC_FAR *pformatetc,
										              STGMEDIUM __RPC_FAR *pstgmed)
{
    // Check for cancel flag

    if(this->cancelFlag)
    {
		return E_ABORT;
	}

    PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
    uint32_t id  = PKVALUE_UINT32(i);

    // See what's up

    uint64_t size   = 0;
	char    *buffer = NULL;

	if(pstgmed != NULL)
	{
		if(pstgmed->tymed == TYMED_ISTREAM)
		{
			DWORD	 read   = 0;
			IStream *stream = pstgmed->pstm;

			if(stream != NULL)
			{
				LARGE_INTEGER  off;
				ULARGE_INTEGER pos;

				off.QuadPart = 0;
				stream->Seek(off, STREAM_SEEK_CUR, &pos);

				size = dwSize;
				size -= pos.QuadPart;

				buffer = new char[(uint32_t)size];
				stream->Read(buffer, (ULONG)size, &read);
			}
		}
	}

	if(buffer == NULL)
	{
		return E_ABORT;
	}

    // Write it down

    if(this->output != NULL)
    {
        // To File

        bool dataStart = false;

        // Check if file is open

        if(!this->output->isOpen())
        {
            PKVariant *f = this->get(PKDownload::FILENAME_WSTRING_PROPERTY);
            std::wstring filename = PKVALUE_WSTRING(f);
            
            // Open the output file
            // and see if we succeed

            PKFileError error = this->output->open(filename,
                                                   PKFILE_MODE_WRITE);

            if(error == PKFILE_OK)
            {
                dataStart = true;
            }
            else
            {
                PKVariantUInt32 err(PKDL_ERR_CANTWRITE);
                this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

                if(this->callback != NULL)
                {
                    this->callback->dlError(id, PKDL_ERR_CANTWRITE);
                }

                PKVariantUInt32 s(PKDL_ERROR);
                this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

                if(this->callback != NULL)
                {
                    this->callback->dlStatus(id, PKDL_ERROR);
                }

                delete [] buffer;
                return E_ABORT;
            }
        }

        // Write to file

		if(this->output->isOpen())
		{
			this->output->write(buffer, (uint32_t)size);
		}

        if(dataStart)
        {
            // Signal we started writing

            PKVariantBool d(true);
            this->set(PKDownload::DATASTARTED_BOOL_PROPERTY , &d);

            if(this->callback != NULL)
            {
                this->callback->dlDataStarted(id);
            }
        }

        if(this->finished)
        {
            // All done

            this->output->close();

            PKVariantBool f(true);
            this->set(PKDownload::DATAFINISHED_BOOL_PROPERTY , &f);

            if(this->callback != NULL)
            {
                this->callback->dlDataFinished(id);
            }

            PKVariantUInt32 s(PKDL_FINISHED);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_FINISHED);
            }
        }
        
        delete [] buffer;
		return S_OK;
    }
    else
    {
        // To Memory

        if(this->memBuffer == NULL)
        {
            PKVariantUInt32 err(PKDL_ERR_NOBUFFER);
            this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

            if(this->callback != NULL)
            {
                this->callback->dlError(id, PKDL_ERR_NOBUFFER);
            }

            PKVariantUInt32 s(PKDL_ERROR);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_ERROR);
            }

            delete [] buffer;
            return E_ABORT;
        }

        bool dataStart = false;

        // Check if file is open

        if(this->memBuffer->getDataSize() == 0)
        {
            dataStart = true;
        }

        // Check buffer size

        uint32_t bsize   = this->memBuffer->getSize();
        uint32_t current = this->memBuffer->getDataSize();

        if(current + (uint32_t)size > bsize)
        {
            // Reallocate buffer
            this->memBuffer->reAlloc(current + (uint32_t)size + 32768);
        }

        // Write to buffer

        char *dbuffer = NULL;
        this->memBuffer->lock((void **) &dbuffer);

        if(dbuffer)
        {
            memcpy(dbuffer + current, buffer, (uint32_t)size);
            this->memBuffer->setDataSize(current+(uint32_t)size);
            this->memBuffer->unlock();
        }
        else
        {
            PKVariantUInt32 err(PKDL_ERR_NOBUFFER);
            this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

            if(this->callback != NULL)
            {
                this->callback->dlError(id, PKDL_ERR_NOBUFFER);
            }

            PKVariantUInt32 s(PKDL_ERROR);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_ERROR);
            }

            delete [] buffer;
            return E_ABORT;
        }

        // Signal
		
        if(dataStart)
        {
            // Signal we started writing

            PKVariantBool d(true);
            this->set(PKDownload::DATASTARTED_BOOL_PROPERTY , &d);

            if(this->callback != NULL)
            {
                this->callback->dlDataStarted(id);
            }
        }

        if(this->finished)
        {
            // All done

            PKVariantBool f(true);
            this->set(PKDownload::DATAFINISHED_BOOL_PROPERTY , &f);

            if(this->callback != NULL)
            {
                this->callback->dlDataFinished(id);
            }

            PKVariantUInt32 s(PKDL_FINISHED);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_FINISHED);
            }
        }
        
        delete [] buffer;
		return S_OK;
    }
    
    delete [] buffer;
    return E_ABORT;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnProgress(ULONG ulProgress,
									             ULONG ulProgressMax,
									             ULONG ulStatusCode,
									             LPCWSTR szStatusText)
{
	if(this->cancelFlag)
	{
		return E_ABORT;
	}

    PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
    uint32_t id  = PKVALUE_UINT32(i);

	switch(ulStatusCode)
	{
	    case BINDSTATUS_FINDINGRESOURCE:
	    case BINDSTATUS_CONNECTING:
        {
            PKVariantUInt32 s(PKDL_CONNECTING);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_CONNECTING);
            }
        }
        break;

	    case BINDSTATUS_REDIRECTING:
        {
            PKVariantUInt32 s(PKDL_REDIRECTING);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_REDIRECTING);
            }
        }
        break;

        case BINDSTATUS_BEGINDOWNLOADDATA:
	    case BINDSTATUS_DOWNLOADINGDATA:
        {
            PKVariantUInt32 s(PKDL_DOWNLOADING);
            this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

            if(this->callback != NULL)
            {
                this->callback->dlStatus(id, PKDL_DOWNLOADING);
            }
        }
        break;

        case BINDSTATUS_ENDDOWNLOADDATA:
        {
            // This can also happen
            // when the connection has
            // been interrupted, so check

            if(ulProgress != ulProgressMax)
            {
                PKVariantUInt32 err(PKDL_ERR_INTERRUPTED);
                this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

                if(this->callback != NULL)
                {
                    this->callback->dlError(id, PKDL_ERR_INTERRUPTED);
                }

                this->finished = false;

                return E_ABORT;
            }

		    this->finished = true;

		    return S_OK; // return because we don't want
					     // to send the message now, since
					     // the file is not 100% written yet
        }
        break;

	    default:
		{
            // Nothing to do
		}
		break;
	}

    if(ulProgressMax != 0)
    {
        // Check if size is set

        if(!this->sizeSet)
        {
            PKVariantUInt64 size(ulProgressMax);
            this->set(PKDownload::FILESIZE_UINT64_PROPERTY, &size);

            if(this->callback != NULL)
            {
                this->callback->dlFileSize(id, ulProgressMax);
            }

            this->sizeSet = true;
        }

        // Update internal vars

        PKVariantUInt64 downloaded(ulProgress);
        this->set(PKDownload::DOWNLOADED_UINT64_PROPERTY, &downloaded);

        uint32_t progress = (uint32_t) ulProgress * 100 / (uint32_t) ulProgressMax;

        PKVariantUInt32 p(progress);
        this->set(PKDownload::PROGRESS_UINT32_PROPERTY, &p);

        // Update progress

        if(this->callback != NULL)
        {
            this->callback->dlProgress(id, ulProgress, ulProgressMax);
        }
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnStopBinding(HRESULT hresult,
										            LPCWSTR szError)
{
    if(hresult != S_OK)
    {
        PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
        uint32_t id  = PKVALUE_UINT32(i);

        if(hresult == 0x800C0005) // Resource not found
        {
            PKVariantUInt32 err(PKDL_ERR_CANTREACH);
            this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

            if(this->callback != NULL)
            {
                this->callback->dlError(id, PKDL_ERR_CANTREACH);
            }
        }

        // There was an error 
        // so we must signal it

        PKVariantUInt32 s(PKDL_ERROR);
        this->set(PKDownload::STATUS_UINT32_PROPERTY, &s);

        if(this->callback != NULL)
        {
            this->callback->dlStatus(id, PKDL_ERROR);
        }

        return E_ABORT;
    }

	return S_OK;
}

HRESULT STDMETHODCALLTYPE PKDownload::GetBindInfo(DWORD __RPC_FAR *grfBINDF,
									              BINDINFO __RPC_FAR *pbindinfo)
{
	ULONG	Size;

	*grfBINDF = BINDF_ASYNCHRONOUS | 
			    BINDF_ASYNCSTORAGE | 
				BINDF_NOWRITECACHE | 
				BINDF_GETNEWESTVERSION |
				BINDF_PRAGMA_NO_CACHE |
				BINDF_PULLDATA |
                BINDF_NO_UI |
                BINDF_SILENTOPERATION;

	Size = pbindinfo->cbSize;
	memset(pbindinfo, 0, Size);
	pbindinfo->cbSize = Size;
	pbindinfo->dwBindVerb = BINDVERB_GET;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnObjectAvailable(REFIID riid,
											            IUnknown __RPC_FAR *punk)
{
	return E_NOTIMPL;
}

// IHttpNegotiate

HRESULT STDMETHODCALLTYPE PKDownload::BeginningTransaction(LPCWSTR szURL,
											               LPCWSTR szHeader,
											               DWORD dwReserved,
											               LPWSTR *pszAdditionalHeaders)
{
    // See about any additional headers
    // that we want to append to the session

	if (!pszAdditionalHeaders)
	{
		return E_POINTER;
	}

	*pszAdditionalHeaders = NULL;
    std::wstring newHeaders = L"";

    // Referer

    PKVariant *r = this->get(PKDownload::REFERER_STRING_PROPERTY);
    std::string referer = PKVALUE_STRING(r);

    if(referer != "")
    {
		newHeaders += L"Referer: " +
				      PKStr::stringToWString(referer) +
				      L"\r\n";
	}

    // User-Agent

    PKVariant *u = this->get(PKDownload::USERAGENT_STRING_PROPERTY);
    std::string ua = PKVALUE_STRING(u);

    if(ua != "")
    {
		newHeaders = newHeaders + 
                     L"User-Agent: " +
					 PKStr::stringToWString(ua) +
					 L"\r\n";
	}

    if(newHeaders != L"")
    {
		LPWSTR wszAdditionalHeaders =
			(LPWSTR) CoTaskMemAlloc((newHeaders.length()+1) *sizeof(wchar_t));

		if (!wszAdditionalHeaders)
		{
			return E_OUTOFMEMORY;
		}

		memcpy(wszAdditionalHeaders, 
			   newHeaders.c_str(), 
			   newHeaders.length()*sizeof(wchar_t));

		wszAdditionalHeaders[newHeaders.length()] = 0;

		*pszAdditionalHeaders = wszAdditionalHeaders;
    }

	return S_OK;
}

HRESULT STDMETHODCALLTYPE PKDownload::OnResponse(DWORD dwResponseCode,
									             LPCWSTR szResponseHeaders,
									             LPCWSTR szRequestHeaders,
									             LPWSTR *pszAdditionalRequestHeaders)
{
    PKVariant *i = this->get(PKDownload::ID_UINT32_PROPERTY);
    uint32_t id  = PKVALUE_UINT32(i);

    // Check the response code

    PKVariantUInt32 code(dwResponseCode);
    this->set(PKDownload::RETURNCODE_UINT32_PROPERTY, &code);

    if(dwResponseCode == 400)
    {
        PKVariantUInt32 err(PKDL_ERR_BAD_REQUEST);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_BAD_REQUEST);
        }

        return E_ABORT;
    }

    if(dwResponseCode == 401)
    {
        PKVariantUInt32 err(PKDL_ERR_UNAUTHORIZED);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_UNAUTHORIZED);
        }

        return E_ABORT;
    }
    
    if(dwResponseCode == 403)
    {
        PKVariantUInt32 err(PKDL_ERR_FORBIDDEN);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_FORBIDDEN);
        }

        return E_ABORT;
    }

    if(dwResponseCode == 404)
    {
        PKVariantUInt32 err(PKDL_ERR_NOT_FOUND);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_NOT_FOUND);
        }

        return E_ABORT;
    }

    if(dwResponseCode >= 500)
    {
        PKVariantUInt32 err(PKDL_ERR_SERVER_ERROR);
        this->set( PKDownload::ERRORCODE_UINT32_PROPERTY, &err);

        if(this->callback != NULL)
        {
            this->callback->dlError(id, PKDL_ERR_SERVER_ERROR);
        }

        return E_ABORT;
    }

    return S_OK;
}

#endif
