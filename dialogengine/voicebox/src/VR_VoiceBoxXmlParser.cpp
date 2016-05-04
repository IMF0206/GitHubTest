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
#include <cstring>
/* Suntec Header */
#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXPHONEDATA_H
#    include "VR_VoiceBoxPhoneData.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#include "VR_Log.h"

// Constructor
VR_VoiceBoxXmlParser::VR_VoiceBoxXmlParser(const std::string& xml)
{
    m_musicDiffDelete.clear();
    m_musicDiffAdd.clear();

    if (NULL == xml.c_str()) {
        return;
    }
    else {
        m_strWholeXml = xml;
        if (!m_xml_document.load(xml.c_str())) {
            std::cout << "load fail " << std::endl;
            return;
        }
        else if ("category" == getRootName()) {
            grammarNewHandle(xml);
        }
        else {
            xmlParser();
        }
    }
}

// Destructor
VR_VoiceBoxXmlParser::~VR_VoiceBoxXmlParser()
{
}

/**
 * parse the xml of back
 */

void VR_VoiceBoxXmlParser::xmlParser()
{
    pugi::xml_node rootNode = m_xml_document.first_child();
    while (rootNode) {
        if (strlen(rootNode.child_value())) {
            structNode.Name = rootNode.name();
            structNode.Value = rootNode.child_value();
            m_vector.push_back(structNode);
        }
        for (pugi::xml_attribute attr = rootNode.first_attribute(); attr; attr = attr.next_attribute()) {
            structNode.Name = attr.name();
            structNode.Value = attr.value();
            m_vector.push_back(structNode);
        }
        childNodeInfo(rootNode);
        rootNode = rootNode.next_sibling();
    }
    setVectorInfo(m_vector);
}

/**
 * get the information from childNode
 *
 * @param node : the node of xml
 */

void VR_VoiceBoxXmlParser::childNodeInfo(pugi::xml_node node)
{
    if (node.first_child()) {
        pugi::xml_node childNode = node.first_child();
        while (childNode) {
            if (strlen(childNode.child_value())) {
                structNode.Name = childNode.name();
                structNode.Value = childNode.child_value();
                m_vector.push_back(structNode);
            }
            for (pugi::xml_attribute attr = childNode.first_attribute(); attr; attr = attr.next_attribute()) {
                structNode.Name = attr.name();
                structNode.Value = attr.value();
                m_vector.push_back(structNode);
            }
            childNodeInfo(childNode);
        childNode = childNode.next_sibling();
        }
    }
}

/**
 * get the name of root node
 *
 * @return the name of root
 */

std::string VR_VoiceBoxXmlParser::getRootName()
{
    pugi::xml_node rootNode = m_xml_document.first_child();
    std::string rootName = rootNode.name();
    return rootName;
}

/**
 * get the name of root node
 *
 * @return the name of root
 */

pugi::xml_node VR_VoiceBoxXmlParser::getRootNode()
{
    pugi::xml_node rootNode = m_xml_document.first_child();
    return rootNode;
}

pugi::xml_node VR_VoiceBoxXmlParser::getRootNode(pugi::xml_document& xml_doc)
{
    pugi::xml_node rootNode = xml_doc.first_child();
    return rootNode;
}

/**
 * get the name of root node
 *
 * @return the name of root-child
 */

std::string VR_VoiceBoxXmlParser::getRootChildName()
{
    pugi::xml_node rootNode = m_xml_document.first_child();
    pugi::xml_node childNode = rootNode.first_child();
    std::string rootChildName = childNode.name();
    return rootChildName;
}

/**
 * get the key of xml
 *
 * @return the key of xml
 */

std::string VR_VoiceBoxXmlParser::getXmlKey()
{
    std::string keyValue = "";
    if (0 == strcmp("action", getRootName().c_str())
        || 0 == strcmp("action-result", getRootName().c_str())) {
        keyValue = getValueByKey("op");
    }
    else if (0 == strcmp("display", getRootName().c_str())) {
        keyValue = getValueByKey("content");
    }
    else if (0 == strcmp("send", getRootName().c_str())) {
        keyValue = getValueByKey("event");
    }
    else if (0 == strcmp("event", getRootName().c_str())) {
        keyValue = getValueByKey("name");
    }
    else if (0 == strcmp("grammar_init", getRootName().c_str())) {
        keyValue = "grammar_init";
    }
    else if (0 == strcmp("grammar_diff", getRootName().c_str())) {
        keyValue = "grammar_diff";
    }
    else if (0 == strcmp("grammar_active", getRootName().c_str())) {
        keyValue = "grammar_active";
    }
    else if (0 == strcmp("grammar_disactive", getRootName().c_str())) {
        keyValue = "grammar_disactive";
    }
    else if (0 == strcmp("grammar_delete", getRootName().c_str())) {
        keyValue = "grammar_delete";
    }
    else if ("grammar_pause" == getRootName()) {
        keyValue = "grammar_pause";
    }
    else if ("grammar_resume" == getRootName()) {
        keyValue = "grammar_resume";
    }
    else if ("grammar_preupdate" == getRootName()) {
        keyValue = "grammar_preupdate";
    }
    else if ("grammar_audiosource_oc" == getRootName()) {
        keyValue = "grammar_audiosource_oc";
    }
    else if ("category" == getRootName()) {
        keyValue = "category";
    }
    else if ("grammar_voicetag" == getRootName()) {
        keyValue = "grammar_voicetag";
    }
    else if (0 == strcmp("command", getRootName().c_str())) {
        if (0 == strcmp("start", getRootChildName().c_str())) {
            keyValue = getRootChildName();
        }
        else if (0 == strcmp("stop", getRootChildName().c_str())) {
            keyValue = getRootChildName();
        }
        else if (0 == strcmp("update", getRootChildName().c_str())) {
            keyValue = getValueByKey("type");
        }
        else {
        }
    }
    else if ("op" == getRootName()) {
        keyValue = getValueByKey("name");
    }
    return keyValue;
}

