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

#include <math.h>
#include "stdafx.h"
#include "VR_IntentionParser.h"
#include "VR_DECommonIF.h"
#include "VR_DataAccessor.h"
#include "VR_DataAccessorManager.h"


VR_IntentionParser::VR_IntentionParser(VR_DECommonIF* pDECommonIF)
{
    m_pDECommonIF = pDECommonIF;
}

VR_IntentionParser::~VR_IntentionParser()
{

}

void VR_IntentionParser::preProcAsrResult(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set retNodeSet = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN2005']");
    if (!retNodeSet.empty()) {
        pugi::xpath_node_set::iterator it = retNodeSet.begin();
        it->parent().parent().remove_child(it->parent());
        ++it;
    }
}

void VR_IntentionParser::changePOINameByPOIId(pugi::xml_node asrNode, VR_DataAccessorManager* pAccessorManager)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set poiIdNodes = asrNode.select_nodes("//POI_category_id");
    pugi::xpath_node_set::iterator itPoiIdNode = poiIdNodes.begin();
    while (itPoiIdNode != poiIdNodes.end()) {
        std::string poiId = itPoiIdNode->node().text().as_string();
        pugi::xml_node poiNameNode = itPoiIdNode->node().next_sibling("POI_category_name");
        if (poiNameNode) {
            std::string changeName;
            pAccessorManager->getInfo("_getPOIPrompt", poiId, changeName);
            if (!changeName.empty()) {
                // replace the name
                poiNameNode.text().set(changeName.c_str());
            }
        }
        ++itPoiIdNode;
    }
}


// true is equal
bool VR_IntentionParser::compareItem(pugi::xml_node curItem, pugi::xml_node otherItem)
{
    VR_LOGD_FUNC();
    pugi::xml_node curItemChild = curItem.first_child();
    pugi::xml_node otherItemChild = otherItem.first_child();
    while (curItemChild && otherItemChild) {
        std::string curItemChildValue = curItemChild.text().as_string();
        std::string otherItemChildValue = otherItemChild.text().as_string();
        if (curItemChildValue == otherItemChildValue) {
            curItemChild = curItemChild.next_sibling();
            otherItemChild = otherItemChild.next_sibling();
        }
        else {
            return false;
        }
    }
    return true;
}


void VR_IntentionParser::removeCurItemFollowSameItem(pugi::xml_node curItem)
{
    VR_LOGD_FUNC();
    pugi::xml_node nextItem = curItem.next_sibling();
    while (nextItem) {
        bool isSame = compareItem(curItem, nextItem);
        pugi::xml_node tmpItem = nextItem.next_sibling();
        if (isSame) {
            nextItem.parent().remove_child(nextItem);
        }
        nextItem = tmpItem;
    }
}


void VR_IntentionParser::removeSameItemInNBest(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xml_node listNode = asrNode.select_node("//items").node();
    pugi::xml_node itemNode = listNode.first_child();
    while (itemNode) {
        removeCurItemFollowSameItem(itemNode);
        itemNode = itemNode.next_sibling();
    }

    pugi::xpath_node_set retItems = asrNode.select_nodes("//item");
    int count = retItems.size();
    pugi::xpath_node countNode = asrNode.select_node("//count");
    countNode.node().text().set(count);
}

bool VR_IntentionParser::filterIntention(pugi::xml_node asrNode, bool isNaviFullData, bool isMusicFilter)
{
    VR_LOGD_FUNC();
    // replace sententce in CMN2034
    replaceSentenceValueCMN2034(asrNode);
    // remove same item in NBest
    removeSameItemInNBest(asrNode);

    // replace ## to /
    // replaceWellNum(asrNode);
    replaceWellNumPOI(asrNode);

    intelligentConventRadio(asrNode);
    mergePhoneContactType(asrNode);
    bool ret1 = filterRadioFreqGen(asrNode);
    bool ret2 = true;
    if (isNaviFullData) {
        ret2  = filterNaviCMN(asrNode);
    }
    bool ret3 = filterValueNull(asrNode);
    bool ret4 = true;
    if (isMusicFilter) {
        ret4  = filterMusicCMN(asrNode);
    }
    bool ret = ret1 & ret2 & ret3 & ret4;
    RETURN(ret);
}


