/*drawop.c 0.2.4*/
#include <raylib.h>
#include <stdbool.h>
#include <math.h>

#include "drawopapi.h"
#include "drawop.h"
#define SPLINE_SEGMENT_DIVISIONS 14

#undef HELP_TEXT_LINES 
#define HELP_TEXT_LINES 8


/*
//template for profiling
get_ns_time(&start);
// code here
get_ns_time(&end);
int64_t dt = delta_time(end,start);
TraceLog(LOG_INFO,"dt: %lld",dt);
*/



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
    } else if (IsKeyPressed(KEY_F1)){             actions|=DRAWOP_CLEAR_NOW;
    } else if (IsKeyPressed(KEY_F2)){             actions|=DRAWOP_SCREENSHOT;
    } else if (IsKeyPressed(KEY_F3)){             actions|=DRAWOP_TOGGLE_TRANSPARENT;
    }
    if((state.mode == BRUSH || state.mode == HIGHLIGHTER) && (!state.pickColor ||
                (!((state.mouse_position.x > state.pickColorLocation.x)
                  && (state.mouse_position.y > state.pickColorLocation.y)
                  && (state.mouse_position.x < state.pickColorLocation.x+250) //color picker + slider
                  && (state.mouse_position.y < state.pickColorLocation.y+200)
                 )))){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){           actions|=DRAWOP_DRAW_ENGAGE ;}
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){     actions|=DRAWOP_DRAW_RELEASE;}
    }
    return actions;
}



void update(drawop_state state,drawop_state* endstate, drawop_action actions){

#ifdef DRAWOPAPI_PROFILE
    struct timespec start_tot,end_tot;
    struct timespec end_updatestate;
    uint64_t dt_tot, dt_updatestate;

    get_ns_time(&start_tot);
#endif
        drawop_updatestate(state,endstate,actions);
#ifdef DRAWOPAPI_PROFILE
        get_ns_time(&end_updatestate);
#endif

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

#ifdef DRAWOPAPI_PROFILE
    get_ns_time(&end_tot);
    dt_tot = delta_time(end_tot,start_tot);
    dt_updatestate = delta_time(end_updatestate,start_tot);
    TraceLog(LOG_INFO,"update(&state) ==> dt_tot: %.2lfms dt_updatestate: %.2lfus",
            (double)dt_tot/1e6,
            (double)dt_updatestate/1e3);
#endif
}



