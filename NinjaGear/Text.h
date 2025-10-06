#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#pragma once
#include <string>
#include <map>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ShaderProgram.h"

struct Character {
    GLuint textureID;   //ID of the glyph texture
    glm::ivec2 size;    //size of glyph
    glm::ivec2 bearing; //offset from baseline to left/top of glyph
    GLuint advance;     //horizontal offset to advance to next glyph
};

struct RenderedText {
    std::string text;
    float x, y, scale;
    glm::vec3 color;
};

class Text {
public:
    Text(const std::string& fontPath, unsigned int fontSize,
        unsigned int screenWidth, unsigned int screenHeight);

    ~Text();

    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

    //if window resized, update window size
    void UpdateScreenSize(unsigned int width, unsigned int height);
    void RenderTextWithId(const std::string& id, const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void ClearText(const std::string& id);
    void RedrawAll(); 

private:
    void SetUpShaders();

    GLuint VAO, VBO;
    ShaderProgram shaderProgram;
    glm::mat4 projectionMatrix;

    std::map<GLchar, Character> characters;
    std::unordered_map<std::string, RenderedText> renderedTexts;

    unsigned int screenWidth, screenHeight;

};

#endif