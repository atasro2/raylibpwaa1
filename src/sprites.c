#include "global.h"
#include "animation.h"

#include <stdio.h>
extern const struct SpriteSizeData gSpriteSizeTable[0xF];

void DrawSprites(void) {
    for(int i = 127; i >= 0; i--) {
        if(gOamObjects[i].attr0 & (ST_OAM_AFFINE_ERASE << 8))
            continue;

        int oamX = gOamObjects[i].attr1 & 0x1FF;
        int oamY = gOamObjects[i].attr0 & 0xFF;
        if(oamY > 160 + 16) {
            oamY -= 256;
        }
        if(oamX > 256) {
            oamX -= 512;
        }
        //struct {signed int x:9; signed int y:8;} s;
        int width, height;
        
        if(gOamObjects[i].width == 0 && gOamObjects[i].height == 0) {
            int magic = ((gOamObjects[i].attr0 & 0xC000) >> 14) | ((gOamObjects[i].attr1 & 0xC000) >> 12);
            width = gSpriteSizeTable[magic].width;
            height = gSpriteSizeTable[magic].height;
            //printf("%d %d\n", width, height);
        } else {
            width = gOamObjects[i].width;
            height = gOamObjects[i].height;
        }
        if(gOamObjects[i].attr1 & (1 << 12)) { // h flip
            width = -width;
        }
        if(gOamObjects[i].attr1 & (1 << 13)) { // v flip
            height = -height;
        }
        if(!IsTextureValid(gOamObjects[i].texture)) {
            DrawRectangle(oamX*2, oamY*2, width*2, height*2, MAGENTA);
            continue;
        }
        DrawTexturePro(gOamObjects[i].texture,
                       (Rectangle){gOamObjects[i].texU, gOamObjects[i].texV, width, height},
                       (Rectangle){oamX*2, oamY*2, width*2, height*2}, (Vector2){0,0}, 0, WHITE);
    }
}

/*
void SetSpriteTextureFromFile(int oamidx, char * filename) {
    struct OamAttrs * oam = gOamObjects[oamidx];
    if(!(oam->attr0 & (ST_OAM_AFFINE_ERASE << 8)) && IsTextureValid(oam->texture)) {// safety net check
    UnloadTexture(oam->texture);
}
oam->texture = LoadTexture(filename);
}
*/

void UnloadSpriteData(int oamidx) {
    struct OamAttrs * oam = &gOamObjects[oamidx];
    if(IsTextureValid(oam->texture)) { // ! we can't guarantee that the texture is invalid, we have this as a failsafe
        UnloadTexture(oam->texture);
        oam->texture = (Texture2D){};
        oam->width = 0;
        oam->height = 0;

        // TODO: Deprecate gba attributes
        oam->attr2 = 0;
        oam->attr1 = 0;
        oam->attr0 = 0x200;
    }
}