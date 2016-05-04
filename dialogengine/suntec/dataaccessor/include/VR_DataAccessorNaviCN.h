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
 * @file VR_DataAccessorNaviCN.h
 * @brief Declaration file of VR_DataAccessorNaviCN.
 *
 * This file includes the declaration of VR_DataAccessorNaviCN.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_NAVI_CN_H
#define VR_DATA_ACCESSOR_NAVI_CN_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessorNaviIF.h"

enum class VR_MappingTypeCN
{
    FULL,
    TILL_STREET,
    TILL_DISTRICT,
    TILL_CITY,
    TILL_STATE,
    EMPTY
};

/**
 * @brief The VR_DataAccessorNaviCN class
 *
 * provide interface for query navi
 */

class VR_DataAccessorNaviCN : public VR_DataAccessorNaviIF
{
public:
    VR_DataAccessorNaviCN();
    virtual ~VR_DataAccessorNaviCN();

    bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) override;
    bool isOperationHandled(const std::string &operation) override;
    void setCountryID(const std::string &countryID) override {}

protected:
    // void requestService(const std::string &agent, const std::string &reqName) override;

    bool addressMapping(const std::string &reqMsg, std::string &response);
    bool buildAddress(const std::string &reqMsg, std::string &response);
    int getAddressMappingResult(pugi::xml_node &firstItem, pugi::xml_node &itemsNode, std::string &mappingType);

    using VR_DataAccessorNaviIF::buildPointType;

private:
    enum class NaviOperation
    {
        None,
        AddressMapping,
        BuildAddress,
        BuildPointType
    };

    VoiceMap<std::string, NaviOperation>::type m_operationMap;
};

#endif /* VR_DATA_ACCESSOR_NAVI_CN_H */
/* EOF */