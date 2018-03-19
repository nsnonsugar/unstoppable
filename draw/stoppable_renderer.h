#ifndef stoppable_renderer_h_
#define stoppable_renderer_h_
#include <GLES2/gl2.h>
#include "renderer.h"

namespace nonsugar{

class StoppableRenderer : public Renderer{
public:
    StoppableRenderer():top_{0.5625f}, bottom_{1}{}
    ~StoppableRenderer(){}
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

    GLfloat top_;
    GLfloat bottom_;
};

} //namespace nonsugar
#endif //stoppable_renderer_h_
