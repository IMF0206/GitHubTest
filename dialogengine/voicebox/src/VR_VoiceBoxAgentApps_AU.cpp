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
#include <cstdlib>
#include "VR_Log.h"
#include <stdio.h>
/* VBT Header */
// #ifndef VECIOUTSTR_H
// #    include "VECIOutStr.h"
// #endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

#ifndef VBT_SDK_LIST_TYPES_H
#include "VBT_SDK_List_Types.h"
#endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXAGENTAPPS_AU_H
#    include "VR_VoiceBoxAgentApps_AU.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif 

 /**
 * @brief The AppsAUActionHandler class
 *
 * class declaration
 */
struct AppsAUActionHandler
{
    char*             szAction; // Action type
    AppsAUMsgHandler  handler;  // Handler pointer
};

const std::string cstVO61001 = "There are no apps available.";
const std::string cstVO61002 = "No apps are available. The command cannot be executed.";

// Constructor
VR_VoiceBoxAgentApps_AU::VR_VoiceBoxAgentApps_AU(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
}

// Destructor
VR_VoiceBoxAgentApps_AU::~VR_VoiceBoxAgentApps_AU()
{
}

bool
VR_VoiceBoxAgentApps_AU::Initialize()
{
     const AppsAUActionHandler actionHandlerMap[] = {
        { "TSL App Not Installed",                 // "TSL App Not Installed"
             &VR_VoiceBoxAgentApps_AU::AppNotInstalled },
        { VBT_ACTION_TYPE_APPS_APPS_NOT_AVAILABLE, // "Apps Not Available"
             &VR_VoiceBoxAgentApps_AU::AppsNotAvailable },
        { "TSL App Not Enabled",                   // "TSL App Not Enabled"
             &VR_VoiceBoxAgentApps_AU::AppsNotAvailable },
        { "Apps Home",                             // "Apps Home"
             &VR_VoiceBoxAgentApps_AU::AppsHome },
        { "Apps Menu",                             // "Apps Menu"
             &VR_VoiceBoxAgentApps_AU::AppsMenu },
        { VBT_ACTION_TYPE_APPS_LAUNCH_APP, // "Launch App"
             &VR_VoiceBoxAgentApps_AU::LaunchApp },
        { "Launch TSL App",                // "Launch TSL App"
             &VR_VoiceBoxAgentApps_AU::LaunchApp },
        { VBT_ACTION_TYPE_APPS_LIST_APPS, // "List Apps"
             &VR_VoiceBoxAgentApps_AU::ListApps },
        { VBT_ACTION_TYPE_APPS_NO_APPS_AVAILABLE, // "No Apps Available"
             &VR_VoiceBoxAgentApps_AU::NoAppsAvailable },

        { VBT_LIST_TYPE_INFORMATION_NBEST, // "n-Best"
             &VR_VoiceBoxAgentApps_AU::NBest },
        { VBT_ACTION_TYPE_APPS_BACK, // "Back"
             &VR_VoiceBoxAgentApps_AU::AppBack },
        { VBT_ACTION_TYPE_APPS_CANCEL, // "Cancel"
             &VR_VoiceBoxAgentApps_AU::AppCancel },
        { VBT_ACTION_TYPE_APPS_HELP, // "Help"
             &VR_VoiceBoxAgentApps_AU::AppHelp },
        { VBT_ACTION_TYPE_APPS_START_OVER, // "Start Over"
             &VR_VoiceBoxAgentApps_AU::AppStartOver },
        { VBT_ACTION_TYPE_APPS_SYSTEM_PAUSE, // "Start Pause"
             &VR_VoiceBoxAgentApps_AU::AppStartPause },
        { VBT_ACTION_TYPE_APPS_SYSTEM_REPEAT, // "Start Repeat"
             &VR_VoiceBoxAgentApps_AU::AppStartRepeat },

        { "TSL App Status",                   // "TSL App Status"
             &VR_VoiceBoxAgentApps_AU::QueryAppStatus },

        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_appsMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }

    return true;
}

void
VR_VoiceBoxAgentApps_AU::UnInitialize()
{
    m_appsMsgHandler.clear();
}

