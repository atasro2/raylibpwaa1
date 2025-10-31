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

#include "sprites.h"
#include "resource_dir.h"

#include "rlgl.h"
#include <stdio.h>

static void DoGameProcess();
static void VBlankIntr();
static void HBlankIntr();
static void IntrDummy();
static void UpdateHardwareBlend();
static void UpdateCourtScroll(struct CourtScroll * );

static void (* const IntrTableFunctionPtrs[])() =
{
    VBlankIntr,
    HBlankIntr,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy,
    IntrDummy
};

void CapcomLogoProcess(struct Main *);
void TitleScreenProcess(struct Main *);

void GameOverScreenProcess(struct Main *);

void SaveGameProcess(struct Main *);
void EpisodeClearedProcess(struct Main *);
void SelectEpisodeProcess(struct Main *);
void ContinueSaveProcess(struct Main *);
void ClearSaveProcess(struct Main *);

void (*gGameProcesses[])(struct Main *) = {
    CapcomLogoProcess,
    TitleScreenProcess,
    GameOverScreenProcess,
    CourtProcess,
    InvestigationProcess,
    TestimonyProcess,
    QuestioningProcess,
    CourtRecordProcess,
    EvidenceAddedProcess,
    VerdictProcess,
    SaveGameProcess,
    EpisodeClearedProcess,
    SelectEpisodeProcess,
    ContinueSaveProcess,
    ClearSaveProcess,
    DebugProcess
};

#define KEY_NEW() ({ (*(u16 *)REG_ADDR_KEYINPUT) ^ KEYS_MASK; })

void CheckAButtonAndGoToClearSaveScreen()
{
    if ((gMain.process[GAME_PROCESS] == CAPCOM_LOGO_PROCESS) && (IsKeyDown(KEY_F9)))
        gMain.process[GAME_PROCESS] = CLEAR_SAVE_PROCESS;
}

struct Shader blendShader;
struct Font font;

