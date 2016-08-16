//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_DOWNLOAD_H
#define PK_DOWNLOAD_H

#include "PKObject.h"
#include "PKThread.h"
#include "PKFile.h"
#include "PKMemBuffer.h"
#include "PKSafeEvents.h"

#ifdef WIN32
#include <windows.h>
#include <Urlmon.h>
#endif

#ifdef MACOSX
#include <Carbon/Carbon.h>
#endif

//
// PKDownloadError
//

typedef enum {

	PKDL_ERR_OK,
    PKDL_ERR_TIMEOUT, 
	PKDL_ERR_CANTREACH,
	PKDL_ERR_INTERRUPTED, 
	PKDL_ERR_NOT_FOUND,
	PKDL_ERR_FORBIDDEN,
	PKDL_ERR_BAD_REQUEST,
	PKDL_ERR_UNAUTHORIZED,
    PKDL_ERR_SERVER_ERROR,
	PKDL_ERR_CANTWRITE,
    PKDL_ERR_NOBUFFER,
	PKDL_ERR_UNKNOWN,
	PKDL_ERR_NOT_CONNECTED,

} PKDownloadError;

//
// PKDownloadState
//

typedef enum {

    PKDL_IDLE,
    PKDL_CONNECTING,
    PKDL_REDIRECTING,
    PKDL_DOWNLOADING,
    PKDL_FINISHED,
    PKDL_ERROR,

} PKDownloadState;

//
// IPKDownloadCallback interface
//

class IPKDownloadCallback
{
public:

    virtual void dlDataStarted(uint32_t id)      = 0;

	virtual void dlFileSize(uint32_t id,
						    uint64_t size)       = 0;

	virtual void dlProgress(uint32_t id,
						    uint64_t downloaded,
							uint64_t total)      = 0;

    virtual void dlStatus(uint32_t id,
                          PKDownloadState state) = 0;

	virtual void dlDataFinished(uint32_t id)     = 0;

	virtual void dldataStalled(uint32_t id)      = 0;

	virtual void dlError(uint32_t id,
						 PKDownloadError reason) = 0;

};

//
// PKDownload
//

class PKDownload : public PKObject, public IPKRunnable, public IPKSafeEventHandler
#ifdef WIN32
,public IBindStatusCallback, public IHttpNegotiate
#endif
{
public:

    // Auto-Set

    static std::string ID_UINT32_PROPERTY;

    // Mandatory

    static std::string URL_STRING_PROPERTY;

    static std::string FILENAME_WSTRING_PROPERTY;
    static std::string MEMBUFFER_VOIDP_PROPERTY;

    // Optional

    static std::string USERAGENT_STRING_PROPERTY;
    static std::string REFERER_STRING_PROPERTY;
    static std::string TIMEOUT_UINT32_PROPERTY;

    // Informational

    static std::string STATUS_UINT32_PROPERTY;
    static std::string RETURNCODE_UINT32_PROPERTY;
    static std::string ERRORCODE_UINT32_PROPERTY;

    static std::string FILESIZE_UINT64_PROPERTY;
    static std::string DOWNLOADED_UINT64_PROPERTY;
    static std::string PROGRESS_UINT32_PROPERTY; // [0-100]

    static std::string DATASTARTED_BOOL_PROPERTY;
    static std::string DATAFINISHED_BOOL_PROPERTY;
    static std::string DATASTALLED_BOOL_PROPERTY;

public:

    PKDownload();

protected:
    virtual ~PKDownload();

public:

    void setDownloadCallback(IPKDownloadCallback *c);

    bool start();
    bool stop();

    bool immediateStop();

protected:

    // IPKRunnable

    void runThread(PKThread *thread);

    // IPKSafeEventHandler

	void processSafeEvent(std::string  eventName,
						  void        *payload);


private:

    PKThread            *dlThread;
    PKThread            *watchThread;
    IPKDownloadCallback *callback;
    uint64_t             resumeFrom;
    PKFile              *output;
    bool                 cancelFlag;
    bool                 finished;
    bool                 sizeSet;
    PKMemBuffer         *memBuffer;

private:

    static uint32_t lastId;

#ifdef MACOSX
	
	CFReadStreamRef  stream;
	CFHTTPMessageRef message;
    uint64_t         dlSoFar;
    uint64_t         dlSize;

	static void	ReadStreamClientCallBack(CFReadStreamRef stream, CFStreamEventType type, void *clientCallBackInfo);	
	
#endif
	
#ifdef WIN32

	// IUnknown

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) 
	{      
		*ppv = NULL;
	
		// IUnknown
		if (IsEqualIID(riid, __uuidof(IUnknown)))
		{
			*ppv = this;
		}
		// IBindStatusCallback
		else if (IsEqualIID(riid, __uuidof(IBindStatusCallback)))
		{
			*ppv = static_cast<IBindStatusCallback *>(this);
		}
		// IHttpNegotiate
		else if (IsEqualIID(riid, __uuidof(IHttpNegotiate)))
		{
			*ppv = static_cast<IHttpNegotiate *>(this);
		}

		if (*ppv)
		{
			(*reinterpret_cast<LPUNKNOWN *>(ppv))->AddRef();
			return S_OK;
		}
	
		return E_NOINTERFACE;
    };
	
    STDMETHODIMP_(ULONG) AddRef() 
	{                             
		return ++m_ulObjRefCount;
	};                             
	
    STDMETHODIMP_(ULONG) Release() 
	{                            
		return --m_ulObjRefCount;
	};

	// IBindStatusCallback

    HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved,
											 IBinding __RPC_FAR *pib);
    
    HRESULT STDMETHODCALLTYPE GetPriority(LONG __RPC_FAR *pnPriority);
    
    HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved);
    
    HRESULT STDMETHODCALLTYPE OnProgress(ULONG ulProgress,
										 ULONG ulProgressMax,
										 ULONG ulStatusCode,
										 LPCWSTR szStatusText);
    
    HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult,
											LPCWSTR szError);
    
    HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD __RPC_FAR *grfBINDF,
										  BINDINFO __RPC_FAR *pbindinfo);
    
    HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF,
											  DWORD dwSize,
											  FORMATETC __RPC_FAR *pformatetc,
											  STGMEDIUM __RPC_FAR *pstgmed);
    
    HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid,
												IUnknown __RPC_FAR *punk);


	// IHttpNegotiate

	HRESULT STDMETHODCALLTYPE BeginningTransaction(LPCWSTR szURL,
												   LPCWSTR szHeaders,
												   DWORD dwReserved,
												   LPWSTR *pszAdditionalHeaders);

	HRESULT STDMETHODCALLTYPE OnResponse(DWORD dwResponseCode,
										 LPCWSTR szResponseHeaders,
										 LPCWSTR szRequestHeaders,
										 LPWSTR *pszAdditionalRequestHeaders);


	DWORD m_ulObjRefCount;

#endif

};

#endif // PK_DOWNLOAD_H
