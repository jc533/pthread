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
	TSQueue<Item*> reader_queue(READER_QUEUE_SIZE);
	TSQueue<Item*> worker_queue(WORKER_QUEUE_SIZE);
	TSQueue<Item*> writer_queue(WRITER_QUEUE_SIZE);

	Transformer transformer;
	Reader reader(n, input_file_name, &reader_queue);
	Writer writer(n, output_file_name, &writer_queue);
	Producer producer(&reader_queue, &worker_queue, &transformer);
	ConsumerController consumer_controller(&worker_queue, &writer_queue, &transformer, CONSUMER_CONTROLLER_CHECK_PERIOD, 
											WORKER_QUEUE_SIZE * CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE / 100,
											WORKER_QUEUE_SIZE * CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE / 100);

	reader.start();
	writer.start();
									
	for(int i = 1; i <= 4; i++){
		producer.start();
		std::cout << "Starting producer no: " << i << '\n';
	}
	
	consumer_controller.start();

	


	
	
	return 0;
}