const u32 codepoints[] = U"０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ！？あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをんがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇぉゃゅょっアイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォャュョッヴ.☞「」（）『』“”▼▲:、，+/*\'―・。％‥～《》&☆♪　ー記緑中断選消去初期状態戻追加逆転日目法廷姉妹探偵最終手今血気千尋所長無事発言証拠品特異議認弁護人慎重失敗残念私見得与違示関係全然考裁判心悪思明顔説力欠感却下誰月午前時分地方被告第控室間小学校級以来部舞台殺件度胸依頼借実士意味耳助何叫死成歩堂矢張有罪刑女生教害犯新聞書名龍一ヶ回若性逮捕不運彼男政志大親友年巻込知開検察側準備完了困進本当質問答綾里帰宅急便届常識落着次者調読忘好高美佳通用自信命因毒飲鈍器殴首絞冷独審理始亜内具体的凶置物受提出武呼話情報逃反撃祈配近世紀向電会況際遊海外旅行亡国付収入金援口走他天動機移屋正直合図送静粛留守結局我々立覚予決定早現場勧誘山野星雄員半腰抜怖警公衆昼停使種子番暴食含妙怒閉登同押切流致傷触玄上携帯持皆後絶対解剖単憶指摘詰音計続型引題刻裏捜査揺待確取交鳴驚瞬声強烈印象細主要池隣住頃別貸遅氏連想由差変先緊救真形式過渡空巣習家狙訪仕侵色作個製角夜祝興幕料払再起約束永久必身柄司預等忙元慣扱械類晩務打店律集油遠沈黙寝課客梅署悟宵放眠面座革絵画並棚整領活役弱暗欲散某官禁止勝鏥糸鋸圭介例松竹観葉植恐片姿古映泣果働即楽担御剣任怜侍獲段涙負歳黒憎才酷偽返号酸江戸戦倒破紙文字筆跡鑑悲相退水少肩抱速板東奪庫頭奥昨業窓帳簿数専針資能視安飛騒願昔両絡談監看朝修装霊媒師系接挟族父母代宿滅極秘笑捨二孤唯存在化粧造花泊夏降丸苦赤推申様伝株社偶盗聴影宇宙値深檀純突億売買冒論点余揮職轄位可迷席投容怪到標語迅和服順疑析注否液給多利右翌令劇盛髪攻弟為低省壁納験拒筋保管君練表求素刺氷溶条材道忠招参婦拝組工許横円共隠白冊項脅迫寄虫治格協写足応息宣誓処箱効乗脱獄球冗増途設序巨枚机青春商握像供輩愛礼老訴未歴史茶界望鏡超背肉曲線賞優秀努階育似迎至区府密基罰襲左英墓穴掘秩乱休憩固傍減訂砕雲掛週紋義万也圧延財腕焼満闇復根殿賃串荷三郎衣袋夢責軽撮都描夕敵闘繰広毎町米門馬将仲燃故荒操汁湯風統倉車坂排鋼皿割城洋菓痛浮木危鼻折経営般簡権裂貼鋭策訊景富光射俳棋幸秒離暑抵抗索済熱狂討督壊歌飯九太滝浅底限雑誌爆拓萌垂創編核姫神汗恩制緒震園栄林価換建免周炉咲鉄柵壇頑丈演節陽算珍喪恨飢短芸術忍柱薬睡案越臨宝平宮団試拾原末踏縁脚積叩凍頂奇讐湖浴防庁支非床銃零森刃阪研究群沢濃西市民拡砲恋悔喜雪夫冬詩寒趣産祖舗火六吐敬尾避狩魔豪倍匠愚央雷誤弾臓拳痕距侮辱適挑封拘双眼晴盤波岸述釈請犬魚釣栓旗属額辺洗健兵紹訓土埋詳兄寿庵毛十響概館灰脳畔桜録吹競技涼包囲殉厚濯量肺撤石替泳費筒授委飼銀鬼惑尊充遂病院井精悩吸徹執暇玉勉労略婚街盲呂謎則争症賢甘挙札寮医惜良比涯葬廊暮丼伸駅■企巧舟岩辰谷規之藤杉雅敦寺田裕稲総抹贈版";

// ! TODO: get rid of this shit
extern RenderTexture2D animationRenderTexture;

