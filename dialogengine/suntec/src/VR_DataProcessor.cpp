/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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
 * @file VR_DeriveDataModel.cpp
 * @brief assign datamodel
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "config.h"
#include "VR_DataProcessor.h"
#include "VR_DECommonIF.h"
#include <boost/assign/list_of.hpp>



VoiceMap<std::string, std::string>::type VR_DataProcessor::m_mapVRLang2DataLang = boost::assign::map_list_of
        ("ms", "en-gb")
        ("id", "en-gb")
        ("vi", "en-gb")
        ("ar-ml", "ar")
        ("ar-fm", "ar");



VR_DataProcessor::VR_DataProcessor(const std::string& path)
    : m_path(path)
{

}

VR_DataProcessor::~VR_DataProcessor()
{

}

bool VR_DataProcessor::initData(const std::string& language)
{
    VR_LOGD_FUNC();
    m_language = language;
    VoiceMap<std::string, std::string>::iterator it = m_mapVRLang2DataLang.find(language);
    if (it != m_mapVRLang2DataLang.end()) {
        m_language = it->second;
    }
    std::string hintsPath = m_path + m_language + "/hints/hints.xml";
    std::string moreHintsPath = m_path + m_language + "/hints/morehints.xml";
    VR_LOGD("init hints form %s", hintsPath.c_str());
    clearHintsList();
    initHinis("list_", hintsPath);
    initHinis("list_more_", moreHintsPath);
    return true;
}

void VR_DataProcessor::updateListByActionResult(pugi::xml_node& action_result)
{
    VR_LOGD_FUNC();
    pugi::xml_node listNode = action_result.select_node("//list").node();
    // save the node in DE
    std::string listId = std::string("list_") + action_result.attribute("op").as_string(); // listNode.attribute("id").as_string();
    listNode.remove_attribute("id");
    listNode.append_attribute("id").set_value(listId.c_str());
    std::string xpathStr = "//list[@id='" + listId + "']";
    pugi::xml_node oldListNode = _listData.select_node(xpathStr.c_str()).node();
    if (NULL != oldListNode) {
        _listData.remove_child(oldListNode);
    }
    _listData.append_copy(listNode);
    return;
}

void VR_DataProcessor::updateListByDataAccessor(const pugi::xml_node& dataAccessor_result)
{
    VR_LOGD_FUNC();
    pugi::xml_node listNode = dataAccessor_result.select_node("//list").node();
    std::string listId = listNode.attribute("id").as_string();
    std::string xpathStr = "//list[@id='" + listId + "']";
    pugi::xml_node oldListNode = _listData.select_node(xpathStr.c_str()).node();
    if (NULL != oldListNode) {
        _listData.remove_child(oldListNode);
    }
    _listData.append_copy(listNode);
    return;
}

bool VR_DataProcessor::updateListByAsr(const std::string& asr_result)
{
    VR_LOGD_FUNC();
    pugi::xml_document doc;
    doc.load_string(asr_result.c_str());
    pugi::xml_node listNode = doc.select_node("//list").node();
    std::string listId = listNode.attribute("id").as_string();
    // listNode.remove_attribute("id");
    // listNode.append_attribute("id").set_value(listId.c_str());
    std::string xpathStr = "//list[@id='" + listId + "']";
    pugi::xml_node oldListNode = _listData.select_node(xpathStr.c_str()).node();
    if (NULL != oldListNode) {
        _listData.remove_child(oldListNode);
    }
    _listData.append_copy(listNode);
    return true;
}

std::string VR_DataProcessor::getHintsCap(const std::string& evtName, const std::string& agentName)
{
    VR_LOGD_FUNC();
    if (evtName == "_getHints") {
        std::string listFrame = getHintCap("list_", agentName);
        return listFrame;
    }
    else if (evtName == "_getMoreHints") {
        std::string listFrame = getHintCap("list_more_", agentName);
        return listFrame;
    }
    else {
        return "";
    }
}



