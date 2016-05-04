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

/* Suntec Header */
#ifndef VR_AUDIOBUFFERMANAGER_H
#    include "VR_AudioBufferManager.h"
#endif

#ifndef VR_AUDIOBUFFER_H
#    include "VR_AudioBuffer.h"
#endif

#ifndef CXX_CL_WAITOBJ_H
#    include "CL_WaitObj.h"
#endif

#ifndef CXX_CL_SYNCOBJ_H
#    include "CL_SyncObj.h"
#endif

#ifndef CXX_CL_AUTOSYNC_H
#    include "CL_AutoSync.h"
#endif

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

// Constructor
VR_AudioBufferManager::VR_AudioBufferManager()
: m_receiver(NULL)
, m_bBosDetecting(false)
, m_bBosDetected(false)
, m_bRecording(false)
, m_bEmpty(true)
, m_bEnd(false)
, m_waitObj(NULL)
, m_syncObj(NULL)
{
    m_waitObj = VR_new CL_WaitObj;
    m_syncObj = VR_new CL_SyncObj;
}

// Destructor
VR_AudioBufferManager::~VR_AudioBufferManager()
{
    m_receiver = NULL;

    delete m_waitObj;
    m_waitObj = NULL;
    delete m_syncObj;
    m_syncObj = NULL;
}

void
VR_AudioBufferManager::OnAudioInData(void* data, int len)
{
    VR_LOGD_FUNC();

    if ((len <= 0) || (NULL == data)) {
        m_bEnd = true;
        return;
    }

    if (m_bBosDetecting) {
        VR_LOG("BOS detecting");
        char* buffer = VR_new char[len];
        if (NULL == buffer) {
            VR_ERROR("create buffer failed");
            return;
        }

        memcpy(buffer, data, len);
        if (NULL != m_syncObj) {
            CL_AutoSync autoSync(*m_syncObj);
            m_bufferList.push_back(
                std::pair<char*, size_t>(buffer, static_cast<size_t>(len))
            );
        }
        else {
            m_bufferList.push_back(
                std::pair<char*, size_t>(buffer, static_cast<size_t>(len))
            );
        }

        m_bEmpty = m_bufferList.empty();

        if (NULL != m_waitObj) {
            m_waitObj->Notify();
        }
    }
    else if (m_bAudioCaching) {
        char* buffer = VR_new char[len];
        if (NULL == buffer) {
            VR_ERROR("create buffer failed");
            return;
        }

        memcpy(buffer, data, len);

        m_bufferList.push_back(
            std::pair<char*, size_t>(buffer, static_cast<size_t>(len))
        );
    }
    else {
        VR_LOG("Recording for recognize");
        if (NULL == m_receiver) {
            VR_ERROR("Null Receiver");
            return;
        }

        if (m_bBosDetected) {
            VR_LOG("Writing the buffers to VBT engine");
            while (!m_parsedBufferList.empty()) {
                if (!m_bRecording) {
                    break;
                }
                void* pFirst = m_parsedBufferList.front().first;
                char* pBuffer = static_cast<char*>(pFirst);
                int size = m_parsedBufferList.front().second;
                m_receiver->OnAudioInData(pBuffer, size);
                delete[] pBuffer;
                m_parsedBufferList.pop_front();
            }
        }
        else {
            if (!m_bEmpty) {
                Clear();
            }
        }

        while (!m_bufferList.empty()) {
            if (!m_bRecording) {
                break;
            }
            void* pFirst = m_bufferList.front().first;
            char* pBuffer = static_cast<char*>(pFirst);
            int size = m_bufferList.front().second;
            m_receiver->OnAudioInData(pBuffer, size);
            delete[] pBuffer;
            m_bufferList.pop_front();
        }

        if (m_bRecording) {
            VR_LOG("Writing the audio data to VBT engine");
            m_receiver->OnAudioInData(data, len);
        }
    }
}

void
VR_AudioBufferManager::SetBosDetecting(bool detecting)
{
    VR_LOGD_FUNC();

    m_bBosDetecting = detecting;
    if (m_bBosDetecting) {
        m_bEnd = false;
    }
    else {
        m_bEnd = true;
        if (NULL != m_waitObj) {
            m_waitObj->Notify();
        }
    }
}

void
VR_AudioBufferManager::SetBosDetected(bool detected)
{
    VR_LOGD_FUNC();

    m_bBosDetected = detected;
}

bool
VR_AudioBufferManager::IsAvailable()
{
    VR_LOGD_FUNC();

    bool available = false;
    if (NULL != m_syncObj) {
        CL_AutoSync autoSync(*m_syncObj);
        available = !m_bufferList.empty();
    }
    else {
        available = !m_bufferList.empty();
    }

    return available;
}

int
VR_AudioBufferManager::FetchData(void** ppBuffer, size_t* size)
{
    VR_LOGD_FUNC();

    if (NULL != m_waitObj) {
        m_waitObj->Reset();
    }

    if (m_bEmpty) {
        VR_LOG("Empty flag: %d, End flag: %d", m_bEmpty, m_bEnd);
        while (m_bEmpty && !m_bEnd) {
            const int FETCHDATA_WAIT_TIME = 20;
            if (NULL != m_waitObj) {
                m_waitObj->Wait(FETCHDATA_WAIT_TIME);
            }
        }

        if (m_bEmpty) {
            *ppBuffer = NULL;
            *size = 0;
            return 0;
        }
    }

    {
        CL_AutoSync autoSync(*m_syncObj);
        *ppBuffer = m_bufferList.front().first;
        *size = m_bufferList.front().second;
        m_parsedBufferList.push_back(
            std::pair<char*, size_t>(
                m_bufferList.front().first,
                m_bufferList.front().second
            )
        );
        m_bufferList.pop_front();
        m_bEmpty = m_bufferList.empty();
    }

    VR_LOG("Get buffer size: %d", *size);

    return 0;
}

int
VR_AudioBufferManager::ReturnData(void* const& rpBuffer)
{
    VR_LOGD_FUNC();

    const int BUFFER_MAX_LEN = 10;
    while (m_parsedBufferList.size() > BUFFER_MAX_LEN) {
        delete[] m_parsedBufferList.front().first;
        m_parsedBufferList.pop_front();
    }

    return 0;
}

void
VR_AudioBufferManager::Clear()
{
    VR_LOGD_FUNC();

    while (!m_bufferList.empty()) {
        delete[] m_bufferList.front().first;
        m_bufferList.pop_front();
    }

    while (!m_parsedBufferList.empty()) {
        delete[] m_parsedBufferList.front().first;
        m_parsedBufferList.pop_front();
    }

    m_bEmpty = true;
}

void
VR_AudioBufferManager::SetReceiver(VR_AudioBuffer* receiver)
{
    VR_LOGD_FUNC();

    m_receiver = receiver;
}

void
VR_AudioBufferManager::SetRecordingFlag(bool bRecording)
{
    VR_LOGD_FUNC();

    m_bRecording = bRecording;
}

void
VR_AudioBufferManager::SetAudioCachingFlag(bool bCaching)
{
    m_bAudioCaching = bCaching;
}

/* EOF */