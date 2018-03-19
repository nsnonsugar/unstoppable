#include <stdlib.h>
#include <string.h>
#include "Message.h"
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

static void OnPlayRequest(const ThreadMsg& msg);
static void OnPauseRequest(const ThreadMsg& msg);
static void OnStopRequest(const ThreadMsg& msg);

void Sound_Thread(void){
    //グローバル変数の初期化
    sound_player_ = nullptr;
    sound_player_ = new(SoundPlayer);

    play_state_ = PlayState::Stop;
    ThreadMsg msg = {0};

    while(1){
        ReceiveMsg(MsgQueueIdSound, msg);
        switch(static_cast<SoundEventId>(msg.event_id)){
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

        if(msg.data != nullptr){
            free(msg.data);
        }
    }
}
void SendPlayRequestMsg(const char* file_name, bool is_loop){
    //malloc失敗時の処理
    ThreadMsg msg;
    msg.event_id = static_cast<int32_t>(SoundEventId::kPlayRequest);

    PlaySoundMsg* snd = (PlaySoundMsg*)malloc(sizeof(PlaySoundMsg));

    char* name = (char*)malloc(strlen(file_name) + 1);
    memset(name, '\0', strlen(file_name) + 1);
    strncpy(name, file_name, strlen(file_name));

    snd->file_name = name;
    snd->is_loop = is_loop;
    msg.data = snd;

    SendMsg(MsgQueueIdSound, msg);
}

void SendStopRequestMsg(void){
    ThreadMsg msg;
    msg.event_id = static_cast<int32_t>(SoundEventId::kStopRequest);
    msg.data = nullptr;

    SendMsg(MsgQueueIdSound, msg);
}

static void OnPlayRequest(const ThreadMsg& msg){
    if(play_state_ == PlayState::Play){
        sound_player_->Stop();
    }

    PlaySoundMsg* data = static_cast<PlaySoundMsg*>(msg.data);
    sound_player_->CreatePlayer(data->file_name, data->is_loop);
    sound_player_->Play();
    play_state_ = PlayState::Play;
    free((void*)data->file_name);
}

static void OnPauseRequest(const ThreadMsg& msg){
    (void)msg;

    if(play_state_ != PlayState::Play){
        return;
    }

    sound_player_->Pause();
    play_state_ = PlayState::Pause;
}

static void OnStopRequest(const ThreadMsg& msg){
    (void)msg;

    if(play_state_ != PlayState::Play){
        return;
    }

    sound_player_->Stop();
    play_state_ = PlayState::Stop;
}

} //namespace nonsugar