void VR_DataProcessor::addItemsToHintsDispaly(pugi::xml_node& hintsOrSelectListsNodeScxml, bool isNavi, bool isInfo)
{
    VR_LOGD_FUNC();
    // get the params (hints) and get some data into _listDataForUI
    std::string listIdScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().attribute("id").as_string();
    std::string pageSizeStrScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().child("header").child("pageSize").child_value();
    int pageSizeScxml = atoi(pageSizeStrScxml.c_str());
    std::string startIndexStrScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().child("header").child("startIndex").child_value();
    int startIndexScxml = atoi(startIndexStrScxml.c_str());
    std::string xpathStr = "/list[@id='" + listIdScxml + "']/items/item";
    pugi::xpath_node_set allNodes = _listData.select_nodes(xpathStr.c_str());

    // remove info and navi
    pugi::xml_document docRet;
    pugi::xpath_node_set::iterator it = allNodes.begin();
    while (it != allNodes.end()) {
        docRet.append_copy(it->node());
        ++it;
    }
    if (!isInfo) {
        // remove show="information"
        pugi::xpath_node_set infoNodeSet = docRet.select_nodes("//item[@show='information']");
        pugi::xpath_node_set::iterator it = infoNodeSet.begin();
        if (it != infoNodeSet.end()) {
            docRet.remove_child(it->node());
        }
    }

    if (!isNavi) {
        // remove show="navi"
        pugi::xpath_node_set naviNodeSet = docRet.select_nodes("//item[@show='navi']");
        pugi::xpath_node_set::iterator it = naviNodeSet.begin();
        if (it != naviNodeSet.end()) {
            docRet.remove_child(it->node());
        }
    }
    pugi::xpath_node_set itemXpathNodes = docRet.select_nodes("//item");
    // end

    int count = itemXpathNodes.size();
    VR_LOG("pageSize = %d, startIndex = %d, all count = %d", pageSizeScxml, startIndexScxml, count);
    if (count < 1) {
        VR_ERROR("when receive the paper operaction of display,can't find listid=%s in DE\n", listIdScxml.c_str());
    }
    else {
        pugi::xml_node listNodeScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node();
        // pugi::xml_node listNodeScxml = hintsOrSelectListsNodeScxml.child("list");
        listNodeScxml.remove_child("items");
        pugi::xml_node itemsNodeScxml = listNodeScxml.append_child("items");
        for (int i = 0; i < pageSizeScxml; ++i) {
            int index = startIndexScxml + i;
            if (index < itemXpathNodes.size()) {
                pugi::xml_node nodeIt = itemXpathNodes[index].node();
                itemsNodeScxml.append_copy(nodeIt);
            }
            else {
                VR_LOG("pagesize=%d", pageSizeScxml);
                VR_LOG("startIndex=%d", startIndexScxml);
                VR_LOG("when add items to dispaly list, item has reached end!");
                break;
            }
        }
    }
}

void VR_DataProcessor::addItemsToDispaly(pugi::xml_node& hintsOrSelectListsNodeScxml)
{
    VR_LOGD_FUNC();
    // get the params (hints) and get some data into _listDataForUI
    std::string listIdScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().attribute("id").as_string();
    std::string pageSizeStrScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().child("header").child("pageSize").child_value();
    int pageSizeScxml = atoi(pageSizeStrScxml.c_str());
    std::string startIndexStrScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node().child("header").child("startIndex").child_value();
    int startIndexScxml = atoi(startIndexStrScxml.c_str());
    std::string xpathStr = "/list[@id='" + listIdScxml + "']/items/item";
    pugi::xpath_node_set itemXpathNodes = _listData.select_nodes(xpathStr.c_str());
    int count = itemXpathNodes.size();
    VR_LOGD("pageSize = %d, startIndex = %d, all count = %d", pageSizeScxml, startIndexScxml, count);
    if (count < 1) {
        VR_ERROR("when receive the paper operaction of display,can't find listid=%s in DE\n", listIdScxml.c_str());
    }
    else {
        pugi::xml_node listNodeScxml = hintsOrSelectListsNodeScxml.select_node("//list[@id]").node(); 
        // pugi::xml_node listNodeScxml = hintsOrSelectListsNodeScxml.child("list");
        listNodeScxml.remove_child("items");
        pugi::xml_node itemsNodeScxml = listNodeScxml.append_child("items");
        for (int i = 0; i < pageSizeScxml; ++i) {
            int index = startIndexScxml + i;
            if (index < itemXpathNodes.size()) {
                pugi::xml_node nodeIt = itemXpathNodes[index].node();
                itemsNodeScxml.append_copy(nodeIt);
            }
            else {
                VR_LOG("pagesize=%d", pageSizeScxml);
                VR_LOG("startIndex=%d", startIndexScxml);
                VR_LOG("when add items to dispaly list, item has reached end!");
                break;
            }
        }
    }
}