void DrawFrame(void) {
    const int screenWidth = 480;
    const int screenHeight = 320;
    
    struct Camera2D camera;
    camera.offset = (Vector2){screenWidth/2, screenHeight/2};
    camera.target = (Vector2){screenWidth/2, screenHeight/2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(blendShader);
            
            camera.offset.x -= gMain.Bg256_pos_x*2;
            camera.offset.y -= gMain.Bg256_pos_y*2;
            camera.offset.x -= gMain.shakeAmountX;
            camera.offset.y -= gMain.shakeAmountY;
            BeginMode2D(camera);
                DrawBackground();
            EndMode2D();
            
            rlSetBlendFactors(RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, RL_FUNC_ADD);
            BeginBlendMode(BLEND_CUSTOM);
                DrawTexturePro(animationRenderTexture.texture, (Rectangle) { 0, 0, (float)animationRenderTexture.texture.width, (float)-animationRenderTexture.texture.height }, (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight }, (Vector2) { 0, 0 }, 0, WHITE);
            EndBlendMode();
            
            DrawSprites();

            //DrawAnimationYaml();
            //if(gMain.tilemapUpdateBits & 2) {
            if(gIORegisters.lcd_dispcnt & DISPCNT_BG1_ON) {
                camera.offset = (Vector2){screenWidth/2, screenHeight/2};
                camera.offset.x -= gMain.shakeAmountY;
                camera.offset.y -= gMain.shakeAmountX;
                camera.offset.y -= (s16)gIORegisters.lcd_bg1vofs*2; // TODO: only used for scrolling the textbox up and down
                BeginMode2D(camera);
                    DrawTextbox();
                EndMode2D();
            }
            DrawTextGS(font);
        EndShaderMode();
    EndDrawing();
}

void main()
{
    double prevTime, nowTime;
    int temp;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);
    InitWindow(1280, 800, "pwaa1");
    SearchAndSetResourceDir("resources");
    font = LoadFontEx("GenShinGothic-Normal.ttf", 32, codepoints, sizeof(codepoints));
    blendShader = LoadShader("shader/base.vs", "shader/gsblend.fs");
    std_scripts = LoadFileData("script/std_scripts.phscr", &temp);
    reset:
    ClearRamAndInitGame();
    InitTextbox();
    CheckAButtonAndGoToClearSaveScreen();
    while (!WindowShouldClose())
    {        
        if (ReadKeysAndTestResetCombo())
            goto reset; // tfw no SoftReset
    
        if (gMain.currentBgStripe == 0)
        {
            gMain.frameCounter++;
            UpdateBackground();
            //UpdateBGTilemaps();
            //MoveAnimationTilesToRam(0);
            //MoveSpritesToOAM();
            SetLCDIORegs();
            //DebugPrintNum(REG_VCOUNT, 23, 0); // You can move this around to analyze performance from different parts forward!
        }
        if (gMain.currentBgStripe > 10)
        {
            gMain.currentBgStripe = 0;
            CopyBGDataToVram(gMain.currentBG);
        }
        
        if(gMain.currentBgStripe == 0)
            DrawFrame();
        else
            WaitTime(0.0167);
            
        if (gMain.currentBgStripe == 0)
            RunScriptContext();
        
        if(gMain.currentBgStripe == 0)
        {
            UpdateTextbox();
            UpdateItemPlate(&gMain);
            DoGameProcess();
            UpdateAnimations(gMain.previousBG);
            UpdateHardwareBlend();
        }
        else
        {
            DecompressCurrentBGStripe(gMain.currentBG);
        }
        UpdateBGMFade();

        //m4aSoundMain();
        //sub_800156C();
        //DebugPrintNumN(gMain.process[GAME_PROCESS], 10, 0, 2);
        //DebugPrintNumN(gMain.process[GAME_PROCESS_STATE], 12, 0, 2);
        //DebugPrintNumN(gMain.process[GAME_PROCESS_VAR1], 14, 0, 2);
        //DebugPrintNumN(gMain.process[GAME_PROCESS_VAR2], 16, 0, 2);
/*
if(gMain.currentBgStripe && gMain.currentBgStripe <= 10) { // we need to waste 10 frames
WaitTime(0.0167); // This is really fucking dumb
//BeginDrawing();
//EndDrawing();
continue;
}
*/
    }
    CloseWindow();
}

void DoGameProcess()
{
    struct CourtScroll *courtScroll = &gCourtScroll;
    struct Main *main = &gMain;

    u8 amplitude;
    u8 rand;

    if (main->gameStateFlags & 1)
    {
        switch (main->shakeIntensity)
        {
        case 0:
            amplitude = 1;
            break;
        case 1:
            amplitude = 3;
            break;
        case 2:
            amplitude = 7;
            break;
        default:
            amplitude = 3;
            break;
        }

        rand = Random() & 0xF;

        if (rand > 7)
        {
            main->shakeAmountX = rand & amplitude;
            main->shakeAmountX *= -1;
        }
        else
        {
            main->shakeAmountX = rand & amplitude;
        }

        rand = Random() & 0xF;

        if (rand > 7)
        {
            main->shakeAmountY = rand & amplitude;
            main->shakeAmountY *= -1;
        }
        else
        {
            main->shakeAmountY = rand & amplitude;
        }

        //gIORegisters.lcd_bg3vofs = main->shakeAmountY + 8;
        //gIORegisters.lcd_bg3hofs = main->shakeAmountX + 8;
        //gIORegisters.lcd_bg1vofs = main->shakeAmountX;
        //gIORegisters.lcd_bg1hofs = main->shakeAmountY;

        if (main->shakeTimer != 0)
        {
            main->shakeTimer--;
            if (main->shakeTimer == 0)
            {
                main->gameStateFlags &= ~1; // disable shakes
                //gIORegisters.lcd_bg3vofs = 8;
                //gIORegisters.lcd_bg3hofs = 8;
                //gIORegisters.lcd_bg1vofs = 0;
                //gIORegisters.lcd_bg1hofs = 0;
                main->shakeAmountX = 0;
                main->shakeAmountY = 0;
            }
        }
    }
    else
    {
        main->shakeAmountX = 0;
        main->shakeAmountY = 0;
    }

    gGameProcesses[gMain.process[GAME_PROCESS]](&gMain);

    if (courtScroll->state != 0)
    {
        UpdateCourtScroll(courtScroll);
    }
}

