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

#include "stdafx.h"
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "VR_DECommon.h"
#include <boost/assign/list_of.hpp>


VoiceMap<std::string, std::string>::type mapAsrLanguageGen = boost::assign::map_list_of
        ("en-us",       "enu")
        ("fr-ca",       "frc")
        ("es-mx",       "spm")
        ("en-au",       "ena")
        ("zh-cn",       "mnc")
        ("zh-tw",       "mnt")
        ("zh-hk",       "cah")
        ("ko-kr",       "kok")
        ("pl",          "plp")
        ("fr",          "frf")
        ("nl",          "dun")
        ("en-gb",       "eng")
        ("en-id",       "eni")
        ("id",          "idi")
        ("el",          "grg")
        ("fi",          "fif")
        ("hi",          "hii")
        ("sv",          "sws")
        ("he",          "hei")
        ("da",          "dad")
        ("no",          "non")
        ("sk",          "sks")
        ("ar-ml",       "arg")
        ("ar-fm",       "arg")
        ("it",          "iti")
        ("hu",          "huh")
        ("pt-br",       "ptb")
        ("tr",          "trt")
        ("th",          "tht")
        ("de",          "ged")
        ("ru",          "rur")
        ("cs",          "czc")
        ("ro",          "ror")
        ("es",          "spe")
        ("pt",          "ptp")
        ("en-me",       "arg");



VR_DECommon::VR_DECommon()
    : m_country("HongkongMacau")
    , m_beepBargein(false)
    , m_pConfigure(NULL)
    , m_dataPath("")
    , m_naviModel(false)
    , m_language("en-us")
{
}

void VR_DECommon::init(VR_ConfigureIF* configure)
{
    VR_LOGD_FUNC();
    m_pConfigure = configure;
    m_dataPath = m_pConfigure->getDataPath();
    m_language = m_pConfigure->getVRLanguage();
    m_country = m_pConfigure->getVRRegion();

    // set beep
    if ((m_country == "South-Africa")
        || (m_country == "Thailand")
        || (m_country == "Southeast-Asia")
        || (m_country == "HongkongMacau")) {
        m_beepBargein = true;
    }
    else {
        m_beepBargein = false;
    }
    loadConfig();
    // set language
    notifyVRLanguageChange();

}

void VR_DECommon::loadConfig()
{
    VR_LOGD_FUNC();
    std::string path = getDataPath() + "dialogengine_configure.xml";
    VR_LOGD("load config %s", path.c_str());
    pugi::xml_document doc;
    doc.load_file(path.c_str());
    // load nice
    pugi::xpath_node_set thNices = doc.select_nodes("//nice");
    pugi::xpath_node_set::iterator it = thNices.begin();
    while (it != thNices.end()) {
        std::string name = it->node().attribute("id").as_string();
        int nice = it->node().text().as_int();
        m_mapNices[name] = nice;
        VR_LOGD("set %s nice %d", name.c_str(), nice);
        ++it;
    }

    // load confidence
    pugi::xpath_node_set confidences = doc.select_nodes("//confidence");
    pugi::xpath_node_set::iterator itConfidence = confidences.begin();
    while (itConfidence != confidences.end()) {
        std::string cmn = itConfidence->node().attribute("id").as_string();
        int confd = it->node().text().as_int();
        m_mapConfidences[cmn] = confd;
        ++itConfidence;
    }
}



int VR_DECommon::getTargetNice(const std::string& thName)
{
    VR_LOGD_FUNC();
    VoiceMap<std::string, int>::iterator it = m_mapNices.find(thName);
    if (it != m_mapNices.end()) {
        return it->second;
    }
    else {
        return INVALID;
    }
}

const std::string& VR_DECommon::getHeadUnitType() const
{
    VR_LOGD_FUNC();
    return m_pConfigure->getHeadUnitType();
}

const std::string VR_DECommon::getDataPath()
{
    return m_pConfigure->getDataPath();
}

std::string VR_DECommon::getVRLanguage()
{
    return m_language;
}

void VR_DECommon::notifyVRLanguageChange()
{
    VR_LOGD_FUNC();
    m_language = m_pConfigure->getVRLanguage();
    // REVIEW:
    VR_PRODUCT_TYPE product = m_pConfigure->getVRProduct();
    if (VR_PRODUCT_TYPE_T1 == product
        || VR_PRODUCT_TYPE_T2 == product
        || VR_PRODUCT_TYPE_L1 == product) {
        if (VR_DECommonIF::cn == getCountryType()) {
            m_language = "zh-cn";
        }
    }
    VR_LOGD("set language = %s", m_language.c_str());
}


bool VR_DECommon::isSupportVR()
{
    VR_PRODUCT_TYPE product = m_pConfigure->getVRProduct();
    VR_DECommonIF::DE_Country region = getCountryType();
    if (VR_PRODUCT_TYPE_T1 == product
        || VR_PRODUCT_TYPE_T2 == product
        || VR_PRODUCT_TYPE_L1 == product) {
        if (VR_DECommonIF::cn == region) {
            if ("zh-cn" != m_pConfigure->getVRLanguage()) {
                return false;
            }
        }
    }
    return true;
}

bool VR_DECommon::getVROverBeep()
{
    return m_beepBargein;
}


