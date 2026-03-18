#ifndef SUNDOWNDRIVE_CORE_INPUT_H
#define SUNDOWNDRIVE_CORE_INPUT_H

#include <GL/freeglut.h>

// Captures GLUT keyboard/mouse callbacks into a queryable state map
class Input {
public:
    static void init();

    // key state queries
    static bool isKeyDown(unsigned char key);
    static bool isSpecialKeyDown(int key);

    // mouse state
    static int  getMouseDeltaX();
    static int  getMouseDeltaY();
    static void resetMouseDelta();

    // mouse capture: warps cursor to window center each frame for unlimited turning
    static void setMouseCaptured(bool captured);
    static bool isMouseCaptured();

    // edge-detected key press (true only once per press)
    static bool isKeyPressed(unsigned char key);

private:
    static void keyDownCallback(unsigned char key, int x, int y);
    static void keyUpCallback(unsigned char key, int x, int y);
    static void specialDownCallback(int key, int x, int y);
    static void specialUpCallback(int key, int x, int y);
    static void mouseMotionCallback(int x, int y);

    static bool keys_[256];
    static bool specialKeys_[256];
    static int  mouseDX_, mouseDY_;
    static bool firstMouse_;
    static bool mouseCaptured_;
    static bool prevKeys_[256];    // previous frame key state for edge detection
};

#endif
