#include <fstream>
#include "thread.hpp"
#include "ts_queue.hpp"
#include "item.hpp"

#ifndef WRITER_HPP
#define WRITER_HPP

class Writer : public Thread {
public:
	// constructor
	Writer(int expected_lines, std::string output_file, TSQueue<Item*>* output_queue);

	// destructor
	~Writer();

	virtual void start() override;
private:
	// the expected lines to write,
	// the writer thread finished after output expected lines of item
	int expected_lines;

	std::ofstream ofs;
	TSQueue<Item*> *output_queue;
	pthread_t thread;
	// the method for pthread to create a writer thread
	static void* process(void* arg);
};

// Implementation start

Writer::Writer(int expected_lines, std::string output_file, TSQueue<Item*>* output_queue)
	: expected_lines(expected_lines), output_queue(output_queue) {
	ofs = std::ofstream(output_file);
}

Writer::~Writer() {
	ofs.close();
}

void Writer::start() {
	// TOD0: starts a Writer thread
	pthread_create(&thread, nullptr, Writer::process, this);
}

void* Writer::process(void* arg) {
	// TOD0: implements the Writer's work
	Writer* writer = (Writer*)arg;
	int lines_written = 0;
	while(lines_written++ < writer->expected_lines){
		Item* item = writer->output_queue->dequeue();
		writer->ofs << item->key << item->val << item->opcode << '\n';
		delete item;
		
	}
	return nullptr;
}

#endif // WRITER_HPP
