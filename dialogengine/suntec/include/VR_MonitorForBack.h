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
 * @file VR_DEMonitorIF.h
 * @brief dialog engine class interface
 *
 *
 * @attention used for C++ only.


 */
#ifndef VR_DEMONITORIF_H
#define VR_DEMONITORIF_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>


/**
 * @brief The VR_DialogEngineIFM class
 *
 * dialog engine class
 */
class VR_MonitorForBack
{
public:
    virtual ~VR_MonitorForBack() {}
    virtual void pushTmpToStack() = 0;
    virtual void saveTmpState(const std::string& stateName,  const std::string& stateAttr)= 0;
    virtual std::string getTmpAttr() = 0;
    virtual void saveCurrentState(const std::string& stateName,  const std::string& stateAttr) = 0;
    virtual void clearSameAgentState() = 0;
    virtual void clearTmpState() = 0;
};

#endif // VR_DEMONITORIF_H
/* EOF */
