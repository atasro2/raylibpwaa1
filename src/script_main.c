#include "global.h"
#include "main.h"
#include "script.h"
#include "sound.h"
#include "ewram.h"
#include "graphics.h"
#include "constants/script.h"
#include "constants/songs.h"
#include "constants/process.h"
#include "constants/oam_allocations.h"

static void AdvanceScriptContext(struct ScriptContext *);
static void DrawTextAndMapMarkers(struct ScriptContext *);
static void PutCharInTextbox(u32, u32, u32);

static u8 gTextColorTileBuffer[0x80];

const u8 * const gScriptTable[] = {
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,
    scenario_0_script,

};

bool32 (*gScriptCmdFuncs[0x60])(struct ScriptContext *) = {
	Command00,
	Command01,
	Command02,
	Command03,
	Command04,
	Command05,
	Command06,
	Command02,
	Command08,
	Command09,
	Command02,
	Command0B,
	Command0C,
	Command0D,
	Command0E,
	Command0F,
	Command10,
	Command11,
	Command12,
	Command13,
	Command14,
	Command15,
	Command16,
	Command17,
	Command18,
	Command19,
	Command1A,
	Command1B,
	Command1C,
	Command1D,
	Command1E,
	Command1F,
	Command20,
	Command21,
	Command22,
	Command23,
	Command24,
	Command25,
	Command26,
	Command27,
	Command28,
	Command29,
	Command2A,
	Command2B,
	Command2C,
	Command02,
	Command2E,
	Command2F,
	Command30,
	Command31,
	Command32,
	Command33,
	Command34,
	Command35,
	Command36,
	Command37,
	Command38,
	Command39,
	Command3A,
	Command3B,
	Command3C,
	Command3D,
	Command3E,
	Command3F,
	Command40,
	Command41,
	Command42,
	Command43,
	Command44,
	Command15,
	Command46,
	Command47,
	Command48,
	Command49,
	Command4A,
	Command4B,
	Command4C,
	Command4D,
	Command4E,
	Command4F,
	Command50,
	Command51,
	Command52,
	Command53,
	Command54,
	Command55,
	Command56,
	Command57,
	Command58,
	Command59,
	Command5A,
	Command5B,
	Command5C,
	Command5D,
	Command5E,
	Command5F,
};

u8 * eScriptHeap = NULL;

const char * scriptFilenames[] = {
    "script/scenario_0_script.phscr",
    "script/scenario_1_0_script.phscr",
    "script/scenario_1_1_script.phscr",
    "script/scenario_1_2_script.phscr",
    "script/scenario_1_3_script.phscr",
    "script/scenario_2_0_script.phscr",
    "script/scenario_2_1_script.phscr",
    "script/scenario_2_2_script.phscr",
    "script/scenario_2_3_script.phscr",
    "script/scenario_2_4_script.phscr",
    "script/scenario_2_5_script.phscr",
    "script/scenario_3_0_script.phscr",
    "script/scenario_3_1_script.phscr",
    "script/scenario_3_2_script.phscr",
    "script/scenario_3_3_script.phscr",
    "script/scenario_3_4_script.phscr",
    "script/scenario_3_5_script.phscr",
};

void LoadCurrentScriptIntoRam(void)
{
    u32 i;
    DmaCopy16(3, gTextPal, OBJ_PLTT, 0x20);

    for (i = 0; i < ARRAY_COUNT(gTextBoxCharacters); i++)
    {
        gTextBoxCharacters[i].state &= ~0x8000;
    }
    if(eScriptHeap != NULL) {
        UnloadFileData(eScriptHeap);
    }
    eScriptHeap = LoadFileData(scriptFilenames[gMain.scenarioIdx], &i);
    //LZ77UnCompWram(gScriptTable[gMain.scenarioIdx], eScriptHeap);
}

void RunScriptContext(void)
{
    if (gMain.advanceScriptContext && gMain.blendMode == 0)
    {
        AdvanceScriptContext(&gScriptContext);
    }
    DrawTextAndMapMarkers(&gScriptContext);
}

