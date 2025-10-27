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
            "4  : TEXT mode\n"
            "c  : Color picker\n"
            "r  : Clear all \n"
            "C-z: Undo last draw\n"
            "t:   Toggle transparent mode\n"
            "h  : Toggle the help menu(this)";


// const Color HELP_COLOR_RECT = (Color){0x0d,0x0c,0x0b,0x88};
const Color HELP_COLOR_RECT = (Color){0xff,0xff,0xff,0x88};
const Color HELP_COLOR = (Color){0x0f,0x0f,0x0f,0xff};

#define add_action(actions_before, to_append)\
       *actions_before |= to_append;


drawop_action get_action(drawop_state state){
    /* update state of program */
    drawop_action actions = 0;
    drawop_action* pactions = &actions;

    if(state.pickColor && state.alpha != state.color.a){
        add_action(pactions,DRAWOP_UPDATE_ALPHA);
    }
    int key_pressed = GetKeyPressed();
    if (state.mode == TEXT){
           char char_pressed = GetCharPressed();
           switch(key_pressed){
                  case KEY_ESCAPE: 
                         add_action(pactions,DRAWOP_SWITCHTO_LASER);
                         break;
                  case KEY_ENTER:
                         add_action(pactions,DRAWOP_TEXT_WRITE_NEWLINE);
                         break;
                  case KEY_BACKSPACE:
                         add_action(pactions,DRAWOP_TEXT_WRITE_BACKSPACE);
                         break;
                  default:
                         if(char_pressed >= 32 && char_pressed <= 126) 
                                add_action(pactions,DRAWOP_TEXT_WRITE_CHAR);
                         drawop_char_pressed = char_pressed;
                         break;
           }
    } else if(key_pressed == KEY_C
     || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){add_action(pactions,DRAWOP_TOGGLE_COLOR_PICKER);
    } else if(key_pressed == KEY_H){              add_action(pactions,DRAWOP_TOGGLE_HELP);
    } else if (key_pressed == KEY_T){             add_action(pactions,DRAWOP_TOGGLE_TRANSPARENT);
    } else if (key_pressed == KEY_ONE){           add_action(pactions,DRAWOP_SWITCHTO_LASER);
    } else if (key_pressed == KEY_TWO){           add_action(pactions,DRAWOP_SWITCHTO_BRUSH);
    } else if (key_pressed == KEY_THREE){         add_action(pactions,DRAWOP_SWITCHTO_HIGHLIGHT);
    } else if (key_pressed == KEY_FOUR){          add_action(pactions,DRAWOP_SWITCHTO_TEXT);
    } else if (key_pressed == KEY_R){             add_action(pactions,DRAWOP_RESET);
    } else if (key_pressed == KEY_LEFT_CONTROL){  add_action(pactions,DRAWOP_CONTROL_PRESSED);
    } else if (key_pressed == KEY_Z){             add_action(pactions,DRAWOP_DELETE_SEQUENCE);
    } else if (IsKeyReleased(KEY_LEFT_CONTROL)){ actions|=DRAWOP_CONTROL_RELEASE;
    } else if((state.mode == BRUSH || state.mode == HIGHLIGHTER) && (!state.pickColor||
                (!((state.mouse_position.x > state.pickColorLocation.x)
                  && (state.mouse_position.y > state.pickColorLocation.y)
                  && (state.mouse_position.x < state.pickColorLocation.x+250) //color picker + slider
                  && (state.mouse_position.y < state.pickColorLocation.y+200)
                 )))){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){           add_action(pactions,DRAWOP_DRAW_ENGAGE);
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){   add_action(pactions,DRAWOP_DRAW_RELEASE); }
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

    // SetConfigFlags(state->config.transparent & FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
    
    BeginDrawing();
    ClearBackground(state->config.transparent? BLANK : state->config.background);
    if(state->help_window){
        /*TraceLog(LOG_INFO, "Drawing rectangle rounded with parameters: %d, %d, %d, %d, %f, %d, (%u, %u, %u)", 
                      screenWidth-MeasureText(HELP_TEXT,20)-1-5,
                (int)(screenHeight/2.0f-floor(HELP_TEXT_LINES*21))-50-5,
                MeasureText(HELP_TEXT,20)+5, (int)floor(HELP_TEXT_LINES*22)+5,0.3,12,
                HELP_COLOR_RECT.r, HELP_COLOR_RECT.g, HELP_COLOR_RECT.b);*/

        DrawRectangleRounded((Rectangle){screenWidth-MeasureText(HELP_TEXT,20)-1-5,
                (float)screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50-5,
                MeasureText(HELP_TEXT,20)+5, (int)floor(HELP_TEXT_LINES*22)+5},0.3,12,
                HELP_COLOR_RECT);
        DrawText(HELP_TEXT,screenWidth-MeasureText(HELP_TEXT,20)-1,
                screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50,20,HELP_COLOR);
    }
    /*if(state->config.show_title){
        DrawRectangleRounded((Rectangle){
                1,1,
                screenWidth-1, 35+5},0.15,12,
                state->config.title_background_color);
        DrawText(state->config.title,(screenWidth-MeasureText(state->config.title,30))/2-1,
                1,30,state->config.title_text_color);
    }*/

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

        case TEXT:
            //TraceLog(LOG_INFO,"%s,%f,%f",state->text.buffer,state->text.position.x, state->text.position.y);
            DrawText("Mode: TEXT",5,5,30,YELLOW);
            DrawText(state->text.buffer,
                     state->text.position.x,
                     state->text.position.y,
                     state->text.fontsize,
                     WHITE);
            break;

        default:
            break;
    }

    if(state->nto_draw < DRAW_MAX){ 
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
    for(size_t i = 0; i < state->ntexts_to_write; i++){
            DrawText(state->texts_to_write[i].buffer,
                     state->texts_to_write[i].position.x,
                     state->texts_to_write[i].position.y,
                     state->texts_to_write[i].fontsize,
                     WHITE);
    }

    EndDrawing();
}



