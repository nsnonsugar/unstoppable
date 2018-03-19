#include <assert.h>
#include "stoppable_renderer.h"
extern "C"{
#include "PngPixelImage.h"
#include "util.h"
}

namespace nonsugar{

void StoppableRenderer::Initialize()
{
    //頂点シェーダーを用意する
    {
        const GLchar *vertex_shader_source =
            //
            "attribute mediump vec4 attr_pos;"
            "attribute mediump vec2 attr_uv;"
            "varying mediump vec2 vary_uv;"
            "void main() {"
            "   gl_Position = attr_pos;"
            "   vary_uv = attr_uv;"
            "}";

        const GLchar *fragment_shader_source =
            //
            "uniform sampler2D unif_texture;"
            "varying mediump vec2 vary_uv;"
            "void main() {"
            "   gl_FragColor = texture2D(unif_texture, vary_uv);"
            "}";

        //コンパイルとリンクを行う
        shader_program_ = Shader_createProgramFromSource( vertex_shader_source, fragment_shader_source );
    }

    //attributeを取り出す
    {
        attr_pos_ = glGetAttribLocation(shader_program_, "attr_pos");
        assert(attr_pos_ >= 0);

        attr_uv_ = glGetAttribLocation(shader_program_, "attr_uv");
        assert(attr_uv_ >= 0);
    }

    //uniformを取り出す
    {
        unif_texture_ = glGetUniformLocation(shader_program_, "unif_texture");
        assert(unif_texture_ >= 0);
    }

    //テクスチャの生成を行う
    {
        glGenTextures(1, &texture_id_);
        assert(texture_id_ != 0);
        assert(glGetError() == GL_NO_ERROR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        assert(glGetError() == GL_NO_ERROR);

        glBindTexture(GL_TEXTURE_2D, texture_id_);
        assert(glGetError() == GL_NO_ERROR);

        //画像ピクセルを読み込む
        {
            PngPixelImage*  image = PngPixelImage_load("oruga.png");
            //正常に読み込まれたかをチェック
            assert(image != nullptr);

            //VRAMへピクセル情報をコピーする
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixel_data);
            assert(glGetError() == GL_NO_ERROR);

            //コピー後は不要になるため、ピクセル画素を解放する
            PngPixelImage_free(image);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        assert(glGetError() == GL_NO_ERROR);
    }

    //シェーダーの利用を開始する
    glUseProgram(shader_program_);
    assert(glGetError() == GL_NO_ERROR);
}

void StoppableRenderer::Rendering()
{
    glClearColor( 0.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    //attr_posを有効にする
    glEnableVertexAttribArray((GLuint)attr_pos_);
    glEnableVertexAttribArray((GLuint)attr_uv_);

    //unif_textureへテクスチャを設定する
    glUniform1i(unif_texture_, 0);

    // 四角形描画
    {
        const GLfloat position[] = {
                // v0(left top)
                -1.00f, 1.00f,
                // v1(left bottom)
                -1.00f, -1.00f,
                // v2(right top)
                1.00f, 1.00f,
                // v3(right bottom)
                1.00f, -1.00f, };

        const GLfloat uv[] = {
            // v0(left top)
            0 , top_,
            // v1(left bottom)
            0, bottom_,
            // v2(right top)
            1 , top_,
            // v3(right bottom)
            1 , bottom_, };

        if(top_ >= 0)
        {
            top_ -= 0.002f;
            bottom_ -= 0.002f;
            if(top_ <= 0){
                top_ = 0;
                bottom_ = 1 - 0.5625f;
            }
        }

        glVertexAttribPointer((GLuint)attr_pos_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
        glVertexAttribPointer((GLuint)attr_uv_, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)uv);
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    }
}

void StoppableRenderer::Destroy()
{
    //シェーダーの利用を終了
    glUseProgram(0);
    assert(glGetError() == GL_NO_ERROR);

    //シェーダープログラムを廃棄
    glDeleteProgram(shader_program_);
    assert(glGetError() == GL_NO_ERROR);

    //テクスチャを廃棄
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_id_);
    assert(glGetError() == GL_NO_ERROR);
}

} //namespace nonsugar