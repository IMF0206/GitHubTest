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

#include "VR_DataAccessorManagerCN.h"
#include "VR_Log.h"
VR_DataAccessorManagerCN::VR_DataAccessorManagerCN(VR_AsrRequestor* asrRequestor, VR_DECommonIF* common, VR_ConfigureIF* config)
    : VR_DataAccessorManager(asrRequestor, common, config)
{
}

void VR_DataAccessorManagerCN::setMusicGrammarActive(
    const std::string &grammarID,
    bool enable,
    int songCount,
    int otherCount)
{
    VR_LOGD_FUNC();
    VoiceList<std::string>::type ruleList;
    if ((songCount + otherCount) > VR_MAX_MUSIC_GRAMMAR_COUNT
        && songCount <= VR_MAX_MUSIC_GRAMMAR_COUNT) {
        ruleList.push_back("needToDisableTopmenuSong");
    }
    m_pDataSynchronizer->setGrammarActive("ctx_media_" + grammarID, enable, ruleList);
}

void VR_DataAccessorManagerCN::setPhoneContactGrammarActive(bool enable)
{
    VR_LOGD_FUNC();
    m_pDataSynchronizer->setGrammarActive("ctx_phone_1", enable);
}

/* EOF */
