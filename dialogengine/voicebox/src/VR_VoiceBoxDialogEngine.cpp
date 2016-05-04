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

/* Suntec Header */
#ifndef VR_VOICEBOXDIALOGENGINE_H
#    include "VR_VoiceBoxDialogEngine.h"
#endif

#ifndef VR_VOICEBOXENGINEIF_H
#    include "VR_VoiceBoxEngineIF.h"
#endif

#ifndef VR_VOICEBOXMSGCONTROLLER_H
#    include "VR_VoiceBoxMsgController.h"
#endif

#ifndef VR_VOICEBOXCATALOGCONTROLLER_H
#    include "VR_VoiceBoxCatalogController.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#ifndef VR_DEF_H
#    include "VR_Def.h"
#endif

#ifndef CXX_CL_DIR_ABS_H
#   include "CL_Dir_Abs.h"
#endif

#ifndef CXX_BL_STRING_H
#   include "BL_String.h"
#endif

extern "C" {
// Factory Method
// Create a dialog engine instance
VR_API VR_DialogEngineIF* VR_CreateDialogEngine()
{
    // The external service should delete created instance
    return VR_new VR_VoiceBoxDialogEngine;
}
}

// Constructor
VR_VoiceBoxDialogEngine::VR_VoiceBoxDialogEngine()
    : m_pcVoiceBoxEngine(NULL)
    , m_pcMsgController(NULL)
    , m_pcCatalogController(NULL)
{
}

// Destructor
VR_VoiceBoxDialogEngine::~VR_VoiceBoxDialogEngine()
{
    delete m_pcVoiceBoxEngine;
    m_pcVoiceBoxEngine = NULL;
    delete m_pcMsgController;
    m_pcMsgController = NULL;
    delete m_pcCatalogController;
    m_pcCatalogController = NULL;
}

// Create the dialog engine related instances and initialize them.
bool VR_VoiceBoxDialogEngine::Initialize(
    VR_DialogEngineListener *pcExternalCallback,
    const VR_Settings &settings
    )
{
    VR_LOG("VR_VoiceBoxDialogEngine Initializing\n");

    CL_Dir_Abs cDir;
    CL_AbstractFile::FileSize size;

    // Confirm the data version, if the version of /data is different from
    // the version of /vdata, we should copy the /vdata to /data again.
    do {
        std::string strBackupVersion;
        std::string strVBTDataPath = VR_ConfigureIF::Instance()->getDataPath();
        strVBTDataPath.append("data/version");
        if (!LoadVersionInfo(strVBTDataPath, strBackupVersion)) {
            // If the backup data is invalid, we should not copy it.
            VR_LOG("Load backup data version failed");
            break;
        }

        // if load current version success, we need to compare it with backup version
        // if load current version failed, we should copy the backup data.
        std::string strCurrentVersion;
        LoadVersionInfo("/data/vr/data/version", strCurrentVersion);

        if (strBackupVersion != strCurrentVersion) {
            VR_LOG("Remove the used data directory");
            cDir.RemoveDir(XTEXT("/data/vr"));
        }
    } while (false);

    if (!cDir.GetFileInfo(XTEXT("/data/vr/config/VoiceBox.cfg"), &size)) {
        if (!cDir.MakeDirTree(XTEXT("/data/vr/data"))) {
            VR_LOG("Make direcotry tree failed\n");
            return false;
        }

        if (!cDir.MakeDirTree(XTEXT("/data/vr/config"))) {
            VR_LOG("Make direcotry tree failed\n");
            return false;
        }

        if (!cDir.MakeDirTree(XTEXT("/data/vr/vbtlog"))) {
            VR_LOG("Make direcotry tree failed\n");
            return false;
        }

        const int DATA_PATH_LEN = 256;
        XCHAR chVBTDataPath[DATA_PATH_LEN] = { 0 };

        std::string strVBTDataPath = VR_ConfigureIF::Instance()->getDataPath();
        strVBTDataPath.append("data");
        int len = CharToXCHAR(strVBTDataPath.c_str(), strVBTDataPath.length(), chVBTDataPath, DATA_PATH_LEN);
        if (len != strVBTDataPath.length()) {
            VR_LOG("Convert char to xchar failed\n");
            return false;
        }

        if (!cDir.CopyDir(chVBTDataPath, XTEXT("/data/vr/data"), CL_TRUE)) {
            VR_LOG("Copy data failed\n");
            return false;
        }

        int fd = open("/data/vr/data/sourceid.txt", O_WRONLY | O_CREAT);
        if (-1 != fd) {
            if (-1 == write(fd, "0000", 4)) {
                VR_LOG("write failed");
            }

            if (-1 == close(fd)) {
                VR_LOG("close failed");
            }
        }

        strVBTDataPath = VR_ConfigureIF::Instance()->getDataPath();
        strVBTDataPath.append("/config");
        len = CharToXCHAR(strVBTDataPath.c_str(), strVBTDataPath.length(), chVBTDataPath, DATA_PATH_LEN);
        if (len != strVBTDataPath.length()) {
            VR_LOG("Convert char to xchar failed\n");
            return false;
        }

        if (!cDir.CopyDir(chVBTDataPath, XTEXT("/data/vr/config"), CL_TRUE)) {
            VR_LOG("Copy config failed\n");
            return false;
        }
    }

    // Create the VoiceBox dialog engine instance
    // Used for voice recognition
    m_pcVoiceBoxEngine = VR_VoiceBoxEngineIF::CreateInstance();
    if (NULL == m_pcVoiceBoxEngine) {
        VR_ERROR("VR_VoiceBoxDialogEngine Create Failed\n");
        return false;
    }

    // Create the message contoller
    // Used for dispatch the XML message the receive from external service
    // or self.
    m_pcMsgController = VR_new VR_VoiceBoxMsgController();
    if (NULL == m_pcMsgController) {
        return false;
    }

    // Create the catalog message contoller
    // Used for dispatch the XML message the receive from external service
    // or self.
    m_pcCatalogController = VR_new VR_VoiceBoxCatalogController();
    if (NULL == m_pcCatalogController) {
        return false;
    }


    // Initialize the VoiceBox dialog engine
    bool bInitRet = m_pcVoiceBoxEngine->Initialize(
                    pcExternalCallback, // Used for notify external service
                    m_pcMsgController,  // Used for receive self's message
                    m_pcCatalogController
                    );
    if (!bInitRet) {
        VR_ERROR("VR_VoiceBoxDialogEngine Initialize failed\n");
    }

    // Initialize the message controller
    bInitRet = m_pcMsgController->Initialize(
               m_pcVoiceBoxEngine  // Used for process VR related requests
               );
    if (!bInitRet) {
        return false;
    }

    bInitRet = m_pcCatalogController->Initialize(
               m_pcVoiceBoxEngine  // Used for process VR related requests
               );
    if (!bInitRet) {
        return false;
    }

    VR_LOG("VR_VoiceBoxDialogEngine Initialize successfully\n");
    return true;
}

