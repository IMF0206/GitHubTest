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
 * @file VR_VoiceBoxCatalogPhone.h
 * @brief interface for EngineWrapper callback.
 *
 *
 * @attention used for C++ only.
 */
#ifndef VR_VOICEBOXCATALOGPHONE_H
#define VR_VOICEBOXCATALOGPHONE_H

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

#include "MEM_list.h"

#ifndef MEM_MAP_H
#    include "MEM_map.h"
#endif

/* Forward Declaration */

/**
 * @brief The VR_VoiceBoxCatalogPhone class
 *
 * class declaration
 */
class VR_VoiceBoxCatalogPhone : public VR_VoiceBoxCatalog
{
public:
    /**
     * Constructor
     *
     */
    VR_VoiceBoxCatalogPhone(
        IVECIEngineClient& client,
        VR_VoiceBoxEngineCallback& engineCallback
        );

    /**
     * Destructor
     *
     */
    virtual ~VR_VoiceBoxCatalogPhone();

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
    virtual void UnInitialize();

    /**
     * UnInitialize
     *
     * UnInitialize
     *
     * @param none
     * @return bool
     * @attention none
     * @see none
     */
    virtual bool Initialize();

    // voice tag

    /**
     * CreateVoiceTag
     *
     * CreateVoiceTag
     *
     * @param none
     * @return void
     * @attention none
     * @see none
     */
    void CreateVoiceTag();

    void UpdateVoiceTagGrammar(VR_VoiceBoxXmlParser& parser);

    void GetContactNodeInfo(std::string strFullName, const std::string &phoneme, CVECIPtr<IVECIListItems>& contactList);
    void GetContactNodeInfo(std::string strFullName, CVECIPtr<IVECIListItems>& deleteContactList);

    bool RecompileTheGrammar();

    bool RecompileTheGrammar(const std::string &strFullName, const std::string &phoneme);

    VoiceVector<PersonDetailInfo>::type GetContactInfo();

    void SetupPhoneBookData(VR_VoiceBoxXmlParser& parser);

    void PhoneGrammarDisActive(VR_VoiceBoxXmlParser& parser);

    void SetPersonInfo(VoiceVector<PersonDetailInfo>::type vecContact);

    void SetDeviceAddress(const std::string deviceAddress);

    std::string GetTransactionID();

    void PauseGrammarPhone();

    void ResumeGrammarPhone();

    static VoiceMap<std::string, std::string>::type GetTransactionIdMap();
    static void SetTransactionIdMap(VoiceMap<std::string, std::string>::type mapTransactionId);

    void InitialHFDPersonData();

    bool UpdateHFDQuickReplyMessages(VR_VoiceBoxXmlParser& parser);
    bool UpdateHFDPhoneTypes(VR_VoiceBoxXmlParser& parser);
    bool UpdateHFDMessageTypes(VR_VoiceBoxXmlParser& parser);

    void CancelGrammarUpdate();

private:
    IVECIEngineClient&                  m_client;           ///< EngineClient
    VR_VoiceBoxEngineCallback&          m_engineCallback; ///< Callback for the dialog engine
    CVECIPtr<IVECIAgentDataCommand>     m_agentDataCommand; ///< Process AgentData related request

    std::string                   m_deviceAddress;
    VoiceVector<PersonDetailInfo>::type       m_vecContact;       // contact
    bool                                m_isDisactiveFromDE;
    std::string                         m_strGrammar;
    std::string GetGrammarName();
    static VoiceMap<std::string, std::string>::type m_mapSaveTransactionId;
    /**
     * Copy Constructor
     *
     */
    VR_VoiceBoxCatalogPhone(const VR_VoiceBoxCatalogPhone&);

    void LoadContactInfo(const VoiceVector<PersonDetailInfo>::type& vecContact,
        CVECIPtr<IVECIListItems>& contactList);

    void LoadMessageInfo(const VoiceVector<FormalTwoItem>::type& vecMessageInfo,
        CVECIPtr<IVECIListItems>& messageInfoList);

    void LoadMsgOrPhoneType(const VoiceVector<FormalInfo>::type& vecMsgorPhoneType,
    CVECIPtr<IVECIListItems>& msgorPhoneTypeList);


    void GetVoiceTagNodeInfo(VoiceList<std::string>::type NameList,
        CVECIPtr<IVECIListItems>& contactList);
    /**
     * Copy Assignment Operator
     *
     */
    VR_VoiceBoxCatalogPhone &operator=(const VR_VoiceBoxCatalogPhone&);

    void UpdateHFDGrammer(const std::string& strTableName, const CVECIPtr<IVECIListItems>& sourceList, const std::string& strLastFlg);
    void RemoveGrammer(const std::string& strTableName, const std::string& strLastFlg);
    void ContactGrammer(const VoiceVector<PersonDetailInfo>::type& vecPersonInfo);

};


#endif // VR_VOICEBOXCATALOGPHONE_H
/* EOF */