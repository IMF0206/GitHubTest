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
 * @file VR_DialogEngineNull.h
 * @brief interface for DialogEngineNull.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_DIALOGENGINENULL_H
#define VR_DIALOGENGINENULL_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_DialogEngineIF.h"

/**
 * @brief The VR_DialogEngineNull class
 *
 * class declaration
 */
class VR_DialogEngineNull : public VR_DialogEngineIF
{
public:
    virtual ~VR_DialogEngineNull() 
    {
    }

    virtual bool IsNullEngine()
    {
        return true; 
    }

    virtual bool Initialize(VR_DialogEngineListener *listener, const VR_Settings &settings)
    {
        return false;
    }

    virtual std::string getHints(const std::string& hintsParams)
    {
        return "NULL dialog engine has no hints";
    }

    virtual bool Start()
    {
        return false;
    }

    virtual void Stop()
    {
        return;
    }

    virtual bool SendMessage(const std::string& message, int actionSeqId = -1)
    {
        return false;
    }

    virtual void UnInitialize()
    {   
        return;
    }
};

#endif // VR_DIALOGENGINENULL_H
/* EOF */