// Start the dialog engine
bool VR_VoiceBoxDialogEngine::Start()
{
    VR_LOG("VR_VoiceBoxDialogEngine Starting\n");

    if (NULL == m_pcVoiceBoxEngine) {
        return false;
    }

    // Start the message controller thread
    if (NULL != m_pcMsgController) {
        m_pcMsgController->Start();
    }

    // Start the catalog controller thread
    if (NULL != m_pcCatalogController) {
        m_pcCatalogController->Start();
    }

    VR_ConfigureIF * pcConfig = VR_ConfigureIF::Instance();
    if (NULL == pcConfig) {
        return false;
    }

    std::string cultureName = "en-US";
    std::string lang = pcConfig->getVRLanguage();
    if (VR_LANGUAGE_FR_CA == lang) {
        cultureName = "fr-CA";
    }
    else if (VR_LANGUAGE_ES_MX == lang) {
        cultureName = "es-MX";
    }
    else if (VR_LANGUAGE_EN_AU == lang) {
        cultureName = "en-AU";
    }
    else {
        // do nothing
    }

    // If start dialog engine failed, we could not used the VR function.
    if (!m_pcVoiceBoxEngine->Start(cultureName)) {
        VR_ERROR("VR_VoiceBoxDialogEngine Start failed\n");
        return false;
    }
    int promptLevel = pcConfig->getVRPromptLevel();
    m_pcVoiceBoxEngine->setPromptLevel(promptLevel);
    VR_LOG("VR_VoiceBoxDialogEngine Start successfully\n");
    return true;
}

// Stop the dialog engine
void VR_VoiceBoxDialogEngine::Stop()
{
    if (NULL != m_pcVoiceBoxEngine) {
        // Stop the dialog engine
        m_pcVoiceBoxEngine->Stop();
    }

    if (NULL != m_pcMsgController) {
        // Stop the message controller thread
        m_pcMsgController->Stop();
    }

    if (NULL != m_pcCatalogController) {
        // Stop the catalog controller thread
        m_pcCatalogController->Stop();
    }
}

// Uninitialize the dialog engine
void VR_VoiceBoxDialogEngine::UnInitialize()
{
    if (NULL != m_pcVoiceBoxEngine) {
        // Uninitialize and shutdown the dialog engine
        m_pcVoiceBoxEngine->UnInitialize();
    }

    // Delete the dialog engine and message controller instances
    delete m_pcVoiceBoxEngine;
    m_pcVoiceBoxEngine = NULL;
    delete m_pcMsgController;
    m_pcMsgController = NULL;
    delete m_pcCatalogController;
    m_pcCatalogController = NULL;
}

// Process the messages that received from external service or self
bool VR_VoiceBoxDialogEngine::SendMessage(const std::string& message, int actionSeqId)
{
    VR_LOG("VR_VoiceBoxDialogEngine SendMessage: %s\n", message.c_str());
    if ((NULL == m_pcVoiceBoxEngine) || (NULL == m_pcMsgController)) {
        // If the dialog engine or message controller thread does not exist,
        // We could not process the messages, so we just return.
        return false;
    }

    // Post message to the message controller,
    // the controller will dispatch the XML message.
    if (!m_pcMsgController->PostMessage(message, actionSeqId)) {
        return false;
    }

    VR_LOG("VR_VoiceBoxDialogEngine SendMessage successfully\n");
    return true;
}

bool
VR_VoiceBoxDialogEngine::LoadVersionInfo(const std::string& strVersionPath, std::string& strVersion)
{
    const size_t VERSION_MAX_LEN = 64;
    char version[VERSION_MAX_LEN] = { 0 };
    FILE *pFile = fopen(strVersionPath.c_str(), "r");
    if (NULL == pFile) {
        VR_LOG("Open the version file failed");
        return false;
    }

    size_t result = fread(version, 1, VERSION_MAX_LEN, pFile);
    fclose(pFile);
    if (0 == result) {
        VR_LOG("Read the version file failed");
        return false;
    }

    strVersion = version;
    return true;
}

std::string
VR_VoiceBoxDialogEngine::getHints(const std::string& hintsParams)
{
    if (NULL == m_pcVoiceBoxEngine) {
        return "";
    }
    else {
        return m_pcVoiceBoxEngine->getHints(hintsParams);
    }
}

/* EOF */
