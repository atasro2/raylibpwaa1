#ifndef GUARD_ANIMATION_YAML_H
#define GUARD_ANIMATION_YAML_H

struct YMLSprite {
	int part;
	int x;
	int y;
	int palette;
	struct YMLSprite * next;
};

struct YMLArrangement {
	struct YMLSprite * sprites; // LL Head
	char * key;
	struct YMLArrangement * next;
};

struct YMLAction {
    int enabled;
    int id;
};

struct YMLFrame {
	int duration;
	struct YMLAction sfx;
	struct YMLAction action;
	char * arrangement;
	struct YMLFrame * next;
};

struct YMLAnimation {
	char * sheet;
	struct YMLArrangement * arrangements; // LL Head
	struct YMLFrame * frames; // LL Head
};

struct YMLPart {
    int x;
    int y;
    int w;
    int h;
    struct YMLPart * next;
};

struct YMLSheet {
    char * gfx;
    int width;
    int height;
	struct YMLPart * parts; // LL Head
};

struct YMLArrangement * getArrangement(struct YMLAnimation * anim, char * key);

#endif