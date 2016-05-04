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

#include <stdio.h> // temp header
#include <string.h> // temp header
#include <sstream>
/* VBT Header */
// #ifndef VECIOUTSTR_H
// #    include "VECIOutStr.h"
// #endif

#ifndef VECICSTR_H
#    include "VECICStr.h"
#endif

#include <cstdlib>

// #ifndef VBT_EXTERNAL_ERRORS_H
// #    include "VBTExternalErrors.h"
// #endif

#ifndef IVBTENGINECLIENT_H
#    include "IVBTEngineClient.h"
#endif

#ifndef VBT_SDK_AGENT_H
#    include "VBT_SDK_Agent.h"
#endif

#ifndef VBT_SDK_ACTION_TYPES_H
#    include "VBT_SDK_Action_Types.h"
#endif

#ifndef VBT_SDK_LIST_TYPES_H
#    include "VBT_SDK_List_Types.h"
#endif

#ifndef VBT_SDK_COMMANDS_H
#    include "VBT_SDK_Commands.h"
#endif

/* Suntec Header */
#ifndef VR_VoiceBoxAgentPhone_AU_H
#    include "VR_VoiceBoxAgentPhone_AU.h"
#endif

#ifndef VR_VOICEBOXLOG_H
#    include "VR_VoiceBoxLog.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

#ifndef VR_VOICEBOXXMLNODESTRING_H
#    include "VR_VoiceBoxXmlNodeString.h"
#endif

#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

using namespace nutshell;

// PhoneActionHandler
struct PhoneActionHandler
{
    char*             szAction; // Action type
    PhoneMsgHandler_AU  handler;  // Handler pointer
};

const std::string cstVO41001 = "There are no phones set up to make calls. To use the handsfree system, connect a phone.";
const std::string cstVO41002 = "The phone is not connected. Check the phone.";
const std::string cstVO41003 = "There are no phones set up to make calls. To use the handsfree system, connect a phone.";
const std::string cstVO41004 = "The phone is not connected. Check the phone.";
const std::string cstVO41005 = "You have no recent calls.";
const std::string cstVO41006 = "There are no phones set up to make calls. To use the handsfree system, connect a phone.";
const std::string cstVO41007 = "The phone is not connected. Check the phone.";
const std::string cstVO41008 = "The contact list for voice recognition is not available. Check the phone.";
const std::string cstVO41009 = "Your phonebook is being prepared for use by voice recognition... Try again later.";
const std::string cstVO41010 = "There are no phones set up to make calls. To use the handsfree system, connect a phone.";
const std::string cstVO41011 = "The phone is not connected. Check the phone.";
const std::string cstVO41012 = "Outgoing call history list is empty. Redial is not available.";
const std::string cstVO41013 = "Incoming call history list empty. Callback is not available.";
const std::string cstVO41014 = "The call cannot be placed. There is no phone number information.";

// Constructor
VR_VoiceBoxAgentPhone_AU::VR_VoiceBoxAgentPhone_AU(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
    m_listNumber = 0;
}

// Destructor
VR_VoiceBoxAgentPhone_AU::~VR_VoiceBoxAgentPhone_AU()
{
}

