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
 * @file VR_AudioBuffer.h
 * @brief interface for VR_AudioBuffer.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_AUDIOBUFFER_H
#define VR_AUDIOBUFFER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/* Forward Declaration */
struct VAAL_PARAMS;
class IVBTVaalBuffer;

/**
 * @brief The VR_AudioBuffer class
 *
 * class declaration
 */
class VR_AudioBuffer
{
public:
    VR_AudioBuffer(VAAL_PARAMS *pParams, IVBTVaalBuffer *buffer);
    ~VR_AudioBuffer();

    void OnAudioInData(void* data, int len);


private:
    IVBTVaalBuffer *m_pVaalBuffer;
    int             m_nBufSizeInBytes;
    int             m_nFrameSizeInBytes;
};

#endif // VR_AUDIOBUFFER_H
/* EOF */
