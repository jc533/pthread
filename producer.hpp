#include <pthread.h>
#include "thread.hpp"
#include "ts_queue.hpp"
#include "item.hpp"
#include "transformer.hpp"

#ifndef PRODUCER_HPP
#define PRODUCER_HPP

class Producer : public Thread {
public:
	// constructor
	Producer(TSQueue<Item*>* input_queue, TSQueue<Item*>* worker_queue, Transformer* transfomrer);

	// destructor
	~Producer();

	virtual void start();
private:
	TSQueue<Item*>* input_queue;
	TSQueue<Item*>* worker_queue;

	Transformer* transformer;

	pthread_t thread;
	// the method for pthread to create a producer thread
	static void* process(void* arg);
};

Producer::Producer(TSQueue<Item*>* input_queue, TSQueue<Item*>* worker_queue, Transformer* transformer)
	: input_queue(input_queue), worker_queue(worker_queue), transformer(transformer) {
}

Producer::~Producer() {}

void Producer::start() {
	// TOD0: starts a Producer thread
	pthread_create(&thread, nullptr, Producer::process, this);
}

void* Producer::process(void* arg) {
	// TOD0: implements the Producer's work
	Producer* producer = (Producer*)arg;
	while(1){
		Item* item = producer->input_queue->dequeue();
		item->val = producer->transformer->producer_transform(item->opcode, item->val);
		producer->worker_queue->enqueue(item);
	}
	delete producer;
	return nullptr;
}

#endif // PRODUCER_HPP