void
VR_VoiceBoxXmlParser::grammarNewHandle(const std::string& strCategory)
{
    pugi::xml_document xml;

    if (strCategory.empty()) {
        return;
    }
    else {
        if (!xml.load(strCategory.c_str())) {
            std::cout << "load fail " << std::endl;
            return;
        }
    }

    pugi::xml_node rootNode = getRootNode(xml);
    std::string strName = rootNode.attribute("name").as_string();
    m_strLastFlg = rootNode.attribute("isLast").as_string();

    if ("quickreplymessage" == strName) {
        setFormalNoAliasInfo(rootNode, m_messageInfo);
    }
    else if ("phonetype" == strName) {
        setPhoneTypeInfo(rootNode, m_phoneType);
    }
    else if ("messagetype" == strName) {
        setFormalInfo(rootNode, m_messageType);
    }
    else if ("audiosource" == strName) {
        setAudioSource(rootNode, m_audioSource);
    }
    else if ("fmgenre" == strName) {
        setFormalAliasPronInfo(rootNode, m_fmGenre);
    }
    else if ("satchannelname" == strName) {
        setFormalAliasPronInfo(rootNode, m_satChannelName);
    }
    else if ("satchannelnumber" == strName) {
        setFormalNoAliasInfo(rootNode, m_satChannelNumber);
    }
    else if ("satgenre" == strName) {
        setFormalAliasPronInfo(rootNode, m_satGenre);
    }
    else if ("hdsubchannel" == strName) {
        setFormalOtherInfo(rootNode, m_hdSubChannel);
    }
    else {
        if (std::string::npos != strCategory.find("<add>")) {
            setMusicDiff(strCategory, "add");
        }
        else if (std::string::npos != strCategory.find("<delete>")) {
            setMusicDiff(strCategory, "delete");
        }
        else {

        }
    }
}

/**
 * get the infomation of person
 *
 * @return the vector of message
 */

void VR_VoiceBoxXmlParser::setFormalNoAliasInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalTwoItem>::type& vecFormalItem)
{
    vecFormalItem.clear();

    if (rootNode.first_child()) {
        FormalTwoItem formalItem;
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            setFormal(rootChildNode, formalItem);

            vecFormalItem.push_back(formalItem);
            rootChildNode = rootChildNode.next_sibling();
        }
    }
    return;
}

/**
 * get the infomation of formal
 *
 * @return the vector of formal
 */

