
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
 
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

//Window
GLFWwindow* window;
unsigned int wWidth = 1500;
unsigned int wHeight = 800;
const char wTitle[] = "Pyramids";

//Shaders
unsigned int backgroundShader;
unsigned int textureShader;
unsigned int groundShader;
unsigned int pyramidShader;
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

//Textures
unsigned bgTexture;
unsigned indexTexture;
unsigned groundTexture;
unsigned pyramidTexture;
static unsigned loadImageToTexture(const char* filePath);
void setupTexture(unsigned int texture);
void renderTexture(GLuint shaderProgram, GLuint textureID);

//Data
unsigned int bgVAO, bgVBO;
unsigned int indexVAO, indexVBO;
unsigned int groundVAO, groundVBO;
unsigned int pyramidVAO, pyramidVBO;

//MVP
unsigned int modelGroundLoc;
unsigned int modelPyramidLoc;

glm::mat4 view;
unsigned int viewGroundLoc;
unsigned int viewPyramidLoc;

glm::mat4 projectionP;
glm::mat4 projectionO;
unsigned int projectionGroundLoc;
unsigned int projectionPyramidLoc;
void setMVPUniforms(GLuint shader, GLuint modelLoc, const glm::mat4& model,
    GLuint viewLoc, GLuint projLoc);


//Central Pyramid
glm::vec3 pyramidPosition = glm::vec3(0.0f, 0.0f, 0.0f);
float pyramidScale = 2.0f;
bool usePerspective = true;

void setBackgroundVAO();

void setBackgroundVAO() {
    float quadVertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        -1.0f, -1.0f,   0.0f, 0.0f,  // bottom-left
         1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right

        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
         1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
         1.0f,  1.0f,   1.0f, 1.0f,   // top-right
    };

        glGenVertexArrays(1, &bgVAO);
        glGenBuffers(1, &bgVBO);

        glBindVertexArray(bgVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position Attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); 
        glEnableVertexAttribArray(0);

        // Texture Coord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); 
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
};

void setGroundVAO();

