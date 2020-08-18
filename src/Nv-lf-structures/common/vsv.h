#ifndef VSV_H
#define VSV_H


/*
#include <chrono>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <list>
#include <atomic>
//#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <array>
#include <map>
#include <unordered_map>
#include <climits>
#include <stack>          // std::stack
#include <math.h>       // pow 
#include <boost/random.hpp>
#include <sys/time.h>
#include <mutex>
#include "tbb/concurrent_hash_map.h"
#include <sstream>
*/

//These options are mutually exclusive
#define LINEARIZABILITY 1
#define STRICT_SERIALIZABILITY 0

//Toggle this flag for buffered durable linearizability
#define BUFFERED_LINEARIZABILITY 0

#define DEBUG_ 0

//Program Input
extern unsigned int NUM_THRDS;
extern unsigned int TEST_SIZE;
extern unsigned int TRANS_SIZE;
extern unsigned int KEY_RANGE_; //Replace TEST_SIZE*NUM_THRDS*TRANS_SIZE

typedef enum Semantics
{
	FIFO,
	LIFO,
	SET,
	MAP,
	PRIORITY
}Semantics;

extern Semantics SEMANTICS;

typedef enum Type
{
	PRODUCER,
	CONSUMER,
	READER,
	WRITER
}Type; 

typedef struct Method
{
	int id;
	int process;
	//int item;
	int item_key;
	int item_val;
	Semantics semantics;
	Type type;
    long int invocation;
	long int response;
	bool status;
	bool persist;

#if STRICT_SERIALIZABILITY
	int txn_id;
	long int txn_invocation;
	long int txn_response;
#endif

	Method(int _id, int _process, int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status) 
	{
		id = _id;
		process = _process;
		item_key = _item_key;
		item_val = _item_val;
		semantics = _semantics;
		type = _type;
		invocation = _invocation;
		response = _response;
		status = _status;
	}
}Method;

//void wait(unsigned int num_thrds);
void print_thread_lists();
void print_thread_lists_persist();

extern void vsv_init();
extern void vsv_startup();
extern void vsv_shutdown();
extern long int get_elapsed_nanoseconds();
extern void create_method(int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status);
extern void create_method_persist(int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status);
extern void insert_persist_map(void* ptr, int _item_key, int _item_val, Semantics _semantics, Type _type);
extern void handle_PWB(void* ptr, long int _invocation, long int _response);
#if STRICT_SERIALIZABILITY
extern void insert_txn(long int _txn_invocation, long int _txn_response, int size);
extern void insert_txn_persist(long int _txn_invocation, long int _txn_response, int size);
extern void rollback_txn();
extern void rollback_txn_persist();
#endif
extern void insert_method();
extern void insert_method_persist();
extern void vsv_exit();

extern void vsv_args(int argc, const char *argv[]);
extern void vsv_args2();

#endif /* end of include guard: VSV_H */
