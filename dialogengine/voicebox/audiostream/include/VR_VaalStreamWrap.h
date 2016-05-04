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
 * @file VR_VaalStreamWrap.h
 * @brief interface for VR_VaalStreamWrap.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VAALSTREAMWRAP_H
#define VR_VAALSTREAMWRAP_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Forward Declaration */
class VR_VoiceBoxAudioIn;
class VR_AudioBufferManager;
class VR_VaalStream;

/**
 * @brief The VR_VaalStreamWrap class
 *
 * class declaration
 */
class VR_VaalStreamWrap
{
public:
    // Constructor
    VR_VaalStreamWrap(
        VR_VoiceBoxAudioIn& audioIn,
        VR_AudioBufferManager& bufferManager
    );

    // Destructor
    ~VR_VaalStreamWrap();

    // Get vaal stream instance and transfer it to VBT.
    VR_VaalStream* GetVaalStream();
private:
    VR_VaalStreamWrap(const VR_VaalStreamWrap& x);
    VR_VaalStreamWrap& operator=(const VR_VaalStreamWrap& x);

private:
    VR_VaalStream* m_vaalStream;
};

#endif // VR_VAALSTREAMWRAP_H
/* EOF */
