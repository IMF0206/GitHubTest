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

#include "VR_DataAccessorNaviCN.h"
#include "VR_Log.h"
#include "MEM_list.h"

#include <pugixml.hpp>
#include <sstream>
#include <string>


#define VR_OPERATION_ADDRESS_MAPPING    "_addressMappingCN"
#define VR_OPERATION_BUILD_ADDRESS      "_buildAddressCN"

#define VR_MSG_NAVI_LIST             "list"
#define VR_MSG_NAVI_ITEMS            "items"
#define VR_MSG_NAVI_ITEM             "item"
#define VR_MSG_NAVI_ADDRESS_ID       "addressId"
#define VR_MSG_NAVI_TYPE_STATE       "STATE"
#define VR_MSG_NAVI_TYPE_CITY        "CITY"
#define VR_MSG_NAVI_TYPE_DISTRICT    "DISTRICT"
#define VR_MSG_NAVI_TYPE_STREET      "STREET"
#define VR_MSG_NAVI_TYPE_NUMBER      "NUMBER"
#define VR_MSG_NAVI_ZONE_ID          "zoneId"
#define VR_MSG_NAVI_CITY_ID          "cityId"
#define VR_MSG_NAVI_DISTRICT_ID      "districtId"
#define VR_MSG_NAVI_STREET_ID        "streetId"
#define VR_MSG_NAVI_ZONE             "zone"
#define VR_MSG_NAVI_CITY             "city"
#define VR_MSG_NAVI_DISTRICT         "district"
#define VR_MSG_NAVI_STREET           "street"
#define VR_MSG_NAVI_STREET_BODY      "streetBody"
#define VR_MSG_NAVI_HOUSE_NUMBER     "houseNumber"

#define VR_MSG_BUILD_ADDRESS_BUILD_TYPE         "buildType"
#define VR_MSG_BUILD_ADDRESS_ADDRESS_INFO       "addressInfo"
#define VR_MSG_BUILD_ADDRESS_ASR_RESULT         "asrResult"

#define VR_MSG_RESPONSE_NODENAME                            "data"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE                "mappingType"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_FULL           "FULL"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STREET    "TILL_STREET"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_DISTRICT  "TILL_DISTRICT"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_CITY      "TILL_CITY"
#define VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STATE     "TILL_STATE"
#define VR_MSG_RESPONSE_ADDRESS_LIST                        "list"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ID                     "id"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ID_PREFIX              "list_"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER                 "header"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_PAGESIZE        "pageSize"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_STARTINDEX      "startIndex"
#define VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT           "count"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS                  "items"
#define VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS_ITEM             "item"

VR_DataAccessorNaviCN::VR_DataAccessorNaviCN()
{
    m_operationMap[VR_OPERATION_ADDRESS_MAPPING] = NaviOperation::AddressMapping;
    m_operationMap[VR_OPERATION_BUILD_ADDRESS] = NaviOperation::BuildAddress;
    m_operationMap[VR_OPERATION_BUILD_POINT_TYPE] = NaviOperation::BuildPointType;
}

VR_DataAccessorNaviCN::~VR_DataAccessorNaviCN()
{
}

bool VR_DataAccessorNaviCN::getInfo(const std::string &operation, const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();
    VR_LOGD("Operation [%s]\nreqMsg [%s]", operation.c_str(), reqMsg.c_str());

    NaviOperation op = NaviOperation::None;
    if (m_operationMap.end() != m_operationMap.find(operation)) {
        op = m_operationMap[operation];
    }
    switch (op) {
    case NaviOperation::AddressMapping:
        {
            return addressMapping(reqMsg, response);
        }
    case NaviOperation::BuildAddress:
        {
            return buildAddress(reqMsg, response);
        }
    case NaviOperation::BuildPointType:
        {
            return buildPointType(reqMsg, response);
        }
    default:
        {
            return false; // can not find the operation
        }
    }
}

bool VR_DataAccessorNaviCN::isOperationHandled(const std::string &operation)
{
    return (m_operationMap.end() != m_operationMap.find(operation));
}

// void VR_DataAccessorNaviCN::requestService(const std::string &agent, const std::string &reqName)
// {

// }

