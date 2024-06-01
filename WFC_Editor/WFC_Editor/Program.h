#include "glew.h"
#include "glfw3.h"
#include"gl/GL.h"
#include <vector>
#include "imageSqaure.h"

class Program
{
public:
    Program();
    int Init();
    void Loop();
    void Render();
    void CleanUp();
    unsigned int loadTexture(const char* path);
    unsigned int compileShader(const char* source, GLenum shaderType);
    unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource); 
    bool setupFramebuffer();
private:
    int m_windowWidth;
    int m_windowHeight;

    GLFWwindow* m_window;

    GLuint fbo;  // Framebuffer object handle
    GLuint tex; // Declare a texture object
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;

    GLuint tileTex; // Declare a texture object
    GLuint shaderProgram;

    std::vector<ImageSquare> squares;

    unsigned int rbo;
};
