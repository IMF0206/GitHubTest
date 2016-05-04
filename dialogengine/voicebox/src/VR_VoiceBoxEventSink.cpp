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

/* VBT Header */
#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif 

std::string VR_VoiceBoxEventSink::m_strPrompt = "";

// Constructor
VR_VoiceBoxEventSink::VR_VoiceBoxEventSink(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
    )
: IVECIEventSink()
, m_reference(0)
, m_client(client)
, m_engineCommand(engineCommand)
, m_engineCallback(engineCallback)
{
}

// Destructor
VR_VoiceBoxEventSink::~VR_VoiceBoxEventSink()
{
}

// QueryInterface
HRESULT
VR_VoiceBoxEventSink::QueryInterface(
    REFIID riid,
    void** ppvObj
)
{
    HRESULT hr = S_OK;
    if (NULL != ppvObj) {
        if (IID_IUnknown == riid) {
            *ppvObj = static_cast<IUnknown*>(this);
        }
        else if (IID_IVECIEventSink == riid) {
            *ppvObj = static_cast<IVECIEventSink*>(this);
        }
        else {
            *ppvObj = NULL;
            hr = E_NOINTERFACE;
        }

        if (SUCCEEDED(hr)) {
            AddRef();
        }
    }
    else {
        hr = E_POINTER;
    }
    return hr;
}

// AddRef
VBT_ULONG
VR_VoiceBoxEventSink::AddRef()
{
    return VBTInterlockedIncrement(&m_reference);
}

// Release
VBT_ULONG
VR_VoiceBoxEventSink::Release()
{
    const VBT_ULONG ulRef = VBTInterlockedDecrement(&m_reference);

    if (0 == ulRef) {
        delete this;
        return 0;
    }

    return ulRef;
}

// OnProcessStatus
HRESULT
VR_VoiceBoxEventSink::OnProcessStatus(
    VBT_CSTR szStatus
    )
{
    if (NULL == szStatus) {
        return E_FAIL;
    }

    return S_OK;
}

// OnSystemNotification
HRESULT
VR_VoiceBoxEventSink::OnSystemNotification(
    VBT_ULONG ulCode,
    IVECIParameterSet *pParameterSet
    )
{
    if (NULL == pParameterSet) {
        return E_FAIL;
    }

    return S_OK;
}