bool
VR_VoiceBoxAgentApps_AU::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentApps_AU::QueryAgentSupport(
    VBT_CSTR szAgent,
    VBT_BOOL *pbSupported
)
{
    if (NULL == pbSupported) {
        VR_ERROR("QueryAgentSupport: NULL Parameter");
        return E_FAIL;
    }

    CVECICStr strAgent(&m_client, szAgent);
    // Indicate we want to receive events for Apps agent messages
    /* || strAgent.IsEqual(VBT_AGENT_SDL) */
    if (strAgent.IsEqual(VBT_AGENT_APPS)) {
        VR_LOG("QueryAgentSupport: %s", szAgent);
        *pbSupported = VBT_TRUE;
    }
    else {
        *pbSupported = VBT_FALSE;
    }

    return S_OK;
}

// OnAgentResultMessage
HRESULT
VR_VoiceBoxAgentApps_AU::OnAgentResultMessage(
    IVECITransaction* pTrans,
    IVECIParsedMessage* pResultMsg
)
{
    if ((NULL == pTrans) || (NULL == pResultMsg)) {
        VR_ERROR("OnAgentResultMessage: NULL Parameter");
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pResultMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

// OnAgentDriverMessage
HRESULT
VR_VoiceBoxAgentApps_AU::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    if (NULL == pDriverMsg) {
        VR_ERROR("OnAgentDriverMessage: NULL Parameter");
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentApps_AU::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        VR_ERROR("ProcessAgentMessage: NULL Parameter");
        return false;
    }

    if (!SendActionMessage(pcMsg)) {
        return false;
    }

    std::string strAgentResult = "";
    do {
        CVECIOutStr strActionType;
        pcMsg->GetActionType(&strActionType);
        if (NULL != strActionType.Get()) {
            strAgentResult = strActionType.Get();
            break;
        }

        CVECIOutStr strListType;
        pcMsg->GetListType(&strListType);
        if (NULL != strListType.Get()) {
            strAgentResult = strListType.Get();
            break;
        }

        VR_ERROR("ProcessAgentMessage: It is NOT action or list type");
        return false;

    } while (false);

    VR_LOG("ProcessAgentMessage: %s", strAgentResult.c_str());

    // Dispatch the messages
    VoiceMap<std::string, AppsAUMsgHandler>::const_iterator iterMap = m_appsMsgHandler.find(strAgentResult);
    if (m_appsMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    VR_ERROR("ProcessAgentMessage: The action or list type is NOT supported");
    return false;
}

bool
VR_VoiceBoxAgentApps_AU::AppsHome(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    VR_LOG("go to apps\n");
    m_engineCallback.OnRequest("<event name=\"startNextAgent\"> <agent>apps</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppsMenu(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"showHintScreen\"> <agent>apps</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::LaunchApp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    VoiceVector<NodeInfo>::type vecActionParameter;

    if (!GetActionParameter(pcMsg, vecActionParameter)) {
        return false;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node nodeAction = xmlBulder.buildStartActionElement("apps", "launchApp");
    VoiceVector<NodeInfo>::const_iterator itor = vecActionParameter.cbegin();
    for (; itor != vecActionParameter.cend(); ++itor) {
        if ("App ID" == itor->strName) {
            xmlBulder.buildGivenElement(nodeAction, "appid", itor->strValue, "", "");
            xmlBulder.buildGivenElement(nodeAction, "dataSize", "0", "", "");
            xmlBulder.buildGivenElement(nodeAction, "data", "", "", "");
        }
    }

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::ListApps(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    VoiceVector<NodeInfo>::type vecActionParameter;
    if (!GetActionParameter(pcMsg, vecActionParameter)) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }
    
    // ReStartRecoSession();

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node nodeCommon = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(nodeCommon, "agent", "apps", "", "");
    xmlBulder.buildGivenElement(nodeCommon, "content", "apps_apps_list_select", "", "");
    nodeCommon = xmlBulder.buildGivenElement(nodeCommon, "selectList", "", "", "");
    nodeCommon = xmlBulder.buildGivenElement(nodeCommon, "list", "", "", "");

    // header
    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeCommon, "header", "", "", "");
    std::string strCount = "5";

    VoiceVector<NodeInfo>::const_iterator itor = vecActionParameter.cbegin();
    for (; itor != vecActionParameter.cend(); ++itor) {
        if ("List Count" == itor->strName) {
            strCount = itor->strValue;
            break;
        }
    }

    std::string strStartIndex = GetStartIndex(pcMsg);
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "startIndex", strStartIndex, "", "");
    xmlBulder.buildGivenElement(nodeHeader, "count", strCount, "", "");

    // items
    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeCommon, "items", "", "", "");
    pugi::xml_node nodeItem;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        nodeItem = xmlBulder.buildGivenElement(nodeItems, "item", "", "", "");
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("App" == itor->strName) {
                xmlBulder.buildGivenElement(nodeItem, "appName", itor->strValue, "", "");
            }
            else if ("App ID" == itor->strName) {
                xmlBulder.buildGivenElement(nodeItem, "appId", itor->strValue, "", "");
            }
        }
    }

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.GetPromptByScreenId("VR-APP-01", m_strPrompt);
    VR_LOG("ListApps:  strReply = %s \n", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::NBest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    ListNodeInfo listNodeInfo;
    if (!GetListParameter(pcMsg, listNodeInfo)) {
        return false;
    }
    
    // ReStartRecoSession();

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node nodeCommon = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(nodeCommon, "agent", "apps", "", "");
    xmlBulder.buildGivenElement(nodeCommon, "content", "apps_app_candidates_select", "", "");
    nodeCommon = xmlBulder.buildGivenElement(nodeCommon, "selectList", "", "", "");
    nodeCommon = xmlBulder.buildGivenElement(nodeCommon, "list", "", "", "");

    // header
    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeCommon, "header", "", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "startIndex", "0", "", "");

    int iCount = listNodeInfo.vecItems.size();
    iCount = (iCount > 5) ? 5 : iCount;
    xmlBulder.buildGivenElement(nodeHeader, "count", std::to_string(iCount), "", "");

    // items
    pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeCommon, "items", "", "", "");
    pugi::xml_node nodeItem;
    VoiceVector<NodeInfo>::const_iterator itor;
    VoiceVector<VoiceVector<NodeInfo>::type>::const_iterator itorItem = listNodeInfo.vecItems.cbegin();
    for (; itorItem != listNodeInfo.vecItems.cend(); ++itorItem) {
        nodeItem = xmlBulder.buildGivenElement(nodeItems, "item", "", "", "");
        for (itor = itorItem->cbegin(); itor != itorItem->cend(); ++itor) {
            if ("App" == itor->strName) {
                xmlBulder.buildGivenElement(nodeItem, "appName", itor->strValue, "", "");
            }
            else if ("App ID" == itor->strName) {
                xmlBulder.buildGivenElement(nodeItem, "appId", itor->strValue, "", "");
            }
        }
    }
    
    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("NBest:  strReply = %s \n", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppNotInstalled(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppsNotAvailable(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstVO61002);
    return true;
}

bool
VR_VoiceBoxAgentApps_AU::NoAppsAvailable(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    ShowPopupMsg(cstVO61001);
    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppBack(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_engineCallback.OnRequest("<send event=\"back-internal\" target=\"de\"/>");
    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppCancel(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"cancel\" target=\"dm\"/>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppStartOver(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"startover-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppStartPause(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"pause-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::AppStartRepeat(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::QueryAppStatus(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    if (m_engineCallback.IsAppsInstalled()) {
        spParams->AddParameter("Installed", "True", "", "");
    }
    else {
        spParams->AddParameter("Installed", "False", "", "");
    }

    if (m_engineCallback.IsAppsEnabled()) {
        spParams->AddParameter("Enabled", "True", "", "");
    }
    else {
        spParams->AddParameter("Enabled", "False", "", "");
    }

    HRESULT retCode = m_engineCommand.DriverActionReply("Apps", "TSL App Status Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::ProcessMessage(
    const std::string& message
)
{
    if (std::string::npos != message.find("TSLINFO")) {
        VR_VoiceBoxXmlParser parser(message);
        std::string strActionType = parser.getValueByKey("type");
        if ("Install Agent" == strActionType) {
            bool bInstall = DoInstall(parser);
            m_vecTSLInstallInfo.clear();
            std::string strInstallFormat = "<event-result name=\"install\" errcode=\"0\">"
                                             "<TSLINFO>"
                                               "<Action type=\"Installed Result\">"
                                                 "<Parameter name=\"Result\" value=\"%s\"/>"
                                               "</Action>"
                                             "</TSLINFO>"
                                           "</event-result>";
            const int nInstallFormatLen = 256;
            char result[nInstallFormatLen] = { 0 };
            snprintf(result, sizeof(result), strInstallFormat.c_str(), (bInstall? "True" : "False"));
            m_engineCallback.OnRecognized(result);
        }
        else if ("Uninstall Agent" == strActionType) {
            bool bUninstall = DoUninstall(parser);
            m_vecTSLUninstallInfo.clear();
            std::string strUninstallFormat = "<event-result name=\"uninstall\" errcode=\"0\">"
                                               "<TSLINFO>"
                                                 "<Action type=\"Uninstalled Result\">"
                                                   "<Parameter name=\"Result\" value=\"%s\"/>"
                                                 "</Action>"
                                               "</TSLINFO>"
                                             "</event-result>";
            const int nUninstallFormatLen = 256;
            char result[nUninstallFormatLen] = { 0 };
            snprintf(result, sizeof(result), strUninstallFormat.c_str(), (bUninstall? "True" : "False"));
            m_engineCallback.OnRecognized(result);
        }
        else if ("Uninstall All Agent" == strActionType) {
            bool bUninstallAll = DoUninstallAll();
            std::string strUninstallFormat = "<event-result name=\"uninstall\" errcode=\"0\">"
                                               "<TSLINFO>"
                                                 "<Action type=\"Uninstalled Result\">"
                                                   "<Parameter name=\"Result\" value=\"%s\"/>"
                                                 "</Action>"
                                               "</TSLINFO>"
                                             "</event-result>";
            const int nUninstallAllFormatLen = 256;
            char result[nUninstallAllFormatLen] = { 0 };
            snprintf(result, sizeof(result), strUninstallFormat.c_str(), (bUninstallAll? "True" : "False"));
            m_engineCallback.OnRecognized(result);
        }
        else if ("Get Supported Language" == strActionType) {
            std::string strSupportedLanguage = "<event-result name=\"getSupportedLanguage\" errcode=\"0\">"
                                                  "<TSLINFO>"
                                                    "<Action type=\"Get Supported Language\"/>"
                                                      "<LanguageList>"
                                                        "<Item>"
                                                          "<Parameter name=\"Language\" value=\"en\"/>"
                                                        "</Item>"
                                                        "<Item>"
                                                          "<Parameter name=\"Language\" value=\"fr\"/>"
                                                        "</Item>"
                                                        "<Item>"
                                                          "<Parameter name=\"Language\" value=\"es\"/>"
                                                        "</Item>"
                                                      "</LanguageList>"
                                                  "</TSLINFO>"
                                                "</event-result>";
            m_engineCallback.OnRecognized(strSupportedLanguage);
        }
        else {
            // do nothing
            return false;
        }
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::DoInstall(VR_VoiceBoxXmlParser& parser)
{
    // Install TSL agent
    VR_LOG("Install TSL agents");

    VoiceVector<Parameter>::type parameterVector = parser.getMsgParameterValue();
    if ("Uninstall Previous Agent" == parameterVector[0].nameValue) {
        if ("True" == parameterVector[0].value) {
            VR_LOG("Uninstall Previous agents");
            DoUninstallAll();
        }
        else {
            VR_LOG("Donot uninstall Previous agents");
        }
    }

    for (int i = 1; i + 1 < parameterVector.size(); i += 2) {
        TSLAUInstallInfo info;
        if ("Path" == parameterVector[i].nameValue) {
            info.path = parameterVector[i].value;
            VR_LOG("Path: %s", info.path.c_str());
        }

        if ("Language" == parameterVector[i + 1].nameValue) {
            info.culture = GetCulture(parameterVector[i + 1].value);
            VR_LOG("Language: %s", info.culture.c_str());
        }

        m_vecTSLInstallInfo.push_back(info);
    }

    if (m_vecTSLInstallInfo.empty()) {
        return true;
    }

    const int nProgressFormatLen = 256;
    std::string strProgressFormat = "<action name=\"notifyInstallProgress\">"
                                      "<TSLINFO>"
                                        "<Action type=\"Install Progress\">"
                                          "<Parameter name=\"Path\" value=\"%s\"/>"
                                          "<Parameter name=\"Index\" value=\"%d\"/>"
                                        "</Action>"
                                      "</TSLINFO>"
                                    "</action>";

    for (int i = 0; i < m_vecTSLInstallInfo.size(); ++i) {
        // Prepare the agent file path
        CVECIPtr<IVECIStringSet> spDAPFile;
        m_client.CreateStringSet(&spDAPFile);
        spDAPFile->AddItem(m_vecTSLInstallInfo[i].path.c_str());

        // Install the agent
        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand.InstallDownloadableAgent(
            &transaction,
            NULL,
            _T("/tmp/"),
            m_vecTSLInstallInfo[i].culture.c_str(),
            spDAPFile
        );
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("Install Failed");
            return false;
        }

        // Make it a sync operation
        VBT_BOOL bTimeout = VBT_FALSE;
        result = transaction->WaitForCompletion(INFINITE, &bTimeout);
        if (FAILED(result) || bTimeout) {
            VR_LOG("Install Wait Failed");
            return false;
        }

        char progress[nProgressFormatLen] = { 0 };
        snprintf(progress, sizeof(progress), strProgressFormat.c_str(), m_vecTSLInstallInfo[i].path.c_str(), i);
        m_engineCallback.OnRecognized(progress);
        VR_LOG("Install %s Successfully", m_vecTSLInstallInfo[i].path.c_str());
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::DoUninstall(VR_VoiceBoxXmlParser& parser)
{
    // Uninstall TSL agent
    VR_LOG("Uninstall TSL agents");
    VoiceVector<Parameter>::type parameterVector = parser.getMsgParameterValue();
    for (int i = 0; i + 1 < parameterVector.size(); i += 2) {
        TSLAUUninstallInfo info;
        if ("Agent" == parameterVector[i].nameValue) {
            info.name = parameterVector[i].value;
            VR_LOG("Agent: %s", info.name.c_str());
        }
        if ("Language" == parameterVector[i + 1].nameValue) {
            info.culture = GetCulture(parameterVector[i + 1].value);
            VR_LOG("Language: %s", info.culture.c_str());
        }

        m_vecTSLUninstallInfo.push_back(info);
    }

    if (m_vecTSLUninstallInfo.empty()) {
        return true;
    }

    for (int i = 0; i < m_vecTSLUninstallInfo.size(); ++i) {
        // Prepare for agent name
        CVECIPtr<IVECIStringSet> spAgentName;
        m_client.CreateStringSet(&spAgentName);
        spAgentName->AddItem(m_vecTSLUninstallInfo[i].name.c_str());

        CVECIPtr<IVECITransaction> transaction;
        HRESULT result = m_engineCommand.UninstallDownloadableAgent(
            &transaction,
            NULL,
            m_vecTSLUninstallInfo[i].culture.c_str(),
            spAgentName
        );
        if (FAILED(result) || (NULL == transaction.ptr())) {
            VR_LOG("Uninstall Failed");
            return false;
        }

        VBT_BOOL bTimeout = VBT_FALSE;
        result = transaction->WaitForCompletion(INFINITE, &bTimeout);
        if (FAILED(result) || bTimeout) {
            VR_LOG("Uninstall Wait Failed");
            return false;
        }

        VR_LOG("Uninstall %s Successfully", m_vecTSLUninstallInfo[i].name.c_str());
    }

    return true;
}

bool
VR_VoiceBoxAgentApps_AU::DoUninstallAll()
{
    // Uninstall all TSL agent
    VR_LOG("Uninstall all TSL agents");
    CVECIPtr<IVECITransaction> transaction;
    HRESULT result = m_engineCommand.UninstallAllDownloadableAgent(
        &transaction,
        NULL,
        NULL
    );
    if (FAILED(result) || (NULL == transaction.ptr())) {
        VR_LOG("Uninstall all Failed");
        return false;
    }

    // Make it a sync operation
    VBT_BOOL bTimeout = VBT_FALSE;
    result = transaction->WaitForCompletion(INFINITE, &bTimeout);
    if (FAILED(result) || bTimeout) {
        VR_LOG("Uninstall all Wait Failed");
        return false;
    }

    VR_LOG("Uninstall all Successfully");
    return true;
}

std::string
VR_VoiceBoxAgentApps_AU::GetCulture(std::string& language)
{
    std::string culture;
    if ("en" == language) {
        culture = "en-US";
    }
    else if ("fr" == language) {
        culture = "fr-CA";
    }
    else if ("es" == language) {
        culture = "en-MX";
    }
    else {
        // do nothing
    }

    return culture;
}

/* EOF */