void ClearRamAndInitGame()
{
    struct Main *main = &gMain;
    struct IORegisters *ioRegsp = &gIORegisters;
    u32 temp = main->process[GAME_PROCESS] != CAPCOM_LOGO_PROCESS ? TITLE_SCREEN_PROCESS : CAPCOM_LOGO_PROCESS;

    SET_PROCESS(temp, 0, 0, 0);

/*
    RegisterRamReset(RESET_OAM | RESET_VRAM | RESET_PALETTE);
    for (temp = 0; temp < ARRAY_COUNT(IntrTableFunctionPtrs); temp++)
    {
        gIntrTable[temp] = IntrTableFunctionPtrs[temp];
    }
*/
    //m4aSoundInit();
    //REG_WAITCNT = WAITCNT_SRAM_4 | WAITCNT_WS0_N_3 | WAITCNT_WS0_S_1 | WAITCNT_WS1_N_4 | WAITCNT_WS1_S_4 | WAITCNT_WS2_N_4 | WAITCNT_WS2_S_8 | WAITCNT_PHI_OUT_NONE | WAITCNT_PREFETCH_ENABLE;
    ioRegsp->iwp_ie = INTR_FLAG_VBLANK | INTR_FLAG_GAMEPAK;
    ioRegsp->lcd_dispstat = DISPSTAT_VBLANK_INTR;
    ioRegsp->lcd_bldcnt = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_EFFECT_DARKEN;
    ioRegsp->lcd_bldy = 0x10;
    //REG_IE = ioRegsp->iwp_ie;
    //REG_DISPSTAT = ioRegsp->lcd_dispstat;
    //REG_IME = TRUE;
}

void ResetGameState()
{
    struct IORegisters *ioRegsp = &gIORegisters;
    struct Main *main = &gMain;
    //DmaFill16(3, 0, VRAM, VRAM_SIZE);
    //DmaFill16(3, 0, OAM, OAM_SIZE);
    //DmaFill16(3, 0, PLTT, PLTT_SIZE);
    DmaFill16(3, 0, &gMain, sizeof(gMain));
    DmaFill16(3, 0, &gScriptContext, sizeof(gScriptContext));
    DmaFill16(3, 0, &gCourtScroll, sizeof(gCourtScroll));
    DmaFill16(3, 0, &gTestimony, sizeof(gTestimony));
    DmaFill16(3, 0, &gInvestigation, sizeof(gInvestigation));
    DmaFill16(3, 0, &gCourtRecord, sizeof(gCourtRecord));
    DmaFill16(3, 0, &gSaveDataBuffer, sizeof(gSaveDataBuffer));
    main->rngSeed = 3383;
    main->scenarioIdx = 0;
    main->caseEnabledFlags = 0xF;
    main->debugFlags |= 2; // ! 2005 pc port? possibly commented on and off to test stuff in slow mo
    ioRegsp->lcd_bg0cnt = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(28) | BGCNT_16COLOR | BGCNT_WRAP;                 // TODO: add TXT/AFF macro once known which one is used
    ioRegsp->lcd_bg1cnt = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(29) | BGCNT_16COLOR | BGCNT_WRAP;                 // TODO: add TXT/AFF macro once known which one is used
    ioRegsp->lcd_bg2cnt = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(30) | BGCNT_16COLOR | BGCNT_WRAP;                 // TODO: add TXT/AFF macro once known which one is used
    ioRegsp->lcd_bg3cnt = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(31) | BGCNT_MOSAIC | BGCNT_256COLOR | BGCNT_WRAP; // TODO: add TXT/AFF macro once known which one is used
    ioRegsp->lcd_bldcnt = BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_BG3 | BLDCNT_TGT1_OBJ | BLDCNT_EFFECT_DARKEN;
    ioRegsp->lcd_bldy = 0x10;
    HideAllSprites();
    InitBGs();
    ResetAnimationSystem(); //init animation system?
    ResetSoundControl();
    LoadCurrentScriptIntoRam();
    SetTimedKeysAndDelay(DPAD_RIGHT | DPAD_LEFT, 15);
    //m4aMPlayAllStop();
}

