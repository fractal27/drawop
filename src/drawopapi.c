/* drawopAPI.c*/
#include <math.h>
#include <stdio.h>

#include "drawopapi.h"
//#include "drawop.h"



void drawop_undohighlight(drawop_state* state){
    int alpha = (int)floor(state->color.a*2);
    if(alpha != state->color.a){
        if(alpha > 255)
            alpha = 255;
        state->color.a = alpha;
            TraceLog(LOG_INFO,"highlighter unloaded: alpha=%d;",alpha);
    } else  TraceLog(LOG_INFO,"highlighter unloaded: alpha already=%d;",alpha);
    state->radius = (int)floor(state->radius/state->config.highlight_radius_mult);
    TraceLog(LOG_INFO,"highlighter unloaded: state->radius=%d;",state->radius);
}


void drawop_updatestate(drawop_state state, drawop_state* endstate, drawop_action actions){
    *endstate = state;
    //drawop_state endstate->= state;

    if(actions & DRAWOP_UPDATE_ALPHA){
        //printf("updated alpha: endstate->color.a=%.2f",state.alpha);
        endstate->color.a = (int)floor(state.alpha*255);
    }
    endstate->mouse_position = GetMousePosition();

    if(actions & DRAWOP_TOGGLE_COLOR_PICKER){
        endstate->pickColor = !state.pickColor;
        endstate->pickColorLocation = state.mouse_position;
    }
    if(actions & DRAWOP_TOGGLE_HELP)
        endstate->help_window = !state.help_window;

    switch(actions & 0x0000f0){
        case DRAWOP_SWITCHTO_LASER:
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
                TraceLog(LOG_INFO,"highlighter unloaded: state.mode=LASER;");
            }
            endstate->pickColor = false;
            endstate->mode = LASER;
            break;

        case DRAWOP_SWITCHTO_BRUSH:
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
                TraceLog(LOG_INFO,"highlighter unloaded: state.mode=BRUSH;");
            }
            endstate->pickColor = false;
            endstate->mode = BRUSH;
            break;

        case DRAWOP_SWITCHTO_HIGHLIGHT:
            endstate->mode = HIGHLIGHTER;
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
            endstate->nto_draw = 0;
            break;

        case DRAWOP_CONTROL_PRESSED:
            endstate->control_pressed = true;
            break;

        case DRAWOP_CONTROL_RELEASE:
            endstate->control_pressed = false;
            break;
    }
    switch(actions & 0x00f000){
        case DRAWOP_DELETE_SEQUENCE:
            if (state.control_pressed){
                if(state.nto_draw){
                    TraceLog(LOG_INFO,"Deleting last sequence");
                    endstate->nto_draw--;
                }
            }
            break;
        case DRAWOP_DRAW_ENGAGE:
            endstate->left_click_pressed = true;
            endstate->segments_to_draw[endstate->nto_draw].npositions = 0;
            endstate->segments_to_draw[endstate->nto_draw].thickness = endstate->radius;
            endstate->segments_to_draw[endstate->nto_draw].color = endstate->color;
            //TraceLog(LOG_INFO,"New segment initialized with radius: %.2f and color (%d,%d,%d,%d)",state.radius,state.color.r,state.color.g,state.color.b,state.color.a);
            break;

        case DRAWOP_DRAW_RELEASE:
            //TraceLog(LOG_INFO,"MOUSEUP:");
            endstate->left_click_pressed = false;
            if(state.nto_draw < SEG_MAX){
                endstate->nto_draw++;
            }
            break;
    }

}

