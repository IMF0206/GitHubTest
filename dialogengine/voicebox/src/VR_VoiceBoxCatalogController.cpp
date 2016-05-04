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

/* Standard Header */

/* VBT Header */

/* Suntec Header */
#include "VR_Log.h"
#ifndef VR_VOICEBOXCATALOGCONTROLLER_H
#    include "VR_VoiceBoxCatalogController.h"
#endif

#ifndef VR_VOICEBOXCATALOGMANAGER_H
#    include "VR_VoiceBoxCatalogManager.h"
#endif

#ifndef VR_VOICEBOXENGINEIF_H
#    include "VR_VoiceBoxEngineIF.h"
#endif

#ifndef CXX_BL_AUTOSYNC_H
#   include "BL_AutoSync.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

BL_SyncObject   VR_VoiceBoxCatalogController::s_cSync;   ///< Sync object

// Constructor
VR_VoiceBoxCatalogController::VR_VoiceBoxCatalogController()
: m_msglist()
, m_voiceBoxEngine(NULL)
, m_pcCatalogManager(NULL)
, m_bUpdateGrammar(true)
, m_index(0)
, m_totalindex(0)
, m_bNeedPauseGrammar(false)
, m_bNeedResumeGrammar(false)
{
}

// Destructor
VR_VoiceBoxCatalogController::~VR_VoiceBoxCatalogController()
{
    m_voiceBoxEngine = NULL;
    m_pcCatalogManager = NULL;
}

// Initialize the Message Controller
bool
VR_VoiceBoxCatalogController::Initialize(VR_VoiceBoxEngineIF* pcEngine)
{
    if (NULL == pcEngine) {
        return false;
    }

    m_voiceBoxEngine = pcEngine;
    m_pcCatalogManager = m_voiceBoxEngine->GetCatalogManager();
    RegisterName("VR_VBT_AGENT_DATA_THREAD");
    return true;
}

// Start the Message Controller thread
void
VR_VoiceBoxCatalogController::Start()
{
    StartRegistThread();
}

// Stop the Message Controller thread
void
VR_VoiceBoxCatalogController::Stop()
{
    StopThread();
}

// Post message to the Message Controller
bool
VR_VoiceBoxCatalogController::PostMessage(const std::string& message, int actionSeqId)
{
    VR_LOGD_FUNC();
    // If the m_pcCatalogManager is not exist,
    // we could not process the message.
    if (NULL == m_pcCatalogManager) {
        return false;
    }

    m_totalindex++;

    VR_LOG("m_totalindex = %d, message = %s", m_totalindex, message.c_str());

    // Add new messages to the list
    {
        BL_AutoSync cAutoSync(s_cSync);
        if (std::string::npos != message.find("<category name=\"audiosource\"")) {
            m_strAudioSource = message;
        }
        else {
            m_msglist.push_back(message);
        }
    }

    Notify();

    return true;
}

