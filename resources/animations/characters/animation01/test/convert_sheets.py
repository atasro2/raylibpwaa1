import sys, os
from PIL import Image
from pathlib import Path
import rpack
import glob
import binascii
import yaml

try:
    from yaml import CLoader as yamlLoader
except ImportError:
    from yaml import Loader as yamlLoader
    
from argparse import ArgumentParser


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

def part_palette(part, image):
    return image.getpixel((part.x, part.y)) // 16

def part_to_tiles(part, image):
    assert(part.w % 8 == 0 and part.h % 8 == 0)
    # TODO: this should assert to GBA sprite sizes
    tiles = bytearray((part.w*part.h) // 2)
    for y in range(part.h):
        for x in range(part.w//2):
            # mod of 16 because we don't care about which palette index we currently use
            pixel_l = image.getpixel((part.x+x*2, part.y+y)) % 16
            pixel_r = image.getpixel((part.x+x*2+1, part.y+y)) % 16
            xx = (x // 4)
            yy = (y // 8)
            tileoff = (xx+yy*(part.w//8))*32
            tiles[tileoff + (y % 8) * 4 + x % 4] = pixel_l | (pixel_r << 4)
    return tiles
    
def read_sheet_yaml_file(path):
    with open(path) as f:
        data = yaml.safe_load(f)
    return data
    
def read_anim_yaml_file(path):
    with open(path) as f:
        data = yaml.safe_load(f)
    return data
    
def rgb32_to_rgb555(r, g, b, a):
    # TODO: (c * 31) / 255 do this because division dumb but for match atm :( 
    return (r // 8) | ((g // 8) << 5) | ((b // 8) << 10) | ((a // 255) << 15)

parser = ArgumentParser()
parser.add_argument('-s', '--sheets', dest='sheets', required=True, nargs='+', help="sheet yamls to be processed")
parser.add_argument('-a', '--anims', dest='anims', required=True, nargs='+', help="animation ymls to be processed")
parser.add_argument("-o", "--output", dest="outfile", required=True,
                    help="Output animation file name",)

args = parser.parse_args()

pixdata = {}
with open(args.outfile + ".pix", "wb") as f:
    sheetoffset = 0
    for sheet in args.sheets:
        size = 0
        sheet_name = os.path.splitext(os.path.basename(sheet))[0]
        
        p = Path(sheet)
        sheetyml = read_sheet_yaml_file(sheet)
        #print(sheetyml)
        image = Image.open(p.parent / sheetyml["gfx"])
        palette = image.getpalette()
        if palette is None:
            sys.exit("Provided image does not have a color palette")
        assert(len(palette) % (16*4) == 0)
        pal_count = len(palette) // (16*4)
        print(f"Image has {pal_count} palette(s)")
        #print(bytes(palette))
        
        pixdata[sheet_name] = {}
        pixdata[sheet_name]["offset"] = sheetoffset
        
        f.write(pal_count.to_bytes(4, byteorder='little'))
        size += 4
        for i in range(pal_count*16):
            f.write(rgb32_to_rgb555(palette[i*4], palette[i*4+1], palette[i*4+2], palette[i*4+3]).to_bytes(2, byteorder='little'))
            size += 2
        pixdata[sheet_name]["parts"] = []
        pixdata[sheet_name]["part_size"] = []
        #i = 0
        for part in sheetyml["parts"]:
            tile_pal = part_palette(Part(part['x'],part['y'],part['w'],part['h']), image)
            tile_bytes = part_to_tiles(Part(part['x'],part['y'],part['w'],part['h']), image)
            pixdata[sheet_name]["parts"].append((size - 4 - pal_count*32, tile_pal))
            pixdata[sheet_name]["part_size"].append((part['w'],part['h']))
            f.write(tile_bytes)
            size += len(tile_bytes)
            #i+=1
        sheetoffset += size
print(pixdata)

with open(args.outfile + ".seq", "wb") as f:   
    for anim in args.anims:
        print(anim)
        animyml = read_anim_yaml_file(anim)
        sheetmeta = pixdata[animyml["sheet"]]
        framecount = len(animyml["frames"]) << 16
        f.write(framecount.to_bytes(4, byteorder='little'))
        f.write(sheetmeta["offset"].to_bytes(4, byteorder='little'))
        # build arrangements first
        frameBytes = bytearray()
        arrangementBytes = bytearray()
        arrangementOffsets = {}
        for arrangement, sprites in animyml["arrangements"].items():
            arrangementOffsets[arrangement] = len(arrangementBytes) + len(animyml["frames"]) * 8 + 8
            if sprites is None:
                continue
            arrangementBytes += len(sprites).to_bytes(4, byteorder='little')
            for sprite in sprites:
                x = sprite["x"] - 120
                y = sprite["y"] - 80
                arrangementBytes.append(x & 0xFF)
                arrangementBytes.append(y & 0xFF)
                data = sheetmeta["parts"][sprite["part"]][0]//32
                # TODO: actually handle palette sizes properly and don't wing 1bit pal mode
                if "palette" in sprite:
                    data |= int(sprite["palette"]) << 11
                else:
                    data |= sheetmeta["parts"][sprite["part"]][1] << 11
                #print(sprite["part"])
                #print(len(sheetyml["parts"]))
                width = sheetmeta["part_size"][sprite["part"]][0]
                height = sheetmeta["part_size"][sprite["part"]][1]
                sizetable = { \
                   '8x8': 0, '8x16': 1, '16x8': 2, \
                   '16x16': 4, '8x32': 5, '32x8': 6, \
                   '32x32': 8, '16x32': 9, '32x16': 10, \
                   '64x64': 12, '32x64': 13, '64x32': 14, \
                }
                data |= sizetable[f'{height}x{width}'] << 12
                arrangementBytes += data.to_bytes(2, byteorder='little')
        
        #print(binascii.hexlify(arrangementBytes, " "))
        #print(arrangementOffsets)
        for frame in animyml["frames"]:
            frameBytes += arrangementOffsets[str(frame["arrangement"])].to_bytes(2, byteorder='little')
            frameBytes.append(frame["duration"])
            sfx = 0
            action = 0
            flag = 0
            if "flag10" in frame and frame["flag10"]: # possibly internal xflip flag
                flag |= 0x10
            if "flag40" in frame and frame["flag40"]:
                flag |= 0x40
            if "sfx" in frame and frame["sfx"] is not None:
                if "id" in frame["sfx"]:
                    sfx = frame["sfx"]["id"]
                    flag |= 2
                if "enabled" in frame["sfx"] and not frame["sfx"]["enabled"]:
                    flag &= ~2
            if "action" in frame and frame["action"] is not None:
                if "id" in frame["action"]:
                    sfx = frame["action"]["id"]
                    flag |= 4
                if "enabled" in frame["action"] and not frame["action"]["enabled"]:
                    flag &= ~4
            frameBytes.append(flag)
            frameBytes.append(sfx)
            frameBytes.append(action)
            frameBytes += bytearray([0,0])
        #print(binascii.hexlify(frameBytes, " "))
        f.write(frameBytes)
        f.write(arrangementBytes)
#struct AnimationFrame
#{
#    /* +0x00 */ u16 spriteDataOffset;
#    /* +0x02 */ u8 frameDuration;
#    /* +0x03 */ u8 flags;
#    /* +0x04 */ u8 songId;
#    /* +0x05 */ u8 action;
#    /* +0x06 */ u8 filler6[2];
#};

#- duration: 100
#  flag10: False
#  flag40: False
#  sfx:
#  action:
#  arrangement: 88