bool VR_DataAccessorNaviCN::addressMapping(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE);
    responseNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST).append_attribute(VR_MSG_RESPONSE_ADDRESS_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_ADDRESS_LIST_ID_PREFIX) + VR_OPERATION_ADDRESS_MAPPING).c_str());
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    // set response data
    pugi::xml_document templateNodeDoc;
    templateNodeDoc.load_string("");
    pugi::xml_node itemNodeTemplate = templateNodeDoc.append_child(VR_MSG_NAVI_ITEM);
    pugi::xml_node addressIDNode = itemNodeTemplate.append_child(VR_MSG_NAVI_ADDRESS_ID);
    addressIDNode.append_child(VR_MSG_NAVI_ZONE_ID);
    addressIDNode.append_child(VR_MSG_NAVI_CITY_ID);
    addressIDNode.append_child(VR_MSG_NAVI_STREET_ID);
    addressIDNode.append_child(VR_MSG_NAVI_ZONE);
    addressIDNode.append_child(VR_MSG_NAVI_CITY);
    addressIDNode.append_child(VR_MSG_NAVI_DISTRICT);
    addressIDNode.append_child(VR_MSG_NAVI_STREET);
    addressIDNode.append_child(VR_MSG_NAVI_STREET_BODY);
    addressIDNode.append_child(VR_MSG_NAVI_HOUSE_NUMBER);

    itemsNode.append_copy(itemNodeTemplate);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    pugi::xml_node listNode = msgDoc.select_node((std::string("//") + VR_MSG_NAVI_ITEMS).c_str()).node();
    pugi::xml_node firstItem;
    if (!listNode.empty()) {
        firstItem = listNode.first_child();
        if (firstItem.empty()) {
            VR_ERROR("asrResult list is empty");
            return false;
        }
    }

    std::string mappingType;
    int itemCount = getAddressMappingResult(firstItem, itemsNode, mappingType);

    if (0 == itemCount) {
        VR_ERROR("zone name is empty.");
        return false;
    }

    char buffer[5] = {};
    snprintf(buffer, 5, "%d", itemCount);
    headerNode.child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set(buffer);

    responseNode.child(VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE).text().set(mappingType.c_str());
    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

