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
 * @file config.h
 * @brief include file for standard system include files,
 *
 * This file include file for standard system include files,
 * or project specific include files that are used frequently,
 * but are changed infrequently
 *
 * @attention not include in any head file.
 */
#ifndef _DE_CONFIG_H
#define _DE_CONFIG_H

#include <fstream>

#ifndef VR_MACRO_H
#   include "VR_Macro.h"
#endif

#ifndef VR_DEF_H
#   include "VR_Def.h"
#endif

#ifndef CXX_VR_LOG_H
#   include "VR_Log.h"
#endif

#include "VR_ConfigureIF.h"


#define INVALID 0x7FFF
#define START_BEEPPATH ("beep/startVR.wav")
#define END_BEEPPATH ("beep/endVR.wav") // quit VR
#define RETURN_BEEPPATH ("beep/returnVR.wav") // return recog result
#define DISPLAYNODE "<display agent='Common' content='ScreenDisplay'>\
<agent>0</agent> \
<content>0</content>\
<recognitionResult></recognitionResult>\
<screenTitle></screenTitle>\
<subTitle></subTitle>\
<prompt></prompt>\
<hints>\
<list>\
<header>\
<pageSize>0</pageSize>\
<startIndex>0</startIndex>\
<count>0</count>\
</header>\
<items>\
</items>\
</list>\
</hints>\
</display>"

#define HINTSDISPLAY "<display agent='Common' content='HintsDisplay'>\
<hints>\
<list>\
<header>\
<startIndex>0</startIndex>\
<pageSize>5</pageSize>\
<count>11</count>\
</header>\
<items>\
</items>\
</list>\
</hints>\
</display>"

#define VR_CONFIDENCE_THRESHOLD                     4500
#define VR_VOICETAG_RECOGNIZE_CONFIDENCE_THRESHOLD  3750

extern int VR_VOICETAG_RECORD_CONFIDENCE_THRESHOLD;
extern int VR_RECOGNIZE_CONFIDENCE_THRESHOLD;
#define VR_PLAY_BEEP(WAVE_PATH) "<action agent=\"prompt\" op=\"playBeep\">"\
    "<beepFile>"\
    + WAVE_PATH + \
    "</beepFile>"\
    "<bargein>"\
    + (VR_ConfigureIF::Instance()->getVROverBeep()==true ? "true":"false") +\
    "</bargein>"\
    "</action>"\

#define VR_PLAY_BEEP_BARGEIN(WAVE_PATH) "<action agent=\"prompt\" op=\"playBeep\">"\
    "<beepFile>"\
    + WAVE_PATH + \
    "</beepFile>"\
    "<bargein>true</bargein>"\
    "</action>"\


#define VR_PLAYTTS(TTS_PROMPT) "<action agent=\"prompt\" op=\"playTts\">"\
    "<language>"\
    + VR_ConfigureIF::Instance()->getVRLanguage() +\
    "</language>"\
    "<text>"\
    + TTS_PROMPT +\
    "</text>"\
    + (VR_ConfigureIF::Instance()->getVROverBeep()==true ? "true":"false") +\
    "</action>"

#define VR_PLAYTTS_BAEGEIN(TTS_PROMPT) "<action agent=\"prompt\" op=\"playTts\">"\
    "<language>"\
    + VR_ConfigureIF::Instance()->getVRLanguage() +\
    "</language>"\
    "<text>"\
    + TTS_PROMPT +\
    "</text>"\
    "<bargein>true</bargein>"\
    "</action>"


#define MODEL_ASR "$modelAsr"
#define MODEL_TTS "$modelTTS"
#define GLOBALSTATESETTING "globalState"
#define FORWARD "forward"
#define FORBACK "back"

#define VR_ASR_EVENT_NAME     "asrEvent_Event"
// ASR event types
#define VR_ASR_EVT_LISTEN     "asr_listen"
#define VR_ASR_EVT_BOS        "asr_speech_begin"
#define VR_ASR_EVT_EOS        "asr_speech_end"
#define VR_ASR_EVT_RESULT     "asr"
#define VR_ASR_EVT_END        "asr_end"

#define VR_DUMP_TESTCASE(name, content) dumpTestcase(name, content)
#define VR_LOG_TO_FILE(name, content) writeToFile(name, content)

void dumpTestcaseSwitch(bool on);
void dumpTestcase(const std::string& name, const std::string& content);
void writeToFile(const std::string& name, const std::string& content);


#endif // _DE_CONFIG_H
/* EOF */
