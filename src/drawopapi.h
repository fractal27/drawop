#ifndef _DRAWOP_API_H
#define _DRAWOP_API_H
#include <stddef.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

//#define DRAWOPAPI_PROFILE
#define DRAWOPAPI_NO_LOG

#ifdef DRAWOPAPI_NO_LOG
#define TraceLog(...) do {} while(0);
#endif


#ifdef DRAWOPAPI_PROFILE
#define get_ns_time(start) clock_gettime(CLOCK_REALTIME,start)
#define delta_time(after,before) (int64_t)(after.tv_sec - before.tv_sec) * (int64_t)1e9\
                                        + (int64_t)(after.tv_nsec - before.tv_nsec)
#else
#define get_ns_time(start)
#define delta_time(after,before)
#endif

#define screenWidth     1920
#define screenHeight    1080
#define DEFAULT_RADIUS  10.0F

#define RADIUS_MAX      100
#define DRAW_MAX        1024
#define SEG_MAX         128

#define DEFAULT_HIGHLIGHT_RADIUS_MULT   2
#define DEFAULT_HIGHLIGHT_ALPHA_DIV     1.5

/*** CONSTANTS ***/


extern const char* HELP_TEXT;
extern const Color HELP_COLOR_RECT;//(Color){0x0d,0x0c,0x0b,0xff};
extern const Color HELP_COLOR;//(Color){0xfd,0xfd,0xfd,0xff};



/*** STRUCTS ***/

typedef struct {
    Color background;
    bool transparent;

    float highlight_radius_mult;
    float highlight_alpha_div;

    const char* title;

    Color title_text_color;
    Color title_background_color;
    
    bool show_title;

} drawop_configuration;


typedef struct {
    //current state base variables
    enum {
        LASER,
        BRUSH,
        HIGHLIGHTER
    } mode;

    Vector2 mouse_position;
    float alpha;
    double radius;
    Color color;


    //configurations and things to draw.
    drawop_configuration config;

    struct segment{
        Vector2 positions[DRAW_MAX];
        size_t npositions;
        float thickness;
        Color color;
    } segments_to_draw[SEG_MAX];
    size_t nto_draw;

    //color picker
    Vector2 pickColorLocation;
    bool pickColor;

    //signals for multiple-key input
    bool left_click_pressed;
    bool control_pressed;
    bool should_clear;

    //help window
    bool help_window;
    int64_t background_cleared;

} drawop_state;



/*Actions to input into the drawop_updatestate(<arg1>,<arg2>,actions)
 * function. It contains the various actions, but for some actions,
 * you can use only one out of:
 *
 *  ==== actions to switch the drawing mode   =============================
 *  * DRAWOP_SWITCHTO_LASER
 *  * DRAWOP_SWITCHTO_BRUSH
 *  * DRAWOP_SWITCHTO_HIGHLIGHT
 *
 *  ==== actions to change the pressed and released state of control ======
 *  * DRAWOP_CONTROL_PRESSED
 *  * DRAWOP_CONTROL_RELEASED
 *
 *  ==== actions to engage(continue drawing) or release a sequence ========
 *  * DRAWOP_DRAW_ENGAGE
 *  * DRAWOP_DRAW_RELEASE
 *
 *  If you mitigate this rule, the api might break.
 *
 * Example:
 *   //updates the state of the alpha and switches mode to laser
 *   drawop_updatestate(prevstate,&state,DRAWOP_UPDATE_ALPHA | DRAWOP_SWITCHTO_LASER);
 * */


typedef enum {                             /*TLDR;*/
    DRAWOP_UPDATE_ALPHA        = 0x000001,
    DRAWOP_TOGGLE_COLOR_PICKER = 0x000002,
    DRAWOP_TOGGLE_HELP         = 0x000004,
    DRAWOP_TOGGLE_TRANSPARENT  = 0x000008,
    DRAWOP_SWITCHTO_LASER      = 0x000010, /*4. Cannot couple together*/
    DRAWOP_SWITCHTO_HIGHLIGHT  = 0x000020, /*5. ***********************/
    DRAWOP_SWITCHTO_BRUSH      = 0x000040, /*6. ***********************/
    DRAWOP_RESET               = 0x000200,
    DRAWOP_CONTROL_PRESSED     = 0x000400, /*8. Cannot couple together*/
    DRAWOP_CONTROL_RELEASE     = 0x000800, /*9. ***********************/
    DRAWOP_DELETE_SEQUENCE     = 0x001000, 
    DRAWOP_DRAW_ENGAGE         = 0x002000, /*11.Cannot couple together*/
    DRAWOP_DRAW_RELEASE        = 0x004000, /*12.***********************/
    DRAWOP_CLEAR_NOW           = 0x008000,
    DRAWOP_SCREENSHOT          = 0x010000,
} drawop_action;



extern void drawop_undohighlight(drawop_state* state);
extern void drawop_updatestate(drawop_state state, drawop_state* endstate, drawop_action actions);

#endif
