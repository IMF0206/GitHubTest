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

/* VBT Header */
#ifndef VBTVAALINT_H
#    include "VBTVaalInt.h"
#endif

/* Suntec Header */
#ifndef VR_VAALSTREAMWRAP_H
#    include "VR_VaalStreamWrap.h"
#endif

#ifndef VR_VAALSTREAM_H
#    include "VR_VaalStream.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

// Constructor
VR_VaalStreamWrap::VR_VaalStreamWrap(
    VR_VoiceBoxAudioIn& audioIn,
    VR_AudioBufferManager& bufferManager
)
: m_vaalStream(NULL)
{
    m_vaalStream = VR_new VR_VaalStream(audioIn, bufferManager);
}

// Destructor
VR_VaalStreamWrap::~VR_VaalStreamWrap()
{
    delete m_vaalStream;
    m_vaalStream = NULL;
}

VR_VaalStream*
VR_VaalStreamWrap::GetVaalStream()
{
    return m_vaalStream;
}

/* EOF */
