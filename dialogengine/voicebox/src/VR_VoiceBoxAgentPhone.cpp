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
#ifndef VR_VOICEBOXAGENTPHONE_H
#    include "VR_VoiceBoxAgentPhone.h"
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

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

using namespace nutshell;

// PhoneActionHandler
struct PhoneActionHandler
{
    char*             szAction; // Action type
    PhoneMsgHandler  handler;  // Handler pointer
};

// Constructor
VR_VoiceBoxAgentPhone::VR_VoiceBoxAgentPhone(
    IVECIEngineClient& client,
    IVECIEngineCommand& engineCommand,
    VR_VoiceBoxEngineCallback& engineCallback
)
: VR_VoiceBoxEventSink(client, engineCommand, engineCallback)
{
    m_listNumber = 0;
}

// Destructor
VR_VoiceBoxAgentPhone::~VR_VoiceBoxAgentPhone()
{
}

bool
VR_VoiceBoxAgentPhone::Initialize()
{
    const PhoneActionHandler actionHandlerMap[] = {
        { VBT_LIST_TYPE_HFD_NBEST,                                               // "n-Best"
            &VR_VoiceBoxAgentPhone::HFDNBest },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS,                                     // "Append Digits"
            &VR_VoiceBoxAgentPhone::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS_ALREADY_AT_MAX,                      // "Append Digits Already At Max"
            &VR_VoiceBoxAgentPhone::AppendDigitsAlreadyAtMax },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS_CATCH,                               // "Append Digits Catch"
            &VR_VoiceBoxAgentPhone::AppendDigitsCatch },
        { VBT_ACTION_TYPE_HFD_APPEND_DIGITS_MAX_REACHED,                         // "Append Digits Max reached"
            &VR_VoiceBoxAgentPhone::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_BUSY_UPDATING_GRAMMAR,                             // "Busy Updating Grammar"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CALL_BACK_CONFIRMED,                               // "Call Back Confirmed"
            &VR_VoiceBoxAgentPhone::CallBackConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_CATCH,                                        // "Call Catch"
            &VR_VoiceBoxAgentPhone::CallCatch },
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT,                                      // "Call Contact"
            &VR_VoiceBoxAgentPhone::CallContact }, // VR-HFD-03
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_CATCH,                                // "Call Contact Catch"
            &VR_VoiceBoxAgentPhone::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_CONFIRMED,                            // "Call Contact Confirmed"
            &VR_VoiceBoxAgentPhone::CallContactConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_CONTACT_REJECTED,                             // "Call Contact Rejected"
            &VR_VoiceBoxAgentPhone::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_CALL_CURRENT_MESSAGE_CONFIRMED,                    // "Call Current Message Confirmed"
            &VR_VoiceBoxAgentPhone::CallCurrentMsgConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_CURRENT_MESSAGE_NO_PHONE_NUMBER,              // "Call Current Message No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CALL_INCOMING_MESSAGE_CONFIRMED,                   // "Call Incoming Message Confirmed"
            &VR_VoiceBoxAgentPhone::CallIncomingMsgConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_INCOMING_MESSAGE_NO_PHONE_NUMBER,             // "Call Incoming Message No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER,                                       // "Call Number"
            &VR_VoiceBoxAgentPhone::CallNumber },
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_CATCH,                                 // "Call Number Catch"
            &VR_VoiceBoxAgentPhone::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_CONFIRMED,                             // "Call Number Confirmed"
            &VR_VoiceBoxAgentPhone::CallNumberConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_REJECTED,                              // "Call Number Rejected"
            &VR_VoiceBoxAgentPhone::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_CALL_NUMBER_WITH_BLOCK_DIALING,                    // "Call Number With Block Dialing"
            &VR_VoiceBoxAgentPhone::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_CALL_SELECTED_MESSAGE_CONFIRMED,                   // "Call Selected Message Confirmed"
            &VR_VoiceBoxAgentPhone::CallSelectedMsgConfirmed },
        { VBT_ACTION_TYPE_HFD_CALL_SELECTED_MESSAGE_NO_PHONE_NUMBER,             // "Call Selected Message No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CALL_SELECTED_RECENT_CALL_CONFIRMED,               // "Call Selected Recent Call Confirmed"
            &VR_VoiceBoxAgentPhone::CallSelectedRecentConfirm },
        { VBT_ACTION_TYPE_HFD_CALL_SELECTED_RECENT_CALL_NO_PHONE_NUMBER,         // "Call Selected Recent Call No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CANNOT_ADD_VOICE_TAG,                              // "Cannot Add Voice Tag"
            &VR_VoiceBoxAgentPhone::CannotAddVoiceTag },
        { VBT_ACTION_TYPE_HFD_CANNOT_REPLY_TO_CURRENT_MESSAGE,                   // "Cannot Reply to Current Message"
            &VR_VoiceBoxAgentPhone::CannotReplytoCurrentMessage },
        { VBT_ACTION_TYPE_HFD_CANNOT_REPLY_TO_INCOMING_MESSAGE,                  // "Cannot Reply to Incoming Message"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CANNOT_REPLY_TO_SELECTED_MESSAGE,                  // "Cannot Reply to Selected Message"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_CHOOSE_VOICE_TAG_CONTACT,                          // "Choose Voice Tag Contact"
            &VR_VoiceBoxAgentPhone::ChooseVoiceTagContact },
        { VBT_ACTION_TYPE_HFD_CONFIRM_SEND_SMS,                                  // "Confirm Send SMS"
            &VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage },
        { VBT_ACTION_TYPE_HFD_CONFIRM_SMS_RECIPIENT,                             // "Confirm SMS Recipient"
            &VR_VoiceBoxAgentPhone::ConfirmSMSRecipient }, // VR-HFD-13
        { VBT_ACTION_TYPE_HFD_DIGITS_CORRECTION,                                 // "Digits Correction"
            &VR_VoiceBoxAgentPhone::DigitsCorrection }, // VR-HFD-12
        { VBT_ACTION_TYPE_HFD_DIGITS_CORRECTION_NO_MORE_DIGITS,                  // "Digits Correction No More Digits"
            &VR_VoiceBoxAgentPhone::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_DIGITS_START_OVER,                                 // "Digits Start Over"
            &VR_VoiceBoxAgentPhone::CallNumberCatch }, // VR-HFD-02
        { VBT_ACTION_TYPE_HFD_HFD_HOME,                                          // "HFD Home"
            &VR_VoiceBoxAgentPhone::HFDHome },
        { VBT_ACTION_TYPE_HFD_HFD_KEYBOARD_DICTATION,                            // "HFD Keyboard Dictation"
            &VR_VoiceBoxAgentPhone::HFDKeyboardDictation },
        { VBT_ACTION_TYPE_HFD_IGNORE_INCOMING_MESSAGE,                           // "Ignore Incoming Message"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_INCOMING_MESSAGE_OFFBOARD_DICTATION,               // "Incoming Message - Offboard Dictation"
            &VR_VoiceBoxAgentPhone::OffboardDictation },
        { VBT_ACTION_TYPE_HFD_LIST_MESSAGES,                                     // "List Messages"
            &VR_VoiceBoxAgentPhone::ListMessages }, // VR-HFD-20
        { VBT_ACTION_TYPE_HFD_LIST_PARTIAL_NAME_MATCHES,                         // "List Partial Name Matches"
            &VR_VoiceBoxAgentPhone::ListPartialNameMatches }, // VR-HFD-05
        { VBT_ACTION_TYPE_HFD_LIST_PHONE_TYPES,                                  // "List Phone Types"
            &VR_VoiceBoxAgentPhone::ListPhoneTypes },
        { VBT_ACTION_TYPE_HFD_LIST_PREDEFINED_SMS,                               // "List Predefined SMS"
            &VR_VoiceBoxAgentPhone::ConfirmSMSRecipient },
        { VBT_ACTION_TYPE_HFD_LIST_RECENT_CALLS,                                 // "List Recent Calls"
            &VR_VoiceBoxAgentPhone::ListRecentCalls },
        { VBT_ACTION_TYPE_HFD_LIST_RECENT_MESSAGES_BY_TYPE,                      // "List Recent Messages by Type"
            &VR_VoiceBoxAgentPhone::ListMessages },
        { VBT_ACTION_TYPE_HFD_MESSAGE_DICTATION,                                 // "Message dictation"
            &VR_VoiceBoxAgentPhone::MessageDictation },
        { VBT_ACTION_TYPE_HFD_MUTE_CALL,                                         // "Mute Call"
            &VR_VoiceBoxAgentPhone::MuteCall },
        { VBT_ACTION_TYPE_HFD_MUTE_OFF,                                          // "Mute Off"
            &VR_VoiceBoxAgentPhone::MuteOff },
        { VBT_ACTION_TYPE_HFD_NO_CALL_BACK_NUMBER,                               // "No Call Back Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_CONTACT_NUMBERS,                                // "No Contact Numbers"
            &VR_VoiceBoxAgentPhone::NoContactNumbers },
        { VBT_ACTION_TYPE_HFD_NO_CONTACTS_FOUND,                                 // "No Contacts Found"
            &VR_VoiceBoxAgentPhone::NoContactsFound },
        { VBT_ACTION_TYPE_HFD_NO_NEXT_MESSAGE,                                   // "No Next Message"
            &VR_VoiceBoxAgentPhone::ListMessages },
        { VBT_ACTION_TYPE_HFD_NO_PHONE_CONNECTED,                                // "No Phone Connected"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_PREVIOUS_MESSAGE,                               // "No Previous Message"
            &VR_VoiceBoxAgentPhone::ListMessages },
        { VBT_ACTION_TYPE_HFD_NO_QUICK_REPLY_MESSAGES,                           // "No Quick Reply Messages"
            &VR_VoiceBoxAgentPhone::NoQuickReplyMessages },
        { VBT_ACTION_TYPE_HFD_NO_RECENT_CALLS,                                   // "No Recent Calls"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_RECENT_MESSAGES,                                // "No Recent Messages"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_RECENT_MESSAGES_BY_TYPE,                        // "No Recent Messages by Type"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_NO_REDIAL_NUMBER,                                  // "No Redial Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_OFFBOARD_DICTATION,                                // "Offboard Dictation"
            &VR_VoiceBoxAgentPhone::OffboardDictation }, // VR-HFD-17
        { VBT_ACTION_TYPE_HFD_OFFBOARD_DICTATION_TO_SELECTED_MESSAGE,            // "Offboard Dictation to Selected Message"
            &VR_VoiceBoxAgentPhone::OffboardDictation },
        { VBT_ACTION_TYPE_HFD_ONE_CALL_SELECTED_CALL_CONFIRMED,                  // "One Call Selected Call Confirmed"
            &VR_VoiceBoxAgentPhone::OneCallSelectedCallConfirmed },
        { VBT_ACTION_TYPE_HFD_ONE_CALL_SELECTED_QUICK_REPLY,                     // "One Call Selected Quick Reply"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_ONE_CONTACT_SELECTED,                              // "One Contact Selected"
            &VR_VoiceBoxAgentPhone::SelectRecCallNoContactInfo },
        { VBT_ACTION_TYPE_HFD_ONE_MESSAGE_SELECTED_OFFBOARD_DICTATION,           // "One Message Selected - Offboard Dictation"
            &VR_VoiceBoxAgentPhone::OffboardDictation },
        { VBT_ACTION_TYPE_HFD_QUICK_REPLY_TO_CURRENT_MESSAGE,                    // "Quick Reply to Current Message"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_QUICK_REPLY_TO_INCOMING_MESSAGE,                   // "Quick Reply to Incoming Message"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_QUICK_REPLY_TO_SELECTED_MESSAGE,                   // "Quick Reply to Selected Message"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_QUICK_REPLY_TO_SELECTED_RECENT_CALL,               // "Quick Reply to Selected Recent Call"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_QUICK_REPLY_TO_SELECTED_RECENT_CALL_NO_CONTACT_INFO, // "Quick Reply to Selected Recent Call No Contact Info"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_READ_INCOMING_MESSAGE,                             // "Read Incoming Message"
            &VR_VoiceBoxAgentPhone::ReadIncomingMessage },
        { VBT_ACTION_TYPE_HFD_READ_NEXT_MESSAGE,                                 // "Read Next Message"
            &VR_VoiceBoxAgentPhone::ReadSelectedMessage },
        { VBT_ACTION_TYPE_HFD_READ_PREVIOUS_MESSAGE,                             // "Read Previous Message"
            &VR_VoiceBoxAgentPhone::ReadSelectedMessage },
        { VBT_ACTION_TYPE_HFD_READ_SELECTED_MESSAGE,                             // "Read Selected Message"
            &VR_VoiceBoxAgentPhone::ReadSelectedMessage }, // VR-HFD-23
        { VBT_ACTION_TYPE_HFD_READ_SELECTED_MESSAGE_INVALID_LIST_NUMBER,         // "Read Selected Message Invalid List Number"
            &VR_VoiceBoxAgentPhone::ReadSelectMsgInvalidListNum },
        { VBT_ACTION_TYPE_HFD_REDIAL_CONFIRMED,                                  // "Redial Confirmed"
            &VR_VoiceBoxAgentPhone::RedialConfirmed },
        { VBT_ACTION_TYPE_HFD_REPLY_TO_SELECTED_RECENT_CALL_OFFBOARD_DICTATION,  // "Reply to Selected Recent Call - Offboard Dictation"
            &VR_VoiceBoxAgentPhone::OffboardDictation },
        { VBT_ACTION_TYPE_HFD_REPLY_TO_SELECTED_RECENT_CALL_NO_PHONE_NUMBER,     // "Reply to Selected Recent Call No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_DICTATED_MESSAGE,                       // "Reply with Dictated Message"
            &VR_VoiceBoxAgentPhone::ReplywithDictatedMessage },
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_DICTATED_MESSAGE_CONFIRMED,             // "Reply with Dictated Message Confirmed"
            &VR_VoiceBoxAgentPhone::ReplywithDictatedMsgConfirm },
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_DICTATED_MESSAGE_REJECTED,              // "Reply with Dictated Message Rejected"
            &VR_VoiceBoxAgentPhone::ReplywithDictatedMsgReject },
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_QUICK_REPLY_MESSAGE,                    // "Reply with Quick Reply Message"
            &VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage }, // VR-HFD-10
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_QUICK_REPLY_MESSAGE_CONFIRMED,          // "Reply with Quick Reply Message Confirmed"
            &VR_VoiceBoxAgentPhone::ReplywithQuickMsgConfirmed },
        { VBT_ACTION_TYPE_HFD_REPLY_WITH_QUICK_REPLY_MESSAGE_REJECTED,           // "Reply with Quick Reply Message Rejected"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_SELECT_NAME_AND_TYPE_MATCHES_CALL_NONE,            // "Select Name And Type Matches - Call None"
            &VR_VoiceBoxAgentPhone::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_SELECT_NAME_AND_TYPE_MATCHES_SMS_NONE,             // "Select Name And Type Matches - SMS None"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected },
        { VBT_ACTION_TYPE_HFD_SELECT_NAME_MATCHES_CALL_NONE,                     // "Select Name Matches - Call None"
            &VR_VoiceBoxAgentPhone::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_SELECT_NAME_MATCHES_SMS_NONE,                      // "Select Name Matches - SMS None"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected },
        { VBT_ACTION_TYPE_HFD_SELECT_PHONE_TYPES_CALL_NONE,                      // "Select Phone Types - Call None"
            &VR_VoiceBoxAgentPhone::CallContactCatch }, // VR-HFD-01
        { VBT_ACTION_TYPE_HFD_SELECT_PHONE_TYPES_SMS_NONE,                       // "Select Phone Types - SMS None"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected },
        { VBT_ACTION_TYPE_HFD_SELECT_QUICK_REPLY,                                // "Select Quick Reply"
            &VR_VoiceBoxAgentPhone::SelectQuickReply }, // VR-HFD-21
        { VBT_ACTION_TYPE_HFD_SELECTED_RECENT_CALL_NO_CONTACT_INFO,              // "Selected Recent Call No Contact Info"
            &VR_VoiceBoxAgentPhone::SelectRecCallNoContactInfo }, // VR-HFD-22
        { VBT_ACTION_TYPE_HFD_SELECTED_RECENT_CALL_NO_PHONE_NUMBER,              // "Selected Recent Call No Phone Number"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_SEND_DICTATED_MESSAGE,                             // "Send Dictated Message"
            &VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage },
        { VBT_ACTION_TYPE_HFD_SEND_DICTATED_MESSAGE_CATCH,                       // "Send Dictated Message Catch"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected },
        { VBT_ACTION_TYPE_HFD_SEND_DICTATED_MESSAGE_CONFIRMED,                   // "Send Dictated Message Confirmed"
            &VR_VoiceBoxAgentPhone::SendDictatedMessageConfirmed },
        { VBT_ACTION_TYPE_HFD_SEND_DICTATED_MESSAGE_REJECTED,                    // "Send Dictated Message Rejected"
            &VR_VoiceBoxAgentPhone::OffboardDictation },
        { VBT_ACTION_TYPE_HFD_SEND_DTMF,                                         // "Send DTMF"
            &VR_VoiceBoxAgentPhone::SendDTMF },
        { VBT_ACTION_TYPE_HFD_SEND_DTMF_CATCH,                                   // "Send DTMF Catch"
            &VR_VoiceBoxAgentPhone::SendDTMFCatch }, // VR-HFD-14
        { VBT_ACTION_TYPE_HFD_SEND_QUICK_REPLY_CATCH,                            // "Send Quick Reply Catch"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected },
        { VBT_ACTION_TYPE_HFD_SEND_QUICK_REPLY_MESSAGE,                          // "Send Quick Reply Message"
            &VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage },
        { VBT_ACTION_TYPE_HFD_SEND_QUICK_REPLY_MESSAGE_CONFIRMED,                // "Send Quick Reply Message Confirmed"
            &VR_VoiceBoxAgentPhone::SendQuickReplyMsgConfirmed },
        { VBT_ACTION_TYPE_HFD_SEND_QUICK_REPLY_MESSAGE_REJECTED,                 // "Send Quick Reply Message Rejected"
            &VR_VoiceBoxAgentPhone::SelectQuickReply },
        { VBT_ACTION_TYPE_HFD_SEND_SMS_CHANGE_PHONE_TYPE,                        // "Send SMS Change Phone Type"
            &VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage },
        { VBT_ACTION_TYPE_HFD_SEND_SMS_CHANGE_PHONE_TYPE_NO_PHONE_TYPES,         // "Send SMS Change Phone Type - No Phone Types"
            &VR_VoiceBoxAgentPhone::SendSMSNoChangePhoneTypes },
        { VBT_ACTION_TYPE_HFD_SEND_SMS_WITH_BLOCK_DIALING,                       // "Send SMS With Block Dialing"
            &VR_VoiceBoxAgentPhone::DigitsCorrection },
        { VBT_ACTION_TYPE_HFD_SMS_NOT_AVAILABLE,                                 // "SMS Not Available"
            &VR_VoiceBoxAgentPhone::SMSNotAvailable },
        { VBT_ACTION_TYPE_HFD_SMS_RECIPIENT_REJECTED,                            // "SMS Recipient Rejected"
            &VR_VoiceBoxAgentPhone::SMSRecipientRejected }, // VR-HFD-11
        { VBT_ACTION_TYPE_HFD_VBT_ERROR,                                         // "VBT Error"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_VOICE_TAG_SID_ERROR,                               // "Voice Tag - SID Error"
            &VR_VoiceBoxAgentPhone::VoiceTagSIDError },
        { VBT_ACTION_TYPE_HFD_START_VOICE_TAG,                                   // "Start Voice Tag"
            &VR_VoiceBoxAgentPhone::StartVoiceTag },
        { VBT_ACTION_TYPE_HFD_VOICE_TAG,                                         // "Voice Tag"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_VOICE_TAG_ERROR,                                   // "Voice Tag Error"
            &VR_VoiceBoxAgentPhone::PhoneTaskComplete },
        { VBT_ACTION_TYPE_HFD_BACK,                                              // "Back"
            &VR_VoiceBoxAgentPhone::PhoneBack },
        { VBT_ACTION_TYPE_HFD_CANCEL, // "Cancel"
            &VR_VoiceBoxAgentPhone::PhoneCancel },
        { VBT_ACTION_TYPE_HFD_HELP, // "Help"
            &VR_VoiceBoxAgentPhone::PhoneHelp },
        { VBT_ACTION_TYPE_HFD_START_OVER, // "Start Over"
            &VR_VoiceBoxAgentPhone::PhoneStartOver },
        { VBT_ACTION_TYPE_HFD_SYSTEM_PAUSE, // "Start Pause"
            &VR_VoiceBoxAgentPhone::PhoneStartPause },
        { VBT_ACTION_TYPE_HFD_SYSTEM_REPEAT, // "Start Repeat"
            &VR_VoiceBoxAgentPhone::PhoneStartRepeat },
        { VBT_ACTION_TYPE_HFD_ESCALATING_ERROR, // "Escalating Error"
            &VR_VoiceBoxAgentPhone::EscalatingError },

        // Info Driver
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_GET_NEXT_MESSAGE_REQUEST,           // "Get Next Message Request"
            &VR_VoiceBoxAgentPhone::GetNextMessageRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_GET_PREVIOUS_MESSAGE_REQUEST,       // "Get Previous Message Request"
            &VR_VoiceBoxAgentPhone::GetPreviousMessageRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_GET_SELECTED_MESSAGE_REQUEST,       // "Get Selected Message Request"
            &VR_VoiceBoxAgentPhone::GetSelectedMessageRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_INCOMING_CALL_INFO_REQUEST,         // "Incoming Call Info Request"
            &VR_VoiceBoxAgentPhone::IncomingCallInfoRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_IS_DRIVING_REQUEST,                 // "Is Driving Request"
            &VR_VoiceBoxAgentPhone::IsDrivingRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_OUTGOING_CALL_INFO_REQUEST,         // "Outgoing Call Info Request"
            &VR_VoiceBoxAgentPhone::OutgoingCallInfoRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_RECENT_CALLS_REQUEST,               // "Recent Calls Request"
            &VR_VoiceBoxAgentPhone::RecentCallsRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_RECENT_MESSAGES_BY_TYPE_REQUEST,    // "Recent Messages By Type Request"
            &VR_VoiceBoxAgentPhone::RecentMessagesByTypeRequest },
        { VBT_DRIVER_REQUEST_ACTION_TYPE_HFD_RECENT_MESSAGES_REQUEST,            // "Recent Messages Request"
            &VR_VoiceBoxAgentPhone::RecentMessagesRequest },
        { NULL, NULL }
    };

    int i = 0;
    while (true) {
        if (NULL == actionHandlerMap[i].szAction) {
            break;
        }

        m_phoneMsgHandler.insert(std::make_pair(
            actionHandlerMap[i].szAction,
            actionHandlerMap[i].handler
            ));

        ++i;
    }
    m_phoneActionResultHandler.insert(std::make_pair("changeLanguage", &VR_VoiceBoxAgentPhone::ChangeCallType));
    m_phoneActionResultHandler.insert(std::make_pair("QuitVRInit", &VR_VoiceBoxAgentPhone::QuitVRInit));
    m_phoneActionResultHandler.insert(std::make_pair("setIncomingMessageInfo", &VR_VoiceBoxAgentPhone::SetIncomingMessageInfo));
    m_phoneActionResultHandler.insert(std::make_pair("queryRecentMessage", &VR_VoiceBoxAgentPhone::RecentMessagesReply));
    m_phoneActionResultHandler.insert(std::make_pair("getMessageDetail", &VR_VoiceBoxAgentPhone::GetSelectedMessageReply));
    m_phoneActionResultHandler.insert(std::make_pair("queryIncomingCall", &VR_VoiceBoxAgentPhone::IncomingCallInfoReply));
    m_phoneActionResultHandler.insert(std::make_pair("queryOutgoingCall", &VR_VoiceBoxAgentPhone::OutgoingCallInfoReply));
    m_phoneActionResultHandler.insert(std::make_pair("queryRecentCallList", &VR_VoiceBoxAgentPhone::RecentCallsReply));
    m_phoneActionResultHandler.insert(std::make_pair("queryRecentMessageByType", &VR_VoiceBoxAgentPhone::RecentMessagesByTypeReply));
    m_phoneActionResultHandler.insert(std::make_pair("queryVehicleinMotion", &VR_VoiceBoxAgentPhone::IsDrivingReply));

    ChangeLanguage();

    return true;
}

void
VR_VoiceBoxAgentPhone::UnInitialize()
{
    m_phoneMsgHandler.clear();
    m_phoneActionResultHandler.clear();
    m_messageInfoVector.clear();
}

bool
VR_VoiceBoxAgentPhone::ReplyQueryInfo(
    VR_VoiceBoxXmlParser& parser)
{
    std::string strActionKey = parser.getValueByKey("op");
    // Dispatch the messages
    VoiceMap<std::string, PhoneActionResultHandler>::const_iterator iterMap = m_phoneActionResultHandler.find(strActionKey);
    if (m_phoneActionResultHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(parser);
            return true;
        }
    }

    return true;
}

// QueryAgentSupport
HRESULT
VR_VoiceBoxAgentPhone::QueryAgentSupport(
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
VR_VoiceBoxAgentPhone::OnAgentResultMessage(
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
VR_VoiceBoxAgentPhone::OnAgentDriverMessage(
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
VR_VoiceBoxAgentPhone::ProcessAgentMessage(IVECIParsedMessage *pcMsg)
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
    VoiceMap<std::string, PhoneMsgHandler>::const_iterator iterMap = m_phoneMsgHandler.find(strAgentResult);
    if (m_phoneMsgHandler.cend() != iterMap) {
        if (NULL != iterMap->second) {
            (this->*(iterMap->second))(pcMsg);
            return true;
        }
    }

    return false;
}

void
VR_VoiceBoxAgentPhone::ChangeLanguage()
{
    std::string strLang;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        strLang = "en-us";
        m_strOutGoing = "outgoing";
        m_strIncoming = "incoming";
        m_strMiss = "missed";
        m_strEmail = "E-mail";
    }
    else {
        strLang = pcConfig->getVRLanguage();
        if ("en-us" == strLang) {
            m_strOutGoing = "outgoing";
            m_strIncoming = "incoming";
            m_strMiss = "missed";
            m_strEmail = "E-mail";
        }
        else if ("fr-ca" == strLang) {
            m_strOutGoing = "émis";
            m_strIncoming = "reçu";
            m_strMiss = "manqué";
            m_strEmail = "courriel";
        }
        else if ("es-mx" == strLang) {
            m_strOutGoing = "saliente";
            m_strIncoming = "entrante";
            m_strMiss = "perdida";
            m_strEmail = "E-mail";
        }
        else {
        }
    }
}

bool
VR_VoiceBoxAgentPhone::ChangeCallType(
    VR_VoiceBoxXmlParser& parser
)
{
    ChangeLanguage();
}

bool
VR_VoiceBoxAgentPhone::QuitVRInit(
    VR_VoiceBoxXmlParser& parser
)
{
    if ("" != m_strInstanceId) {
        m_strInstanceId = "";
    }
    if ("" != m_strMessageType) {
        m_strMessageType = "";
    }
}

bool
VR_VoiceBoxAgentPhone::SetIncomingMessageInfo(
    VR_VoiceBoxXmlParser& parser
)
{
    m_incomingMessageInfo.instanceId = parser.getValueByKey("instanceId");
    m_incomingMessageInfo.messageId = parser.getValueByKey("messageId");
    m_incomingMessageInfo.phoneType = parser.getValueByKey("phoneType");
    m_incomingMessageInfo.contactID = parser.getValueByKey("contactID");
    m_strMessageType = parser.getValueByKey("messageType");
    m_strSubject = parser.getValueByKey("subject");
    m_strPhoneNumber = parser.getValueByKey("phoneNumber");
    m_strInstanceId = m_incomingMessageInfo.instanceId;

    return true;
}

bool
VR_VoiceBoxAgentPhone::HFDNBest(
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
        strContent = "na_phone_contact_select_name";
    }
    else if (numberFlag) {
        strContent = "na_phone_contact_select_number";
    }
    else if (digitsFlag) {
        strContent = "na_phone_contact_select_digits";
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
VR_VoiceBoxAgentPhone::AppendDigitsAlreadyAtMax(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("AppendDigitsAlreadyAtMax-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-12", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::AppendDigitsCatch(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("AppendDigitsCatch-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-12", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::PhoneTaskComplete(
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

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallBackConfirmed(
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

    std::string strPath = m_engineCallback.GetPhonBookDBPath();
    VR_VoiceBoxPhoneData phoneData;
    std::string tempPhoneType = phoneData.GetPhoneType(m_strContactID, m_strPhoneNumber, m_strPhoneType, strPath);
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", m_strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
    if ("" != m_strContactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", m_strContactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", tempPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;

}

bool
VR_VoiceBoxAgentPhone::CallCatch(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_call_speak_name_number", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.GetPromptByScreenId("VR-HFD-16", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallContact(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_call_contact", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");

    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneType);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);

    if (NULL != strPhoneNumber.Get()) {
        VR_LOG("strPhoneNumber : %s", strPhoneNumber.Get());
    }
    if (NULL != strContactName.Get()) {
        VR_LOG("strContactName : %s", strContactName.Get());
    }
    if (NULL != strPhoneType.Get()) {
        VR_LOG("strPhoneType : %s", strPhoneType.Get());
    }
    if (NULL != strContactID.Get()) {
        VR_LOG("strContactID : %s", strContactID.Get());
    }

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
        xmlBulder.buildGivenElement(phoneInfoNode, "name", strContactName.Get(), "", "");
    }
    xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", tempPhoneType.c_str(), "", "");

    m_engineCallback.GetPromptByScreenId("VR-HFD-03", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();    

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallContactCatch(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_speak_contact", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.GetPromptByScreenId("VR-HFD-01", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallContactConfirmed(
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
    CVECIOutStr strPhoneTypeID;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneTypeID);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);

    if (NULL != strPhoneNumber.Get()) {
        VR_LOG("strPhoneNumber : %s", strPhoneNumber.Get());
    }
    if (NULL != strContactName.Get()) {
        VR_LOG("strContactName : %s", strContactName.Get());
    }
    if (NULL != strPhoneTypeID.Get()) {
        VR_LOG("strPhoneType : %s", strPhoneTypeID.Get());
    }
    if (NULL != strContactID.Get()) {
        VR_LOG("strContactID : %s", strContactID.Get());
    }

    std::string tempPhoneType;
    if ((NULL != strPhoneNumber.Get()) && (NULL != strContactID.Get()) && (NULL != strPhoneTypeID.Get())) {
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        tempPhoneType = phoneData.GetPhoneType(strContactID.Get(), strPhoneNumber.Get(), strPhoneTypeID.Get(), strPath);
    }
    if ("" == tempPhoneType) {
        tempPhoneType = strPhoneTypeID.Get();
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
VR_VoiceBoxAgentPhone::CallCurrentMsgConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallCurrentMsgConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);
    
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
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", m_strPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallIncomingMsgConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallIncomingMsgConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);


    VR_VoiceBoxPhoneData phoneData;
    std::string tempPhoneType;
    if (NULL != strPhoneNumber.Get()) {
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        tempPhoneType = phoneData.GetPhoneType(m_incomingMessageInfo.contactID, strPhoneNumber.Get(), m_incomingMessageInfo.phoneType, strPath);
    }
    if ("" == tempPhoneType) {
        tempPhoneType = m_incomingMessageInfo.phoneType;
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
    if ("" != m_incomingMessageInfo.contactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", m_incomingMessageInfo.contactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", tempPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallNumber(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_call_number", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");

    CVECIOutStr strPhoneNumber;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);

    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-06", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallNumberCatch(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_speak_phone_number", "", "");
    
    m_engineCallback.GetPromptByScreenId("VR-HFD-02", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallNumberConfirmed(
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
    CVECIOutStr strContactId;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Contact ID", &strContactId);

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
    if (NULL != strContactId.Get()) {
        std::string tempContactId = strlen(strContactId.Get()) ?  strContactId.Get() : "0";
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", tempContactId.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", "-1", "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallSelectedMsgConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallSelectedMsgConfirmed: %s", xmlMessage.Get());
    CVECIOutStr strListNumber;
    pcMsg->GetActionParameterValue("List Number", &strListNumber);
    std::string strIndex;
    if (NULL != strListNumber.Get()) {
        strIndex = strListNumber.Get();
    }
    int listNumber;
    listNumber = atoi(strIndex.c_str());
    VR_LOG("listNumber : %d", listNumber);
    std::string strPhoneType;
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        if ((*iteratorValueTwo).index == listNumber) {
            strPhoneType = (*iteratorValueTwo).phoneType;
        }
    }
    VR_LOG("strPhoneType :%s", strPhoneType.c_str());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);

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
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", strPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CallSelectedRecentConfirm(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CallSelectedRecentConfirm: %s", xmlMessage.Get());

    CVECIOutStr strListNumber;
    pcMsg->GetActionParameterValue("List Number", &strListNumber);
    std::string strIndex;
    if (NULL != strListNumber.Get()) {
        strIndex = strListNumber.Get();
    }
    VR_LOG("CallSelectedRecentConfirm   strIndex: %s", strIndex.c_str());
    std::string strContactName;
    std::string strPhoneNumber;
    std::string strPhoneType;

    VoiceVector<RecentCallsInfo>::iterator callsVector;
    for (callsVector = m_recentCallsInfo.begin(); callsVector != m_recentCallsInfo.end(); ++callsVector) {
        if (0 == strcmp((*callsVector).strIndex.c_str(), strIndex.c_str())) {
            strContactName = (*callsVector).contact;
            strPhoneNumber = (*callsVector).phoneNumber;
            strPhoneType = (*callsVector).phoneType;
            VR_LOG("CallSelectedRecentConfirm   strContactName: %s", strContactName.c_str());
            VR_LOG("CallSelectedRecentConfirm   strPhoneNumber: %s", strPhoneNumber.c_str());
            VR_LOG("CallSelectedRecentConfirm   strPhoneType: %s", strPhoneType.c_str());
        }
    }

    int listNumber;
    listNumber = atoi(strIndex.c_str());
    std::string contactID;
    std::string phoneTypeID;
    VoiceVector<CallHistory>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_callHistory.begin(); iteratorValueTwo != m_callHistory.end(); ++iteratorValueTwo) {
        if ((*iteratorValueTwo).index == listNumber) {
            contactID = (*iteratorValueTwo).contactID;
            phoneTypeID = (*iteratorValueTwo).phoneType;
        }
    }
    VR_LOG("contactID :%s", contactID.c_str());
    std::string tempPhoneType;
    std::string strPath = m_engineCallback.GetPhonBookDBPath();
    VR_VoiceBoxPhoneData phoneData;
    tempPhoneType = phoneData.GetPhoneType(contactID, strPhoneNumber, phoneTypeID, strPath);
    VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");

    xmlBulder.buildGivenElement(node, "dialType", "CALL_RECENT", "", "");
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "phoneInfo", "", "", "");

    xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.c_str(), "", "");
    if ("" != contactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", contactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", tempPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;

}

bool
VR_VoiceBoxAgentPhone::CannotAddVoiceTag(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CannotAddVoiceTag-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-16", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::CannotReplytoCurrentMessage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("CannotReplytoCurrentMessage-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-23", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ChooseVoiceTagContact(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ChooseVoiceTagContact: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "showAddVoiceTagUI");


    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ChooseVoiceTagContact: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ConfirmSMSRecipient(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_sms_recipient", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneType);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);

    if (NULL != strPhoneNumber.Get()) {
        VR_LOG("strPhoneNumber : %s", strPhoneNumber.Get());
    }
    if (NULL != strContactName.Get()) {
        VR_LOG("strContactName : %s", strContactName.Get());
    }
    if (NULL != strPhoneType.Get()) {
        VR_LOG("strPhoneType : %s", strPhoneType.Get());
    }
    if (NULL != strContactID.Get()) {
        VR_LOG("strContactID : %s", strContactID.Get());
    }

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
        xmlBulder.buildGivenElement(phoneInfoNode, "name", strContactName.Get(), "", "");
    }
    xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", tempPhoneType.c_str(), "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-13", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::DigitsCorrection(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_speak_digits", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");

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
VR_VoiceBoxAgentPhone::HFDHome(
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
VR_VoiceBoxAgentPhone::HFDKeyboardDictation(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxAgentPhone::BuildListHeader(IVECIParsedMessage *pcMsg,
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

    std::string strStartIndex = GetStartIndex(pcMsg);
    xmlBulder.buildGivenElement(nodeHeader, "startIndex", strStartIndex, "", "");
    xmlBulder.buildGivenElement(nodeHeader, "pageSize", "5", "", "");

    if (NULL != strListCount.Get()) {
        xmlBulder.buildGivenElement(nodeHeader, "count", strListCount.Get(), "", "");
    }

    return;
}

bool
VR_VoiceBoxAgentPhone::ListMessages(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_strInstanceId = "";
    m_strMessageType = "";
    m_pcMsg = pcMsg;
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ListMessages: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_list_messages", "", "");
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

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

            CVECIOutStr strPhoneNumber;
            CVECIOutStr strListIndex;
            CVECIOutStr strEntry;
            CVECIOutStr strSender;
            CVECIOutStr strTimeStamp;

            pcMsg->GetListItemParameterValue(i, "Phone Number", &strPhoneNumber);
            pcMsg->GetListItemParameterValue(i, "List Index", &strListIndex);
            pcMsg->GetListItemParameterValue(i, "Entry", &strEntry);
            pcMsg->GetListItemParameterValue(i, "Sender", &strSender);
            pcMsg->GetListItemParameterValue(i, "Time Stamp", &strTimeStamp);

            node.Name = "listindex";
            if (NULL != strListIndex.Get()) {
                node.Value = strListIndex.Get();
            }
            attributeVector.push_back(node);

            node.Name = "entry";
            if (NULL != strEntry.Get()) {
                node.Value = strEntry.Get();
                if ("" == node.Value) {
                    VR_LOG("number");
                    if (NULL != strPhoneNumber.Get()) {
                        node.Value = strPhoneNumber.Get();
                    }
                }
            }
            attributeVector.push_back(node);

            std::string tempName;
            node.Name = "sender";
            if (NULL != strSender.Get()) {
                node.Value = strSender.Get();
                tempName = strSender.Get();
                if ("" == node.Value) {
                    VR_LOG("number");
                    if (NULL != strPhoneNumber.Get()) {
                        node.Value = strPhoneNumber.Get();
                    }
                }
            }
            attributeVector.push_back(node);

            std::string phoneNameValue = "true";
            if ("" == tempName) {
                phoneNameValue = "false";
            }
            node.Name = "phonenamevalue";
            node.Value = phoneNameValue;
            attributeVector.push_back(node);

            node.Name = "timestamp";
            if (NULL != strTimeStamp.Get()) {
                node.Value = strTimeStamp.Get();
            }
            attributeVector.push_back(node);
            xmlBulder.buildListItemChildElement(nodeItems, "messageinfo", actionListMessages, attributeVector);
        }
    }

    m_engineCallback.GetPromptByScreenId("VR-HFD-20", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ListPartialNameMatches(
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

    bool bPhoneNum = false;
    bool bPhoneType = false;
    bool bPhoneTypeID = false;
    VBT_ULONG uSize = 0;
    if (NULL != strListType.Get()) {
        pcMsg->GetListSize(&uSize);
        CVECIOutStr strPhoneNum;
        CVECIOutStr strPhoneType;
        CVECIOutStr strPhoneTypeID;
        if (uSize > 0) {
            pcMsg->GetListItemParameterValue(0, "Phone Number", &strPhoneNum);
            pcMsg->GetListItemParameterValue(0, "Phone Type", &strPhoneType);
            pcMsg->GetListItemParameterValue(0, "Phone Type ID", &strPhoneTypeID);

            if (NULL != strPhoneNum.Get()) {
                bPhoneNum = true;
            }
            if (NULL != strPhoneType.Get()) {
                bPhoneType = true;
            }
            if (NULL != strPhoneTypeID.Get()) {
                bPhoneTypeID = true;
            }
        }
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");

    if (bPhoneNum || bPhoneType || bPhoneTypeID) {
        xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_select_name_and_type_matches_call_sms", "", "");
        m_engineCallback.GetPromptByScreenId("VR-HFD-08", m_strPrompt);
    }
    else {
        xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_select_name_matches_call_sms", "", "");
        m_engineCallback.GetPromptByScreenId("VR-HFD-05", m_strPrompt);
    }
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");


    if (NULL != strListType.Get()) {
        pugi::xml_node nodeList = xmlBulder.buildGivenElement(selectListNode, "list", "", "", "");

        BuildListHeader(pcMsg, xmlBulder, nodeList);

        pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

        VoiceVector<StructNode>::type attributeVector;
        StructNode node;
        CVECIOutStr strContactName;
        CVECIOutStr strContactID;
        CVECIOutStr strPhoneNum;
        CVECIOutStr strPhoneType;
        CVECIOutStr strPhoneTypeID;

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

            // if (bPhoneNum) {
            //     pcMsg->GetListItemParameterValue(i, "Phone Number", &strPhoneNum);
            //     node.Name = "phoneNum";
            //     if (NULL != strPhoneNum.Get()) {
            //         node.Value = strPhoneNum.Get();
            //         attributeVector.push_back(node);
            //     }
            //     else {
            //         VR_ERROR("Data not Matches!");
            //         return false;
            //     }
            // }

            // if (bPhoneType) {
            //     pcMsg->GetListItemParameterValue(i, "Phone Type", &strPhoneType);
            //     node.Name = "phoneType";
            //     if (NULL != strPhoneType.Get()) {
            //         node.Value = strPhoneType.Get();
            //         attributeVector.push_back(node);
            //     }
            //     else {
            //         VR_VBT_ERROR("Data not Matches!");
            //         return false;
            //     }
            // }

            // if (bPhoneTypeID) {
            //     pcMsg->GetListItemParameterValue(i, "Phone Type ID", &strPhoneTypeID);
            //     node.Name = "phoneTypeId";
            //     if (NULL != strPhoneTypeID.Get()) {
            //         node.Value = strPhoneTypeID.Get();
            //         attributeVector.push_back(node);
            //     }
            //     else {
            //         VR_VBT_ERROR("Data not Matches!");
            //         return false;
            //     }
            // }

            xmlBulder.buildListItemChildElement(nodeItems, "nameMatchInfo", nameMatchInfo, attributeVector);
        }
    }

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ListPhoneTypes(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_contact_type_select", "", "");
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

            if (NULL != strPhoneNumber.Get()) {
                VR_LOG("strPhoneNumber : %s", strPhoneNumber.Get());
            }
            if (NULL != strContactName.Get()) {
                VR_LOG("strContactName : %s", strContactName.Get());
            }
            if (NULL != strPhoneType.Get()) {
                VR_LOG("strPhoneType : %s", strPhoneType.Get());
            }
            if (NULL != strContactID.Get()) {
                VR_LOG("strContactID : %s", strContactID.Get());
            }

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
            attributeVector.push_back(node);

            node.Name = "number";
            if (NULL != strPhoneNumber.Get()) {
                node.Value = strPhoneNumber.Get();
            }
            attributeVector.push_back(node);

            node.Name = "name";
            if (NULL != strContactName.Get()) {
                node.Value = strContactName.Get();
                name = strContactName.Get();
            }
            attributeVector.push_back(node);

            xmlBulder.buildListItemChildElement(nodeItems, "phoneinfo", phoneTypeInfo, attributeVector);
        }
    }
    xmlBulder.buildGivenElement(xmlNode, "prompt", name, "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-18", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ListRecentCalls(
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
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_list_recent_calls", "", "");

    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

    CVECIOutStr strListType;
    (void)pcMsg->GetListType(&strListType);
    RecentCallsInfo recentCallsInfo;
    if (NULL != strListType.Get()) {
        pugi::xml_node nodeList = xmlBulder.buildGivenElement(selectListNode, "list", "", "", "");

        BuildListHeader(pcMsg, xmlBulder, nodeList);

        pugi::xml_node nodeItems = xmlBulder.buildGivenElement(nodeList, "items", "", "", "");

        VBT_ULONG uSize = 0;
        pcMsg->GetListSize(&uSize);
        for (VBT_ULONG i = 0; i < uSize; ++i) {
            VoiceVector<StructNode>::type attributeVector;
            StructNode node;
            CVECIOutStr strListIndex;
            CVECIOutStr strCallType;
            CVECIOutStr strEntry;
            CVECIOutStr strPhoneNumber;
            CVECIOutStr strPhoneType;
            CVECIOutStr strTimeStamp;

            pcMsg->GetListItemParameterValue(i, "List Index", &strListIndex);
            pcMsg->GetListItemParameterValue(i, "Call Type", &strCallType);
            pcMsg->GetListItemParameterValue(i, "Entry", &strEntry);
            pcMsg->GetListItemParameterValue(i, "Phone Number", &strPhoneNumber);
            pcMsg->GetListItemParameterValue(i, "Phone Type", &strPhoneType);
            pcMsg->GetListItemParameterValue(i, "Time Stamp", &strTimeStamp);

            node.Name = "listindex";
            std::string tempIndex;
            if (NULL != strListIndex.Get()) {
                tempIndex = strListIndex.Get();
                node.Value = strListIndex.Get();
                VR_LOG("listindex : %s", strListIndex.Get());
                recentCallsInfo.strIndex = strListIndex.Get();
            }
            attributeVector.push_back(node);

            int listNumber;
            listNumber = atoi(tempIndex.c_str());
            std::string tempName;
            std::string contactID;
            VoiceVector<CallHistory>::iterator iteratorValueTwo;
            for (iteratorValueTwo = m_callHistory.begin(); iteratorValueTwo != m_callHistory.end(); ++iteratorValueTwo) {
                if ((*iteratorValueTwo).index == listNumber) {
                    tempName = (*iteratorValueTwo).contactName;
                    contactID = (*iteratorValueTwo).contactID;
                }
            }
            std::string phoneNameValue;
            if ("" == tempName) {
                phoneNameValue = "false";
            }
            else {
                phoneNameValue = "true";
            }
            node.Name = "phonenamevalue";
            node.Value = phoneNameValue;
            attributeVector.push_back(node);
            node.Name = "calltype";
            if (NULL != strCallType.Get()) {
                node.Value = "";
                std::string callType = strCallType.Get();
                if ("" == callType) {
                    node.Value = "-1";
                }
                if (m_strOutGoing == callType) {
                    node.Value = "0";
                }
                if (m_strIncoming == callType) {
                    node.Value = "1";
                }
                if (m_strMiss == callType) {
                    node.Value = "2";
                }
                VR_LOG("calltype : %s", node.Value.c_str());
            }
            attributeVector.push_back(node);
            node.Name = "entry";
            if ("" == tempName) {
                if (NULL != strPhoneNumber.Get()) {
                    std::string tempNumber = strPhoneNumber.Get();
                    if ("" == tempNumber) {
                        node.Value = "";
                    }
                    else {
                        node.Value = tempNumber;
                    }
                }
            }
            else {
                node.Value = tempName;
            }
            attributeVector.push_back(node);
            node.Name = "phonenumber";
            if (NULL != strPhoneNumber.Get()) {
                node.Value = strPhoneNumber.Get();
                recentCallsInfo.phoneNumber = strPhoneNumber.Get();
            }
            attributeVector.push_back(node);

            std::string tempPhoneType;
            if ((NULL != strPhoneNumber.Get()) && (NULL != strPhoneType.Get())) {
                std::string strPath = m_engineCallback.GetPhonBookDBPath();
                VR_VoiceBoxPhoneData phoneData;
                VR_LOG("strPhoneType.Get(), : %s", strPhoneType.Get());
                tempPhoneType = phoneData.GetPhoneType(contactID, strPhoneNumber.Get(), strPhoneType.Get(), strPath);
            }
            if ("" == tempPhoneType) {
                tempPhoneType = strPhoneType.Get();
            }
            VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());

            node.Name = "phoneType";
            node.Value = tempPhoneType;
            if (NULL != strPhoneType.Get()) {
                recentCallsInfo.phoneType = strPhoneType.Get();
            }
            attributeVector.push_back(node);

            node.Name = "timestamp";
            if (NULL != strTimeStamp.Get()) {
                node.Value = strTimeStamp.Get();
            }
            attributeVector.push_back(node);
            m_recentCallsInfo.push_back(recentCallsInfo);
            xmlBulder.buildListItemChildElement(nodeItems, "callinfo", listRecentCalls, attributeVector);
        }
    }

    m_engineCallback.GetPromptByScreenId("VR-HFD-19", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::MessageDictation(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_reply_send_msg", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node callInfoNode = xmlBulder.buildGivenElement(detailNode, "callinfo", "", "", "");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type", &strPhoneType);

    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "messagebody", strMessageBody.Get(), "", "");
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    std::string tempName;
    if (NULL != strContactName.Get()) {
        tempName = strContactName.Get();
        std::string name = strContactName.Get();
        if ("" == name) {
            if (NULL != strPhoneNumber.Get()) {
                xmlBulder.buildGivenElement(callInfoNode, "name", strPhoneNumber.Get(), "", "");
            }
        }
        else {
            xmlBulder.buildGivenElement(callInfoNode, "name", strContactName.Get(), "", "");
        }
    }
    if ("" == tempName) {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "false", "", "");
    }
    else {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "true", "", "");
    }
    if (NULL != strPhoneType.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "phoneType", strPhoneType.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("MessageDictation: %s", strReply.c_str());
    m_engineCallback.GetPromptByScreenId("VR-HFD-10", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::MuteCall(
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

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "muteCall");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::MuteOff(
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

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "muteOff");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::NoContactNumbers(
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
VR_VoiceBoxAgentPhone::NoContactsFound(
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
VR_VoiceBoxAgentPhone::NoQuickReplyMessages(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("NoQuickReplyMessages-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-02", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::OffboardDictation(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_dictate_sms", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type", &strPhoneType);
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "name", strContactName.Get(), "", "");
    }
    if (NULL != strPhoneType.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", strPhoneType.Get(), "", "");
    }
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-17", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::OneCallSelectedCallConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("OneCallSelectedCallConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "dial");

    xmlBulder.buildGivenElement(node, "dialType", "CALL_SHORTRING", "", "");
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "phoneInfo", "", "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", m_strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
    if ("" != m_strContactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", m_strContactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    // previout have set phontype
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", m_strPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReadIncomingMessage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    std::string strScreen = "<display agent=\"Common\" content=\"ScreenDisplay\">"
    "<agent>phone</agent>"
    "<content>na_phone_in_message</content>"
    "</display>";
    m_engineCallback.OnRecognized(strScreen);

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReadIncomingMessage-Stay in this screen: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "getMessageDetail");
    xmlBulder.buildGivenElement(node, "instanceId", m_incomingMessageInfo.instanceId, "", "");
    xmlBulder.buildGivenElement(node, "messageId", m_incomingMessageInfo.messageId, "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReadSelectedMessage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReadSelectedMessage: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_one_message_selected", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node oneMessageInfo = xmlBulder.buildGivenElement(detailNode, "onemessageinfo", "", "", "");
    CVECIOutStr strSender;
    CVECIOutStr strTimeStamp;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strBody;
    CVECIOutStr strPhoneType;

    pcMsg->GetActionParameterValue("Sender", &strSender);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Time Stamp", &strTimeStamp);
    pcMsg->GetActionParameterValue("Body", &strBody);
    pcMsg->GetActionParameterValue("Phone Type", &strPhoneType);

    CVECIOutStr strListNumber;

    pcMsg->GetActionParameterValue("List Number", &strListNumber);
    std::string strIndex;
    if (NULL != strListNumber.Get()) {
        strIndex = strListNumber.Get();
        int listNumber;
        listNumber = atoi(strIndex.c_str());
        VR_LOG("listNumber : %d", listNumber);
        VoiceVector<MessageInfo>::iterator iteratorValueTwo;
        for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
            if ((*iteratorValueTwo).index == listNumber) {
                m_strInstanceId = (*iteratorValueTwo).instanceId;
                m_strPhoneNumber = (*iteratorValueTwo).phoneNumber;
                m_strSubject = (*iteratorValueTwo).subject;
                m_strMessageType = (*iteratorValueTwo).messageType;
            }
        }
        VR_LOG("m_strInstanceId : %s", m_strInstanceId.c_str());
    }
    if (NULL != strPhoneType.Get()) {
        m_strPhoneType = strPhoneType.Get();
    }
    std::string tempName;
    if (NULL != strSender.Get()) {
        tempName = strSender.Get();
        std::string name = strSender.Get();
        if ("" == name) {
            if (NULL != strPhoneNumber.Get()) {
                xmlBulder.buildGivenElement(oneMessageInfo, "name", strPhoneNumber.Get(), "", "");
            }
        }
        else {
            xmlBulder.buildGivenElement(oneMessageInfo, "name", strSender.Get(), "", "");
        }
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(oneMessageInfo, "phonenumber", strPhoneNumber.Get(), "", "");
    }
    if ("" == tempName) {
        xmlBulder.buildGivenElement(oneMessageInfo, "phonenamevalue", "false", "", "");
    }
    else {
        xmlBulder.buildGivenElement(oneMessageInfo, "phonenamevalue", "true", "", "");
    }
    if (NULL != strTimeStamp.Get()) {
        xmlBulder.buildGivenElement(oneMessageInfo, "timestamp", strTimeStamp.Get(), "", "");
    }
    if (NULL != strBody.Get()) {
        xmlBulder.buildGivenElement(oneMessageInfo, "body", strBody.Get(), "", "");
    }

    std::string strMsgAvailable = m_engineCallback.GetMessageAvailable() ? "1" : "0";

    VR_LOG("msgavailable-nextitem-previousitem : %s, %s, %s", strMsgAvailable.c_str(), m_strNextItem.c_str(), m_strPreviousItem.c_str());
    xmlBulder.buildGivenElement(oneMessageInfo, "msgavailable", strMsgAvailable.c_str(), "", "");
    xmlBulder.buildGivenElement(oneMessageInfo, "nextitem", m_strNextItem.c_str(), "", "");
    xmlBulder.buildGivenElement(oneMessageInfo, "previousitem", m_strPreviousItem.c_str(), "", "");
    
    m_engineCallback.GetPromptByScreenId("VR-HFD-23", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReadSelectMsgInvalidListNum(
    IVECIParsedMessage *pcMsg
)
{

    /* CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReadSelectMsgInvalidListNum: %s", xmlMessage.Get());

    pcMsg = m_pcMsg;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReadSelectMsgInvalidListNum: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "phone_list_messages", "", "");
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

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
            CVECIOutStr strListIndex;
            CVECIOutStr strEntry;
            CVECIOutStr strSender;
            CVECIOutStr strTimeStamp;

            pcMsg->GetListItemParameterValue(i, "List Index", &strListIndex);
            pcMsg->GetListItemParameterValue(i, "Entry", &strEntry);
            pcMsg->GetListItemParameterValue(i, "Sender", &strSender);
            pcMsg->GetListItemParameterValue(i, "Time Stamp", &strTimeStamp);

            node.Name = "listindex";
            if (NULL != strListIndex.Get()) {
                node.Value = strListIndex.Get();
            }
            attributeVector.push_back(node);

            node.Name = "entry";
            if (NULL != strEntry.Get()) {
                node.Value = strEntry.Get();
            }
            attributeVector.push_back(node);

            node.Name = "sender";
            if (NULL != strSender.Get()) {
                node.Value = strSender.Get();
            }
            attributeVector.push_back(node);

            node.Name = "timestamp";
            if (NULL != strTimeStamp.Get()) {
                node.Value = strTimeStamp.Get();
            }
            attributeVector.push_back(node);
            xmlBulder.buildListItemChildElement(nodeItems, "messageinfo", actionListMessages, attributeVector);
        }
    }

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);
*/
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReadSelectMsgInvalidListNum-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-20", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::RedialConfirmed(
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

    std::string strPath = m_engineCallback.GetPhonBookDBPath();
    VR_VoiceBoxPhoneData phoneData;
    std::string tempPhoneType = phoneData.GetPhoneType(m_strContactID, m_strPhoneNumber, m_strPhoneType, strPath);
    xmlBulder.buildGivenElement(nodePhoneInfo, "contactName", m_strContactName.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
    if ("" != m_strContactID) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", m_strContactID.c_str(), "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "contactId", "0", "", "");
    }
    xmlBulder.buildGivenElement(nodePhoneInfo, "phoneType", tempPhoneType.c_str(), "", "");
    xmlBulder.buildGivenElement(nodePhoneInfo, "listType", "0", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReplywithDictatedMessage(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_reply_send_msg", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node callInfoNode = xmlBulder.buildGivenElement(detailNode, "callinfo", "", "", "");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);

    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "messagebody", strMessageBody.Get(), "", "");
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    std::string tempName;
    if (NULL != strContactName.Get()) {
        tempName = strContactName.Get();
        VR_LOG("tempName : %s", tempName.c_str());
        std::string name = strContactName.Get();
        if ("" == name) {
            if (NULL != strPhoneNumber.Get()) {
                xmlBulder.buildGivenElement(callInfoNode, "name", strPhoneNumber.Get(), "", "");
            }
        }
        else {
            xmlBulder.buildGivenElement(callInfoNode, "name", strContactName.Get(), "", "");
        }
    }
    if ("" == tempName) {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "false", "", "");
    }
    else {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "true", "", "");
    }
    m_engineCallback.GetPromptByScreenId("VR-HFD-10", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReplywithDictatedMsgConfirm(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReplywithDictatedMsgConfirm: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "sendMessage");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);

    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "messageInfo", "", "", "");
    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageBody", strMessageBody.Get(), "", "");
    }
    if ("" == m_strInstanceId) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", "0", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", m_strInstanceId.c_str(), "", "");
    }
    if ("" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", "2", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", m_strMessageType.c_str(), "", "");
    }
    if ("1" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
        xmlBulder.buildGivenElement(nodePhoneInfo, "name", m_strSubject.c_str(), "", "");
    }
    else {
        if (NULL != strPhoneNumber.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
        }
        if (NULL != strContactName.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "name", strContactName.Get(), "", "");
        }
    }
    m_strSubject = "";
    m_strMessageType = "";
    m_strInstanceId = "";
    m_strPhoneNumber = "";
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ReplywithDictatedMsgConfirm: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReplywithDictatedMsgReject(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_dictate_sms", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strMessageBody;

    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);
    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "name", strContactName.Get(), "", "");
    }
    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "messagebody", strMessageBody.Get(), "", "");
    }

    m_engineCallback.GetPromptByScreenId("VR-HFD-17", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReplywithQuickReplyMessage(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReplywithQuickReplyMessage: %s", xmlMessage.Get());

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_confirm_reply_send_msg", "", "");
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node callInfoNode = xmlBulder.buildGivenElement(detailNode, "callinfo", "", "", "");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Phone Type ID", &strPhoneType);

    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "messagebody", strMessageBody.Get(), "", "");
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "number", strPhoneNumber.Get(), "", "");
    }
    std::string tempName;
    if (NULL != strContactName.Get()) {
        tempName = strContactName.Get();
        std::string name = strContactName.Get();
        if ("" == name) {
            if (NULL != strPhoneNumber.Get()) {
                xmlBulder.buildGivenElement(callInfoNode, "name", strPhoneNumber.Get(), "", "");
            }
        }
        else {
            xmlBulder.buildGivenElement(callInfoNode, "name", strContactName.Get(), "", "");
        }
    }
    if ("" == tempName) {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "false", "", "");
    }
    else {
        xmlBulder.buildGivenElement(callInfoNode, "phonenamevalue", "true", "", "");
    }
    if (NULL != strPhoneType.Get()) {
        xmlBulder.buildGivenElement(callInfoNode, "phoneType", strPhoneType.Get(), "", "");
    }

    m_engineCallback.GetPromptByScreenId("VR-HFD-10", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ReplywithQuickReplyMessage: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::ReplywithQuickMsgConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("ReplywithQuickMsgConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "sendMessage");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Sender", &strContactName);
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "messageInfo", "", "", "");
    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageBody", strMessageBody.Get(), "", "");
    }
    if ("" == m_strInstanceId) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", "0", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", m_strInstanceId.c_str(), "", "");
    }
    if ("" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", "2", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", m_strMessageType.c_str(), "", "");
    }
    if ("1" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
        xmlBulder.buildGivenElement(nodePhoneInfo, "name", m_strSubject.c_str(), "", "");
    }
    else {
        if (NULL != strPhoneNumber.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
        }
        if (NULL != strContactName.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "name", strContactName.Get(), "", "");
        }
    }
    m_strSubject = "";
    m_strMessageType = "";
    m_strInstanceId = "";
    m_strPhoneNumber = "";
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("ReplywithQuickMsgConfirmed: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SelectQuickReply(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SelectQuickReply: %s", xmlMessage.Get());

    CVECIOutStr strListNumber;

    pcMsg->GetActionParameterValue("List Number", &strListNumber);
    std::string strIndex;
    if (NULL != strListNumber.Get()) {
        strIndex = strListNumber.Get();
        int listNumber;
        listNumber = atoi(strIndex.c_str());
        VR_LOG("listNumber : %d", listNumber);
        VoiceVector<MessageInfo>::iterator iteratorValueTwo;
        for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
            if ((*iteratorValueTwo).index == listNumber) {
                m_strMessageType = (*iteratorValueTwo).messageType;
                m_strInstanceId = (*iteratorValueTwo).instanceId;
                m_strPhoneNumber = (*iteratorValueTwo).phoneNumber;
                m_strSubject = (*iteratorValueTwo).subject;
            }
        }
    }

    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_select_quick_reply", "", "");
    pugi::xml_node selectListNode = xmlBulder.buildGivenElement(xmlNode, "selectList", "", "", "");

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
            CVECIOutStr strListIndex;
            CVECIOutStr strMessageBody;

            pcMsg->GetListItemParameterValue(i, "List Index", &strListIndex);
            pcMsg->GetListItemParameterValue(i, "Message Body", &strMessageBody);


            node.Name = "listindex";
            if (NULL != strListIndex.Get()) {
                node.Value = strListIndex.Get();
            }
            attributeVector.push_back(node);

            node.Name = "messagebody";
            if (NULL != strMessageBody.Get()) {
                node.Value = strMessageBody.Get();
            }
            attributeVector.push_back(node);

            xmlBulder.buildListItemChildElement(nodeItems, "quickmessageinfo", quickReplytoCurrentMessage, attributeVector);
        }
    }

    m_engineCallback.GetPromptByScreenId("VR-HFD-21", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SelectRecCallNoContactInfo(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SelectRecCallNoContactInfo: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node xmlNode = xmlBulder.buildDisplayElement("Common", "ScreenDisplay");
    xmlBulder.buildGivenElement(xmlNode, "agent", "phone", "", "");
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_one_call_contact_message_selected", "", "");
    CVECIOutStr strActionType;
    pcMsg->GetActionType(&strActionType);
    if (NULL != strActionType.Get()) {
        xmlBulder.buildGivenElement(xmlNode, "screenType", strActionType.Get(), "", "");
    }
    CVECIOutStr strContactName;
    CVECIOutStr strCallType;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strPhoneType;
    CVECIOutStr strTimeStamp;
    CVECIOutStr strContactID;

    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pcMsg->GetActionParameterValue("Call Type", &strCallType);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Phone Type", &strPhoneType);
    pcMsg->GetActionParameterValue("Time Stamp", &strTimeStamp);
    pcMsg->GetActionParameterValue("Contact ID", &strContactID);
    m_strContactName.clear();
    m_strPhoneNumber.clear();
    m_strPhoneType.clear();
    m_strContactID.clear();
    std::string tempPhoneType;
    if ((NULL != strPhoneNumber.Get()) && (NULL != strContactID.Get()) && (NULL != strPhoneType.Get())) {
        m_strContactID = strContactID.Get();
        std::string strPath = m_engineCallback.GetPhonBookDBPath();
        VR_VoiceBoxPhoneData phoneData;
        tempPhoneType = phoneData.GetPhoneType(strContactID.Get(), strPhoneNumber.Get(), strPhoneType.Get(), strPath);
    }
    if ("" == tempPhoneType) {
        tempPhoneType = strPhoneType.Get();
    }
    VR_LOG("tempPhoneType : %s", tempPhoneType.c_str());

    if (NULL != strCallType.Get()) {
        xmlBulder.buildGivenElement(xmlNode, "prompt", strCallType.Get(), "", "");
    }
    pugi::xml_node detailNode = xmlBulder.buildGivenElement(xmlNode, "detail", "", "", "");
    pugi::xml_node phoneInfoNode = xmlBulder.buildGivenElement(detailNode, "phoneinfo", "", "", "");
    if (NULL != strCallType.Get()) {
        std::string callType = strCallType.Get();
        if ("" == callType) {
            xmlBulder.buildGivenElement(phoneInfoNode, "calltype", "-1", "", "");
        }
        if (m_strOutGoing == callType) {
            xmlBulder.buildGivenElement(phoneInfoNode, "calltype", "0", "", "");
        }
        if (m_strIncoming == callType) {
            xmlBulder.buildGivenElement(phoneInfoNode, "calltype", "1", "", "");
        }
        if (m_strMiss == callType) {
            xmlBulder.buildGivenElement(phoneInfoNode, "calltype", "2", "", "");
        }
    }
    if (NULL != strContactName.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "name", strContactName.Get(), "", "");
        m_strContactName = strContactName.Get();
    }
    if (NULL != strPhoneNumber.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "number", strPhoneNumber.Get(), "", "");
        m_strPhoneNumber = strPhoneNumber.Get();
    }
    m_strPhoneType = tempPhoneType;
    xmlBulder.buildGivenElement(phoneInfoNode, "phoneType", tempPhoneType.c_str(), "", "");
    if (NULL != strTimeStamp.Get()) {
        xmlBulder.buildGivenElement(phoneInfoNode, "timestamp", strTimeStamp.Get(), "", "");
    }
    std::string strMsgAvailable = m_engineCallback.GetMessageAvailable() ? "1" : "0";

    VR_LOG("strMsgAvailable : %s", strMsgAvailable.c_str());
    xmlBulder.buildGivenElement(phoneInfoNode, "msgavailable", strMsgAvailable.c_str(), "", "");
    m_engineCallback.GetPromptByScreenId("VR-HFD-22", m_strPrompt);

    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SendDictatedMessageConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SendDictatedMessageConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "sendMessage");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;
    CVECIOutStr strPhoneType;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "messageInfo", "", "", "");
    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageBody", strMessageBody.Get(), "", "");
    }
    if ("" == m_strInstanceId) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", "0", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", m_strInstanceId.c_str(), "", "");
    }
    if ("" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", "2", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", m_strMessageType.c_str(), "", "");
    }
    if ("1" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
        xmlBulder.buildGivenElement(nodePhoneInfo, "name", m_strSubject.c_str(), "", "");
    }
    else {
        if (NULL != strPhoneNumber.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
        }
        if (NULL != strContactName.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "name", strContactName.Get(), "", "");
        }
    }
    m_strSubject = "";
    m_strMessageType = "";
    m_strInstanceId = "";
    m_strPhoneNumber = "";
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("SendDictatedMessageConfirmed: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SendDTMF(
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
VR_VoiceBoxAgentPhone::SendDTMFCatch(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_send_dtmf", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.GetPromptByScreenId("VR-HFD-14", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SendQuickReplyMsgConfirmed(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SendQuickReplyMsgConfirmed: %s", xmlMessage.Get());
    VR_VoiceBoxXmlBuilder xmlBulder;
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "sendMessage");

    CVECIOutStr strMessageBody;
    CVECIOutStr strPhoneNumber;
    CVECIOutStr strContactName;

    pcMsg->GetActionParameterValue("Message Body", &strMessageBody);
    pcMsg->GetActionParameterValue("Phone Number", &strPhoneNumber);
    pcMsg->GetActionParameterValue("Contact Name", &strContactName);
    pugi::xml_node nodePhoneInfo = xmlBulder.buildGivenElement(node, "messageInfo", "", "", "");
    if (NULL != strMessageBody.Get()) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageBody", strMessageBody.Get(), "", "");
    }
    if ("" == m_strInstanceId) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", "0", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "instanceId", m_strInstanceId.c_str(), "", "");
    }
    if ("" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", "2", "", "");
    }
    else {
        xmlBulder.buildGivenElement(nodePhoneInfo, "messageType", m_strMessageType.c_str(), "", "");
    }
    if ("1" == m_strMessageType) {
        xmlBulder.buildGivenElement(nodePhoneInfo, "number", m_strPhoneNumber.c_str(), "", "");
        xmlBulder.buildGivenElement(nodePhoneInfo, "name", m_strSubject.c_str(), "", "");
    }
    else {
        if (NULL != strPhoneNumber.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "number", strPhoneNumber.Get(), "", "");
        }
        if (NULL != strContactName.Get()) {
            xmlBulder.buildGivenElement(nodePhoneInfo, "name", strContactName.Get(), "", "");
        }
    }
    m_strSubject = "";
    m_strMessageType = "";
    m_strInstanceId = "";
    m_strPhoneNumber = "";
    std::string strReply = xmlBulder.getXmlString();
    VR_LOG("SendQuickReplyMsgConfirmed: %s", strReply.c_str());
    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SendSMSNoChangePhoneTypes(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SendSMSNoChangePhoneTypes-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone::SMSNotAvailable(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("SMSNotAvailable-Stay in this screen: %s", xmlMessage.Get());

    m_engineCallback.GetPromptByScreenId("VR-HFD-12", m_strPrompt);

    return true;
}

bool
VR_VoiceBoxAgentPhone::SMSRecipientRejected(
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
    xmlBulder.buildGivenElement(xmlNode, "content", "na_phone_search_contacts_sms", "", "");
    std::string strReply = xmlBulder.getXmlString();

    m_engineCallback.GetPromptByScreenId("VR-HFD-11", m_strPrompt);

    m_engineCallback.OnRecognized(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::VoiceTagSIDError(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("VoiceTagSIDError-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone::StartVoiceTag(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("StartVoiceTag-Stay in this screen: %s", xmlMessage.Get());

    return true;
}

bool
VR_VoiceBoxAgentPhone::GetNextMessageRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    m_requestType.clear();
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("GetNextMessageRequest: %s", xmlMessage.Get());
    m_listNumber = m_listNumber + 1;
    VR_LOG("m_listNumber: %d", m_listNumber);
    m_strNextItem.clear();
    m_strPreviousItem.clear();
    m_strNextItem = "1";
    m_strPreviousItem = "1";
    if (1 == m_listNumber) {
        m_strPreviousItem = "0";
    }
    VR_LOG("m_messageInfoVector size : %d", m_messageInfoVector.size());
    if (m_messageInfoVector.size() < m_listNumber) {
        std::string strReply = "<action-result agent=\"phone\" op=\"getMessageDetail\" errcode=\"ALREADY_ON_LAST_MESSAGE\"/>";
        VR_VoiceBoxXmlParser parser(strReply);
        ReplyQueryInfo(parser);
    }
    if (m_messageInfoVector.size() == m_listNumber) {
        m_strNextItem = "0";
    }
    VR_VoiceBoxXmlBuilder xmlBulder;
    std::string strInstanceId;
    std::string strMessageId;
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        if ((*iteratorValueTwo).index == m_listNumber) {
            strInstanceId = (*iteratorValueTwo).instanceId;
            strMessageId = (*iteratorValueTwo).messageId;
        }
    }
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "getMessageDetail");
    m_requestType = "GetNextMessageRequest";
    xmlBulder.buildGivenElement(node, "instanceId", strInstanceId, "", "");
    xmlBulder.buildGivenElement(node, "messageId", strMessageId, "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::GetPreviousMessageRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("GetPreviousMessageRequest: %s", xmlMessage.Get());
    m_listNumber = m_listNumber - 1;
    VR_LOG("m_listNumber: %d", m_listNumber);
    m_strNextItem.clear();
    m_strPreviousItem.clear();
    m_strNextItem = "1";
    m_strPreviousItem = "1";
    if (1 == m_listNumber) {
        m_strPreviousItem = "0";
    }
    VR_LOG("m_messageInfoVector size : %d", m_messageInfoVector.size());
    if (1 > m_listNumber) {
        std::string strReply = "<action-result agent=\"phone\" op=\"getMessageDetail\" errcode=\"ALREADY_ON_FIRST_MESSAGE\"/>";
        VR_VoiceBoxXmlParser parser(strReply);
        ReplyQueryInfo(parser);
    }
    if (m_messageInfoVector.size() == m_listNumber) {
        m_strNextItem = "0";
    }
    VR_VoiceBoxXmlBuilder xmlBulder;
    std::string strInstanceId;
    std::string strMessageId;
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        if ((*iteratorValueTwo).index == m_listNumber) {
            strInstanceId = (*iteratorValueTwo).instanceId;
            strMessageId = (*iteratorValueTwo).messageId;
        }
    }
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "getMessageDetail");
    m_requestType = "GetPreviousMessageRequest";
    xmlBulder.buildGivenElement(node, "instanceId", strInstanceId, "", "");
    xmlBulder.buildGivenElement(node, "messageId", strMessageId, "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::GetSelectedMessageRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    VR_LOG("GetSelectedMessageRequest: %s", xmlMessage.Get());
    CVECIOutStr strListNumber;

    pcMsg->GetActionParameterValue("List Number", &strListNumber);
    std::string strIndex;
    if (NULL != strListNumber.Get()) {
        strIndex = strListNumber.Get();
    }
    int listNumber;
    listNumber = atoi(strIndex.c_str());
    VR_LOG("listNumber : %d", listNumber);
    m_listNumber = listNumber;
    m_strNextItem.clear();
    m_strPreviousItem.clear();
    m_strNextItem = "1";
    m_strPreviousItem = "1";
    if (1 == listNumber) {
        m_strPreviousItem = "0";
    }
    VR_LOG("m_messageInfoVector size : %d", m_messageInfoVector.size());
    if (m_messageInfoVector.size() == listNumber) {
        m_strNextItem = "0";
    }
    VR_LOG("m_listNumber: %d", m_listNumber);
    VR_VoiceBoxXmlBuilder xmlBulder;
    std::string strInstanceId;
    std::string strMessageId;
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        if ((*iteratorValueTwo).index == listNumber) {
            strInstanceId = (*iteratorValueTwo).instanceId;
            strMessageId = (*iteratorValueTwo).messageId;
        }
    }
    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "getMessageDetail");
    m_requestType = "GetSelectedMessageRequest";
    xmlBulder.buildGivenElement(node, "instanceId", strInstanceId, "", "");
    xmlBulder.buildGivenElement(node, "messageId", strMessageId, "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::IncomingCallInfoRequest(
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
VR_VoiceBoxAgentPhone::IsDrivingRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryVehicleinMotion");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::OutgoingCallInfoRequest(
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
VR_VoiceBoxAgentPhone::RecentCallsRequest(
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
VR_VoiceBoxAgentPhone::RecentMessagesByTypeRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    CVECIOutStr xmlMessage;
    (void)pcMsg->GetString(&xmlMessage);
    CVECIOutStr strMessageType;
    pcMsg->GetActionParameterValue("Message Type", &strMessageType);
    std::string messageType = "";
    if (NULL != strMessageType.Get()) {
        messageType = strMessageType.Get();
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryRecentMessageByType");
    xmlBulder.buildGivenElement(node, "messageType", messageType.c_str(), "", "");
    xmlBulder.buildGivenElement(node, "count", "25", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::RecentMessagesRequest(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }
    VR_VoiceBoxXmlBuilder xmlBulder;

    pugi::xml_node node = xmlBulder.buildStartActionElement("phone", "queryRecentMessage");
    xmlBulder.buildGivenElement(node, "count", "25", "", "");
    std::string strReply = xmlBulder.getXmlString();
    m_engineCallback.OnInfoQueryMessage(strReply);

    return true;
}

bool
VR_VoiceBoxAgentPhone::RecentMessagesReply(
    VR_VoiceBoxXmlParser& parser
)
{
    m_messageInfoVector.clear();
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    CVECIPtr<IVECIListItems> pListItems;
    m_client.CreateListItems(&pListItems);
    CVECIPtr<IVECIParameterSet> spParamsList;
    m_client.CreateParameterSet(&spParamsList);
    if (NULL == spParams.ptr() || NULL == spParamsList.ptr() || NULL == pListItems.ptr()) {
        return false;
    }
    MessageInfo messageInfoID;

    HRESULT retCode = S_OK;
    VoiceVector<RecentMessageInfo>::type recentMessageList = parser.getRecentMessageInfo();
    VoiceVector<RecentMessageInfo>::iterator iteratorValue;
    std::string strRecentMessageAvaiable = parser.getValueByKey("errcode");

    if ("0" == strRecentMessageAvaiable) {
        VR_LOG("strRecentMessageAvaiable : %s", strRecentMessageAvaiable.c_str());
        spParams->AddParameter("Recent Messages Available", "True", "", "");
        int count = 1;
        for (iteratorValue = recentMessageList.begin(); iteratorValue != recentMessageList.end(); ++iteratorValue) {
            messageInfoID.index = count;
            messageInfoID.instanceId = (*iteratorValue).instanceId;
            messageInfoID.messageId = (*iteratorValue).messageId;
            messageInfoID.phoneNumber = (*iteratorValue).phoneNumber;
            if ("1" == (*iteratorValue).messageType) {
                spParamsList->AddParameter("Phone Number", "", "", "");
            }
            else {
                spParamsList->AddParameter("Phone Number", (*iteratorValue).phoneNumber.c_str(), "", "");
            }
            messageInfoID.messageType = (*iteratorValue).messageType;
            std::string strMessageType;
            if ("1" == (*iteratorValue).messageType) {
                strMessageType = m_strEmail;
            }
            else if ("2" == (*iteratorValue).messageType || "3" == (*iteratorValue).messageType) {
                strMessageType = "SMS";
            }
            else if ("4" == (*iteratorValue).messageType) {
                strMessageType = "MMS";
            }
            else {
                strMessageType = (*iteratorValue).messageType;
            }
            spParamsList->AddParameter("Message Type", strMessageType.c_str(), "", "");
            messageInfoID.phoneType = (*iteratorValue).phoneType;
            spParamsList->AddParameter("Phone Type", (*iteratorValue).phoneType.c_str(), "", "");
            messageInfoID.sender = (*iteratorValue).sender;
            spParamsList->AddParameter("Sender", (*iteratorValue).sender.c_str(), "", "");
            if ("" != (*iteratorValue).sender) {
                spParamsList->AddParameter("Entry", (*iteratorValue).sender.c_str(), "", "");
            }
            else if ("" != (*iteratorValue).phoneNumber) {
                spParamsList->AddParameter("Entry", (*iteratorValue).phoneNumber.c_str(), "", "");
            }
            else {
                spParamsList->AddParameter("Entry", "unknown", "", "");
            }
            messageInfoID.subject = (*iteratorValue).subject;
            messageInfoID.timeStamp = (*iteratorValue).timeStamp;
            spParamsList->AddParameter("Time Stamp", (*iteratorValue).timeStamp.c_str(), "", "");
            m_messageInfoVector.push_back(messageInfoID);
            ++count;
            pListItems->AddItem(spParamsList);
            m_client.CreateParameterSet(&spParamsList);
        }
        retCode = m_engineCommand.DriverActionReply("HFD", "Recent Messages Reply", spParams, "Recent Messages", pListItems);
    }
    else if ("ERR_PHONE_NO_MESSAGE_LIST" == strRecentMessageAvaiable) {
        VR_LOG("strRecentMessageAvaiable : %s", strRecentMessageAvaiable.c_str());
        spParams->AddParameter("Recent Messages Available", "False", "", "");

        retCode = m_engineCommand.DriverActionReply("HFD", "Recent Messages Reply", spParams, NULL, NULL);
    }
    else {

    }
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        VR_LOG("index: %d", (*iteratorValueTwo).index);
        VR_LOG("instanceId: %s", (*iteratorValueTwo).instanceId.c_str());
        VR_LOG("messageId: %s", (*iteratorValueTwo).messageId.c_str());
    }
    VR_LOG("m_messageInfoVector size : %d", m_messageInfoVector.size());
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone::GetSelectedMessageReply(
    VR_VoiceBoxXmlParser& parser
)
{
    std::string strErrCode = parser.getValueByKey("errcode");
    if ("0" != strErrCode) {
        GetMessageBodyError(parser);
        return false;
    }
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return false;
    }
    std::string strMessageType;
    std::string strSender;
    std::string strPhoneNumber;
    std::string strPhoneType;
    std::string strTimeStamp;
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        VR_LOG("index: %d", (*iteratorValueTwo).index);
        VR_LOG("m_listNumber: %d", m_listNumber);
        if (m_listNumber == (*iteratorValueTwo).index) {
            strPhoneNumber = (*iteratorValueTwo).phoneNumber;
            strMessageType = (*iteratorValueTwo).messageType;
            strPhoneType = (*iteratorValueTwo).phoneType;
            strSender = (*iteratorValueTwo).sender;
            strTimeStamp = (*iteratorValueTwo).timeStamp;
            break;
        }
    }
    std::string strBody = parser.getValueByKey("messagebody");
    if ("''" == strBody) {
        strBody = "";
    }
    if ("''" == strMessageType) {
        strMessageType = "";
    }
    if ("''" == strSender) {
        strSender = "";
    }
    if ("''" == strPhoneNumber) {
        strPhoneNumber = "";
    }
    if ("''" == strPhoneType) {
        strPhoneType = "";
    }
    if ("''" == strTimeStamp) {
        strTimeStamp = "";
    }

    VR_LOG("strPhoneNumber: %s", strPhoneNumber.c_str());
    std::string strReply;
    if (std::string("GetNextMessageRequest") == m_requestType) {
        strReply = "Get Next Message Reply";
        spParams->AddParameter("Already On Last Message", "False", "", "");
        spParams->AddParameter("Body", strBody.c_str(), "", "");
        // spParams->AddParameter("List Number Valid ", strListNumber.c_str(), "", "");
        spParams->AddParameter("Client TTS", "False", "", "");
        spParams->AddParameter("Message Type", strMessageType.c_str(), "", "");
        spParams->AddParameter("Entry", strSender.c_str(), "", "");
        spParams->AddParameter("Sender", strSender.c_str(), "", "");
        spParams->AddParameter("Phone Number", strPhoneNumber.c_str(), "", "");
        spParams->AddParameter("Phone Type", strPhoneType.c_str(), "", "");
        spParams->AddParameter("Time Stamp", strTimeStamp.c_str(), "", "");
    }
    else if (std::string("GetPreviousMessageRequest") == m_requestType) {
        strReply = "Get Previous Message Reply";
        spParams->AddParameter("Already On First Message", "False", "", "");
        spParams->AddParameter("Body", strBody.c_str(), "", "");
        // spParams->AddParameter("List Number Valid ", strListNumber.c_str(), "", "");
        spParams->AddParameter("Client TTS", "False", "", "");
        spParams->AddParameter("Message Type", strMessageType.c_str(), "", "");
        spParams->AddParameter("Entry", strSender.c_str(), "", "");
        spParams->AddParameter("Sender", strSender.c_str(), "", "");
        spParams->AddParameter("Phone Number", strPhoneNumber.c_str(), "", "");
        spParams->AddParameter("Phone Type", strPhoneType.c_str(), "", "");
        spParams->AddParameter("Time Stamp", strTimeStamp.c_str(), "", "");
    }
    else if (std::string("GetSelectedMessageRequest") ==  m_requestType) {
        strReply = "Get Selected Message Reply";
        spParams->AddParameter("Body", strBody.c_str(), "", "");
        spParams->AddParameter("List Number Valid", "True", "", "");
        spParams->AddParameter("Client TTS", "False", "", "");
    }

    HRESULT retCode  = m_engineCommand.DriverActionReply("HFD", strReply.c_str(), spParams, NULL, NULL);
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

void
VR_VoiceBoxAgentPhone::GetMessageBodyError(
    VR_VoiceBoxXmlParser& parser
)
{
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);

    if (NULL == spParams.ptr()) {
        return;
    }
    std::string strErrCode = parser.getValueByKey("errcode");
    VR_LOG("errcode : %s", strErrCode.c_str());
    std::string strReply;
    if ("ERR_PHONE_QUERY_MESSAGE_DETAIL" == strErrCode) {
        strReply = "Get Selected Message Reply";
        spParams->AddParameter("List Number Valid", "False", "", "");
    }
    else if ("ALREADY_ON_LAST_MESSAGE" == strErrCode) {
        strReply = "Get Next Message Reply";
        spParams->AddParameter("Already On Last Message", "True", "", "");
    }
    else if ("ALREADY_ON_FIRST_MESSAGE" == strErrCode) {
        strReply = "Get Previous Message Reply";
        spParams->AddParameter("Already On First Message", "True", "", "");
    }
    else {
    }
    HRESULT retCode  = m_engineCommand.DriverActionReply("HFD", strReply.c_str(), spParams, NULL, NULL);
    if (S_OK != retCode) {
        VR_LOG("send to VBT error");
    }
}

bool
VR_VoiceBoxAgentPhone::IncomingCallInfoReply(
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
        VR_LOG("strIncommingCallType: %s", strIncommingCallType.c_str());
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
        m_strPhoneType = parser.getValueByKey("phoneType");
        if ("''" == m_strPhoneType) {
            m_strPhoneType = "";
        }
        spParams->AddParameter("Contact Name", strContactName.c_str(), "", "");
        spParams->AddParameter("Phone Number", strNumber.c_str(), "", "");

        m_strContactName = strContactName;
        m_strPhoneNumber = strNumber;
        m_strContactID = strContactID;
    }
    else if ("ERR_PHONE_NO_INCOMMING" == strIncommingCallType) {
         VR_LOG("strIncommingCallType: %s", strIncommingCallType.c_str());
        spParams->AddParameter("Contact Name", "", "", "");
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
VR_VoiceBoxAgentPhone::OutgoingCallInfoReply(
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
        VR_LOG("strOutcommingCallType: %s", strOutcommingCallType.c_str());
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

        m_strPhoneType = parser.getValueByKey("phoneType");
        if ("''" == m_strPhoneType) {
            m_strPhoneType = "";
        }
        spParams->AddParameter("Phone Number", strNumber.c_str(), "", "");
        spParams->AddParameter("Contact Name", strContactName.c_str(), "", "");

        m_strContactName = strContactName;
        m_strPhoneNumber = strNumber;
        m_strContactID = strContactID;
    }
    else if ("ERR_PHONE_NO_OUTGOING" == strOutcommingCallType) {
         VR_LOG("strOutcommingCallType: %s", strOutcommingCallType.c_str());
        spParams->AddParameter("Phone Number", "", "", "");
        spParams->AddParameter("Contact Name", "", "", "");
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
VR_VoiceBoxAgentPhone::RecentCallsReply(
    VR_VoiceBoxXmlParser& parser
)
{
    m_messageInfoVector.clear();
    m_callHistory.clear();
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
        int count = 1;
        CallHistory tempCallHistory;
        spParams->AddParameter("Recent Calls Available", "True", "", "");
        for (iteratorValue = recentCallList.begin(); iteratorValue != recentCallList.end(); ++iteratorValue) {
            tempCallHistory.index = count;
            if ("-1" == (*iteratorValue).phoneKind) {
                spParamsList->AddParameter("Call Type", "", "", "");
            }
            else if ("0" == (*iteratorValue).phoneKind) {
                spParamsList->AddParameter("Call Type", m_strOutGoing.c_str(), "", "");
            }
            else if ("1" == (*iteratorValue).phoneKind) {
                spParamsList->AddParameter("Call Type", m_strIncoming.c_str(), "", "");
            }
            else if ("2" == (*iteratorValue).phoneKind) {
                spParamsList->AddParameter("Call Type", m_strMiss.c_str(), "", "");
            }
            else {
                spParamsList->AddParameter("Call Type", (*iteratorValue).phoneKind.c_str(), "", "");
            }
            tempCallHistory.phoneType = (*iteratorValue).phoneType;
            spParamsList->AddParameter("Phone Type", (*iteratorValue).phoneType.c_str(), "", "");
            tempCallHistory.contactID = (*iteratorValue).contactId;
            tempCallHistory.contactName = (*iteratorValue).contactName;
            if ("" != (*iteratorValue).contactName) {
                spParamsList->AddParameter("Entry", (*iteratorValue).contactName.c_str(), "", "");
            }
            else if ("" != (*iteratorValue).number) {
                spParamsList->AddParameter("Entry", (*iteratorValue).number.c_str(), "", "");
            }
            else {
                spParamsList->AddParameter("Entry", "Blocked", "", "");
            }
            spParamsList->AddParameter("Phone Number", (*iteratorValue).number.c_str(), "", "");
            spParamsList->AddParameter("Time Stamp", (*iteratorValue).timeStamp.c_str(), "", "");
            pListItems->AddItem(spParamsList);
            m_callHistory.push_back(tempCallHistory);
            ++count;
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
VR_VoiceBoxAgentPhone::RecentMessagesByTypeReply(
    VR_VoiceBoxXmlParser& parser
)
{
    m_messageInfoVector.clear();
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    CVECIPtr<IVECIListItems> pListItems;
    m_client.CreateListItems(&pListItems);
    CVECIPtr<IVECIParameterSet> spParamsList;
    m_client.CreateParameterSet(&spParamsList);
    if (NULL == spParams.ptr() || NULL == spParamsList.ptr() || NULL == pListItems.ptr()) {
        return false;
    }
    MessageInfo messageInfoID;

    HRESULT retCode = S_OK;
    VoiceVector<RecentMessageInfo>::type recentMessageList = parser.getRecentMessageInfo();
    VoiceVector<RecentMessageInfo>::iterator iteratorValue;
    std::string strRecentMessageAvaiable = parser.getValueByKey("errcode");

    if ("0" == strRecentMessageAvaiable) {
        VR_LOG("strRecentMessageAvaiable : %s", strRecentMessageAvaiable.c_str());
        spParams->AddParameter("Recent Messages Available", "True", "", "");
        int count = 1;
        for (iteratorValue = recentMessageList.begin(); iteratorValue != recentMessageList.end(); ++iteratorValue) {
            messageInfoID.index = count;
            messageInfoID.instanceId = (*iteratorValue).instanceId;
            messageInfoID.messageId = (*iteratorValue).messageId;
            messageInfoID.phoneNumber = (*iteratorValue).phoneNumber;
            if ("1" == (*iteratorValue).messageType) {
                spParamsList->AddParameter("Phone Number", "", "", "");
            }
            else {
                spParamsList->AddParameter("Phone Number", (*iteratorValue).phoneNumber.c_str(), "", "");
            }
            messageInfoID.messageType = (*iteratorValue).messageType;
            std::string strMessageType;
            if ("1" == (*iteratorValue).messageType) {
                strMessageType = m_strEmail;
            }
            else if ("2" == (*iteratorValue).messageType || "3" == (*iteratorValue).messageType) {
                strMessageType = "SMS";
            }
            else if ("4" == (*iteratorValue).messageType) {
                strMessageType = "MMS";
            }
            else {
                strMessageType = (*iteratorValue).messageType;
            }
            spParamsList->AddParameter("Message Type", strMessageType.c_str(), "", "");
            messageInfoID.phoneType = (*iteratorValue).phoneType;
            spParamsList->AddParameter("Phone Type", (*iteratorValue).phoneType.c_str(), "", "");
            messageInfoID.sender = (*iteratorValue).sender;
            spParamsList->AddParameter("Sender", (*iteratorValue).sender.c_str(), "", "");
            if ("" != (*iteratorValue).sender) {
                spParamsList->AddParameter("Entry", (*iteratorValue).sender.c_str(), "", "");
            }
            else if ("" != (*iteratorValue).phoneNumber) {
                spParamsList->AddParameter("Entry", (*iteratorValue).phoneNumber.c_str(), "", "");
            }
            else {
                spParamsList->AddParameter("Entry", "unknown", "", "");
            }
            messageInfoID.subject = (*iteratorValue).subject;
            messageInfoID.timeStamp = (*iteratorValue).timeStamp;
            spParamsList->AddParameter("Time Stamp", (*iteratorValue).timeStamp.c_str(), "", "");
            m_messageInfoVector.push_back(messageInfoID);
            ++count;
            pListItems->AddItem(spParamsList);
            m_client.CreateParameterSet(&spParamsList);
        }
        retCode = m_engineCommand.DriverActionReply("HFD", "Recent Messages By Type Reply", spParams, "Recent Messages", pListItems);
    }
    else if ("ERR_PHONE_NO_MESSAGE_LIST" == strRecentMessageAvaiable) {
        VR_LOG("strRecentMessageAvaiable : %s", strRecentMessageAvaiable.c_str());
        spParams->AddParameter("Recent Messages Available", "False", "", "");

        retCode = m_engineCommand.DriverActionReply("HFD", "Recent Messages By Type Reply", spParams, NULL, NULL);
    }
    else {

    }
    VoiceVector<MessageInfo>::iterator iteratorValueTwo;
    for (iteratorValueTwo = m_messageInfoVector.begin(); iteratorValueTwo != m_messageInfoVector.end(); ++iteratorValueTwo) {
        VR_LOG("index: %d", (*iteratorValueTwo).index);
        VR_LOG("instanceId: %s", (*iteratorValueTwo).instanceId.c_str());
        VR_LOG("messageId: %s", (*iteratorValueTwo).messageId.c_str());
    }
    VR_LOG("m_messageInfoVector size : %d", m_messageInfoVector.size());
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone::IsDrivingReply(
    VR_VoiceBoxXmlParser& parser
)
{
    VR_LOG("IsDrivingReply");
    CVECIPtr<IVECIParameterSet> spParams;
    m_client.CreateParameterSet(&spParams);
    if (NULL == spParams.ptr()) {
        return false;
    }

    HRESULT retCode = S_OK;
    std::string strMotion = parser.getValueByKey("motion");

    if ("false" == strMotion) {
        spParams->AddParameter("Driving", "False", "", "");
        retCode = m_engineCommand.DriverActionReply("HFD", "Is Driving Reply", spParams, NULL, NULL);
    }
    else if ("true" == strMotion) {
        spParams->AddParameter("Driving", "True", "", "");
        retCode  = m_engineCommand.DriverActionReply("HFD", "Is Driving Reply", spParams, NULL, NULL);
    }
    else {

    }
    if (S_OK != retCode) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone::PhoneBack(
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
VR_VoiceBoxAgentPhone::PhoneCancel(
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
VR_VoiceBoxAgentPhone::PhoneHelp(
    IVECIParsedMessage *pcMsg
)
{
    if (NULL == pcMsg) {
        return false;
    }

    return true;
}

bool
VR_VoiceBoxAgentPhone::PhoneStartOver(
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
VR_VoiceBoxAgentPhone::PhoneStartPause(
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
VR_VoiceBoxAgentPhone::PhoneStartRepeat(
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
VR_VoiceBoxAgentPhone::EscalatingError(IVECIParsedMessage *pcMsg)
{
    if (NULL == pcMsg) {
        return false;
    }

    m_engineCallback.OnRequest("<event name=\"escalating-error\"></event>");
    return true;
}

/* EOF */