bool VR_DECommon::getAsrSupportLanguage(const std::string& lang, std::string& outLanguage)
{
    VR_LOGD_FUNC();
    VoiceMap<std::string, std::string>::iterator it = mapAsrLanguageGen.find(lang);
    if (it != mapAsrLanguageGen.end()) {
        outLanguage = it->second;
    }

    // check path = "/pdata/.VDATA/VDATA/VR/ASR/Model/Language_***" exist
    std::string fPath = m_dataPath + "../ASR/Model/Language_";

    std::string path = fPath + outLanguage;
    VR_LOGD("asr data path = %s", path.c_str());
    std::ifstream ofin(path.c_str());
    if (ofin) {
        RETURN(true);
    }
    // eng
    outLanguage = "eng";
    path = fPath + outLanguage;
    std::ifstream efin(path.c_str());
    if (efin) {
        RETURN(true);
    }
    // enu
    outLanguage = "enu";
    path = fPath + outLanguage;
    std::ifstream ufin(path.c_str());
    if (ufin) {
        RETURN(true);
    }

    VoiceList<std::string>::type languageList;
    readExistDirLanguageName(languageList);
    if (!languageList.empty()) {
        outLanguage = languageList.front();
        VR_LOGD("outLanguage = %s", outLanguage.c_str());
        RETURN(true);
    }

    RETURN(false);
}

void VR_DECommon::readExistDirLanguageName(VoiceList<std::string>::type & languageNameList)
{
    VR_LOGD_FUNC();
    std::string path = m_dataPath + "../ASR/Model/";
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    int n = 0;
    VR_LOGD("%s", path.c_str());
    dp = opendir(path.c_str());
    if (dp == NULL) {
        VR_ERROR("can't open %s", path.c_str());
        return;
    }
    dirp = readdir(dp);
    while ((dirp != NULL) && (n <= 50)) {
        n++;
        std::string dirName = dirp->d_name; // Language_enu
        if (dirName.size() > 9) {
            std::string languageName = dirName.substr(9);
            VR_LOGD("push language into list %s", languageName.c_str());
            languageNameList.push_back(languageName);
        }
        dirp = readdir(dp);
    }
    closedir(dp);
}


// REVIEW: check get scxml from system or vdata
bool VR_DECommon::isGetFromData()
{
    VR_LOGD_FUNC();
    bool isGetData = true;
    std::string verSystem;
    std::string verData;

    std::string versionSystem = "/system/etc/voicerecog/settings/.version";
    std::ifstream sysFin(versionSystem.c_str());
    if (sysFin) {
        std::getline(sysFin, verSystem);
        std::string versionData = m_dataPath + "settings/" + ".version";
        std::ifstream dataFin(versionData.c_str());
        if (!dataFin) {
            isGetData = false;
        }
        else {
            // check sys.ver > data.ver
            std::getline(dataFin, verData);
            if (verSystem > verData) {
                isGetData = false;
            }
        }
    }
    RETURN(isGetData);
}

std::string VR_DECommon::getProductCountry()
{
    return m_country;
}


VR_DECommonIF::DE_Country VR_DECommon::getCountryType()
{
    std::string country = getProductCountry();
    VoiceList<std::string>::iterator it1 = std::find(VR_DECommon::m_listGENCountry.begin(), VR_DECommon::m_listGENCountry.end(), country);
    if (it1 != VR_DECommon::m_listGENCountry.end()) {
        return VR_DECommonIF::DE_Country::gen;
    }

    VoiceList<std::string>::iterator it2 = std::find(VR_DECommon::m_listEUCountry.begin(), VR_DECommon::m_listEUCountry.end(), country);
    if (it2 != VR_DECommon::m_listEUCountry.end()) {
        return VR_DECommonIF::DE_Country::eu;
    }

    VoiceList<std::string>::iterator it3 = std::find(VR_DECommon::m_listCNCountry.begin(), VR_DECommon::m_listCNCountry.end(), country);
    if (it3 != VR_DECommon::m_listCNCountry.end()) {
        return VR_DECommonIF::DE_Country::cn;
    }

    VoiceList<std::string>::iterator it4 = std::find(VR_DECommon::m_listVoiceTagCountry.begin(), VR_DECommon::m_listVoiceTagCountry.end(), country);
    if (it4 != VR_DECommon::m_listVoiceTagCountry.end()) {
        return VR_DECommonIF::DE_Country::vt;
    }
    return VR_DECommonIF::DE_Country::gen;
}


void VR_DECommon::setNaviModel(bool is)
{
    m_naviModel = is;
}

bool VR_DECommon::getNaviModel()
{
    return m_naviModel;
}

bool VR_DECommon::getHybridVRFlag()
{
    return m_pConfigure->getHybridVRFlag();
}

VoiceList<std::string>::type VR_DECommon::m_listGENCountry = boost::assign::list_of
        ("Japan")
        ("North-Africa")
        ("South-Africa")
        ("Southeast-Asia")
        ("Thailand")
        ("South-Asia")
        ("HongkongMacau")
        ("SouthKorea");

VoiceList<std::string>::type VR_DECommon::m_listEUCountry = boost::assign::list_of
        ("European")
        ("Russia");




VoiceList<std::string>::type VR_DECommon::m_listCNCountry = boost::assign::list_of
        ("China");


VoiceList<std::string>::type VR_DECommon::m_listVoiceTagCountry = boost::assign::list_of
        ("America")
        ("Taiwan")
        ("South-America")
        ("Brazil")
        ("Middle-America")
        ("Middle-East")
        ("India");

/* EOF */
