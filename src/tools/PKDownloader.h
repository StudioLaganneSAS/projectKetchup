//
//
// Copyright (c) 2007-2010 - Damien Chavarria
// -----------------------------------------
//
//

#ifndef PK_DOWNLOADER_H
#define PK_DOWNLOADER_H

#include "PKDownload.h"
#include "PKCritSec.h"

//
// PKDownloader
//

class PKDownloader : public IPKDownloadCallback
{
public:

    PKDownloader();
    ~PKDownloader();

   PKDownloadError synchronousURLDownloadToFile(std::wstring URL,
                                                std::wstring filePath);

   void cancelSynchronousURLDownload();

protected:

    // IPKDownloadCallback

    virtual void dlDataStarted(uint32_t id);

	virtual void dlFileSize(uint32_t id,
						    uint64_t size);

	virtual void dlProgress(uint32_t id,
						    uint64_t downloaded,
							uint64_t total);

    virtual void dlStatus(uint32_t id,
                          PKDownloadState state);

	virtual void dlDataFinished(uint32_t id);

	virtual void dldataStalled(uint32_t id);

	virtual void dlError(uint32_t id,
						 PKDownloadError reason);

private:

    PKDownload *download;
    PKDownloadError error;
    PKCritSec  critsec;
    bool done;
    PKCritSec doneSec;
};

#endif // PK_DOWNLOADER_H