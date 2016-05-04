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
 * @file VR_VoiceBoxXmlParser.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXXMLPARSER_H
#define VR_VOICEBOXXMLPARSER_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* Standard Header */
#include <string>
#include <iostream>

/* Suntec Header */
#ifndef MEM_VECTOR_H
#    include "MEM_vector.h"
#endif

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif
#ifndef HEADER_PUGIXML_HPP
#    include "pugixml.hpp"
#endif

// save the information of node
struct StructNode
{
    std::string Name;                                      // the name of node
    std::string Value;                                     // the value of node
};

// save the information of node
struct Parameter
{
    std::string name;                                      // the name of node
    std::string nameValue;                                 // the value of node
    std::string parameterValue;                            // the name of attribute
    std::string value;                                     // the value of attribute
    
};

// save the information of node
struct PersonInfo
{
    std::string strId;                                     // the id
    std::string firstName;                                 // the first-name
    std::string lastName;                                  // the last-name
    VoiceVector<StructNode>::type phoneItemVector;               // the value of attribute
};

// save the information of node
struct FormalTwoItem
{
    std::string strId;                                     // the id 
    std::string name;                                      // the first-name                                 // the last-name
};

struct FormalThreeItem
{
    std::string strId;                                     // the id 
    std::string name;                                      // the first-name
    std::string threeItem;                                // the last-name
};

// save the information of node
struct FormalPronInfo
{
    FormalTwoItem formalItem;                                     // the id 
    VoiceMap<std::string, std::string>::type aliasMap;                  // the value of attribute
};

// save the information of node
struct FormalInfo
{
    FormalTwoItem formalItem;                                      // the first-name                                 // the last-name
    VoiceVector<std::string>::type aliasVector;                  // the value of attribute
};

struct FormalOtherInfo
{
    FormalThreeItem formalItem;
    VoiceVector<std::string>::type aliasVector;                  // the value of attribute
};

struct AudioSourceInfo
{
    FormalThreeItem formalItem;
    VoiceMap<std::string, std::string>::type aliasMap;                  // the value of attribute
};

struct FormalShortCutPronInfo
{
    std::string strId;                                     // the id 
    std::string name;                                      // the first-name                                 // the last-name
    std::string shortcut;
    VoiceMap<std::string, std::string>::type aliasMap;                  // the value of attribute
};

// save the information of delete node
struct MusicDiffDelete
{
    std::string id;                                     // the id
    std::string name;                                   // the name
};

// save the information of add node
struct MusicDiffAdd
{
    std::string id;                                     // the id
    std::string name;                                   // the name
    std::string pron;
    std::string pronid;
};

// alias
struct Alias
{
    std::string strName;                                   // the name
    std::string strPron;                                   // the pron
};

// formal
struct AppFormal
{
    std::string strId;                                      // the id
    std::string strName;                                    // the name
    std::string strShortCut;                                // the short cut
    VoiceVector<Alias>::type vecAlias;                      // the vector of alias
};

struct AppCategory
{
    std::string strName;                                    // the name
    VoiceVector<AppFormal>::type vecFormal;                 // the vector of formal
};

// save the information of person node
struct PersonDetailInfo
{
    std::string strId;                                     // the id
    std::string firstName;                                 // the first-name
    std::string lastName;                                  // the last-name
    std::string phonetype;                                 // the phonetypeid
    std::string number;                                    // the number
};

// personInfo
struct PersonInfoStruct
{
    bool isExit;                                            // whether the cotent of category
    VoiceVector<PersonDetailInfo>::type personInfo;               // the vector of personInfo
};

struct RecentCallInfo
{
    std::string contactId;                                 // the contactId;
    std::string phoneKind;                                 // the phoneKind;
    std::string contactName;                               // the contactName;
    std::string phoneType;                                 // the phoneType;
    std::string number;                                    // the contactId;
    std::string timeStamp;                                 // the timeStamp;
    std::string listType;                                  // the listType;
    std::string phoneTypeName;                             // the phoneTypeName;
};

