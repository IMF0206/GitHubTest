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

#include "VR_DataAccessorNaviIF.h"
#include "VR_Log.h"
#include "MEM_list.h"

#include <sstream>
#include <fstream>

const char* VR_OPERATION_BUILD_POINT_TYPE   = "_buildPointType";

#define VR_MSG_RESPONSE_NODENAME                            "data"
#define VR_MSG_RESPONSE_POINT_NEED_SELECT_NODE              "needSelect"
#define VR_MSG_RESPONSE_POINT_DEFAULT_GUIDE_NODE            "defaultGuidePoint"
#define VR_MSG_RESPONSE_POINT_SELECT_GUIDE_NODE             "selectGuidePoint"
#define VR_MSG_RESPONSE_POINT_DISPLAY_GUIDE_TYPE_NODE       "displayGuideType"
#define VR_MSG_RESPONSE_POINT_LONGITUDE_NODE                "longitude"
#define VR_MSG_RESPONSE_POINT_LATITUDE_NODE                 "latitude"
#define VR_MSG_RESPONSE_POINT_LINK_ID_NODE                  "linkId"

bool VR_DataAccessorNaviIF::buildPointType(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node needSelectNode = responseNode.append_child(VR_MSG_RESPONSE_POINT_NEED_SELECT_NODE);
    needSelectNode.text().set("false");
    responseNode.append_child(VR_MSG_RESPONSE_POINT_DEFAULT_GUIDE_NODE);
    responseNode.append_child(VR_MSG_RESPONSE_POINT_SELECT_GUIDE_NODE);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node guidePointNode = msgDoc.select_node((std::string("//") + "guidePoint").c_str()).node();
    pugi::xml_node resultNode = msgDoc.select_node((std::string("//") + "result").c_str()).node();
    if (resultNode.empty() || guidePointNode.empty()) {
        return false;
    }
    pugi::xml_node listNode = resultNode.child("list");
    if (listNode.empty()) {
        return false;
    }
    pugi::xml_node headerNode = listNode.child("header");
    if (headerNode.empty()) {
        return false;
    }
    pugi::xml_node countNode = headerNode.child("count");
    if (countNode.empty()) {
        return false;
    }
    int count = atoi(countNode.text().as_string());
    if (0 < count) {
        responseNode.remove_child(VR_MSG_RESPONSE_POINT_DEFAULT_GUIDE_NODE);
        responseNode.insert_copy_after(guidePointNode, needSelectNode).set_name(VR_MSG_RESPONSE_POINT_DEFAULT_GUIDE_NODE);
        if (2 == count) {
            needSelectNode.text().set("true");
            responseNode.remove_child(VR_MSG_RESPONSE_POINT_SELECT_GUIDE_NODE);
            responseNode.append_copy(guidePointNode).set_name(VR_MSG_RESPONSE_POINT_SELECT_GUIDE_NODE);
        }
    }
    else {
        return false;
    }

    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    pugi::xml_node itemsNode = listNode.child("items");
    if (itemsNode.empty()) {
        return false;
    }

    VoiceList<pugi::xml_node>::type pointNodeList;
    pugi::xml_node itemNode = itemsNode.first_child();
    while (!itemNode.empty()) {
        pugi::xml_node pointNode = itemNode.select_node("guidePoint").node();
        if (!pointNode.empty()) {
            if (isGuidePointValid(pointNode)) {
                std::string defaultValue = pointNode.child(VR_MSG_RESPONSE_POINT_DISPLAY_GUIDE_TYPE_NODE).text().as_string();
                if ("DEFAULT" == defaultValue) {
                    pointNodeList.push_front(pointNode);
                }
                else {
                    pointNodeList.push_back(pointNode);
                }
            }
        }
        itemNode = itemNode.next_sibling();
    }

    if (pointNodeList.empty()) {
        return false;
    }
    else {
        pugi::xml_node valueNode = pointNodeList.front();
        pugi::xml_node defaultNode = responseNode.child(VR_MSG_RESPONSE_POINT_DEFAULT_GUIDE_NODE);
        changeValueByResult(defaultNode, valueNode);

        pointNodeList.pop_front();
        if (!pointNodeList.empty()) {
            valueNode = pointNodeList.front();
            pugi::xml_node selectNode = responseNode.child(VR_MSG_RESPONSE_POINT_SELECT_GUIDE_NODE);
            changeValueByResult(selectNode, valueNode);
        }
    }

    responseNode.print(oss);
    response = oss.str();

    return true;
}

bool VR_DataAccessorNaviIF::isGuidePointValid(pugi::xml_node &guidePointNode)
{
    bool result = !guidePointNode.child(VR_MSG_RESPONSE_POINT_LONGITUDE_NODE).text().empty()
        && !guidePointNode.child(VR_MSG_RESPONSE_POINT_LATITUDE_NODE).text().empty()
        && !guidePointNode.child(VR_MSG_RESPONSE_POINT_DISPLAY_GUIDE_TYPE_NODE).empty();
    std::string longitude(guidePointNode.child(VR_MSG_RESPONSE_POINT_LONGITUDE_NODE).text().as_string());
    std::string latitude(guidePointNode.child(VR_MSG_RESPONSE_POINT_LATITUDE_NODE).text().as_string());
    result = result && (longitude != "''") && (latitude != "''");
    return result;
}

void VR_DataAccessorNaviIF::changeValueByResult(pugi::xml_node &originNode, pugi::xml_node &resultNode)
{
    originNode.remove_child(VR_MSG_RESPONSE_POINT_DISPLAY_GUIDE_TYPE_NODE);
    originNode.remove_child(VR_MSG_RESPONSE_POINT_LONGITUDE_NODE);
    originNode.remove_child(VR_MSG_RESPONSE_POINT_LATITUDE_NODE);
    originNode.remove_child(VR_MSG_RESPONSE_POINT_LINK_ID_NODE);

    originNode.prepend_copy(resultNode.child(VR_MSG_RESPONSE_POINT_LINK_ID_NODE));
    originNode.prepend_copy(resultNode.child(VR_MSG_RESPONSE_POINT_LATITUDE_NODE));
    originNode.prepend_copy(resultNode.child(VR_MSG_RESPONSE_POINT_LONGITUDE_NODE));
    originNode.prepend_copy(resultNode.child(VR_MSG_RESPONSE_POINT_DISPLAY_GUIDE_TYPE_NODE));
}

/* EOF */
