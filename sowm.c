// sowm - An itsy bitsy floating window manager.

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "sowm.h"

static client       *list = {0}, *ws_list[10] = {0}, *cur;
static int          ws = 1, sw, sh, wx, wy, numlock = 0;
static unsigned int ww, wh;

static int          s;
static Display      *d;
static XButtonEvent mouse;
static Window       root;

static void (*events[LASTEvent])(XEvent *e) = {
    [ButtonPress]      = button_press,
    [ButtonRelease]    = button_release,
    [ConfigureRequest] = configure_request,
    [KeyPress]         = key_press,
    [MapRequest]       = map_request,
    [DestroyNotify]    = notify_destroy,
    [EnterNotify]      = notify_enter,
    [MotionNotify]     = notify_motion
};

#include "config.h"

void title_add(client *c) {
    if (c->t) return;

    XClassHint cl;
    XGetClassHint(d, c->w, &cl);

    if (!strcmp(cl.res_name, "no-title")) return;

    win_size(c->w, &wx, &wy, &ww, &wh);
    c->t = XCreateSimpleWindow(d, root,
            wx, wy - TITLE_HEIGHT, ww + (BORDER_WIDTH * 2), TITLE_HEIGHT,
            0, getcolor(TITLE_COLOR), getcolor(TITLE_COLOR));

    XMapWindow(d, c->t);
}

void title_del(client *c) {
    if (!c->t) return;

    XUnmapWindow(d, c->t);
    XDestroyWindow(d, c->t);
    c->t = 0;
}

unsigned long getcolor(const char *col) {
    Colormap m = DefaultColormap(d, s);
    XColor c;
    return (!XAllocNamedColor(d, m, col, &c, &c))?0:c.pixel;
}

void win_move(const Arg arg) {
    int  r = arg.com[0][0] == 'r';
    char m = arg.com[1][0];

    title_del(cur);

    win_size(cur->w, &wx, &wy, &ww, &wh);

    XMoveResizeWindow(d, cur->w,
        wx + (r ? 0 : m == 'e' ?  arg.i : m == 'w' ? -arg.i : 0),
        wy + (r ? 0 : m == 'n' ? -arg.i : m == 's' ?  arg.i : 0),
        MAX(10, ww + (r ? m == 'e' ?  arg.i : m == 'w' ? -arg.i : 0 : 0)),
        MAX(10, wh + (r ? m == 'n' ? -arg.i : m == 's' ?  arg.i : 0 : 0)));

    title_add(cur);
}

void win_focus(client *c) {
    cur = c;
    XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
}

void notify_destroy(XEvent *e) {
    win_del(e->xdestroywindow.window);

    if (list) win_focus(list->prev);
}

void notify_enter(XEvent *e) {
    while(XCheckTypedEvent(d, EnterNotify, e));
    while(XCheckTypedWindowEvent(d, mouse.subwindow, MotionNotify, e));

    for win if (c->w == e->xcrossing.window) win_focus(c);
}

void notify_motion(XEvent *e) {
    if (!mouse.subwindow || cur->f) return;

    if (mouse.subwindow == cur->t) {
        mouse.subwindow = cur->w;
        win_size(cur->w, &wx, &wy, &ww, &wh);
    }

    while(XCheckTypedEvent(d, MotionNotify, e));

    int xd = e->xbutton.x_root - mouse.x_root;
    int yd = e->xbutton.y_root - mouse.y_root;

    XMoveResizeWindow(d, mouse.subwindow,
        wx + (mouse.button == 1 ? xd : 0),
        wy + (mouse.button == 1 ? yd : 0),
        MAX(1, ww + (mouse.button == 3 ? xd : 0)),
        MAX(1, wh + (mouse.button == 3 ? yd : 0)));

    if (cur->t) XMoveResizeWindow(d, cur->t,
        wx + (mouse.button == 1 ? xd : 0),
        wy + (mouse.button == 1 ? yd : 0) - TITLE_HEIGHT,
        MAX(1, ww + (mouse.button == 3 ? xd : 0)), TITLE_HEIGHT);
}

void key_press(XEvent *e) {
    KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

    for (unsigned int i=0; i < sizeof(keys)/sizeof(*keys); ++i)
        if (keys[i].keysym == keysym &&
            mod_clean(keys[i].mod) == mod_clean(e->xkey.state))
            keys[i].function(keys[i].arg);
}

void button_press(XEvent *e) {
    if (!e->xbutton.subwindow) return;

    win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
    XRaiseWindow(d, e->xbutton.subwindow);
    mouse = e->xbutton;
}

void button_release(XEvent *e) {
    mouse.subwindow = 0;
}

void win_add(Window w) {
    client *c;

    if (!(c = (client *) calloc(1, sizeof(client))))
        exit(1);

    c->w = w;

    if (list) {
        list->prev->next = c;
        c->prev          = list->prev;
        list->prev       = c;
        c->next          = list;

    } else {
        list = c;
        list->prev = list->next = list;
    }

    ws_save(ws);
}

void win_del(Window w) {
    client *x = 0;

    for win if (c->w == w) x = c;

    if (!list || !x)  return;
    if (x->prev == x) list = 0;
    if (list == x)    list = x->next;
    if (x->next)      x->next->prev = x->prev;
    if (x->prev)      x->prev->next = x->next;

    title_del(x);
    free(x);
    ws_save(ws);
}