bool
VR_VoiceBoxAgentPhone_AU::Initialize()
{
    const PhoneActionHandler actionHandlerMap[] = {
        { VBT_LIST_TYPE_HFD_NBEST,                                               // "n-Best"
            &VR_VoiceBoxAgentPhone_AU::HFDNBest },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS,                                     // "Append Digits"
            &VR_VoiceBoxAgentPhone_AU::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS_ALREADY_AT_MAX,                      // "Append Digits Already At Max"
            &VR_VoiceBoxAgentPhone_AU::AppendDigitsAlreadyAtMax },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS_MAX_REACHED,                         // "Append Digits Max reached"
            &VR_VoiceBoxAgentPhone_AU::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_BUSY_UPDATING_GRAMMAR,                             // "Busy Updating Grammar"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { "Call Back",                                                           // "Call Back"
            &VR_VoiceBoxAgentPhone_AU::CallNumber },
        { VBT_ACTION_TYPE_HFD_CALL_BACK_CONFIRMED,                               // "Call Back Confirmed"
            &VR_VoiceBoxAgentPhone_AU::CallBackConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT,                                      // "Call Contact"
            &VR_VoiceBoxAgentPhone_AU::CallContact }, // VR-HFD-03
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_CATCH,                                // "Call Contact Catch"
            &VR_VoiceBoxAgentPhone_AU::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_CONFIRMED,                            // "Call Contact Confirmed"
            &VR_VoiceBoxAgentPhone_AU::CallContactConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_REJECTED,                             // "Call Contact Rejected"
            &VR_VoiceBoxAgentPhone_AU::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER,                                       // "Call Number"
            &VR_VoiceBoxAgentPhone_AU::CallNumber },
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_CATCH,                                 // "Call Number Catch"
            &VR_VoiceBoxAgentPhone_AU::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_CONFIRMED,                             // "Call Number Confirmed"
            &VR_VoiceBoxAgentPhone_AU::CallNumberConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_REJECTED,                              // "Call Number Rejected"
            &VR_VoiceBoxAgentPhone_AU::DigitsCorrection }, // VR-HFD-12
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_WITH_BLOCK_DIALING,                    // "Call Number With Block Dialing"
            &VR_VoiceBoxAgentPhone_AU::DigitsCorrection },
        { "Call Recent Number",                                                  // "Call Recent Number"
            &VR_VoiceBoxAgentPhone_AU::CallRecentNumber },
        { VBT_ACTION_TYPE_HFD_DIGITS_CORRECTION,                                 // "Digits Correction"
            &VR_VoiceBoxAgentPhone_AU::DigitsCorrection }, // VR-HFD-12
        { VBT_ACTION_TYPE_HFD_DIGITS_CORRECTION_NO_MORE_DIGITS,                  // "Digits Correction No More Digits"
            &VR_VoiceBoxAgentPhone_AU::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_HFD_HOME,                                          // "HFD Home"
            &VR_VoiceBoxAgentPhone_AU::HFDHome },
        { "HFD Menu",                                     // "HFD Menu"
            &VR_VoiceBoxAgentPhone_AU::HFDMenu },
        { VBT_ACTION_TYPE_HFD_LIST_PARTIAL_NAME_MATCHES,                         // "List Partial Name Matches"
            &VR_VoiceBoxAgentPhone_AU::ListPartialNameMatches }, // VR-HFD-05
        { VBT_ACTION_TYPE_HFD_LIST_PHONE_TYPES,                                  // "List Phone Types"
            &VR_VoiceBoxAgentPhone_AU::ListPhoneTypes },
        { VBT_ACTION_TYPE_HFD_LIST_RECENT_CALLS,                                 // "List Recent Calls"
            &VR_VoiceBoxAgentPhone_AU::ListRecentCalls },
        { VBT_ACTION_TYPE_HFD_NO_CALL_BACK_NUMBER,                               // "No Call Back Number"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_CONTACT_NUMBERS,                                // "No Contact Numbers"
            &VR_VoiceBoxAgentPhone_AU::NoContactNumbers },
        { VBT_ACTION_TYPE_HFD_NO_CONTACTS_FOUND,                                 // "No Contacts Found"
            &VR_VoiceBoxAgentPhone_AU::NoContactsFound },
        { VBT_ACTION_TYPE_HFD_NO_PHONE_CONNECTED,                                // "No Phone Connected"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { "No Phone Registered",                                                 // "No Phone Registered"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_RECENT_CALLS,                                   // "No Recent Calls"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_REDIAL_NUMBER,                                  // "No Redial Number"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { "Outgoing Call History Empty",                                         // "Outgoing Call History Empty"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { "Incoming Call History Empty",                                         // "Incoming Call History Empty"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { "Redial",                                                              // "Redial"
            &VR_VoiceBoxAgentPhone_AU::CallNumber },
        { VBT_ACTION_TYPE_HFD_SEND_DTMF,                                         // "Send DTMF"
            &VR_VoiceBoxAgentPhone_AU::SendDTMF },
        { VBT_ACTION_TYPE_HFD_SEND_DTMF_CATCH,                                   // "Send DTMF Catch"
            &VR_VoiceBoxAgentPhone_AU::SendDTMFCatch }, // VR-HFD-14
        { "Send DTMF Confirmed",                                                 // "Send DTMF Confirmed"
            &VR_VoiceBoxAgentPhone_AU::SendDTMFConfirmed },
        { "Send DTMF Rejected",                                                 // "Send DTMF Rejected"
            &VR_VoiceBoxAgentPhone_AU::SendDTMFRejected },
        { VBT_ACTION_TYPE_HFD_VBT_ERROR,                                         // "VBT Error"
            &VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_BACK,                                              // "Back"
            &VR_VoiceBoxAgentPhone_AU::PhoneBack },
        { VBT_ACTION_TYPE_HFD_CANCEL, // "Cancel"
            &VR_VoiceBoxAgentPhone_AU::PhoneCancel },
        { VBT_ACTION_TYPE_HFD_HELP, // "Help"
            &VR_VoiceBoxAgentPhone_AU::PhoneHelp },
        { VBT_ACTION_TYPE_HFD_START_OVER, // "Start Over"
            &VR_VoiceBoxAgentPhone_AU::PhoneStartOver },
        { VBT_ACTION_TYPE_HFD_SYSTEM_PAUSE, // "Start Pause"
            &VR_VoiceBoxAgentPhone_AU::PhoneStartPause },
        { VBT_ACTION_TYPE_HFD_SYSTEM_REPEAT, // "Start Repeat"
            &VR_VoiceBoxAgentPhone_AU::PhoneStartRepeat },
        // Info Driver
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_INCOMING_CALL_INFO_REQUEST,         // "Incoming Call Info Request"
            &VR_VoiceBoxAgentPhone_AU::IncomingCallInfoRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_OUTGOING_CALL_INFO_REQUEST,         // "Outgoing Call Info Request"
            &VR_VoiceBoxAgentPhone_AU::OutgoingCallInfoRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_RECENT_CALLS_REQUEST,               // "Recent Calls Request"
            &VR_VoiceBoxAgentPhone_AU::RecentCallsRequest },
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_PhoneMsgHandler_AU.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }
    m_PhoneActionResultHandler_AU.insert(std::make_pair("queryIncomingCall", &VR_VoiceBoxAgentPhone_AU::IncomingCallInfoReply));
    m_PhoneActionResultHandler_AU.insert(std::make_pair("queryOutgoingCall", &VR_VoiceBoxAgentPhone_AU::OutgoingCallInfoReply));
    m_PhoneActionResultHandler_AU.insert(std::make_pair("queryRecentCallList", &VR_VoiceBoxAgentPhone_AU::RecentCallsReply));

    return true;
}

