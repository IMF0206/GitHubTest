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
 * @file VR_DECommonIF.h
 * @brief VR_DECommonIF
 *
 *
 * @attention used for C++ only.
 */
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#ifndef _VR_DECOMMONIF_H_
#define _VR_DECOMMONIF_H_



#include "pugixml.hpp"
#include <iostream>
#include <sstream>
#include <ctime>
#include "MEM_list.h"

class VR_ConfigureIF;

/**
 * @brief The _VR_DECOMMONIF_H_ class
 *
 * Some common function in this class
 */
class VR_DECommonIF
{
public:
    enum DE_Country
    {
        gen = 0,
        eu = 1,
        cn = 2,
        vt = 3
    };

    virtual ~VR_DECommonIF() {}
    virtual void init(VR_ConfigureIF* configure) = 0;
    virtual bool isGetFromData() = 0;
    virtual std::string getProductCountry() = 0;
    virtual DE_Country getCountryType() = 0;
    virtual bool getAsrSupportLanguage(const std::string& lang, std::string& outLanguage) = 0;
    virtual bool getVROverBeep() = 0;
    virtual void setNaviModel(bool is) = 0;
    virtual bool getNaviModel() = 0;
    virtual bool getHybridVRFlag() = 0;
    virtual bool isSupportVR() = 0;
    virtual std::string getVRLanguage() = 0;
    virtual void notifyVRLanguageChange() = 0;
    virtual const std::string getDataPath() = 0;
    virtual int getTargetNice(const std::string& thName) = 0;
    virtual const std::string& getHeadUnitType() const = 0;

};

#endif // _VR_DECOMMONIF_H_
/* EOF */
