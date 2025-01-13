#ifndef _DRAWOP_H
#define _DRAWOP_H

#include <raylib.h>
#include <stddef.h>
#include "drawopapi.h"

extern void GuiColorPicker(Rectangle rec,const char* caption,Color* color);
extern void GuiSlider(Rectangle rec,const char*first, const char*last,float* value,int min,int max);
extern drawop_action get_action(drawop_state state);
extern void update(drawop_state state, drawop_state* newstate, drawop_action actions);
extern void draw(drawop_state* state);



#endif


