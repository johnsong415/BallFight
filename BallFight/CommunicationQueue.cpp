#include "CommunicationQueue.h"

CommunicationQueue::CommunicationQueue()
{

}

CommunicationQueue::~CommunicationQueue()
{

}

bool CommunicationQueue::Enqueue(Message* msg)
{
	m_mutex.lock();
	m_queue.push(msg);
	m_mutex.unlock();
	return true;
}

Message* CommunicationQueue::Pop()
{
    Message* front = nullptr;

	m_mutex.lock();
	if (!m_queue.empty()) {
		front = m_queue.front();
		m_queue.pop();
	}
	m_mutex.unlock();

	return front;
}