struct RecentMessageInfo
{
    std::string instanceId;                                 // the instanceId;
    std::string messageId;                                  // the messageId;
    std::string messageType;                                // the messageType;
    std::string phoneNumber;                                // the phoneNumber;
    std::string phoneType;                                   // the phoneType;
    std::string sender;                                      // the sender;
    std::string subject;                                     // the subject;
    std::string timeStamp;                                   // the timeStamp;
    std::string status;                                      // the status;
};

/**
 * @brief The VR_VoiceBoxXmlParser class
 *
 * class declaration
 */
class VR_VoiceBoxXmlParser
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxXmlParser(const std::string& xml);

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxXmlParser();
    // get rootNode
    pugi::xml_node getRootNode();
    pugi::xml_node getRootNode(pugi::xml_document& xml_doc);
        // parse the xml
    void xmlParser();
    // parse the child-node of xml
    void childNodeInfo(pugi::xml_node node);
    // get the value of sendEvent
    std::string getSendEvent();
    // get the value of actionType
    std::string getActionType();
    // get the value of updateType
    std::string getUpdateType();
    // get the value of agent
    std::string getAgentName();
    // get the value of actionState
    std::string getActionState();
    void getMessageInfo(VoiceVector<FormalTwoItem>::type& messageInfo);
    void getPhoneType(VoiceVector<FormalInfo>::type& phoneType);
    void getMessageType(VoiceVector<FormalInfo>::type& messageType);
    void getAudioSource(VoiceVector<AudioSourceInfo>::type& audioSource);
    void getFmGenre(VoiceVector<FormalPronInfo>::type& fmGenre);
    void getSatChannelName(VoiceVector<FormalPronInfo>::type& satChannelName);
    void getSatChannelNumber(VoiceVector<FormalTwoItem>::type& satChannelNumber);
    void getSatGenre(VoiceVector<FormalPronInfo>::type& satGenre);
    void getHdSubChannel(VoiceVector<FormalOtherInfo>::type& hdSubChannel);
    void getApps(VoiceVector<FormalShortCutPronInfo>::type& apps);
    void getMusicDiffDelete(VoiceMap<std::string, VoiceVector<MusicDiffDelete>::type>::type& musicDiffDelete);
    void getMusicDiffAdd(VoiceMap<std::string, VoiceVector<MusicDiffAdd>::type>::type& musicDiffAdd);

    std::string getLastFlg();
    VoiceVector<RecentCallInfo>::type getRecentCallInfo();
    VoiceVector<RecentMessageInfo>::type getRecentMessageInfo();

    /*
     * get the value by key
     *
     * we can get the value by giving a key
     *
     * @param key : key node
     * @return key value
     */

    std::string getValueByKey(const std::string& key);

    /*
     * get the value by key
     *
     * the value is that we get belongs to the node of item
     *
     * @param key : key node
     * @return the container that contain the value of item
     */

    VoiceVector<std::string>::type getListItemValue(const std::string& key);

    /*
     * get the information of item's node
     *
     * @param nodeString : the group of string
     * @return the container that contain the information of node
     */
    VoiceVector<StructNode>::type getListStructValue(const std::string* nodeString);

    // get the action class attribute value
    std::string getActionClass();

    // get the message class attribute value
    std::string getMsgClass();

    std::string getMsgSource();

    // get the information of parameter node
    VoiceVector<Parameter>::type getMsgParameterValue();

    /*
     * get the value of parameter's node
     *
     * @param parameterStructValue : the infomaton of
     * @return the container that contain the information of node
     */
    std::string getMsgParameterValue(VoiceVector<Parameter>::type parameterStructValue, const std::string &strName);

    // get the keyValue of key
    std::string getXmlKey();

    std::string getXmlString();

    // set the vector of parseXml
    void setVectorInfo(VoiceVector<StructNode>::type m_vector)
    {
        this->m_vector = m_vector;
    }
    
    // get the vector of parseXml
    VoiceVector<StructNode>::type getVectorInfo()
    {
        return m_vector;
    }

    // set the size of list
    void setListSize(size_t listSize)
    {
        this->listSize = listSize;
    }

    // get the size of list
    size_t getListSize()
    {
        return listSize;
    }

    void getApps(VoiceVector<AppCategory>::type& vecAppCategory);

