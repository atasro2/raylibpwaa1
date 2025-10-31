#include "global.h"
#include "main.h"
#include "animation.h"
#include "sound.h"
#include "m4a.h"
#include "ewram.h"
#include "utils.h"
#include "script.h"
#include "background.h"
#include "court_record.h"
#include "investigation.h"
#include "save.h"
#include "court.h"
#include "debug.h"
#include "constants/process.h"

struct AnimationListEntry gAnimation[32];
struct CourtRecord gCourtRecord;
u16 gBG0MapBuffer[0x400];
u16 gBG1MapBuffer[0x400];
u16 gBG2MapBuffer[0x400];
u16 gBG3MapBuffer[0x400];
u16 gTilemapBuffer[0x400];
struct Joypad gJoypad;
struct Main gMain;
struct IORegisters gIORegisters;
struct OamAttrs gOamObjects[128];
struct TalkData gTalkData[32];
struct ExaminationData gExaminationData[16];
struct InvestigationStruct gInvestigation;
struct ScriptContext gScriptContext;
struct TextBoxCharacter gTextBoxCharacters[0x3F];
struct MapMarker gMapMarker[8];
struct TestimonyStruct gTestimony;
struct CourtScroll gCourtScroll;
struct DebugContext gDebugCtx;

struct SaveData gSaveDataBuffer;

/* 0x2540 bytes of usable space*/