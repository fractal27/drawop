#include <raylib.h>
#include <stdbool.h>
#include <math.h>

#include "drawop.h"

//int main(int argc, char** argv){
int main(){
    drawop_init(0,0);
    drawop_state current_state = {
           .color = (Color){0x64,0x95,0xED,0xFF},
           .radius = DEFAULT_RADIUS,
           .mode = LASER,
           .segments_to_draw[0].color = current_state.color,
           .segments_to_draw[0].thickness = current_state.radius,
           .help_window = true,
           .alpha = 0xff,
           .config = {
                  .highlight_alpha_div = DEFAULT_HIGHLIGHT_ALPHA_DIV,
                  .highlight_radius_mult = DEFAULT_HIGHLIGHT_RADIUS_MULT,
           },
           .text.bufsize = 0,
           .text.fontsize = 30
    };
    // current_state.config.title = "drawop 0.2.3";
    // current_state.config.title_text_color = (Color){
    //     0xff,0x00,0x00,0xff
    // };
    // current_state.config.show_title = true;
    // current_state.config.title_background_color = (Color){
    //     0x00,0x00,0x00,0x88
    // };

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT); // Configures window to be transparent
    InitWindow(screenWidth, screenHeight, "Transparent");
    // SetWindowPosition((GetMonitorWidth(drawop_screen_used)  - screenWidth) / 2, (GetMonitorHeight(drawop_screen_used) - screenHeight ) / 2);
    SetWindowPosition(0,0);
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

