#include "glew.h"
#include "stb_image.h"
#include <iostream>

class ImageSquare {
public:
    ImageSquare(const char* imagePath) : textureID(0), VAO(0), VBO(0), EBO(0) {
        // Load the texture
        textureID = loadTexture(imagePath);
        if (!textureID) {
            std::cerr << "Failed to load texture: " << imagePath << std::endl;
            return;
        }

        // Initialize buffers
        initializeBuffers();
    }

    ~ImageSquare() {
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (textureID) glDeleteTextures(1, &textureID);
    }

    void render(GLuint shaderProgram) {
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);
    }

private:
    GLuint VAO, VBO, EBO;
    GLuint textureID;

    void initializeBuffers() {
        float vertices[] = {
            // Positions       // Texture Coords
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top Right
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom Right
           -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom Left
           -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top Left 
        };

        unsigned int indices[] = {
            0, 1, 3, // First triangle
            1, 2, 3  // Second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // 3 * sizeof(float) offset for texture coords
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    unsigned int loadTexture(const char* path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};
