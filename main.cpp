#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>

#include "scene/Scene.h"
#include "core/Input.h"
#include "utils/Constants.h"

// global state
static Scene  gScene;
static int    gLastTime = 0;

// initialization functions
void initializeGLUT(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(Constants::DefaultWidth, Constants::DefaultHeight);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    std::string title = "SundownDrive v" + std::string(PROJECT_VERSION);
    glutCreateWindow(title.c_str());
}

void initializeGLEW() {
    // initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewErr) << std::endl;
        return;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
}

// GLUT callbacks
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
}

void display() {
    // compute delta time
    int now = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (now - gLastTime) / 1000.0f;
    gLastTime = now;

    // clamp to avoid spiral-of-death on breakpoints / lag spikes
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    gScene.update(deltaTime);
    gScene.draw();

    glutSwapBuffers();
}

// entry point
int main(int argc, char** argv) {
    initializeGLUT(argc, argv);
    initializeGLEW();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(glutPostRedisplay);

    Input::init();

    if (!gScene.init()) {
        std::cerr << "Scene initialization failed!" << std::endl;
        return 1;
    }

    gLastTime = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();
    return 0;
}