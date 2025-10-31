import sys
from PIL import Image
from pathlib import Path
import rpack
import glob
import binascii
import yaml

class SpriteTemplate:
    def __init__(self, x, y, tile_id, width, height, palette):
        self.x = x
        self.y = y
        self.tile_id = tile_id
        self.width = width
        self.height = height
        self.palette = palette
    @staticmethod
    def from_bytes(b, paletteMode):
        if len(b) != 4:
            raise RuntimeError(f'Data has bad length {len(b)} (expected 4)')
        offsetX = b[0] - 256 if b[0] > 127 else b[0] 
        offsetY = b[1] - 256 if b[1] > 127 else b[1]

        data = b[2] | (b[3] << 8)
        if paletteMode == 1:
            tileid = data & 0x7FF
            palette = (data & 0x800) >> 11
        elif paletteMode == 2:
            tileid = data & 0x3FF
            palette = (data & 0xC00) >> 10
        elif paletteMode == 3:
            print("palette mode 3")
            tileid = data & 0x1FF
            palette = (data & 0xE00) >> 9
        else:
            raise RuntimeError(f'palette mode for arrangement is invalid')
        
        shapeSize = (data & 0xF000) >> 12

        # These sprite sizes are invalid 
        assert(shapeSize != 3 and shapeSize != 7 and shapeSize != 11 and shapeSize != 15)
        
        idk = [ \
               [8,8],[8,16],[16,8],[0,0], \
               [16,16],[8,32],[32,8],[0,0], \
               [32,32],[16,32],[32,16],[0,0], \
               [64,64],[32,64],[64,32],[0,0], \
              ] 
        
        width = idk[shapeSize][1]
        height = idk[shapeSize][0]

        return SpriteTemplate(offsetX, \
                              offsetY, \
                              tileid, \
                              width, \
                              height, \
                              palette)

    def __repr__(self):
        return f'SpriteTemplate({self.x}, {self.y}, {self.tile_id:X}, {self.width}, {self.height}, {self.palette})'

    def __str__(self):
        return f'{self.x}:{self.y}:{self.tile_id}:{self.width}:{self.height}:{self.palette}'
    def __lt__(self, other):
        return self.height*self.width > other.height*other.width
        
class AnimationFrame:
    def __init__(self, arrangement, duration, palette_bits, flag10, flag40, sfx, action):
        self.arrangement = arrangement
        self.duration = duration
        self.palette_bits = palette_bits
        self.flag10 = flag10
        self.flag40 = flag40
        self.sfx = sfx
        self.action = action
    
    @staticmethod
    def from_bytes(b):
        if len(b) == 8:
            if (b[6] != 0 or b[7] != 0):
                raise RuntimeError(f'Non-zero padding {b[6:8]} in animation frame?!')
        elif len(b) != 6:
            raise RuntimeError(f'Data has bad length {len(b)} (expected 6 or 8)')
        arrangement_offset = int.from_bytes(b[0:2], 'little')
        duration = b[2]
        flags = b[3]
        sfx = b[4]
        action = b[5]
        
        palette_bits = None
        if (flags & 1) != 0:
            assert (flags & 8) == 0
            palette_bits = 3
        elif (flags & 8) != 0:
            # Technically this only becomes a thing in GS2.
            # GS1 doesn't support 2-bit palette counts
            palette_bits = 2
        else:
            palette_bits = 1

        if (flags & 2) != 0:
            sfx = { 'enabled': True, 'id': sfx }
        elif sfx != 0:
            sfx = { 'enabled': False, 'id': sfx }
        else:
            sfx = {}

        if (flags & 4) != 0:
            action = { 'enabled': True, 'id': action }
        elif action != 0:
            action = { 'enabled': False, 'id': action }
        else:
            action = {}
            
        # TODO: What is this flag??? animation37.seq sets it for all animation frames
        flag10 = (flags & 0x10) != 0
        #if flag10:
        #    print("flag10-----------------------------------------------------------------------------------")
        # TODO: What is THIS flag? Phoenix's animation AA0 sets it for all animation frames
        flag40 = (flags & 0x40) != 0

        # Check that we're not dropping flags or fields on the floor
        assert (flags & 0xA0) == 0
        return AnimationFrame(arrangement_offset, \
                              duration, \
                              palette_bits, \
                              flag10, \
                              flag40, \
                              sfx, \
                              action)

    def __repr__(self):
        return f'AnimationFrame({self.arrangement}, {self.duration}, {self.palette_bits}, {self.flag10}, {self.flag40}, {self.sfx}, {self.action})'

    def __str__(self):
        return f'{self.arrangement}:{self.duration}:{self.palette_bits}:{self.flag10}:{self.flag40}:{self.sfx}:{self.action}'