void VR_IntentionParser::replaceSentenceValueCMN2034(pugi::xml_node asrNode)
{
    pugi::xml_node nodeIntent = asrNode.select_node("//node[substring(@value, 1, 7) = 'CMN2034']").node();
    if (nodeIntent) {
        std::string sentenceValue = nodeIntent.child("list").child("items").first_child().first_child().text().as_string();
        nodeIntent.attribute("sentenceValue").set_value(sentenceValue.c_str());
    }
}


void VR_IntentionParser::intelligentConventRadio(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set radioNodes = asrNode.select_nodes("//FM_frequency");
    int count = radioNodes.size();
    for (int i = 0; i < count; i++) {
        double radio = radioNodes[i].node().text().as_double();
        if ((8750 > radio) && (radio >= 875)) {
            radio = radio / 10;
        }
        else if (radio >= 8750) {
            radio = radio / 100;
        }
        //
        char buf[30] = {};
        snprintf(buf, sizeof(buf), "%f", radio);
        std::string str = buf;
        std::size_t pos = str.find(".");
        std::string ret;
        if (pos == std::string::npos) {
            ret = str;
        }
        else {
            ret = str.substr(0, pos + 3);
        }
        while (ret.size() != 0) {
            char a = ret.back();
            if ((a == '0') || (a == '.')) {
                ret = ret.substr(0, ret.size() -1);
            }
            else {
                break;
            }
        }
        radioNodes[i].node().text().set(ret.c_str());
    }
}


void VR_IntentionParser::intelligentConventRadioCN(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set radioNodes = asrNode.select_nodes("//FM_frequency");
    int count = radioNodes.size();
    for (int i = 0; i < count; i++) {
        double radio = radioNodes[i].node().text().as_double();
        if ((8750 > radio) && (radio >= 870)) {
            radio = radio / 10;
        }
        else if (radio >= 8700) {
            radio = radio / 100;
        }
        //
        char buf[30] = {};
        snprintf(buf, sizeof(buf), "%f", radio);
        std::string str = buf;
        std::size_t pos = str.find(".");
        std::string ret;
        if (pos == std::string::npos) {
            ret = str;
        }
        else {
            ret = str.substr(0, pos + 3);
        }
        // 
        while (ret.size() != 0) {
            char a = ret.back();
            if ((a == '0') || (a == '.')) {
                ret = ret.substr(0, ret.size() -1);
            }
            else {
                break;
            }
        }
        radioNodes[i].node().text().set(ret.c_str());
    }
}

// if cmn has no ##, only save the first item and need no other action
bool VR_IntentionParser::removeOtherItemsWithOutWell(pugi::xml_node cmnNode)
{
    std::string poiName = cmnNode.attribute("sentenceValue").as_string();
    std::size_t pos = poiName.find(" ## ");
    if (pos != std::string::npos) {
        return false;
    }

    // has no ' ## ', save the first item, remove next nodes
    pugi::xml_node firstItemNode = cmnNode.child("list").child("items").first_child();
    pugi::xml_node secondItemNode = firstItemNode.next_sibling();
    while (secondItemNode) {
        secondItemNode.parent().remove_child(secondItemNode);
        secondItemNode = cmnNode.child("list").child("items").first_child().next_sibling();
    }

    // update count
    pugi::xpath_node_set itemNodes = cmnNode.select_nodes("//item");
    int count = itemNodes.size();
    cmnNode.child("list").child("header").child("count").text().set(count);

    return true;
}


// save the poiNode item the confidence equal the first poiNode item confidence
void VR_IntentionParser::removeNotSameConfidenceItem(pugi::xml_node cmnNode)
{
    pugi::xml_node firstItemNode = cmnNode.child("list").child("items").first_child();
    int poiConfidence = firstItemNode.child("POI_category_name").attribute("confidence").as_int();

    pugi::xml_node nextItem = firstItemNode.next_sibling();
    while (nextItem) {
        pugi::xml_node tmpNextNextItem = nextItem.next_sibling();

        int nextConfidence = nextItem.child("POI_category_name").attribute("confidence").as_int();
        if (nextConfidence != poiConfidence) {
            nextItem.parent().remove_child(nextItem); // remove the item because the confidence is not equal first
        }
        nextItem = tmpNextNextItem;
    }
    // update count
    pugi::xpath_node_set itemNodes = cmnNode.select_nodes("//item");
    int count = itemNodes.size();
    cmnNode.child("list").child("header").child("count").text().set(count);
}