void draw(drawop_state* state){
    float scroll = 0.0;
    struct segment* current_segment;

#ifdef DRAWOPAPI_PROFILE
    struct timespec start_tot,end_tot;
    uint64_t dt_tot;

    struct timespec start_spline={0},end_spline={0}; // can also not be computed
    uint64_t dt_spline;

    struct timespec start_mode_check, end_mode_check;
    uint64_t dt_mode_check;

    struct timespec start_help_window={0},end_help_window={0}; // can also not be computed
    uint64_t dt_help_window;
#endif

    get_ns_time(&start_tot);
    BeginDrawing();
        //template for profiling
        if(state->background_cleared % 100 == 0 || state->should_clear){
            if(!state->config.transparent){
                ClearBackground(state->config.background);
            } else {
                ClearBackground(BLANK);
            }
            TraceLog(LOG_INFO,"- Clearing screen.. -");
            if(state->config.show_title){
                TraceLog(LOG_INFO,"\t=> Showing title");
                DrawRectangleRounded((Rectangle){
                        1,1,
                        screenWidth-1, 35+5},0.15,0,
                        state->config.title_background_color);
                DrawText(state->config.title,screenWidth/2-MeasureText(state->config.title,30)/2-1,
                        1,30,state->config.title_text_color);
            }

#ifdef DRAWOPAPI_PROFILE
            get_ns_time(&start_help_window);
#endif
            if(state->help_window){
                TraceLog(LOG_INFO,"\t=> Showing help window");
                DrawRectangleRounded((Rectangle){screenWidth-MeasureText(HELP_TEXT,20)-1-5,
                        screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50-5,
                        MeasureText(HELP_TEXT,20)+5, (int)floor(HELP_TEXT_LINES*22)+5},0.2,0,
                        HELP_COLOR_RECT);
                DrawText(HELP_TEXT,screenWidth-MeasureText(HELP_TEXT,20)-1,
                        screenHeight/2-(int)floor(HELP_TEXT_LINES*21)-50,20,HELP_COLOR);
            }
#ifdef DRAWOPAPI_PROFILE
            get_ns_time(&end_help_window);
#endif
            switch(state->mode){
                case LASER:
                    DrawText("Mode: LASER",5,5,30,RED);
                    break;
                case BRUSH:
                    DrawText("Mode: BRUSH",5,5,30,LIME);
                    break;
                case HIGHLIGHTER:
                    DrawText("Mode: HIGHLIGHTER",5,5,30,YELLOW);
                    break;
                }
            state->background_cleared++;
        }

        
#ifdef DRAWOPAPI_PROFILE
        get_ns_time(&start_mode_check);
#endif

        switch(state->mode){
            case LASER:

                ClearBackground(BLANK);
                DrawCircleV(state->mouse_position, state->radius, state->color); 
                scroll = GetMouseWheelMove()*3;
                if(scroll && state->radius+scroll<RADIUS_MAX && state->radius+scroll > 3){
                    state->radius += scroll;
                }
                break;


            case BRUSH: 
            case HIGHLIGHTER:

                scroll = GetMouseWheelMove()*3;
                
                if(state->nto_draw < DRAW_MAX+1 && state->left_click_pressed){
                    //drawing phase
                    current_segment = &state->segments_to_draw[state->nto_draw];
                    if(current_segment->positions[current_segment->npositions].x !=     state->mouse_position.x 
                        && current_segment->positions[current_segment->npositions].y != state->mouse_position.y){
                        //TraceLog(LOG_DEBUG,"Adding to current segment: mouse_position(%f,%f)",
                        //          state->mouse_position.x,state->mouse_position.y);
                        current_segment->positions[current_segment->npositions++] =\
                                    state->mouse_position;
                    }
                }
                break;

            default:
                break;
        }
        if(scroll && state->radius+scroll<RADIUS_MAX && state->radius > 3){
            state->radius += scroll;
        }

#ifdef DRAWOPAPI_PROFILE
        get_ns_time(&end_mode_check);
        get_ns_time(&start_spline);
#endif

        if(state->mode != LASER){
            if(state->nto_draw < DRAW_MAX
                    && ((state->background_cleared % 100 == 0)
                    || state->should_clear)){
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

            } else if(state->background_cleared % 100){
                current_segment = &state->segments_to_draw[state->nto_draw];
                if(current_segment->npositions > 4)
                    DrawSplineSegmentBasis(
                            current_segment->positions[current_segment->npositions-1],
                            current_segment->positions[current_segment->npositions-2],
                            current_segment->positions[current_segment->npositions-3],
                            current_segment->positions[current_segment->npositions-4],
                            current_segment->thickness, current_segment->color);
            }
        }
#ifdef DRAWOPAPI_PROFILE
        get_ns_time(&end_spline);
#endif


    EndDrawing();
    // gathering profiling data and logging.

#ifdef DRAWOPAPI_PROFILE
        get_ns_time(&end_tot);
        dt_tot = delta_time(end_tot,start_tot);
        dt_spline = delta_time(end_spline,start_spline);
        dt_help_window = delta_time(end_help_window,start_help_window);
        dt_mode_check = delta_time(end_mode_check,start_mode_check);

        TraceLog(LOG_INFO,"draw(&state) ==> dt_tot: %.2lfms dt_spline: %02lldns dt_help_window:%02lldns dt_mode_check:%02lldns",(double)dt_tot/1e6,dt_spline,dt_help_window,dt_mode_check);
#endif
}