void win_kill(const Arg arg) {
    if (cur) XKillClient(d, cur->w);
}

void win_center(const Arg arg) {
    if (!cur) return;

    win_size(cur->w, &(int){0}, &(int){0}, &ww, &wh);
    XMoveWindow(d, cur->w, (sw - ww) / 2, (sh - wh) / 2);

    if (cur->t) XMoveWindow(d, cur->t, (sw - ww) / 2, (sh - wh - TITLE_HEIGHT * 2) / 2);
}

void win_fs(const Arg arg) {
    if (!cur) return;

    if ((cur->f = cur->f ? 0 : 1)) {
        win_size(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);
        XMoveResizeWindow(d, cur->w, 0, 0, sw, sh);

        XRaiseWindow(d, cur->w);
        title_del(cur);
    } else {
        XMoveResizeWindow(d, cur->w, cur->wx, cur->wy, cur->ww, cur->wh);
        title_add(cur);
    }
}

void win_to_ws(const Arg arg) {
    int tmp = ws;

    if (arg.i == tmp) return;

    ws_sel(arg.i);
    win_add(cur->w);
    ws_save(arg.i);

    ws_sel(tmp);
    win_del(cur->w);
    XUnmapWindow(d, cur->w);
    title_del(cur);
    ws_save(tmp);

    if (list) win_focus(list);
}

void win_prev(const Arg arg) {
    if (!cur) return;

    XRaiseWindow(d, cur->prev->w);

    if (cur->prev->t)
        XRaiseWindow(d, cur->prev->t);

    win_focus(cur->prev);
}

void win_next(const Arg arg) {
    if (!cur) return;

    XRaiseWindow(d, cur->next->w);

    if (cur->next->t)
        XRaiseWindow(d, cur->next->t);

    win_focus(cur->next);
}

void ws_go(const Arg arg) {
    int tmp = ws;

    if (arg.i == ws) return;

    ws_save(ws);
    ws_sel(arg.i);

    for win {
        XMapWindow(d, c->w);
        title_add(c);
    }

    ws_sel(tmp);

    for win {
        XUnmapWindow(d, c->w);
        title_del(c);
    }

    ws_sel(arg.i);

    if (list) win_focus(list); else cur = 0;
}

void configure_request(XEvent *e) {
    XConfigureRequestEvent *ev = &e->xconfigurerequest;

    XConfigureWindow(d, ev->window, ev->value_mask, &(XWindowChanges) {
        .x          = ev->x,
        .y          = ev->y,
        .width      = ev->width,
        .height     = ev->height,
        .sibling    = ev->above,
        .stack_mode = ev->detail
    });
}

void map_request(XEvent *e) {
    Window w = e->xmaprequest.window;

    XSelectInput(d, w, StructureNotifyMask|EnterWindowMask);
    win_size(w, &wx, &wy, &ww, &wh);
    win_add(w);
    cur = list->prev;
    XSetWindowBorder(d, w, getcolor(BORDER_COLOR));
    XConfigureWindow(d, w, CWBorderWidth, &(XWindowChanges){.border_width = BORDER_WIDTH});

    if (wx + wy == 0) win_center((Arg){0});

    XMapWindow(d, w);
    win_focus(list->prev);
    title_add(cur);
}

void run(const Arg arg) {
    if (fork()) return;
    if (d) close(ConnectionNumber(d));

    setsid();
    execvp((char*)arg.com[0], (char**)arg.com);
}

void input_grab(Window root) {
    unsigned int i, j, modifiers[] = {0, LockMask, numlock, numlock|LockMask};
    XModifierKeymap *modmap = XGetModifierMapping(d);
    KeyCode code;

    for (i = 0; i < 8; i++)
        for (int k = 0; k < modmap->max_keypermod; k++)
            if (modmap->modifiermap[i * modmap->max_keypermod + k]
                == XKeysymToKeycode(d, 0xff7f))
                numlock = (1 << i);

    for (i = 0; i < sizeof(keys)/sizeof(*keys); i++)
        if ((code = XKeysymToKeycode(d, keys[i].keysym)))
            for (j = 0; j < sizeof(modifiers)/sizeof(*modifiers); j++)
                XGrabKey(d, code, keys[i].mod | modifiers[j], root,
                        True, GrabModeAsync, GrabModeAsync);

    for (i = 1; i < 4; i += 2)
        for (j = 0; j < sizeof(modifiers)/sizeof(*modifiers); j++)
            XGrabButton(d, i, MOD | modifiers[j], root, True,
                ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                GrabModeAsync, GrabModeAsync, 0, 0);

    XFreeModifiermap(modmap);
}

int main(void) {
    XEvent ev;

    if (!(d = XOpenDisplay(0))) exit(1);

    signal(SIGCHLD, SIG_IGN);
    XSetErrorHandler(xerror);

    root  = RootWindow(d, s);
    sw    = XDisplayWidth(d, s) - (2*BORDER_WIDTH);
    sh    = XDisplayHeight(d, s) - (2*BORDER_WIDTH);

    XSelectInput(d,  root, SubstructureRedirectMask);
    XDefineCursor(d, root, XCreateFontCursor(d, 68));
    input_grab(root);

    while (1 && !XNextEvent(d, &ev)) // 1 && will forever be here.
        if (events[ev.type]) events[ev.type](&ev);
}