void ChangeScriptSection(u32 newSection)
{
    gScriptContext.previousSection = gScriptContext.currentSection;
    gScriptContext.currentSection = newSection;
    InitScriptSection(&gScriptContext);
    gScriptContext.scriptPtr++;
}

void InitScriptSection(struct ScriptContext *scriptCtx)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(gTextBoxCharacters); i++)
    {
        gTextBoxCharacters[i].state &= ~0x8000;
    }
    scriptCtx->textX = 0;
    scriptCtx->textY = 0;
    if (!(gMain.process[GAME_PROCESS] == INVESTIGATION_PROCESS && gMain.process[GAME_PROCESS_STATE] == INVESTIGATION_TALK))
        scriptCtx->textSkip = 0;
    scriptCtx->unk15 = 0;
    scriptCtx->paragraphSkipDelayCounter = 8;
    scriptCtx->soundCueSkip = 1;
    scriptCtx->currentSoundCue = 0;
    scriptCtx->textXOffset = 9;
    scriptCtx->textYOffset = 0x74;
    scriptCtx->nextSection = scriptCtx->currentSection + 1;
    scriptCtx->holdItSection = 0;
    scriptCtx->holdItFlag = 0;
    scriptCtx->textboxNameId = 0;
    scriptCtx->textboxDownArrowIndex = 0;
    scriptCtx->textboxDownArrowDelayCounter = 0;
    scriptCtx->flags = 0;
    scriptCtx->waitTimer = 0;
    scriptCtx->textColor = 0;
    scriptCtx->textSpeed = 3;
    scriptCtx->prevTextSpeed = 3;
    scriptCtx->textDelayTimer = 0;
    scriptCtx->fullscreenTextXOffset = 0x18;
    scriptCtx->fullscreenTextYOffset = 0x56;
    {
        const u8 *r1;
        const u32 *r0;
        if (scriptCtx->currentSection >= 0x80)
        {
            r1 = eScriptHeap;
            r0 = &((u32*)eScriptHeap)[scriptCtx->currentSection-0x80];
            scriptCtx->scriptPtr = (u16*)(r1 + r0[1]);
            scriptCtx->scriptSectionPtr = scriptCtx->scriptPtr;
            scriptCtx->scriptHeaderSize = *(u16*)r1;
        }
        else
        {
            r1 = std_scripts;
            r0 = &((u32*)std_scripts)[scriptCtx->currentSection];
            scriptCtx->scriptPtr = (u16*)(r1 + r0[1]);
            scriptCtx->scriptSectionPtr = scriptCtx->scriptPtr;
            scriptCtx->scriptHeaderSize = *(u16*)r1;
        }
    }
    scriptCtx->mapMarkerVramPtr = (void*)(VRAM + 0x11800);
    for (i = 0; i < ARRAY_COUNT(gMapMarker); i++)
    {
        gMapMarker[i].id |= 0xFF;
        gMapMarker[i].isBlinking = 0;
        gMapMarker[i].flags = 0;
        gMapMarker[i].attr0 = SPRITE_ATTR0_CLEAR;
    }
}

