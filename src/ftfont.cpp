#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ftfont.hpp"

FtFont::FtFont(std::string fontFilename, int size, int window_width, int window_height) {
    this->size = size;
    // Load shader
    shader.loadFromFile("shaders/text.vert", "", "shaders/text.frag");
    attr_pos = shader.getAttribLocation("pos_uv");
    setWindow(window_width, window_height);

    // Load font via freetype
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "failed to initialize freetype library" << std::endl;
        exit(1);
    }

    FT_Face face;
    if (FT_New_Face(ft, fontFilename.c_str(), 0, &face)) {
        std::cout << "failed to initialize freetype face" << std::endl;
        exit(1);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    FT_Set_Pixel_Sizes(face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    GLint _w = 1024;
    GLint _h = 1024;
    GLuint dummyImage[_w * _h];
    memset(dummyImage, 0, sizeof(dummyImage));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _w, _h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, dummyImage);

    float xof = 0.0, yof = 0.0;
    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        glTexSubImage2D(GL_TEXTURE_2D, 0, xof, yof,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_ALPHA, GL_UNSIGNED_BYTE, 
            face->glyph->bitmap.buffer
        );

        Character charr = {
            xof, yof,
            face->glyph->bitmap.width,
            (GLuint) size,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            face->glyph->advance.x
        };

        xof += face->glyph->bitmap.width;
        if (xof > 1024 - size) {
          yof += size;
          xof = 0;
        }
        if (yof > 1024 - size) {
          std::cerr << "Font size too large to fit into atlas!" << std::endl;
          return;
        }

        characters.insert(std::pair<GLchar, Character>(c, charr));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Bind VAO
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(attr_pos, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(attr_pos);

    // Unbind VAO
    glBindVertexArray(0);
    glDisableVertexAttribArray(attr_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

FtFont::~FtFont() { }

void FtFont::setWindow(int width, int height) {
    shader.bind();
    glm::mat4 projection = glm::ortho(0.f, static_cast<GLfloat>(width), 0.f, static_cast<GLfloat>(height));
    shader.setUniform("projection", projection);
}

void FtFont::renderText(std::string text, GLfloat _x, GLfloat _y, GLfloat scale) {
    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    GLfloat x = _x;
    GLfloat y = _y;

    GLfloat vertices[text.length()][6][4];

    for (int i = 0; i < text.length(); i++) {
        GLchar c = text[i];
        Character ch = characters[c];

        GLfloat posX = x + ch.bearingX * scale;
        GLfloat posY = y - (ch.h - ch.bearingY) * scale;
        GLfloat width = ch.w * scale;
        GLfloat height = ch.h * scale;

        auto init = std::initializer_list<GLfloat>({
            posX        , posY + height, ch.x,        ch.y       ,
            posX        , posY         , ch.x,        ch.y + ch.h,
            posX + width, posY         , ch.x + ch.w, ch.y + ch.h,

            posX        , posY + height, ch.x,        ch.y       ,
            posX + width, posY         , ch.x + ch.w, ch.y + ch.h,
            posX + width, posY + height, ch.x + ch.w, ch.y       ,
        });
        std::copy(init.begin(), init.end(), (GLfloat*)vertices[i]);

        x += (ch.advance >> 6) * scale;

        if (c == '\n') {
            y -= size * scale;
            x = _x;
        }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, text.length() * 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

