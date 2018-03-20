#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>
#include <memory>
#include "MessageQueue.h"

namespace nonsugar{

//キューの最大数
static constexpr int32_t kMsgQueueMaxNum = 10;

//メッセージキューID
enum MsgQueueID : int32_t{
    MsgQueueIdDraw = 0,
    MsgQueueIdSound,
    MsgQueueIdMax = kMsgQueueMaxNum, //キューの有効数はここまで
};

//メッセージ構造体
struct ThreadMsg{
    int32_t event_id;
    virtual ~ThreadMsg(){}
};

enum class MsgQueueStatus : int8_t{
    UNUSE = 0,
    USE,
};

//メッセージキュー設定テーブル用
struct MessageConfig{
    MsgQueueID queue_id; //キューIDの有効値は0～MSG_QUEUE_MAX_NUM - 1
    MsgQueueStatus use;
};

/**
* メッセージキュー設定テーブルを元にメッセージキューを生成する
*
* @param config   --- 設定テーブル
* @param num      --- 設定テーブルの要素数
*
* @attention MSG_QUEUE_MAX_NUMより大きい要素数を設定した場合失敗する
*/
void CreateMessageQueue(const MessageConfig* config, int32_t num);

/**
* CreateMessageQueueで生成したメッセージキューを全て削除する
*
* @attention この関数より前にCreateMessageQueueを呼んでいること
*/
void DeleteMessageQueue(void);

/**
* メッセージ受信
*
* @param queue_id   --- 受信先キューのID
* @param msg        --- メッセージの格納先アドレス
*
* @attention メッセージを受信するまでブロッキングされる
*/
void ReceiveMsg(MsgQueueID queue_id, std::shared_ptr<ThreadMsg>* msg);

/**
* メッセージ受信(ポーリング)
*
* @param queue_id   --- 受信先キューのID
* @param msg        --- メッセージの格納先アドレス
*
*/
void ReceiveMsgPolling(MsgQueueID queue_id, std::shared_ptr<ThreadMsg>* msg);

/**
* メッセージ送信
*
* @param queue_id   --- 送信先キューのID
* @param msg        --- 送信するメッセージ
*
*/
void SendMsg(MsgQueueID queue_id, const std::shared_ptr<ThreadMsg>& msg);

} //namespace nonsugar
#endif //MESSAGE_H_
