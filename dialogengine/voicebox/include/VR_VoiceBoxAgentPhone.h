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
 * @file VR_VoiceBoxAgentPhone.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXAGENTPHONE_H
#define VR_VOICEBOXAGENTPHONE_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <sstream>

/* Suntec Header */
#ifndef VR_VOICEBOXEVENTSINK_H
#    include "VR_VoiceBoxEventSink.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

#ifndef VR_VOICEBOXXMLBUILDER_H
#    include "VR_VoiceBoxXmlBuilder.h"
#endif

/* Forward Declaration */
class VR_VoiceBoxAgentPhone;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentPhone::*PhoneMsgHandler)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentPhone::*PhoneActionResultHandler)(VR_VoiceBoxXmlParser&);

// save the information of messages
struct MessageInfo
{
    int index;                                                  // the index
    std::string instanceId;                                     // instanceId
    std::string messageId;                                      // messageId
    std::string phoneNumber;
    std::string messageType;
    std::string phoneType;
    std::string sender;
    std::string timeStamp;
    std::string subject;
    std::string contactID;
};

// save the information of callhistory
struct CallHistory
{
    int index;                                                  // the index
    std::string contactName;                                     // contactName
    std::string contactID;
    std::string phoneType;
};


// save the  of messages
struct MessageInfoVector
{
    VoiceVector<MessageInfo>::type messageVector;              // the value of attribute
};

// save the recentcalls
struct RecentCallsInfo
{
    std::string strIndex;                                      // the index
    std::string contact;                                       // the contact
    std::string phoneNumber;                                   // the number of phone
    std::string phoneType;                                     // the type of phone
};

// save the recentMessage
struct RecentMsgInfo
{
    VBT_ULONG index;                                           // the index
    VoiceVector<StructNode>::type attributeVector;             // the messages
};

/**
 * @brief The VR_VoiceBoxAgentPhone class
 *
 * class declaration
 */