void VR_IntentionParser::replaceWellInSentence(pugi::xml_node cmnNode)
{
    // sentence 'find kfc1 ## kfc2', remove' kfc1 ##'
    std::string cmnSentence = cmnNode.attribute("sentenceValue").as_string();
    std::size_t pos = cmnSentence.find(" ## ");
    if (pos != std::string::npos) {
        std::string poiName = cmnNode.child("list").child("items").first_child().child("POI_category_name").text().as_string();
        std::size_t posWell = poiName.find(" ## ");

        if (posWell != std::string::npos) {
            std::string lastPoi =  poiName.substr(posWell + 4);
            std::size_t poiNamePos = cmnSentence.find(poiName.c_str());

            if (poiNamePos != std::string::npos) {
                cmnSentence.replace(poiNamePos, poiName.size(), lastPoi);
                cmnNode.attribute("sentenceValue").set_value(cmnSentence.c_str());
            }
        }
    }
}

void VR_IntentionParser::replaceWellInPoiNode(pugi::xml_node cmnNode)
{
    // POI_category_name, 'kfc1 ## kfc2', remove 'kfc1 ##'
    pugi::xpath_node_set poiNodes = cmnNode.select_nodes("//POI_category_name");
    pugi::xpath_node_set::iterator it = poiNodes.begin();
    while (it != poiNodes.end()) {
        std::string poiTxt = it->node().text().as_string();
        std::size_t poiWellPos = poiTxt.find(" ## ");
        if (poiWellPos != std::string::npos) {
            poiTxt.replace(poiWellPos, 4, "/");
        }
        it->node().text().set(poiTxt.c_str());
        ++it;
    }
}


void VR_IntentionParser::replaceWellNumPOI(pugi::xml_node asrNode)
{
    std::string xpath = "//node[(substring(@value, 1, 7) = 'CMN2006') or (substring(@value, 1, 7) = 'CMN2008')]";
    pugi::xpath_node_set cmnNodes = asrNode.select_nodes(xpath.c_str());
    if (cmnNodes.size() != 1) {
        return;
    }
    pugi::xml_node cmnNode = cmnNodes[0].node();

    if (removeOtherItemsWithOutWell(cmnNode)) {
        return;
    }
    removeNotSameConfidenceItem(cmnNode);
    replaceWellInSentence(cmnNode);
    replaceWellInPoiNode(cmnNode);
}

// void VR_IntentionParser::replaceWellNum(pugi::xml_node asrNode)
// {
//     VR_LOGD_FUNC();
//     pugi::xpath_node_set intentionNodes = asrNode.select_nodes("//node[@name='intent']");
//     int count2 = intentionNodes.size();
//     for (int i = 0; i < count2; i++) {
//         std::string poiName = intentionNodes[i].node().attribute("sentenceValue").as_string();
//         std::size_t pos = poiName.find(" ## ");
//         while (pos != std::string::npos) {
//             poiName.replace(pos, 4, "/");
//             pos = poiName.find(" ## ");
//         }
//         intentionNodes[i].node().attribute("sentenceValue").set_value(poiName.c_str());
//     }
// }

bool VR_IntentionParser::filterValueNull(pugi::xml_node asrNode)
{
    pugi::xpath_node_set nodeSet = asrNode.select_nodes("//CMN2031_navi_addr_state_x");
    int count = nodeSet.size();
    if (count > 0) {
        //
        pugi::xml_node stateNode = nodeSet[0].node();
        std::string stateValue = stateNode.text().as_string();
        pugi::xml_node cityNode = stateNode.next_sibling();
        std::string cityValue = cityNode.text().as_string();
        pugi::xml_node streetNode = cityNode.next_sibling();
        std::string streetValue = streetNode.text().as_string();
        VR_LOG("state = %s, city = %s, street = %s", stateValue.c_str(), cityValue.c_str(), streetValue.c_str());
        if (stateValue.empty() && cityValue.empty() && streetValue.empty()) {
            RETURN(false);
        }
        else {
            RETURN(true);
        }

        // for (int i = 0; i < count; i++) {
        //     std::string tmp = nodeSet[i].node().text().as_string();
        //     if (tmp == "") {
        //         if (i == (count - 1)) {
        //             RETURN(false);
        //         }
        //     }
        //     else {
        //         RETURN(true);
        //     }
        // }
    }
    RETURN(true);
}