class Part():
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
    def __repr__(self):
        return f'Part({self.x}, {self.y}, {self.w}, {self.h})'
    def __str__(self):
        return f'{self.x}:{self.y}:{self.w}:{self.h}'
    
class Sheet():
    def __init__(self, gfx, sprites):
        self.gfx = gfx # Path to graphics file
        self.sprites = sprites # List of sprite parts

    def __repr__(self):
            return f'Sheet({self.gfx}, {self.sprites})'
    #def __repr__(self):
    #    return f'AnimationFrame({self.arrangement}, {self.duration}, {self.palette_bits}, {self.flag10}, {self.flag40}, {self.sfx}, {self.action})'

    #def __str__(self):
    #    return f'{self.arrangement}:{self.duration}:{self.palette_bits}:{self.flag10}:{self.flag40}:{self.sfx}:{self.action}'

tile_palette = [[255]*32 for i in range(32)]
def findInSheet(animname, sheetid, pix, pixoff, width, height, palette):
    sheet = None
    with open(f"{animname}/{sheetid}.bin", 'rb') as f:
        sheet = f.read()
    failedMatches = []
    mismatchPalCnt = 0
    result = True
    for y in range(32-height+1):
        for x in range(32-width+1):
            result = True
            palette_match = True
            for yy in range(height):
                for xx in range(width):
                    soff = (yy*width+xx)*32
                    offset = ((y+yy)*32+(x+xx))*32
                    #if sheetid == 6 and x == 6 and y == 28:
                    #    print(binascii.hexlify(sheet[offset:offset+32]))
                    #    print(binascii.hexlify(pix[pixoff+soff:pixoff+soff+32]))
                    if (palette != tile_palette[y+yy][x+xx] and tile_palette[y+yy][x+xx] != 255):
                        palette_match = False # TODO: technically we match to absolutely nothing as well, so there might still be some issues especially with full transparency  *sigh*
                    if sheet[offset:offset+32] != pix[pixoff+soff:pixoff+soff+32]:# or (palette != tile_palette[y+yy][x+xx] and tile_palette[y+yy][x+xx] != 255):
                        result = False
                        if yy*xx != 0:
                            failedMatches.append((x, y, xx, yy))
                        break
                if result == False:
                    break
            if result == True:
                return {'pos': (x*8, y*8), 'palette_match': palette_match}
    print("---------------------------")
    print(failedMatches)

class Animation():
    def __init__(self, name, frames, arrangements, sheet):
        self.name = name
        self.frames = frames
        self.arrangements = arrangements
        self.sheet = sheet
        