void HideAllSprites()
{
    u32 i;
    for (i = 0; i < MAX_OAM_OBJ_COUNT; i++)
    {
        gOamObjects[i].attr0 = SPRITE_ATTR0_CLEAR;
        UnloadSpriteData(i);
    }
}

void SetLCDIORegs()
{
    struct IORegisters *ioRegsp = &gIORegisters;
    float blendIntensity = (float)ioRegsp->lcd_bldy / 16;
    int blendMode = (ioRegsp->lcd_bldcnt >> 6) & 3;
    SetShaderValue(blendShader, GetShaderLocation(blendShader, "blendMode"), &blendMode, SHADER_UNIFORM_INT);
    SetShaderValue(blendShader, GetShaderLocation(blendShader, "intensity"), &blendIntensity, SHADER_UNIFORM_FLOAT);
    /*
    REG_IE = ioRegsp->iwp_ie;
    REG_DISPSTAT = ioRegsp->lcd_dispstat;
    REG_DISPCNT = ioRegsp->lcd_dispcnt;
    DataCopy32(&REG_BG0CNT, &ioRegsp->lcd_bg0cnt);
    DataCopy32(&REG_BG0HOFS, &ioRegsp->lcd_bg0hofs);
    DataCopy32(&REG_BG1HOFS, &ioRegsp->lcd_bg1hofs);
    DataCopy32(&REG_BG2CNT, &ioRegsp->lcd_bg2cnt);
    DataCopy32(&REG_BG2HOFS, &ioRegsp->lcd_bg2hofs);
    DataCopy32(&REG_BG3HOFS, &ioRegsp->lcd_bg3hofs);
    DataCopy32(&REG_BG2PA, &ioRegsp->lcd_bg2pa);
    DataCopy32(&REG_BG2PC, &ioRegsp->lcd_bg2pc);
    REG_BG2X = ioRegsp->lcd_bg2x;
    REG_BG2Y = ioRegsp->lcd_bg2y;
    DataCopy32(&REG_BG3PA, &ioRegsp->lcd_bg3pa);
    DataCopy32(&REG_BG3PC, &ioRegsp->lcd_bg3pc);
    REG_BG3X = ioRegsp->lcd_bg3x;
    REG_BG3Y = ioRegsp->lcd_bg3y;
    DataCopy32(&REG_WIN0H, &ioRegsp->lcd_win0h);
    DataCopy32(&REG_WIN0V, &ioRegsp->lcd_win0v);
    DataCopy32(&REG_WININ, &ioRegsp->lcd_winin);
    DataCopy32(&REG_MOSAIC, &ioRegsp->lcd_mosaic); // this writes to REG_BLDCNT, it shouldn't, should theoretically just write 0.
    REG_BLDCNT = ioRegsp->lcd_bldcnt;
    REG_BLDALPHA = ioRegsp->lcd_bldalpha;
    REG_BLDY = ioRegsp->lcd_bldy;
    */
}