std::string VR_DataProcessor::getHintsData(const std::string& agentName, int pageSize, bool isNavi, bool isInfo)
{
    VR_LOGD_FUNC();
    std::string listId = "list_" + agentName;
    std::string xpathStr = "//list[@id='" + listId + "']//item";

    pugi::xpath_node_set allNodes = _listData.select_nodes(xpathStr.c_str());
    // remove info and navi
    pugi::xml_document docRet;
    pugi::xpath_node_set::iterator it = allNodes.begin();
    while (it != allNodes.end()) {
        docRet.append_copy(it->node());
        ++it;
    }
    if (!isInfo) {
        // remove show="information"
        pugi::xpath_node_set infoNodeSet = docRet.select_nodes("//item[@show='information']");
        pugi::xpath_node_set::iterator it = infoNodeSet.begin();
        if (it != infoNodeSet.end()) {
            docRet.remove_child(it->node());
        }
    }

    if (!isNavi) {
        // remove show="navi"
        pugi::xpath_node_set naviNodeSet = docRet.select_nodes("//item[@show='navi']");
        pugi::xpath_node_set::iterator it = naviNodeSet.begin();
        if (it != naviNodeSet.end()) {
            docRet.remove_child(it->node());
        }
    }
    pugi::xpath_node_set nodes = docRet.select_nodes("//item");
    // end

    int nodeCount = nodes.size();
    if (nodeCount <= 0) {
        VR_ERROR("can't find hints with xpath=%s", xpathStr.c_str());
        return "";
    }

    if (pageSize <= 0) {
        pageSize = 5;
        VR_LOG("the pageSize=%d,so we modify it to 5", pageSize);
    }

    pugi::xml_document doc;
    doc.load_string(HINTSDISPLAY);
    pugi::xml_node hintItems = doc.select_node("//items").node();
    for (unsigned int i = 0; i < pageSize; ++i) {
        if (i < nodeCount) {
            hintItems.append_copy(nodes[i].node());
        }
        else {
            break;
        }
    }

    pugi::xml_node countNode = doc.select_node("//count").node();
    countNode.text().set(nodeCount);

    pugi::xml_node pageSizeNode = doc.select_node("//pageSize").node();
    pageSizeNode.text().set(pageSize);
    // pugi::xml_node agentNode = doc.select_node("//agent").node();
    // agentNode.first_child().set_value(agentName.c_str());
    // pugi::xml_node contentNode = doc.select_node("//content").node();
    // std::string contentStr = agentName + "_idle";
    // contentNode.first_child().set_value(contentStr.c_str());

    std::stringstream oss;
    doc.print(oss);
    std::string resultStr = oss.str();
    return resultStr;
}

pugi::xml_node VR_DataProcessor::getNode(const std::string& listId)
{
    std::string xpathStr = "//list[@id='" + listId + "']";
    pugi::xml_node listNode = _listData.select_node(xpathStr.c_str()).node();
    return listNode;
}

void VR_DataProcessor::clearListDataFromDM()
{
    VR_LOGD_FUNC();
    pugi::xml_node itNode = _listData.first_child();
    pugi::xml_node tmpNode;
    while (itNode != NULL) {
        tmpNode = itNode.next_sibling();
        if (std::string(itNode.attribute("type").as_string()) != "hint") {
            _listData.remove_child(itNode);
        }
        itNode = tmpNode;
    }
}

void VR_DataProcessor::clearHintsList()
{
    VR_LOGD_FUNC();
    pugi::xml_node itNode = _listData.first_child();
    pugi::xml_node tmpNode;
    while (itNode != NULL) {
        tmpNode = itNode.next_sibling();
        if (std::string(itNode.attribute("type").as_string()) == "hint") {
            _listData.remove_child(itNode);
        }
        itNode = tmpNode;
    }
}