static void AdvanceScriptContext(struct ScriptContext * scriptCtx)
{
    if(scriptCtx->textSkip > 0 && (gJoypad.pressedKeys & A_BUTTON || gJoypad.heldKeys & B_BUTTON)) // text skip
        scriptCtx->textSkip = 2;
    
    continueScript:
    scriptCtx->currentToken = *scriptCtx->scriptPtr;
    if(scriptCtx->currentToken < 0x80)
    {
        if(gScriptCmdFuncs[scriptCtx->currentToken](scriptCtx))
            return;
        else
            goto continueScript;
    }
    if(scriptCtx->textSkip > 1)
    {
        scriptCtx->textSpeed = 0;
    }
    scriptCtx->textDelayTimer++;
    if(scriptCtx->textDelayTimer >= scriptCtx->textSpeed)
    {
        scriptCtx->textDelayTimer = 0;
        scriptCtx->currentToken -= 0x80;
        if (scriptCtx->flags & SCRIPT_FULLSCREEN)
        {
            PutCharInTextbox(scriptCtx->currentToken, scriptCtx->fullscreenTextY, scriptCtx->fullscreenCharCount);
            scriptCtx->fullscreenCharCount++;
            scriptCtx->fullscreenTextX++;
        }
        else
        {
            PutCharInTextbox(scriptCtx->currentToken, scriptCtx->textY, scriptCtx->textX);
            scriptCtx->textX++;
        }

        scriptCtx->scriptPtr++;
        
        if (!(scriptCtx->currentSection == 0x80 && gMain.scenarioIdx == 0) && scriptCtx->currentToken != 0xFF)
        {
            if ( scriptCtx->textSpeed > 0)
            {
                if ( scriptCtx->soundCueSkip == 0 || scriptCtx->textSpeed > 4 )
                {
                    if ( scriptCtx->currentSoundCue != 2 )
                        scriptCtx->soundCueSkip = 1;

                    if (!(gMain.soundFlags & SOUND_FLAG_DISABLE_CUE))
                    {
                        if (scriptCtx->currentSoundCue == 2)
                        {
                            PlaySE(SE01A_TEXT_TYPEWRITER_LOW);
                        }
                        else if (scriptCtx->currentSoundCue == 1)
                        {
                            PlaySE(SE004_TEXT_BLIP_FEMALE);
                        }
                        else
                        {
                            PlaySE(SE003_TEXT_BLIP_MALE);
                        }
                    }
                }
                else
                {
                    scriptCtx->soundCueSkip--;
                }
            }
        }
        if(scriptCtx->textSpeed == 0)
        {
            goto continueScript;
        }
    }
}

static void PutCharInTextbox(u32 characterCode, u32 y, u32 x)
{
    uintptr_t i;
    uintptr_t temp = characterCode*0x80;
    temp += (uintptr_t)gCharSet;
    if(gScriptContext.textColor)
    {
        u8 * vram;
        u8 * pixel;
        DmaCopy16(3, (void*)temp, gTextColorTileBuffer, 0x80);
        pixel = gTextColorTileBuffer;
        temp = gScriptContext.textColor * 3;
        for(i = 0; i < 0x80; i++)
        {
            u32 pixelLeft, pixelRight;
            pixelLeft = pixelRight = *pixel;
            pixelLeft &= 0xF;
            pixelRight &= 0xF0;
            if(pixelLeft)
                pixelLeft += temp;
            if(pixelRight)
                pixelRight += temp << 4;
            *pixel++ = pixelLeft | pixelRight;
        }
        i = (uintptr_t)OBJ_VRAM0;
        i += x * 0x80;
        if(gScriptContext.flags & 4)
            i += 0x80 * (2 * 16);
        else
            i += 0x80 * (y * 16);
        DmaCopy16(3, gTextColorTileBuffer, (void*)i, 0x80);
    }
    else
    {
        i = (uintptr_t)OBJ_VRAM0;
        i += x * 0x80;
        if(gScriptContext.flags & 4)
            i += 0x80 * (2 * 16);
        else
            i += 0x80 * (y * 16);
        DmaCopy16(3, (void*)temp, (void*)i, 0x80);
    }
    if(gScriptContext.flags & 4)
    {
        temp = x + 2 * 0x10;
        gTextBoxCharacters[temp].x = gScriptContext.fullscreenTextX * (16-2); 
        gTextBoxCharacters[temp].y = (y - 2) * (16+4);
        gTextBoxCharacters[temp].objAttr2 = 2 * 0x40 + x * 4;
    }
    else
    {
        temp = x + y * 0x10;
        gTextBoxCharacters[temp].x = x * (16-2); 
        gTextBoxCharacters[temp].y = y * (16+2);
        gTextBoxCharacters[temp].objAttr2 = y * 0x40 + x * 4;
    }
    gTextBoxCharacters[temp].objAttr2 += 0x400;
    gTextBoxCharacters[temp].state = characterCode | 0x8000;
    gTextBoxCharacters[temp].color = gScriptContext.textColor;
}