bool VR_DataAccessorNaviCN::buildAddress(const std::string &reqMsg, std::string &response)
{
    VR_LOGD_FUNC();

    // create response message
    pugi::xml_document responseDoc;
    responseDoc.load_string("");
    responseDoc.append_child(VR_MSG_RESPONSE_NODENAME);
    pugi::xml_node responseNode = responseDoc.child(VR_MSG_RESPONSE_NODENAME);
    responseNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST).append_attribute(VR_MSG_RESPONSE_ADDRESS_LIST_ID).set_value((std::string(VR_MSG_RESPONSE_ADDRESS_LIST_ID_PREFIX) + VR_OPERATION_BUILD_ADDRESS).c_str());
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).append_child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    pugi::xml_node headerNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER);
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_PAGESIZE).text().set("5");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_STARTINDEX).text().set("0");
    headerNode.append_child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set("0");

    pugi::xml_node itemsNode = responseNode.child(VR_MSG_RESPONSE_ADDRESS_LIST).child(VR_MSG_RESPONSE_ADDRESS_LIST_ITEMS);

    // process request message
    VR_LOGD("reqMsg [%s]", reqMsg.c_str());
    pugi::xml_document msgDoc;
    msgDoc.load_string(reqMsg.c_str());
    std::string buildType = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_BUILD_TYPE).c_str()).node().text().as_string();
    pugi::xml_node addressInfoNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_ADDRESS_INFO).c_str()).node();
    pugi::xml_node asrResuldNode = msgDoc.select_node((std::string("//") + VR_MSG_BUILD_ADDRESS_ASR_RESULT).c_str()).node();

    // set response data
    pugi::xml_document templateNodeDoc;
    templateNodeDoc.load_string("");
    pugi::xml_node itemNodeTemplate = templateNodeDoc.append_child(VR_MSG_NAVI_ITEM);
    pugi::xml_node addressIDNode = itemNodeTemplate.append_child(VR_MSG_NAVI_ADDRESS_ID);
    addressIDNode.append_child(VR_MSG_NAVI_ZONE_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_ZONE_ID).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_CITY_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_CITY_ID).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_STREET_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_STREET_ID).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_DISTRICT_ID).text().set(addressInfoNode.child(VR_MSG_NAVI_DISTRICT_ID).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_ZONE).text().set(addressInfoNode.child(VR_MSG_NAVI_ZONE).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_CITY).text().set(addressInfoNode.child(VR_MSG_NAVI_CITY).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_DISTRICT).text().set(addressInfoNode.child(VR_MSG_NAVI_DISTRICT).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_STREET).text().set(addressInfoNode.child(VR_MSG_NAVI_STREET).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_STREET_BODY).text().set(addressInfoNode.child(VR_MSG_NAVI_STREET_BODY).text().as_string());
    addressIDNode.append_child(VR_MSG_NAVI_HOUSE_NUMBER).text().set(addressInfoNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().as_string());

    // create response when error occur
    pugi::xml_node tempAddresssIDItemNode = itemsNode.append_copy(itemNodeTemplate);

    std::ostringstream oss;
    responseNode.print(oss);
    response = oss.str();
    oss.str("");

    if (asrResuldNode.first_child().empty()) {
        VR_ERROR("asrResult list is empty, no voiceid input");
        return false;
    }

    asrResuldNode.print(oss);
    pugi::xml_document asrResuldNodeDoc;
    asrResuldNodeDoc.load_string(oss.str().c_str());
    oss.str("");

    std::string typeNodeName;
    std::string typeNodeNameEx;
    if (VR_MSG_NAVI_TYPE_STATE == buildType) {
        typeNodeName = VR_MSG_NAVI_ZONE;
    }
    else if (VR_MSG_NAVI_TYPE_CITY == buildType) {
        typeNodeName = VR_MSG_NAVI_CITY;
    }
    else if (VR_MSG_NAVI_TYPE_DISTRICT == buildType) {
        typeNodeName = VR_MSG_NAVI_DISTRICT;
    }
    else if (VR_MSG_NAVI_TYPE_STREET == buildType) {
        typeNodeName = VR_MSG_NAVI_STREET;
        typeNodeNameEx = VR_MSG_NAVI_STREET_BODY;
    }
    else if (VR_MSG_NAVI_TYPE_NUMBER == buildType) {
        typeNodeName = VR_MSG_NAVI_HOUSE_NUMBER;
        itemNodeTemplate.remove_child(VR_MSG_NAVI_ADDRESS_ID);
        itemNodeTemplate.append_copy(addressInfoNode);
    }
    else {
        VR_ERROR("unhandle buildType");
        return false;
    }

    VoiceList<std::string>::type nameList;
    VoiceList<std::string>::type nameListEx;
    pugi::xpath_node_set nameNodes = asrResuldNodeDoc.select_nodes((std::string("//") + typeNodeName).c_str());
    for (pugi::xpath_node_set::iterator it = nameNodes.begin(); it != nameNodes.end(); ++it) {
        std::string name = it->node().text().as_string();
        if (!name.empty()) {
            nameList.push_back(name);
        }
    }
    if (!typeNodeNameEx.empty()) {
        pugi::xpath_node_set nameExNodes = asrResuldNodeDoc.select_nodes((std::string("//") + typeNodeNameEx).c_str());
        if (nameList.size() != nameExNodes.size()) {
            VR_ERROR("streetBody error");
            return false;
        }
        VoiceList<std::string>::iterator nameIt = nameList.begin();
        for (pugi::xpath_node_set::iterator it = nameExNodes.begin(); it != nameExNodes.end(); ++it) {
            std::string name = it->node().text().as_string();
            if (!name.empty()) {
                nameListEx.push_back(name);
                ++nameIt;
            }
            else {
                VR_ERROR("ASR result : street [%s] has no streed body", nameIt->c_str());
                nameIt = nameList.erase(nameIt);
            }
        }
    }

    itemsNode.remove_child(tempAddresssIDItemNode);

    VoiceList<std::string>::iterator it = nameList.begin();
    VoiceList<std::string>::iterator it1 = nameListEx.begin();
    while (it != nameList.end()) {
        pugi::xml_node itemNode = itemsNode.append_copy(itemNodeTemplate);
        itemNode.first_child().child(typeNodeName.c_str()).text().set(it->c_str());
        ++it;

        if (it1 != nameListEx.end()) {
            itemNode.first_child().child(typeNodeNameEx.c_str()).text().set(it1->c_str());
            ++it1;
        }
    }

    char buffer[5] = {};
    snprintf(buffer, 5, "%d", nameList.size());
    headerNode.child(VR_MSG_RESPONSE_ADDRESS_LIST_HEADER_COUNT).text().set(buffer);

    responseNode.print(oss);
    response = oss.str();
    VR_LOGD("response msg is [%s]", response.c_str());

    return true;
}

