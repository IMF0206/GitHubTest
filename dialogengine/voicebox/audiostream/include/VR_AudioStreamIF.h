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
 * @file VR_AudioStreamIF.h
 * @brief interface for VR_AudioStreamIF.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_AUDIOSTREAMIF_H
#define VR_AUDIOSTREAMIF_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Forward Declaration */
class VR_AudioStreamListener;
class VR_VaalStream;

/**
 * @brief The VR_AudioBuffer class
 *
 * class declaration
 */
class VR_AudioStreamIF
{
public:
    ~VR_AudioStreamIF() {}
    static VR_AudioStreamIF* Instance();

    virtual bool Initialize(VR_AudioStreamListener* listener) = 0;
    virtual void UnInitialize() = 0;
    virtual bool ShareFrontEnd(void *handleAsr, void *handleAcMod) = 0;
    virtual bool ReleaseFrontEnd() = 0;
    virtual bool StartAudioInWithBargeIn() = 0;
    virtual bool StartAudioInWithCache() = 0;
    virtual bool StartAudioIn() = 0;
    virtual bool StopAudioIn() = 0;
    virtual unsigned int GetAecAudioTypeDataSize() = 0;
    virtual bool GetAecAudioTypeData(unsigned int& size, void* data) = 0;
    virtual VR_VaalStream* GetVaalStream() = 0;
    virtual void SetVoiceTag(bool bOn) = 0;
    virtual std::string GetTempPcmFile() = 0;
};

#endif // VR_AUDIOSTREAMIF_H
/* EOF */