void VR_VoiceBoxXmlParser::setFormalInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalInfo>::type& formalVector)
{
    formalVector.clear();

    if (rootNode.first_child()) {

        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            FormalInfo formalInfo;
            setFormal(rootChildNode, formalInfo.formalItem);

            pugi::xml_node itemNode;
            if (rootChildNode.first_child()) {
                itemNode = rootChildNode.first_child();
            }

            setAlias(itemNode, formalInfo.aliasVector);

            formalVector.push_back(formalInfo);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void VR_VoiceBoxXmlParser::setPhoneTypeInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalInfo>::type& formalVector)
{
    formalVector.clear();
    if (rootNode.first_child()) {

        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            FormalInfo formalInfo;
            setFormal(rootChildNode, formalInfo.formalItem);
            setPhoneTypeAlias(rootChildNode, formalInfo.aliasVector);
            formalVector.push_back(formalInfo);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void VR_VoiceBoxXmlParser::setFormalOtherInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalOtherInfo>::type& formalOtherVector)
{
    formalOtherVector.clear();

    FormalOtherInfo formalOhterInfo;
    if (rootNode.first_child()) {
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            setFormal(rootChildNode, "intval", formalOhterInfo.formalItem);
            pugi::xml_node itemNode;
            if (rootChildNode.first_child()) {
                itemNode = rootChildNode.first_child();
            }

            setAlias(itemNode, formalOhterInfo.aliasVector);

            formalOtherVector.push_back(formalOhterInfo);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void
VR_VoiceBoxXmlParser::setFormal(
    const pugi::xml_node& rootChildNode,
    FormalTwoItem& formalItem
)
{
    formalItem.strId.clear();
    formalItem.name.clear();
    for (pugi::xml_attribute attr = rootChildNode.first_attribute(); attr; attr = attr.next_attribute()) {
        if (0 == strcmp("id", attr.name())) {
            formalItem.strId = attr.value();
            continue;
        }
        else if (0 == strcmp("name", attr.name())) {
            formalItem.name = attr.value();
            continue;
        }
    }
}

void
VR_VoiceBoxXmlParser::setFormal(const pugi::xml_node& rootChildNode,
    const std::string& strOtherItemName, FormalThreeItem& formalItem
)
{
    formalItem.strId.clear();
    formalItem.name.clear();
    formalItem.threeItem.clear();

    for (pugi::xml_attribute attr = rootChildNode.first_attribute(); attr; attr = attr.next_attribute()) {
        if (0 == strcmp("id", attr.name())) {
            formalItem.strId = attr.value();
            continue;
        }
        else if (0 == strcmp("name", attr.name())) {
            formalItem.name = attr.value();
            continue;
        }
        else if (0 == strcmp(strOtherItemName.c_str(), attr.name())) {
            formalItem.threeItem = attr.value();
            continue;
        }
    }
}

void
VR_VoiceBoxXmlParser::setAlias(pugi::xml_node& itemNode,
    const std::string& strItem,
    VoiceMap<std::string, std::string>::type& aliasMap)
{
    aliasMap.clear();

    std::string strName;
    std::string strPron;

    while (itemNode) {
        strName.clear();
        strPron.clear();
        for (pugi::xml_attribute attr = itemNode.first_attribute(); attr; attr = attr.next_attribute()) {
            if (0 == strcmp("name", attr.name())) {
                strName = attr.value();
                continue;
            }

            if (0 == strcmp(strItem.c_str(), attr.name())) {
                strPron = attr.value();
                continue;
            }
        }
        aliasMap.insert(std::make_pair(strName, strPron));

        itemNode = itemNode.next_sibling();
    }
}

void
VR_VoiceBoxXmlParser::setAlias(pugi::xml_node& itemNode,
    VoiceVector<std::string>::type& aliasVector)
{
    aliasVector.clear();
    while (itemNode) {
        for (pugi::xml_attribute attr = itemNode.first_attribute(); attr; attr = attr.next_attribute()) {
            if (0 == strcmp("name", attr.name())) {
                aliasVector.push_back(attr.value());
                break;
            }
        }

        itemNode = itemNode.next_sibling();
    }

}

void
VR_VoiceBoxXmlParser::setPhoneTypeAlias(pugi::xml_node& rootChildNode,
    VoiceVector<std::string>::type& aliasVector)
{
    aliasVector.clear();
    std::string strLang;
    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    std::string phoneType = rootChildNode.attribute("name").as_string();
    if (NULL == pcConfig) {
        setENPhoneTypeAlis(phoneType, aliasVector);
    }
    else {
        strLang = pcConfig->getVRLanguage();
        if (("en-us" == strLang) || ("en-au" == strLang)) {
            setENPhoneTypeAlis(phoneType, aliasVector);
        }
        else if ("es-mx" == strLang) {
            setESPhoneTypeAlis(phoneType, aliasVector);
        }
        else if ("fr-ca" == strLang) {
            setFRPhoneTypeAlis(phoneType, aliasVector);
        }
        else {
        }
    }
}

void VR_VoiceBoxXmlParser::setENPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector)
{
    VR_LOG("phoneType : %s", phoneType.c_str());
    if ("Mobile" == phoneType) {
        aliasVector.push_back("Mobile");
        aliasVector.push_back("Mobile Phone");
        aliasVector.push_back("Cell");
        aliasVector.push_back("Cell Phone");
        aliasVector.push_back("Mobile 1");
    }
    else if ("Mobile 2" == phoneType) {
        aliasVector.push_back("Mobile 2");
        aliasVector.push_back("Mobile Phone 2");
        aliasVector.push_back("Cell 2");
        aliasVector.push_back("Cell Phone 2");
    }
    else if ("Mobile 3" == phoneType) {
        aliasVector.push_back("Mobile 3");
        aliasVector.push_back("Mobile Phone 3");
        aliasVector.push_back("Cell 3");
        aliasVector.push_back("Cell Phone 3");
    }
    else if ("Mobile 4" == phoneType) {
        aliasVector.push_back("Mobile 4");
        aliasVector.push_back("Mobile Phone 4");
        aliasVector.push_back("Cell 4");
        aliasVector.push_back("Cell Phone 4");
    }
    else if ("Home" == phoneType) {
        aliasVector.push_back("Home");
        aliasVector.push_back("Home Phone");
        aliasVector.push_back("House");
        aliasVector.push_back("Home 1");
    }
    else if ("Home 2" == phoneType) {
        aliasVector.push_back("Home 2");
        aliasVector.push_back("Home Phone 2");
        aliasVector.push_back("House 2");
    }
    else if ("Home 3" == phoneType) {
        aliasVector.push_back("Home 3");
        aliasVector.push_back("Home Phone 3");
        aliasVector.push_back("House 3");
    }
    else if ("Home 4" == phoneType) {
        aliasVector.push_back("Home 4");
        aliasVector.push_back("Home Phone 4");
        aliasVector.push_back("House 4");
    }
    else if ("Work" == phoneType) {
        aliasVector.push_back("Work");
        aliasVector.push_back("Office");
        aliasVector.push_back("Business");
        aliasVector.push_back("Work 1");
    }
    else if ("Work 2" == phoneType) {
        aliasVector.push_back("Work 2");
        aliasVector.push_back("Office 2");
        aliasVector.push_back("Business 2");
    }
    else if ("Work 3" == phoneType) {
        aliasVector.push_back("Work 3");
        aliasVector.push_back("Office 3");
        aliasVector.push_back("Business 3");
    }
    else if ("Work 4" == phoneType) {
        aliasVector.push_back("Work 4");
        aliasVector.push_back("Office 4");
        aliasVector.push_back("Business 4");
    }
    else if ("Other" == phoneType) {
        aliasVector.push_back("Other");
        aliasVector.push_back("Other 1");
    }
    else if ("Other 2" == phoneType) {
        aliasVector.push_back("Other 2");
    }
    else if ("Other 3" == phoneType) {
        aliasVector.push_back("Other 3");
    }
    else if ("Other 4" == phoneType) {
        aliasVector.push_back("Other 4");
    }
    else {
    }
}

void VR_VoiceBoxXmlParser::setESPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector)
{
    VR_LOG("phoneType : %s", phoneType.c_str());
    if ("Mobile" == phoneType) {
        aliasVector.push_back("Móvil");
        aliasVector.push_back("Teléfono celular");
        aliasVector.push_back("Teléfono móvil");
        aliasVector.push_back("Celular");
        aliasVector.push_back("Cel");
        aliasVector.push_back("Móvil 1");
    }
    else if ("Mobile 2" == phoneType) {
        aliasVector.push_back("Móvil 2");
        aliasVector.push_back("Teléfono cellular 2");
        aliasVector.push_back("Teléfono móvil 2");
        aliasVector.push_back("Celular 2");
        aliasVector.push_back("Cel 2");
    }
    else if ("Mobile 3" == phoneType) {
        aliasVector.push_back("Móvil 3");
        aliasVector.push_back("Teléfono cellular 3");
        aliasVector.push_back("Teléfono móvil 3");
        aliasVector.push_back("Celular 3");
        aliasVector.push_back("Cel 3");
    }
    else if ("Mobile 4" == phoneType) {
        aliasVector.push_back("Móvil 4");
        aliasVector.push_back("Teléfono cellular 4");
        aliasVector.push_back("Teléfono móvil 4");
        aliasVector.push_back("Celular 4");
        aliasVector.push_back("Cel 4");
    }
    else if ("Home" == phoneType) {
        aliasVector.push_back("Casa");
        aliasVector.push_back("Vivienda");
        aliasVector.push_back("Hogar");
        aliasVector.push_back("En casa");
        aliasVector.push_back("Casa 1");
    }
    else if ("Home 2" == phoneType) {
        aliasVector.push_back("Casa 2");
        aliasVector.push_back("Vivienda 2");
        aliasVector.push_back("Hogar 2");
        aliasVector.push_back("En casa 2");
    }
    else if ("Home 3" == phoneType) {
        aliasVector.push_back("Casa 3");
        aliasVector.push_back("Vivienda 3");
        aliasVector.push_back("Hogar 3");
        aliasVector.push_back("En casa 3");
    }
    else if ("Home 4" == phoneType) {
        aliasVector.push_back("Casa 4");
        aliasVector.push_back("Vivienda 4");
        aliasVector.push_back("Hogar 4");
        aliasVector.push_back("En casa 4");
    }
    else if ("Work" == phoneType) {
        aliasVector.push_back("Trabajo");
        aliasVector.push_back("Oficina");
        aliasVector.push_back("Negocio");
        aliasVector.push_back("Trabajo 1");
    }
    else if ("Work 2" == phoneType) {
        aliasVector.push_back("Trabajo 2");
        aliasVector.push_back("Oficina 2");
        aliasVector.push_back("Negocio 2");
    }
    else if ("Work 3" == phoneType) {
        aliasVector.push_back("Trabajo 3");
        aliasVector.push_back("Oficina 3");
        aliasVector.push_back("Negocio 3");
    }
    else if ("Work 4" == phoneType) {
        aliasVector.push_back("Trabajo 4");
        aliasVector.push_back("Oficina 4");
        aliasVector.push_back("Negocio 4");
    }
    else if ("Other" == phoneType) {
        aliasVector.push_back("Otro");
        aliasVector.push_back("Otro 1");
    }
    else if ("Other 2" == phoneType) {
        aliasVector.push_back("Otro 2");
    }
    else if ("Other 3" == phoneType) {
        aliasVector.push_back("Otro 3");
    }
    else if ("Other 4" == phoneType) {
        aliasVector.push_back("Otro 4");
    }
    else {
    }
}

void VR_VoiceBoxXmlParser::setFRPhoneTypeAlis(const std::string phoneType, VoiceVector<std::string>::type& aliasVector)
{
    VR_LOG("phoneType : %s", phoneType.c_str());
    if ("Mobile" == phoneType) {
        aliasVector.push_back("Portable");
        aliasVector.push_back("Téléphone portable");
        aliasVector.push_back("Cellulaire");
        aliasVector.push_back("Téléphone cellulaire");
        aliasVector.push_back("Mobile");
        aliasVector.push_back("Téléphone mobile");
        aliasVector.push_back("Cel");
        aliasVector.push_back("Portable 1");
    }
    else if ("Mobile 2" == phoneType) {
        aliasVector.push_back("Portable 2");
        aliasVector.push_back("Téléphone portable 2");
        aliasVector.push_back("Cellulaire 2");
        aliasVector.push_back("Téléphone cellulaire 2");
        aliasVector.push_back("Mobile 2");
        aliasVector.push_back("Téléphone mobile 2");
        aliasVector.push_back("Cel 2");
    }
    else if ("Mobile 3" == phoneType) {
        aliasVector.push_back("Portable 3");
        aliasVector.push_back("Téléphone portable 3");
        aliasVector.push_back("Cellulaire 3");
        aliasVector.push_back("Téléphone cellulaire 3");
        aliasVector.push_back("Mobile 3");
        aliasVector.push_back("Téléphone mobile 3");
        aliasVector.push_back("Cel 3");
    }
    else if ("Mobile 4" == phoneType) {
        aliasVector.push_back("Portable 4");
        aliasVector.push_back("Téléphone portable 4");
        aliasVector.push_back("Cellulaire 4");
        aliasVector.push_back("Téléphone cellulaire 4");
        aliasVector.push_back("Mobile 4");
        aliasVector.push_back("Téléphone mobile 4");
        aliasVector.push_back("Cel 4");
    }
    else if ("Home" == phoneType) {
        aliasVector.push_back("Domicile");
        aliasVector.push_back("Maison");
        aliasVector.push_back("Fixe");
        aliasVector.push_back("Domicile 1");
    }
    else if ("Home 2" == phoneType) {
        aliasVector.push_back("Domicile 2");
        aliasVector.push_back("Maison 2");
        aliasVector.push_back("Fixe 2");
    }
    else if ("Home 3" == phoneType) {
        aliasVector.push_back("Domicile 3");
        aliasVector.push_back("Maison 3");
        aliasVector.push_back("Fixe 3");
    }
    else if ("Home 4" == phoneType) {
        aliasVector.push_back("Domicile 4");
        aliasVector.push_back("Maison 4");
        aliasVector.push_back("Fixe 4");
    }
    else if ("Work" == phoneType) {
        aliasVector.push_back("Travail");
        aliasVector.push_back("Bureau");
        aliasVector.push_back("Professionnel");
        aliasVector.push_back("Boulo");
        aliasVector.push_back("Entreprise");
        aliasVector.push_back("Travail 1");
    }
    else if ("Work 2" == phoneType) {
        aliasVector.push_back("Travail 2");
        aliasVector.push_back("Bureau 2");
        aliasVector.push_back("Professionnel 2");
        aliasVector.push_back("Boulo 2");
        aliasVector.push_back("Entreprise 2");
    }
    else if ("Work 3" == phoneType) {
        aliasVector.push_back("Travail 3");
        aliasVector.push_back("Bureau 3");
        aliasVector.push_back("Professionnel 3");
        aliasVector.push_back("Boulo 3");
        aliasVector.push_back("Entreprise 3");
    }
    else if ("Work 4" == phoneType) {
        aliasVector.push_back("Travail 4");
        aliasVector.push_back("Bureau 4");
        aliasVector.push_back("Professionnel 4");
        aliasVector.push_back("Boulo 4");
        aliasVector.push_back("Entreprise 4");
    }
    else if ("Other" == phoneType) {
        aliasVector.push_back("Autre");
        aliasVector.push_back("Autre 1");
    }
    else if ("Other 2" == phoneType) {
        aliasVector.push_back("Autre 2");
    }
    else if ("Other 3" == phoneType) {
        aliasVector.push_back("Autre 3");
    }
    else if ("Other 4" == phoneType) {
        aliasVector.push_back("Autre 4");
    }
    else {
    }
}

void VR_VoiceBoxXmlParser::setFormalAliasPronInfo(const pugi::xml_node& rootNode,
    VoiceVector<FormalPronInfo>::type& formalPronVec)
{
    formalPronVec.clear();

    FormalPronInfo formalPronInfo;
    if (rootNode.first_child()) {
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            setFormal(rootChildNode, formalPronInfo.formalItem);
            pugi::xml_node itemNode;
            if (rootChildNode.first_child()) {
                itemNode = rootChildNode.first_child();
            }

            setAlias(itemNode, "pron", formalPronInfo.aliasMap);

            formalPronVec.push_back(formalPronInfo);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void VR_VoiceBoxXmlParser::setAudioSource(const pugi::xml_node& rootNode,
    VoiceVector<AudioSourceInfo>::type& audioSourceVec)
{
    audioSourceVec.clear();

    AudioSourceInfo audioSource;
    if (rootNode.first_child()) {
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {

            setFormal(rootChildNode, "grammarid", audioSource.formalItem);

            pugi::xml_node itemNode;
            if (rootChildNode.first_child()) {
                itemNode = rootChildNode.first_child();
            }

            setAlias(itemNode, "tts", audioSource.aliasMap);

            audioSourceVec.push_back(audioSource);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void VR_VoiceBoxXmlParser::setFormalShortCutPronInfo(const std::string& xml,
    VoiceVector<FormalShortCutPronInfo>::type& formalShortCutPronInfoVec)
{
    pugi::xml_document xml_Formal;

    if (NULL == xml.c_str()) {
        return;
    }
    else {
        if (!xml_Formal.load(xml.c_str())) {
            std::cout << "load fail " << std::endl;
            return;
        }
    }

    formalShortCutPronInfoVec.clear();

    pugi::xml_node rootNode = getRootNode(xml_Formal);

    FormalShortCutPronInfo formalShortCutPron;
    if (rootNode.first_child()) {
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            for (pugi::xml_attribute attr = rootChildNode.first_attribute(); attr; attr = attr.next_attribute()) {
                if (0 == strcmp("id", attr.name())) {
                    formalShortCutPron.strId = attr.value();
                }
                else if (0 == strcmp("name", attr.name())) {
                    formalShortCutPron.name = attr.value();
                }
                else if (0 == strcmp("shortcut", attr.name())) {
                    formalShortCutPron.shortcut = attr.value();
                }
            }
            if (rootChildNode.first_child()) {
                pugi::xml_node itemNode = rootChildNode.first_child();
                std::string strName;
                std::string strPron;
                VoiceMap<std::string, std::string>::type mapAliasPron;

                while (itemNode) {
                    strName.clear();
                    strPron.clear();
                    for (pugi::xml_attribute attr = itemNode.first_attribute(); attr; attr = attr.next_attribute()) {
                        if (0 == strcmp("name", attr.name())) {
                            strName = attr.value();
                        }

                        if (0 == strcmp("pron", attr.name())) {
                            strPron = attr.value();
                        }
                    }
                    mapAliasPron.insert(std::make_pair(strName, strPron));

                    itemNode = itemNode.next_sibling();
                }
                formalShortCutPron.aliasMap = mapAliasPron;
            }

            formalShortCutPronInfoVec.push_back(formalShortCutPron);
            rootChildNode = rootChildNode.next_sibling();
        }
    }

    return;
}

void VR_VoiceBoxXmlParser::setMusicDiff(const std::string& strCategory, const std::string& strDiffType)
{
    if (strCategory.empty()) {
        return;
    }

    pugi::xml_document xmlMusicDiff;
    if (!xmlMusicDiff.load(strCategory.c_str())) {
        std::cout << "load fail" << std::endl;
        return;
    }

    pugi::xml_node rootNode = getRootNode(xmlMusicDiff);
    std::string categoryNameAttr;
    for (pugi::xml_attribute attr = rootNode.first_attribute(); attr; attr = attr.next_attribute()) {
        if (0 == strcmp("name", attr.name())) {
            categoryNameAttr = attr.value();
            break;
        }
    }

    if (categoryNameAttr.empty()) {
        return;
    }

    if (rootNode.first_child()) {
        pugi::xml_node rootChildNode = rootNode.first_child();
        while (rootChildNode) {
            if ("delete" == strDiffType) {
                setMusicDel(categoryNameAttr, rootChildNode);
            }
            else if ("add" == strDiffType) {
                setMusicAdd(categoryNameAttr, rootChildNode);
            }
            rootChildNode = rootChildNode.next_sibling();
        }
    }

}

void
VR_VoiceBoxXmlParser::setMusicDel(const std::string& categoryNameAttr,
    const pugi::xml_node& rootChildNode)
{
    MusicDiffDelete musicDiffDelete;

    VoiceVector<MusicDiffDelete>::type vecMusicDiffDelete; // music diff grammar
    pugi::xml_node deleteChildNode = rootChildNode.first_child();
    while (deleteChildNode) {

        for (pugi::xml_attribute attr = deleteChildNode.first_attribute(); attr; attr = attr.next_attribute()) {
            if (0 == strcmp("id", attr.name())) {
                musicDiffDelete.id = attr.value();
            }
            if (0 == strcmp("name", attr.name())) {
                musicDiffDelete.name = attr.value();
            }
        }
        vecMusicDiffDelete.push_back(musicDiffDelete);
        deleteChildNode = deleteChildNode.next_sibling();
    }

    if (!vecMusicDiffDelete.empty()) {
        m_musicDiffDelete.insert(std::make_pair(categoryNameAttr, vecMusicDiffDelete));
    }

}

void
VR_VoiceBoxXmlParser::setMusicAdd(const std::string& categoryNameAttr,
    const pugi::xml_node& rootChildNode)
{
    // do not clear m_musicDiffAdd
    VoiceVector<MusicDiffAdd>::type vecMusicDiffAdd;       // music diff grammar
    MusicDiffAdd musicDiffAdd;
    pugi::xml_node addChildNode = rootChildNode.first_child();
    while (addChildNode) {
        for (pugi::xml_attribute attr = addChildNode.first_attribute(); attr; attr = attr.next_attribute()) {
            if (0 == strcmp("id", attr.name())) {
                musicDiffAdd.id = attr.value();
                continue;
            }
            if (0 == strcmp("name", attr.name())) {
                musicDiffAdd.name = attr.value();
                continue;
            }
            if (0 == strcmp("pron", attr.name())) {
                musicDiffAdd.pron = attr.value();
                continue;
            }
            if (0 == strcmp("pronid", attr.name())) {
                musicDiffAdd.pronid = attr.value();
                continue;
            }
        }
        vecMusicDiffAdd.push_back(musicDiffAdd);
        addChildNode = addChildNode.next_sibling();
    }

    if (!vecMusicDiffAdd.empty()) {
        m_musicDiffAdd.insert(std::make_pair(categoryNameAttr, vecMusicDiffAdd));
    }

}

/**
 * get the action type
 *
 * @return the type of action
 */

std::string VR_VoiceBoxXmlParser::getActionType()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("type", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the update type
 *
 * @return the type of update
 */

std::string VR_VoiceBoxXmlParser::getUpdateType()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("type", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the action state
 *
 * @return the state of acton
 */

std::string VR_VoiceBoxXmlParser::getActionState()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("state", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the action agent
 *
 * @return the name of agent
 */

std::string VR_VoiceBoxXmlParser::getAgentName()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("agent", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the value by key
 *
 * @param key : the key
 * @return the value of key
 */

std::string VR_VoiceBoxXmlParser::getValueByKey(const std::string& key)
{
    if (key.empty()) {
        return "";
    }
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp(key.c_str(), (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the message class attribute value
 *
 * @return the value of class
 */

std::string VR_VoiceBoxXmlParser::getMsgClass()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("class", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the message source attribute value
 *
 * @return the value source
 */

std::string VR_VoiceBoxXmlParser::getMsgSource()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("source", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}
/**
 * get the action class attribute value
 *
 * @return the value of action's class
 */

std::string VR_VoiceBoxXmlParser::getActionClass()
{
    std::string value = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("class", (*iteratorValue).Name.c_str()) == 0) {
            value = (*iteratorValue).Value;
            break;
        }
    }
    return value;
}

/**
 * get the information of event node
 *
 * @return the value of event
 */

std::string VR_VoiceBoxXmlParser::getSendEvent()
{
    std::string eventValue = "";
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("event", (*iteratorValue).Name.c_str()) == 0) {
            eventValue = (*iteratorValue).Value;
            break;
        }
    }
    return eventValue;
}

/**
 * get the information of item node
 *
 * @param key : the key that we retrieve
 * @rerurn the value of item
 */

VoiceVector<std::string>::type VR_VoiceBoxXmlParser::getListItemValue(const std::string& key)
{
    VoiceVector<std::string>::type listItemVector;
    if (key.empty()) {
        return listItemVector;
    }
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp(key.c_str(), (*iteratorValue).Name.c_str()) == 0) {
            listItemVector.push_back((*iteratorValue).Value);
        }
    }
    setListSize(listItemVector.size());
    return listItemVector;
}

/**
 * get the information of list struct node
 *
 * @param nodeString : the group of string
 * @return the node of group
 */

VoiceVector<StructNode>::type VR_VoiceBoxXmlParser::getListStructValue(const std::string* nodeString)
{
    VoiceVector<StructNode>::type listItemVector;
    if (NULL == nodeString) {
        return listItemVector;
    }
    size_t count = 0;
    StructNode mStructNode;
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        while (0 == strcmp("", nodeString[count].c_str())) {
            count = 0;
        }
        if (strcmp(nodeString[count].c_str(), (*iteratorValue).Name.c_str()) == 0) {
            mStructNode.Name = (*iteratorValue).Name;
            mStructNode.Value = (*iteratorValue).Value;
            listItemVector.push_back(mStructNode);
            ++count;
        }

    }
    setListSize(listItemVector.size());
    return listItemVector;
}

/**
 * get the information of parameter node
 *
 * @return the information of parameter
 */

VoiceVector<Parameter>::type VR_VoiceBoxXmlParser::getMsgParameterValue()
{
    Parameter parameter;
    VoiceVector<Parameter>::type parameterVector;
    VoiceVector<StructNode>::iterator iteratorValue;
    for (iteratorValue = m_vector.begin(); iteratorValue != m_vector.end(); ++iteratorValue) {
        if (strcmp("name", (*iteratorValue).Name.c_str()) == 0) {
            parameter.name = (*iteratorValue).Name;
            parameter.nameValue = (*iteratorValue).Value;
        }
        if (strcmp("value", (*iteratorValue).Name.c_str()) == 0) {
            parameter.parameterValue = (*iteratorValue).Name;
            parameter.value = (*iteratorValue).Value;
            parameterVector.push_back(parameter);
        }
    }

    return parameterVector;
}

/**
 * get the information of parameterNameValue
 *
 * @param parameterStructVector : the group of parameter
 * @param strName : the name of we retrieve
 * @return the value of parameter
 */

std::string VR_VoiceBoxXmlParser::getMsgParameterValue(VoiceVector<Parameter>::type parameterStructVector, const std::string& strName)
{
    if ((!parameterStructVector.size()) || strName.empty()) {
        return "";
    }
    std::string parameterValue;
    VoiceVector<Parameter>::iterator iteratorValue;
    for (iteratorValue = parameterStructVector.begin(); iteratorValue != parameterStructVector.end(); ++iteratorValue) {
        if (strcmp(strName.c_str(), (*iteratorValue).nameValue.c_str()) == 0) {
            parameterValue = (*iteratorValue).value;
            break;
        }
    }
    return parameterValue;
}

/**
 * get the infomation of person
 *
 * @return the vector of person
 */

void
VR_VoiceBoxXmlParser::getMessageInfo(VoiceVector<FormalTwoItem>::type& messageInfo)
{
    messageInfo = m_messageInfo;
}

void
VR_VoiceBoxXmlParser::getPhoneType(VoiceVector<FormalInfo>::type& phoneType)
{
    phoneType = m_phoneType;
}

void
VR_VoiceBoxXmlParser::getMessageType(VoiceVector<FormalInfo>::type& messageType)
{
    messageType = m_messageType;
}

void
VR_VoiceBoxXmlParser::getAudioSource(VoiceVector<AudioSourceInfo>::type& audioSource)
{
    audioSource.clear();
    audioSource = m_audioSource;
}

void
VR_VoiceBoxXmlParser::getFmGenre(VoiceVector<FormalPronInfo>::type& fmGenre)
{
    fmGenre.clear();
    fmGenre = m_fmGenre;
}

void
VR_VoiceBoxXmlParser::getSatChannelName(VoiceVector<FormalPronInfo>::type& satChannelName)
{
    satChannelName.clear();
    satChannelName = m_satChannelName;
}

void
VR_VoiceBoxXmlParser::getSatChannelNumber(VoiceVector<FormalTwoItem>::type& satChannelNumber)
{
    satChannelNumber.clear();
    satChannelNumber = m_satChannelNumber;
}

void
VR_VoiceBoxXmlParser::getSatGenre(VoiceVector<FormalPronInfo>::type& satGenre)
{
    satGenre.clear();
    satGenre = m_satGenre;
}

void
VR_VoiceBoxXmlParser::getHdSubChannel(VoiceVector<FormalOtherInfo>::type& hdSubChannel)
{
    hdSubChannel.clear();
    hdSubChannel = m_hdSubChannel;
}

void VR_VoiceBoxXmlParser::getApps(VoiceVector<AppCategory>::type& vecAppCategory)
{
    vecAppCategory.clear();
    pugi::xml_node rootNode = m_xml_document.first_child();

    pugi::xml_node categoryNode = rootNode;
    pugi::xml_node formalNode = rootNode;
    pugi::xml_node aliasNode = rootNode;

    Alias aliasTmp;
    AppFormal appFormalTmp;
    AppCategory appCategoryTmp;

    while (categoryNode) {
        appCategoryTmp.vecFormal.clear();
        if (!getGivedNode(categoryNode, "category")) {
            break;
        }
        setAppCategory(categoryNode, appCategoryTmp);

        formalNode = categoryNode;
        while (formalNode) {
            appFormalTmp.vecAlias.clear();
            if (!getGivedNode(formalNode, "formal")) {
                break;
            }

            setAppFormal(formalNode, appFormalTmp);
            aliasNode = formalNode;
            while (aliasNode) {
                if (!getGivedNode(aliasNode, "alias")) {
                    break;
                }
                setAlias(aliasNode, aliasTmp);

                appFormalTmp.vecAlias.push_back(aliasTmp);
                aliasNode = aliasNode.next_sibling();
            }

            appCategoryTmp.vecFormal.push_back(appFormalTmp);
            formalNode = formalNode.next_sibling();
        }
        vecAppCategory.push_back(appCategoryTmp);
        categoryNode = categoryNode.next_sibling();
    }
}

bool
VR_VoiceBoxXmlParser::getGivedNode(
    pugi::xml_node& node,
    const std::string& strGivedName
    )
{
    pugi::xml_node nodeNext = node.next_sibling();
    pugi::xml_node nodeChild = node.first_child();

    std::string strName = node.name();
    if (node) {
        if (0 == strcmp(strName.c_str(), strGivedName.c_str())) {
            return true;
        }

        if (getGivedNode(nodeChild, strGivedName)) {
            node = nodeChild;
            return true;
        }

        if (getGivedNode(nodeNext, strGivedName)) {
            node = nodeNext;
            return true;
        }
    }
    return false;
}

void
VR_VoiceBoxXmlParser::setAppCategory(
    const pugi::xml_node& node,
    AppCategory& appCategory
)
{
    appCategory.strName = "";
    for (pugi::xml_attribute attr = node.first_attribute();
        attr;
        attr = attr.next_attribute()) {
        if (0 == strcmp("name", attr.name())) {
            appCategory.strName = attr.value();
        }
    }
}

void
VR_VoiceBoxXmlParser::setAppFormal(
    const pugi::xml_node& node,
    AppFormal& appFormal
)
{
    appFormal.strId = "";
    appFormal.strName = "";
    appFormal.strShortCut = "";

    for (pugi::xml_attribute attr = node.first_attribute();
        attr;
        attr = attr.next_attribute()) {
        if (0 == strcmp("id", attr.name())) {
            appFormal.strId = attr.value();
        }
        else if (0 == strcmp("name", attr.name())) {
            appFormal.strName = attr.value();
        }
        else if (0 == strcmp("shortcut", attr.name())) {
            appFormal.strShortCut = attr.value();
        }
    }
}

void
VR_VoiceBoxXmlParser::setAlias(
    const pugi::xml_node& node,
    Alias& alias
)
{
    alias.strName = "";
    alias.strPron = "";

    for (pugi::xml_attribute attr = node.first_attribute();
        attr;
        attr = attr.next_attribute()) {
        if (0 == strcmp("name", attr.name())) {
            alias.strName = attr.value();
        }
        else if (0 == strcmp("pron", attr.name())) {
            alias.strPron = attr.value();
        }
    }
}

void
VR_VoiceBoxXmlParser::getMusicDiffDelete(VoiceMap<std::string, VoiceVector<MusicDiffDelete>::type>::type& musicDiffDelete)
{
    musicDiffDelete.clear();
    musicDiffDelete = m_musicDiffDelete;
}

void
VR_VoiceBoxXmlParser::getMusicDiffAdd(VoiceMap<std::string, VoiceVector<MusicDiffAdd>::type>::type& musicDiffAdd)
{
    musicDiffAdd.clear();
    musicDiffAdd = m_musicDiffAdd;
}

std::string
VR_VoiceBoxXmlParser::getXmlString()
{
    return m_strWholeXml;
}

std::string
VR_VoiceBoxXmlParser::getLastFlg()
{
    return m_strLastFlg;
}

VoiceVector<RecentCallInfo>::type VR_VoiceBoxXmlParser::getRecentCallInfo()
{
    VoiceVector<RecentCallInfo>::type recentCallList;
    pugi::xml_node personalInfoNode = m_xml_document.select_node("//phoneInfo").node();
    while (!personalInfoNode.empty()) {
        RecentCallInfo tempRecentCallInfo;
        tempRecentCallInfo.contactId = personalInfoNode.child("contactId").text().as_string();
        tempRecentCallInfo.phoneKind = personalInfoNode.child("phoneKind").text().as_string();
        tempRecentCallInfo.contactName = personalInfoNode.child("contactName").text().as_string();
        tempRecentCallInfo.phoneType = personalInfoNode.child("phoneType").text().as_string();
        tempRecentCallInfo.number = personalInfoNode.child("number").text().as_string();
        tempRecentCallInfo.timeStamp = personalInfoNode.child("timeStamp").text().as_string();
        tempRecentCallInfo.listType = personalInfoNode.child("listType").text().as_string();
        tempRecentCallInfo.phoneTypeName = personalInfoNode.child("phoneTypeName").text().as_string();
        recentCallList.push_back(tempRecentCallInfo);

        personalInfoNode = personalInfoNode.next_sibling();
    }

    return recentCallList;
}

VoiceVector<RecentMessageInfo>::type VR_VoiceBoxXmlParser::getRecentMessageInfo()
{
    VoiceVector<RecentMessageInfo>::type recentMessageList;
    pugi::xml_node messageInfoNode = m_xml_document.select_node("//messageInfo").node();
    while (!messageInfoNode.empty()) {
        RecentMessageInfo tempRecentMessageInfo;
        tempRecentMessageInfo.instanceId = messageInfoNode.child("instanceId").text().as_string();
        tempRecentMessageInfo.messageId = messageInfoNode.child("messageId").text().as_string();
        tempRecentMessageInfo.messageType = messageInfoNode.child("messageType").text().as_string();
        tempRecentMessageInfo.phoneNumber = messageInfoNode.child("phoneNumber").text().as_string();
        tempRecentMessageInfo.phoneType = messageInfoNode.child("phoneType").text().as_string();
        tempRecentMessageInfo.sender = messageInfoNode.child("sender").text().as_string();
        tempRecentMessageInfo.subject = messageInfoNode.child("subject").text().as_string();
        tempRecentMessageInfo.timeStamp = messageInfoNode.child("timeStamp").text().as_string();
        tempRecentMessageInfo.status = messageInfoNode.child("status").text().as_string();
        recentMessageList.push_back(tempRecentMessageInfo);

        messageInfoNode = messageInfoNode.next_sibling();
    }

    return recentMessageList;
}
/* EOF */
