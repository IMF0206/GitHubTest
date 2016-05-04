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

/**
 * @file VR_AudioBufferManager.h
 * @brief interface for VR_AudioBufferManager.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_AUDIOBUFFERMANAGER_H
#define VR_AUDIOBUFFERMANAGER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <utility>

/* Suntec Header */
#include "MEM_list.h"

/* Forward Declaration */
class VR_AudioBuffer;
class CL_WaitObj;
class CL_SyncObj;

/**
 * @brief The VR_AudioBufferManager class
 *
 * class declaration
 */
class VR_AudioBufferManager
{
public:
    /* Constructor */
    VR_AudioBufferManager();

    /* Destructor */
    ~VR_AudioBufferManager();

    void OnAudioInData(void* data, int len);

    bool IsAvailable();
    int FetchData(void** ppBuffer, size_t* size);
    int ReturnData(void* const& rpBuffer);
    void Clear();

    void SetReceiver(VR_AudioBuffer* receiver);
    void SetBosDetecting(bool detecting);
    void SetBosDetected(bool detected);
    void SetRecordingFlag(bool bRecording);
    void SetAudioCachingFlag(bool bCaching);

private:
    VR_AudioBufferManager(const VR_AudioBufferManager& x);
    VR_AudioBufferManager& operator=(const VR_AudioBufferManager& x);

private:
    VoiceList<std::pair<char*, size_t> >::type m_bufferList;
    VoiceList<std::pair<char*, size_t> >::type m_parsedBufferList;
    VR_AudioBuffer* m_receiver; // Used for sending data to VBT engine
    bool m_bBosDetecting;
    bool m_bBosDetected;
    bool m_bRecording;
    bool m_bAudioCaching;
    volatile bool m_bEmpty;
    volatile bool m_bEnd;
    CL_WaitObj* m_waitObj;
    CL_SyncObj* m_syncObj;
};

#endif // VR_AUDIOBUFFERMANAGER_H
/* EOF */
