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
#include "VBTExternalErrors.h"

/* Suntec Header */
#ifndef VR_VOICEBOXFRONTENDSHARE_H
#    include "VR_VoiceBoxFrontEndShare.h"
#endif

#ifndef VR_AUDIOSTREAMIF_H
#    include "VR_AudioStreamIF.h"
#endif

// Constructor
VR_VoiceBoxFrontEndShare::VR_VoiceBoxFrontEndShare()
{
}

// Destructor
VR_VoiceBoxFrontEndShare::~VR_VoiceBoxFrontEndShare()
{
}

HRESULT
VR_VoiceBoxFrontEndShare::ShareFrontEnd(const LH_COMPONENT *phAsr, const LH_OBJECT *phAcMod)
{
    if ((NULL == phAsr) || (NULL == phAcMod)) {
        return E_FAIL;
    }

    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL == pcAudioStream) {
        return E_FAIL;
    }

    bool bRet = pcAudioStream->ShareFrontEnd(
        const_cast<LH_COMPONENT *>(phAsr),
        const_cast<LH_OBJECT *>(phAcMod)
    );

    if (!bRet) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT
VR_VoiceBoxFrontEndShare::ReleaseFrontEnd()
{
    VR_AudioStreamIF* pcAudioStream = VR_AudioStreamIF::Instance();
    if (NULL == pcAudioStream) {
        return E_FAIL;
    }

    if (!pcAudioStream->ReleaseFrontEnd()) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT
VR_VoiceBoxFrontEndShare::ReleaseFrontEndShare()
{
	// Do nothing
    return S_OK;
}

/* EOF */
