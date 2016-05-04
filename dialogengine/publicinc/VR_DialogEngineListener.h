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
 * @file VR_DialogEngineListener.h
 * @brief interface for DialogEngineIF's callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_DIALOG_ENGINE_LISTENER_H
#define VR_DIALOG_ENGINE_LISTENER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <string>

/**
 * @brief The VR_DialogEngineListener class
 *
 * class declaration
 */
class VR_DialogEngineListener
{
public:
    virtual ~VR_DialogEngineListener() {}
    virtual void OnRequestAction(const std::string& message, int seqId) = 0;
    virtual void OnStarted() = 0;
    virtual void OnStopped() = 0;
};

#endif  // VR_DIALOG_ENGINE_LISTENER_H
/* EOF */
