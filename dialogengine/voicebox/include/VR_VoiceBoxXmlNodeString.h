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
 * @file VR_VoiceBoxXmlNodeString.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXXMLNODESTRING_H
#define VR_VOICEBOXXMLNODESTRING_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>

// the parameter of node
extern const std::string phoneInfo[];
extern const std::string itemDisplay[];
extern const std::string cityItem[];
extern const std::string messageList[];
extern const std::string musicInfo[];
extern const std::string radioInfo[];
extern const std::string messageInfo[];
extern const std::string actionListMessages[];
extern const std::string quickReplytoCurrentMessage[];
extern const std::string phoneTypeInfo[];
extern const std::string callPhoneInfo[];
extern const std::string nameMatchInfo[];
extern const std::string confirmCallInfo[];
extern const std::string listRecentCalls[];
extern const std::string nBestName[];
extern const std::string nBestNumber[];
extern const std::string nBestDigits[];
extern const std::string addressInfo[];
extern const std::string hint[];

/**
 * @brief The VR_VoiceBoxXmlNodeString class
 *
 * class declaration
 */
class VR_VoiceBoxXmlNodeString
{
public:
    VR_VoiceBoxXmlNodeString();
    // message
    virtual ~VR_VoiceBoxXmlNodeString();
};

#endif // VR_VOICEBOXXMLNODESTRING_H
/* EOF */
