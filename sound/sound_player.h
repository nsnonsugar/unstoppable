#ifndef sound_player_h_
#define sound_player_h_
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace nonsugar{

class SoundPlayer{
public:
    SoundPlayer();
    ~SoundPlayer();

public:
    void CreatePlayer(const std::string& file_name, bool is_loop);
    void Play();
    void Stop();
    void Pause();

private:
    //engine interfaces
    SLObjectItf engine_object_;
    SLEngineItf engine_engine_;

    //output mix interfaces
    SLObjectItf output_mix_object_;

    //player interfaces
    SLObjectItf player_object_;
    SLPlayItf player_play_;
    SLSeekItf player_seek_;
    SLVolumeItf player_volume_;
};

} //namespace nonsugar
#endif //sound_payer_h_
