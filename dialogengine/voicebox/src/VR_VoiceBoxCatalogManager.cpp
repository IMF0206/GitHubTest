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
#include "VR_Log.h"
/* Suntec Header */
#ifndef VR_VOICEBOXCATALOGMANAGER_H
#    include "VR_VoiceBoxCatalogManager.h"
#endif

#ifndef VR_VOICEBOXCATALOGAPPS_H
#    include "VR_VoiceBoxCatalogApps.h"
#endif

#ifndef VR_VOICEBOXCATALOGAUDIO_H
#    include "VR_VoiceBoxCatalogAudio.h"
#endif

#ifndef VR_VOICEBOXCATALOGPHONE_H
#    include "VR_VoiceBoxCatalogPhone.h"
#endif

#ifndef VR_VOICEBOXCATALOGPOI_H
#    include "VR_VoiceBoxCatalogPoi.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

VR_VoiceBoxCatalogManager::VR_VoiceBoxCatalogManager(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: m_client(client)
, m_engineCommand(engineCommand)
, m_engineCallback(engineCallback)
, m_catalogAudio(NULL)
, m_catalogPhone(NULL)
, m_catalogApps(NULL)
, m_catalogPoi(NULL)
, m_isUpdateGrammar(false)
{
}

VR_VoiceBoxCatalogManager::~VR_VoiceBoxCatalogManager()
{
    delete m_catalogApps;
    m_catalogApps = NULL;
    delete m_catalogAudio;
    m_catalogAudio = NULL;
    delete m_catalogPhone;
    m_catalogPhone = NULL;
    delete m_catalogPoi;
    m_catalogPoi = NULL;
}

bool
VR_VoiceBoxCatalogManager::Initialize()
{
    m_catalogApps = VR_new VR_VoiceBoxCatalogApps(m_client, m_engineCallback);
    if (NULL == m_catalogApps) {
        VR_ERROR("Create Catalog for Audio Failed!!!");
        return false;
    }

    if (NULL != m_catalogApps) {
        m_catalogApps->Initialize();
    }

    m_catalogAudio = VR_new VR_VoiceBoxCatalogAudio(m_client, m_engineCommand, m_engineCallback);
    if (NULL == m_catalogAudio) {
        VR_ERROR("Create Catalog for Audio Failed!!!");
        return false;
    }

    if (NULL != m_catalogAudio) {
        m_catalogAudio->Initialize();
    }

    m_catalogPhone = VR_new VR_VoiceBoxCatalogPhone(m_client, m_engineCallback);
    if (NULL == m_catalogPhone) {
        VR_ERROR("Create Catalog for Phone Failed!!!");
        return false;
    }

    if (NULL != m_catalogPhone) {
        m_catalogPhone->Initialize();
    }

    m_catalogPoi = VR_new VR_VoiceBoxCatalogPoi(m_client, m_engineCallback);
    if (NULL == m_catalogPoi) {
        VR_ERROR("Create Catalog for POI Failed!!!");
        return false;
    }

    if (NULL != m_catalogPoi) {
        m_catalogPoi->Initialize();
    }

    return true;
}

void
VR_VoiceBoxCatalogManager::ProcessMessage(const std::string& message)
{
    VR_VoiceBoxXmlParser parser(message);
    std::string strXmlKey = parser.getXmlKey();
    std::string strAgent = parser.getValueByKey("agent");
    bool bEngineStarted = m_engineCallback.GetEngineStatus();
    VR_LOG("strXmlKey = %s, strAgent = %s", strXmlKey.c_str(), strAgent.c_str());
    if (bEngineStarted) {
        if ("grammar_init" == strXmlKey || "grammar_diff" == strXmlKey) {
            if ("phone" == strAgent) {
                m_isUpdateGrammar = true;
                m_catalogPhone->SetupPhoneBookData(parser);
            }
            else if ("media" == strAgent) {
                m_catalogAudio->SetupMusicData(parser);
            }
            else if ("poi" == strAgent) {
                m_catalogPoi->Cataloguing(parser);
            }
            else {
                m_engineCallback.SetUpdateGammarFlg(true);
            }
        }
        else if ("category" == strXmlKey) {
            if (std::string::npos != message.find("<category name=\"quickreplymessage\"")) {
                m_catalogPhone->UpdateHFDQuickReplyMessages(parser);
            }
            else if (std::string::npos != message.find("<category name=\"phonetype\"")) {
                m_catalogPhone->UpdateHFDPhoneTypes(parser);
            }
            else if (std::string::npos != message.find("<category name=\"messagetype\"")) {
                m_catalogPhone->UpdateHFDMessageTypes(parser);
            }
            else if (std::string::npos != message.find("<category name=\"audiosource\"")) {
                m_catalogAudio->UpdateMusicAudioSources(parser);
            }
            else if (std::string::npos != message.find("<category name=\"fmgenre\"")) {
                m_catalogAudio->UpdateRadioFMGenres(parser);
            }
            else if (std::string::npos != message.find("<category name=\"satchannelname\"")) {
                m_catalogAudio->UpdateRadioSatelliteChannelNames(parser);
            }
            else if (std::string::npos != message.find("<category name=\"satchannelnumber\"")) {
                m_catalogAudio->UpdateRadioSatelliteChannelNumbers(parser);
            }
            else if (std::string::npos != message.find("<category name=\"satgenre\"")) {
                m_catalogAudio->UpdateRadioSatelliteGenres(parser);
            }
            else if (std::string::npos != message.find("<category name=\"hdsubchannel\"")) {
                m_engineCallback.SetUpdateGammarFlg(true);
                // m_catalogAudio->UpdateRadioHDSubChannels(parser);
            }
            else if (std::string::npos != message.find("<add>")) {
                m_catalogAudio->MusicAddUpdate(parser);
            }
            else if (std::string::npos != message.find("<delete>")) {
                m_catalogAudio->MusicDeleteUpdate(parser);
            }
            else {
                m_engineCallback.SetUpdateGammarFlg(true);
            }
        }
        // else if ("grammar_diff" == strXmlKey) {
        //     m_catalogAudio->MusicGrammarDiff(parser);
        // }
        else if ("grammar_active" == strXmlKey) {
            if ("phone" == strAgent) {
                if (m_isUpdateGrammar) {
                    m_isUpdateGrammar = false;
                    std::string grammarInitResult = "<grammar_result op=\"active\" agent=\"phone\" grammarid=\"1\" errcode=\"0\"/>";
                    m_engineCallback.OnRecognized(grammarInitResult);
                    m_engineCallback.SetUpdateGammarFlg(true);
                }
                else {
                    m_catalogPhone->SetupPhoneBookData(parser);
                }
            }
            else if ("media" == strAgent) {
                m_catalogAudio->MusicGrammarActive(parser);
            }
            else {
                m_engineCallback.SetUpdateGammarFlg(true);
            }
        }
        else if ("grammar_disactive" == strXmlKey) {
            if ("phone" == strAgent) {
                m_catalogPhone->PhoneGrammarDisActive(parser);
            }
            else if ("media" == strAgent) {
                m_catalogAudio->MusicGrammarDisActive(parser);
            }
            else {
                m_engineCallback.SetUpdateGammarFlg(true);
            }
        }
        else if ("grammar_voicetag" == strXmlKey) {
            m_catalogPhone->UpdateVoiceTagGrammar(parser);
        }
        else if ("grammar_audiosource_oc" == strXmlKey) {
            m_catalogAudio->MusicGrammarAudioSourceOC(parser);
        }
        else if ("grammar_delete" == strXmlKey) {
            m_catalogPoi->DeleteGrammar();
        }
        else {
            m_engineCallback.SetUpdateGammarFlg(true);
        }
    }
    else {
        VR_LOG("the engine is not started");
        if ("grammar_init" == strXmlKey || "grammar_diff" == strXmlKey) {
            if ("media" == strAgent) {
                std::string strGrammarId = parser.getValueByKey("grammarid");
                m_engineCallback.SendGrammarResult("grammar", "media", strGrammarId, "1");
            }
            else if ("phone" == strAgent) {
                m_engineCallback.SendGrammarResult("grammar", "phone", "1", "2");
            }
            else {

            }
        }
        // else if ("grammar_diff" == strXmlKey) {
        //    std::string strGrammarId = parser.getValueByKey("grammarid");
        //     m_engineCallback.SendGrammarResult("grammar", "media", strGrammarId, "1");
        // }
        else if ("grammar_active" == strXmlKey) {
            if ("phone" == strAgent) {
                m_engineCallback.SendGrammarResult("active", "phone", "1", "2");
            }
            else if ("media" == strAgent) {
                std::string strReply = parser.getValueByKey("reply");
                if ("true" == strReply) {
                    std::string strGrammarId = parser.getValueByKey("grammarid");
                    m_engineCallback.SendGrammarResult("active", "media", strGrammarId.c_str(), "1");
                }
            }
            else {

            }
        }
        else if ("grammar_disactive" == strXmlKey) {
            if ("phone" == strAgent) {
                std::string strSender = parser.getValueByKey("sender");
                if ("DE" != strSender) {
                    m_engineCallback.SendGrammarResult("disactive", "phone", "1", "2");
                }
            }
            else if ("media" == strAgent) {
                std::string strReply = parser.getValueByKey("reply");
                if ("true" == strReply) {
                    std::string strGrammarId = parser.getValueByKey("grammarid");
                    m_engineCallback.SendGrammarResult("disactive", "media", strGrammarId.c_str(), "1");
                }
            }
        }
        else {
            VR_LOG("engine is not started, strXmlKey = %s", strXmlKey.c_str());
        }

        m_engineCallback.SetUpdateGammarFlg(true);
    }

}

VR_VoiceBoxCatalogPhone*
VR_VoiceBoxCatalogManager::GetCatalogPhone()
{
    return m_catalogPhone;
}

VR_VoiceBoxCatalogAudio*
VR_VoiceBoxCatalogManager::GetCatalogAudio()
{
    return m_catalogAudio;
}

void
VR_VoiceBoxCatalogManager::PauseGrammarUpdate(const std::string& strAgent)
{
    VR_LOG("strAgent = %s", strAgent.c_str());
    if (strAgent.empty()) {
        return;
    }

    if ("music" == strAgent) {
        m_catalogAudio->PauseGrammarMusic();
    }
    else if ("radio" == strAgent) {
        m_catalogAudio->PauseGrammarRadio();
    }
    else if ("hfd" == strAgent) {
        m_catalogPhone->PauseGrammarPhone();
    }
    else if ("apps" == strAgent) {
        m_catalogApps->PauseGrammarApps();
    }
    else if ("poi" == strAgent) {
        m_catalogPoi->PauseGrammarPoi();
    }
    else {

    }

    return;
}

void
VR_VoiceBoxCatalogManager::ResumeGrammarUpdate(const std::string& strAgent)
{
    VR_LOG("strAgent = %s", strAgent.c_str());
    if (strAgent.empty()) {
        return;
    }

    if ("music" == strAgent) {
        m_catalogAudio->ResumeGrammarMusic();
    }
    else if ("radio" == strAgent) {
        m_catalogAudio->ResumeGrammarRadio();
    }
    else if ("hfd" == strAgent) {
        m_catalogPhone->ResumeGrammarPhone();
    }
    else if ("apps" == strAgent) {
        m_catalogApps->ResumeGrammarApps();
    }
    else if ("poi" == strAgent) {
        m_catalogPoi->ResumeGrammarPoi();
    }
    else {

    }

    return;
}

void
VR_VoiceBoxCatalogManager::SendPhoneGrammarResult(const std::string& strPhoneGrammar)
{
    std::string strOp;
    if (std::string::npos != strPhoneGrammar.find("<grammar_active agent=\"phone\"")) {
        strOp = "active";
    }
    else if (std::string::npos != strPhoneGrammar.find("<grammar_disactive agent=\"phone\"")) {
        strOp = "disactive";
    }
    else if (std::string::npos != strPhoneGrammar.find("<grammar_init agent=\"phone\"")) {
        strOp = "grammar";
    }
    else {

    }

    m_engineCallback.SendGrammarResult(strOp, "phone", "1", "2");

    return;
}

std::pair<std::string, std::string>
VR_VoiceBoxCatalogManager::GetCurrentTransaction()
{
    return m_engineCallback.GetCurrentTransaction();
}

void
VR_VoiceBoxCatalogManager::CancelGrammarUpdate(const std::string& strAgent)
{
    VR_LOGD_FUNC();
    if ("music" == strAgent && NULL != m_catalogAudio) {
        m_catalogAudio->CancelMusicGrammarUpdate();
    }
    else if ("radio" == strAgent && NULL != m_catalogAudio) {
        m_catalogAudio->CancelRadioGrammarUpdate();
    }
    else if ("hfd" == strAgent && NULL != m_catalogPhone) {
        m_catalogPhone->CancelGrammarUpdate();
    }
    else if ("apps" == strAgent && NULL != m_catalogApps) {
        m_catalogApps->CancelGrammarUpdate();
    }
    else if ("poi" == strAgent && NULL != m_catalogPoi) {
        m_catalogPoi->CancelGrammarUpdate();
    }
    else {

    }

    return;
}

void
VR_VoiceBoxCatalogManager::InitialPersonData()
{
    if (NULL != m_catalogPhone) {
        m_catalogPhone->InitialHFDPersonData();
    }

    if (NULL != m_catalogPhone) {
        m_catalogAudio->InitialMusicPersonData();
    }

    return;
}

/* EOF */
