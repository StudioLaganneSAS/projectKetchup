//
//
// Copyright (c) 2007-2010 - Damien Chavarria
// -----------------------------------------
//
//

#include "PKDownloader.h"
#include "PKStr.h"
#include "PKTime.h"

//
//
//

PKDownloader::PKDownloader()
{
    this->download = NULL;
}

PKDownloader::~PKDownloader()
{
    
}

PKDownloadError PKDownloader::synchronousURLDownloadToFile(std::wstring URL,
                                                           std::wstring filePath)
{
    PKDownloadError err = PKDL_ERR_UNKNOWN;

    this->critsec.wait();

    // Create download object

    this->download = new PKDownload();

    // Setup 

    PKVariantString url(PKStr::wStringToString(URL));
    this->download->set(PKDownload::URL_STRING_PROPERTY, &url);

    PKVariantWString filename(filePath);
    this->download->set(PKDownload::FILENAME_WSTRING_PROPERTY, &filename);

    PKVariantUInt32 timeout(15);
    this->download->set(PKDownload::TIMEOUT_UINT32_PROPERTY, &timeout);

    this->download->setDownloadCallback(this);
    this->error = PKDL_ERR_OK;

    this->doneSec.wait();
    this->done = false;
    this->doneSec.release();
    
    if(this->download->start())
    {
        while(!this->done)
        {
            PKTime::sleep(100);
        }
    }

    this->download->stop();
    this->download->release();
    this->download = NULL;

    err = this->error;

    this->critsec.release();

    return err;
}

void PKDownloader::cancelSynchronousURLDownload()
{
    this->doneSec.wait();
    this->error = PKDL_ERR_INTERRUPTED;
    this->done = true;
    this->doneSec.release();
}


// IPKDownloadCallback

void PKDownloader::dlDataStarted(uint32_t id)
{
}

void PKDownloader::dlFileSize(uint32_t id,
					          uint64_t size)
{
}

void PKDownloader::dlProgress(uint32_t id,
					          uint64_t downloaded,
						      uint64_t total)
{
}

void PKDownloader::dlStatus(uint32_t id,
                            PKDownloadState state)
{
}

void PKDownloader::dlDataFinished(uint32_t id)
{
    this->error = PKDL_ERR_OK;
    this->doneSec.wait();
    this->done = true;
    this->doneSec.release();
}

void PKDownloader::dldataStalled(uint32_t id)
{
    this->error = PKDL_ERR_TIMEOUT;
    this->doneSec.wait();
    this->done = true;
    this->doneSec.release();
}

void PKDownloader::dlError(uint32_t id,
					       PKDownloadError reason)
{
    this->error = reason;
    this->doneSec.wait();
    this->done = true;
    this->doneSec.release();
}
