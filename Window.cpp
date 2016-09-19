#include "Window.h"
Window::Window(){
    mHDC = NULL;
    mHRC = NULL;
    hWnd = NULL;
    active = true;
    fullscreen = false;
}

Window::~Window(){
    if(fullscreen){
        ChangeDisplaySettings(NULL, 0);
        ShowCursor(true);
    }
    if(mHRC){
        if(!wglMakeCurrent(NULL, NULL)){
            MessageBox(NULL, "Release of DC and RC failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }
        if(!wglDeleteContext(mHRC)){
            MessageBox(NULL, "Release of rendering context failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }
        mHRC = NULL;
    }
    if(mHDC and !ReleaseDC(hWnd, mHDC)){
        MessageBox(NULL, "Release of device context failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        mHDC = NULL;
    }
    if(hWnd and !DestroyWindow(hWnd)){
        MessageBox(NULL, "Release of hWnd failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;
    }
    if(!UnregisterClass("OpenGL", hInstance)){
        MessageBox(NULL, "Unregistration of Class has failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;
    }
}

GLvoid Window::resizeScene(GLsizei w, GLsizei h){
    if(h == 0){
        h = 1;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat) w / (GLfloat) h, 0.1f, 100.0f );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int Window::InitGL(){
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    return TRUE;
}

BOOL Window::CreateGL(std::string title, int w, int h, int bits, bool fScreen){
    GLuint PxForm;
    WNDCLASS wc;
    DWORD dwExStyle;
    DWORD dwStyle;
    RECT wRect;
    wRect.left = (long) 0;
    wRect.right = (long) w;
    wRect.bottom = (long) h;
    wRect.top = (long) 0;
    fullscreen = fScreen;
    hInstance = GetModuleHandle(NULL);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) &WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "OpenGL";
    if(!RegisterClass(&wc)){
        MessageBox(NULL, "Failed to register wc", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if(fullscreen){
        DEVMODE dmSS;
        memset(&dmSS, 0, sizeof(dmSS));
        dmSS.dmSize = sizeof(dmSS);
        dmSS.dmPelsWidth = w;
        dmSS.dmPelsHeight = h;
        dmSS.dmBitsPerPel = bits;
        dmSS.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if(ChangeDisplaySettings(&dmSS, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL){
            MessageBox(NULL, "Fullscreen not supported on device. Switching to windowed instead...", "OpenGL", MB_OK | MB_ICONEXCLAMATION);
            fullscreen = false;
        }
    }

    if(fullscreen){
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP;
    }else{
        dwExStyle = WS_EX_APPWINDOW || WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW;
    }

    AdjustWindowRectEx(&wRect, dwStyle, FALSE, dwExStyle);

    if(!SUCCEEDED(hWnd = CreateWindowEx(dwExStyle, "OpenGL", title.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, NULL, NULL, hInstance, NULL))){
        delete this;
        MessageBox(NULL, "Creation of hWnd has failed", "ERROR", MB_OK);
        return FALSE;
    }

    static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, bits, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    if(!(mHDC = GetDC(hWnd))){
        delete this;
        MessageBox(NULL, "Can't create a GL device context", "OpenGL", MB_OK);
        return FALSE;
    }

    if(!(PxForm = ChoosePixelFormat(mHDC, &pfd))){
        delete this;
        MessageBox(NULL, "Can't create pixelformat", "OpenGL", MB_OK);
        return FALSE;
    }

    if(!SetPixelFormat(mHDC, PxForm, &pfd)){
        delete this;
        MessageBox(NULL, "Can't set pixel format", "OpenGL", MB_OK);
        return FALSE;
    }

    if(!(mHRC = wglCreateContext(mHDC))){
        delete this;
        MessageBox(NULL, "Can't create a GL context", "OpenGL", MB_OK);
        return FALSE;
    }

    if(!wglMakeCurrent(mHDC, mHRC)){
        delete this;
        MessageBox(NULL, "Can't activate GL context", "OpenGL", MB_OK);
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    resizeScene(w, h);

    if(!InitGL()){
        delete this;
        MessageBox(NULL, "Init failed", "OpenGL", MB_OK);
        return FALSE;
    }
    return TRUE;
}


LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_ACTIVATE:{
            if(!HIWORD(wParam)){
                active = true;
            }else{
                active = false;
            }
            return 0;
        }
        case WM_CLOSE:{
            PostQuitMessage(0);
            return 0;
        }
        case WM_KEYDOWN:{
            keys[wParam] = true;
            return 0;
        }
        case WM_KEYUP:{
            keys[wParam] = false;
            return 0;
        }
        case WM_SIZE:{
            resizeScene(LOWORD(lParam),HIWORD(lParam));
            return 0;
        }

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
