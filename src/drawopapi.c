/* drawopAPI.c 0.2.4*/
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "drawopapi.h"

void drawop_undohighlight(drawop_state* state){
    int alpha = (int)floor(state->color.a*2);
    if(alpha != state->color.a){
        if(alpha > 255)
            alpha = 255;
        state->color.a = alpha;
            TraceLog(LOG_INFO,"highlighter unloaded: alpha=%d;",alpha);
    } else  TraceLog(LOG_INFO,"highlighter unloaded: alpha already=%d;",alpha);
    state->radius = (int)floor(state->radius/state->config.highlight_radius_mult);
    TraceLog(LOG_INFO,"highlighter unloaded: state->radius=%lf;",state->radius);
}


void drawop_updatestate(drawop_state state, drawop_state* endstate, drawop_action actions){
    
    *endstate = state;
    endstate->should_clear = actions & DRAWOP_CLEAR_NOW;

    if(actions & DRAWOP_UPDATE_ALPHA){
        //printf("updated alpha: endstate->color.a=%.2f",state.alpha);
        endstate->should_clear = true;
        endstate->color.a = (int)floor(state.alpha*255);
    }
    endstate->mouse_position = GetMousePosition();

    if(actions & DRAWOP_TOGGLE_COLOR_PICKER){
        endstate->pickColor = !state.pickColor;
        endstate->should_clear = true;
        endstate->pickColorLocation = state.mouse_position;
    }
    if(actions & DRAWOP_TOGGLE_HELP){
        endstate->help_window = !state.help_window;
        endstate->should_clear = true;
    }
    if(actions & DRAWOP_TOGGLE_TRANSPARENT){
        endstate->config.transparent = !state.config.transparent;
        endstate->should_clear = true;
    }
    if(actions & DRAWOP_SCREENSHOT){
        const char* fn = TextFormat("~/Pictures/drawop/%08X.png");
        TakeScreenshot(fn);
    }

    switch(actions & 0x0000f0){
        case DRAWOP_SWITCHTO_LASER:
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
                TraceLog(LOG_INFO,"highlighter unloaded: state.mode=LASER;");
            }
            endstate->pickColor = false;
            endstate->should_clear = true;
            endstate->mode = LASER;
            break;
            
        case DRAWOP_SWITCHTO_BRUSH:
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
                TraceLog(LOG_INFO,"highlighter unloaded: state.mode=BRUSH;");
            }
            endstate->pickColor = false;
            endstate->should_clear = true;
            endstate->mode = BRUSH;
            break;

        case DRAWOP_SWITCHTO_HIGHLIGHT:
            endstate->mode = HIGHLIGHTER;
            endstate->should_clear = true;
            endstate->pickColor = false;
            int alpha = (int)floor(state.color.a/state.config.highlight_alpha_div);
            if(alpha != state.color.a){
                endstate->color.a = alpha;
                TraceLog(LOG_INFO,"highlighter loaded: state.color.a=%d;",state.color.a);
            }
            if((int)floor(state.radius*state.config.highlight_radius_mult) < RADIUS_MAX){
                endstate->radius = (int)floor(state.radius*state.config.highlight_alpha_div);
                TraceLog(LOG_INFO,"highlighter loaded: state.radius=%.3f;",state.radius);
            }
            break;
    }
    switch(actions & 0x000f00){
        case DRAWOP_RESET:
            endstate->should_clear = true;
            endstate->nto_draw = 0;
            break;

        case DRAWOP_CONTROL_PRESSED:
            endstate->control_pressed = true;
            break;

        case DRAWOP_CONTROL_RELEASE:
            endstate->should_clear = true;
            endstate->control_pressed = false;
            break;
    }
    switch(actions & 0x00f000){
        case DRAWOP_DELETE_SEQUENCE:
            if (state.control_pressed){
                if(state.nto_draw){
                    TraceLog(LOG_INFO,"Deleting last sequence");
                    memset(&endstate->segments_to_draw[endstate->nto_draw--],0,sizeof(struct segment));
                }
            }
            endstate->should_clear = true;
            break;
        case DRAWOP_DRAW_ENGAGE:
            endstate->left_click_pressed = true;
            endstate->segments_to_draw[endstate->nto_draw].npositions = 0;
            endstate->segments_to_draw[endstate->nto_draw].thickness = endstate->radius;
            endstate->segments_to_draw[endstate->nto_draw].color = endstate->color;
            break;

        case DRAWOP_DRAW_RELEASE:
            //TraceLog(LOG_INFO,"MOUSEUP:");
            endstate->left_click_pressed = false;
            if(state.nto_draw < SEG_MAX){
                endstate->nto_draw++;
            } else {
                TraceLog(LOG_ERROR,"Segment array out of bounds.");
            }
            break;
    }

}

