#ifndef draw_thread_h__
#define draw_thread_h__
#include <stdint.h>
namespace nonsugar{
//イベントID
enum class DrawEventId : int32_t {
    kInitDisplay = 0,
    kUnstoppable,
    kStoppable,
    kExit,
    kEmpty,
};

void draw_thread(void);

} //namespace nonsugar
#endif
