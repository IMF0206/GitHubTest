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
 * @file VR_VoiceBoxLog.h
 * @brief Declaration file of log macro.
 *
 * This file includes the declaration of log macro.
 *
 * @attention used on *.cpp only.
 */

#ifndef VR_VOICEBOXLOG_H
#define VR_VOICEBOXLOG_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <stdio.h>

#define VR_VBT_ERROR(...)                   printf(__VA_ARGS__);\
                                            printf("\n")
#define VR_VBT_LOG(...)                     printf(__VA_ARGS__);\
                                            printf("\n")

#endif /* VR_VOICEBOXLOG_H */
/* EOF */
