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
 * @file VR_VaalStream.h
 * @brief interface for VR_VaalStream.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VAALSTREAM_H
#define VR_VAALSTREAM_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* VBT Header */
#ifndef VBTVAALSTREAM_H
#    include "VBTVaalStream.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAudioIn;
class VR_AudioBufferManager;
class VR_AudioBuffer;

/**
 * @brief The VR_VaalStream class
 *
 * class declaration
 */
class VR_VaalStream : public IVBTVaalStream
{
public:
    // Constructor
    VR_VaalStream(
        VR_VoiceBoxAudioIn& audioIn,
        VR_AudioBufferManager& bufferManager
    );

    // Destructor
    ~VR_VaalStream();

    // @brief     Initialize the audio stream
    // @param[in] pParams
    // @param     pBuffer
    virtual HRESULT Init(VAAL_PARAMS *pParams, IVBTVaalBuffer *pBuffer);

    // @brief     Uninitialize the audio stream
    // @return    HRESULT - S_FALSE - already uninitialized
    //
    // Does not need to be called directly, will be called by the destructor
    virtual HRESULT Terminate();

    // @brief     Opens the audio stream
    // @return    HRESULT - S_FALSE - already open
    virtual HRESULT Open();

    // @brief     Close the audio stream
    // @return    HRESULT - S_FALSE - wasn't previously open
    virtual HRESULT Close();

    // @brief      Prepare to start recording or playback
    // @return     HRESULT used to indicate status
    virtual HRESULT Reset();

    // @brief      Start recording or playback
    // @return     HRESULT used to indicate status
    virtual HRESULT Start();

    // @brief     Stop recording or playback
    // @param[in] bAbort If true will stop immediately, if false will
    //                   stop after pending frames.
    // @return     HRESULT used to indicate status
    virtual HRESULT Stop(bool bAbort = true);

    // @brief      Used to determine whether it is actively in recording/playback
    // @param[out] bIsActive Returns true if it is active
    // @return     HRESULT used to indicate status
    virtual HRESULT IsStreamActive(bool& bIsActive);

    // @brief     Query for supported audio stream properties
    // @warning   Not implemented
    virtual HRESULT QuerySupport(unsigned long *pulSupportMask);

    // @brief     Set audio stream property
    // @warning   Not implemented
    virtual HRESULT SetProperty(VAAL_PROPTYPE type, VAAL_PROPVAL val);

    // @brief     Get audio stream property
    // @warning   Not implemented
    virtual HRESULT GetProperty(VAAL_PROPTYPE type, VAAL_PROPVAL *pVal);

    // @brief     Register statistics callback
    // @warning   Not implemented
    virtual HRESULT RegisterStatCallback(VBTVaalStatCallback pCallback, void * pUserData);

private:
    VR_VoiceBoxAudioIn&      m_audioIn;
    VR_AudioBufferManager&   m_bufferManager;
    VR_AudioBuffer*          m_audioBuffer;
    VAAL_PARAMS              m_params;
    bool                     m_bStreamActive;
};

#endif // VR_VAALSTREAM_H
/* EOF */
