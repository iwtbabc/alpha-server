#include "message_queque.h"

namespace alpha{

MessageQueue::MessageQueue(){
	
	pthread_mutex_init(&mq2s_lock_, NULL);
	pthread_cond_init(&mq2s_cond_, NULL);

	pthread_mutex_init(&mq2c_lock_, NULL);

}

MessageQueue::~MessageQueue(){

	pthread_cond_destroy(&mq2s_cond_);

	if (!mq2s_.empty()){
		list<message_queue*>::iterator iter;
		for(iter = mq2s_.begin(); iter != mq2s_.end(); ++iter){
			message_queue* mq = *iter;
			delete mq;
		}
	}

	if (!mq2c_.empty())
	{
		list<message_queue*>::iterator iter2;
		for(iter2 = mq2c_.begin(); iter2 != mq2c_.end(); ++iter2){
			message_queue* mq2 = *iter2;
			delete mq2;
		}
	}
}

void MQ2S_Push(int fd, int type, std::string& buffer){
	MQ2S_Lock();

	message_queue* queue = new message_queue;
	queue->fd = fd;
	queue->type = type;
	queue->buffer = buffer;
	
	mq2s_.push_back(queue);

	MQ2S_Unlock();
}

struct message_queue* MQ2S_Pop(){

	MQ2S_Lock();

	while(mq2s_.empty()){
		MQ2S_Wait();
	}

	message_queue* queue = mq2S_.front();
	mq2s_.pop_front();

	MQ2S_Unlock();

	return queue;
}

bool MessageQueue::_Push_With_Notify(message_queue* queue)
{
	bool notify = false;
	if (mq2s_.empty())
	{
		notify = true;
	}

	mq2s_.push_back(queue);
	return notify;

}

void MessageQueue::MQ2S_Lock()
{
	pthread_mutex_lock(&mq2s_lock_);
}

void MessageQueue::MQ2S_Unlock()
{
	pthread_mutex_unlock(&mq2s_lock_);
}

void MessageQueue::MQ2S_Wait()
{
	pthread_cond_wait(&mq2s_cond_, &mq2s_lock_);
}

void MessageQueue::MQ2S_Signal()
{
	pthread_cond_signal(&mq2s_cond_);
}

void MessageQueue::MQ2S_Broadcast()
{
	pthread_cond_broadcast(&mq2s_cond_);
}

void MessageQueue::MQ2C_Lock()
{
	pthread_mutex_lock(&mq2c_lock_);
}

void MessageQueue::MQ2C_Unlock()
{
	pthread_mutex_unlock(&mq2c_lock_);
}

}