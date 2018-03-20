#include "Message.h"
#include <stdio.h>
#include <stdlib.h>

namespace nonsugar{

struct MessageManage{
    int32_t queue_num;
    MessageQueue<std::shared_ptr<ThreadMsg>>* queue[kMsgQueueMaxNum];
};

MessageManage message_;

//#define DEBUG_PRINT(...)  printf(__VA_ARGS__)
#define DEBUG_PRINT(...) (void)(__VA_ARGS__)

void CreateMessageQueue(const MessageConfig* config, int32_t num){
    //最大数以上を指定
    if(num >= kMsgQueueMaxNum){
        DEBUG_PRINT( "[CreateMessageQueue]Parameter error\n" );
    }

    //バッファを初期化
    message_.queue_num = 0;
    for(int i = 0; i<kMsgQueueMaxNum; ++i){
        message_.queue[i] = nullptr;
    }

    //メッセージキューを生成
    for(int i = 0; i<num; ++i){
        if(config[i].use == MsgQueueStatus::UNUSE){
            continue;
        }

        if(config[i].queue_id < MsgQueueIdMax){
            message_.queue[config[i].queue_id] = new(MessageQueue<std::shared_ptr<ThreadMsg>>);
            ++message_.queue_num;
        }else{
            //0～kMsgQueueMaxNum1以上のIDは無効
            DEBUG_PRINT( "[CreateMessageQueue]BAD Queue ID\n" );
        }
    }
}

void DeleteMessageQueue(void){
    for (int i = 0; i<kMsgQueueMaxNum; ++i) {
        if(message_.queue[i] != nullptr){
            delete message_.queue[i];
        }
    }
}

void ReceiveMsg( MsgQueueID queue_id, std::shared_ptr<ThreadMsg>* msg){
    message_.queue[queue_id]->Receive(msg);
}

void ReceiveMsgPolling( MsgQueueID queue_id, std::shared_ptr<ThreadMsg>* msg){
    message_.queue[queue_id]->ReceivePolling(msg);
}

void SendMsg( MsgQueueID queue_id, const std::shared_ptr<ThreadMsg>& msg){
    message_.queue[queue_id]->Send(msg);
}

} //namespace nonsugar