void
VR_VoiceBoxAgentPhone_AU::UnInitialize()
{
    m_PhoneMsgHandler_AU.clear();
    m_PhoneActionResultHandler_AU.clear();
    m_messageInfoVector.clear();
}

bool
VR_VoiceBoxAgentPhone_AU::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser)
{
    std::string strActionKey = parser.getValueByKey("op");
    // Dispatch the messages
    VoiceMap<std::string, PhoneActionResultHandler_AU>::const_iterator iterMap = m_PhoneActionResultHandler_AU.find(strActionKey);
    if (m_PhoneActionResultHandler_AU.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }

    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentPhone_AU::QueryAgentSupport(
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
    if (strAgent.IsEqual(VBT_AGENT_HFD)) {
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
VR_VoiceBoxAgentPhone_AU::OnAgentResultMessage(
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
VR_VoiceBoxAgentPhone_AU::OnAgentDriverMessage(
    IVECIParsedMessage* pDriverMsg
)
{
    if (NULL == pDriverMsg) {
        return E_FAIL;
    }

    if (!ProcessAgentMessage(pDriverMsg)) {
        return E_FAIL;
    }

    return S_OK;
}

bool
VR_VoiceBoxAgentPhone_AU::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
{
    VR_LOGP("DE: receive VBT processing data, will run operation about phone...case : 212-119 212-125");


    if (NULL == pcMsg) {
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

        return false;

    } while (false);

    VR_LOG("ProcessAgentMessage: %s", strAgentResult.c_str());
    m_engineCallback.setActionResultValue(strAgentResult);

    // Dispatch the messages
    VoiceMap<std::string, PhoneMsgHandler_AU>::const_iterator iterMap = m_PhoneMsgHandler_AU.find(strAgentResult);
    if (m_PhoneMsgHandler_AU.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return false;
}

bool
VR_VoiceBoxAgentPhone_AU::HFDNBest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("HFDNBest: %s", xmlMessage.Get());

    bool nameFlag = false;
    bool numberFlag = false;
    bool digitsFlag = false;
    VBT_ULONG uSize = 0;
    (void)pcMsg->GetListSize(&uSize);
    VoiceVector<StructNode>::type attributeVector;
    for (VBT_ULONG i = 0; i < uSize; ++i) {
        StructNode node;
        CVECIOutStr strContact;
        CVECIOutStr strPhoneNumber;
        CVECIOutStr strDigits;
        (void)pcMsg->GetListItemParameterValue(i, "Contact Name", &strContact);
        (void)pcMsg->GetListItemParameterValue(i, "Phone Number", &strPhoneNumber);
        (void)pcMsg->GetListItemParameterValue(i, "Digits", &strDigits);

        if (NULL != strContact.Get()) {
            node.Name = "name";
            node.Value = strContact.Get();
            nameFlag = true;
        }
        else if (NULL != strPhoneNumber.Get()) {
            node.Name = "number";
            node.Value = strPhoneNumber.Get();
            numberFlag = true;
        }
        else if (NULL != strDigits.Get()) {
            node.Name = "digits";
            node.Value = strDigits.Get();
            digitsFlag = true;
        }
        attributeVector.push_back(node);

    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");

    std::string strContent;
    if (nameFlag) {
        strContent = "phone_contact_select";
    }
    else if (numberFlag) {
        strContent = "phone_contact_select_number";
    }
    else if (digitsFlag) {
        strContent = "phone_contact_select_digits";
    }

    xmlBulder.buildGivenElement(xmlNode, "content", strContent, "", "");
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");
    pugi::xml_node listNode = xmlBulder.buildGivenElement(selectListNode, "list", "", "", "");
    pugi::xml_node headerNode = xmlBulder.buildGivenElement(listNode, "header", "", "", "");
    xmlBulder.buildGivenElement(headerNode, "pageSize", "5", "", "");
    xmlBulder.buildGivenElement(headerNode, "startIndex", "0", "", "");

    int iCount = attributeVector.size();
    iCount = (iCount > 5) ? 5 : iCount;
    xmlBulder.buildGivenElement(headerNode, "count", std::to_string(iCount), "", "");

    pugi::xml_node itemsNode = xmlBulder.buildGivenElement(listNode, "items", "", "", "");
    xmlBulder.buildListItemChildElement(itemsNode, "nbestphoneinfo", nBestDigits, attributeVector);

    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::AppendDigitsAlreadyAtMax(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("AppendDigitsAlreadyAtMax-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::PhoneTaskComplete(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    std::string strResult = "";
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        strResult = strActionType.Get();
    }
    VR_LOG("PhoneTaskComplete: %s", strResult.c_str());

    std::string strPrompt;
    if ("No Phone Registered" == strResult) {
        strPrompt = cstVO41001;
    }
    else if (VBT_ACTION_TYPE_HFD_NO_PHONE_CONNECTED == strResult) {
        strPrompt = cstVO41002;
    }
    else if (VBT_ACTION_TYPE_HFD_NO_RECENT_CALLS == strResult) {
        strPrompt = cstVO41005;
    }
    else if (VBT_ACTION_TYPE_HFD_NO_CONTACTS_FOUND == strResult) {
        strPrompt = cstVO41008;
    }
    else if ("Outgoing Call History Empty" == strResult) {
        strPrompt = cstVO41012;
    }
    else if ("Incoming Call History Empty" == strResult) {
        strPrompt = cstVO41013;
    }
    else if (VBT_ACTION_TYPE_HFD_NO_REDIAL_NUMBER == strResult) {
        strPrompt = cstVO41014;
    }
    else if (VBT_ACTION_TYPE_HFD_NO_CALL_BACK_NUMBER == strResult) {
        strPrompt = cstVO41014;
    }
    else {
        VR_LOG("no message popup");
        return false;
    }
    ShowPopupMsg(strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallBackConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");

    xmlBulder.buildGivenElement(node, "dialType", "REDIAL", "", "");
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "phoneInfo", "", "", "");

    xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", m_strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", m_strPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;

}

bool
VR_VoiceBoxAgentPhone_AU::CallContact(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_confirm_call_contact", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneInfo", "", "", "");

    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneType);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);

    std::string tempPhoneType;
    if ((NULL != strPhoneNumber.Get()) && (NULL != strContactID.Get()) && (NULL != strPhoneType.Get())) {
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        tempPhoneType = phoneData.GetPhoneType(strContactID.Get(), strPhoneNumber.Get(), strPhoneType.Get(), strPath);
    }
    if ("" == tempPhoneType) {
        tempPhoneType = strPhoneType.Get();
    }
    VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());

    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "contactName", strContactName.Get(), "", "");
    }
    xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", tempPhoneType.c_str(), "", "");

    m_engineCallback.GetPromptByScreenId("VR-HFD-03", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallRecentNumber(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_confirm_call_contact", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneInfo", "", "", "");
    xmlBulder.buildGivenElement(phoneInfoNode, "number", m_strPhoneNumber.c_str(), "", "");
    xmlBulder.buildGivenElement(phoneInfoNode, "contactName", m_strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", m_strPhoneType.c_str(), "", "");

    m_engineCallback.GetPromptByScreenId("VR-HFD-03", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallContactCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_speak_contact", "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    m_engineCallback.GetPromptByScreenId("VR-HFD-01", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallContactConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    VR_LOGP("DE: start running operation... call contact directly .... case : 212-119-99 212-123-99 212-125-99");


    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallContactConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneType);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);

     std::string tempPhoneType;
    if ((NULL != strPhoneNumber.Get()) && (NULL != strContactID.Get()) && (NULL != strPhoneType.Get())) {
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        tempPhoneType = phoneData.GetPhoneType(strContactID.Get(), strPhoneNumber.Get(), strPhoneType.Get(), strPath);
    }
    if ("" == tempPhoneType) {
        tempPhoneType = strPhoneType.Get();
    }
    VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());
    xmlBulder.buildGivenElement(node, "dialType", "CALL", "", "");
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "phoneInfo", "", "", "");
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", strContactName.Get(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", "", "", "");
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", "", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", tempPhoneType.c_str(), "", "");
    if (NULL != strContactID.Get()) {
        std::string tempContactId = strlen(strContactID.Get()) ?  strContactID.Get() : "0";
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", tempContactId.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallNumber(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_confirm_call_number", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneInfo", "", "", "");

    CVECIOutStr strPhoneNumber;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);

    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", m_strPhoneNumber.c_str(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-06", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallNumberCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_speak_phone_number", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    m_engineCallback.GetPromptByScreenId("VR-HFD-02", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::CallNumberConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallNumberConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);

    xmlBulder.buildGivenElement(node, "dialType", "CALL_NUMBER", "", "");
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "phoneInfo", "", "", "");
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", strContactName.Get(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", "", "", "");
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", "", "", "");
    }
    if ("" != m_strContactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", m_strContactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    VR_LOG("m_strPhoneType :%s", m_strPhoneType.c_str());
    if ("" == m_strPhoneType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", "-1", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", m_strPhoneType.c_str(), "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::DigitsCorrection(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    m_strPhoneType.clear();
    m_strContactID.clear();
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_speak_digits", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneInfo", "", "", "");

    CVECIOutStr strPhoneNumber;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-12", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::HFDHome(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_engineCallback.GetPromptByScreenId("VR-HNT-01", m_strPrompt);

    m_engineCallback.OnRequest("<event name=\"startNextAgent\"><agent>phone</agent></event>");

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::HFDMenu(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"showHintScreen\"><agent>phone</agent></event>");

    return true;
}

