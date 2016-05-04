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
#include "VR_Log.h"


/* VBT Header */
#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_PREFERENCES_H
#    include "VBT_SDK_Preferences.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXCATALOGAUDIO_H
#    include "VR_VoiceBoxCatalogAudio.h"
#endif

#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

using namespace nutshell;

std::string VR_VoiceBoxCatalogAudio::m_strSourceId = "";
std::string VR_VoiceBoxCatalogAudio::m_strDBPath = "";
bool VR_VoiceBoxCatalogAudio::m_bIPodUSB2 = false;
bool VR_VoiceBoxCatalogAudio::m_bUSBIPod2 = false;

VR_VoiceBoxCatalogAudio::VR_VoiceBoxCatalogAudio(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: m_client(client)
, m_engineCommand(engineCommand)
, m_engineCallback(engineCallback)
{
}

VR_VoiceBoxCatalogAudio::~VR_VoiceBoxCatalogAudio()
{
}

bool
VR_VoiceBoxCatalogAudio::Initialize()
{
    HRESULT result = m_client.CreateAgentDispatcher(&m_agentDataCommandMusic);
    if (S_OK != result) {
        return false;
    }
    if (NULL == m_agentDataCommandMusic.ptr()) {
        return false;
    }

    result = m_client.CreateAgentDispatcher(&m_agentDataCommandRadio);
    if (S_OK != result) {
        return false;
    }
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    result = m_agentDataCommandMusic->Init(VBT_AGENT_MUSIC);
    if (S_OK != result) {
        return false;
    }

    result = m_agentDataCommandRadio->Init(VBT_AGENT_RADIO);
    if (S_OK != result) {
        return false;
    }

    m_mapUSENAudioSource.clear();
    m_mapUSESAudioSource.clear();
    m_mapUSFRAudioSource.clear();

    InsertAudioSourceBuf("AM", "AM", m_mapUSENAudioSource);
    InsertAudioSourceBuf("AM", "AM Radio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("FM", "FM", m_mapUSENAudioSource);
    InsertAudioSourceBuf("FM", "FM Radio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("FM", "Radio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "XM", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "Syrius", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "Syrius XM", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "Sat", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "Satellite", m_mapUSENAudioSource);
    InsertAudioSourceBuf("XM", "Satellite Radio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("CD", "CD", m_mapUSENAudioSource);
    InsertAudioSourceBuf("CD", "CD Player", m_mapUSENAudioSource);
    InsertAudioSourceBuf("CD", "CD Changer", m_mapUSENAudioSource);
    InsertAudioSourceBuf("CD", "Disc", m_mapUSENAudioSource);
    InsertAudioSourceBuf("CD", "MP3 CD", m_mapUSENAudioSource);
    InsertAudioSourceBuf("iPod", "iPod", m_mapUSENAudioSource);
    InsertAudioSourceBuf("iPod", "iPod 1", m_mapUSENAudioSource);
    InsertAudioSourceBuf("iPod2", "iPod 2", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "USB", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "USB Audio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "USB 1", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "MP3 Player", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "MP3 Player 1", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "MP3 1", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB", "MP3", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB2", "USB Audio 2", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB2", "USB 2", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB2", "MP3 Player 2", m_mapUSENAudioSource);
    InsertAudioSourceBuf("USB2", "MP3 2", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Bluetooth", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Bluetooth Audio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "BT Audio", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "AUX", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Aux jack", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Auxiliary", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Audio jack", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Stereo plug", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Headphone jack", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "A V", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Composite video ", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Video", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Auxiliary video", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Back Seat", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear Seat Entertainment", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear System", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "R S E", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "R E S", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear Entertainment System", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "DVD", m_mapUSENAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "DVD Player", m_mapUSENAudioSource);

    InsertAudioSourceBuf("AM", "AM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("AM", "Radio AM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("FM", "FM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("FM", "Radio FM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "XM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "Sirius", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "Sirius XM", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "Sat", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "Satélite", m_mapUSESAudioSource);
    InsertAudioSourceBuf("XM", "Radio Satelital", m_mapUSESAudioSource);
    InsertAudioSourceBuf("CD", "CD", m_mapUSESAudioSource);
    InsertAudioSourceBuf("CD", "Reproductor de CD", m_mapUSESAudioSource);
    InsertAudioSourceBuf("CD", "Reproductor de Disco Compacto", m_mapUSESAudioSource);
    InsertAudioSourceBuf("CD", "Disco", m_mapUSESAudioSource);
    InsertAudioSourceBuf("iPod", "iPod", m_mapUSESAudioSource);
    InsertAudioSourceBuf("iPod", "iPod 1", m_mapUSESAudioSource);
    InsertAudioSourceBuf("iPod2", "iPod 2", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "USB", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "Memoria USB", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "USB 1", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "Tarjeta de memoria", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "Llavero USB", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "Llave de memoria", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "Unidad flash USB", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "MP3", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB", "MP3 1", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB2", "USB 2", m_mapUSESAudioSource);
    InsertAudioSourceBuf("USB2", "MP3 2", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Bluetooth", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Aparato de Bluetooth", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Dispositivo de Bluetooth", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Auxiliar", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Entrada auxiliar", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Puerto auxiliar", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Entrada de audífonos", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Entrada de estéreo", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Aux", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "A V", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Video Compuesto ", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Audio/Video", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Video Auxiliar", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Entrada de audio/video", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "DVD Trasero", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Sistema Trasero", m_mapUSESAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Sistema de entretenimiento Trasero ", m_mapUSESAudioSource);

    InsertAudioSourceBuf("AM", "AM", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("AM", "AM Radio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("FM", "FM", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("FM", "FM Radio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("FM", "Radio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "XM", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "Syrius", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "Syrius XM", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "Sat", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "Satellite", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("XM", "Satellite Radio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("CD", "CD", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("CD", "CD Player", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("CD", "CD Changer", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("CD", "Disc", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("CD", "MP3 CD", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("iPod", "iPod", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("iPod", "iPod 1", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("iPod2", "iPod 2", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "USB", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "USB Audio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "USB 1", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "MP3 Player", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "MP3 Player 1", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "MP3 1", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB", "MP3", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB2", "USB Audio 2", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB2", "USB 2", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB2", "MP3 Player 2", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("USB2", "MP3 2", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Bluetooth", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "Bluetooth Audio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Bluetooth Audio", "BT Audio", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "AUX", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Aux jack", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Auxiliary", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Audio jack", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Stereo plug", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Headphone jack", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "A V", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Composite video ", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Video", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Auxiliary", "Auxiliary video", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Back Seat", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear Seat Entertainment", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear System", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "R S E", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "R E S", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "Rear Entertainment System", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "DVD", m_mapUSFRAudioSource);
    InsertAudioSourceBuf("Rear Entertainment System", "DVD Player", m_mapUSFRAudioSource);

    return true;
}

void
VR_VoiceBoxCatalogAudio::InsertAudioSourceBuf(
    const std::string& strFormal, const std::string& strAlias,
    VoiceMap<std::string, VoiceList<std::string >::type >::type& mapUSAudioSource)
{
    VoiceMap<std::string, VoiceList<std::string >::type >::iterator iter = mapUSAudioSource.find(strFormal);
    if (mapUSAudioSource.end() == iter) {
        VoiceList<std::string >::type lstAlias;
        lstAlias.push_back(strAlias);
        mapUSAudioSource.insert(std::make_pair(strFormal, lstAlias));
    }
    else {
        iter->second.push_back(strAlias);
    }
}

bool
VR_VoiceBoxCatalogAudio::LoadAudioSource(
    const VoiceVector<AudioSourceInfo>::type& vecAudioSource,
    CVECIPtr<IVECIListItems>& audioSourceList)
{
    if (vecAudioSource.empty()) {
        return true;
    }

    VR_LOG("VR_VoiceBoxCatalogAudio LoadAudioSource");
    bool bUSB1 = false;
    bool bUSB2 = false;
    bool bIPod1 = false;
    bool bIPod2 = false;
    std::string striPodName;
    std::string striPod2Name;
    for (size_t i = 0; i < vecAudioSource.size(); ++i) {
        if ("USB" == vecAudioSource[i].formalItem.name) {
            bUSB1 = true;
        }
        else if ("USB2" == vecAudioSource[i].formalItem.name) {
            bUSB2 = true;
        }
        else if ("iPod" == vecAudioSource[i].formalItem.name) {
            bIPod1 = true;
            VoiceMap<std::string, std::string>::const_iterator iter = vecAudioSource[i].aliasMap.begin();
            for (; vecAudioSource[i].aliasMap.end() != iter; ++iter) {
                if ("iPod" != iter->first || "iPod 1" != iter->first) {
                    striPodName = iter->first;
                    break;
                }
            }
        }
        else if ("iPod2" == vecAudioSource[i].formalItem.name) {
            bIPod2 = true;
            VoiceMap<std::string, std::string>::const_iterator iter = vecAudioSource[i].aliasMap.begin();
            for (; vecAudioSource[i].aliasMap.end() != iter; ++iter) {
                if ("iPod 2" != iter->first) {
                    striPod2Name = iter->first;
                    break;
                }
            }
        }
        else {

        }
    }

    m_bIPodUSB2 = (bIPod1 && bUSB2) ? true : false;
    m_bUSBIPod2 = (bUSB1 && bIPod2) ? true : false;

    std::string strCulture =  m_engineCallback.GetCultureName();
    VoiceMap<std::string, VoiceList<std::string >::type >::type mapUSAudioSource;
    if ("en-US" == strCulture) {
        mapUSAudioSource = m_mapUSENAudioSource;
    }
    else if ("es-MX" == strCulture) {
        mapUSAudioSource = m_mapUSESAudioSource;
    }
    else if ("fr-CA" == strCulture) {
        mapUSAudioSource = m_mapUSFRAudioSource;
    }
    else {

    }
    for (size_t i = 0; i < vecAudioSource.size(); ++i) {
        std::string strSourceId = std::to_string(i + 1);
        std::string strFormal = vecAudioSource[i].formalItem.name;
        if ("iPod2" == vecAudioSource[i].formalItem.name && m_bUSBIPod2) {
            strFormal = "iPod";
            AddAudioSourceItem(strSourceId, "iPod", striPod2Name, vecAudioSource[i].formalItem.threeItem, "", audioSourceList);
        }
        else if ("iPod2" == vecAudioSource[i].formalItem.name && !m_bUSBIPod2) {
            AddAudioSourceItem(strSourceId, "iPod2", striPod2Name, vecAudioSource[i].formalItem.threeItem, "", audioSourceList);
        }
        else if ("iPod" == vecAudioSource[i].formalItem.name) {
            AddAudioSourceItem(strSourceId, "iPod", striPodName, vecAudioSource[i].formalItem.threeItem, "", audioSourceList);
        }
        else if ("USB2" == vecAudioSource[i].formalItem.name && m_bIPodUSB2) {
            strFormal = "USB";
        }
        else {

        }

        AddAudioSourceItem(strSourceId, strFormal, vecAudioSource[i].formalItem.threeItem, mapUSAudioSource, audioSourceList);
    }

    return true;
}

void
VR_VoiceBoxCatalogAudio::AddAudioSourceItem(
    const std::string& strId, const std::string& strFormal, const std::string& strGrammarId,
    const VoiceMap<std::string, VoiceList<std::string >::type >::type& mapUSAudioSource,
    CVECIPtr<IVECIListItems>& audioSourceList)
{
    VoiceMap<std::string, VoiceList<std::string >::type >::const_iterator iter = mapUSAudioSource.find(strFormal);
    if (mapUSAudioSource.end() != iter) {
        VoiceList<std::string >::const_iterator iterAlias = iter->second.begin();
        for (; iter->second.end() != iterAlias; ++iterAlias) {
            AddAudioSourceItem(strId, strFormal, *iterAlias, strGrammarId, "", audioSourceList);
        }
    }
    else {
        AddAudioSourceItem(strId, strFormal, strFormal, strGrammarId, "", audioSourceList);
    }
}

bool
VR_VoiceBoxCatalogAudio::LoadSatChannelNumber(
    const VoiceVector<FormalTwoItem>::type& vecSatChannelNumber,
    CVECIPtr<IVECIListItems>& satChannelNumberList)
{
    if (vecSatChannelNumber.empty()) {
        return true;
    }

    if (NULL == satChannelNumberList.ptr()) {
        return true;
    }

    VR_LOG("VR_VoiceBoxCatalogAudio LoadSatChannelNumber");

    for (size_t i = 0; i < vecSatChannelNumber.size(); ++i) {
        CVECIPtr<IVECIParameterSet> spSatChannelNumber;
        (void)m_client.CreateParameterSet(&spSatChannelNumber);
        if (NULL == spSatChannelNumber.ptr()) {
            continue;
        }

        (void)spSatChannelNumber->AddParameter(_T("nId"), vecSatChannelNumber[i].strId.c_str(), NULL, NULL);
        (void)spSatChannelNumber->AddParameter(_T("cFormal"), vecSatChannelNumber[i].name.c_str(), NULL, NULL);
        (void)satChannelNumberList->AddItem(spSatChannelNumber);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::LoadFormalPronInfo(
    const VoiceVector<FormalPronInfo>::type& vecFormalPronInfo,
    CVECIPtr<IVECIListItems>& listItems)
{
    if (vecFormalPronInfo.empty() || NULL == listItems.ptr()) {
        return true;
    }

    for (size_t i = 0; i < vecFormalPronInfo.size(); ++i) {

        VoiceMap<std::string, std::string>::const_iterator iter = vecFormalPronInfo[i].aliasMap.begin();
        for (; vecFormalPronInfo[i].aliasMap.end() != iter; ++iter) {
            CVECIPtr<IVECIParameterSet> spItems;
            (void)m_client.CreateParameterSet(&spItems);
            if (NULL == spItems.ptr()) {
                continue;
        }

            (void)spItems->AddParameter(_T("nId"), vecFormalPronInfo[i].formalItem.strId.c_str(), NULL, NULL);
            (void)spItems->AddParameter(_T("cFormal"), vecFormalPronInfo[i].formalItem.name.c_str(), NULL, NULL);
            (void)spItems->AddParameter(_T("cAlias"), iter->first.c_str(), NULL, NULL);
            (void)spItems->AddParameter(_T("cPronunciation"), iter->second.c_str(), NULL, NULL);
            (void)listItems->AddItem(spItems);
        }
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::LoadHdSubChannel(
    const VoiceVector<FormalOtherInfo>::type& vecHdSubChannelInfo,
    CVECIPtr<IVECIListItems>& listHdSubChannel)
{
    if (vecHdSubChannelInfo.empty()) {
        return true;
    }

    VR_LOG("VR_VoiceBoxCatalogAudio LoadHdSubChannel");

    for (size_t i = 0; i < vecHdSubChannelInfo.size(); ++i) {
        for (size_t index = 0; index < vecHdSubChannelInfo[i].aliasVector.size(); ++index) {
            CVECIPtr<IVECIParameterSet> spItem;
            (void)m_client.CreateParameterSet(&spItem);

            // (void)spItem->AddParameter(_T("nId"), vecHdSubChannelInfo[i].formalItem.strId.c_str(), NULL, NULL);
            (void)spItem->AddParameter(_T("cFormal"), vecHdSubChannelInfo[i].formalItem.name.c_str(), NULL, NULL);
            (void)spItem->AddParameter(_T("cAlias"), vecHdSubChannelInfo[i].aliasVector[index].c_str(), NULL, NULL);
            (void)spItem->AddParameter(_T("cIntval"), vecHdSubChannelInfo[i].formalItem.threeItem.c_str(), NULL, NULL);
            (void)listHdSubChannel->AddItem(spItem);
        }
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateRadioFMGenres(VR_VoiceBoxXmlParser& parser)
{
    VoiceVector<FormalPronInfo>::type vecFmGenre;
    parser.getFmGenre(vecFmGenre);
    std::string strLastFlg = parser.getLastFlg();
    if (vecFmGenre.empty()) {
        RadioRemoveGrammer("RadioAgentFMGenres", "fmgenres", strLastFlg);
    }
    else {
        RadioAgentFMGenres(vecFmGenre, strLastFlg);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateRadioSatelliteChannelNames(VR_VoiceBoxXmlParser& parser)
{
    VoiceVector<FormalPronInfo>::type vecSatChannelName;
    parser.getSatChannelName(vecSatChannelName);
    std::string strLastFlg = parser.getLastFlg();
    if (vecSatChannelName.empty()) {
        RadioRemoveGrammer("RadioAgentSatelliteChannelNames", "satchannelnames", strLastFlg);
    }
    else {
        RadioAgentSatelliteChannelNames(vecSatChannelName, strLastFlg);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateRadioSatelliteChannelNumbers(VR_VoiceBoxXmlParser& parser)
{
    VoiceVector<FormalTwoItem>::type vecSatChannelNumber;
    parser.getSatChannelNumber(vecSatChannelNumber);
    std::string strLastFlg = parser.getLastFlg();
    if (vecSatChannelNumber.empty()) {
        RadioRemoveGrammer("RadioAgentSatelliteChannelNumbers", "satchannelnumbers", strLastFlg);
    }
    else {
        RadioAgentSatelliteChannelNumbers(vecSatChannelNumber, strLastFlg);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateRadioSatelliteGenres(VR_VoiceBoxXmlParser& parser)
{
    VoiceVector<FormalPronInfo>::type vecSatGenre;
    parser.getSatGenre(vecSatGenre);
    std::string strLastFlg = parser.getLastFlg();
    if (vecSatGenre.empty()) {
        RadioRemoveGrammer("RadioAgentSatelliteGenres", "satgenres", strLastFlg);
    }
    else {
        RadioAgentSatelliteGenres(vecSatGenre, strLastFlg);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateRadioHDSubChannels(VR_VoiceBoxXmlParser& parser)
{
    VoiceVector<FormalOtherInfo>::type vecHdSubChannel;
    parser.getHdSubChannel(vecHdSubChannel);
    std::string strLastFlg = parser.getLastFlg();
    if (vecHdSubChannel.empty()) {
        RadioRemoveGrammer("RadioAgentHDSubChannels", "hdsubchannels", strLastFlg);
    }
    else {
        RadioAgentHDSubChannels(vecHdSubChannel, strLastFlg);
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioAgentFMGenres(const VoiceVector<FormalPronInfo>::type& vecFmGenre, const std::string& strLastFlg)
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentFMGenres", "fmgenres", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> lstItems;
    (void)m_client.CreateListItems(&lstItems);
    LoadFormalPronInfo(vecFmGenre, lstItems);

    retCode = m_agentDataCommandRadio->ReloadData(NULL, _T(""), "RadioAgentFMGenres", lstItems);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentFMGenres", "fmgenres", VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->ReloadGrammar(&spTrans, _T(""),  "RadioAgentFMGenres");
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());

    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioAgentSatelliteChannelNames(const VoiceVector<FormalPronInfo>::type& vecSatChannelName, const std::string& strLastFlg)
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteChannelNames", "satchannelnames", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> lstItems;
    (void)m_client.CreateListItems(&lstItems);

    LoadFormalPronInfo(vecSatChannelName, lstItems);

    retCode = m_agentDataCommandRadio->ReloadData(NULL, _T(""), "RadioAgentSatelliteChannelNames", lstItems);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteChannelNames", "satchannelnames", VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->ReloadGrammar(&spTrans, _T(""),  "RadioAgentSatelliteChannelNames");
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());
    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioAgentSatelliteChannelNumbers(const VoiceVector<FormalTwoItem>::type& vecSatChannelNumber, const std::string& strLastFlg)
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteChannelNumbers", "satchannelnumbers", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> lstItems;
    (void)m_client.CreateListItems(&lstItems);

    LoadSatChannelNumber(vecSatChannelNumber, lstItems);

    retCode = m_agentDataCommandRadio->ReloadData(NULL, _T(""), "RadioAgentSatelliteChannelNumbers", lstItems);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteChannelNumbers", "satchannelnumbers", VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->ReloadGrammar(&spTrans, _T(""),  "RadioAgentSatelliteChannelNumbers");
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());
    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioAgentSatelliteGenres(const VoiceVector<FormalPronInfo>::type& vecSatGenre, const std::string& strLastFlg)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteGenres", "satgenres", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> lstItems;
    (void)m_client.CreateListItems(&lstItems);

    LoadFormalPronInfo(vecSatGenre, lstItems);

    retCode = m_agentDataCommandRadio->ReloadData(NULL, _T(""), "RadioAgentSatelliteGenres", lstItems);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentSatelliteGenres", "satgenres", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->ReloadGrammar(&spTrans, _T(""),  "RadioAgentSatelliteGenres");
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());
    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioAgentHDSubChannels(const VoiceVector<FormalOtherInfo>::type& vecHdSubChannel, const std::string& strLastFlg)
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return false;
    }

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentHDSubChannels", "hdsubchannels", VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> lstItems;
    (void)m_client.CreateListItems(&lstItems);

    LoadHdSubChannel(vecHdSubChannel, lstItems);

    retCode = m_agentDataCommandRadio->ReloadData(NULL, _T(""), "RadioAgentHDSubChannels", lstItems);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), "RadioAgentHDSubChannels", "hdsubchannels", VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->ReloadGrammar(&spTrans, _T(""),  "RadioAgentHDSubChannels");
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());
    return true;
}

bool
VR_VoiceBoxCatalogAudio::RadioRemoveGrammer(
    const std::string& strTableName, const std::string& strHandlerName, const std::string& strLastFlg)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandRadio.ptr() || "" == strTableName) {
        return false;
    }
    VR_LOG("strTableName: %s", strTableName.c_str());

    HRESULT retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), strHandlerName.c_str(), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }
    retCode = m_agentDataCommandRadio->RemoveAllData(NULL, _T(""), strTableName.c_str());
    if (S_OK != retCode) {
        VR_ERROR("RemoveAllData: 0x%lx", retCode);
        return false;
    }
    retCode = m_agentDataCommandRadio->SetDataSynchronized(NULL, _T(""), strTableName.c_str(), strHandlerName.c_str(), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }
    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandRadio->RemoveAllGrammars(&spTrans, _T(""), strTableName.c_str());

    if (S_OK != retCode) {
        VR_ERROR("RemoveAllGrammars: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.SetCurrentTransaction("radio", strTransaction);
    }

     VR_LOG("grammar : %s", strTransaction.c_str());

     return true;
}

bool
VR_VoiceBoxCatalogAudio::UpdateMusicAudioSources(
    VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandMusic.ptr()) {
        return false;
    }

    VoiceVector<AudioSourceInfo>::type vecAudioSource;
    parser.getAudioSource(vecAudioSource);
    HRESULT retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, _T(""), _T("MusicAgentAudioSources"), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> audioSourceList;
    (void)m_client.CreateListItems(&audioSourceList);

    // Music AudioSource
    LoadAudioSource(vecAudioSource, audioSourceList);

    retCode = m_agentDataCommandMusic->ReloadData(NULL, _T(""), _T("MusicAgentAudioSources"), audioSourceList);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, _T(""), _T("MusicAgentAudioSources"), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandMusic->ReloadGrammar(&spTrans, _T(""),  _T("MusicAgentAudioSources"));
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.InsertMusicUpdateGrammar(strTransaction, "5");
        m_engineCallback.SetCurrentTransaction("music", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());

    return true;
}

void
VR_VoiceBoxCatalogAudio::AddAudioSourceItem(const std::string& stdId,
    const std::string& strFormal, const std::string& strAlias,
    const std::string& strGrammarId, const std::string& strIspermanent,
    CVECIPtr<IVECIListItems>& audioSourceList)
{
    CVECIPtr<IVECIParameterSet> spItems;
    (void)m_client.CreateParameterSet(&spItems);
    if (NULL == spItems.ptr() || NULL == audioSourceList.ptr()) {
        return;
    }

    VR_LOG("nId = %s, cFormal = %s, cAlias = %s, cTts = %s, cGrammarId = %s, cIspermanent = %s", \
        stdId.c_str(), strFormal.c_str(), strAlias.c_str(), strFormal.c_str(), strGrammarId.c_str(), strIspermanent.c_str());

    (void)spItems->AddParameter(_T("nId"), stdId.c_str(), NULL, NULL);
    (void)spItems->AddParameter(_T("cFormal"), strFormal.c_str(), NULL, NULL);
    (void)spItems->AddParameter(_T("cAlias"), strAlias.c_str(), NULL, NULL);
    (void)spItems->AddParameter(_T("cTts"), strFormal.c_str(), NULL, NULL);
    (void)spItems->AddParameter(_T("cGrammarId"), strGrammarId.c_str(), NULL, NULL);
    if (!strIspermanent.empty()) {
        (void)spItems->AddParameter(_T("cIspermanent"), strIspermanent.c_str(), NULL, NULL);
    }

    (void)audioSourceList->AddItem(spItems);
}

bool
VR_VoiceBoxCatalogAudio::MusicGrammarAudioSourceOC(VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    if (NULL == m_agentDataCommandMusic.ptr()) {
        return false;
    }

    std::string strWithDisc = parser.getValueByKey("withdisc");
    VR_LOG("strWithDisc = %s", strWithDisc.c_str());
    HRESULT retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, _T(""), _T("MusicAgentAudioSources"), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECIListItems> audioSourceList;
    (void)m_client.CreateListItems(&audioSourceList);

    // id formal alias tts grammarid ispermanent
    AddAudioSourceItem("1", "USB", "USB", "", "0", audioSourceList);
    AddAudioSourceItem("2", "iPod", "iPod", "", "0", audioSourceList);
    AddAudioSourceItem("3", "USB2", "USB2", "", "0", audioSourceList);
    AddAudioSourceItem("4", "iPod2", "iPod2", "", "0", audioSourceList);
    AddAudioSourceItem("5", "Bluetooth Audio", "bluetooth audio", "", "0", audioSourceList);
    AddAudioSourceItem("5", "Bluetooth Audio", "bluetooth", "", "0", audioSourceList);
    AddAudioSourceItem("6", "AM Radio", "AM radio", "", "1", audioSourceList);
    AddAudioSourceItem("6", "AM Radio", "AM", "", "1", audioSourceList);
    AddAudioSourceItem("7", "FM Radio", "FM radio", "", "1", audioSourceList);
    AddAudioSourceItem("7", "FM Radio", "FM", "", "1", audioSourceList);
    AddAudioSourceItem("8", "Auxiliary", "auxiliary", "", "1", audioSourceList);
    AddAudioSourceItem("8", "Auxiliary", "line in", "", "1", audioSourceList);
    AddAudioSourceItem("9", "DAB", "DAB", "", "1", audioSourceList);
    AddAudioSourceItem("9", "DAB", "dab", "", "1", audioSourceList);

    if ("true" == strWithDisc) {
        AddAudioSourceItem("10", "Disc", "disc", "", "1", audioSourceList);
        AddAudioSourceItem("10", "Disc", "Compact Disc", "", "1", audioSourceList);
        AddAudioSourceItem("10", "Disc", "CD", "", "1", audioSourceList);
        AddAudioSourceItem("10", "Disc", "DVD", "", "1", audioSourceList);
    }

    retCode = m_agentDataCommandMusic->ReloadData(NULL, _T(""), _T("MusicAgentAudioSources"), audioSourceList);
    if (S_OK != retCode) {
        VR_ERROR("ReloadData: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, _T(""), _T("MusicAgentAudioSources"), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandMusic->ReloadGrammar(&spTrans, _T(""),  _T("MusicAgentAudioSources"));
    if (S_OK != retCode) {
        VR_ERROR("ReloadGrammar: 0x%lx", retCode);
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.InsertMusicUpdateGrammar(strTransaction, "5");
        m_engineCallback.SetCurrentTransaction("music", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("grammar : %s", strTransaction.c_str());

    return true;
}

bool
VR_VoiceBoxCatalogAudio::MusicGrammarActive(
    VR_VoiceBoxXmlParser& parser)
{
    m_strSourceId = parser.getValueByKey("grammarid");
    std::string strPath = parser.getValueByKey("path");
    std::string strReply = parser.getValueByKey("reply");
    std::string strDMActiveSourceId = m_engineCallback.GetMDActiveSourceId();
    VR_LOG("strDMActiveSourceId = %s, m_strSourceId = %s", strDMActiveSourceId.c_str(), m_strSourceId.c_str());
    if (strDMActiveSourceId != m_strSourceId) {
        if ("true" == strReply) {
            m_engineCallback.SendGrammarResult("active", "media", m_strSourceId.c_str(), "0");
        }
        m_engineCallback.SetUpdateGammarFlg(true);

        return true;
    }

    std::string strAudioSourceConnected = m_engineCallback.GetAudioConnected();
    HRESULT retCode = m_engineCommand.SetPreference(NULL, _T("Music"),
        VBT_USR_PREF_MUSIC_AUDIOSOURCECONNECTED, strAudioSourceConnected.c_str());
    if (S_OK != retCode) {
        return false;
    }

    m_strDBPath = strPath;
    VR_LOG("VR_VoiceBoxCatalogAudio m_strDBPath = %s, m_strSourceId = %s", m_strDBPath.c_str(), m_strSourceId.c_str());

    SetMusicDBPathByPath(strPath);
    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandMusic->SetDataActiveSource(&spTrans, m_strSourceId.c_str(), NULL, NULL);
    if (S_OK != retCode || NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        std::string strTranstmp = strTrans.Get();
        if ("true" == strReply) {
            m_engineCallback.SetActiveSouceTrans(strTranstmp, "active", m_strSourceId);
        }
    }

    return true;
}

void
VR_VoiceBoxCatalogAudio::SetDataActiveSource(
    const std::string& strActiveSourceId)
{
    VR_LOGD_FUNC();

    if (NULL == m_agentDataCommandMusic) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    HRESULT retCode = m_agentDataCommandMusic->SetDataActiveSource(&spTrans, strActiveSourceId.c_str(), NULL, NULL);
    if (S_OK != retCode || NULL == spTrans.ptr()) {
        return;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        VR_LOG("strTrans = %s", strTrans.Get());
    }
}

bool
VR_VoiceBoxCatalogAudio::MusicGrammarDisActive(
    VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();

    m_strDBPath.clear();
    m_strSourceId.clear();

    std::string strSourceId = parser.getValueByKey("grammarid");
    std::string strReply = parser.getValueByKey("reply");

    std::string strDMActiveSourceId = m_engineCallback.GetMDActiveSourceId();
    VR_LOG("strDMActiveSourceId = %s", strDMActiveSourceId.c_str());
    if (strDMActiveSourceId != "5") {
        if ("true" == strReply) {
            m_engineCallback.SendGrammarResult("disactive", "media", m_strSourceId.c_str(), "0");
        }
        m_engineCallback.SetUpdateGammarFlg(true);

        return true;
    }
    CVECIPtr<IVECITransaction> spTrans;
    HRESULT retCode = m_agentDataCommandMusic->SetDataActiveSource(&spTrans, "5", NULL, NULL);
    if (S_OK != retCode || NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    if (NULL != strTrans.Get()) {
        std::string strTranstmp = strTrans.Get();
        if ("true" == strReply) {
            m_engineCallback.SetActiveSouceTrans(strTranstmp, "disactive", strSourceId);
        }
    }

    return true;
}

void
VR_VoiceBoxCatalogAudio::SaveMusicSourceId(const std::string& strSourceId)
{
    if (strSourceId.empty()) {
        return;
    }

    int iSoureceId = std::atoi(strSourceId.c_str());
    if (iSoureceId < 1 || iSoureceId > 4) {
        VR_LOG("strSourceId = %s is not available", strSourceId.c_str());
        return;
    }

    FILE * fp = fopen("/data/vr/data/sourceid.txt", "rb+");
    if (NULL == fp) {
        VR_LOG("open sourceid.txt false");
        return;
    }
    if (-1 != (fseek(fp, iSoureceId - 1, SEEK_SET))) {
        (void)fwrite("1", 1, 1, fp);
    }
    else {
        VR_LOG("fseek sourceid.txt false");
    }

    (void)fclose(fp);
}

bool
VR_VoiceBoxCatalogAudio::SetupMusicData(
    VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: update music grammar... case : 212-137-00 212-139-00");

    if (NULL == m_agentDataCommandMusic.ptr()) {
        return false;
    }

    std::string strGrammarId = parser.getValueByKey("grammarid");
    std::string strPath = parser.getValueByKey("path");
    std::string strSongItem = parser.getValueByKey("songitemcount");
    std::string strOtherItem = parser.getValueByKey("otheritemcount");

    int iSongItem = std::atoi(strSongItem.c_str());
    int iOtherItem = std::atoi(strOtherItem.c_str());

    m_engineCallback.SetGrammarInitSourceId(strGrammarId);

    VR_LOG("Music iSongItem = %d, iOtherItem = %d", iSongItem, iOtherItem);

    std::string strAudioSourceConnected = m_engineCallback.GetAudioConnected();

    HRESULT retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_AUDIOSOURCECONNECTED, strAudioSourceConnected.c_str());
    if (S_OK != retCode) {
        return false;
    }

    SaveMusicSourceId(strGrammarId);
    SetMusicDBPathByPath(strPath);
    SetMusicEnableByCount(iSongItem, iOtherItem);

    VR_LOG("begin to reload music grammar");

    retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, strGrammarId.c_str(), _T(""), _T(""), VBT_FALSE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, strGrammarId.c_str(), _T(""), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return false;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandMusic->ReloadGrammar(&spTrans, strGrammarId.c_str(), NULL);
    if (S_OK != retCode) {
        return false;
    }

    if (NULL == spTrans.ptr()) {
        return false;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return false;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        m_engineCallback.InsertMusicUpdateGrammar(strTransaction, strGrammarId);
        m_engineCallback.SetCurrentTransaction("music", strTransaction);
    }
    else {
        return false;
    }
    VR_LOG("music grammar strTransaction : %s", strTransaction.c_str());

    std::string strDMActiveSourceId = m_engineCallback.GetMDActiveSourceId();
    if (strGrammarId == strDMActiveSourceId) {
        CVECIPtr<IVECITransaction> spTransActive;
        m_agentDataCommandMusic->SetDataActiveSource(&spTransActive, strGrammarId.c_str(), NULL, NULL);
        if (S_OK != retCode || NULL == spTransActive.ptr()) {
            return false;
        }
    }

    return true;
}

void
VR_VoiceBoxCatalogAudio::SetMusicDBPathByPath(std::string& strPath)
{
    if (strPath.empty()) {
        return;
    }

    size_t ipos1 = strPath.rfind(".db");
    size_t ipos2 = strPath.rfind("MusicCatalog");

    if (std::string::npos != ipos1 && std::string::npos != ipos2) {
        strPath = strPath.replace(ipos2 + 12, (ipos1 - (ipos2 + 12)), "");
    }

    HRESULT retCode = m_engineCommand.SetPreference(NULL, _T("Music"), _T("DBPath"), strPath.c_str());
    if (S_OK != retCode) {
        return;
    }
}

void
VR_VoiceBoxCatalogAudio::SetMusicEnableByCount(const int iSongItem, const int iOtherItem)
{
    HRESULT retCode = S_OK;
    int iTotal = iSongItem + iOtherItem;

    if (iTotal > 15000) {
        retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_ENABLESONGS, "False");
        if (S_OK != retCode) {
            return;
        }

        std::string strEnableOthers = (iOtherItem > 15000) ? "False" : "True";
        retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_ENABLEOTHERS, strEnableOthers.c_str());
        if (S_OK != retCode) {
            return;
        }
    }
    else {
        retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_ENABLESONGS, "True");
        if (S_OK != retCode) {
            return;
        }

        retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_ENABLEOTHERS, "True");
        if (S_OK != retCode) {
            return;
        }
    }
}

bool
VR_VoiceBoxCatalogAudio::MusicDeleteUpdate(
     VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: differential update of music grammar... case : 212-13 212-15");

    VoiceMap<std::string, VoiceVector<MusicDiffDelete>::type>::type musicDiffDelete;
    parser.getMusicDiffDelete(musicDiffDelete);

    std::string strTableName;
    std::string strHandlerName;
    HRESULT retCode = S_OK;
    CVECIPtr<IVECITransaction> spTrans;

    VoiceMap<std::string, VoiceVector<MusicDiffDelete>::type>::const_iterator iterOut = musicDiffDelete.begin();
    for (; musicDiffDelete.end() != iterOut; ++iterOut) {
        strTableName.clear();
        strHandlerName.clear();
        GetTableHandlerName(iterOut->first, strTableName, strHandlerName);

        const VoiceVector<MusicDiffDelete>::type& vecDeleteData = iterOut->second;
        CVECIPtr<IVECIListItems> spListDel;
        m_client.CreateListItems(&spListDel);
        if (NULL == spListDel.ptr()) {
            continue;
        }

        for (size_t i = 0; i < vecDeleteData.size(); ++i) {
            CVECIPtr<IVECIParameterSet> spParams;
            m_client.CreateParameterSet(&spParams);
            if (NULL == spParams.ptr()) {
                continue;
             }

            spParams->AddParameter(_T("nId"), vecDeleteData[i].id.c_str(), NULL, NULL);

            spListDel->AddItem(spParams);
        }

        retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), VBT_FALSE);
        if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        continue;
        }

        retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), VBT_TRUE);
          if (S_OK != retCode) {
            VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
           continue;
        }

        retCode = m_agentDataCommandMusic->RemoveGrammar(&spTrans, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), spListDel);
        if (S_OK != retCode || NULL == spTrans.ptr()) {
            continue;
        }

        CVECIOutStr strTrans;
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            continue;
        }

        std::string strTransaction;
        if (NULL != strTrans.Get()) {
            strTransaction = strTrans.Get();
            m_engineCallback.InsertMusicUpdateGrammar(strTransaction, m_strSourceIdDiff);
            m_engineCallback.SetCurrentTransaction("music", strTransaction);
        }
        VR_LOG("music delete grammar  strTransaction : %s", strTransaction.c_str());
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::MusicAddUpdate(
    VR_VoiceBoxXmlParser& parser)
{
    VR_LOGD_FUNC();
    VR_LOGP("DE: differential update of music grammar... case : 212-12 212-14");

    if (NULL == m_agentDataCommandMusic.ptr()) {
        return false;
    }

    VoiceMap<std::string, VoiceVector<MusicDiffAdd>::type>::type musicDiffAdd;
    parser.getMusicDiffAdd(musicDiffAdd);

    std::string strTableName;
    std::string strHandlerName;
    HRESULT retCode = S_OK;
    CVECIPtr<IVECITransaction> spTrans;

    VoiceMap<std::string, VoiceVector<MusicDiffAdd>::type>::const_iterator iterOut = musicDiffAdd.begin();
    for (; musicDiffAdd.end() != iterOut; ++iterOut) {
        strTableName.clear();
        strHandlerName.clear();
        GetTableHandlerName(iterOut->first, strTableName, strHandlerName);

        const VoiceVector<MusicDiffAdd>::type& vecAddData = iterOut->second;
        CVECIPtr<IVECIListItems> spListAdd;
        m_client.CreateListItems(&spListAdd);
        if (NULL == spListAdd.ptr()) {
            continue;
        }

        for (size_t i = 0; i < vecAddData.size(); ++i) {
            CVECIPtr<IVECIParameterSet> spParams;
            m_client.CreateParameterSet(&spParams);
            if (NULL == spParams.ptr()) {
                continue;
            }

            spParams->AddParameter(_T("nId"), vecAddData[i].id.c_str(), NULL, NULL);

            spListAdd->AddItem(spParams);
        }

        retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), VBT_FALSE);
        if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        continue;
        }

        retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), VBT_TRUE);
        if (S_OK != retCode) {
            VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
            continue;
        }

        retCode = m_agentDataCommandMusic->AddGrammar(&spTrans, m_strSourceIdDiff.c_str(), strTableName.c_str(), strHandlerName.c_str(), spListAdd);
        if (S_OK != retCode || NULL == spTrans.ptr()) {
            continue;
        }

        CVECIOutStr strTrans;
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            continue;
        }

        std::string strTransaction;
        if (NULL != strTrans.Get()) {
            strTransaction = strTrans.Get();
            m_engineCallback.InsertMusicUpdateGrammar(strTransaction, m_strSourceIdDiff);
            m_engineCallback.SetCurrentTransaction("music", strTransaction);
        }
        VR_LOG("music add grammar  strTransaction: %s", strTransaction.c_str());
    }

    return true;
}

bool
VR_VoiceBoxCatalogAudio::MusicGrammarDiff(
    VR_VoiceBoxXmlParser& parser)
{
    VR_LOGP("DE: differential update of music grammar... case : 212-12-00 212-13-00 212-14-00 212-15-00");

    m_strSourceIdDiff = parser.getValueByKey("grammarid");
    std::string strPath = parser.getValueByKey("path");

    VR_LOG("m_strSourceIdDiff = %s", m_strSourceIdDiff.c_str());

    std::string strSongItem = parser.getValueByKey("songitemcount");
    std::string strOtherItem = parser.getValueByKey("otheritemcount");

    int iSongItem = std::atoi(strSongItem.c_str());
    int iOtherItem = std::atoi(strOtherItem.c_str());

    VR_LOG("begin to diff update music grammar,  Music iSongItem = %d, iOtherItem = %d", iSongItem, iOtherItem);
    std::string strAudioSourceConnected = m_engineCallback.GetAudioConnected();

    HRESULT retCode = m_engineCommand.SetPreference(NULL, _T("Music"), VBT_USR_PREF_MUSIC_AUDIOSOURCECONNECTED, strAudioSourceConnected.c_str());
    if (S_OK != retCode) {
        return false;
    }

    SetMusicDBPathByPath(strPath);
    SetMusicEnableByCount(iSongItem, iOtherItem);

    m_engineCallback.SetUpdateGammarFlg(true);

    return true;
}

void
VR_VoiceBoxCatalogAudio::GetTableHandlerName(const std::string& strGrammarName,
    std::string& strTableName, std::string& strHandlerName)
{
    if (strGrammarName.empty()) {
        return;
    }

    if ("song" == strGrammarName) {
        strTableName = "MusicAgentSongs";
        strHandlerName = "songs";
    }
    else if ("album" == strGrammarName) {
        strTableName = "MusicAgentAlbums";
        strHandlerName = "albums";
    }
    else if ("artist" == strGrammarName) {
        strTableName = "MusicAgentArtists";
        strHandlerName = "artists";
    }
    else if ("genre" == strGrammarName) {
        strTableName = "MusicAgentGenres";
        strHandlerName = "genres";
    }
    else if ("audiobook" == strGrammarName) {
        strTableName = "MusicAgentAudiobooks";
        strHandlerName = "audiobooks";
    }
    else if ("composer" == strGrammarName) {
        strTableName = "MusicAgentComposers";
        strHandlerName = "composers";
    }
    else if ("playlist" == strGrammarName) {
        strTableName = "MusicAgentPlaylists";
        strHandlerName = "playlists";
    }
    else if ("podcast" == strGrammarName) {
        strTableName = "MusicAgentPodcasts";
        strHandlerName = "podcasts";
    }
    else {

    }
}

void
VR_VoiceBoxCatalogAudio::PauseGrammarMusic()
{
    if (NULL == m_agentDataCommandMusic.ptr()) {
        return;
    }

    std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
    if (pairAgent2TransId.first.empty()) {
        VR_LOG("grammar has updated finish");
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode =  m_agentDataCommandMusic->PauseGrammarUpdate(&spTrans);
    if (FAILED(retCode)  || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }
}

void
VR_VoiceBoxCatalogAudio::PauseGrammarRadio()
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return;
    }
    std::pair<std::string, std::string> pairAgent2TransId = m_engineCallback.GetCurrentTransaction();
    if (pairAgent2TransId.first.empty()) {
        VR_LOG("grammar has updated finish");
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode =  m_agentDataCommandRadio->PauseGrammarUpdate(&spTrans);
    if (FAILED(retCode)  || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }
}

void
VR_VoiceBoxCatalogAudio::ResumeGrammarMusic()
{
    if (NULL == m_agentDataCommandMusic.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandMusic->ResumeGrammarUpdate(&spTrans);
    if (FAILED(retCode) || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }

    m_engineCallback.SetGrammarInitSourceId("5");
}

void
VR_VoiceBoxCatalogAudio::ResumeGrammarRadio()
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandRadio->ResumeGrammarUpdate(&spTrans);
    if (FAILED(retCode) || NULL == spTrans.ptr()) {
        return;
    }

    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != strTrans.Get()) {
        std::string strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
    }
}

void
VR_VoiceBoxCatalogAudio::InitialMusicPersonData()
{
    VR_LOGD_FUNC();
    const int MAXOURCEIDNUM = 4;
    for (int i = 1; i <= MAXOURCEIDNUM; ++i) {
        MusicRemoveAllGrammars(std::to_string(i));
    }
}

void
VR_VoiceBoxCatalogAudio::MusicRemoveAllGrammars(const std::string& strSourceId)
{
    if (NULL == m_agentDataCommandMusic.ptr() || strSourceId.empty()) {
        return;
    }

    HRESULT retCode = m_agentDataCommandMusic->SetDataSynchronized(NULL, _T(""), _T(""), _T(""), VBT_TRUE);
    if (S_OK != retCode) {
        VR_ERROR("SetDataSynchronized: 0x%lx", retCode);
        return;
    }

    CVECIPtr<IVECITransaction> spTrans;
    retCode = m_agentDataCommandMusic->RemoveAllGrammars(&spTrans, strSourceId.c_str(), "");
    if (FAILED(retCode) || NULL == spTrans.ptr()) {
        return;
    }

    CVECIOutStr strTrans;
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    std::string strTransaction;
    if (NULL != strTrans.Get()) {
        strTransaction = strTrans.Get();
        VR_LOG("strTransaction = %s", strTransaction.c_str());
        m_engineCallback.InsertMusicUpdateGrammar(strTransaction, "5");
        m_engineCallback.SetCurrentTransaction("music", strTransaction);
    }
    else {
        return;
    }

    VBT_BOOL bTimeout = VBT_FALSE;
    retCode = spTrans->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(retCode) || bTimeout) {
        VR_LOG("WaitForCompletion music RemoveAllGrammars failed: %xl", retCode);
        return;
    }

    VR_LOG("MusicRemoveAllGrammars end : strTransaction = %s, strSourceId = %s", strTransaction.c_str(), strSourceId.c_str());
}

void
VR_VoiceBoxCatalogAudio::CancelMusicGrammarUpdate()
{
    VR_LOGD_FUNC();
    if (NULL == m_agentDataCommandMusic.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandMusic->CancelGrammarUpdate(&spTrans);
    if (S_OK != retCode) {
        VR_LOG("CancelGrammarUpdate: 0x%lx", retCode);
        return;
    }
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != spTrans.ptr()) {
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            return;
        }
        VBT_BOOL bSaveTimeout = VBT_FALSE;
        retCode = spTrans->WaitForCompletion(INFINITE, &bSaveTimeout);
        if (FAILED(retCode) || bSaveTimeout) {
            VR_LOG("CancelGrammarUpdate failed: %lx", retCode);
        }

        if (NULL != strTrans.Get()) {
            std::string strTransaction = strTrans.Get();
            VR_LOG("strTransaction = %s", strTransaction.c_str());
        }
    }
}

void
VR_VoiceBoxCatalogAudio::CancelRadioGrammarUpdate()
{
    if (NULL == m_agentDataCommandRadio.ptr()) {
        return;
    }
    CVECIPtr<IVECITransaction> spTrans;
    CVECIOutStr strTrans;
    HRESULT retCode = m_agentDataCommandRadio->CancelGrammarUpdate(&spTrans);
    if (S_OK != retCode) {
        VR_LOG("CancelGrammarUpdate: 0x%lx", retCode);
        return;
    }
    retCode = spTrans->GetTransactionId(&strTrans);
    if (FAILED(retCode)) {
        return;
    }

    if (NULL != spTrans.ptr()) {
        retCode = spTrans->GetTransactionId(&strTrans);
        if (FAILED(retCode)) {
            return;
        }
        VBT_BOOL bSaveTimeout = VBT_FALSE;
        retCode = spTrans->WaitForCompletion(INFINITE, &bSaveTimeout);
        if (FAILED(retCode) || bSaveTimeout) {
            VR_LOG("CancelGrammarUpdate failed: %lx", retCode);
        }

        if (NULL != strTrans.Get()) {
            std::string strTransaction = strTrans.Get();
            VR_LOG("strTransaction = %s", strTransaction.c_str());
        }
    }
}
/* EOF */
