#ifndef _DRAWOP_API_H
#define _DRAWOP_API_H
#include <stdint.h>
#include <stddef.h>
#include <raylib.h>
#include <stddef.h>

extern uint32_t drawop_screen_used;
extern char drawop_char_pressed;
extern int screenWidth;
extern int screenHeight;

#define DEFAULT_RADIUS  10.0F

#define RADIUS_MAX      100
#define DRAW_MAX        1024
#define TEXT_BUFFER_MAX 1024
#define MAX_BUFFERS_TO_WRITE 100
#define SEG_MAX         128

#define DEFAULT_HIGHLIGHT_RADIUS_MULT   2
#define DEFAULT_HIGHLIGHT_ALPHA_DIV     1.5

/*** CONSTANTS ***/


extern const char* HELP_TEXT;
extern const Color HELP_COLOR_RECT;//(Color){0x0d,0x0c,0x0b,0xff};
extern const Color HELP_COLOR;//(Color){0xfd,0xfd,0xfd,0xff};



/*** STRUCTS ***/

typedef struct {
    const char* assets_dir;
    Color background;
    bool transparent;

    float highlight_radius_mult;
    float highlight_alpha_div;

    // Temporarily disabled because of bugs
    // const char* title;

    // Color title_text_color;
    // Color title_background_color;
    // 
    // bool show_title;

} drawop_configuration;


typedef struct {
    //current state base variables
    enum {
        LASER = 0,
        BRUSH,
        HIGHLIGHTER,
        TEXT
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

    struct drawop_text {
           Vector2 position;
           char buffer[TEXT_BUFFER_MAX];
           int bufsize;
           int fontsize;
    } text;

    struct drawop_text texts_to_write[MAX_BUFFERS_TO_WRITE];
    uint32_t ntexts_to_write;

    //color picker
    Vector2 pickColorLocation;
    bool pickColor;

    //signals for multiple-key input
    bool left_click_pressed;
    bool control_pressed;

    //help window
    bool help_window;
} drawop_state;



/*Actions to input into the drawop_updatestate(<arg1>,<arg2>,actions)
 * function. It contains the various actions, but for some actions,
 * you can use only one out of:
 *
 *  ==== actions to switch the drawing mode   =============================
 *  * DRAWOP_SWITCHTO_LASER
 *  * DRAWOP_SWITCHTO_BRUSH
 *  * DRAWOP_SWITCHTO_HIGHLIGHT
 *  * DRAWOP_SWITCHTO_TEXT
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
    DRAWOP_TOGGLE_TRANSPARENT  = 0x000004,
    DRAWOP_TOGGLE_HELP         = 0x000008,
    DRAWOP_SWITCHTO_LASER      = 0x000010, /*4. Cannot couple together*/
    DRAWOP_SWITCHTO_HIGHLIGHT  = 0x000020, /*5. ***********************/
    DRAWOP_SWITCHTO_BRUSH      = 0x000040, /*6. ***********************/
    DRAWOP_SWITCHTO_TEXT       = 0x000080, /*6. ***********************/
    DRAWOP_RESET               = 0x000200,
    DRAWOP_CONTROL_PRESSED     = 0x000400, /*8. Cannot couple together*/
    DRAWOP_CONTROL_RELEASE     = 0x000800, /*9. ***********************/
    DRAWOP_DELETE_SEQUENCE     = 0x001000, 
    DRAWOP_DRAW_ENGAGE         = 0x002000, /*11.Cannot couple together*/
    DRAWOP_DRAW_RELEASE        = 0x004000, /*12.***********************/
    DRAWOP_TEXT_WRITE_NEWLINE  = 0x010000,
    DRAWOP_TEXT_WRITE_CHAR     = 0x020000,
    DRAWOP_TEXT_WRITE_BACKSPACE= 0x040000
} drawop_action;



extern void drawop_init(int height, int width);
extern void drawop_undohighlight(drawop_state* state);
extern void drawop_updatestate(drawop_state state, drawop_state* endstate, drawop_action actions);

#endif
