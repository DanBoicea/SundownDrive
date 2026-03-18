#include "core/Input.h"
#include <cstring>

bool Input::keys_[256]        = {};
bool Input::specialKeys_[256] = {};
bool Input::prevKeys_[256]    = {};
int  Input::mouseDX_ = 0;
int  Input::mouseDY_ = 0;
bool Input::firstMouse_ = true;
bool Input::mouseCaptured_ = true;

void Input::init() {
    std::memset(keys_, 0, sizeof(keys_));
    std::memset(specialKeys_, 0, sizeof(specialKeys_));

    glutKeyboardFunc(keyDownCallback);
    glutKeyboardUpFunc(keyUpCallback);
    glutSpecialFunc(specialDownCallback);
    glutSpecialUpFunc(specialUpCallback);
    glutPassiveMotionFunc(mouseMotionCallback);
    glutMotionFunc(mouseMotionCallback);

    glutSetCursor(GLUT_CURSOR_NONE);
}

bool Input::isKeyDown(unsigned char key)  { return keys_[key]; }
bool Input::isSpecialKeyDown(int key)     { return specialKeys_[key & 0xFF]; }
int  Input::getMouseDeltaX()              { return mouseDX_; }
int  Input::getMouseDeltaY()              { return mouseDY_; }
void Input::resetMouseDelta() {
    mouseDX_ = 0; mouseDY_ = 0;
    std::memcpy(prevKeys_, keys_, sizeof(keys_));
}

// GLUT callback implementations
void Input::keyDownCallback(unsigned char key, int, int)  { keys_[key] = true; }
void Input::keyUpCallback(unsigned char key, int, int)    { keys_[key] = false; }
void Input::specialDownCallback(int key, int, int)        { specialKeys_[key & 0xFF] = true; }
void Input::specialUpCallback(int key, int, int)          { specialKeys_[key & 0xFF] = false; }

void Input::mouseMotionCallback(int x, int y) {
    if (!mouseCaptured_) return;

    int cx = glutGet(GLUT_WINDOW_WIDTH)  / 2;
    int cy = glutGet(GLUT_WINDOW_HEIGHT) / 2;

    // ignore the warp-back event (cursor already at center)
    if (x == cx && y == cy) return;

    if (firstMouse_) {
        firstMouse_ = false;
        glutWarpPointer(cx, cy);
        return;
    }

    // delta is displacement from center
    mouseDX_ += x - cx;
    mouseDY_ += cy - y;   // inverted Y: screen down is negative look

    // warp back to center immediately
    glutWarpPointer(cx, cy);
}

void Input::setMouseCaptured(bool captured) {
    mouseCaptured_ = captured;
    if (captured) {
        glutSetCursor(GLUT_CURSOR_NONE);
        firstMouse_ = true;  // avoid a jump when re-capturing
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}

bool Input::isMouseCaptured() { return mouseCaptured_; }

bool Input::isKeyPressed(unsigned char key) {
    return keys_[key] && !prevKeys_[key];
}