static void DrawTextAndMapMarkers(struct ScriptContext * scriptCtx)
{
    struct OamAttrs * oam;
    u32 i;
    u32 y, x;
    if(!(scriptCtx->flags & SCRIPT_FULLSCREEN))
    {
        oam = &gOamObjects[OAM_IDX_MAP_MARKER];
        for(i = 0; i < ARRAY_COUNT(gMapMarker); i++) 
        {
            if(gMapMarker[i].id != 0xFF)
            {    
                oam->attr0 = gMapMarker[i].attr0;
                oam->attr1 = gMapMarker[i].attr1;
                if(gMapMarker[i].isBlinking)
                {
                    gMapMarker[i].blinkTimer++;
                    gMapMarker[i].blinkTimer &= 0x1F;
                    if (gMapMarker[i].blinkTimer < 16)
                        oam->attr0 = SPRITE_ATTR0_CLEAR;
                }
                if(gMapMarker[i].flags & 0x2)
                {
                    gMapMarker[i].distanceMoved += gMapMarker[i].speed;
                    if (gMapMarker[i].distanceMoved >= gMapMarker[i].distanceToMove)
                    {
                        gMapMarker[i].speed -= gMapMarker[i].distanceMoved - gMapMarker[i].distanceToMove;
                        gMapMarker[i].flags &= ~2;
                    }
                    switch(gMapMarker[i].direction)
                    {
                        case 0:
                            y = gMapMarker[i].attr0 & 0xFF;
                            gMapMarker[i].attr0 &= ~0xFF;
                            y -= gMapMarker[i].speed;
                            y &= 0xFF;
                            gMapMarker[i].attr0 += y;
                            oam->attr0 = gMapMarker[i].attr0;
                            break;
                        case 1:
                            y = gMapMarker[i].attr0 & 0xFF;
                            gMapMarker[i].attr0 &= ~0xFF;
                            y += gMapMarker[i].speed;
                            y &= 0xFF;
                            gMapMarker[i].attr0 += y;
                            oam->attr0 = gMapMarker[i].attr0;
                            break;
                        case 2:
                            x = gMapMarker[i].attr1 & 0x1FF;
                            gMapMarker[i].attr1 &= ~0x1FF;
                            x -= gMapMarker[i].speed;
                            x &= 0x1FF;
                            gMapMarker[i].attr1 += x;
                            oam->attr1 = gMapMarker[i].attr1;
                            break;
                        case 3:
                            x = gMapMarker[i].attr1 & 0x1FF;
                            gMapMarker[i].attr1 &= ~0x1FF;
                            x += gMapMarker[i].speed;
                            x &= 0x1FF;
                            gMapMarker[i].attr1 += x;
                            oam->attr1 = gMapMarker[i].attr1;
                            break;
                        default:
                            break;
                    }
                }
                if(gMapMarker[i].flags & 0x4)
                    oam->attr0 = SPRITE_ATTR0_CLEAR;
                oam++;
            }
            else
            {
                oam->attr0 = SPRITE_ATTR0_CLEAR;
                oam++;
            }
        }
        
    }

    if(gMain.showTextboxCharacters)
    {
        oam = &gOamObjects[OAM_IDX_TEXT];
        for(i = 0; i < OAM_COUNT_TEXT; i++)
        {
            
            if(gTextBoxCharacters[i].state & 0x8000)
            {
                oam->attr0 = SPRITE_ATTR0(gTextBoxCharacters[i].y + scriptCtx->textYOffset, ST_OAM_AFFINE_OFF, ST_OAM_OBJ_NORMAL, FALSE, ST_OAM_4BPP, ST_OAM_SQUARE);
                oam->attr1 = SPRITE_ATTR1_NONAFFINE(gTextBoxCharacters[i].x + scriptCtx->textXOffset, FALSE, FALSE, 1);
                oam->attr2 = gTextBoxCharacters[i].objAttr2;
            }
            else
                oam->attr0 = SPRITE_ATTR0_CLEAR;
            oam++;
        }   
        if(scriptCtx->flags & SCRIPT_FULLSCREEN)
        {
            oam = &gOamObjects[OAM_IDX_TEXT_FULLSCREEN];
            for(i = 32; i < ARRAY_COUNT(gTextBoxCharacters); i++)
            {
                if(gTextBoxCharacters[i].state & 0x8000)
                {
                    oam->attr0 = SPRITE_ATTR0(gTextBoxCharacters[i].y + scriptCtx->fullscreenTextYOffset, ST_OAM_AFFINE_OFF, ST_OAM_OBJ_NORMAL, FALSE, ST_OAM_4BPP, ST_OAM_SQUARE);
                    oam->attr1 = SPRITE_ATTR1_NONAFFINE(gTextBoxCharacters[i].x + scriptCtx->fullscreenTextXOffset, FALSE, FALSE, 1);
                    oam->attr2 = gTextBoxCharacters[i].objAttr2;
                }
                else
                    oam->attr0 = SPRITE_ATTR0_CLEAR;
                oam++;
            }
        }
    }
    else
    {
        oam = &gOamObjects[OAM_IDX_TEXT];
        for(i = 0; i < OAM_COUNT_TEXT; i++)
        {
            oam->attr0 = SPRITE_ATTR0_CLEAR;
            oam++;
        }
        if(scriptCtx->flags & SCRIPT_FULLSCREEN)
        {
            oam = &gOamObjects[OAM_IDX_TEXT_FULLSCREEN];
            for(i = 32; i < ARRAY_COUNT(gTextBoxCharacters); i++)
            {
                oam->attr0 = SPRITE_ATTR0_CLEAR;
                oam++;
            }
        }
    }
}