void VR_DataProcessor::initHinis(const std::string& preListName, const std::string& fileFullPath)
{
    VR_LOGD_FUNC();
    // read hints
    pugi::xml_document hitsDoc;
    hitsDoc.load_file(fileFullPath.c_str());
    pugi::xpath_node_set listNode = hitsDoc.select_nodes("//list");
    pugi::xpath_node_set::iterator it = listNode.begin();
    while (it != listNode.end()) {
        pugi::xml_document doc;
        doc.load_string("<list></list>");

        std::string agentName = it->node().attribute("agent").as_string();
        doc.child("list").append_attribute("id").set_value(std::string(preListName + agentName).c_str());
        doc.child("list").append_attribute("type").set_value("hint");
        it->node().set_name("items");
        doc.child("list").append_copy(it->node());
        _listData.append_copy(doc.child("list"));
        ++it;
        // std::string agentName = it->node().attribute("agent").as_string();
        // it->node().append_attribute("id").set_value(std::string(preListName + agentName).c_str());
        // it->node().append_attribute("type").set_value("hint");
        // _listData.append_copy(it->node());
        // ++it;
    }
}

std::string VR_DataProcessor::getHintCap(const std::string& preListName, const std::string& agentName)
{
    VR_LOGD_FUNC();
    std::string listId = preListName + agentName;
    pugi::xml_document docDisplay;
    docDisplay.load_string(DISPLAYNODE);
    pugi::xml_node listNode = docDisplay.select_node("//list").node();
    listNode.append_attribute("id").set_value(listId.c_str());

    // count the item in list;
    std::string xpathStr = "//list[@id='" + listId + "']//item";
    pugi::xpath_node_set itemNodes = _listData.select_nodes(xpathStr.c_str());
    int itemCount = itemNodes.size();
    char buf[64] = {};
    snprintf(buf, sizeof(buf), "%d", itemCount);
    pugi::xml_node countNode = listNode.select_node("//count").node();
    countNode.first_child().set_value(buf);
    std::stringstream oss;
    listNode.parent().print(oss);
    std::string listStr = oss.str();
    return listStr;
}

bool VR_DataProcessor::updateListByWebSearch(const std::string& asrWebSearchResult)
{
    VR_LOGD_FUNC();
    pugi::xml_document doc;
    doc.load_string(asrWebSearchResult.c_str());
    std::string xpathStr = "//list[@id='webSearch']";
    pugi::xml_node listNode = doc.select_node(xpathStr.c_str()).node();    
    pugi::xml_node oldListNode = _listData.select_node(xpathStr.c_str()).node();
    if (NULL != oldListNode) {
        _listData.remove_child(oldListNode);
    }
    _listData.append_copy(listNode);
    return true;
}

std::string VR_DataProcessor::getWebSearchResult()
{
    VR_LOGD_FUNC();
    // get the params (hints) and get some data into _listDataForUI
    std::string result;
    std::string xpathStr = "//list[@id='webSearch']";
    pugi::xpath_node_set itemXpathNodes = _listData.select_nodes(xpathStr.c_str());

    int count = itemXpathNodes.size();
    VR_LOG("all count = %d", count);
    if (count < 1) {
        VR_ERROR("when receive the paper operaction of display,can't find listid=webSearch in DE\n");
    }
    else {
        // add 20160311 remove more than 5 item records
        pugi::xml_document tmpDoc;
        pugi::xml_node tmpListNode = itemXpathNodes[0].node();
        tmpDoc.append_copy(tmpListNode);

        pugi::xpath_node_set itemNodeSet = tmpDoc.select_nodes("//list[@id]/items/item");
        int nCount = itemNodeSet.size();
        int nRemainCount = nCount;
        int nIndex = 5;
        while (nIndex < nCount) {
            itemNodeSet[nIndex].node().parent().remove_child(itemNodeSet[nIndex].node());
            --nRemainCount;
            ++nIndex;
        }

        pugi::xml_node listNode = tmpDoc.select_node(xpathStr.c_str()).node();
        // end add 20160311
        // pugi::xml_node listNode = itemXpathNodes[0].node();
        std::stringstream oss;
        listNode.print(oss);
        result = oss.str();
    }

    return result;
}

/* EOF */
