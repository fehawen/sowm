#ifndef CONFIG_H
#define CONFIG_H

#define MOD Mod4Mask

#define BORDER_COLOR "#eeeeee"
#define BORDER_WIDTH 4

const char* menu[]    = {"dmn",                     0};
const char* term[]    = {"st",                      0};
const char* scrot[]   = {"scr",                     0};
const char* clock[]   = {"clk",                     0};
const char* battery[] = {"bat", "notify",           0};
const char* palette[] = {"rpg", "g",                0};
const char* briup[]   = {"xbacklight", "-inc", "5", 0};
const char* bridown[] = {"xbacklight", "-dec", "5", 0};
const char* voldown[] = {"pamixer", "-d", "5",      0};
const char* volup[]   = {"pamixer", "-i", "5",      0};
const char* volmute[] = {"pamixer", "-t",           0};

static struct key keys[] = {
    {MOD,  XK_q,  win_kill,   {0}},
    {MOD,  XK_c,  win_center, {0}},
    {MOD,  XK_f,  win_fs,     {0}},
    {MOD,  XK_m,  win_mx,     {0}},

    {MOD,              XK_k,  win_move,  {.com = (const char*[]){"move",   "n"}, .i = 100}},
    {MOD,              XK_j,  win_move,  {.com = (const char*[]){"move",   "s"}, .i = 100}},
    {MOD,              XK_l,  win_move,  {.com = (const char*[]){"move",   "e"}, .i = 100}},
    {MOD,              XK_h,  win_move,  {.com = (const char*[]){"move",   "w"}, .i = 100}},

    {MOD|Mod1Mask,     XK_k,  win_move,  {.com = (const char*[]){"move",   "n"}, .i = 25}},
    {MOD|Mod1Mask,     XK_j,  win_move,  {.com = (const char*[]){"move",   "s"}, .i = 25}},
    {MOD|Mod1Mask,     XK_l,  win_move,  {.com = (const char*[]){"move",   "e"}, .i = 25}},
    {MOD|Mod1Mask,     XK_h,  win_move,  {.com = (const char*[]){"move",   "w"}, .i = 25}},

    {MOD|ShiftMask,    XK_k,  win_move,  {.com = (const char*[]){"resize", "n"}, .i = 100}},
    {MOD|ShiftMask,    XK_j,  win_move,  {.com = (const char*[]){"resize", "s"}, .i = 100}},
    {MOD|ShiftMask,    XK_l,  win_move,  {.com = (const char*[]){"resize", "e"}, .i = 100}},
    {MOD|ShiftMask,    XK_h,  win_move,  {.com = (const char*[]){"resize", "w"}, .i = 100}},

    {MOD|ControlMask,  XK_k,  win_move,  {.com = (const char*[]){"resize", "n"}, .i = 25}},
    {MOD|ControlMask,  XK_j,  win_move,  {.com = (const char*[]){"resize", "s"}, .i = 25}},
    {MOD|ControlMask,  XK_l,  win_move,  {.com = (const char*[]){"resize", "e"}, .i = 25}},
    {MOD|ControlMask,  XK_h,  win_move,  {.com = (const char*[]){"resize", "w"}, .i = 25}},

    {MOD,           XK_Tab, win_next,   {0}},
    {MOD|ShiftMask, XK_Tab, win_prev,   {0}},

    {MOD, XK_d,           run, {.com = menu}},
    {MOD, XK_p,           run, {.com = scrot}},
    {MOD, XK_Return,      run, {.com = term}},
    {MOD, XK_t,           run, {.com = clock}},
    {MOD, XK_b,           run, {.com = battery}},
    {MOD|ShiftMask, XK_d, run, {.com = palette}},

    {0,   XF86XK_AudioLowerVolume,  run, {.com = voldown}},
    {0,   XF86XK_AudioRaiseVolume,  run, {.com = volup}},
    {0,   XF86XK_AudioMute,         run, {.com = volmute}},
    {0,   XF86XK_MonBrightnessUp,   run, {.com = briup}},
    {0,   XF86XK_MonBrightnessDown, run, {.com = bridown}},

    {MOD,           XK_1, ws_go,     {.i = 1}},
    {MOD|ShiftMask, XK_1, win_to_ws, {.i = 1}},
    {MOD,           XK_2, ws_go,     {.i = 2}},
    {MOD|ShiftMask, XK_2, win_to_ws, {.i = 2}},
    {MOD,           XK_3, ws_go,     {.i = 3}},
    {MOD|ShiftMask, XK_3, win_to_ws, {.i = 3}},
    {MOD,           XK_4, ws_go,     {.i = 4}},
    {MOD|ShiftMask, XK_4, win_to_ws, {.i = 4}},
    {MOD,           XK_5, ws_go,     {.i = 5}},
    {MOD|ShiftMask, XK_5, win_to_ws, {.i = 5}},
    {MOD,           XK_6, ws_go,     {.i = 6}},
    {MOD|ShiftMask, XK_6, win_to_ws, {.i = 6}},
    {MOD,           XK_7, ws_go,     {.i = 7}},
    {MOD|ShiftMask, XK_7, win_to_ws, {.i = 7}},
    {MOD,           XK_8, ws_go,     {.i = 8}},
    {MOD|ShiftMask, XK_8, win_to_ws, {.i = 8}},
    {MOD,           XK_9, ws_go,     {.i = 9}},
    {MOD|ShiftMask, XK_9, win_to_ws, {.i = 9}},
};

#endif
