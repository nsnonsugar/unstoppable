#include <assert.h>
#include <stdio.h>
#include <string>
#include "android_fopen.h"
#include "sound_player.h"


namespace nonsugar{

SoundPlayer::SoundPlayer()
{
    SLresult result;

    {
        //エンジンの生成
        result = slCreateEngine(&engine_object_, 0, NULL, 0, NULL, NULL);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        //エンジンをリアライズ
        result = (*engine_object_)->Realize(engine_object_, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        //エンジンのインタフェースを取得
        result = (*engine_object_)->GetInterface(engine_object_,
                                                 SL_IID_ENGINE,
                                                 &engine_engine_);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }

    {
        //OoutputMixの生成
        const SLInterfaceID ids[1] = {SL_IID_PLAYBACKRATE};
        const SLboolean req[1] = {SL_BOOLEAN_FALSE};
        result = (*engine_engine_)->CreateOutputMix(engine_engine_,
                                                  &output_mix_object_,
                                                  1,
                                                  ids,
                                                  req);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        // Output Mixをリアライズ
        result = (*output_mix_object_)->Realize(output_mix_object_,
                                             SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }
}

SoundPlayer::~SoundPlayer()
{
    if (player_object_ != NULL) {
        (*player_object_)->Destroy(player_object_);
        player_object_ = NULL;
        player_play_ = NULL;
        player_seek_ = NULL;
        player_volume_ = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (output_mix_object_ != NULL) {
        (*output_mix_object_)->Destroy(output_mix_object_);
        output_mix_object_ = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engine_object_ != NULL) {
        (*engine_object_)->Destroy(engine_object_);
        engine_object_ = NULL;
        engine_engine_ = NULL;
    }

}

void SoundPlayer::CreatePlayer(const std::string& file_name, bool is_loop)
{
    SLresult result;
    //AssetManagerを用いてファイルオープン
    AAsset* asset = AAssetManagerOpen(file_name.c_str());
    if(asset == NULL){
        //ファイルオープン失敗
        return;
    }

    {
        //ファイルディスクリプタオープン
        off_t start, length;
        int fd = AAsset_openFileDescriptor(asset, &start, &length);
        assert(0 <= fd);
        AAsset_close(asset);

        //DataSource設定
        SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD,
                                          fd, start, length};
        SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL,
                                         SL_CONTAINERTYPE_UNSPECIFIED};
        SLDataSource audio_src = {&loc_fd, &format_mime};

        //DataSinkの設定
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX,
                                              output_mix_object_};
        SLDataSink audio_snk = {&loc_outmix, NULL};

        //AudioPlayerの生成
        const SLInterfaceID ids[2] = {SL_IID_SEEK, SL_IID_VOLUME};
        const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
        result = (*engine_engine_)->CreateAudioPlayer(engine_engine_,
                                                    &player_object_,
                                                    &audio_src,
                                                    &audio_snk,
                                                    2, ids, req);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        //AudioPlayerのリアライズ
        result = (*player_object_)->Realize(player_object_,
                                            SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }

    {
        //再生インタフェースを取得
        result = (*player_object_)->GetInterface(player_object_,
                                                 SL_IID_PLAY, &player_play_);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        //SEEKインタフェースを取得
        result = (*player_object_)->GetInterface(player_object_,
                                                    SL_IID_SEEK, &player_seek_);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        //ボリュームインタフェースを取得
        result = (*player_object_)->GetInterface(player_object_,
                                                 SL_IID_VOLUME,
                                                 &player_volume_);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        if(is_loop){
            //ループを有効にする
            result = (*player_seek_)->SetLoop(player_seek_, SL_BOOLEAN_TRUE,
                                              0, SL_TIME_UNKNOWN);
            assert(SL_RESULT_SUCCESS == result);
            (void) result;
        }
    }
}

void SoundPlayer::Play()
{
    if(player_play_ == NULL){
        return;
    }

    SLresult result;

    // set the player's state
    result = (*player_play_)->SetPlayState(player_play_, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

void SoundPlayer::Stop()
{
    if(player_play_ == NULL){
        return;
    }

    SLresult result;
    // set the player's state
    result = (*player_play_)->SetPlayState(player_play_, SL_PLAYSTATE_STOPPED);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

void SoundPlayer::Pause()
{
    if(player_play_ == NULL){
        return;
    }
    SLresult result;
    // set the player's state
    result = (*player_play_)->SetPlayState(player_play_, SL_PLAYSTATE_PAUSED);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

} //namespace nonsugar

