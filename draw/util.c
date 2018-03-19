#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>



static GLint Shader_load( const char* shader_source, const GLenum GL_XXXX_SHADER ) {
	const GLint shader = glCreateShader( GL_XXXX_SHADER );
	assert( glGetError() == GL_NO_ERROR );

	glShaderSource( shader, 1, &shader_source, NULL );
	glCompileShader( shader );

	// コンパイルエラーをチェックする
	{
		GLint compileSuccess = 0;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compileSuccess );
		if (compileSuccess == GL_FALSE) {
			// エラーが発生した
			GLint infoLen = 0;
			// エラーメッセージを取得
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );
			if (infoLen > 1) {
				GLchar *message = (GLchar*)calloc( infoLen, sizeof( GLchar ) );
				glGetShaderInfoLog( shader, infoLen, NULL, message );
                //LOGI( message );
				free( (void*)message );
			}
			else {
                //LOGI( "comple error not info..." );
			}
		}

		// コンパイル失敗していたらここでプログラムを停止する
		assert( compileSuccess == GL_TRUE );
	}

	return shader;
}


GLuint Shader_createProgramFromSource( const char* vertex_shader_source, const char* fragment_shader_source ) {
	const GLuint vertex_shader = Shader_load( vertex_shader_source, GL_VERTEX_SHADER );
	const GLuint fragment_shader = Shader_load( fragment_shader_source, GL_FRAGMENT_SHADER );


	const GLuint program = glCreateProgram();
	assert( glGetError() == GL_NO_ERROR );
	assert( program != 0 );

	glAttachShader( program, vertex_shader ); // バーテックスシェーダーとプログラムを関連付ける
	glAttachShader( program, fragment_shader ); // フラグメントシェーダーとプログラムを関連付ける
	assert( glGetError() == GL_NO_ERROR );

	// コンパイルを行う
	glLinkProgram( program );

	// リンクエラーをチェックする
	{
		GLint linkSuccess = 0;
		glGetProgramiv( program, GL_LINK_STATUS, &linkSuccess );
		if (linkSuccess == GL_FALSE) {
			// エラーが発生した
			GLint infoLen = 0;
			// エラーメッセージを取得
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &infoLen );
			if (infoLen > 1) {
				GLchar *message = (GLchar*)calloc( infoLen, sizeof( GLchar ) );
				glGetProgramInfoLog( program, infoLen, NULL, message );
                //LOGI( message );
				free( (void*)message );
			}
		}
		assert( linkSuccess == GL_TRUE );
	}

	// リンク済みのため、個々のシェーダーオブジェクトの解放フラグを立てる
	glDeleteShader( vertex_shader );
	glDeleteShader( fragment_shader );

	// リンク済みのプログラムを返す
	return program;
}