void ReadKeys()
{
    struct Joypad *joypadCtrl = &gJoypad;
    u32 input = 0;
    joypadCtrl->previousHeldKeys = joypadCtrl->heldKeys;
    joypadCtrl->previousPressedKeys = joypadCtrl->pressedKeys;
    if(IsKeyDown(KEY_UP)) {
        input |= DPAD_UP;
    }
    if(IsKeyDown(KEY_DOWN)) {
        input |= DPAD_DOWN;
    }
    if(IsKeyDown(KEY_LEFT)) {
        input |= DPAD_LEFT;
    }
    if(IsKeyDown(KEY_RIGHT)) {
        input |= DPAD_RIGHT;
    }
    if(IsKeyDown(KEY_SPACE)) {
        input |= A_BUTTON;
    }
    if(IsKeyDown(KEY_BACKSPACE)) {
        input |= B_BUTTON;
    }
    // I DON'T WANT TO GO TO THE SAVE SCREEN FOR FUCK'S SAKE
    //if(IsKeyDown(KEY_ENTER)) {
    //    input |= START_BUTTON;
    //}
    if(IsKeyDown(KEY_TAB)) {
        input |= R_BUTTON;
    }
    if(IsKeyDown(KEY_Q)) {
        input |= L_BUTTON;
    }
    joypadCtrl->heldKeys = input;
    joypadCtrl->pressedKeys = joypadCtrl->heldKeys & ~joypadCtrl->previousHeldKeys;
    joypadCtrl->activeTimedKeys = 0;

    if (joypadCtrl->heldKeys & joypadCtrl->timedKeys)
    {
        if (joypadCtrl->timedHoldTimer >= joypadCtrl->timedHoldDelay)
        {
            joypadCtrl->timedHoldTimer = 0;
            joypadCtrl->activeTimedKeys = joypadCtrl->heldKeys & joypadCtrl->timedKeys;
        }
        else
        {
            joypadCtrl->timedHoldTimer++;
        }
    }
    else
    {
        joypadCtrl->timedHoldTimer = joypadCtrl->timedHoldDelay;
    }
}

void SetTimedKeysAndDelay(u32 keyBits, u32 delay)
{
    gJoypad.timedKeys = keyBits;
    gJoypad.timedHoldDelay = delay;
}

u32 ReadKeysAndTestResetCombo()
{
    struct Joypad *joypadCtrl = &gJoypad;
    if (gMain.currentBgStripe == 0)
    {
        ReadKeys();
    }

    gMain.vblankWaitAmount = 1;

    if (joypadCtrl->heldKeys == (A_BUTTON|B_BUTTON|START_BUTTON|SELECT_BUTTON))
    {
        return 1;
    }
    if(joypadCtrl->heldKeys & L_BUTTON && gMain.debugFlags & 2) { // ! 2005 pc port
        gMain.vblankWaitAmount = 20;
    }
    if (joypadCtrl->pressedKeys & SELECT_BUTTON)
        gMain.debugFlags ^= 4; // if(gMain.debugFlags & 4) gMain.debugFlags &= ~4; else gMain.debugFlags |= 4;
    if (joypadCtrl->heldKeys & SELECT_BUTTON && joypadCtrl->pressedKeys & A_BUTTON) {
        gIORegisters.lcd_dispcnt |= DISPCNT_BG0_ON;
        *(u32*)gDebugCtx.process = *(u32*)gMain.process;
        SET_PROCESS(0xF, 0, 0, 0); 
    }
    return 0;
}

void InitCourtScroll(u8 * arg0, u32 arg1, u32 arg2, u32 arg3)
{
    gCourtScroll.frameDataPtr = arg0;
    gCourtScroll.state = arg3;
    gCourtScroll.frameCounter = arg1;
    gCourtScroll.endFrame = arg2;
    gMain.isBGScrolling = 0;
}

