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
 * @file VR_DataAccessorVoiceTag.h
 * @brief Declaration file of VR_DataAccessorVoiceTag.
 *
 * This file includes the declaration of VR_DataAccessorVoiceTag.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_VOICETAG_H
#define VR_DATA_ACCESSOR_VOICETAG_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessor.h"
#include "MEM_map.h"

#include <pugixml.hpp>

#define VR_OPERATION_REQ_VOICETAG        "_reqVoiceTag"
#define VR_OPERATION_REQ_VOICETAG_DETAIL "_reqVoiceTagDetail"
#define VR_OPERATION_GET_VOICETAG_NUMBER "_getVoiceTagNumber"

class VR_VoiceTagQuerier;

/**
 * @brief The VR_DataAccessorVoiceTag class
 *
 * provide interface for query VoiceTag
 */

class VR_DataAccessorVoiceTag : public VR_DataAccessor
{
public:
    explicit VR_DataAccessorVoiceTag(VR_VoiceTagQuerier *querier);
    virtual ~VR_DataAccessorVoiceTag() {}

    bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) override;
    bool isOperationHandled(const std::string &operation) override;

// protected:
//     void requestService(const std::string &agent, const std::string &reqName) override;

private:
    enum class VoiceTagOperation
    {
        None,
        ReqVoiceTag,
        ReqVoiceTagDetail,
        GetVoiceTagNumber
    };

    VoiceMap<std::string, VoiceTagOperation>::type m_operationMap;
    VR_VoiceTagQuerier * m_querier;
    pugi::xml_document m_voiceTagInfoSampleDoc;

    bool reqVoiceTag(const std::string &reqMsg, std::string &response);
    bool reqVoiceTagDetail(const std::string &reqMsg, std::string &response);
    bool getVoiceTagNumber(std::string &response);
};

#endif /* VR_DATA_ACCESSOR_VOICETAG_H */
/* EOF */