bool VR_IntentionParser::filterMusicCMN(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set retNodeSet = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN3002']");
    int count = retNodeSet.size();
    if (count > 0) {
        RETURN(false);
    }
    pugi::xpath_node_set retNodeSet2 = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN3006']");
    int count2 = retNodeSet2.size();
    if (count2 > 0) {
        RETURN(false);
    }
    pugi::xpath_node_set retNodeSet3 = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN3007']");
    int count3 = retNodeSet3.size();
    if (count3 > 0) {
        RETURN(false);
    }
    pugi::xpath_node_set retNodeSet4 = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN3010']");
    int count4 = retNodeSet4.size();
    if (count4 > 0) {
        RETURN(false);
    }

    RETURN(true);
}



bool VR_IntentionParser::filterNaviCMN(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set retNodeSet = asrNode.select_nodes("//node[substring(@value, 1, 5) = 'CMN20']");
    int count = retNodeSet.size();
    if (count > 0) {
        RETURN(false);
    }
    pugi::xpath_node_set retNodeSet2 = asrNode.select_nodes("//node[substring(@value, 1, 7) = 'CMN0001']");
    int count2 = retNodeSet2.size();
    if (count2 > 0) {
        RETURN(false);
    }
    RETURN(true);
}

void VR_IntentionParser::mergePhoneContactType(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    pugi::xpath_node_set nodeSet = asrNode.select_nodes("//node[substring(@value, 8, 14) = '_phone_contact']");
    int count = nodeSet.size();
    if (count <= 0) {
        return;
    }
    // compability old data
    pugi::xpath_node_set typeNodeSet = asrNode.select_nodes("//contact_phone_type");
    int typeCount = typeNodeSet.size();
    for (int i = 0; i < typeCount; ++i) {
        std::string typeStr = typeNodeSet[i].node().text().as_string();
        if ((typeStr == "preset") || (typeStr == "default")) {
            typeNodeSet[i].node().text().set("phone_contact_type_default");
        }
        else if ((typeStr == "mobile") || (typeStr == "phone") || (typeStr == "phone_contact_type_mobile")) {
            typeNodeSet[i].node().text().set("phone_contact_type_mobile_0");
        }
        else if ((typeStr == "home") || (typeStr == "house") || (typeStr == "phone_contact_type_home")) {
            typeNodeSet[i].node().text().set("phone_contact_type_home_0");
        }
        else if ((typeStr == "work") || (typeStr == "company") || (typeStr == "phone_contact_type_work")) {
            typeNodeSet[i].node().text().set("phone_contact_type_work_0");
        }
        else if ((typeStr == "other") || (typeStr == "others") || (typeStr == "other phone") || (typeStr == "phone_contact_type_other")) {
            typeNodeSet[i].node().text().set("phone_contact_type_other_0");
        }

    }

    removeSameNode(asrNode, "phone_contact_type_default", 3);

    std::ostringstream oss;
    for (int index = 0; index < 5; ++index) {
        oss << index;
        removeSameNode(asrNode, std::string("phone_contact_type_mobile_").append(oss.str()), 0 + index * 4);
        removeSameNode(asrNode, std::string("phone_contact_type_home_").append(oss.str()), 1 + index * 4);
        removeSameNode(asrNode, std::string("phone_contact_type_work_").append(oss.str()), 2 + index * 4);
        removeSameNode(asrNode, std::string("phone_contact_type_other_").append(oss.str()), 3 + index * 4);
        oss.str("");
    }
}

void VR_IntentionParser::removeSameNode(pugi::xml_node asrNode, const std::string& contactType, int id)
{
    VR_LOGD_FUNC();
    pugi::xml_node countNode = asrNode.select_node("//count").node();
    int count = countNode.text().as_int();


    std::string xpathStr = "//item[contact_phone_type='" + contactType + "']";
    pugi::xpath_node_set itemNodeSet = asrNode.select_nodes(xpathStr.c_str());
    int itemCount = itemNodeSet.size();
    for (int i = 0; i < itemCount; i++) {
        pugi::xml_node it = itemNodeSet[i].node();
        std::string contactId = it.child("contact_id").text().as_string();

        it.child("contact_phone_type").text().set(id);
        std::stringstream ss;
        ss << id;
        std::string name = VR_DataAccessor::getPhoneTypeName(ss.str());
        it.append_child("contact_phone_type_name").text().set(name.c_str());

        for (int j = i + 1; j < itemCount; j++) {
            pugi::xml_node itj = itemNodeSet[j].node();
            std::string contactIdj = itj.child("contact_id").text().as_string();
            if (contactId == contactIdj) {
                itj.parent().remove_child(itj);
                count--;
            }
        }
    }
    countNode.text().set(count);
}



