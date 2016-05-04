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
 * @file VR_VoiceBoxAppsXml.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAPPSXML_H
#define VR_VOICEBOXAPPSXML_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

/* Suntec Header */
#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

/**
 * @brief The VR_VoiceBoxAppsXml class
 *
 * class declaration
 */
class VR_VoiceBoxAppsXml
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAppsXml();

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAppsXml();

    // Extract the VBT XML from the message that send from TSL
    bool GetVBTXml(const std::string& xml,
        VoiceVector<std::string>::type& messages
    );

    // Get the action type of the message
    std::string GetVBTActionType(const std::string& message);

    std::string GetVBTCommand(const std::string& message);

    std::string GetMessageClass(const std::string& message);

    std::string GetVBTActionParamValue(
        const std::string& message,
        const std::string& paramName
    );
};

#endif // VR_VOICEBOXAPPSXML_H
/* EOF */