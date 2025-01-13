#include <stdbool.h>
#include <math.h>

#include "drawopapi.h"
#include "drawop.h"

#undef HELP_TEXT_LINES 
#define HELP_TEXT_LINES 8

const char* HELP_TEXT = "KEYS:\n"
            "1  : LASER mode\n"
            "2  : BRUSH mode\n"
            "3  : HIGHLIGHTER mode\n"
            "c  : Color picker\n"
            "r  : Clear all \n"
            "C-z: Undo last draw\n"
            /*TODO:"t: Toggle transparent mode\n"*/
            "h  : Toggle the help menu(this)";


const Color HELP_COLOR_RECT = (Color){0x0d,0x0c,0x0b,0x88};
const Color HELP_COLOR = (Color){0x0f,0x0f,0x0f,0xff};


drawop_action get_action(drawop_state state){
    /* update state of program */
    drawop_action actions = 0;

    if(state.pickColor && state.alpha != state.color.a){
        actions |= DRAWOP_UPDATE_ALPHA;
    }
    if(IsKeyPressed(KEY_C)
    || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){actions|=DRAWOP_TOGGLE_COLOR_PICKER;
    } else if(IsKeyPressed(KEY_H)){              actions|=DRAWOP_TOGGLE_HELP;
    } else if (IsKeyPressed(KEY_ONE)){           actions|=DRAWOP_SWITCHTO_LASER;
    } else if (IsKeyPressed(KEY_TWO)){           actions|=DRAWOP_SWITCHTO_BRUSH;
    } else if (IsKeyPressed(KEY_THREE)){         actions|=DRAWOP_SWITCHTO_HIGHLIGHT;
    } else if (IsKeyPressed(KEY_R)){             actions|=DRAWOP_RESET;
    } else if (IsKeyPressed(KEY_LEFT_CONTROL)){  actions|=DRAWOP_CONTROL_PRESSED;
    } else if (IsKeyReleased(KEY_LEFT_CONTROL)){ actions|=DRAWOP_CONTROL_RELEASE;
    } else if (IsKeyPressed(KEY_Z)){             actions|=DRAWOP_DELETE_SEQUENCE;
    } else if((state.mode == BRUSH || state.mode == HIGHLIGHTER) && (!state.pickColor||
                (!((state.mouse_position.x > state.pickColorLocation.x)
                  && (state.mouse_position.y > state.pickColorLocation.y)
                  && (state.mouse_position.x < state.pickColorLocation.x+250) //color picker + slider
                  && (state.mouse_position.y < state.pickColorLocation.y+200)
                 )))){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            actions+=DRAWOP_DRAW_ENGAGE;
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
            actions+=DRAWOP_DRAW_RELEASE;
        }
    }
    return actions;
}

void update(drawop_state state,drawop_state* endstate, drawop_action actions){
    drawop_updatestate(state,endstate,actions);
    if(endstate->pickColor){
        GuiColorPicker((Rectangle){
                endstate->pickColorLocation.x,endstate->pickColorLocation.y,
                200,200},"Pick color",
                &endstate->color);
        GuiSlider((Rectangle){
                endstate->pickColorLocation.x,
                endstate->pickColorLocation.y+200,
                200,50},
                "transparent","full",&endstate->alpha,0,255);
    }
}



void draw(drawop_state* state){
    float scroll;
    struct segment* current_segment;

    BeginDrawing();
    ClearBackground(BLANK);
    if(state->help_window){
        DrawRectangleRounded((Rectangle){screenWidth-MeasureText(HELP_TEXT,20)-1-5,
                screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50-5,
                MeasureText(HELP_TEXT,20)+5, (int)floor(HELP_TEXT_LINES*22)+5},0.2,0,
                HELP_COLOR_RECT);
        DrawText(HELP_TEXT,screenWidth-MeasureText(HELP_TEXT,20)-1,
                screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50,20,HELP_COLOR);
    }
    if(state->config.show_title){
        DrawRectangleRounded((Rectangle){
                1,1,
                screenWidth-1, 35+5},0.15,0,
                state->config.title_background_color);
        DrawText(state->config.title,screenWidth/2-MeasureText(state->config.title,30)/2-1,
                1,30,state->config.title_text_color);
    }

    switch(state->mode){
        case LASER:

            DrawText("Mode: LASER",5,5,30,RED);
            DrawCircleV(state->mouse_position, state->radius, state->color); 
            scroll = GetMouseWheelMove()*3;
            if(scroll && state->radius+scroll<RADIUS_MAX && state->radius+scroll > 3){
                state->radius += scroll;
            }
            break;


        case BRUSH: DrawText("Mode: BRUSH",5,5,30,LIME);
        case HIGHLIGHTER:
            if(state->mode==HIGHLIGHTER)
                    DrawText("Mode: HIGHLIGHTER",5,5,30,YELLOW);

            DrawCircleV(state->mouse_position, state->radius, state->color); 
            scroll = GetMouseWheelMove()*3;
            if(scroll && state->radius+scroll<RADIUS_MAX){
                state->radius += scroll;
            }
            if(state->nto_draw < DRAW_MAX+1 && state->left_click_pressed){
                //drawing phase
                current_segment = &state->segments_to_draw[state->nto_draw];
                if(current_segment->positions[current_segment->npositions].x != state->mouse_position.x 
                    && current_segment->positions[current_segment->npositions].y != state->mouse_position.y){
                    current_segment->positions[current_segment->npositions++] =\
                                state->mouse_position;
                }
            }
            break;

        default:
            break;
    }

    if(state->nto_draw < DRAW_MAX){ 
        //if(state->left_click_pressed && !state->nto_draw){
        for(size_t i = 0;i < state->nto_draw;i++){
            current_segment = &state->segments_to_draw[i];
            if(current_segment->npositions > 3)
                DrawSplineBasis(current_segment->positions, current_segment->npositions,
                        current_segment->thickness, current_segment->color);
        }
        if(state->left_click_pressed && state->nto_draw != DRAW_MAX){
            current_segment = &state->segments_to_draw[state->nto_draw];
            if(current_segment->npositions > 3)
                DrawSplineBasis(current_segment->positions, current_segment->npositions,
                        current_segment->thickness, current_segment->color);
        }
    }

    EndDrawing();
}