// Process the messages
void
VR_VoiceBoxCatalogController::Run()
{
    while (true) {
        if (m_bNeedPauseGrammar) {
            if (NULL != m_pcCatalogManager) {
                std::pair<std::string, std::string> pairAgent2TransId = m_pcCatalogManager->GetCurrentTransaction();
                if (!pairAgent2TransId.first.empty()) {
                    m_bNeedPauseGrammar = false;
                    m_pcCatalogManager->PauseGrammarUpdate(pairAgent2TransId.first);
                }
            }
        }

        if (m_bNeedResumeGrammar) {
            if (NULL != m_pcCatalogManager) {
                std::pair<std::string, std::string> pairAgent2TransId = m_pcCatalogManager->GetCurrentTransaction();
                if (!pairAgent2TransId.first.empty()) {
                    m_bNeedResumeGrammar = false;
                    m_pcCatalogManager->ResumeGrammarUpdate(pairAgent2TransId.first);
                }
            }
        }
        if (m_strAudioSource.empty() && m_strUpdateAgainMsg.empty() && m_msglist.empty()) {
            Wait();
        }

        if (TRUE == CheckQuit()) {
            break;
        }

        if (m_bUpdateGrammar) {
            VR_LOG("m_bUpdateGrammar is true");
            if (!m_strAudioSource.empty()) {
                m_bUpdateGrammar = false;
                BL_AutoSync cAutoSync(s_cSync);
                m_strCurrentMsg = m_strAudioSource;
                m_index++;
                VR_LOG("m_index = %d begin, m_strCurrentMsg = %s", m_index, m_strAudioSource.c_str());
                m_pcCatalogManager->ProcessMessage(m_strAudioSource);
                VR_LOG("m_index = %d end, m_strCurrentMsg = %s", m_index, m_strAudioSource.c_str());
                m_strAudioSource.clear();
            }
            else if (!m_strUpdateAgainMsg.empty()) {
                m_bUpdateGrammar = false;
                m_strCurrentMsg = m_strUpdateAgainMsg;
                m_index++;
                VR_LOG("m_index = %d begin, m_strCurrentMsg = %s", m_index, m_strUpdateAgainMsg.c_str());
                m_pcCatalogManager->ProcessMessage(m_strUpdateAgainMsg);
                VR_LOG("m_index = %d end, m_strCurrentMsg = %s", m_index, m_strUpdateAgainMsg.c_str());
                m_strUpdateAgainMsg.clear();
                m_index++;
            }
            else {
                while (!m_msglist.empty() && m_bUpdateGrammar) {
                    if (TRUE == CheckQuit()) {
                        break;
                    }

                    {
                        BL_AutoSync cAutoSync(s_cSync);
                        if (!m_msglist.empty()) {
                            m_strCurrentMsg = m_msglist.front();
                            m_msglist.pop_front();
                        }
                        else {
                            continue;
                        }
                    }
                    m_bUpdateGrammar = false;
                    m_index++;
                    VR_LOG("m_index = %d begin, m_strCurrentMsg = %s", m_index, m_strCurrentMsg.c_str());

                    m_pcCatalogManager->ProcessMessage(m_strCurrentMsg);

                    VR_LOG("m_index = %d end, m_strCurrentMsg = %s", m_index, m_strCurrentMsg.c_str());
                }
            }
        }
        else {
            Wait();
        }
    }
}

void
VR_VoiceBoxCatalogController::ClearMessage()
{
    BL_AutoSync cAutoSync(s_cSync);
    m_msglist.clear();
}

void
VR_VoiceBoxCatalogController::DeletePhoneGrammarBuf()
{
    VR_LOGD_FUNC();

    BL_AutoSync cAutoSync(s_cSync);

    VoiceList<std::string>::iterator iter =  m_msglist.begin();
    for (; m_msglist.end() != iter;) {
        std::string strTmp = *iter;
        if ((std::string::npos != strTmp.find("quickreplymessage")
            || std::string::npos != strTmp.find("phonetype")
            || std::string::npos != strTmp.find("messagetype"))
            || std::string::npos != strTmp.find("<grammar_active agent=\"phone\"")
            || std::string::npos != strTmp.find("<grammar_disactive agent=\"phone\"")
            || std::string::npos != strTmp.find("<grammar_init agent=\"phone\"")) {
            VR_LOG("delete phone grammar msg = %s", strTmp.c_str());
            m_pcCatalogManager->SendPhoneGrammarResult(strTmp);
            iter = m_msglist.erase(iter);
            --m_totalindex;
        }
        else {
            ++iter;
        }
    }

    VR_LOG("m_totalindex = %d", m_totalindex);
}

void
VR_VoiceBoxCatalogController::ProcessOpMessage(const std::string& strOpMsg)
{
    VR_VoiceBoxXmlParser parser(strOpMsg);
    std::string strXmlKey = parser.getXmlKey();
    std::string strValue = parser.getValueByKey("value");
    bool bFlg = ("true" == strValue) ? true : false;
    if ("needpause" == strXmlKey) {
        m_bNeedPauseGrammar = bFlg;
    }
    else if ("needresume" == strXmlKey) {
        m_bNeedResumeGrammar = bFlg;
    }
    else if ("finishupdate" == strXmlKey) {
        VR_LOG("m_index = %d FinishUpdateGammar", m_index);
        m_bUpdateGrammar = bFlg;
        m_strCurrentMsg.clear();
    }
    else if ("updateagain" == strXmlKey) {
        m_strUpdateAgainMsg = m_strCurrentMsg;
    }
    else if ("deletephonebuf" == strXmlKey) {
        DeletePhoneGrammarBuf();
    }
    else {

    }

    Notify();

    return;
}

std::string
VR_VoiceBoxCatalogController::GetCurrentMessage()
{
    return m_strCurrentMsg;
}

/* EOF */