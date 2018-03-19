#include <EGL/egl.h>

#include <android/log.h>
#include <android_native_app_glue.h>
#include "android_fopen.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include "Message.h"
#include "draw_thread.h"
#include "platform_dependent.h"
#include "sound_player.h"
#include "sound_thread.h"

#define LOG_TAG ("unstoppable")
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#include <string>
std::string android_temp_folder( struct android_app *app ) {
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread( &env, NULL );

    jclass activityClass = env->FindClass( "android/app/NativeActivity" );
    jmethodID getCacheDir = env->GetMethodID( activityClass, "getCacheDir", "()Ljava/io/File;" );
    jobject cache_dir = env->CallObjectMethod( app->activity->clazz, getCacheDir );

    jclass fileClass = env->FindClass( "java/io/File" );
    jmethodID getPath = env->GetMethodID( fileClass, "getPath", "()Ljava/lang/String;" );
    jstring path_string = (jstring)env->CallObjectMethod( cache_dir, getPath );

    const char *path_chars = env->GetStringUTFChars( path_string, NULL );
    std::string temp_folder( path_chars );

    env->ReleaseStringUTFChars( path_string, path_chars );
    app->activity->vm->DetachCurrentThread();
    return temp_folder;
}

//メッセージキュー生成用テーブル
static const nonsugar::MessageConfig config_table[] = {
        {nonsugar::MsgQueueIdDraw, nonsugar::MsgQueueStatus::USE},
        {nonsugar::MsgQueueIdSound, nonsugar::MsgQueueStatus::USE},
};

void android_main(android_app* state) {

    //assetsフォルダ下のリソースにアクセス用初期化
    android_fopen_set_asset_manager(state->activity->assetManager);

    //メッセージキュー生成
    CreateMessageQueue( config_table, sizeof(config_table)/sizeof(config_table[0]));

    state->onAppCmd = nonsugar::OnAppCmd;
    state->onInputEvent = nonsugar::OnInput;

    //描画スレッド起動
    std::thread draw(nonsugar::draw_thread);
    draw.detach();

    //音声スレッド起動
    std::thread sound(nonsugar::Sound_Thread);
    sound.detach();

    while (1) {
        int ident, events;
        android_poll_source* source;
        while ((ident=ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }
            if (state->destroyRequested != 0) {
                return;
            }
        }
    }
}
