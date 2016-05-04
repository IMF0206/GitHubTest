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

/* Suntec Header */
#ifndef VR_VOICEBOXMSGCONTROLLER_H
#    include "VR_VoiceBoxMsgController.h"
#endif

#ifndef VR_VOICEBOXENGINEIF_H
#    include "VR_VoiceBoxEngineIF.h"
#endif

#ifndef CXX_BL_AUTOSYNC_H
#   include "BL_AutoSync.h"
#endif

BL_SyncObject   VR_VoiceBoxMsgController::s_cSync;   ///< Sync object

// Constructor
VR_VoiceBoxMsgController::VR_VoiceBoxMsgController()
: m_msglist()
, m_voiceBoxEngine(NULL)
{
}

// Destructor
VR_VoiceBoxMsgController::~VR_VoiceBoxMsgController()
{
    m_voiceBoxEngine = NULL;
}

// Initialize the Message Controller
bool
VR_VoiceBoxMsgController::Initialize(VR_VoiceBoxEngineIF* pcEngine)
{
    if (NULL == pcEngine) {
        return false;
    }

    m_voiceBoxEngine = pcEngine;
    RegisterName("VR_MSG_CONTROLLER_THREAD");
    return true;
}

// Start the Message Controller thread
void
VR_VoiceBoxMsgController::Start()
{
    StartRegistThread();
}

// Stop the Message Controller thread
void
VR_VoiceBoxMsgController::Stop()
{
    StopThread();
}

// Post message to the Message Controller
bool
VR_VoiceBoxMsgController::PostMessage(const std::string& message, int actionSeqId)
{
    // If the VoiceBoxEngine is not exist,
    // we could not process the message.
    if (NULL == m_voiceBoxEngine) {
        return false;
    }

    // Add new messages to the list
    {
        VR_MsgInfo msgInfo;
        msgInfo.iMsgId = actionSeqId;
        msgInfo.strMsg = message;
        BL_AutoSync cAutoSync(s_cSync);
        m_msglist.push_back(msgInfo);
    }

    Notify();

    return true;
}

// Process the messages
void
VR_VoiceBoxMsgController::Run()
{
    while (true) {
        if (m_msglist.empty()) {
            Wait();
        }

        if (TRUE == CheckQuit()) {
            break;
        }

        while (!m_msglist.empty()) {
            if (TRUE == CheckQuit()) {
                break;
            }

            VR_MsgInfo msgInfo;
            {
                BL_AutoSync cAutoSync(s_cSync);
                if (!m_msglist.empty()) {
                    msgInfo = m_msglist.front();
                    m_msglist.pop_front();
                }
                else {
                    continue;
                }
            }

            m_voiceBoxEngine->ProcessMessage(msgInfo.strMsg, msgInfo.iMsgId);
        }
    }
}

void
VR_VoiceBoxMsgController::ClearMessage()
{
    BL_AutoSync cAutoSync(s_cSync);
    m_msglist.clear();
}

void
VR_VoiceBoxMsgController::ProcessOpMessage(const std::string& strOpMsg)
{

}

std::string
VR_VoiceBoxMsgController::GetCurrentMessage()
{

}

/* EOF */