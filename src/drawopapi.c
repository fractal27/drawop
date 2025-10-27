/* drawopAPI.c*/
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "drawopapi.h"
//#include "drawop.h"

uint32_t drawop_screen_used = 0;
int screenWidth;
int screenHeight;
char drawop_char_pressed = ' ';

void drawop_init(int height, int width){
       // initialize variables
       screenHeight = GetMonitorHeight(drawop_screen_used);
       screenWidth = GetMonitorHeight(drawop_screen_used);
       if(height < screenHeight && width > 0)
              screenHeight = height;
       if(width < screenWidth && width > 0)
              screenWidth = width;
}

void drawop_undohighlight(drawop_state* state){
    int alpha = (int)floor(state->color.a*2);
    if(alpha != state->color.a){

        if(alpha > 255)     alpha = 255;
        else if(alpha < 0)  alpha = 0;

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

    if(state.mode == TEXT){
           switch(actions & 0x0f0000){
                  case DRAWOP_TEXT_WRITE_NEWLINE:
                         endstate->text.buffer[endstate->text.bufsize++] = '\n';
                         endstate->text.buffer[endstate->text.bufsize] = '\0';
                         break;
                  case DRAWOP_TEXT_WRITE_CHAR:
                         endstate->text.buffer[endstate->text.bufsize++] = drawop_char_pressed;
                         endstate->text.buffer[endstate->text.bufsize] = '\0';
                         break;
                  case DRAWOP_TEXT_WRITE_BACKSPACE:
                         endstate->text.buffer[--endstate->text.bufsize] = '\0';
                         break;
                  default:
                         break;
           }
    } else {
           if(actions & DRAWOP_TOGGLE_COLOR_PICKER){
               endstate->pickColor = !state.pickColor;
               endstate->pickColorLocation = state.mouse_position;
           }

           if(actions & DRAWOP_TOGGLE_TRANSPARENT){
                  endstate->config.transparent = !state.config.transparent;
           }
           
           if(actions & DRAWOP_TOGGLE_HELP){ 
               endstate->help_window = !state.help_window;
               TraceLog(LOG_INFO,"helpwindow now %d",endstate->help_window);
           }
    }
    switch(actions & 0x0000f0){
        case DRAWOP_SWITCHTO_LASER:
            if(state.mode == TEXT){
                   endstate->texts_to_write[state.ntexts_to_write].position = state.text.position;
                   endstate->texts_to_write[state.ntexts_to_write].bufsize = state.text.bufsize;
                   endstate->texts_to_write[state.ntexts_to_write].fontsize = state.text.fontsize;
                   strncpy(endstate->texts_to_write[endstate->ntexts_to_write].buffer, endstate->text.buffer, state.text.bufsize-1);
                   endstate->ntexts_to_write++;

                   *endstate->text.buffer = '\0';
                   endstate->text.bufsize = 0;
            }
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
            }
            endstate->pickColor = false;
            endstate->mode = LASER;
            break;

        case DRAWOP_SWITCHTO_TEXT:
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
            }
            endstate->pickColor = false;
            endstate->mode = TEXT;
            endstate->text.position = endstate->mouse_position;
            break;

        case DRAWOP_SWITCHTO_BRUSH:
            if(state.mode == TEXT) *endstate->text.buffer = '\0';
            if(state.mode == HIGHLIGHTER){
                drawop_undohighlight(endstate);
            }
            endstate->pickColor = false;
            endstate->mode = BRUSH;
            break;

        case DRAWOP_SWITCHTO_HIGHLIGHT:
            if(state.mode == TEXT) *endstate->text.buffer = '\0';
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

