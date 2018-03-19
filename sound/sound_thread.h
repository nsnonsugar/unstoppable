#ifndef UNSTOPPABLE_SOUND_THREAD_H
#define UNSTOPPABLE_SOUND_THREAD_H
#include <stdint.h>
#include <string>

namespace nonsugar{

//イベントID
enum class SoundEventId : int32_t {
    kPlayRequest = 0,
    kPouseRequest,
    kStopRequest,
    kExit,
};

struct PlaySoundMsg{
    const char* file_name;
    bool is_loop;
};

void Sound_Thread(void);
void SendPlayRequestMsg(const char* file_name, bool is_loop);
void SendStopRequestMsg(void);

} //namespace nonsugar

#endif //UNSTOPPABLE_SOUND_THREAD_H
