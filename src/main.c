#include <raylib.h>
#include <stdbool.h>
#include <math.h>

#include "drawop.h"

//int main(int argc, char** argv){
int main(){
    drawop_state current_state = {};
    current_state.color = (Color){0xff,0,0,0xff};
    current_state.radius = DEFAULT_RADIUS;
    current_state.mode = LASER;
    current_state.segments_to_draw[0].color = current_state.color;
    current_state.segments_to_draw[0].thickness = current_state.radius;
    current_state.help_window = true;
    current_state.alpha = 0xff;
    current_state.config.title = "drawop 0.2.3";
    current_state.config.title_text_color = (Color){
        0xff,0x00,0x00,0xff
    };
    current_state.config.show_title = true;
    current_state.config.title_background_color = (Color){
        0x00,0x00,0x00,0x88
    };
    current_state.config.highlight_alpha_div = DEFAULT_HIGHLIGHT_ALPHA_DIV;
    current_state.config.highlight_radius_mult= DEFAULT_HIGHLIGHT_RADIUS_MULT;

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
    InitWindow(screenWidth, screenHeight, "Transparent");
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    //SetWindowState(FLAG_WINDOW_UNDECORATED); // Hide border/titlebar; omit if you want them there.

    SetExitKey(KEY_Q);
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        drawop_action action = get_action(current_state);
        update(current_state,&current_state,action);
        draw(&current_state);
    }

    //UnloadRenderTexture(target);
    CloseWindow();
}

