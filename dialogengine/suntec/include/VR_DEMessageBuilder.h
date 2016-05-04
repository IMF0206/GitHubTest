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

#ifndef _VR_DEMESSAGEBUILDER_H_
#define _VR_DEMESSAGEBUILDER_H_


#include "config.h"
#include "VR_DECommon.h"
#include "Vr_Asr_Event_Notify.h"
#include "MEM_list.h"

/**
 * @brief The _VR_DEMESSAGEBUILDER_H_ class
 *
 * Some common function in this class
 */
class VR_DEMessageBuilder
{
public:
    virtual ~VR_DEMessageBuilder();
    VR_DEMessageBuilder(VR_DECommonIF* common);

    void buildVolumeDisplay(N_Vr::N_Asr::C_Event_Notify const& notify, std::string& outMsg);
    void buildPlayTTSAction(std::string txt, std::string& outMsg);
    void buildPlayTTSAction(const VoiceList<std::string>::type& txtList, std::string& outMsg);
    void buildStopTTSAction(std::string& outMsg);
    void buildPlayBeepAction(std::string txt, std::string& outMsg);
    void buildStopBeepAction(std::string& outMsg);
    void buildCloseSessionAction(std::string& outMsg);
    void buildQuitVRAppDisplay(std::string& outMsg);
    void buildVRStateQuitDisplay(std::string& outMsg);

protected:
    VR_DECommonIF* m_pDECommonIF;

private:


};

#endif // _VR_DEMESSAGEBUILDER_H_
/* EOF */
