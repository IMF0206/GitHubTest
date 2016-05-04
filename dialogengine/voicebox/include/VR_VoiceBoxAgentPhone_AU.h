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
 * @file VR_VoiceBoxAgentPhone_AU.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VoiceBoxAgentPhone_AU_H
#define VR_VoiceBoxAgentPhone_AU_H

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
class VR_VoiceBoxAgentPhone_AU;

// Define the agent message handler pointer
typedef bool (VR_VoiceBoxAgentPhone_AU::*PhoneMsgHandler_AU)(IVECIParsedMessage*);
typedef bool (VR_VoiceBoxAgentPhone_AU::*PhoneActionResultHandler_AU)(VR_VoiceBoxXmlParser&);

// save the information of messages
struct MessageInfo_AU
{
    int index;                                                  // the index
    std::string instanceId;                                     // instanceId
    std::string messageId;                                      // messageId
};

// save the  of messages
struct MessageInfoVector_AU
{
    std::string strIndex;                                      // the index
    VoiceVector<MessageInfo_AU>::type messageVector;              // the value of attribute
};

// save the recentcalls
struct RecentCallsInfo_AU
{
    std::string strIndex;                                      // the index
    std::string contact;                                       // the contact
    std::string phoneNumber;                                   // the number of phone
};

// save the recentMessage
struct RecentMsgInfo_AU
{
    VBT_ULONG index;                                           // the index
    VoiceVector<StructNode>::type attributeVector;             // the messages
};

/**
 * @brief The VR_VoiceBoxAgentPhone_AU class
 *
 * class declaration
 */
class VR_VoiceBoxAgentPhone_AU : public VR_VoiceBoxEventSink
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxAgentPhone_AU(
        IVECIEngineClient& client,
        IVECIEngineCommand& engineCommand,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxAgentPhone_AU();

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
    VoiceMap<std::string, PhoneMsgHandler_AU>::type m_PhoneMsgHandler_AU;
    VoiceMap<std::string, PhoneActionResultHandler_AU>::type m_PhoneActionResultHandler_AU;
    VoiceMap<std::string, std::string>::type m_phoneCommandtoContent;
    std::string m_strDialType;
    std::string m_strContactID;
    std::string m_strContactName;
    std::string m_strPhoneNumber;
    std::string m_strPhoneType;
    std::string m_requestType;
    IVECIParsedMessage *m_pcMsg;
    int m_listNumber;
    VoiceVector<MessageInfo_AU>::type m_messageInfoVector;     // save the information of the node
    VoiceVector<RecentCallsInfo_AU>::type m_recentCallsInfo;   // save the information of the recent calls
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
    bool CallRecentNumber(IVECIParsedMessage *pcMsg);
    bool CallContactCatch(IVECIParsedMessage *pcMsg); // VR-HFD-01
    bool CallContactConfirmed(IVECIParsedMessage *pcMsg);
    bool CallNumber(IVECIParsedMessage *pcMsg);
    bool CallNumberCatch(IVECIParsedMessage *pcMsg); // VR-HFD-02
    bool CallNumberConfirmed(IVECIParsedMessage *pcMsg);
    bool CallSelectedMsgConfirmed(IVECIParsedMessage *pcMsg);
    bool ConfirmSMSRecipient(IVECIParsedMessage *pcMsg); // VR-HFD-13
    bool DigitsCorrection(IVECIParsedMessage *pcMsg); // VR-HFD-12
    bool HFDHome(IVECIParsedMessage *pcMsg);
    bool HFDMenu(IVECIParsedMessage *pcMsg);
    bool ListPartialNameMatches(IVECIParsedMessage *pcMsg);
    bool ListPhoneTypes(IVECIParsedMessage *pcMsg);
    bool ListRecentCalls(IVECIParsedMessage *pcMsg);
    bool NoContactNumbers(IVECIParsedMessage *pcMsg);
    bool NoContactsFound(IVECIParsedMessage *pcMsg);
    bool RedialConfirmed(IVECIParsedMessage *pcMsg);
    bool SendDTMF(IVECIParsedMessage *pcMsg);
    bool SendDTMFCatch(IVECIParsedMessage *pcMsg);
    bool SendDTMFConfirmed(IVECIParsedMessage *pcMsg);
    bool SendDTMFRejected(IVECIParsedMessage *pcMsg);
    bool PhoneBack(IVECIParsedMessage *pcMsg);
    bool PhoneCancel(IVECIParsedMessage *pcMsg);
    bool PhoneHelp(IVECIParsedMessage *pcMsg);
    bool PhoneStartOver(IVECIParsedMessage *pcMsg);
    bool PhoneStartPause(IVECIParsedMessage *pcMsg);
    bool PhoneStartRepeat(IVECIParsedMessage *pcMsg);

    // info driver
    bool IncomingCallInfoRequest(IVECIParsedMessage *pcMsg);
    bool OutgoingCallInfoRequest(IVECIParsedMessage *pcMsg);
    bool RecentCallsRequest(IVECIParsedMessage *pcMsg);

    bool IncomingCallInfoReply(VR_VoiceBoxXmlParser& parser);
    bool OutgoingCallInfoReply(VR_VoiceBoxXmlParser& parser);
    bool RecentCallsReply(VR_VoiceBoxXmlParser& parser);

    void BuildListHeader(IVECIParsedMessage *pcMsg,
    VR_VoiceBoxXmlBuilder& xmlBulder, pugi::xml_node& nodeList);

    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxAgentPhone_AU(const VR_VoiceBoxAgentPhone_AU&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxAgentPhone_AU &operator=(const VR_VoiceBoxAgentPhone_AU&);
};


#endif // VR_VoiceBoxAgentPhone_AU_H
/* EOF */