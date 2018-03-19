#ifndef unstoppable_renderer_h_
#define unstoppable_renderer_h_
#include <GLES2/gl2.h>
#include "renderer.h"

namespace nonsugar{

class UnstoppableRenderer : public Renderer{
public:
    UnstoppableRenderer():begin_(0.0f), end_(0.1f){}
    ~UnstoppableRenderer(){}

public:
    void Initialize() override;
    void Rendering() override;
    void Destroy() override;

private:
    GLuint shader_program_;
    GLint attr_pos_;
    GLint attr_uv_;
    GLint unif_texture_;
    GLuint texture_id_;

    GLfloat begin_;
    GLfloat end_;
};

} //namespace nonsugar
#endif //unstoppable_renderer_h_
