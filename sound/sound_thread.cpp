#include <stdlib.h>
#include <string.h>
#include "sound_player.h"
#include "sound_thread.h"

namespace nonsugar{

enum class PlayState : int32_t{
    Stop = 0,
    Play,
    Pause,
};

//グローバル変数
SoundPlayer* sound_player_;
PlayState play_state_;

static void OnPlayRequest(const std::shared_ptr<ThreadMsg>& msg);
static void OnPauseRequest(const std::shared_ptr<ThreadMsg>& msg);
static void OnStopRequest(const std::shared_ptr<ThreadMsg>& msg);

void Sound_Thread(void){
    //グローバル変数の初期化
    sound_player_ = nullptr;
    sound_player_ = new(SoundPlayer);

    play_state_ = PlayState::Stop;

    while(1){
        std::shared_ptr<ThreadMsg> msg;
        ReceiveMsg(MsgQueueIdSound, &msg);
        switch(static_cast<SoundEventId>(msg->event_id)){
            case SoundEventId::kPlayRequest:
                OnPlayRequest(msg);
                break;

            case SoundEventId::kPouseRequest:
                OnPauseRequest(msg);
                break;

            case SoundEventId::kStopRequest:
                OnStopRequest(msg);
                break;

            case SoundEventId::kExit:
                return;
        }
    }
}
void SendPlayRequestMsg(const std::string& file_name, bool is_loop){
    auto msg = std::make_shared<PlaySoundMsg>();
    msg->event_id = static_cast<int32_t>(SoundEventId::kPlayRequest);

    msg->file_name = file_name;
    msg->is_loop = is_loop;

    SendMsg(MsgQueueIdSound, msg);
}

void SendStopRequestMsg(void){
    auto msg = std::make_shared<ThreadMsg>();
    msg->event_id = static_cast<int32_t>(SoundEventId::kStopRequest);

    SendMsg(MsgQueueIdSound, msg);
}

static void OnPlayRequest(const std::shared_ptr<ThreadMsg>& msg){
    if(play_state_ == PlayState::Play){
        sound_player_->Stop();
    }

    auto rcv = std::dynamic_pointer_cast<PlaySoundMsg>(msg);
    sound_player_->CreatePlayer(rcv->file_name, rcv->is_loop);
    sound_player_->Play();
    play_state_ = PlayState::Play;
}

static void OnPauseRequest(const std::shared_ptr<ThreadMsg>& msg){
    (void)msg;

    if(play_state_ != PlayState::Play){
        return;
    }

    sound_player_->Pause();
    play_state_ = PlayState::Pause;
}

static void OnStopRequest(const std::shared_ptr<ThreadMsg>& msg){
    (void)msg;

    if(play_state_ != PlayState::Play){
        return;
    }

    sound_player_->Stop();
    play_state_ = PlayState::Stop;
}

} //namespace nonsugar