void
VR_VoiceBoxAgentPhone_AU::BuildListHeader(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& nodeList)
{
    if (NULL == pcMsg) {
        return;
    }

    // CVECIOutStr strCurrentPageIndex;
    // (void)pcMsg->GetActionParameterValue("Current Page Index", &strCurrentPageIndex);
    CVECIOutStr strListCount;
    (void)pcMsg->GetActionParameterValue("List Count", &strListCount);

    pugi::xml_node nodeHeader = xmlBulder.buildGivenElement(nodeList, "header", "", "", "");

    xmlBulder.buildGivenElement(nodeHeader, "startIndex", "0", "", "");
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");

    if (NULL != strListCount.Get()) {
        xmlBulder.buildGivenElement(nodeHeader, "count", strListCount.Get(), "", "");
    }

    return;
}

bool
VR_VoiceBoxAgentPhone_AU::ListPartialNameMatches(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ListPartialNameMatches: %s", xmlMessage.Get());

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    VBT_ULONG uSize = 0;

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_contact_select", "", "");
    m_engineCallback.GetPromptByScreenId("VR-SYS-01", m_strPrompt);
    m_engineCallback.SetNBestScreenFlg(false);
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    if (NULL != strListType.Get()) {
        pcMsg->GetListSize(&uSize);
        pugi::xml_node nodeList = xmlBulder.buildGivenElement(selectListNode, "list", "", "", "");

        BuildListHeader(pcMsg, xmlBulder, nodeList);

        pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

        VoiceVector<StructNode>::type attributeVector;
        StructNode node;
        CVECIOutStr strContactName;
        CVECIOutStr strContactID;

        for (VBT_ULONG i = 0; i < uSize; ++i) {
            attributeVector.clear();

            pcMsg->GetListItemParameterValue(i, "Contact Name", &strContactName);
            pcMsg->GetListItemParameterValue(i, "Contact ID", &strContactID);

            node.Name = "name";
            if (NULL != strContactName.Get()) {
                node.Value = strContactName.Get();
            }
            attributeVector.push_back(node);

            node.Name = "contactID";
            if (NULL != strContactID.Get()) {
                node.Value = strContactID.Get();
            }
            attributeVector.push_back(node);

            xmlBulder.buildListItemChildElement(nodeItems, "nameMatchInfo", nameMatchInfo, attributeVector);
        }
    }

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::ListPhoneTypes(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ListPhoneTypes: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_contact_type_select", "", "");
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");
    std::string name;
    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);

    if (NULL != strListType.Get()) {
        pugi::xml_node nodeList = xmlBulder.buildGivenElement(selectListNode, "list", "", "", "");

        BuildListHeader(pcMsg, xmlBulder, nodeList);

        pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");
        VBT_ULONG uSize = 0;
        pcMsg->GetListSize(&uSize);
        for (VBT_ULONG i = 0; i < uSize; ++i) {
            VoiceVector<StructNode>::type attributeVector;
            StructNode node;
            CVECIOutStr strPhoneType;
            CVECIOutStr strPhoneNumber;
            CVECIOutStr strContactName;
            CVECIOutStr strContactID;

            pcMsg->GetListItemParameterValue(i, "Phone Type ID", &strPhoneType);
            pcMsg->GetListItemParameterValue(i, "Phone Number", &strPhoneNumber);
            pcMsg->GetListItemParameterValue(i, "Contact Name", &strContactName);
            pcMsg->GetListItemParameterValue(i, "Contact ID", &strContactID);

            std::string tempPhoneType;
            if ((NULL != strPhoneNumber.Get()) && (NULL != strContactID.Get()) && (NULL != strPhoneType.Get())) {
                std::string strPath = m_engineCallback.GetPhonBookDBPath();
                VR_VoiceBoxPhoneData phoneData;
                tempPhoneType = phoneData.GetPhoneType(strContactID.Get(), strPhoneNumber.Get(), strPhoneType.Get(), strPath);
            }
            if ("" == tempPhoneType) {
                tempPhoneType = strPhoneType.Get();
            }
            VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());

            node.Name = "phoneType";
            node.Value = tempPhoneType;
            VR_LOG("node.Value: %s", node.Value.c_str());
            attributeVector.push_back(node);

            node.Name = "number";
            if (NULL != strPhoneNumber.Get()) {
                node.Value = strPhoneNumber.Get();
            }
            attributeVector.push_back(node);

            node.Name = "contactName";
            if (NULL != strContactName.Get()) {
                node.Value = strContactName.Get();
                name = strContactName.Get();
            }
            attributeVector.push_back(node);

            xmlBulder.buildListItemChildElement(nodeItems, "phoneInfo", phoneTypeInfo, attributeVector);
        }
    }
    xmlBulder.buildGivenElement(xmlNode, "prompt", name, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-18", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::ListRecentCalls(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ListRecentCalls: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    xmlBulder.buildStartActionElement("phone", "showRecentCallList");
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ListRecentCalls: %s", strReply.c_str());

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::NoContactNumbers(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("NoContactNumbers-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::NoContactsFound(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("NoContactsFound-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::SendDTMF(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // std::string strResult = "";
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        // strResult = strActionType.Get();
    }
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "sendtone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "sendtone_confirm_tones", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");

    CVECIOutStr strDigits;
    pcMsg->GetActionParameterValue("Digits", &strDigits);
    if (NULL != strDigits.Get()) {
        xmlBulder.buildGivenElement(detailNode, "tones", strDigits.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::SendDTMFCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_send_dtmf", "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::SendDTMFConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    // std::string strResult = "";
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        // strResult = strActionType.Get();
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "sendDTMFTones");

    CVECIOutStr strDigits;
    pcMsg->GetActionParameterValue("Digits", &strDigits);
    if (NULL != strDigits.Get()) {
        xmlBulder.buildGivenElement(node, "tones", strDigits.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::SendDTMFRejected(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "sendtone_speak_tones", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::IncomingCallInfoRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryIncomingCall");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::OutgoingCallInfoRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
   VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryOutgoingCall");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::RecentCallsRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryRecentCallList");
    xmlBulder.buildGivenElement(node, "count", "30", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::IncomingCallInfoReply(
    VR_VoiceBoxXmlParser& parser
)
{
    m_strDialType.clear();
    m_strContactName.clear();
    m_strPhoneNumber.clear();
    m_strPhoneType.clear();

    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strIncommingCallType = parser.getValueByKey("errcode");
    m_strDialType = strIncommingCallType;

    if ("0" == strIncommingCallType) {
        VR_LOG("strIncommingCallType : %s", strIncommingCallType.c_str());
        std::string strContactName = parser.getValueByKey("contactName");
        if ("''" == strContactName) {
            strContactName = "";
        }
        std::string strNumber = parser.getValueByKey("number");
        if ("''" == strNumber) {
            strNumber = "";
        }
        std::string strContactID = parser.getValueByKey("contactId");
        if ("''" == strContactID) {
            strContactID = "";
        }
        std::string strPhoneType = parser.getValueByKey("phoneType");
        if ("''" == strPhoneType) {
            strPhoneType = "";
        }

        m_strPhoneType = strPhoneType;

        spParams->AddParameter("Contact ID", strContactID.c_str(), "", "");
        spParams->AddParameter("Contact Name", strContactName.c_str(), "", "");
        spParams->AddParameter("Phone Type", "", "", "");
        spParams->AddParameter("Phone Type ID", strPhoneType.c_str(), "", "");
        spParams->AddParameter("Phone Number", strNumber.c_str(), "", "");

        m_strContactName = strContactName;
        m_strPhoneNumber = strNumber;
        m_strContactID = strContactID;
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        m_strPhoneType = phoneData.GetPhoneType(m_strContactID, m_strPhoneNumber, m_strPhoneType, strPath);
    }
    else if ("ERR_PHONE_NO_INCOMMING" == strIncommingCallType) {
        VR_LOG("strIncommingCallType : %s", strIncommingCallType.c_str());
        spParams->AddParameter("Contact ID", "", "", "");
        spParams->AddParameter("Contact Name", "", "", "");
        spParams->AddParameter("Phone Type", "", "", "");
        spParams->AddParameter("Phone Type ID", "", "", "");
        spParams->AddParameter("Phone Number", "", "", "");
    }
    else {
    }

    HRESULT retCode  = m_engineCommand.DriverActionReply("HFD", "Incoming Call Info Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::OutgoingCallInfoReply(
    VR_VoiceBoxXmlParser& parser
)
{
    m_strDialType.clear();
    m_strContactName.clear();
    m_strPhoneNumber.clear();
    m_strPhoneType.clear();
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }

    std::string strOutcommingCallType = parser.getValueByKey("errcode");

    m_strDialType = strOutcommingCallType;
    if ("0" == strOutcommingCallType) {
        VR_LOG("strOutcommingCallType : %s", strOutcommingCallType.c_str());
        std::string strContactID = parser.getValueByKey("contactId");
        if ("''" == strContactID) {
            strContactID = "";
        }
        std::string strContactName = parser.getValueByKey("contactName");
        if ("''" == strContactName) {
            strContactName = "";
        }
        std::string strNumber = parser.getValueByKey("number");
        if ("''" == strNumber) {
            strNumber = "";
        }
        std::string strPhoneType = parser.getValueByKey("phoneType");
        if ("''" == strPhoneType) {
            strPhoneType = "";
        }

        m_strPhoneType = strPhoneType;

        spParams->AddParameter("Contact ID", strContactID.c_str(), "", "");
        spParams->AddParameter("Contact Name", strContactName.c_str(), "", "");
        spParams->AddParameter("Phone Type", "", "", "");
        spParams->AddParameter("Phone Type ID", strPhoneType.c_str(), "", "");
        spParams->AddParameter("Phone Number", strNumber.c_str(), "", "");

        m_strContactName = strContactName;
        m_strPhoneNumber = strNumber;
        m_strContactID = strContactID;

        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        m_strPhoneType = phoneData.GetPhoneType(m_strContactID, m_strPhoneNumber, m_strPhoneType, strPath);
    }
    else if ("ERR_PHONE_NO_OUTGOING" == strOutcommingCallType) {
        VR_LOG("strOutcommingCallType : %s", strOutcommingCallType.c_str());
        spParams->AddParameter("Contact ID", "", "", "");
        spParams->AddParameter("Contact Name", "", "", "");
        spParams->AddParameter("Phone Type", "", "", "");
        spParams->AddParameter("Phone Type ID", "", "", "");
        spParams->AddParameter("Phone Number", "", "", "");
    }
    else {
    }
    HRESULT retCode  = m_engineCommand.DriverActionReply("HFD", "Outgoing Call Info Reply", spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::RecentCallsReply(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    CVECIPtr<IVECIListItems> pListItems;
    m_client.CreateListItems(&pListItems);
    CVECIPtr<IVECIParameterSet> spParamsList;
    m_client.CreateParameterSet(&spParamsList);
    if (NULL == spParams.ptr() || NULL == spParamsList.ptr() || NULL == pListItems.ptr()) {
        return false;
    }

    HRESULT retCode = S_OK;
    VoiceVector<RecentCallInfo>::type recentCallList = parser.getRecentCallInfo();
    VoiceVector<RecentCallInfo>::iterator iteratorValue;

    std::string strRecentCallsAvailable = parser.getValueByKey("errcode");
    VR_LOG("strRecentCallsAvailable: %s", strRecentCallsAvailable.c_str());
    if ("0" == strRecentCallsAvailable) {
        VR_LOG("strRecentCallsAvailable: %s", strRecentCallsAvailable.c_str());
        spParams->AddParameter("Recent Calls Available", "True", "", "");
        for (iteratorValue = recentCallList.begin(); iteratorValue != recentCallList.end(); ++iteratorValue) {
            spParamsList->AddParameter("Call Type", (*iteratorValue).phoneKind.c_str(), "", "");
            spParamsList->AddParameter("Phone Type", (*iteratorValue).phoneType.c_str(), "", "");
            spParamsList->AddParameter("Entry", (*iteratorValue).contactName.c_str(), "", "");
            spParamsList->AddParameter("Phone Number", (*iteratorValue).number.c_str(), "", "");
            spParamsList->AddParameter("Time Stamp", (*iteratorValue).timeStamp.c_str(), "", "");
            pListItems->AddItem(spParamsList);
            m_client.CreateParameterSet(&spParamsList);
        }
        retCode  = m_engineCommand.DriverActionReply("HFD", "Recent Calls Reply", spParams, "Recent Calls", pListItems);
    }
    else if ("ERR_PHONE_NO_RECENT_CALL_LIST" == strRecentCallsAvailable) {
        VR_LOG("strRecentCallsAvailable: %s", strRecentCallsAvailable.c_str());
        spParams->AddParameter("Recent Calls Available", "False", "", "");
        retCode  = m_engineCommand.DriverActionReply("HFD", "Recent Calls Reply", spParams, NULL, NULL);
    }
    else {
    }

    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::PhoneBack(
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
VR_VoiceBoxAgentPhone_AU::PhoneCancel(
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
VR_VoiceBoxAgentPhone_AU::PhoneHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone_AU::PhoneStartOver(
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
VR_VoiceBoxAgentPhone_AU::PhoneStartPause(
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
VR_VoiceBoxAgentPhone_AU::PhoneStartRepeat(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<send event=\"repeat-internal\" target=\"de\"/>");

    return true;
}

/* EOF */
