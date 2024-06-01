#include "Program.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "includes/imgui.h"
#include "includes/imgui_impl_opengl3.h"
#include "includes/imgui_impl_glfw.h"
#include "Structures.h"
#include "iostream"

#define _CRT_SECURE_NO_WARNINGS
#include <vector>

struct Timer {
    std::chrono::steady_clock::time_point lastUpdate;
    float deltaTime = 0.0; // Delta time in seconds

    // Initializes the timer with the current time
    Timer() : lastUpdate(std::chrono::steady_clock::now()) {}

    // Updates the delta time
    void update() {
        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(now - lastUpdate).count();
        lastUpdate = now;
    }

    // Get the current delta time
    float getDeltaTime() const {
        return deltaTime;
    }
};

Program::Program() : shaderProgram(0), fbo(0), tex(0)
{
}

int Program::Init()
{
    m_windowWidth = 1280;
    m_windowHeight = 720;

    if (!glfwInit())
        return -1;

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Hello World", NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window." << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(m_window);
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.WindowPadding = ImVec2(0.0f, 0.0f);
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_windowWidth, m_windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);

    // Create multiple ImageSquare instances
    squares.emplace_back("Cheems.jpg");
    squares.emplace_back("AnotherImage.jpg");

    return 0;
}

void Program::Loop()
{
    Timer timer;
    timer.update();
    float timeSinceRender = 0.0f;
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        timer.update();

        timeSinceRender += timer.getDeltaTime();
        if (timeSinceRender >= 1.0f / 60.0f) {
            Render();
            timeSinceRender = 0.0f;
        }
    }
}

void Program::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, m_windowWidth, m_windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& square : squares) {
        square.render(shaderProgram);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    ImGui::Begin("GameWindow");
    {
        ImGui::BeginChild("GameRender");
        ImVec2 wsize = ImGui::GetWindowSize();
        ImGui::Image((ImTextureID)(intptr_t)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();

    ImGui::Begin("NotGameWindow");
    {
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(m_window);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << std::hex << error << std::endl;
    }
}

void Program::CleanUp()
{
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
    }
    if (tex) {
        glDeleteTextures(1, &tex);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

unsigned int Program::compileShader(const char* source, GLenum shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int Program::createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

const char* Program::vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* Program::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";
