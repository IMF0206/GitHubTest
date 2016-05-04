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
 * @file VR_AsrRequestor.h
 * @brief inteface for interpeter or asr engine to perfer command
 *
 *
 * @attention used for C++ only.
 */

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __VR_ASRREQUESTOR_H__
#define __VR_ASRREQUESTOR_H__

#include <string>


/**
 * @brief The VR_AsrRequestor class
 *
 * update grammar into asr
 */

class VR_AsrRequestor
{
public:
    virtual ~VR_AsrRequestor() {}
    virtual bool updateGrammar(N_Vr::N_Asr::C_Request_Context_List_Update& terms) = 0;
    virtual bool genVoiceTagPhoneme() = 0;
    virtual void setGrammarActive(const std::string &contextID, bool isActive, const VoiceList<std::string>::type &ruleIDList) = 0;
    virtual void updateGrammarCategoryFinish(const std::string &category) = 0;
    virtual void updateGrammarFinish() = 0;
};

#endif
/* EOF */
