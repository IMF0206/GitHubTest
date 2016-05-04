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
 * @file VR_DataAccessorNavi.h
 * @brief Declaration file of VR_DataAccessorNavi.
 *
 * This file includes the declaration of VR_DataAccessorNavi.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_NAVI_H
#define VR_DATA_ACCESSOR_NAVI_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessorNaviIF.h"
#include "VR_NaviAddressCommon.h"

class VR_ConfigureIF;

/**
 * @brief The VR_DataAccessorNavi class
 *
 * provide interface for query navi
 */

class VR_DataAccessorNavi : public VR_DataAccessorNaviIF, public VR_NaviAddressCommon
{
public:
    VR_DataAccessorNavi(VR_ConfigureIF* config);
    virtual ~VR_DataAccessorNavi();

    bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) override;
    bool isOperationHandled(const std::string &operation) override;
    void setCountryID(const std::string &countryID) override;

protected:
    // void requestService(const std::string &agent, const std::string &reqName) override;

    // sendVoiceID return voiceID according to addressType and addressInfo
    // responseMsg like: <data><type>STATE</type><voiceId>12345</voiceId></data>
    bool sendVoiceID(const std::string &reqMsg, std::string &response);

    using VR_NaviAddressCommon::addressMapping;
    using VR_NaviAddressCommon::buildAddress;
    using VR_NaviAddressCommon::buildAddressName;
    using VR_NaviAddressCommon::getPOIPrompt;
    using VR_DataAccessorNaviIF::buildPointType;

private:
    enum class NaviOperation
    {
        None,
        AddressMapping,
        BuildAddress,
        SendVoiceID,
        BuildAddressName,
        GetPOIPrompt,
        BuildPointType
    };

    VoiceMap<std::string, NaviOperation>::type m_operationMap;

    void processRequestMsg(const std::string &inputMsg, std::string &outputMsg);
};

#endif /* VR_DATA_ACCESSOR_NAVI_H */
/* EOF */