bool VR_IntentionParser::filterRadioFreqGen(pugi::xml_node asrNode)
{
    VR_LOGD_FUNC();
    VR_DECommonIF::DE_Country country = m_pDECommonIF->getCountryType();

    double AM_Min  = 0.0;
    double AM_Max  = 0.0;
    double AM_Step = 0.0;

    double FM_Min  = 0.0;
    double FM_Max  = 0.0;
    double FM_Step = 0.0;

    double PAM_Min = 0.0;
    double PAM_Max = 0.0;

    double PFM_Min = 0.0;
    double PFM_Max = 0.0;

    double DAB_Min = 0.0;
    double DAB_Max = 0.0;
    if (VR_DECommonIF::gen == country) { // general region
        AM_Min = 531;
        AM_Max = 1602;
        AM_Step = 9;

        FM_Min = 87.5;
        FM_Max = 108.0;
        FM_Step = 0.05;

        PAM_Min = 1;
        PAM_Max = 12;

        PFM_Min = 1;
        PFM_Max = 12;

        DAB_Min = 1;
        DAB_Max = 12;

    }
    else if (VR_DECommonIF::eu == country) { // Europe
        AM_Min = 531;
        AM_Max = 1602;
        AM_Step = 9;

        FM_Min = 87.5;
        FM_Max = 108.0;
        FM_Step = 0.05;

        PAM_Min = 1;
        PAM_Max = 6;

        PFM_Min = 1;
        PFM_Max = 6;

        DAB_Min = 1;
        DAB_Max = 6;
    }
    else if (VR_DECommonIF::cn == country) { // china
        AM_Min = 531;
        AM_Max = 1602;
        AM_Step = 9;

        FM_Min = 87.0;
        FM_Max = 108.0;
        FM_Step = 0.1;

        PAM_Min = 1;
        PAM_Max = 6;

        PFM_Min = 1;
        PFM_Max = 12;

        DAB_Min = 0; // no exit
        DAB_Max = 99999; // no exit
    }

    pugi::xml_node countNode = asrNode.select_node("//count").node();
    int count = countNode.text().as_int();
    VR_LOG("count = %d", count);

    // AM
    pugi::xpath_node_set amXpathNodes = asrNode.select_nodes("//AM_frequency");
    int amCount = amXpathNodes.size();
    VR_LOG("find AM_frequency count = %d", amCount);
    if (amCount > 0) {
        for (int i = 0; i < 1; ++i) {
            pugi::xml_node thisNode = amXpathNodes[i].node();
            double value = thisNode.text().as_double();
            VR_LOG("value = %f, AM_Min = %f, AM_Max = %f, AM_Step = %f", value, AM_Min, AM_Max, AM_Step);
            if (value < AM_Min || value > AM_Max || (fmod(value, AM_Step) != 0)) {
                VR_LOG("remove_child AM_frequency");
                thisNode.parent().parent().remove_child(thisNode.parent());
                // count--;
                RETURN(false);
            }
        }
    }

    // FM
    pugi::xpath_node_set fmXpathNodes = asrNode.select_nodes("//FM_frequency");
    int fmCount = fmXpathNodes.size();
    VR_LOG("find FM_frequency count = %d", fmCount);
    if (fmCount > 0) {
        for (int i = 0; i < 1; ++i) {
            pugi::xml_node thisNode = fmXpathNodes[i].node();
            double value = thisNode.text().as_double();
            VR_LOG("value = %f, FM_Min = %f, FM_Max = %f, FM_Step = %f", value, FM_Min, FM_Max, FM_Step);
            bool bStep = ((int)(value * 100)) % ((int)(FM_Step * 100)); // (fmod(value, FM_Step) != 0;
            if (value < FM_Min || value > FM_Max || bStep) {
                thisNode.parent().parent().remove_child(thisNode.parent());
                VR_LOG("remove_child FM_frequency");
                // count--;
                RETURN(false);
            }
        }
    }
    bool isPreset;
    // Present AM
    isPreset = checkRemoveNode(asrNode, count, "//node[substring(@value, 8, 21) = '_media_AM_preset_num_']", "//preset_number", PAM_Min, PAM_Max, 0);
    if (!isPreset) {
        RETURN(false);
    }

    // pugi::xpath_node_set pAMXpathNodes = asrNode.select_nodes("//node[substring(@value, 8, 21) = '_media_AM_preset_num_']");
    // int pAMCount = pAMXpathNodes.size();
    // if (pAMCount > 0) {
    //  for (int i = 0; i < pAMCount; ++i) {
    //      pugi::xml_node thisNode = pAMXpathNodes[i].node();
    //      double value = thisNode.attribute("value").as_double();
    //      if (value < PAM_Min || value > PAM_Max) {
    //          thisNode.parent().remove_child(thisNode);
    //          count--;
    //      }
    //  }
    // }

    // Present FM
    isPreset = checkRemoveNode(asrNode, count, "//node[substring(@value, 8, 21) = '_media_FM_preset_num_']", "//preset_number", PFM_Min, PFM_Max, 0);
    if (!isPreset) {
        RETURN(false);
    }

    // pugi::xpath_node_set pFMXpathNodes = asrNode.select_nodes("//node[substring(@value, 8, 21) = '_media_FM_preset_num_']");
    // int pFMCount = pFMXpathNodes.size();
    // if (pFMCount > 0) {
    //  for (int i = 0; i < pFMCount; ++i) {
    //      pugi::xml_node thisNode = pFMXpathNodes[i].node();
    //      double value = thisNode.attribute("value").as_double();
    //      if (value < PFM_Min || value > PFM_Max) {
    //          thisNode.parent().remove_child(thisNode);
    //          count--;
    //      }
    //  }
    // }

    // Present DAB
    isPreset = checkRemoveNode(asrNode, count, "//node[substring(@value, 8, 22) = '_media_DAB_preset_num_']", "//preset_number", DAB_Min, DAB_Max, 0);
    if (!isPreset) {
        RETURN(false);
    }

    // pugi::xpath_node_set pDABXpathNodes = asrNode.select_nodes("//node[substring(@value, 8, 22) = '_media_DAB_preset_num_']");
    // int pDABCount = pDABXpathNodes.size();
    // if (pDABCount > 0) {
    //  pugi::xpath_node_set nodeSet = pDABXpathNodes[1].node().select_nodes("//preset_numbe");
    //  int nodeCount = nodeSet.size();
    //  for (int i = 0; i < nodeCount; ++i) {
    //      pugi::xml_node thisNode = nodeSet[i].node();


    //      double value = thisNode.text().as_double();
    //      if (value < DAB_Min || value > DAB_Max) {
    //          thisNode.parent().parent().remove_child(thisNode.parent());
    //          count--;
    //      }
    //  }
    // }
    countNode.text().set(count);
    VR_LOG("the result count = %d", count);
    RETURN(true);
}

