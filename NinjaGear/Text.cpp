#include "Text.h"
#include <iostream>
#include <fstream>   
#include <sstream>  

Text::Text(const std::string& fontPath, unsigned int fontSize,
    unsigned int screenWidth, unsigned int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font at " << fontPath << std::endl;
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for char: " << c << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SetUpShaders();

    projectionMatrix = glm::ortho(0.0f, static_cast<float>(screenWidth),
        static_cast<float>(screenHeight), 0.0f);
}

Text::~Text() {
    //clean up OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    for (auto& pair : characters) {
        glDeleteTextures(1, &pair.second.textureID);
    }
}

void Text::SetUpShaders() {
    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/text.vert");
    if (!vShader.isCompiled()) {
        std::cerr << "Text Vertex Shader Error" << std::endl;
        std::cerr << vShader.log() << std::endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/text.frag");
    if (!fShader.isCompiled()) {
        std::cerr << "Text Fragment Shader Error" << std::endl;
        std::cerr << fShader.log() << std::endl;
    }

    shaderProgram.init();
    shaderProgram.addShader(vShader);
    shaderProgram.addShader(fShader);
    shaderProgram.link();
    if (!shaderProgram.isLinked()) {
        std::cerr << "Text Shader Linking Error" << std::endl;
        std::cerr << shaderProgram.log() << std::endl;
    }
    shaderProgram.bindFragmentOutput("color"); // Use "color" as defined in text.frag

    vShader.free();
    fShader.free();
}

void Text::RenderText(const std::string& text, float x, float y,
    float scale, const glm::vec3& color) {
    // SAVE OPENGL STATE
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    shaderProgram.use();
    shaderProgram.setUniformMatrix4f("projection", projectionMatrix);
    shaderProgram.setUniform3f("textColor", color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0); //Text renderer uses texture unit 0
    glBindVertexArray(VAO);

    for (char c : text) {
        Character ch = characters[c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y + (characters['H'].bearing.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * scale;
    }

    //RESTORE OPENGL STATE
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (!blendEnabled) glDisable(GL_BLEND);
    else glBlendFunc(blendSrc, blendDst);

    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);

    glActiveTexture(last_active_texture);
    glUseProgram(last_program);
}

void Text::UpdateScreenSize(unsigned int width, unsigned int height) {
    screenWidth = width;
    screenHeight = height;
    projectionMatrix = glm::ortho(0.0f, static_cast<float>(screenWidth),
        static_cast<float>(screenHeight), 0.0f);
}

void Text::RenderTextWithId(const std::string& id, const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    renderedTexts[id] = { text, x, y, scale, color };
}

void Text::ClearText(const std::string& id) {
    renderedTexts.erase(id);
}

void Text::RedrawAll() {
    for (const auto& pair : renderedTexts) {
        const auto& t = pair.second;
        RenderText(t.text, t.x, t.y, t.scale, t.color);
    }
}