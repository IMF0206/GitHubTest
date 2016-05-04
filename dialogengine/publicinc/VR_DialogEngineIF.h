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
 * @file VR_DialogEngineIF.h
 * @brief interface for DialogEngineIF.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_DIALOGENGINEIF_H
#define VR_DIALOGENGINEIF_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

// class VR_DialogEngineListener;
#include "VR_DialogEngineListener.h"

/**
 * @brief The VR_Settings struct
 *
 * struct declaration
 */
struct VR_Settings
{
    std::string language;
    std::string country;
    bool naviExist;
};

/**
 * @brief The VR_DialogEngineIF class
 *
 * class declaration
 */
class VR_DialogEngineIF
{
public:
    virtual ~VR_DialogEngineIF() {}
    // whether be null engine
    virtual bool IsNullEngine()
    {
        return false; 
    }

    // sync
    virtual bool Initialize(VR_DialogEngineListener *listener, const VR_Settings &settings) = 0;

    // sync
    virtual std::string getHints(const std::string& hintsParams) = 0;

    // async
    virtual bool Start() = 0;

    // async
    virtual void Stop() = 0;

    // async
    virtual bool SendMessage(const std::string& message, int actionSeqId = -1) = 0;

    // sync
    virtual void UnInitialize() = 0;
};

#endif // VR_DIALOGENGINEIF_H
/* EOF */