static void UpdateCourtScroll(struct CourtScroll * courtScroll)
{
    if (courtScroll->state & 1)
    {
        courtScroll->frameCounter--;
        if (courtScroll->frameCounter < 0)
        {
            courtScroll->state = 0;
        }
    }
    else
    {
        courtScroll->frameCounter++;
        if (courtScroll->frameCounter >= courtScroll->endFrame)
        {
            courtScroll->state &= 1;
        }
    }
}

void StartHardwareBlend(u32 mode, u32 delay, u32 deltaY, u32 target)
{
    gMain.blendTarget = target;
    gMain.blendMode = mode;
    gMain.blendDelay = delay;
    gMain.blendDeltaY = deltaY;
    gMain.blendCounter = 0;
}

static void UpdateHardwareBlend()
{
    struct Main *main = &gMain;
    struct IORegisters *ioRegsp = &gIORegisters;
    switch (main->blendMode)
    {
    case 0:
    default:
        break;
    case 1:
        ioRegsp->lcd_bldcnt = main->blendTarget | BLDCNT_EFFECT_DARKEN;
        main->blendCounter++;
        if (main->blendCounter >= main->blendDelay)
        {
            main->blendCounter = 0;
            ioRegsp->lcd_bldy -= main->blendDeltaY;
        }
        ioRegsp->lcd_bldy &= 0x1F;
        if (ioRegsp->lcd_bldy == 0) // ! will break with odd numbers
        {
            ioRegsp->lcd_bldy = 0;
            ioRegsp->lcd_bldcnt = BLDCNT_TGT1_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_EFFECT_BLEND;
            ioRegsp->lcd_bldalpha = BLDALPHA_BLEND(0x1F, 0x7);
            main->blendMode = 0;
        }
        break;
    case 2:
        ioRegsp->lcd_bldcnt = main->blendTarget | BLDCNT_EFFECT_DARKEN;
        main->blendCounter++;
        if (main->blendCounter >= main->blendDelay)
        {
            main->blendCounter = 0;
            ioRegsp->lcd_bldy += main->blendDeltaY;
        }
        ioRegsp->lcd_bldy &= 0x1F;
        if (ioRegsp->lcd_bldy == 0x10) // ! will break with odd numbers
        {
            main->blendMode = 0;
        }
        break;
    case 3:
        ioRegsp->lcd_bldcnt = main->blendTarget | BLDCNT_EFFECT_LIGHTEN;
        main->blendCounter++;
        if (main->blendCounter >= main->blendDelay)
        {
            main->blendCounter = 0;
            ioRegsp->lcd_bldy -= main->blendDeltaY;
        }
        ioRegsp->lcd_bldy &= 0x1F;
        if (ioRegsp->lcd_bldy == 0) // ! will break with odd numbers
        {
            ioRegsp->lcd_bldy = 0;
            ioRegsp->lcd_bldcnt = BLDCNT_TGT1_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_EFFECT_BLEND;
            ioRegsp->lcd_bldalpha = BLDALPHA_BLEND(0x1F, 0x7);
            main->blendMode = 0;
        }
        break;
    case 4:
        ioRegsp->lcd_bldcnt = main->blendTarget | BLDCNT_EFFECT_LIGHTEN;
        main->blendCounter++;
        if (main->blendCounter >= main->blendDelay)
        {
            main->blendCounter = 0;
            ioRegsp->lcd_bldy += main->blendDeltaY;
        }
        ioRegsp->lcd_bldy &= 0x1F;
        if (ioRegsp->lcd_bldy == 0x10) // ! will break with odd numbers
        {
            main->blendMode = 0;
        }
        break;
    }
}

void VBlankIntr()
{
    //m4aSoundVSync();
    gMain.vblankWaitCounter++;
}

void HBlankIntr()
{
}

void IntrDummy()
{
}