void setGroundVAO() {
    float vertices[] = {
        //triangle 1
        -5.0f, 0.0f, -5.0f,    0.0f, 0.0f,
         5.0f, 0.0f,  5.0f,    1.0f, 1.0f,
         5.0f, 0.0f, -5.0f,    1.0f, 0.0f,

         //triangle 2
         -5.0f, 0.0f, -5.0f,    0.0f, 0.0f,
         -5.0f, 0.0f,  5.0f,    0.0f, 1.0f,
          5.0f, 0.0f,  5.0f,    1.0f, 1.0f
    };

    unsigned int stride = (3 + 2) * sizeof(float);

    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void setPyramidVAO();

void setPyramidVAO() {
    float vertices[] = {
         // Side 1
         0.0f, 1.0f, 0.0f,     0.5f, 1.0f,  // top
        -0.5f, 0.0f, -0.5f,    0.0f, 0.0f,  // left
         0.5f, 0.0f, -0.5f,    1.0f, 0.0f,  // right

         // Side 2
         0.0f, 1.0f, 0.0f,     0.5f, 1.0f,
         0.5f, 0.0f, -0.5f,    0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,    1.0f, 0.0f,

         // Side 3
         0.0f, 1.0f, 0.0f,     0.5f, 1.0f,
         0.5f, 0.0f, 0.5f,     0.0f, 0.0f,
        -0.5f, 0.0f, 0.5f,     1.0f, 0.0f,

         // Side 4
         0.0f, 1.0f, 0.0f,     0.5f, 1.0f,
        -0.5f, 0.0f, 0.5f,     0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,    1.0f, 0.0f,

        // Base - triangle 1
        -0.5f, 0.0f, -0.5f,    0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,    1.0f, 0.0f,
         0.5f, 0.0f, 0.5f,     1.0f, 1.0f,

        // Base - triangle 2
        -0.5f, 0.0f, -0.5f,    0.0f, 0.0f,
         0.5f, 0.0f, 0.5f,     1.0f, 1.0f,
        -0.5f, 0.0f, 0.5f,     0.0f, 1.0f,
    };


    unsigned int stride = (3+2) * sizeof(float);

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void setIndexVAO();

void setIndexVAO() {
    float vertices[] = {
    -0.95f,  0.95f,  0.0f, 1.0f,  
    -0.95f,  0.70f,  0.0f, 0.0f,  
    -0.65f,  0.70f,  1.0f, 0.0f,  

    -0.65f,  0.70f,  1.0f, 0.0f,  
    -0.65f,  0.95f,  1.0f, 1.0f,  
    -0.95f,  0.95f,  0.0f, 1.0f   
    };

    glGenVertexArrays(1, &indexVAO);
    glGenBuffers(1, &indexVBO);

    glBindVertexArray(indexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, indexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void renderIndexTexture() {

        glUseProgram(textureShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glUniform1i(glGetUniformLocation(textureShader, "ourTexture"), 0);

        glBindVertexArray(indexVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    
}

void renderBackground() {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(backgroundShader);
    glBindVertexArray(bgVAO);
    renderTexture(backgroundShader, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void renderGround() {
    glUseProgram(groundShader);
    glBindVertexArray(groundVAO);

    renderTexture(groundShader, groundTexture);

    glm::mat4 modelGround = glm::mat4(1.0f);
    setMVPUniforms(groundShader, modelGroundLoc, modelGround, viewGroundLoc, projectionGroundLoc);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderPyramids() {
    glUseProgram(pyramidShader);
    glBindVertexArray(pyramidVAO);
    renderTexture(pyramidShader, pyramidTexture);

    // Biggest center pyramid
    glm::mat4 modelPyramid1 = glm::mat4(1.0f);
    modelPyramid1 = glm::translate(modelPyramid1, pyramidPosition);
    modelPyramid1 = glm::scale(modelPyramid1, glm::vec3(pyramidScale));

    setMVPUniforms(pyramidShader, modelPyramidLoc, modelPyramid1, viewPyramidLoc, projectionPyramidLoc);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    // Smaller pyramid 1
    glm::mat4 modelPyramid2 = glm::mat4(1.0f);
    modelPyramid2 = glm::translate(modelPyramid2, glm::vec3(-3.0f, 0.0f, -1.0f));

    setMVPUniforms(pyramidShader, modelPyramidLoc, modelPyramid2, viewPyramidLoc, projectionPyramidLoc);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    // Smaller pyramid 2
    glm::mat4 modelPyramid3 = glm::mat4(1.0f);
    modelPyramid3 = glm::translate(modelPyramid3, glm::vec3(2.5f, 0.0f, 3.0f));
    modelPyramid3 = glm::scale(modelPyramid3, glm::vec3(1.2f, 1.2f, 1.2f));

    setMVPUniforms(pyramidShader, modelPyramidLoc, modelPyramid3, viewPyramidLoc, projectionPyramidLoc);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(0);
}

void setMVPUniforms(GLuint shader, GLuint modelLoc, const glm::mat4& model,
    GLuint viewLoc, GLuint projLoc) {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(usePerspective ? projectionP : projectionO));
}

void renderTexture(GLuint shaderProgram, GLuint textureID) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint uniformLocation = glGetUniformLocation(shaderProgram, "uTexture");
    glUniform1i(uniformLocation, 0);
}

int initWindow() {
    if (!glfwInit())
    {
        std::cout << "GLFW Library was not loaded! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "The window was not created.! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW could not be loaded.! :'(\n";
        return 3;
    }
}


int main(void)
{
    initWindow();
    
    //Shaders
    backgroundShader = createShader("texture.vert", "texture.frag");
    textureShader = createShader("texture.vert", "texture.frag");
    pyramidShader = createShader("basic.vert", "basic.frag");
    groundShader = createShader("basic.vert", "basic.frag");

    //Set-up VAO
    setBackgroundVAO();
    setGroundVAO();
    setPyramidVAO();
    setIndexVAO();

    //Textures
    bgTexture = loadImageToTexture("res/background.png");
    setupTexture(bgTexture);

    indexTexture = loadImageToTexture("res/index.png");
    setupTexture(indexTexture);

    groundTexture = loadImageToTexture("res/ground.png");
    setupTexture(groundTexture);

    pyramidTexture = loadImageToTexture("res/wall.png");
    setupTexture(pyramidTexture);

    //MVP
    projectionP = glm::perspective(glm::radians(45.0f), (float)wWidth / wHeight, 1.1f, 200.0f);
    projectionO = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 100.0f);

    modelGroundLoc = glGetUniformLocation(groundShader, "uM");
    viewGroundLoc = glGetUniformLocation(groundShader, "uV");
    projectionGroundLoc = glGetUniformLocation(groundShader, "uP");

    modelPyramidLoc = glGetUniformLocation(pyramidShader, "uM");
    viewPyramidLoc = glGetUniformLocation(pyramidShader, "uV");
    projectionPyramidLoc = glGetUniformLocation(pyramidShader, "uP");

    glCullFace(GL_BACK);

    float lastFrameTime = 0.0f;
    const double targetFrameRate = 1.0 / 60.0;

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        if (currentTime - lastFrameTime >= targetFrameRate)
        {
            lastFrameTime = currentTime;
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            {
                glEnable(GL_CULL_FACE);
            }
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            {
                glDisable(GL_CULL_FACE);
            }

            const float moveSpeed = 0.1f;
            const float scaleSpeed = 0.01f;
            const float minScale = 0.1f;
            const float maxScale = 5.0f;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                pyramidPosition.z -= moveSpeed;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                pyramidPosition.z += moveSpeed;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                pyramidPosition.x -= moveSpeed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                pyramidPosition.x += moveSpeed;

            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                pyramidScale = std::min(maxScale, pyramidScale + scaleSpeed);

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                pyramidScale = std::max(minScale, pyramidScale - scaleSpeed);

            if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
                usePerspective = false;
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
                usePerspective = true;


            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /*           view = glm::lookAt(
                     glm::vec3(pyramidPosition.x + 5.0f, 5.0f, pyramidPosition.z + 10.0f),
                     pyramidPosition,
                     glm::vec3(0.0f, 1.0f, 0.0f));*/

            if (!usePerspective) {
                view = glm::lookAt(
                    glm::vec3(pyramidPosition.x, pyramidPosition.y + 2.0f, pyramidPosition.z + 20.0f),
                    pyramidPosition,
                    glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else {
                view = glm::lookAt(
                    glm::vec3(pyramidPosition.x + 5.0f, 5.0f, pyramidPosition.z + 10.0f),
                    pyramidPosition,
                    glm::vec3(0.0f, 1.0f, 0.0f));
            }


            renderBackground();
            renderGround();
            renderPyramids();
            renderIndexTexture();


            glfwSwapBuffers(window);
            glfwPollEvents();
        }

     
    }

    glDeleteBuffers(1, &bgVBO);
    glDeleteVertexArrays(1, &bgVAO);
    glDeleteProgram(backgroundShader);

    glDeleteBuffers(1, &groundVBO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteProgram(groundShader);

    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteProgram(pyramidShader);

    glDeleteBuffers(1, &indexVBO);
    glDeleteVertexArrays(1, &indexVAO);
    glDeleteProgram(textureShader);

    glfwTerminate();
    return 0;
}


static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
       
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Texture not loaded! Texture path: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

void setupTexture(unsigned int texture) {
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Successfully read file from path \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Error reading file from path \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" The shader has an error! Error: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "The unified shader has an error! Error: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
