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
 * @file VR_DataAccessorNaviIF.h
 * @brief Declaration file of VR_DataAccessorNaviIF.
 *
 * This file includes the declaration of VR_DataAccessorNaviIF.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_IF_H
#define VR_DATA_ACCESSOR_IF_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DataAccessor.h"
#include <pugixml.hpp>
#include <string>

extern const char* VR_OPERATION_BUILD_POINT_TYPE;

/**
 * @brief The VR_DataAccessorNaviIF class
 *
 * interface for DataAccessorNavi
 */

class VR_DataAccessorNaviIF : public VR_DataAccessor
{
public:
    VR_DataAccessorNaviIF() {}
    virtual ~VR_DataAccessorNaviIF() {}

    virtual void setCountryID(const std::string &countryID) = 0;
    virtual bool buildPointType(const std::string &reqMsg, std::string &response);
protected:
    virtual bool isGuidePointValid(pugi::xml_node &guidePointNode);
    virtual void changeValueByResult(pugi::xml_node &originNode, pugi::xml_node &resultNode);
};


#endif /* VR_DATA_ACCESSOR_H */
/* EOF */
