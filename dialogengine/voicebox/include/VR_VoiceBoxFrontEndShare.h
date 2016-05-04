/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_VoiceBoxFrontEndShare.h
 * @brief Declaration file of VR_VoiceBoxFrontEndShare.
 *
 * This file includes the declaration of VR_VoiceBoxFrontEndShare.
 *
 * @attention used on *.cpp only.
 */

#ifndef VR_VOICEBOXFRONTENDSHARE_H
#define VR_VOICEBOXFRONTENDSHARE_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include "VBTOSSupportLite.h"
#include "AsrVocon4x_FrontEndShare.h"

/* Suntec Header */


/**
 * @brief The VR_VoiceBoxFrontEndShare class
 *
 * class declaration
 */
class VR_VoiceBoxFrontEndShare : public IVBTVocon4FrontEndShare
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxFrontEndShare();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxFrontEndShare();

    // release this instance of the interface
    virtual HRESULT ReleaseFrontEndShare();

    // tell the listener that it can create a Vocon4x front end
    virtual HRESULT ShareFrontEnd(const LH_COMPONENT * phAsr,
                                  const LH_OBJECT    * phAcMod);

    // tell the listener to destroy its instance of the Vocon4x
    virtual HRESULT ReleaseFrontEnd();
};

#endif /* VR_VOICEBOXFRONTENDSHARE_H */
/* EOF */