void RedrawTextboxCharacters()
{
    u32 i;
    u8 * src;
    u8 * dst;
    for(i = 0; i < ARRAY_COUNT(gTextBoxCharacters); i++)
    {
        struct TextBoxCharacter *theCharacter = &gTextBoxCharacters[i];
        if(theCharacter->state & 0x8000)
        {
            u32 temp = theCharacter->state & 0x7FFF;
            temp *= 0x80;
            temp += (u32)gCharSet; //! why tho
            if(theCharacter->color)
            {
                u32 j;
                u8 * tileBuf;
                u32 colorIdx;
                DmaCopy16(3, (void*)temp, gTextColorTileBuffer, 0x80);
                tileBuf = gTextColorTileBuffer;
                temp = theCharacter->color * 3;
                for(j = 0; j < 0x80; j++)
                {
                    u32 half1, half2;
                    half2 = *tileBuf; // honestly wtf capcom
                    half1 = half2 & 0xF;
                    half2 = half2 & 0xF0;
                    if(half1)
                        half1 += temp;
                    if(half2)
                        half2 += temp << 4;
                    *tileBuf++ = half1 | half2;
                }
                src = gTextColorTileBuffer;
                dst = (u8*)(OBJ_VRAM0 + i * 0x80);
                DmaCopy16(3, src, dst, 0x80);
            }
            else
            {
                dst = (u8*)(OBJ_VRAM0 + i * 0x80);
                DmaCopy16(3, (void*)temp, dst, 0x80);
            }
        }
    }
}

static void DrawCharacter(Font font, int x, int y, int color, int size, int character);

