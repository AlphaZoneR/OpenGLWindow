#include "Window.h"


Window * window;
MSG msg;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLinde, int nCmdShow){
    window = new Window();
    if(MessageBox(NULL, "Would you like to start the program in full screen or not?", "OpenGL", MB_YESNO | MB_ICONINFORMATION) == IDNO){
        window->fullscreen = false;
    }else{
        window->fullscreen = true;
    }
    if(!window->CreateGL("OpenGL", 1280, 720, 16, window->fullscreen)){
        cout << "Error" << endl;
        return 0;
    }
    while(true){
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            if (msg.message == WM_QUIT){
                PostQuitMessage(0);
                break;
            }else{
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }else{
            window->Draw();
            window->Update();
            SwapBuffers(window->mHDC);
        }
    }
    delete window;
    return (msg.wParam);

}

GLvoid Window::Draw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

GLvoid Window::Update(){


}
