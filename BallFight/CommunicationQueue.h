#pragma once

#include "Messages.h"

#include <queue>
#include <mutex>


class CommunicationQueue
{
public:
	CommunicationQueue();
	~CommunicationQueue();

public:
	bool Enqueue(Message* msg);
	Message* Pop();

private:
	std::queue<Message*> m_queue;
	std::mutex m_mutex;

};