private:
    void grammarNewHandle(const std::string& strCategory);

    // parse the person of category
    void setFormalInfo(const pugi::xml_node& rootNode, VoiceVector<FormalInfo>::type& formalVector);
    // parse the message of category

    void setPhoneTypeInfo(const pugi::xml_node& rootNode, VoiceVector<FormalInfo>::type& formalVector);

    void setFormalAliasPronInfo(const pugi::xml_node& rootNode, VoiceVector<FormalPronInfo>::type& formalPronVec);

    void setFormalNoAliasInfo(const pugi::xml_node& rootNode, VoiceVector<FormalTwoItem>::type& vecFormalTwoItem);

    void setFormalOtherInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalOtherInfo>::type& formalOtherVector);

    void setAudioSource(const pugi::xml_node& rootNode,
    VoiceVector<AudioSourceInfo>::type& audioSourceVec);

    void setFormalShortCutPronInfo(const std::string& xml,
    VoiceVector<FormalShortCutPronInfo>::type& formalShortCutPronInfoVec);

    void setMusicDiff(const std::string& strCategory, const std::string& strDiffType);

    void setMusicDel(const std::string& categoryNameAttr,
    const pugi::xml_node& rootChildNode);

    void setMusicAdd(const std::string& categoryNameAttr,
    const pugi::xml_node& rootChildNode);

    void setFormal(
    const pugi::xml_node& rootChildNode, FormalTwoItem& formalItem);

    void setFormal(const pugi::xml_node& rootChildNode,
    const std::string& strOtherItemName, FormalThreeItem& formalItem);

    void setAlias(pugi::xml_node& itemNode,
    const std::string& strItem, VoiceMap<std::string, std::string>::type& aliasVector);

    void setAlias(pugi::xml_node& itemNode, VoiceVector<std::string>::type& aliasVector);

    pugi::xml_document m_xml_document;          // get the information of the xml
    VoiceVector<StructNode>::type m_vector;     // save the information of the node
    StructNode structNode;                      // node struct
    size_t listSize;                            // the size of list

    std::string getRootName();                  // get the name of rootNode
    std::string getRootChildName();             // get the name of childRootNode

    bool getGivedNode(pugi::xml_node& node, const std::string& strGivedName);
    void setAppCategory(const pugi::xml_node& node, AppCategory& appCategory);
    void setAppFormal(const pugi::xml_node& node, AppFormal& appFormal);
    void setAlias(const pugi::xml_node& node, Alias& alias);
    void setPhoneTypeAlias(pugi::xml_node& rootChildNode, VoiceVector<std::string>::type& aliasVector);
    void setENPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector);
    void setESPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector);
    void setFRPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector);

    VoiceVector<FormalTwoItem>::type m_messageInfo; // messageInfo grammar
    VoiceVector<FormalInfo>::type m_phoneType;   // phoneType grammar
    VoiceVector<FormalInfo>::type m_messageType; // messageType grammar

    VoiceVector<AudioSourceInfo>::type m_audioSource;
    VoiceVector<FormalPronInfo>::type m_fmGenre;
    VoiceVector<FormalPronInfo>::type m_satChannelName;
    VoiceVector<FormalTwoItem>::type m_satChannelNumber;
    VoiceVector<FormalPronInfo>::type m_satGenre;
    VoiceVector<FormalOtherInfo>::type m_hdSubChannel;

    VoiceMap<std::string, VoiceVector<MusicDiffDelete>::type>::type m_musicDiffDelete;
    VoiceMap<std::string, VoiceVector<MusicDiffAdd>::type>::type m_musicDiffAdd;
    std::string m_strWholeXml;
    VoiceVector<PersonDetailInfo>::type m_personDetailInfo;

    std::string m_strLastFlg;
};

#endif // VR_VOICEBOXXMLPARSER_H
/* EOF */