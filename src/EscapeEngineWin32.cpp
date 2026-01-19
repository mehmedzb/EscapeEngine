#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <glad/glad.h>
#include <gl/GL.h>

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

LRESULT CALLBACK WndProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    if (Message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    /// windows window setup

    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hInstance = hInst;
    WindowClass.lpszClassName = "EscapeEngineWindowClass";
    RegisterClass(&WindowClass);

    HWND WindowHandle = CreateWindowEx(
        0, "EscapeEngineWindowClass", "Escape Engine",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr, nullptr, hInst, nullptr
    );

    HDC DeviceContext = GetDC(WindowHandle);

    PIXELFORMATDESCRIPTOR PixelFormatDesc = {};
    PixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    PixelFormatDesc.nVersion = 1;
    PixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    PixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDesc.cColorBits = 32;
    PixelFormatDesc.cDepthBits = 24;
    PixelFormatDesc.cStencilBits = 8;
    PixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(DeviceContext, &PixelFormatDesc);
    SetPixelFormat(DeviceContext, pixelFormat, &PixelFormatDesc);

    HGLRC TempRC = wglCreateContext(DeviceContext);
    wglMakeCurrent(DeviceContext, TempRC);

    if (!gladLoadGL())
    {
        MessageBoxA(WindowHandle, "Failed to load GLAD", "Error", MB_OK);
        return -1;
    }

    // Modern OpenGL 4.5 Context
    int attribs[] = {
        0x2091, 4, // WGL_CONTEXT_MAJOR_VERSION_ARB
        0x2092, 5, // WGL_CONTEXT_MINOR_VERSION_ARB
        0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB -> CORE
        0
    };

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

    HGLRC GLContext = wglCreateContextAttribsARB(DeviceContext, 0, attribs);

    // Switch to modern context
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(TempRC);
    wglMakeCurrent(DeviceContext, GLContext);

    /// windows window setup ending.


const char* vs = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fs = R"(
#version 450 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1, 0, 0, 1); // kırmızı
}
)";

GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vs, NULL);
glCompileShader(vertexShader);

GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fs, NULL);
glCompileShader(fragmentShader);

// Program link
GLuint program = glCreateProgram();
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);
glLinkProgram(program);

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

GLuint VAO, VBO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



    // Render Loop
    MSG Message = {};
    while (true)
    {
        while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
        {
            if (Message.message == WM_QUIT)
                return 0;
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(DeviceContext);
    }

    return 0;
}
