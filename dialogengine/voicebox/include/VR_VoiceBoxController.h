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
 * @file VR_VoiceBoxController.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCONTROLLER_H
#define VR_VOICEBOXCONTROLLER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/**
 * @brief The VR_VoiceBoxController class
 *
 * class declaration
 */
class VR_VoiceBoxController
{
public:
    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxController() {}

    /**
     * Post Message
     *
     * Post message to the Controller
     *
     * @param message [IN] : XML format message
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool PostMessage(const std::string& message, int actionSeqId = -1) = 0;

    virtual void ClearMessage() = 0;

    virtual void ProcessOpMessage(const std::string& strOpMsg) = 0;

    virtual std::string GetCurrentMessage() = 0;

};

#endif // VR_VOICEBOXCONTROLLER_H
/* EOF */