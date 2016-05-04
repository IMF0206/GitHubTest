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
 * @file VR_AsrRequestor.h
 * @brief inteface for interpeter or asr engine to perfer command
 *
 *
 * @attention used for C++ only.
 */

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __VR_INTENTION_PARSER_H__
#define __VR_INTENTION_PARSER_H__

#include "pugixml.hpp"
class VR_DataAccessorManager;
class VR_DECommonIF;

/**
 * @brief The VR_IntentionParser class
 *
 * VR_IntentionParser
 */

class VR_IntentionParser
{
public:
    VR_IntentionParser(VR_DECommonIF* pDECommonIF);
    virtual ~VR_IntentionParser();
    void preProcAsrResult(pugi::xml_node asrNode);
    bool filterIntention(pugi::xml_node asrNode, bool isNaviFullData, bool isMusicFilter);
    void changePOINameByPOIId(pugi::xml_node asrNode, VR_DataAccessorManager* pAccessorManager);

    // for CN
    void intelligentConventRadioCN(pugi::xml_node asrNode);

protected:
    VR_DECommonIF* m_pDECommonIF;

private:
    bool filterNaviCMN(pugi::xml_node asrNode);
    bool filterRadioFreqGen(pugi::xml_node asrNode);
    bool checkRemoveNode(pugi::xml_node asrNode, int& count, const std::string& selectXpathStr, const std::string& nodeName, double min, double max, double step);
    void mergePhoneContactType(pugi::xml_node asrNode);
    void removeSameNode(pugi::xml_node asrNode, const std::string& contactType, int id);
    bool filterValueNull(pugi::xml_node asrNode);
    bool filterMusicCMN(pugi::xml_node asrNode);
    // void replaceWellNum(pugi::xml_node asrNode);
    void replaceWellNumPOI(pugi::xml_node asrNode);
    bool removeOtherItemsWithOutWell(pugi::xml_node cmnNode);
    void removeNotSameConfidenceItem(pugi::xml_node cmnNode);
    void replaceWellInSentence(pugi::xml_node cmnNode);
    void replaceWellInPoiNode(pugi::xml_node cmnNode);

    void intelligentConventRadio(pugi::xml_node asrNode);
    bool compareItem(pugi::xml_node curItem, pugi::xml_node otherItem);
    void removeCurItemFollowSameItem(pugi::xml_node curItem);
    void removeSameItemInNBest(pugi::xml_node asrNode);
    void replaceSentenceValueCMN2034(pugi::xml_node asrNode);
};

#endif
/* EOF */
