#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <stdbool.h>
#include <math.h>

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


void undoHighlight(DrawopState* state){
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



void update(DrawopState* state){
    /* update state of program */
    if(state->pickColor && state->alpha != state->color.a){
        state->color.a = state->alpha;
    } state->mouse_position = GetMousePosition();

    if(IsKeyPressed(KEY_C) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){

        state->pickColor = !state->pickColor;
        state->pickColorLocation = state->mouse_position;
    } else if(IsKeyPressed(KEY_H)){

        state->help_window = !state->help_window;
    } else if (IsKeyPressed(KEY_ONE)){

        if(state->mode == HIGHLIGHTER){
            undoHighlight(state);
            TraceLog(LOG_INFO,"highlighter unloaded: state->mode=LASER;");
        }
        state->pickColor = false;
        state->mode = LASER;
    } else if (IsKeyPressed(KEY_TWO)){

        if(state->mode == HIGHLIGHTER){
            undoHighlight(state);
            TraceLog(LOG_INFO,"highlighter unloaded: state->mode=BRUSH;");
        }
        state->pickColor = false;
        state->mode = BRUSH;
    } else if (IsKeyPressed(KEY_THREE)){

        state->mode = HIGHLIGHTER;
        state->pickColor = false;
        int alpha = (int)floor(state->color.a/state->config.highlight_alpha_div);
        if(alpha != state->color.a){
            state->color.a = alpha;
            TraceLog(LOG_INFO,"highlighter loaded: state->color.a=%d;",state->color.a);
        }
        if((int)floor(state->radius*state->config.highlight_radius_mult) < RADIUS_MAX){
            state->radius = (int)floor(state->radius*state->config.highlight_alpha_div);
            TraceLog(LOG_INFO,"highlighter loaded: state->radius=%.3f;",state->radius);
        }
    } else if (IsKeyPressed(KEY_R)){

        state->nto_draw = 0;
    } else if (IsKeyPressed(KEY_LEFT_CONTROL)){

        state->control_pressed = true;
    } else if (IsKeyReleased(KEY_LEFT_CONTROL)){

        state->control_pressed = false;
    } else if (IsKeyPressed(KEY_Z)){

        if (state->control_pressed){
            if(state->nto_draw){
                TraceLog(LOG_INFO,"Deleting last sequence");
                state->nto_draw--;
            }
        }
    } else if((state->mode == BRUSH || state->mode == HIGHLIGHTER) && (!state->pickColor||
                (!((state->mouse_position.x > state->pickColorLocation.x)
                  && state->mouse_position.y > state->pickColorLocation.y)
                 && (state->mouse_position.x < state->pickColorLocation.x+250) //color picker + slider
                  && (state->mouse_position.y < state->pickColorLocation.y+200)
                 ))){
        

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            state->left_click_pressed = true;
            state->segments_to_draw[state->nto_draw].npositions = 0;
            state->segments_to_draw[state->nto_draw].thickness = state->radius;
            state->segments_to_draw[state->nto_draw].color = state->color;
            //TraceLog(LOG_INFO,"New segment initialized with radius: %.2f and color (%d,%d,%d,%d)",state->radius,state->color.r,state->color.g,state->color.b,state->color.a);

        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
            //TraceLog(LOG_INFO,"MOUSEUP:");
            state->left_click_pressed = false;
            if(state->nto_draw < SEG_MAX){
                state->nto_draw++;
            }
        }
    }

    if(state->pickColor){

        GuiColorPicker((Rectangle){
                state->pickColorLocation.x,state->pickColorLocation.y,
                200,200},"Pick color",
                &state->color);
        GuiSlider((Rectangle){
                state->pickColorLocation.x,
                state->pickColorLocation.y+200,
                200,50},
                "transparent","full",&state->alpha,0,255);
    }
}



void draw(DrawopState* state){
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