// OnSpeak
HRESULT
VR_VoiceBoxEventSink::OnSpeak(
    IVECITransaction* pTrans,
    IVECIStringSet* pSpeakList
    )
{
    if ((NULL == pTrans) || (NULL == pSpeakList)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnCommandComplete
HRESULT
VR_VoiceBoxEventSink::OnCommandComplete(
    IVECITransaction* pTrans
    )
{
    if (NULL == pTrans) {
        return E_FAIL;
    }

    return S_OK;
}

// OnBeepSync
HRESULT
VR_VoiceBoxEventSink::OnBeepSync()
{
    return S_OK;
}

// OnExtendedEvent
HRESULT
VR_VoiceBoxEventSink::OnExtendedEvent(VeciExtendedEvent type, IUnknown* pUnk)
{
    if (NULL == pUnk) {
        return E_FAIL;
    }

    return S_OK;
}

// OnClientMessage
HRESULT
VR_VoiceBoxEventSink::OnClientMessage(IVECIParsedMessage* pClientMsg)
{
    if (NULL == pClientMsg) {
        return E_FAIL;
    }

    return S_OK;
}

// OnKeywordActivation
HRESULT
VR_VoiceBoxEventSink::OnKeywordActivation()
{
    return S_OK;
}

// OnAudioLevelUpdate
HRESULT
VR_VoiceBoxEventSink::OnAudioLevelUpdate(VBT_LONG lValue)
{
    return S_OK;
}

bool
VR_VoiceBoxEventSink::ProcessMessage(
    const std::string& message
)
{
    return true;
}

std::string
VR_VoiceBoxEventSink::GetActionParaByName(IVECIParsedMessage *pcMsg, 
    const VBT_STR& strActionParaName)
{
    if (NULL == pcMsg || NULL == strActionParaName) {
        return "";
    }

    std::string strValue;

    CVECIOutStr strVBTValue;
    (void)pcMsg->GetActionParameterValue(strActionParaName, &strVBTValue);

    if (NULL != strVBTValue.Get()) {
        strValue = strVBTValue.Get();
    }

    return strValue;
}

std::string
VR_VoiceBoxEventSink::GetListParaByName(IVECIParsedMessage *pcMsg, 
    int index, const std::string& strListParaName)
{
    if (NULL == pcMsg || strListParaName.empty()) {
        return "";
    }

    std::string strValue;
    CVECIOutStr strVBTValue;

    (void)pcMsg->GetListItemParameterValue(index, strListParaName.c_str(), &strVBTValue);

    if (NULL != strVBTValue.Get()) {
        strValue = strVBTValue.Get();
    }

    return strValue;
}

bool 
VR_VoiceBoxEventSink::GetActionParameter(
    IVECIParsedMessage *pcMsg, 
    VoiceVector<NodeInfo>::type& vecActionParameter
)
{
    vecActionParameter.clear();

    if (NULL == pcMsg) {
        return false;
    }

    CVECIPtr<IVECIParameterSet> parmeterSet;
    m_client.CreateParameterSet(&parmeterSet);

    pcMsg->GetActionParameters(&parmeterSet);
    VBT_ULONG uSize;
    if (NULL == parmeterSet) {
        return false;
    }

    parmeterSet->GetSize(&uSize);
    CVECIOutStr strOut;
    VBT_STR strVBTValue; 
    std::string strName = "";
    std::string strValue = "";
    NodeInfo actionNode;

    for (VBT_ULONG uIndex = 0; uIndex < uSize; ++uIndex) {
        parmeterSet->GetParameter(uIndex, AttribName, &strOut);
        strVBTValue = strOut.Get();
        if (NULL == strVBTValue) {
            continue;
        }
        strName = strVBTValue;

        actionNode.strName = strName;
        parmeterSet->GetParameter(uIndex, AttribValue, &strOut);
        strVBTValue = strOut.Get();
        if (NULL == strVBTValue) {
            strValue = "";
        }
        else {
            strValue = strVBTValue;
        }
        
        actionNode.strValue = strValue;

        vecActionParameter.push_back(actionNode);
    }

    return true;
}

bool 
VR_VoiceBoxEventSink::GetListParameter(
    IVECIParsedMessage *pcMsg,
    ListNodeInfo& listParameter
)
{
    listParameter.strListType = "";
    listParameter.vecItems.clear();

    if (NULL == pcMsg) {
        return false;
    }

    VBT_ULONG uListSize;
    pcMsg->GetListSize(&uListSize);

    CVECIOutStr strOut;
    pcMsg->GetListType(&strOut);

    VBT_STR strVBTValue;
    strVBTValue = strOut.Get();
    if (NULL != strVBTValue) {
        listParameter.strListType = strVBTValue;
    }

    CVECIPtr<IVECIParameterSet> parmeterSet;
    m_client.CreateParameterSet(&parmeterSet);

    VoiceVector<NodeInfo>::type vecItem;
    VBT_ULONG uValueSize;
    NodeInfo nodeInfo;

    for (VBT_ULONG uListIndex = 0; uListIndex < uListSize; ++uListIndex) {
        vecItem.clear();
        pcMsg->GetListItem(uListIndex, &parmeterSet);
        if (NULL == parmeterSet) {
            continue;
        }

        parmeterSet->GetSize(&uValueSize);
        for (VBT_ULONG uItemIndex = 0; uItemIndex < uValueSize; ++uItemIndex) {
            parmeterSet->GetParameter(uItemIndex, AttribName, &strOut);
            strVBTValue = strOut.Get();
            if (NULL == strVBTValue) {
                continue;
            }

            nodeInfo.strName = strVBTValue;

            parmeterSet->GetParameter(uItemIndex, AttribValue, &strOut);
            strVBTValue = strOut.Get();
            if (NULL == strVBTValue) {
                nodeInfo.strValue = "";
            }
            else {
                nodeInfo.strValue = strVBTValue;
            }
            vecItem.push_back(nodeInfo);
        }
        listParameter.vecItems.push_back(vecItem);
    }
    return true;
}

bool
VR_VoiceBoxEventSink::ReStartRecoSession()
{
    VR_VoiceBoxXmlBuilder xmlStartBulder;
    VoiceVector<StructNode>::type attributeStartVector;
    StructNode structNode;
    structNode.Name = "event";
    structNode.Value = "waitStart";
    attributeStartVector.push_back(structNode);

    pugi::xml_node root_node = xmlStartBulder.buildGivenElement("send",
        attributeStartVector);
    std::string strStartReply = xmlStartBulder.getXmlString();
    m_engineCallback.OnRequest(strStartReply);

    return true;
}

std::string
VR_VoiceBoxEventSink::GetStartIndex(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return "0";
    }

    std::string strStartIndex;
    std::string strCurPageIndex = GetActionParaByName(pcMsg, "Current Page Index");

    int iValue = atoi(strCurPageIndex.c_str());
    const int cstPageSize = 5;
    iValue = (iValue - 1) * cstPageSize;

    strStartIndex = std::to_string(iValue);

    return strStartIndex;
}

bool
VR_VoiceBoxEventSink::SendActionMessage(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr cstrAgentName;
    pcMsg->GetAgentName(&cstrAgentName);
    CVECIOutStr cstrActionClass;
    pcMsg->GetActionClass(&cstrActionClass);
    CVECIOutStr cstrActionType;
    pcMsg->GetActionType(&cstrActionType);
    CVECIOutStr cstrListType;
    pcMsg->GetListType(&cstrListType);

    std::string strAgentName;
    if (NULL != cstrAgentName.Get()) {
        strAgentName = cstrAgentName.Get();
    }
    std::string strActionClass;
    if (NULL != cstrActionClass.Get()) {
        strActionClass = cstrActionClass.Get();
    }
    std::string strActionType;
    if (NULL != cstrActionType.Get()) {
        strActionType = cstrActionType.Get();
    }
    else if (NULL != cstrListType.Get()) {
        strActionType = cstrListType.Get();
    }
    else {
        return false;
    }

    std::string strEvent = "<event name=\"actionMessage\">";
    strEvent.append("<agentName>");
    strEvent.append(strAgentName);
    strEvent.append("</agentName>");
    strEvent.append("<actionClass>");
    strEvent.append(strActionClass);
    strEvent.append("</actionClass>");
    strEvent.append("<actionType>");
    strEvent.append(strActionType);
    strEvent.append("</actionType>");
    strEvent.append("</event>");

    m_engineCallback.OnRequest(strEvent);

    return true;
}

void
VR_VoiceBoxEventSink::ShowPopupMsg(const std::string& strPrompt)
{
    std::string strDisplay = "<display agent=\"Common\" content=\"ShowPopupMessage\">";
    strDisplay.append("<prompt>");
    strDisplay.append(strPrompt);
    strDisplay.append("</prompt>");
    strDisplay.append("<type>normal</type>");
    strDisplay.append("</display>");

    m_engineCallback.OnRecognized(strDisplay);
}

/* EOF */
