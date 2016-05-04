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
 * @file VR_VoiceBoxXmlNodeString.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXVOICETAG_H
#define VR_VOICEBOXVOICETAG_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <sstream>

/* Suntec Header */
#ifndef MEM_MAP_H
#    include "MEM_list.h"
#endif

struct VoiceTagInfo
{
    std::string voiceTagID;
    std::string pcmPath;
    std::string phoneme;
    std::string contactMsg;
};

/**
 * @brief The VR_VoiceBoxVoiceTag class
 *
 * class declaration
 */
class VR_VoiceBoxVoiceTag
{
public:
    VR_VoiceBoxVoiceTag();
    // message
    virtual ~VR_VoiceBoxVoiceTag();

    void CheckTempVoiceTag(const std::string &deviceAddress);

    bool SaveVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress, VoiceTagInfo voiceTagPara, const std::string tempData, bool update = false);

    /**
     * DeleteAllVoiceTag
     *
     * DeleteAllVoiceTag
     *
     * @param deviceAddress
     * @return void
     * @attention none
     * @see none
     */
    bool DeleteAllVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress);

    bool RemoveAllRegisteredVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress);

    /**
     * DeleteVoiceTag
     *
     * DeleteVoiceTag
     *
     * @param deviceAddress
     * @param voiceTagIDList
     * @return void
     * @attention none
     * @see none
     */
    bool DeleteVoiceTag(std::string &strAddGrammar, const std::string &deviceAddress, const VoiceList<std::string>::type &voiceTagIDList);

    /**
     * GetVoiceTagPCMPath
     *
     * GetVoiceTagPCMPath
     *
     * @param voiceTagID
     * @return std::string
     * @attention none
     * @see none
     */
    std::string GetVoiceTagPCMPath(const std::string &voiceTagID, const std::string &deviceAddress); // play voice tag
};

#endif // VR_VOICEBOXVOICETAG_H
/* EOF */
