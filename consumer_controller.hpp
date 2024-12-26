#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "consumer.hpp"
#include "ts_queue.hpp"
#include "item.hpp"
#include "transformer.hpp"

#ifndef CONSUMER_CONTROLLER
#define CONSUMER_CONTROLLER

class ConsumerController : public Thread {
public:
	// constructor
	ConsumerController(
		TSQueue<Item*>* worker_queue,
		TSQueue<Item*>* writer_queue,
		Transformer* transformer,
		int check_period,
		int low_threshold,
		int high_threshold
	);

	// destructor
	~ConsumerController();

	virtual void start();

private:
	std::vector<Consumer*> consumers;

	TSQueue<Item*>* worker_queue;
	TSQueue<Item*>* writer_queue;

	Transformer* transformer;
	pthread_t thread;
	// Check to scale down or scale up every check period in microseconds.
	int check_period;
	// When the number of items in the worker queue is lower than low_threshold,
	// the number of consumers scaled down by 1.
	int low_threshold;
	// When the number of items in the worker queue is higher than high_threshold,
	// the number of consumers scaled up by 1.
	int high_threshold;

	static void* process(void* arg);
};

// Implementation start

ConsumerController::ConsumerController(
	TSQueue<Item*>* worker_queue,
	TSQueue<Item*>* writer_queue,
	Transformer* transformer,
	int check_period,
	int low_threshold,
	int high_threshold
) : worker_queue(worker_queue),
	writer_queue(writer_queue),
	transformer(transformer),
	check_period(check_period),
	low_threshold(low_threshold),
	high_threshold(high_threshold) {
}

ConsumerController::~ConsumerController() {}

void ConsumerController::start() {
	// TOD0: starts a ConsumerController thread
	pthread_create(&thread, nullptr, ConsumerController::process, this);
}

void* ConsumerController::process(void* arg) {
	// TOD0: implements the ConsumerController's work
	ConsumerController* controller = (ConsumerController*)arg;
	while(1){
		size_t queue_size = controller->worker_queue->get_size();
		if(queue_size > controller->high_threshold){
			Consumer* new_consumer = new Consumer(controller->worker_queue, controller->writer_queue, controller->transformer);
			new_consumer->start();
			int size_before = controller->consumers.size();
			controller->consumers.push_back(new_consumer);
			std::cout << "Scaling up consumers from " << size_before++ << " to " << size_before << '\n';
		}
		else if (queue_size < controller->low_threshold && controller->consumers.size() > 1){
			Consumer* consumer_cancel = controller->consumers.back();
			consumer_cancel->cancel();
			int size_before = controller->consumers.size();
			controller->consumers.pop_back();
			delete consumer_cancel;
			std::cout << "Scaling down consumers from " << size_before-- << " to " << size_before << '\n';
		}
		usleep(controller -> check_period);
	}
	return nullptr;
}

#endif // CONSUMER_CONTROLLER_HPP
