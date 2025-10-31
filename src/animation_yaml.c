#include "global.h"
#include "raylib.h"
#include "animation_yaml.h"
#include "animation.h"
#include "sound.h"
#include "script.h"

#include "lodepng.h"
#include <yaml.h>

#include <ctype.h>
#include <assert.h>
#include <errno.h>

struct YMLPart * getPart(struct YMLSheet * sheet, int idx) {
    struct YMLPart * part = sheet->parts;
    while(part != NULL && idx--) {
        part = part->next;
    }
    return part;
}

struct YMLArrangement * getArrangement(struct YMLAnimation * anim, char * key) {
    struct YMLArrangement * arr = anim->arrangements;
    while(arr != NULL) {
        if(strcmp(arr->key, key) == 0)
            return arr;
        arr = arr->next;
    }
    return NULL;
}

struct YMLSprite * newSpriteEx(struct YMLSprite ** head, int part, int x, int y, int palette) {
	struct YMLSprite ** nextNode;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLSprite));
	(*nextNode)->part = part;
	(*nextNode)->x = x;
    (*nextNode)->y = y;
    (*nextNode)->palette = palette;
    return *nextNode;
}

struct YMLSprite * newSprite(struct YMLSprite ** head) {
	struct YMLSprite ** nextNode;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLSprite));
    return *nextNode;
}

struct YMLPart * newPart(struct YMLPart ** head) {
	struct YMLPart ** nextNode;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLPart));
    return *nextNode;
}


struct YMLFrame * newFrame(struct YMLFrame ** head) {
	struct YMLFrame ** nextNode;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLFrame));
    return *nextNode;
}


struct YMLArrangement * newArrangement(struct YMLAnimation * anim, char * key) {
	struct YMLArrangement ** head;
	struct YMLArrangement ** nextNode;
    //printf("%p\n", anim);
	head = &anim->arrangements;
	nextNode = head;
    //printf("%p\n", nextNode);
	while(*nextNode != NULL) {
        //printf("%p\n", nextNode);
		nextNode = &((*nextNode)->next);
        //printf("next %p\n", *nextNode);
        
    }
    //printf("next %p\n", *nextNode);
	*nextNode = calloc(1, sizeof(struct YMLArrangement));
	(*nextNode)->key = strdup(key);
	(*nextNode)->sprites = NULL;
	(*nextNode)->next = NULL;
    return *nextNode;
}

/*
struct YMLAnimation * newAnimation(struct YMLAnimList * list) {
	struct YMLAnimation ** head;
	struct YMLAnimation ** nextNode;
	head = &list->animations;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLAnimation));
	(*nextNode)->sheet = NULL;
	(*nextNode)->frames = NULL;
	(*nextNode)->arrangements = NULL;
	(*nextNode)->next = NULL;
    return *nextNode;
}

struct YMLSheet * newSheet(struct YMLAnimList * list) {
	struct YMLSheet ** head;
	struct YMLSheet ** nextNode;
	head = &list->sheets;
	nextNode = head;
	while(*nextNode != NULL) {
		nextNode = &((*nextNode)->next);
	}
	*nextNode = calloc(1, sizeof(struct YMLSheet));
	(*nextNode)->gfx = NULL;
	(*nextNode)->parts = NULL;
	(*nextNode)->next = NULL;
    return *nextNode;
}
*/
void parser_error(yaml_parser_t * parser) {
    printf("Parser error %d, line %d\n", parser->error, parser->mark.line);
    exit(EXIT_FAILURE);
}
// assumes YAML_NO_EVENT == 0
static const int eventMapping[] = {
    [YAML_MAPPING_START_EVENT] = YAML_MAPPING_END_EVENT,
    [YAML_SEQUENCE_START_EVENT] = YAML_SEQUENCE_END_EVENT,
    [YAML_DOCUMENT_START_EVENT] = YAML_DOCUMENT_END_EVENT,
    [YAML_MAPPING_END_EVENT] = YAML_NO_EVENT // Last event to pad out the array properly
};

