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
 * @file VR_DataAccessor.h
 * @brief Declaration file of VR_DataAccessor.
 *
 * This file includes the declaration of VR_DataAccessor.
 *
 * @attention used for C++ only.
 */

#ifndef VR_DATA_ACCESSOR_H
#define VR_DATA_ACCESSOR_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "MEM_map.h"
#include <string>

/**
 * @brief The VR_DataAccessor class
 *
 * interface for DataAccessor
 */

class VR_DataAccessor
{
public:
    VR_DataAccessor() {}
    virtual ~VR_DataAccessor() {}

    virtual bool getInfo(const std::string &operation, const std::string &reqMsg, std::string &response) = 0;
    virtual bool isOperationHandled(const std::string &operation) = 0;

    static std::string getPhoneTypeName(const std::string &phoneTypeID);
    static void setPhoneTypeName(int typeID, const std::string &typeName);

// protected:
//    virtual void requestService(const std::string &agent, const std::string &reqName) = 0;

private:
    static VoiceMap<int, std::string>::type m_phoneTypeNameMap;
};


#endif /* VR_DATA_ACCESSOR_H */
/* EOF */
