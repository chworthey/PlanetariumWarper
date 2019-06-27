#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <engine/filesystem.h>
#include <engine/shader.h>

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ConfigFile.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::string asString(float val)
{
    std::ostringstream strm;
    strm << val;
    return strm.str();
}

float degrees2Radians(float degrees)
{
    return static_cast<float>(degrees * 3.14159265359 / 180.0);
}

void renderText(const char *text, float x, float y, float sx, float sy, FT_Face& face) {
    const char *p;

    for (p = text; *p; p++) {
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
            continue;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        float x2 = x + face->glyph->bitmap_left * sx;
        float y2 = -y - face->glyph->bitmap_top * sy;
        float w = face->glyph->bitmap.width * sx;
        float h = face->glyph->bitmap.rows * sy;

        GLfloat box[4][4] = {
            {x2,     -y2    , 0, 0},
            {x2 + w, -y2    , 1, 0},
            {x2,     -y2 - h, 0, 1},
            {x2 + w, -y2 - h, 1, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += (face->glyph->advance.x / 64) * sx;
        y += (face->glyph->advance.y / 64) * sy;
    }
}

std::string readShaderFile(const std::string& filename)
{
    std::string code;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        shaderFile.open(filename);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        // convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    return code;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Planetarium Warper", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // font library initialization
    // ---------------------------
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        std::cout << "Could not initialize freetype library" << std::endl;
        return -1;
    }

    FT_Face face;

    if (FT_New_Face(ft, FileSystem::getPath("resources/Roboto-Regular.ttf").c_str(), 0, &face)) {
        std::cout << "Could not open font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure program
    // -----------------
    ConfigFile config;
    config.loadFromFile("config.json");

    // read shader code
    // ----------------
    std::string vertexCode = readShaderFile("FullscreenQuad.vs.in");
    std::string fragmentCode = readShaderFile("FullscreenQuad.fs.in");

    // Shader file constant variable replacements
    // ------------------------------------------
    replaceAll(vertexCode, "$(UV_SCALE_X)", config.getFlipX() ? "-1.0" : "1.0");
    replaceAll(vertexCode, "$(UV_SCALE_Y)", config.getFlipY() ? "-1.0" : "1.0");
    replaceAll(vertexCode, "$(UV_OFFSET_X)", config.getFlipX() ? "1.0" : "0.0");
    replaceAll(vertexCode, "$(UV_OFFSET_Y)", config.getFlipY() ? "1.0" : "0.0");

    replaceAll(fragmentCode, "$(DOME_RADIUS)", asString(config.getDomeRadius()));
    replaceAll(fragmentCode, "$(PHASE_ANGLE)", asString(config.getPhaseAngle()));
    replaceAll(fragmentCode, "$(BEAM)", asString(config.getProjectorOpeningAngle()));
    replaceAll(fragmentCode, "$(ASPECT)", asString(config.getAspectRatio()));
    replaceAll(fragmentCode, "$(ALPHA)", asString(config.getTableTilt()));

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader(vertexCode.c_str(), fragmentCode.c_str()); 

    // text shader
    // -----------
    std::string textVertexCode = readShaderFile("Text.vs");
    std::string textFragCode = readShaderFile("Text.fs");
    Shader textShader(textVertexCode.c_str(), textFragCode.c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int textVBO, textVAO;
    glGenVertexArrays(1, &textVAO);

    glBindVertexArray(textVAO);

    glGenBuffers(1, &textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // load and create textures 
    // -------------------------
    unsigned int textures[2] = { 0 };
    glGenTextures(2, textures);
    unsigned int texture = textures[0];
    unsigned int textTexture = textures[1];
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/radialgrid.tga").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, textTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        ourShader.use();

        ourShader.setFloat("R", config.getMirrorRadius());
        ourShader.setFloat("v", config.getProjectorHorizontalDistance());
        ourShader.setFloat("beta", degrees2Radians(config.getBottomRowOfPixels()));
        ourShader.setFloat("ba", config.getOblateSpheroidAxialRatio());
        ourShader.setFloat("M", config.getMirrorCenterToDomeCenter());
        ourShader.setFloat("H", config.getGapDistance());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // text
        // ----

        glBindTexture(GL_TEXTURE_2D, textTexture);

        glBindVertexArray(textVAO);
        textShader.use();
        textShader.setVec4("color", 1.0f, 1.0f, 1.0f, 1.0f);
        float sx = 2.0 / SCR_WIDTH;
        float sy = 2.0 / SCR_HEIGHT;
        // renderText("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy, face);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}