void parser_skip_event(yaml_parser_t * parser, int eventToSkip) {
    yaml_event_t event;
    int eventToWait = eventMapping[eventToSkip];
    if(eventToWait == YAML_NO_EVENT) {
        printf("Can't skip event %d on line %d\n", event.type, parser->mark.line);
        exit(EXIT_FAILURE);
    }
    do {
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);
        if(eventMapping[event.type] != YAML_NO_EVENT) {
            parser_skip_event(parser, event.type);
        }
        if(event.type != eventToWait)
            yaml_event_delete(&event);
    }
    while(event.type != eventToWait);
}

void parser_skip_value(yaml_parser_t * parser) {
    yaml_event_t event;
    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    if(event.type == YAML_SCALAR_EVENT) {
		yaml_event_delete(&event);
        return;
	}
    parser_skip_event(parser, event.type);
    yaml_event_delete(&event);
}

int parser_get_int(yaml_parser_t * parser) {
    yaml_event_t event;
    char * end;
    errno = 0;
    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    if(event.type != YAML_SCALAR_EVENT) {
		printf("Expected Scalar at line %d but got event %d while trying to parse number\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    char * value = event.data.scalar.value;
    int num = strtol(value, &end, 10);
    if(errno == ERANGE) {
        printf("Scalar at line %d cannot be parsed into number\n", parser->mark.line, event.type);
        exit(EXIT_FAILURE);
    }
    yaml_event_delete(&event);
    return num;
}

char * parser_get_string(yaml_parser_t * parser) {
    yaml_event_t event;
    
    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    if(event.type != YAML_SCALAR_EVENT) {
		printf("Expected Scalar at line %d but got event %d while trying to parse number\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    char * value = strdup(event.data.scalar.value);
    yaml_event_delete(&event);
    return value;
}

int getbool(char * str) {
    char * t = "true";
    char * f = "false";
    char * cmp;
    if(tolower(str[0]) == t[0]) {
        cmp = t;
    } else if(tolower(str[0]) == f[0]) {
        cmp = f;
    } else {
        return -1;
    }
    
    for(int i = 1; str[i]; i++){
        if(cmp[i] == 0)
            return -1;
        if(tolower(str[i]) != cmp[i]) return -1;
    }
    return cmp == t; // ensure null term and return true if 
}

struct YMLAction parser_get_action(yaml_parser_t * parser) {
    yaml_event_t event;
    struct YMLAction action = {0, 0};
    
    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
    if(event.type == YAML_SCALAR_EVENT && event.data.scalar.length == 0) { // ignore Empty action
        //printf("empty action on line %d\n", parser->mark.line);
        yaml_event_delete(&event);
        return action;
	}
    if(event.type != YAML_MAPPING_START_EVENT) {
		printf("Expected Mapping at line %d but got event %d while trying to parse number\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT:
            char * value = event.data.scalar.value;
            if(strcmp(value, "enabled") == 0) {
                yaml_event_delete(&event);
                if (!yaml_parser_parse(parser, &event))
                    parser_error(parser);
                if(event.type != YAML_SCALAR_EVENT) {
                    printf("Expected Scalar at line %d but got event %d while trying to parse number\n", parser->mark.line, event.type);
                    exit(EXIT_FAILURE);
                }
                int b = getbool(event.data.scalar.value);
                if(b < 0) {
                    printf("Expected Bool at line %d\n", parser->mark.line);
                    exit(EXIT_FAILURE);                    
                }
                action.enabled = b;
                //printf("enabled %d", b);
            } else if (strcmp(value, "id") == 0){
                action.id = parser_get_int(parser);
                //printf("id %d ", action.id);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            break;
        default:
            printf("\nline %d, event %d\n", parser->mark.line, event.type);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
    } while (event.type != YAML_MAPPING_END_EVENT);
    yaml_event_delete(&event);
    return action;
}

void handle_sprite_mapping(yaml_parser_t * parser, struct YMLSprite * sprite) {
	yaml_event_t event;
    do {
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT:
            char * value = event.data.scalar.value;
            if(strcmp(value, "part") == 0) {
                sprite->part = parser_get_int(parser);
                //printf("part %d ", sprite->part);
            } else if (strcmp(value, "x") == 0){
                sprite->x = parser_get_int(parser);
                //printf("x %d ", sprite->x);
            } else if (strcmp(value, "y") == 0){
                sprite->y = parser_get_int(parser);
                //printf("y %d ", sprite->y);
            } else if (strcmp(value, "palette") == 0){
                sprite->palette = parser_get_int(parser);
                //printf("palette %d ", sprite->palette);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            yaml_event_delete(&event);
            return;
        default:
            printf("\nline %d, event %d\n", parser->mark.line, event.type);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
        yaml_event_delete(&event);
    } while (event.type != YAML_MAPPING_END_EVENT);
    printf("we shouldn't be here\n");
    exit(EXIT_FAILURE);
}

void handle_sprites_sequence(yaml_parser_t * parser, struct YMLArrangement * arrangement) {
	yaml_event_t event;

	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);

	if(event.type != YAML_SEQUENCE_START_EVENT) {
        if(event.type != YAML_SCALAR_EVENT) {
            printf("Expected Sequnce at line %d but got event %d\n", parser->mark.line, event.type);
            exit(EXIT_FAILURE);
        }
        if(event.data.scalar.length == 0) {
            //printf("Empty sprites on line %d length == 0\n", parser->mark.line);
            yaml_event_delete(&event);
            return;
        }
        if(event.data.scalar.value[0] == 0) {
            //printf("Empty sprites on line %d str[0] == 0\n", parser->mark.line);
            yaml_event_delete(&event);
            return;
        }
	}

    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_MAPPING_START_EVENT: // We have a Squence of Sprite Mappings
            handle_sprite_mapping(parser, newSprite(&arrangement->sprites));
            //printf("handled sprite\n");
            break;
        case YAML_SEQUENCE_END_EVENT:
            yaml_event_delete(&event);
            return;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
    } while (event.type != YAML_SEQUENCE_END_EVENT);
}

void handle_arrangement_mapping(yaml_parser_t * parser, struct YMLArrangement * arrangement) {
	yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_MAPPING_START_EVENT) {
		printf("Expected Mapping at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT: // We get the arrangement's key from this scalar event
            char * value = event.data.scalar.value;
            if(strcmp(value, "pal_mode") == 0) { // Skip this
                yaml_event_delete(&event);
                if (!yaml_parser_parse(parser, &event))
                    parser_error(parser);
            } else if (strcmp(value, "sprites") == 0){
                handle_sprites_sequence(parser, arrangement);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            break;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }

    } while (event.type != YAML_MAPPING_END_EVENT);
    //printf("arr map finished\n");
    yaml_event_delete(&event);
}
    
void handle_arrangements_mapping(yaml_parser_t * parser, struct YMLAnimation * anim) {
	yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_MAPPING_START_EVENT) {
		printf("Expected Mapping at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT: // We get the arrangement's key from this scalar event
            char * value = event.data.scalar.value;
            struct YMLArrangement * arr = newArrangement(anim, value);
            handle_arrangement_mapping(parser, arr);
            break;
        case YAML_MAPPING_END_EVENT:
            break;
        default:
        printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }

    } while (event.type != YAML_MAPPING_END_EVENT);
    //printf("arrs map finished\n");
    yaml_event_delete(&event);
}

/*
struct YMLFrame {
	int duration;
	struct YMLAction sfx;
	struct YMLAction action;
	char * arrangement;
	struct YMLFrame * next;
};
*/

void handle_frame_mapping(yaml_parser_t * parser, struct YMLFrame * frame) {
	yaml_event_t event;
    do {
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT:
            char * value = event.data.scalar.value;
            if(strcmp(value, "duration") == 0) {
                frame->duration = parser_get_int(parser);
                //printf("duration %d\n", frame->duration);
            } else if (strcmp(value, "flag10") == 0){ // SKIPPED
                free(parser_get_string(parser)); // ! this is dumb
            } else if (strcmp(value, "flag40") == 0){ // SKIPPED
                free(parser_get_string(parser));
            } else if (strcmp(value, "sfx") == 0){
                frame->sfx = parser_get_action(parser);
                //printf("sfx enable %d, ", frame->sfx.enabled);
                //printf("sfx id %d\n", frame->sfx.id);
            } else if (strcmp(value, "action") == 0){
                frame->action = parser_get_action(parser);
                //printf("action enable %d, ", frame->action.enabled);
                //printf("action id %d\n", frame->action.id);
            } else if (strcmp(value, "arrangement") == 0){
                frame->arrangement = parser_get_string(parser);
                //printf("arrangement %s\n", frame->arrangement);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            yaml_event_delete(&event);
            return;
        default:
            printf("\nline %d, event %d\n", parser->mark.line, event.type);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
        yaml_event_delete(&event);
    } while (event.type != YAML_MAPPING_END_EVENT);
    printf("we shouldn't be here\n");
    exit(EXIT_FAILURE);
}

void handle_frames_sequence(yaml_parser_t * parser, struct YMLAnimation * anim){
    yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_SEQUENCE_START_EVENT) {
        printf("Expected Sequnce at line %d but got event %d\n", parser->mark.line, event.type);
        exit(EXIT_FAILURE);
	}
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_MAPPING_START_EVENT: // We have a Squence of frame mappings
            handle_frame_mapping(parser, newFrame(&anim->frames));
            //printf("handled frame\n");
            break;
        case YAML_SEQUENCE_END_EVENT:
            break;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
    } while (event.type != YAML_SEQUENCE_END_EVENT);
    //printf("finished frames\n");
    yaml_event_delete(&event);
}

void handle_anim_mapping(yaml_parser_t * parser, struct YMLAnimation * anim) {
    yaml_event_t event;
    do {
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT:
            char * value = event.data.scalar.value;
            if(strcmp(value, "sheet") == 0) {
                yaml_event_delete(&event);
                if(!yaml_parser_parse(parser, &event))
                    parser_error(parser);
                if(event.type != YAML_SCALAR_EVENT) {
                    printf("Expected Scalar at line %d\n", parser->mark.line);
                    exit(EXIT_FAILURE);
                }
                char * value = event.data.scalar.value;
                anim->sheet = strdup(value);
            } else if (strcmp(value, "arrangements") == 0) {
                handle_arrangements_mapping(parser, anim);
            } else if (strcmp(value, "frames") == 0) {
                handle_frames_sequence(parser, anim);
            } else {
                printf("Unknown Key %s\n", value);
                exit(EXIT_FAILURE);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            //printf("anim map finished\n");
            yaml_event_delete(&event);
            return;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
        yaml_event_delete(&event);
    } while (event.type != YAML_MAPPING_END_EVENT);

}

void handle_anim_yaml(yaml_parser_t * parser, struct YMLAnimation * anim) {
	yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_MAPPING_START_EVENT) {
		printf("Expected Mapping at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
	handle_anim_mapping(parser, anim);
    yaml_event_delete(&event);
    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    if(event.type != YAML_DOCUMENT_END_EVENT) {
        printf("Expected YAML_DOCUMENT_END_EVENT at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
    }
}

void freeSprite(struct YMLSprite * sprite) {
    if(sprite != NULL) {
        freeSprite(sprite->next);
        free(sprite);
    }
}

void freeArrangement(struct YMLArrangement * arr) {
    if(arr != NULL) {
        freeArrangement(arr->next);
        freeSprite(arr->sprites);
        free(arr->key);
        free(arr);
    }
}

void freeFrame(struct YMLFrame * frame) {
    if(frame != NULL) {
        freeFrame(frame->next);
        free(frame->arrangement);
        free(frame);
    }
}

void freeAnim(struct YMLAnimation * anim) {
    if(anim->sheet != NULL)
        free(anim->sheet);
    freeArrangement(anim->arrangements);
    freeFrame(anim->frames);
}

void freePart(struct YMLPart * part) {
    if(part != NULL) {
        freePart(part->next);
        free(part);
    }
}

void freeSheet(struct YMLSheet * sheet) {
    if(sheet->gfx != NULL)
        free(sheet->gfx);
    freePart(sheet->parts);
}


void handle_part_mapping(yaml_parser_t * parser, struct YMLPart * part) {
	yaml_event_t event;
    do {
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT:
            char * value = event.data.scalar.value;
            if(strcmp(value, "x") == 0) {
                part->x = parser_get_int(parser);
            } else if (strcmp(value, "y") == 0){
                part->y = parser_get_int(parser);
            } else if (strcmp(value, "w") == 0){
                part->w = parser_get_int(parser);
            } else if (strcmp(value, "h") == 0){
                part->h = parser_get_int(parser);
            } else {
                parser_skip_value(parser);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            yaml_event_delete(&event);
            return;
        default:
            printf("\nline %d, event %d\n", parser->mark.line, event.type);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
        yaml_event_delete(&event);
    } while (event.type != YAML_MAPPING_END_EVENT);
    printf("we shouldn't be here\n");
    exit(EXIT_FAILURE);
}

void handle_parts_sequence(yaml_parser_t * parser, struct YMLSheet * sheet) {
    yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_SEQUENCE_START_EVENT) {
		printf("Expected Sequence at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_MAPPING_START_EVENT: // We have a Squence of parts mappings
            handle_part_mapping(parser, newPart(&sheet->parts));
            break;
        case YAML_SEQUENCE_END_EVENT:
            break;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
    } while (event.type != YAML_SEQUENCE_END_EVENT);
    //printf("finished frames\n");
    yaml_event_delete(&event);
    
}

void handle_sheet_mapping(yaml_parser_t * parser, struct YMLSheet * sheet) {
	yaml_event_t event;
	if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
	if(event.type != YAML_MAPPING_START_EVENT) {
		printf("Expected Mapping at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
	}
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_SCALAR_EVENT: // We get the arrangement's key from this scalar event
            char * value = event.data.scalar.value;
            if(strcmp(value, "gfx") == 0) { // Skip this
                sheet->gfx = parser_get_string(parser);
            } else if (strcmp(value, "width") == 0){
                sheet->width = parser_get_int(parser);
            } else if (strcmp(value, "height") == 0){
                sheet->height = parser_get_int(parser);
            } else if (strcmp(value, "parts") == 0){
                handle_parts_sequence(parser, sheet);
            } else {
                parser_skip_value(parser);
            }
            break;
        case YAML_MAPPING_END_EVENT:
            break;
        default:
            printf("line %d\n", parser->mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }
    } while (event.type != YAML_MAPPING_END_EVENT);
    //printf("sheet map finished\n");
    yaml_event_delete(&event);
}

void handle_sheet_yaml(yaml_parser_t * parser, struct YMLSheet * sheet) {
	yaml_event_t event;

	handle_sheet_mapping(parser, sheet);

    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    
    if(event.type != YAML_DOCUMENT_END_EVENT) {
        printf("Expected YAML_DOCUMENT_END_EVENT at line %d but got event %d\n", parser->mark.line, event.type);
		exit(EXIT_FAILURE);
    }
    yaml_event_delete(&event);
}

void load_anim_from_yaml(char * data, struct YMLAnimation * anim, int size) {
    yaml_parser_t parser;
    yaml_event_t event;

    if(!yaml_parser_initialize(&parser))
        fputs("Failed to initialize parser!\n", stderr);

    yaml_parser_set_input_string(&parser, data, size);
    
    if (!yaml_parser_parse(&parser, &event))
        parser_error(&parser);

    // Skip stream start event
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(&parser, &event))
            parser_error(&parser);

        switch (event.type) {
        case YAML_DOCUMENT_START_EVENT:
            handle_anim_yaml(&parser, anim);
            break;
        case YAML_STREAM_END_EVENT:
            break;
        default:
            printf("line %d\n", parser.mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }

    } while (event.type != YAML_STREAM_END_EVENT);
    
    yaml_event_delete(&event);
    yaml_parser_delete(&parser);
}

void load_sheet_from_yaml(char * data, struct YMLSheet * sheet, int size) {
    yaml_parser_t parser;
    yaml_event_t event;

    if(!yaml_parser_initialize(&parser))
        fputs("Failed to initialize parser!\n", stderr);

    yaml_parser_set_input_string(&parser, data, size);
    
    if (!yaml_parser_parse(&parser, &event))
        parser_error(&parser);

    // Skip stream start event
    
    do {
        yaml_event_delete(&event);
        if (!yaml_parser_parse(&parser, &event))
            parser_error(&parser);

        switch (event.type) {
        case YAML_DOCUMENT_START_EVENT:
            handle_sheet_yaml(&parser, sheet);
            break;
        case YAML_STREAM_END_EVENT:
            break;
        default:
            printf("line %d\n", parser.mark.line);
            fputs("An unexpected problem happened\n", stderr);
            exit(EXIT_FAILURE);
        }

    } while (event.type != YAML_STREAM_END_EVENT);
    
    yaml_event_delete(&event);
    yaml_parser_delete(&parser);
}

RenderTexture2D animationRenderTexture = {};
Shader animationRenderShader = {};

void InitYMLAnim(void) {
    if(IsRenderTextureValid(animationRenderTexture))
        UnloadRenderTexture(animationRenderTexture);
    if(IsShaderValid(animationRenderShader))
        UnloadShader(animationRenderShader);
    animationRenderTexture = LoadRenderTexture(240, 160);
    BeginTextureMode(animationRenderTexture);
    ClearBackground(BLACK);
    EndTextureMode();
    animationRenderShader = LoadShader("shader/base.vs", "shader/palette_switch.fs");
}

void FreeAnimation(struct AnimationListEntry *animation) {
    if(IsTextureValid(animation->animationInfo.sheetTexture))
        UnloadTexture(animation->animationInfo.sheetTexture);
    if(IsTextureValid(animation->animationInfo.paletteTexture))
        UnloadTexture(animation->animationInfo.paletteTexture);
    animation->animationInfo.sheetTexture = (Texture2D){};
    animation->animationInfo.paletteTexture = (Texture2D){};
    freeSheet(&animation->animationInfo.sheet);
    freeAnim(&animation->animationInfo.animation);
    animation->animationInfo.sheet = (struct YMLSheet){};
    animation->animationInfo.animation = (struct YMLAnimation){};
}

int ChangeAnimation(struct AnimationListEntry *animation, u32 animOffset) {
    struct AnimationInfo * animInfo = &animation->animationInfo;
    char * folder = NULL;
    // Lol
    if(strcmp(animInfo->animName, TextFormat("%X", animOffset)) == 0) return 0;
    if(animInfo->animId == 0xFF) {
        folder = gPersonAnimData[animInfo->personId].folder;
        if(folder == NULL) return 0; // This should assert
        FreeAnimation(animation);
        printf("Loaded new animData\n");
        LoadAnimDataIntoInfo(animInfo, folder, animOffset);
        return 1;
    }
    return 0;
}

void LoadAnimDataFileIntoInfo(struct AnimationInfo * animInfo, char * animYml) {
    uint8_t * yamlData;
    int yamlSize = 0;
    char * animationYml = animYml;
    char * baseName = strrchr(animYml, '/');
    char * animFolder = strdup(animYml);
    animFolder[(int)(baseName - animYml)] = '\0'; // FUN
    
    baseName = baseName ? baseName+1 : animYml;
    strncpy(animInfo->animName, baseName, 32);
    yamlData = LoadFileData(animationYml, &yamlSize);
    load_anim_from_yaml(yamlData, &animInfo->animation, yamlSize);
    UnloadFileData(yamlData);
    //MemFree(animationYml);
    yamlSize = 0;
    char * sheetYml = TextFormat("%s/%s.yml", animFolder, animInfo->animation.sheet);
    yamlData = LoadFileData(sheetYml, &yamlSize);
    load_sheet_from_yaml(yamlData, &animInfo->sheet, yamlSize);
    UnloadFileData(yamlData);
    //MemFree(sheetYml);
    //printf("Help\n");
    // Create Paletted Texture/Shader
    char * sheetGfx = TextFormat("%s/%s", animFolder, animInfo->sheet.gfx);
    
    struct LodePNGState lodepngState;
    lodepng_state_init(&lodepngState);
    //printf("Help\n");
    lodepngState.info_raw.colortype = LCT_PALETTE;
    lodepngState.info_raw.bitdepth = 8;
    yamlSize = 0;
    uint8_t * sheetPng = LoadFileData(sheetGfx, &yamlSize);
    uint8_t * imageData = NULL;
    unsigned w;
    unsigned h;
    //printf("Help\n");
    lodepng_decode(&imageData, &w, &h, &lodepngState, sheetPng, yamlSize);
    //printf("Help\n");
    if(lodepngState.info_png.color.colortype != LCT_PALETTE) {
        return;
    }
    Image palettedImage = {
        .data = imageData,
        .width = w,
        .height = h,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE, // 1
    };
    Image paletteImage = {
        .data = lodepngState.info_png.color.palette,
        .width = 256,
        .height = 1,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    //printf("Help\n");
    animInfo->sheetTexture = LoadTextureFromImage(palettedImage);
    animInfo->paletteTexture = LoadTextureFromImage(paletteImage);
    lodepng_state_cleanup(&lodepngState);
    free(imageData);
    free(animFolder);
    //printf("Help\n");
}

void LoadAnimDataIntoInfo(struct AnimationInfo * animInfo, char * animFolder, int animOff) {
    uint8_t * yamlData;
    int yamlSize = 0;
    char * animationYml = TextFormat("%s/%X.yml", animFolder, animOff);
    strncpy(animInfo->animName, TextFormat("%X", animOff), 32);
    yamlData = LoadFileData(animationYml, &yamlSize);
    load_anim_from_yaml(yamlData, &animInfo->animation, yamlSize);
    UnloadFileData(yamlData);
    //MemFree(animationYml);
    yamlSize = 0;
    char * sheetYml = TextFormat("%s/%s.yml", animFolder, animInfo->animation.sheet);
    yamlData = LoadFileData(sheetYml, &yamlSize);
    load_sheet_from_yaml(yamlData, &animInfo->sheet, yamlSize);
    UnloadFileData(yamlData);
    //MemFree(sheetYml);
    //printf("Help\n");
    // Create Paletted Texture/Shader
    char * sheetGfx = TextFormat("%s/%s", animFolder, animInfo->sheet.gfx);
    
    struct LodePNGState lodepngState;
    lodepng_state_init(&lodepngState);
    //printf("Help\n");
    lodepngState.info_raw.colortype = LCT_PALETTE;
    lodepngState.info_raw.bitdepth = 8;
    yamlSize = 0;
    uint8_t * sheetPng = LoadFileData(sheetGfx, &yamlSize);
    uint8_t * imageData = NULL;
    unsigned w;
    unsigned h;
    //printf("Help\n");
    lodepng_decode(&imageData, &w, &h, &lodepngState, sheetPng, yamlSize);
    //printf("Help\n");
    if(lodepngState.info_png.color.colortype != LCT_PALETTE) {
        return;
    }
    Image palettedImage = {
        .data = imageData,
        .width = w,
        .height = h,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE, // 1
    };
    Image paletteImage = {
        .data = lodepngState.info_png.color.palette,
        .width = 256,
        .height = 1,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    //printf("Help\n");
    animInfo->sheetTexture = LoadTextureFromImage(palettedImage);
    animInfo->paletteTexture = LoadTextureFromImage(paletteImage);
    lodepng_state_cleanup(&lodepngState);
    free(imageData);
    //printf("Help\n");
}

u32 AdvanceAnimationYaml(struct AnimationListEntry * animation)
{
    void * gfxDataStart;
    void * prev;
    u32 retVal = 4;

    if (gScriptContext.personAnimPauseCounter && animation->animationInfo.animId == 0xFF)
        return retVal;
    if (animation->frameDataYml->duration > ++animation->frameDurationCounter)
        return retVal;
    //printf("Fuck\n");
    animation->frameDurationCounter = 0;
    prev = animation->frameDataYml;
    animation->frameDataYml = animation->frameDataYml->next;
    assert(animation->frameDataYml != NULL);
    if (animation->frameDataYml->sfx.enabled)
        PlaySE(animation->frameDataYml->sfx.id);
    if (animation->frameDataYml->action.enabled)
        DoAnimationAction(animation->frameDataYml->action.id);
    //printf("Fuck\n");
    switch (animation->frameDataYml->duration)
    {
    case ANIM_LOOP:
        animation->frameDataYml = animation->animationInfo.animation.frames;
        animation->arrYml = getArrangement(&animation->animationInfo.animation, animation->frameDataYml->arrangement);
        animation->flags |= ANIM_QUEUED_TILE_UPLOAD;
        retVal = 7;
        break;
    case ANIM_STOP:
        animation->flags &= ~ANIM_PLAYING;
        retVal = 0;
        animation->frameDataYml = prev;
        break;
    case ANIM_DESTROY:
        DestroyAnimation(animation);
        retVal = 0;
        break;
    default:
        animation->arrYml = getArrangement(&animation->animationInfo.animation, animation->frameDataYml->arrangement);
        animation->flags |= ANIM_QUEUED_TILE_UPLOAD;
        retVal = 5;
        break;
    }
    //printf("Fuck\n");
    return retVal;
}

// Now we render...
void DrawAnimationYaml(void) {
    
    u32 var0 = 0x80;
    //struct OamAttrs *oam = &gOamObjects[ARRAY_COUNT(gOamObjects)];
    struct AnimationListEntry *animation;
    BeginTextureMode(animationRenderTexture);
    ClearBackground(BLANK);
    //DrawRectangle(0, 0, 240, 120, RED);
    for (animation = gAnimation[0].next; animation != NULL; animation = animation->next)
    {
        u32 i, j;
        if ((animation->flags & ANIM_ALLOCATED) == 0)
            continue;
        animation->animtionOamEndIdx = var0;
        if ((gMain.animationFlags & 2) && (animation->flags & ANIM_ACTIVE))
        {
            BeginShaderMode(animation->paletteShader);
                void *ptr = (void *)animation->spriteData;
                struct YMLSprite *spriteTemplate = animation->arrYml->sprites;
                int paletteLoc = GetShaderLocation(animation->paletteShader, "palette");
                //int paletteIdxLoc = GetShaderLocation(animation->paletteShader, "pidx");
                s32 xOrigin = animation->animationInfo.xOrigin - gMain.shakeAmountX;
                s32 yOrigin = animation->animationInfo.yOrigin - gMain.shakeAmountY;
                SetShaderValueTexture(animation->paletteShader, paletteLoc, animation->animationInfo.paletteTexture);
                while(spriteTemplate != NULL) {
                    struct YMLPart * part = getPart(&animation->animationInfo.sheet, spriteTemplate->part);
                    s32 x, y, w;
                    int alpha = 255;
                    y = yOrigin + spriteTemplate->y;
                    if (y < -64)
                        y = -64;
                    if (y > 224)
                        y = 224;
                    if (animation->flags & ANIM_BLEND_ACTIVE) {
                        alpha = ((16-gMain.blendDeltaY)*255)/16;
                        printf("%d\n", alpha);
                    }
                    if (animation->flags & ANIM_ENABLE_XFLIP)
                    {
                        x = xOrigin - (spriteTemplate->x + part->w);
                        w = -part->w;
                        //oam->attr1 |= 0x1000 | x;
                    }
                    else
                    {
                        x = xOrigin + spriteTemplate->x;
                        w = part->w;
                        //oam->attr1 |= (*(u8 *)(&animation->spritePriorityMatrixIndex)) << 9 | x;
                    }
                    //printf("Draw Texture\n");
                    DrawTextureRec(animation->animationInfo.sheetTexture, (Rectangle){part->x, part->y, w, part->h}, (Vector2){x, y}, (Color){255, 255, spriteTemplate->palette, alpha});
                    //DrawTexturePro(animation->animationInfo.paletteTexture, (Rectangle){0, 0, 256, 1}, (Rectangle){0, 0, 512, 20}, (Vector2){0, 0}, 0, (Color){255, 255, 255, 255});
                    spriteTemplate = spriteTemplate->next;
                }
            EndShaderMode();
        }
        animation->animtionOamStartIdx = animation->animtionOamEndIdx - animation->animationInfo.spriteCount;
        var0 -= animation->animtionOamStartIdx;
        /*
        for (var0 -= 1; var0 != -1; var0--)
        {
            oam--;
            oam->attr0 = SPRITE_ATTR0_CLEAR;
        }
        */
        var0 = animation->animtionOamStartIdx;
    }
    EndTextureMode();
}
