#include <assert.h>
#include <stdlib.h>
#include "ts_queue.hpp"
#include "item.hpp"
#include "reader.hpp"
#include "writer.hpp"
#include "producer.hpp"
#include "consumer_controller.hpp"

#define READER_QUEUE_SIZE 200
#define WORKER_QUEUE_SIZE 200
#define WRITER_QUEUE_SIZE 4000
#define CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE 20
#define CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE 80
#define CONSUMER_CONTROLLER_CHECK_PERIOD 1000000

int main(int argc, char** argv) {
	assert(argc == 4);

	int n = atoi(argv[1]);
	std::string input_file_name(argv[2]);
	std::string output_file_name(argv[3]);

	// TOD0: implements main function
	TSQueue<Item*>* reader_queue = new TSQueue<Item*>(READER_QUEUE_SIZE);
	TSQueue<Item*>* worker_queue = new TSQueue<Item*>(WORKER_QUEUE_SIZE);
	TSQueue<Item*>* writer_queue = new TSQueue<Item*>(WRITER_QUEUE_SIZE);

	Transformer* transformer = new Transformer();
	
	Reader* reader = new Reader(n, input_file_name, reader_queue);
	reader->start();
	Writer* writer = new Writer(n, output_file_name, writer_queue);
	writer->start();
	std::vector<Producer> producer(4, Producer(reader_queue, worker_queue, transformer));

	for (auto &prod: producer){
		prod.start();
	}
	
	ConsumerController* consumer_controller = new ConsumerController(worker_queue, writer_queue, transformer, CONSUMER_CONTROLLER_CHECK_PERIOD, 
																	WORKER_QUEUE_SIZE * CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE / 100,
																	WORKER_QUEUE_SIZE * CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE / 100);
	consumer_controller->start();
	
	reader->join();
	writer->join();

	delete reader;
	delete writer;
	delete consumer_controller;
	delete transformer;
	delete reader_queue;
	delete worker_queue;
	delete writer_queue;
	return 0;
}
