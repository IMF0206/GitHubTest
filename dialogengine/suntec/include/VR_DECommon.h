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
 * @file VR_DataProcessor.h
 * @brief process list data in DE
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_DECOMMON_H_
#define _VR_DECOMMON_H_


#include "config.h"
#include "VR_DECommonIF.h"
#include "pugixml.hpp"
#include <iostream>
#include <sstream>
#include <ctime>
#include "MEM_list.h"
#include "MEM_map.h"
#include "VR_ConfigureIF.h"
/**
 * @brief The _VR_DECOMMON_H_ class
 *
 * Some common function in this class
 */
class VR_DECommon : public VR_DECommonIF
{
public:
    VR_DECommon();
    virtual ~VR_DECommon()
    {
    }

    virtual void init(VR_ConfigureIF* configure);
    virtual bool isGetFromData();
    virtual std::string getProductCountry();
    virtual VR_DECommonIF::DE_Country getCountryType(); // REVIEW: product type
    virtual bool getAsrSupportLanguage(const std::string& lang, std::string& outLanguage);
    virtual bool getVROverBeep();
    virtual void setNaviModel(bool is);
    virtual bool getNaviModel();
    virtual bool getHybridVRFlag();
    virtual bool isSupportVR();
    virtual std::string getVRLanguage();
    virtual void notifyVRLanguageChange();
    virtual const std::string getDataPath();
    virtual int getTargetNice(const std::string& thName);
    virtual const std::string& getHeadUnitType() const;

    static VoiceList<std::string>::type m_listGENCountry;
    static VoiceList<std::string>::type m_listEUCountry;
    static VoiceList<std::string>::type m_listCNCountry;
    static VoiceList<std::string>::type m_listVoiceTagCountry;

protected:
    std::string m_dataPath;
    std::string m_country;
    bool m_beepBargein;
    bool m_naviModel;
    std::string m_language;
    VR_ConfigureIF* m_pConfigure;
    VoiceMap<std::string, int>::type m_mapNices;
    VoiceMap<std::string, int>::type m_mapConfidences;

private:
    void readExistDirLanguageName(VoiceList<std::string>::type& languageNameList);
    void loadConfig();

};

#endif // _VR_DECOMMON_H_
/* EOF */
