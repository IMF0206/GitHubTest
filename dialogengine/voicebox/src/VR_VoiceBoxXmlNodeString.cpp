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
#ifndef VR_VOICEBOXXMLNODESTRING_H
#    include "VR_VoiceBoxXmlNodeString.h"
#endif

// the parameter of node
const std::string phoneInfo[] = { "firstname", "lastname", "fullname", "phonetype", "number", "" };
const std::string itemDisplay[] = { "id", "name", "value", "formal", "alias", "number", "pronunciation", "route", "shortcut", "" };
const std::string cityItem[] = { "id", "city", "cityalias", "citystate", "state", "" };
const std::string messageList[] = { "listindex", "ttsindex", "sender", "calltype", "phonetype", "entry", "ttsentry",
    "phonenumber", "messagetype", "timestamp", "" };
const std::string musicInfo[] = { "name", "sourceId", "id", "" };
const std::string radioInfo[] = { "formal_name", "alias", "id", "" };
const std::string messageInfo[] = { "instanceId", "messageId", "messageType", "phoneNumber", "phoneType", "sender", "subject", "timeStamp", "status", "" };
const std::string actionListMessages[] = { "listindex", "entry", "phonenamevalue", "sender", "timestamp", "" };
const std::string quickReplytoCurrentMessage[] = { "listindex", "messagebody", "" };
const std::string phoneTypeInfo[] = { "phonetype", "number", "name", "" };
const std::string callPhoneInfo[] = { "contactId", "phoneKind", "contactName", "phoneType", "number", "timeStamp", "" };
const std::string nameMatchInfo[] = { "contactName", "contactId", "" };
const std::string confirmCallInfo[] = { "contactName", "phoneType", "phoneNumber", "" };
const std::string listRecentCalls[] = { "listindex", "calltype", "entry", "phonenamevalue", "phonenumber", "phonetype", "timestamp", "" };
const std::string nBestName[] = { "name", "" };
const std::string nBestNumber[] = { "number", "" };
const std::string nBestDigits[] = { "digits", "" };
const std::string addressInfo[] = { "stateId", "country", "state", "statePron", "cityId", "city", "cityPron", "streetExist", "streetId", "street", "streetPron", "houseNumberExist", "houseNumber", "" };

const std::string hint[] = { "hint", "" };

// Constructor
VR_VoiceBoxXmlNodeString::VR_VoiceBoxXmlNodeString()
{
}

// Destructor
VR_VoiceBoxXmlNodeString::~VR_VoiceBoxXmlNodeString()
{
}
/* EOF */
