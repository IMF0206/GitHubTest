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
 * @file VR_VoiceBoxCatalogPoi.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCATALOGPOI_H
#define VR_VOICEBOXCATALOGPOI_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

/* VBT Header */
#ifndef VECIPTR_H
#    include "VECIPtr.h"
#endif

/* Suntec Header */
#ifndef VR_VOICEBOXCATALOG_H
#    include "VR_VoiceBoxCatalog.h"
#endif
 
#ifndef VR_VOICEBOXXMLPARSER_H
#    include "VR_VoiceBoxXmlParser.h"
#endif

#ifndef VR_VOICEBOXENGINECALLBACK_H
#    include "VR_VoiceBoxEngineCallback.h"
#endif

/* Forward Declaration */

/**
 * @brief The VR_VoiceBoxCatalogApps class
 *
 * class declaration
 */
class VR_VoiceBoxCatalogPoi : public VR_VoiceBoxCatalog
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxCatalogPoi(
        IVECIEngineClient& client,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxCatalogPoi();

    /**
     * Initialize
     *
     * Initialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize();

    /**
     * Cataloguing
     *
     * Cataloguing
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Cataloguing(
        VR_VoiceBoxXmlParser& parser
        );

    bool DeleteGrammar();

    void PauseGrammarPoi();

    void ResumeGrammarPoi();

    void CancelGrammarUpdate();

private:
    IVECIEngineClient&                  m_client;           ///< EngineClient
    VR_VoiceBoxEngineCallback&          m_engineCallback; ///< Callback for the dialog engine
    CVECIPtr<IVECIAgentDataCommand>     m_agentDataCommandPoi; ///< Process AgentData related request

    typedef VoiceVector<std::string>::type VecString;
    VoiceVector<VecString>::type m_vecItemData;
    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxCatalogPoi(const VR_VoiceBoxCatalogPoi&);

    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxCatalogPoi &operator=(const VR_VoiceBoxCatalogPoi&);

    bool ReadPoiData();

    bool GetItem(
        const std::string& strLine,
        const std::string strPattern,
        VoiceVector<std::string>::type& vecItem
        );

    void LoadInfo(
        IVECIListItems& listItems
        );
};


#endif // VR_VOICEBOXCATALOGPOI_H
/* EOF */