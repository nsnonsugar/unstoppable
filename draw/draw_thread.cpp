#include <stdlib.h>
#include <unistd.h>
#include <EGL/egl.h>
#include "draw_thread.h"
#include "unstoppable_renderer.h"
#include "stoppable_renderer.h"
#include "sound_thread.h"
#include "Message.h"
#include "platform_dependent.h"


namespace nonsugar{

enum class Scene : int32_t{
    kNone = 0,
    kUnstoppable,
    kStoppable,
};

Scene current_drawing_;
bool is_initialize_;
EGLDisplay display_;
EGLSurface surface_;
Renderer* render_;

static void OnInitialize(const std::shared_ptr<ThreadMsg>& msg);
static void OnUnstoppable(void);
static void OnStoppable(void);
static void OnExit(void);

void draw_thread(void)
{
    //グローバル変数の初期化
    current_drawing_ = Scene::kNone;
    is_initialize_ = false;
    render_ = nullptr;

    while(1){
        //メッセージ初期化
        std::shared_ptr<ThreadMsg> msg;

        ReceiveMsgPolling(MsgQueueIdDraw, &msg);

        if( msg.get() != nullptr) {
            switch (static_cast<DrawEventId>(msg->event_id)) {
                case DrawEventId::kInitDisplay:
                    OnInitialize(msg);
                    break;

                case DrawEventId::kUnstoppable:
                    OnUnstoppable();
                    break;

                case DrawEventId::kStoppable:
                    OnStoppable();
                    break;

                case DrawEventId::kExit:
                    OnExit();
                    return;
            }
        }


        if(render_ != nullptr){
            render_->Rendering();
            PostFrontBuffer( display_, surface_ );
            //適当に待つ
            usleep(30000);
        }
    }
}

static void OnInitialize(const std::shared_ptr<ThreadMsg>& msg)
{
    if(is_initialize_){
        return;
    }

    auto rcv = std::dynamic_pointer_cast<InitMsg>(msg);

    InitDisplay(rcv->app, &display_, &surface_);
    is_initialize_ = true;

    auto snd = std::make_shared<ThreadMsg>();
    snd->event_id = static_cast<int32_t>(DrawEventId::kUnstoppable);
    SendMsg(MsgQueueIdDraw, snd);
}

static bool CanUpdate(Scene update)
{
    if(current_drawing_ == update){
        return false;
    }

    return is_initialize_;
}

static void OnUnstoppable(void)
{
    if(!CanUpdate(Scene::kUnstoppable)){
        return;
    }

    if(render_ != nullptr){
        delete render_;
    }

    render_ = new(UnstoppableRenderer);
    render_->Initialize();
    current_drawing_ = Scene::kUnstoppable;
    SendPlayRequestMsg(u8"rage_of_dust.mp3", true);
 
}

static void OnStoppable(void)
{
    if(!CanUpdate(Scene::kStoppable)){
        return;
    }

    if(render_ != nullptr){
        delete render_;
    }

    render_ = new(StoppableRenderer);
    render_->Initialize();
    current_drawing_ = Scene::kStoppable;
    SendPlayRequestMsg(u8"freesia.mp3", false);
}

static void OnExit(void)
{
    if(render_ == nullptr){
        return;
    }

    render_->Destroy();
    delete render_;
    return;
}

} //namespace nonsugar
