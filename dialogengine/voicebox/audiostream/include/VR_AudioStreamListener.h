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
 * @file VR_AudioStreamListener.h
 * @brief interface for VR_AudioStreamListener.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_AUDIOSTREAMLISTENER_H
#define VR_AUDIOSTREAMLISTENER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/**
 * @brief The VR_AudioStreamListener class
 *
 * class declaration
 */
class VR_AudioStreamListener
{
public:
    virtual ~VR_AudioStreamListener() {}
    virtual void OnBosDetected() = 0;
};

#endif // VR_AUDIOSTREAMLISTENER_H
/* EOF */