bool VR_IntentionParser::checkRemoveNode(pugi::xml_node asrNode, int& count, const std::string& selectXpathStr, const std::string& nodeName, double min, double max, double step)
{
    pugi::xpath_node_set pDABXpathNodes = asrNode.select_nodes(selectXpathStr.c_str());

    int pDABCount = pDABXpathNodes.size();
    VR_LOG("checkRemoveNode pDABCount1 = %d", pDABCount);
    if (pDABCount > 0) {
        pugi::xpath_node_set nodeSet = pDABXpathNodes[0].node().select_nodes(nodeName.c_str());
        int nodeCount = nodeSet.size();
        VR_LOG("checkRemoveNode nodeCount2 = %d", nodeCount);
        for (int i = 0; i < 1; ++i) {
            pugi::xml_node thisNode = nodeSet[i].node();

            double value = thisNode.text().as_double();
            VR_LOG("value = %f", value);
            bool is;
            if (step == 0) {
                is = value < min || value > max;
            }
            else {
                is = value < min || value > max || (fmod(value, step) != 0);

            }
            if (is) {
                thisNode.parent().parent().remove_child(thisNode.parent());
                VR_LOG("remove_child %s", selectXpathStr.c_str());
                // count--;
                RETURN(false);
            }
        }
    }
    RETURN(true);
}




/* EOF */