extern const int codepoints[];// = U"０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ！？あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをんがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇぉゃゅょっアイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォャュョッヴ.☞「」（）『』“”▼▲:、，+/*\'―・。％‥～《》&☆♪　ー記緑中断選消去初期状態戻追加逆転日目法廷姉妹探偵最終手今血気千尋所長無事発言証拠品特異議認弁護人慎重失敗残念私見得与違示関係全然考裁判心悪思明顔説力欠感却下誰月午前時分地方被告第控室間小学校級以来部舞台殺件度胸依頼借実士意味耳助何叫死成歩堂矢張有罪刑女生教害犯新聞書名龍一ヶ回若性逮捕不運彼男政志大親友年巻込知開検察側準備完了困進本当質問答綾里帰宅急便届常識落着次者調読忘好高美佳通用自信命因毒飲鈍器殴首絞冷独審理始亜内具体的凶置物受提出武呼話情報逃反撃祈配近世紀向電会況際遊海外旅行亡国付収入金援口走他天動機移屋正直合図送静粛留守結局我々立覚予決定早現場勧誘山野星雄員半腰抜怖警公衆昼停使種子番暴食含妙怒閉登同押切流致傷触玄上携帯持皆後絶対解剖単憶指摘詰音計続型引題刻裏捜査揺待確取交鳴驚瞬声強烈印象細主要池隣住頃別貸遅氏連想由差変先緊救真形式過渡空巣習家狙訪仕侵色作個製角夜祝興幕料払再起約束永久必身柄司預等忙元慣扱械類晩務打店律集油遠沈黙寝課客梅署悟宵放眠面座革絵画並棚整領活役弱暗欲散某官禁止勝鏥糸鋸圭介例松竹観葉植恐片姿古映泣果働即楽担御剣任怜侍獲段涙負歳黒憎才酷偽返号酸江戸戦倒破紙文字筆跡鑑悲相退水少肩抱速板東奪庫頭奥昨業窓帳簿数専針資能視安飛騒願昔両絡談監看朝修装霊媒師系接挟族父母代宿滅極秘笑捨二孤唯存在化粧造花泊夏降丸苦赤推申様伝株社偶盗聴影宇宙値深檀純突億売買冒論点余揮職轄位可迷席投容怪到標語迅和服順疑析注否液給多利右翌令劇盛髪攻弟為低省壁納験拒筋保管君練表求素刺氷溶条材道忠招参婦拝組工許横円共隠白冊項脅迫寄虫治格協写足応息宣誓処箱効乗脱獄球冗増途設序巨枚机青春商握像供輩愛礼老訴未歴史茶界望鏡超背肉曲線賞優秀努階育似迎至区府密基罰襲左英墓穴掘秩乱休憩固傍減訂砕雲掛週紋義万也圧延財腕焼満闇復根殿賃串荷三郎衣袋夢責軽撮都描夕敵闘繰広毎町米門馬将仲燃故荒操汁湯風統倉車坂排鋼皿割城洋菓痛浮木危鼻折経営般簡権裂貼鋭策訊景富光射俳棋幸秒離暑抵抗索済熱狂討督壊歌飯九太滝浅底限雑誌爆拓萌垂創編核姫神汗恩制緒震園栄林価換建免周炉咲鉄柵壇頑丈演節陽算珍喪恨飢短芸術忍柱薬睡案越臨宝平宮団試拾原末踏縁脚積叩凍頂奇讐湖浴防庁支非床銃零森刃阪研究群沢濃西市民拡砲恋悔喜雪夫冬詩寒趣産祖舗火六吐敬尾避狩魔豪倍匠愚央雷誤弾臓拳痕距侮辱適挑封拘双眼晴盤波岸述釈請犬魚釣栓旗属額辺洗健兵紹訓土埋詳兄寿庵毛十響概館灰脳畔桜録吹競技涼包囲殉厚濯量肺撤石替泳費筒授委飼銀鬼惑尊充遂病院井精悩吸徹執暇玉勉労略婚街盲呂謎則争症賢甘挙札寮医惜良比涯葬廊暮丼伸駅■企巧舟岩辰谷規之藤杉雅敦寺田裕稲総抹贈版";

