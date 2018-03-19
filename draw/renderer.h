#ifndef renderer_h_
#define renderer_h_

namespace nonsugar {

class Renderer{
public:
    virtual ~Renderer(){}

public:
    virtual void Initialize() = 0;
    virtual void Rendering() = 0;
    virtual void Destroy() = 0;
};

} //namespace nonsugar
#endif //renderer_h_