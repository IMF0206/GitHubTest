/**
 * Copyright @ 2015 - 2016 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

/* Standard Header */
#include <ctype.h>

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

/* VBT Header */
#ifndef VBTVAALINT_H
#    include "VBTVaalInt.h"
#endif

/* Suntec Header */
#ifndef VR_VAALSTREAM_H
#    include "VR_VaalStream.h"
#endif

#ifndef VR_AUDIOBUFFER_H
#    include "VR_AudioBuffer.h"
#endif

#ifndef VR_AUDIOBUFFERMANAGER_H
#    include "VR_AudioBufferManager.h"
#endif

#ifndef VR_VOICEBOXAUDIOIN_H
#    include "VR_VoiceBoxAudioIn.h"
#endif

#ifndef VR_AUDIOSTREAMIF_H
#    include "VR_AudioStreamIF.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

// Constructor
VR_VaalStream::VR_VaalStream(
    VR_VoiceBoxAudioIn& audioIn,
    VR_AudioBufferManager& bufferManager
)
: m_audioIn(audioIn)
, m_bufferManager(bufferManager)
, m_audioBuffer(NULL)
, m_bStreamActive(false)
{
}

// Destructor
VR_VaalStream::~VR_VaalStream()
{
    Terminate();
}

HRESULT
VR_VaalStream::Init(VAAL_PARAMS *pParams, IVBTVaalBuffer *pBuffer)
{
    VR_LOGD_FUNC();

    if (NULL == pParams || NULL == pBuffer) {
        return E_FAIL;
    }

    if (!pParams->m_bIsRecord) {
        return E_FAIL;
    }

    HRESULT hr = pBuffer->Init(pParams->m_bIsRecord);
    if (S_OK != hr) {
        return E_FAIL;
    }

    if (NULL != m_audioBuffer) {
        delete m_audioBuffer;
        m_audioBuffer = NULL;
    }

    m_audioBuffer = VR_new VR_AudioBuffer(pParams, pBuffer);
    if (NULL == m_audioBuffer) {
        return E_FAIL;
    }

    m_bufferManager.SetReceiver(m_audioBuffer);
    m_params = *pParams;
    return S_OK;
}


HRESULT VR_VaalStream::Terminate()
{
    VR_LOGD_FUNC();

    if (!m_params.m_bIsRecord) {
        return S_OK;
    }

    if (NULL != m_audioBuffer) {
        delete m_audioBuffer;
        m_audioBuffer = NULL;
    }

    return S_OK;
}


HRESULT VR_VaalStream::Open()
{
    VR_LOGD_FUNC();

    if (!m_params.m_bIsRecord) {
        return E_FAIL;
    }

    return S_OK;
}


HRESULT VR_VaalStream::Close()
{
    VR_LOGD_FUNC();

    if (!m_params.m_bIsRecord) {
        return S_OK;
    }

    return S_OK;
}


HRESULT VR_VaalStream::Reset()
{
    VR_LOGD_FUNC();

    m_bStreamActive = false;
    return S_OK;
}


HRESULT VR_VaalStream::Start()
{
    VR_LOGD_FUNC();

    if (!m_params.m_bIsRecord) {
        return E_FAIL;
    }

    m_bufferManager.SetRecordingFlag(true);
    m_bStreamActive = true;
    return S_OK;
}


HRESULT VR_VaalStream::Stop(bool bAbort)
{
    VR_LOGD_FUNC();

    if (!m_params.m_bIsRecord) {
        return S_OK;
    }

    m_bufferManager.SetRecordingFlag(false);
    m_bStreamActive = false;
    return S_OK;
}

HRESULT VR_VaalStream::IsStreamActive(bool& bIsActive)
{
    VR_LOGD_FUNC();

    bIsActive = m_bStreamActive;
    return S_OK;
}


HRESULT VR_VaalStream::QuerySupport(unsigned long *pulSupportMask)
{
    return S_OK;
}


HRESULT VR_VaalStream::SetProperty(VAAL_PROPTYPE type, VAAL_PROPVAL val)
{
    return S_OK;
}


HRESULT VR_VaalStream::GetProperty(VAAL_PROPTYPE type, VAAL_PROPVAL *pVal)
{
    return S_OK;
}

HRESULT VR_VaalStream::RegisterStatCallback(VBTVaalStatCallback pCallback, void * pUserData)
{
    return S_OK;
}

extern "C"
{
    //  Factory methods
    IVBTVaalStream*  newCustomVBTVaalStream(void)
    {
        VR_LOGD_FUNC();

        if (NULL == VR_AudioStreamIF::Instance()) {
            VR_ERROR("Vaal Stream instance is NULL");
            return NULL;
        }

        return VR_AudioStreamIF::Instance()->GetVaalStream();
    }

    void deleteCustomVBTVaalStream(IVBTVaalStream *pStream)
    {
        VR_LOGD_FUNC();
    }
};

/* EOF */