int VR_DataAccessorNaviCN::getAddressMappingResult(pugi::xml_node &firstItem, pugi::xml_node &itemsNode, std::string &mappingType)
{
    int count = 0;
    VR_MappingTypeCN firstResultType = VR_MappingTypeCN::EMPTY;
    pugi::xml_node itemNodeTemplate = itemsNode.first_child();
    for (pugi::xml_node itemNode = firstItem; !itemNode.empty(); itemNode = itemNode.next_sibling()) {
        pugi::xml_node itemAddressNode = itemNode.first_child();
        std::string zone = itemAddressNode.child(VR_MSG_NAVI_ZONE).text().as_string();
        std::string city = itemAddressNode.child(VR_MSG_NAVI_CITY).text().as_string();
        std::string district = itemAddressNode.child(VR_MSG_NAVI_DISTRICT).text().as_string();
        std::string street = itemAddressNode.child(VR_MSG_NAVI_STREET).text().as_string();
        std::string streetBody = itemAddressNode.child(VR_MSG_NAVI_STREET_BODY).text().as_string();
        std::string houseNumber = itemAddressNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().as_string();

        VR_MappingTypeCN currentType = VR_MappingTypeCN::EMPTY;
        if (!zone.empty()) {
            currentType = VR_MappingTypeCN::TILL_STATE;
            if (!street.empty()) {
                currentType = VR_MappingTypeCN::TILL_STREET;
                if (!houseNumber.empty()) {
                    currentType = VR_MappingTypeCN::FULL;
                }
            }
            else {
                if (!district.empty()) {
                    currentType = VR_MappingTypeCN::TILL_DISTRICT;
                }
                else {
                    if (!city.empty()) {
                        currentType = VR_MappingTypeCN::TILL_CITY;
                    }
                }
            }
        }

        if (VR_MappingTypeCN::EMPTY == firstResultType) {
            if (VR_MappingTypeCN::EMPTY == currentType) {
                return count;
            }
            firstResultType = currentType;
        }

        if (firstResultType != currentType) {
            continue;
        }
        pugi::xml_node addressNode = itemsNode.append_copy(itemNodeTemplate).child(VR_MSG_NAVI_ADDRESS_ID);
        switch (firstResultType) {
        case VR_MappingTypeCN::FULL:
            addressNode.child(VR_MSG_NAVI_HOUSE_NUMBER).text().set(houseNumber.c_str());
        // need add stret info to addressNode
        case VR_MappingTypeCN::TILL_STREET:
            addressNode.child(VR_MSG_NAVI_STREET).text().set(street.c_str());
            addressNode.child(VR_MSG_NAVI_STREET_BODY).text().set(streetBody.c_str());
        // need add district info to addressNode
        case VR_MappingTypeCN::TILL_DISTRICT:
            addressNode.child(VR_MSG_NAVI_DISTRICT).text().set(district.c_str());
        // need add city info to addressNode
        case VR_MappingTypeCN::TILL_CITY:
            addressNode.child(VR_MSG_NAVI_CITY).text().set(city.c_str());
        // need add state info to addressNode
        case VR_MappingTypeCN::TILL_STATE:
            addressNode.child(VR_MSG_NAVI_ZONE).text().set(zone.c_str());
            break;
        default:
            break;
        }
        ++count;
    }

    // remove the itemNodeTemplate node
    itemsNode.remove_child(itemsNode.first_child());

    // get mappingType
    switch (firstResultType) {
    case VR_MappingTypeCN::FULL:
        mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_FULL;
        break;
    case VR_MappingTypeCN::TILL_STREET:
        mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STREET;
        break;
    case VR_MappingTypeCN::TILL_DISTRICT:
        mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_DISTRICT;
        break;
    case VR_MappingTypeCN::TILL_CITY:
        mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_CITY;
        break;
    case VR_MappingTypeCN::TILL_STATE:
        mappingType = VR_MSG_RESPONSE_ADDRESS_MAPPING_TYPE_TILL_STATE;
        break;
    default:
        break;
    }

    return count;
}

/* EOF */
