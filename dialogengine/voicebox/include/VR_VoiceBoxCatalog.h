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
 * @file VR_VoiceBoxCatalog.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCATALOG_H
#define VR_VOICEBOXCATALOG_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Forward Declaration */
class VR_VoiceBoxXmlParser;

/**
 * @brief The VR_VoiceBoxCatalog class
 *
 * class declaration
 */
class VR_VoiceBoxCatalog
{
public:
    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxCatalog() {}

    /**
     * Initialize
     *
     * Initialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize() = 0;

};

#endif // VR_VOICEBOXCATALOG_H
/* EOF */