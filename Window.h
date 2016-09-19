#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
using namespace std;
class Window{
public:
    HDC mHDC;
    HGLRC mHRC;
    HWND hWnd;
    HINSTANCE hInstance;
    bool keys[256];
    bool active;
    bool fullscreen;
    Window();
    ~Window();
    GLvoid resizeScene(GLsizei w, GLsizei h);
    int InitGL();
    CreateGL(std::string title, int w, int h, int bits, bool fScreen);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    GLvoid Draw();
    GLvoid Update();
};
