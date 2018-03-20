#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <stdint.h>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nonsugar {
    template <typename T>
    class MessageQueue final{
    private:
        std::mutex mutex_;
        std::condition_variable cond_;
        std::queue<T> msg_queue_;

    public:
        MessageQueue(void);
        ~MessageQueue(void);

        //コピー禁止
        MessageQueue(const MessageQueue&) = delete;
        void operator=( const MessageQueue& ) = delete;

    public:
        /**
        * メッセージ受信
        *
        * @param msg   --- 送信するメッセージ
        *
        */
        void Send(const T& msg);

        /**
        * メッセージ受信
        *
        * @param msg   --- メッセージの格納先
        *
        */
        void Receive(T* msg);

        void ReceivePolling(T* msg);
    };

    template <typename T>
    MessageQueue<T>::MessageQueue(void)
    {
        //nop
    }

    template <typename T>
    MessageQueue<T>::~MessageQueue(void)
    {
        //nop
    }

    template <typename T>
    void MessageQueue<T>::Send(const T& msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        msg_queue_.push(msg);

        cond_.notify_all();
    }

    template <typename T>
    void MessageQueue<T>::Receive(T* msg)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if(msg_queue_.empty()){
            cond_.wait(lock, [&]{ return msg_queue_.size(); });
        }

        *msg = msg_queue_.front();
        msg_queue_.pop();
    }

    template <typename T>
    void MessageQueue<T>::ReceivePolling(T* msg)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if(msg_queue_.empty()){
            //空の場合はメッセージに何も入れず抜ける
            return;
        }

        *msg = msg_queue_.front();
        msg_queue_.pop();
    }

} //namespace nonsugar
#endif //MESSAGEQUEUE_H_