def readAnimSeq(seq, name, pixName = None):
    # i = address of a given animation
    i = 0
    pix = None
    if pixName == None:
        with open(f"{name}.pix", 'rb') as f:
            pix = f.read()
    else:
        with open(pixName, 'rb') as f:
            pix = f.read()
    anims = []
    sheets = {}
    while i < len(seq):
        #print(f'Reading animation @ {i:X}...')
        header_00 = int.from_bytes(seq[i:i+2], 'little')
        if header_00 != 0:
            # These two bytes would be the count of OBJs in the arrangements. If this is
            # set, we're probably looking at an arrangement and not the beginning of an
            # animation.
            raise RuntimeError(f'animation @ {i:X}: header_00 is {header_00}, not 0')
        anim_frame_count = int.from_bytes(seq[i+2:i+4], 'little')
        gfx_offset = int.from_bytes(seq[i+4:i+8], 'little')
        if gfx_offset not in sheets:
            sheets[gfx_offset] = {}
        #print(f"graphics offset {gfx_offset:X}")
        palette_count = int.from_bytes(pix[gfx_offset+0:gfx_offset+4], 'little')

        # Sprites that are not referenced in any frames will have a 1 bit palette
        animation_arrangements = []
        animation_frames = []
        animation_a = []
        # Read all of the frames making up the animation
        referenced_arrangement_offsets = set()
        for j in range(anim_frame_count):
            frame = AnimationFrame.from_bytes(seq[i+8+(j*8):i+8+(j*8)+8])
            arrangement_file_offset = i + frame.arrangement
            obj_count = int.from_bytes(seq[arrangement_file_offset:arrangement_file_offset+2], 'little')
            #print(obj_count)
            #print(arrangement_file_offset)
            #print(frame.arrangement)
            fuck = []
            for k in range(obj_count):
                #print(seq[arrangement_file_offset+4+4*k:arrangement_file_offset+4+4*k+4])
                template = SpriteTemplate.from_bytes(seq[arrangement_file_offset+4+4*k:arrangement_file_offset+4+4*k+4], frame.palette_bits)
                fuck.append(template)
                animation_arrangements.append(template)
                if template.tile_id not in sheets[gfx_offset]:
                    sheets[gfx_offset][template.tile_id] = template
                
            referenced_arrangement_offsets.add((frame.arrangement, frame.palette_bits))
            frame.sprites = fuck
            animation_frames.append(frame)
        del j
        referenced_arrangement_offsets = list(referenced_arrangement_offsets)
        referenced_arrangement_offsets.sort()
        # Ensure there's no data between the animation frames and the arrangement data
        assert i + referenced_arrangement_offsets[0][0] == i + 8 + anim_frame_count * 8
        #print(animation_frames)
        #print(referenced_arrangement_offsets)
        
        anim = Animation(f"{i:X}", animation_frames, {}, f"{gfx_offset:X}")
        anims.append(anim)

        # Go over all the arrangements, but don't dump them quite yet
        # We can't rely on all arrangements being referenced as part of an AnimationFrame;
        # loop until we find an arrangement with a length of 0 OBJs instead, because that
        # will be the start of the next animation!
        arrangement_offset = 0
        anim_initial_data_size = 8 + anim_frame_count * 8
        actual_arrangement_offsets = []
        actual_animation_arrangements = []
        # ...Or loop until we hit the end of the file. != is intentional, I want an exception
        # if it advances too far and indexes out of bounds while I'm not actually reading the
        # data
        while i + anim_initial_data_size + arrangement_offset != len(seq):
            anim.arrangements[anim_initial_data_size + arrangement_offset] = []
            arrangement_file_offset = i + anim_initial_data_size + arrangement_offset
            obj_count = int.from_bytes(seq[arrangement_file_offset:arrangement_file_offset+2], 'little')
            if obj_count == 0:
                break
            assert int.from_bytes(seq[arrangement_file_offset+2:arrangement_file_offset+4], 'little') == 0
            actual_arrangement_offsets.append(anim_initial_data_size + arrangement_offset)
            shit = [x for x in referenced_arrangement_offsets if x[0] == anim_initial_data_size + arrangement_offset]
            print(shit)
            shit.append((0,1))
            for j in range(obj_count):
                arrangement = SpriteTemplate.from_bytes(seq[arrangement_file_offset+4+4*j:arrangement_file_offset+8+4*j], shit[0][1]) # TODO: How do we figure out sprite templates that don't have a clue for the palette bit? user input?
                actual_animation_arrangements.append(arrangement)
                anim.arrangements[anim_initial_data_size + arrangement_offset].append(arrangement)
            
            #print(animation_arrangements)   
            arrangement_offset += 4 + obj_count * 4
        animation_arrangements.sort()
        actual_animation_arrangements.sort()
        
        tile_offset = gfx_offset + 4 + palette_count * 32
        bigX = 0

        for sprite in actual_animation_arrangements:
            if sprite.tile_id not in sheets[gfx_offset]:
                sheets[gfx_offset][sprite.tile_id] = sprite
            #bigX = max(bigX, sprite.width)
            #sheets[gfx_offset]["maxY"] += sprite.height
        # animation24.seq's last frame has the offset of the end of the animation... So, uh,
        # add it here so the subset check doesn't crash
        actual_arrangement_offsets.append(anim_initial_data_size + arrangement_offset)
        #print(actual_arrangement_offsets)
        #assert set(referenced_arrangement_offsets) <= set(actual_arrangement_offsets)

        i += anim_initial_data_size + arrangement_offset

    for gfx_offset, v in sheets.items():
        for tile_id, sprite in v.items():
            for tile_id2, sprite2 in v.items():
                if sprite2 == sprite:
                    continue
                tileArea = sprite2.width//8*sprite2.height//8
                if tile_id2 <= tile_id and tile_id2+tileArea > tile_id:
                    del v[tile_id]
                    print("-------------------------------------------------------------------------")

                #list(frame.keys(anim.sheet)).index()
    sheetcnt = 0
    for gfx_offset, v in sorted(sheets.items()):
        for i in range(32):
            for j in range(32):
                tile_palette[i][j] = 255
        palette_count = int.from_bytes(pix[gfx_offset+0:gfx_offset+4], 'little')
        tile_offset = gfx_offset + 4 + palette_count * 32
        p = Path(f"{name}/")
        #sheets4bpp = glob.glob(p/'*.bin')
        p.mkdir(parents=True, exist_ok=True)
        image = Image.new("P", (256,256))
        pal = []
        for i in range(palette_count*16):
            color = int.from_bytes(pix[gfx_offset+4+i*2:gfx_offset+4+i*2+2], 'little') 
            r = color & 0x1F
            g = (((color >> 5) & 0x1F) << 1) | ((color >> 15) & 1)
            b = (color >> 10) & 0x1F
            pal.append((r * 255) // 31)
            pal.append((g * 255) // 63)
            pal.append((b * 255) // 31)
        image.putpalette(pal)
        image.putdata([255]*256*256)
        rects = [(sprite.width, sprite.height) for tile_id, sprite in sorted(v.items())]
        #print(len(rects))
        #print(len(positions))
        #print(v.items())
        parts = []
        i = 0
        print(f"writing sheet {gfx_offset:X}")
        for tile_id, sprite in sorted(v.items()):
            #print(f"finding tile {tile_id:X}")
            sheetPos = findInSheet(name, sheetcnt, pix, tile_offset+tile_id*32, sprite.width//8, sprite.height//8, sprite.palette)
            
            if sheetPos == None:
                positions = rpack.pack(rects, 256, 256)
                #exit()
                #preview = Image.new("P", (256,256))
                #preview.putpalette(pix[gfx_offset+4:gfx_offset+4+palette_count * 32], "RGB;15")
                #preview.putdata([15]*256*256)
                for y in range(sprite.height):
                    for x in range(sprite.width//2):
                        sheetpos = positions[i]
                        sprite.u = sheetpos[0]
                        sprite.v = sheetpos[1]
                        sp1 = image.getpixel((sprite.u+x*2+1,sprite.v+y))
                        sp2 = image.getpixel((sprite.u+x*2,sprite.v+y))
                        tx = (x // 4)
                        ty = (y // 8) * sprite.width//8
                        p4bpp = pix[tile_offset+(sprite.tile_id+tx+ty)*32 + (y % 8) * 4 + x % 4]
                        p1 = (p4bpp >> 4) & 0xF
                        p2 = p4bpp & 0xF
                        xx = sheetpos[0]+x*2
                        yy = sheetpos[1]+y
                        if xx >= 255 or xx < 0:
                            raise RuntimeError(f'tile_id 0x{sprite.tile_id:X} of {sprite.width}x{sprite.height} is too big to fit in {sheetpos[0]} {sheetpos[1]}')
                        if yy > 255 or yy < 0:
                            raise RuntimeError(f'tile_id 0x{sprite.tile_id:X} of {sprite.width}x{sprite.height} is too big to fit in {sheetpos[0]} {sheetpos[1]}')
                        #preview.putpixel((xx+1,yy), p1)
                        #preview.putpixel((xx,yy), p2)
                        if sp1 == 255:
                            image.putpixel((xx+1,yy), p1 + 16 * sprite.palette)
                        if sp2 == 255:
                            image.putpixel((xx,yy), p2 + 16 * sprite.palette)
                sprite.force_pal = True
                #preview.show()
            else:
                #for i in range(sprite.height//8):
                #    for j in range(sprite.width//8):
                #        tile_palette[sheetPos[1]//8+i][sheetPos[0]//8+j] = sprite.palette
                sprite.u = sheetPos['pos'][0]
                sprite.v = sheetPos['pos'][1]
                sheetbin = None
                with open(f"{name}/{sheetcnt}.bin", 'rb') as f:
                    sheetbin = f.read()
                for y in range(sprite.height):
                    for x in range(sprite.width//2):
                        sp1 = image.getpixel((sprite.u+x*2+1,sprite.v+y))
                        sp2 = image.getpixel((sprite.u+x*2,sprite.v+y))
                        if sp1 == 255:
                            tile_palette[(sprite.v+y)//8][(sprite.u+x*2+1)//8] = sprite.palette
                        if sp2 == 255: 
                            tile_palette[(sprite.v+y)//8][(sprite.u+x*2)//8] = sprite.palette
                        tx = (x // 4)
                        ty = (y // 8)
                        #tile_palette[ty][tx] = sprite.palette
                        tileIndex = sheetPos['pos'][0]//8 + sheetPos['pos'][1]//8 * 32
                        p4bpp = sheetbin[(tileIndex+tx+ty*32)*32 + (y % 8) * 4 + x % 4]
                        p1 = (p4bpp >> 4) & 0xF
                        p2 = p4bpp & 0xF
                        xx = sprite.u+x*2
                        yy = sprite.v+y
                        if xx >= 511 or xx < 0:
                            raise RuntimeError(f'tile_id 0x{sprite.tile_id:X} of {sprite.width}x{sprite.height} is too big to fit in {sheetpos[0]} {sheetpos[1]}')
                        if yy >= 511 or yy < 0:
                            raise RuntimeError(f'tile_id 0x{sprite.tile_id:X} of {sprite.width}x{sprite.height} is too big to fit in {sheetpos[0]} {sheetpos[1]}')
                        if sp1 == 255:
                            image.putpixel((xx+1,yy), p1 + 16 * sprite.palette)
                        if sp2 == 255:
                            image.putpixel((xx,yy), p2 + 16 * sprite.palette)
                sprite.force_pal = not sheetPos['palette_match']
            # We add the sprite as a part for our sheet
            pc = Part(sprite.u, sprite.v, sprite.width, sprite.height)
            parts.append(pc)
            pc.pal = sprite.palette
            i+=1
        cSheet = Sheet(f"sheet-{gfx_offset:X}.png", parts)
        with open(p / f"sheet-{gfx_offset:X}.yml", 'w') as ymlfile:
            #yaml.safe_dump(cSheet, ymlfile)
            ymlfile.write(f"gfx: {cSheet.gfx}\n")
            ymlfile.write(f"width: 256\n")
            ymlfile.write(f"height: 256\n")
            ymlfile.write(f"parts:\n")
            for part in parts:
                ymlfile.write(f"  - x: {part.x}\n" \
                              f"    y: {part.y}\n" \
                              f"    w: {part.w}\n" \
                              f"    h: {part.h}\n" \
                              f"    p: {part.pal}\n") # palette is only used for later dumping
                              
        image.save(p / f"sheet-{gfx_offset:X}.png", transparency = 0)
        sheetcnt+=1
        
    for anim in anims:
        p = Path(f"{name}/")
        p.mkdir(parents=True, exist_ok=True)
        with open(p / f"{anim.name}.yml", 'w') as ymlfile:
            ymlfile.write(f"sheet: sheet-{anim.sheet}\n")
            ymlfile.write(f"arrangements:\n")
            for arrangement, sprites in anim.arrangements.items():
                ymlfile.write(f"  '{arrangement}':\n")
                for sprite in sprites:
                    fuck = sorted(sheets[int(anim.sheet, 16)].keys()).index(sprite.tile_id)
                    ymlfile.write(f"  - part: {fuck}\n")
                    ymlfile.write(f"    x: {sprite.x+120}\n")
                    ymlfile.write(f"    y: {sprite.y+80}\n")
                    if sheets[int(anim.sheet, 16)][sprite.tile_id].force_pal:
                        ymlfile.write(f"    palette: {sprite.palette}\n")
            ymlfile.write(f"frames:\n")
            for frame in anim.frames:
                ymlfile.write(f"- duration: {frame.duration}\n")
                ymlfile.write(f"  flag10: {frame.flag10}\n")
                ymlfile.write(f"  flag40: {frame.flag40}\n")
                ymlfile.write(f"  sfx:\n")
                if bool(frame.sfx): # why does this work python?
                    ymlfile.write(f"    id: {frame.sfx['id']}\n")
                    ymlfile.write(f"    enabled: {frame.sfx['enabled']}\n")
                ymlfile.write(f"  action:\n")
                if bool(frame.action):
                    ymlfile.write(f"    id: {frame.action['id']}\n")
                    ymlfile.write(f"    enabled: {frame.action['enabled']}\n")
                ymlfile.write(f"  arrangement: {frame.arrangement}\n")
    with open(p / f"{name}.yml", 'w') as ymlfile:
        ymlfile.write(f"sheets:\n")
        for gfx_offset in sorted(sheets.keys()):
            ymlfile.write(f"- 'sheet-{gfx_offset:X}.yml'\n")
        ymlfile.write(f"anims:\n")
        for anim in anims:
            ymlfile.write(f"- '{anim.name}.yml'\n")
    #print(buildcmd + "\n")

#for i in range(1,27):
seq = None
with open(sys.argv[1], 'rb') as f:
    seq = f.read()
print(f"currently reading {sys.argv[1]} ")
readAnimSeq(seq, sys.argv[2], sys.argv[3])
    
# def main(args):
#     

# if __name__ == '__main__':
#     main(sys.argv)