class VR_VoiceBoxAgentPhone : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentPhone(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentPhone();

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
    virtual bool Initialize();

    /**
     * UnInitialize
     *
     * UnInitialize
     *
     * @param none
     * @return none
     * @attention none
     * @see none
     */
    virtual void UnInitialize();

    /**
     * ReplyQueryInfo
     *
     * ReplyQueryInfo
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool ReplyQueryInfo(
        VR_VoiceBoxXmlParser& parser
        );

    // IVECIEventSink's pure virtual functions
    virtual HRESULT QueryAgentSupport(VBT_CSTR szAgent, VBT_BOOL *pbSupported);
    virtual HRESULT OnAgentResultMessage(IVECITransaction* pTrans, IVECIParsedMessage* pResultMsg);
    virtual HRESULT OnAgentDriverMessage(IVECIParsedMessage* pDriverMsg);

    virtual bool ProcessAgentMessage(IVECIParsedMessage *pcMsg);

private:
    VoiceMap<std::string, PhoneMsgHandler>::type m_phoneMsgHandler;
    VoiceMap<std::string, PhoneActionResultHandler>::type m_phoneActionResultHandler;
    VoiceMap<std::string, std::string>::type m_phoneCommandtoContent;
    std::string m_strDialType;
    std::string m_strContactID;
    std::string m_strContactName;
    std::string m_strPhoneNumber;
    std::string m_strPhoneType;
    std::string m_requestType;
    std::string m_strNextItem;
    std::string m_strPreviousItem;
    std::string m_strMessageType;
    std::string m_strInstanceId;
    std::string m_strSubject;
    std::string m_strOutGoing;
    std::string m_strIncoming;
    std::string m_strMiss;
    std::string m_strEmail;

    IVECIParsedMessage *m_pcMsg;
    int m_listNumber;
    VoiceVector<MessageInfo>::type m_messageInfoVector;     // save the information of the node
    VoiceVector<RecentCallsInfo>::type m_recentCallsInfo;   // save the information of the recent calls
    MessageInfo m_incomingMessageInfo;
    VoiceVector<CallHistory>::type m_callHistory;
    /**
     * Process Agent Message
     *
     * Process message that was sent by VoiceBox
     *
     * @param pcMsg [IN] : Agent result message or info driver message
     * @return bool
     * @attention none
     * @see none
     */

    bool HFDNBest(IVECIParsedMessage *pcMsg);
    bool AppendDigitsAlreadyAtMax(IVECIParsedMessage *pcMsg);
    bool AppendDigitsCatch(IVECIParsedMessage *pcMsg);
    bool PhoneTaskComplete(IVECIParsedMessage *pcMsg);
    bool CallBackConfirmed(IVECIParsedMessage *pcMsg);
    bool CallCatch(IVECIParsedMessage *pcMsg);
    bool CallContact(IVECIParsedMessage *pcMsg);
    bool CallContactCatch(IVECIParsedMessage *pcMsg); // VR-HFD-01
    bool CallContactConfirmed(IVECIParsedMessage *pcMsg);
    bool CallCurrentMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool CallIncomingMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool CallNumber(IVECIParsedMessage *pcMsg);
    bool CallNumberCatch(IVECIParsedMessage *pcMsg); // VR-HFD-02
    bool CallNumberConfirmed(IVECIParsedMessage *pcMsg);
    bool CallSelectedMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool CallSelectedRecentConfirm(IVECIParsedMessage *pcMsg);
    bool CannotAddVoiceTag(IVECIParsedMessage *pcMsg);
    bool CannotReplytoCurrentMessage(IVECIParsedMessage *pcMsg);
    bool ChooseVoiceTagContact(IVECIParsedMessage *pcMsg);
    bool ConfirmSMSRecipient(IVECIParsedMessage *pcMsg); // VR-HFD-13
    bool DigitsCorrection(IVECIParsedMessage *pcMsg); // VR-HFD-12
    bool HFDHome(IVECIParsedMessage *pcMsg);
    bool HFDKeyboardDictation(IVECIParsedMessage *pcMsg);
    bool ListMessages(IVECIParsedMessage *pcMsg);
    bool ListPartialNameMatches(IVECIParsedMessage *pcMsg);
    bool ListPhoneTypes(IVECIParsedMessage *pcMsg);
    bool ListRecentCalls(IVECIParsedMessage *pcMsg);
    bool MessageDictation(IVECIParsedMessage *pcMsg);
    bool MuteCall(IVECIParsedMessage *pcMsg);
    bool MuteOff(IVECIParsedMessage *pcMsg);
    bool NoContactNumbers(IVECIParsedMessage *pcMsg);
    bool NoContactsFound(IVECIParsedMessage *pcMsg);
    bool NoPreviousMessage(IVECIParsedMessage *pcMsg);
    bool NoQuickReplyMessages(IVECIParsedMessage *pcMsg);
    bool OffboardDictation(IVECIParsedMessage *pcMsg); // VR-HFD-17
    bool OneCallSelectedCallConfirmed(IVECIParsedMessage *pcMsg);
    bool ReadIncomingMessage(IVECIParsedMessage *pcMsg);
    bool ReadSelectedMessage(IVECIParsedMessage *pcMsg); // VR-HFD-23
    bool ReadSelectMsgInvalidListNum(IVECIParsedMessage *pcMsg);
    bool RedialConfirmed(IVECIParsedMessage *pcMsg);
    bool ReplywithDictatedMessage(IVECIParsedMessage *pcMsg);
    bool ReplywithDictatedMsgConfirm(IVECIParsedMessage *pcMsg);
    bool ReplywithDictatedMsgReject(IVECIParsedMessage *pcMsg);
    bool ReplywithQuickReplyMessage(IVECIParsedMessage *pcMsg); // VR-HFD-10
    bool ReplywithQuickMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool SelectQuickReply(IVECIParsedMessage *pcMsg); // VR-HFD-21
    bool SelectRecCallNoContactInfo(IVECIParsedMessage *pcMsg);
    bool SendDictatedMessageConfirmed(IVECIParsedMessage *pcMsg);
    bool SendDTMF(IVECIParsedMessage *pcMsg);
    bool SendDTMFCatch(IVECIParsedMessage *pcMsg);
    bool SendQuickReplyMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool SendSMSNoChangePhoneTypes(IVECIParsedMessage *pcMsg);
    bool SMSNotAvailable(IVECIParsedMessage *pcMsg);
    bool SMSRecipientRejected(IVECIParsedMessage *pcMsg); // VR-HFD-11
    bool VoiceTagSIDError(IVECIParsedMessage *pcMsg);
    bool StartVoiceTag(IVECIParsedMessage *pcMsg);
    bool VoiceTagError(IVECIParsedMessage *pcMsg);
    bool VoiceTag(IVECIParsedMessage *pcMsg);
    bool PhoneBack(IVECIParsedMessage *pcMsg);
    bool PhoneCancel(IVECIParsedMessage *pcMsg);
    bool PhoneHelp(IVECIParsedMessage *pcMsg);
    bool PhoneStartOver(IVECIParsedMessage *pcMsg);
    bool PhoneStartPause(IVECIParsedMessage *pcMsg);
    bool PhoneStartRepeat(IVECIParsedMessage *pcMsg);
    bool EscalatingError(IVECIParsedMessage *pcMsg);

    // info driver
    bool GetNextMessageRequest(IVECIParsedMessage *pcMsg);
    bool GetPreviousMessageRequest(IVECIParsedMessage *pcMsg);
    bool GetSelectedMessageRequest(IVECIParsedMessage *pcMsg);
    bool IncomingCallInfoRequest(IVECIParsedMessage *pcMsg);
    bool IsDrivingRequest(IVECIParsedMessage *pcMsg);
    bool OutgoingCallInfoRequest(IVECIParsedMessage *pcMsg);
    bool RecentCallsRequest(IVECIParsedMessage *pcMsg);
    bool RecentMessagesByTypeRequest(IVECIParsedMessage *pcMsg);
    bool RecentMessagesRequest(IVECIParsedMessage *pcMsg);

    bool RecentMessagesReply(VR_VoiceBoxXmlParser& parser);
    bool GetSelectedMessageReply(VR_VoiceBoxXmlParser& parser);
    void GetMessageBodyError(VR_VoiceBoxXmlParser& parser);
    bool IncomingCallInfoReply(VR_VoiceBoxXmlParser& parser);
    bool OutgoingCallInfoReply(VR_VoiceBoxXmlParser& parser);
    bool RecentCallsReply(VR_VoiceBoxXmlParser& parser);
    bool RecentMessagesByTypeReply(VR_VoiceBoxXmlParser& parser);
    bool IsDrivingReply(VR_VoiceBoxXmlParser& parser);
    bool QuitVRInit(VR_VoiceBoxXmlParser& parser);
    void ChangeLanguage();
    bool ChangeCallType(VR_VoiceBoxXmlParser& parser);
    bool SetIncomingMessageInfo(VR_VoiceBoxXmlParser& parser);

    void BuildListHeader(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& nodeList);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentPhone(const VR_VoiceBoxAgentPhone&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentPhone &operator=(const VR_VoiceBoxAgentPhone&);
};


#endif // VR_VOICEBOXAGENTPHONE_H
/* EOF */