void DrawTextGS(Font font) {
    //if(textbox.finished) {
    //    if((textbox.counter++ % 5) == 0)
    //        textbox.counter2++;
    //    DrawTexturePro(textbox.miscElementsTex, (Rectangle){0, ((textbox.counter2 % 3) + 3) * 16, 16, 16}, (Rectangle){456, 296, 16, 16}, (Vector2){0, 0}, 0.0f, WHITE);
    //}
    struct ScriptContext * scriptCtx = &gScriptContext;
    Vector2 position = {18, 240};
    int fontSize = 32;
    //int spacing = 0;
    //int textOffsetY = 0;            // Offset between lines (on line break '\n')
    //float textOffsetX = 0.0f;       // Offset X to next character to draw

    //float scaleFactor = fontSize/font.baseSize;         // Character quad scaling factor
    if(gMain.showTextboxCharacters)
    {
        for(int i = 0; i < OAM_COUNT_TEXT; i++)
        {
            
            u32 temp = gTextBoxCharacters[i].state & 0x7FFF;
            int index = GetGlyphIndex(font, codepoints[temp]);
            int character = codepoints[temp];
            if(gTextBoxCharacters[i].state & 0x8000)
            {
                DrawCharacter(font, gTextBoxCharacters[i].x + scriptCtx->textXOffset, gTextBoxCharacters[i].y + scriptCtx->textYOffset, gTextBoxCharacters[i].color, fontSize, character);
                //oam->attr0 = SPRITE_ATTR0(gTextBoxCharacters[i].y + scriptCtx->textYOffset, ST_OAM_AFFINE_OFF, ST_OAM_OBJ_NORMAL, FALSE, ST_OAM_4BPP, ST_OAM_SQUARE);
                //oam->attr1 = SPRITE_ATTR1_NONAFFINE(gTextBoxCharacters[i].x + scriptCtx->textXOffset, FALSE, FALSE, 1);
                //oam->attr2 = gTextBoxCharacters[i].objAttr2;
            }
            //else {

                //oam->attr0 = SPRITE_ATTR0_CLEAR;
            //}
            //oam++;
        }   
        if(scriptCtx->flags & SCRIPT_FULLSCREEN)
        {
            //oam = &gOamObjects[OAM_IDX_TEXT_FULLSCREEN];
            for(int i = 32; i < ARRAY_COUNT(gTextBoxCharacters); i++)
            {
                u32 temp = gTextBoxCharacters[i].state & 0x7FFF;
                int index = GetGlyphIndex(font, codepoints[temp]);
                int character = codepoints[temp];
                if(gTextBoxCharacters[i].state & 0x8000)
                {
                    DrawCharacter(font, gTextBoxCharacters[i].x + scriptCtx->fullscreenTextXOffset, gTextBoxCharacters[i].y + scriptCtx->fullscreenTextYOffset, gTextBoxCharacters[i].color, fontSize, character);
                    ///oam->attr0 = SPRITE_ATTR0(gTextBoxCharacters[i].y + scriptCtx->fullscreenTextYOffset, ST_OAM_AFFINE_OFF, ST_OAM_OBJ_NORMAL, FALSE, ST_OAM_4BPP, ST_OAM_SQUARE);
                    //oam->attr1 = SPRITE_ATTR1_NONAFFINE(gTextBoxCharacters[i].x + scriptCtx->fullscreenTextXOffset, FALSE, FALSE, 1);
                    //oam->attr2 = gTextBoxCharacters[i].objAttr2;
                }
                //else
                //    oam->attr0 = SPRITE_ATTR0_CLEAR;
                //oam++;
            }
        }

        //if (font.glyphs[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width*scaleFactor + spacing);
        //else textOffsetX += ((float)font.glyphs[index].advanceX*scaleFactor + spacing);
    }
    //DrawTextCodepoints(font, textbox.text, textbox.curend, (Vector2){18, 240}, 26, 0, WHITE);

}

static void DrawCharacter(Font font, int x, int y, int color, int size, int character) {
    if ((character != ' ') && (character != '\t'))
    {
        static const Color colors[] = {
            WHITE,
            RED,
            SKYBLUE,
            GREEN,

        };
        DrawTextCodepoint(font, character, (Vector2){ x*2, y*2 }, size, colors[color]);
    }
}