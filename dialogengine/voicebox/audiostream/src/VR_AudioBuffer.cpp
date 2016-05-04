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
#include <unistd.h>

/* VBT Header */
#ifndef __VECIIface_h__
#    include "VECIIface.h"
#endif

#ifndef VBT_EXTERNAL_ERRORS_H
#    include "VBTExternalErrors.h"
#endif

#ifndef VBTVAALBUFFER_H
#    include "VBTVaalBuffer.h"
#endif

#ifndef VBTVAALSTREAM_H
#    include "VBTVaalStream.h"
#endif

/* Suntec Header */
#ifndef VR_AUDIOBUFFER_H
#    include "VR_AudioBuffer.h"
#endif

#ifndef _VC_WAV_FILE_H_
#    include "VC_WavFile.h"
#endif

// Constructor
VR_AudioBuffer::VR_AudioBuffer(VAAL_PARAMS *pParams, IVBTVaalBuffer *buffer)
: m_pVaalBuffer(buffer)
, m_nBufSizeInBytes(0)
, m_nFrameSizeInBytes(0)
{
    if (NULL != pParams) {
        m_nBufSizeInBytes = pParams->m_ulBufSizeInBytes;
        m_nFrameSizeInBytes = (pParams->m_ulBitsPerSample / 8) * pParams->m_ulNumChannels;
    }
}

// Destructor
VR_AudioBuffer::~VR_AudioBuffer()
{
}

void
VR_AudioBuffer::OnAudioInData(void* data, int len)
{
    if ((len <= 0) || (NULL == data)) {
        return;
    }

    if (NULL == m_pVaalBuffer) {
        return;
    }

    const int WRITE_TIMEOUT = 10000; // 10s
    BYTE* localBuffer = (BYTE*)data;

    while (len - m_nBufSizeInBytes >= 0) {
        int nFramesWritten;
        int nFramesRead = m_nBufSizeInBytes / m_nFrameSizeInBytes;
        if (S_OK != m_pVaalBuffer->Write(localBuffer, nFramesRead, WRITE_TIMEOUT, &nFramesWritten)) {
            return;
        }

        localBuffer += m_nBufSizeInBytes;
        len -= m_nBufSizeInBytes;
    }

    if (len > 0) {
        int nFramesWritten;
        int nFramesRead = len / m_nFrameSizeInBytes;
        if (S_OK != m_pVaalBuffer->Write(localBuffer, nFramesRead, WRITE_TIMEOUT, &nFramesWritten)) {
            return;
        }
    }

    const int WRITE_INTERVAL = 1000; // 1ms
    usleep(WRITE_INTERVAL);
}

/* EOF */