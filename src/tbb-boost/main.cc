//sudo apt-get install libtbb-dev
//sudo apt-get install libboost-dev

//#include <iostream>
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
#include <math.h>       /* pow */
#include <boost/random.hpp>
#include <sys/time.h>

#include "tbb/concurrent_queue.h"
#include <boost/lockfree/stack.hpp>
#include "tbb/concurrent_hash_map.h"

//#define NUM_THRDS 8
unsigned int NUM_THRDS;
//#define TEST_SIZE 100
unsigned int TEST_SIZE;
//#define TRANS_SIZE 2
unsigned int TRANS_SIZE;
unsigned int KEY_RANGE; //Replace TEST_SIZE*NUM_THRDS*TRANS_SIZE


//These options are mutually exclusive
#define LINEARIZABILITY 0
#define STRICT_SERIALIZABILITY 1

//Toggle this flag for buffered durable linearizability
#define BUFFERED_LINEARIZABILITY 0

#define DEBUG_ 0

unsigned int TBB_QUEUE;
unsigned int BOOST_STACK;
unsigned int TBB_MAP;

struct MyHashCompare {
    static size_t hash( int x ) {
        return x;
    }
    //! True if strings are equal
    static bool equal( int x, int y ) {
        return x==y;
    }
};

typedef enum ItemStatus
{
	PRESENT,
	ABSENT
}ItemStatus; 

typedef enum Semantics
{
	FIFO,
	LIFO,
	SET,
	MAP,
	PRIORITY
}Semantics; 

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

typedef struct Item
{
	int key;
	int value;
	//int sum;
	double sum;
	
	//READS needs a separate sum, check linearizability by taking ceiling of sum + sum_reads > 0

	long int numerator;
	long int denominator;	

	double exponent; 

	ItemStatus status;
	
	//std::stack<int> promote_items;
	std::list<int> promote_items;

	std::list<Method> demote_methods;

	std::map<long int,Method,bool(*)(long int,long int)>::iterator producer;

	//Failed Consumer
	double sum_f;
	long int numerator_f;
	long int denominator_f;	
	double exponent_f;

	//Reader
	double sum_r;
	long int numerator_r;
	long int denominator_r;	
	double exponent_r;

	Item() {} //default

	Item(int _key) 
	{
		key = _key;
		value = INT_MIN;
		sum = 0;
		numerator = 0;
		denominator = 1;
		exponent = 0;
		status = PRESENT;
		sum_f = 0;
		numerator_f = 0;
		denominator_f = 1;
		exponent_f = 0;
		sum_r = 0;
		numerator_r = 0;
		denominator_r = 1;
		exponent_r = 0;
	}

	Item(int _key, int _val) 
	{
		key = _key;
		value = _val;
		sum = 0;
		numerator = 0;
		denominator = 1;
		exponent = 0;
		status = PRESENT;
		sum_f = 0;
		numerator_f = 0;
		denominator_f = 1;
		exponent_f = 0;
		sum_r = 0;
		numerator_r = 0;
		denominator_r = 1;
		exponent_r = 0;
	}

	void add_int(long int x)
	{
		long int add_num = x * denominator;
		numerator = numerator + add_num;
		//printf("add_num = %ld, numerator/denominator = %ld\n", add_num, numerator/denominator);
		sum = (double) numerator/denominator;
	}
	void sub_int(long int x)
	{
		long int sub_num = x * denominator;
		numerator = numerator - sub_num;
		//printf("sub_num = %ld, numerator/denominator = %ld\n", sub_num, numerator/denominator);
		sum = (double) numerator/denominator;
	}
	void add_frac(long int num, long int den)
	{
		//printf("num = %ld, den = %ld, numerator = %ld, denominator = %ld\n", num, den, numerator, denominator);
#if DEBUG_
		if(den == 0)
			printf("WARNING: add_frac: den = 0\n");
		if(denominator == 0)
			printf("WARNING: add_frac: 1. denominator = 0\n");
#endif
		if(denominator%den == 0)
		{
			numerator = numerator + num * denominator/den;
		} else if (den%denominator == 0) {		
			numerator = numerator * den/denominator + num;
			denominator = den;
		} else {
			numerator = numerator * den + num * denominator;
			denominator = denominator * den;
		}
#if DEBUG_
		if(denominator == 0)
			printf("WARNING: add_frac: 2. denominator = 0\n");
#endif
		sum = (double) numerator/denominator;

		//if(sum < 0)
			//printf("WARNING: ADD sum < 0\n");
	}

	void sub_frac(long int num, long int den)
	{
#if DEBUG_
		if(den == 0)
			printf("WARNING: sub_frac: den = 0\n");
		if(denominator == 0)
			printf("WARNING: sub_frac: 1. denominator = 0\n");
#endif
		if(denominator%den == 0)
		{
			numerator = numerator - num * denominator/den;
		} else if (den%denominator == 0) {		
			numerator = numerator * den/denominator - num;
			denominator = den;
		} else {
			numerator = numerator * den - num * denominator;
			denominator = denominator * den;
		}
#if DEBUG_
		if(denominator == 0)
			printf("WARNING: sub_frac: 2. denominator = 0\n");
#endif
		sum = (double) numerator/denominator;

	}
	
	void demote()
	{
		exponent = exponent + 1;
		long int den = (long int) pow (2, exponent);
		//printf("denominator = %ld\n", den);
		sub_frac(1, den);
	}

	void promote()
	{
		long int den = (long int) pow (2, exponent);
#if DEBUG_
		if(den == 0)
			printf("2 ^ %f = %ld?\n", exponent, den);
#endif
		if(exponent < 0)
		{
			den = 1;
			printf("exponent = %.2f\n", exponent);
		}
		//printf("denominator = %ld\n", den);
		add_frac(1, den);
		exponent = exponent - 1;
	}

	//Failed Consumer
	void add_frac_f(long int num, long int den)
	{
#if DEBUG_
		if(den == 0)
			printf("WARNING: add_frac_f: den = 0\n");
		if(denominator_f == 0)
			printf("WARNING: add_frac_f: 1. denominator_f = 0\n");
#endif
		if(denominator_f%den == 0)
		{
			numerator_f = numerator_f + num * denominator_f/den;
		} else if (den%denominator_f == 0) {		
			numerator_f = numerator_f * den/denominator_f + num;
			denominator_f = den;
		} else {
			numerator_f = numerator_f * den + num * denominator_f;
			denominator_f = denominator_f * den;
		}
#if DEBUG_
		if(denominator_f == 0)
			printf("WARNING: add_frac_f: 2. denominator_f = 0\n");
#endif
		sum_f = (double) numerator_f/denominator_f;
	}

	void sub_frac_f(long int num, long int den)
	{
#if DEBUG_
		if(den == 0)
			printf("WARNING: sub_frac_f: den = 0\n");
		if(denominator_f == 0)
			printf("WARNING: sub_frac_f: 1. denominator_f = 0\n");
#endif
		if(denominator_f%den == 0)
		{
			numerator_f = numerator_f - num * denominator_f/den;
		} else if (den%denominator_f == 0) {		
			numerator_f = numerator_f * den/denominator_f - num;
			denominator_f = den;
		} else {
			numerator_f = numerator_f * den - num * denominator_f;
			denominator_f = denominator_f * den;
		}
#if DEBUG_
		if(denominator_f == 0)
			printf("WARNING: sub_frac_f: 2. denominator_f = 0\n");
#endif
		sum_f = (double) numerator_f/denominator_f;
	}

	void demote_f()
	{
		exponent_f = exponent_f + 1;
		long int den = (long int) pow (2, exponent_f);
		//printf("denominator = %ld\n", den);
		sub_frac_f(1, den);
	}

	void promote_f()
	{
		long int den = (long int) pow (2, exponent_f);
		//printf("denominator = %ld\n", den);
		add_frac_f(1, den);
		exponent_f = exponent_f - 1;
	}	

	//Reader
	void add_frac_r(long int num, long int den)
	{
		if(denominator_r%den == 0)
		{
			numerator_r = numerator_r + num * denominator_r/den;
		} else if (den%denominator_r == 0) {		
			numerator_r = numerator_r * den/denominator_r + num;
			denominator_r = den;
		} else {
			numerator_r = numerator_r * den + num * denominator_r;
			denominator_r = denominator_r * den;
		}
		sum_r = (double) numerator_r/denominator_r;
	}

	void sub_frac_r(long int num, long int den)
	{
		if(denominator_r%den == 0)
		{
			numerator_r = numerator_r - num * denominator_r/den;
		} else if (den%denominator_r == 0) {		
			numerator_r = numerator_r * den/denominator_r - num;
			denominator_r = den;
		} else {
			numerator_r = numerator_r * den - num * denominator_r;
			denominator_r = denominator_r * den;
		}
		sum_r = (double) numerator_r/denominator_r;
	}

	void demote_r()
	{
		exponent_r = exponent_r + 1;
		long int den = (long int) pow (2, exponent_r);
		//printf("denominator = %ld\n", den);
		sub_frac_r(1, den);
	}

	void promote_r()
	{
		long int den = (long int) pow (2, exponent_r);
		//printf("denominator = %ld\n", den);
		add_frac_r(1, den);
		exponent_r = exponent_r - 1;
	}	

}Item;

FILE *pfile;

unsigned int method_count;

#if BUFFERED_LINEARIZABILITY
long int sync_time;
#endif

bool fncomp (long int lhs, long int rhs) {return lhs < rhs;}

tbb::concurrent_queue<int> queue;
boost::lockfree::stack<int> stack(128);
tbb::concurrent_hash_map<int,int,MyHashCompare> map;

//std::list<Method> thrd_lists[NUM_THRDS];
std::list<Method>* thrd_lists;
//std::list<Method> thrd_lists_persist[NUM_THRDS];
std::list<Method>* thrd_lists_persist;

//std::atomic<int> thrd_lists_size[NUM_THRDS];
std::atomic<int>* thrd_lists_size;
//std::atomic<int> thrd_lists_size_persist[NUM_THRDS];
std::atomic<int>* thrd_lists_size_persist;

std::unordered_map<int,int> incorrect_methods;

//std::atomic<bool> done[NUM_THRDS];
std::atomic<bool>* done;

std::atomic<int> barrier(0);

void wait(unsigned int num_thrds)
{
	barrier.fetch_add(1);
	while(barrier.load() < num_thrds) { }
}

//long int method_time[NUM_THRDS];
long int* method_time;

std::chrono::time_point<std::chrono::high_resolution_clock> start;

long int elapsed_time_verify;

bool final_outcome;
bool final_outcome_persist;
bool final_outcome_compare;
std::thread* t;
std::thread v;

//void handle_failed_consumer(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item* vector_items[], std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
//void handle_failed_consumer(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, std::vector<Item*>& vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
void handle_failed_consumer(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
{
	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_0;	
	for (it_0=map_methods.begin(); it_0 != it; ++it_0)
	{
#if LINEARIZABILITY
		if(it_0->second.response < it->second.invocation)
#elif STRICT_SERIALIZABILITY
		//if(it_0->second.response < it->second.invocation)
		if((it_0->second.txn_id == it->second.txn_id && it_0->second.response < it->second.invocation) || (it_0->second.txn_response < it->second.txn_invocation))
#endif
		{
			//std::unordered_map<int,Item>::iterator it_item_0;
			//it_item_0 = map_items.find(it_0->second.item_key);
			
			Item* vec_item_0;
			if(it_0->second.item_key != INT_MIN)
				vec_item_0 = vector_items[it_0->second.item_key];
			else
				vec_item_0 = vector_items[KEY_RANGE];
			
			if(it_0->second.type == PRODUCER && vec_item->status == PRESENT && (it_0->second.semantics == FIFO || it_0->second.semantics == LIFO || it->second.item_key == it_0->second.item_key))
			{
				stack_failed.push(vec_item_0);
			}
		}
	}
}

//void handle_failed_read(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, std::vector<Item*>& vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
//void handle_failed_read(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item* vector_items[], std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
void handle_failed_read(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it, Item* vec_item, std::stack<Item*>& stack_failed)
{
	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_0;	
	for (it_0=map_methods.begin(); it_0 != it; ++it_0)
	{
#if LINEARIZABILITY
		if(it_0->second.response < it->second.invocation)
#elif STRICT_SERIALIZABILITY
		//if(it_0->second.response < it->second.invocation)
		if((it_0->second.txn_id == it->second.txn_id && it_0->second.response < it->second.invocation) || (it_0->second.txn_response < it->second.txn_invocation))
#endif
		{
			//std::unordered_map<int,Item>::iterator it_item_0;
			//it_item_0 = map_items.find(it_0->second.item_key);
			
			Item* vec_item_0;
			if(it_0->second.item_key != INT_MIN)
				vec_item_0 = vector_items[it_0->second.item_key];
			else
				vec_item_0 = vector_items[KEY_RANGE];
			
			if(it_0->second.type == PRODUCER && vec_item->status == PRESENT && it->second.item_key == it_0->second.item_key)
			{
				stack_failed.push(vec_item_0);
			}
		}
	}
}

//void verify_checkpoint(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, std::vector<Item*>& vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it_start, long unsigned int& count_iterated, long int min, bool reset_it_start)
//void verify_checkpoint(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item* vector_items[], std::map<long int,Method,bool(*)(long int,long int)>::iterator& it_start, long unsigned int& count_iterated, long int min, bool reset_it_start)
void verify_checkpoint(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::map<long int,Method,bool(*)(long int,long int)>::iterator& it_start, long unsigned int& count_iterated, long int min, bool reset_it_start, bool* final_outcome)
{
	//bool is_quiescent = true; //ADD to VectorSpace

	if(!map_methods.empty())
	{
		std::stack<Item*> stack_consumer;
		std::stack<std::map<long int,Method,bool(*)(long int,long int)>::iterator> stack_finished_methods;
		std::stack<Item*> stack_failed;

		//bool reset_it_start = true;
		std::map<long int,Method,bool(*)(long int,long int)>::iterator it;

		if(count_iterated == 0)
		{
			it=map_methods.begin();
			reset_it_start = false;
		//} else if(it != map_methods.end()) {
		} else if(it_start != map_methods.end()) { //ADD to VectorSpace
			it=++it_start;
		} else {
			it = it_start;
		}
	
		for (; it!=map_methods.end(); ++it)
		{
			/*if(it->second.response > min) //ADD to VectorSpace
			{
				long int invoc[NUM_THRDS] = {};
				std::map<long int,Method,bool(*)(long int,long int)>::iterator q_check;
				int q_count = 0;
				for(q_check = it; q_check != map_methods.end(); ++q_check)
				{
					if(invoc[q_check->second.process] != 0)
					{
						invoc[q_check->second.process] = q_check->second.invocation;
						if(q_check->second.invocation < min)
						{
							is_quiescent = false;
							break;
						}

						q_count = q_count + 1;
						if(q_count >= (NUM_THRDS -1))
							break;
					}
				}

				break;
			}*/
#if DEBUG_
			//if(vec_item->status != PRESENT)
				//printf("WARNING: Current item not present!\n");

			if((it->second).type == PRODUCER)
				printf("PRODUCER invocation %ld, response %ld, item %d\n", it->second.invocation, it->second.response, it->second.item_key);
			else if((it->second).type == CONSUMER)
				printf("CONSUMER invocation %ld, response %ld, item %d\n", it->second.invocation, it->second.response, it->second.item_key);
#endif


			if(it->second.response > min) 
			{
				break;
			}

			if(method_count%5000 == 0)
				printf("Method Count = %u\n", method_count);
			method_count = method_count + 1;

			it_start = it;
			reset_it_start = false;
			count_iterated = count_iterated + 1;

			if(it->second.item_key != INT_MIN && vector_items[it->second.item_key] == NULL)
			{
				Item* item = new Item(it->second.item_key);
				item->producer = map_methods.end();
				vector_items[it->second.item_key] = item;
			} else if(it->second.item_key == INT_MIN && vector_items[KEY_RANGE] == NULL)
			{
				Item* item = new Item(it->second.item_key);
				item->producer = map_methods.end();
				vector_items[KEY_RANGE] = item;
			}
			
			Item* vec_item = NULL;
			if(it->second.item_key != INT_MIN) {
				vec_item = vector_items[it->second.item_key];
			} else {	
				vec_item = vector_items[KEY_RANGE];
				//vec_item->producer = map_methods.end();
			}

			if(it->second.type == PRODUCER)
			{
				vec_item->producer = it;

				vec_item->add_int(1);
	
				/*if(vec_item->status == ABSENT)
				{				
					vec_item->status = PRESENT;
					vec_item->demote_methods.clear();
				}*/
				
				if(vec_item->status == PRESENT) //ADD to VectorSpace
				{
					if(it->second.semantics == FIFO || it->second.semantics == LIFO)
					{
						std::map<long int,Method,bool(*)(long int,long int)>::iterator it_0;	
						for (it_0=map_methods.begin(); it_0 != it; ++it_0)
						{
		#if LINEARIZABILITY
							if(it_0->second.response < it->second.invocation)
		#elif STRICT_SERIALIZABILITY
							//if(it_0->second.response < it->second.invocation)
							if((it_0->second.txn_id == it->second.txn_id && it_0->second.response < it->second.invocation) || (it_0->second.txn_response < it->second.txn_invocation))
		#endif
							{
								
								Item* vec_item_0;
								if(it_0->second.item_key != INT_MIN)
									vec_item_0 = vector_items[it_0->second.item_key];
								else	
									vec_item_0 = vector_items[KEY_RANGE];
							
								//Demotion
								//FIFO Semantics
								if((it_0->second.type == PRODUCER && vec_item_0->status == PRESENT) && (it->second.type == PRODUCER) && (it_0->second.semantics == FIFO))
								{
									//vec_item_0->promote_items.push(vec_item->key);
									vec_item_0->promote_items.push_back(vec_item->key);
									vec_item->demote();
									vec_item->demote_methods.push_back(it_0->second);
									
								} 
								
								//LIFO Semantics
								if((it_0->second.type == PRODUCER && vec_item_0->status == PRESENT) && (it->second.type == PRODUCER) && (it_0->second.semantics == LIFO))
								{
									//vec_item->promote_items.push(vec_item_0->key);
									vec_item->promote_items.push_back(vec_item_0->key);
									vec_item_0->demote();
									vec_item_0->demote_methods.push_back(it->second);

								} 

							}
						}
					}
				}
			}
			if(it->second.type == READER) //&& (it->second.semantics == SET || it->second.semantics == MAP)
			{
				if(it->second.status == true)
				{
					vec_item->demote_r();

				} else {
					//TODO: Probably should handle the same as a failed consumer
					handle_failed_read(map_methods, vector_items, it, vec_item, stack_failed);
				}

			}

			//if(((it->second).type == CONSUMER) && ((it->second).semantics == FIFO))
			if(it->second.type == CONSUMER)
			{
				//printf("Consume Item %d\n", it->second.item_key);
				if(it->second.status == true)
				{
					//PROMOTE READS
					if(vec_item->sum > 0)
					{
						vec_item->sum_r = 0;
					}

					vec_item->sub_int(1);

					vec_item->status = ABSENT;

					
					if(vec_item->sum < 0)
					{
						//printf("WARNING: Sum < 0\n");
						std::list<Method>::iterator it_method;
						for(it_method = vec_item->demote_methods.begin(); it_method != vec_item->demote_methods.end(); ++it_method)
						{
							//printf("Item %d considers promoting Item %d\n", it_method->item_key, vec_item->key);

							if((it->second.response < it_method->invocation) || (it_method->response < it->second.invocation))
							{
								//Methods do not overlap
								//printf("NOTE: Methods do not overlap\n");
							} else {
								//printf("NOTE: CONSUME Item %d overlaps with PRODUCE Item %d\n", it->second.item_val, it_method->item_val);
								
								vec_item->promote();

#if DEBUG_
								printf("Item %d promotes Item %d\n", it_method->item_key, vec_item->key);
#endif

								//Need to remove from promote list
								
								Item* vec_method_item;
								if(it_method->item_key != INT_MIN)
									vec_method_item = vector_items[it_method->item_key];
								else
									vec_method_item = vector_items[KEY_RANGE];
								
								/*std::stack<int> temp;

								while(!vec_method_item->promote_items.empty())
								{
									int top = vec_method_item->promote_items.top();
									if(top != it->second.item_key)
									{
										temp.push(top);
									}
									vec_method_item->promote_items.pop();
									//printf("Stuck here?\n");
								}
								vec_method_item->promote_items.swap(temp);*/

								std::list<int>::iterator it_item;
								for(it_item = vec_method_item->promote_items.begin(); it_item != vec_method_item->promote_items.end(); ++it_item)
								{
									if(*it_item == it->second.item_key)
										it_item = vec_method_item->promote_items.erase(it_item);
								}

								it_method = vec_item->demote_methods.erase(it_method);
								--it_method;
								
							}

						}
					}

					stack_consumer.push(vec_item);

					//map_methods.erase(it); //TODO: Dangerous, make sure this is correct
					stack_finished_methods.push(it);
					//printf("stack_finished_methods.push(Consume(%d))\n", it->second.item_key);

					if(vec_item->producer != map_methods.end())
					{
						
						stack_finished_methods.push(vec_item->producer);
						//printf("stack_finished_methods.push(Produce(%d))\n", vec_item->producer->second.item_key);
					}

				} else {	
					handle_failed_consumer(map_methods, vector_items, it, vec_item, stack_failed);
				}
			}
		}
		if(reset_it_start)
		{
			--it_start;
		}
		
		//NEED TO FLAG ITEMS ASSOCIATED WITH CONSUMER METHODS AS ABSENT
		while(!stack_consumer.empty())
		{
			Item* it_top = stack_consumer.top();

			int item_promote;

			std::list<int>::iterator it_item1;
			for(it_item1 = it_top->promote_items.begin(); it_item1 != it_top->promote_items.end(); ++it_item1)
			{
				item_promote = *it_item1;
				Item* vec_promote_item;
				if(item_promote != INT_MIN)
					vec_promote_item = vector_items[item_promote];
				else
					vec_promote_item = vector_items[KEY_RANGE];
				vec_promote_item->promote();
#if DEBUG_
				printf("Item %d promotes Item %d\n", it_top->key, item_promote);
#endif
			}

			/*	
			while(!it_top->promote_items.empty())
			{
				//TODO: Check that item is not ABSENT
				item_promote = it_top->promote_items.top();
				Item* vec_promote_item;
				if(item_promote != INT_MIN)
					vec_promote_item = vector_items[item_promote];
				else
					vec_promote_item = vector_items[KEY_RANGE];
				vec_promote_item->promote();
				it_top->promote_items.pop();
				printf("Item %d promotes Item %d\n", it_top->key, vec_promote_item->key);
			}*/
			//printf("\n");

			

			stack_consumer.pop();
		}	

		while(!stack_failed.empty())
		{
			Item* it_top = stack_failed.top();

			if(it_top->status == PRESENT)
			{
				it_top->demote_f();
			}
			
			stack_failed.pop();
		}

		//Remove methods that are no longer active
		
		while(!stack_finished_methods.empty())
		{
			std::map<long int,Method,bool(*)(long int,long int)>::iterator methods_top = stack_finished_methods.top();
			
			//if(methods_top->second.type == PRODUCER)
				//printf("Erase PRODUCER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
			//else if (methods_top->second.type == CONSUMER)
				//printf("Erase CONSUMER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
				
			if(methods_top->second.item_key != it_start->second.item_key) 
				map_methods.erase(methods_top); //TODO: Dangerous, make sure this is correct, fails with the persistent verification
			stack_finished_methods.pop();
		}

		//Verify Sums 
		bool outcome = true; //ADD to VectorSpace

		//if(is_quiescent) //ADD to VectorSpace
		//{
			Item* vec_verify;
			for (int i = 0; i < KEY_RANGE+1; i++)
			{
				vec_verify = vector_items[i];

				if(vec_verify == NULL) continue;

				if(vec_verify->sum < 0)
				{
					outcome = false;
	//#if DEBUG_
					//printf("WARNING: Item %d, sum %.2lf\n", it_verify->second.key, it_verify->second.sum);
					printf("WARNING: Item %d, sum %.2lf\n", vec_verify->key, vec_verify->sum);
					std::pair<int,int> mypair (vec_verify->key,vec_verify->key);
					incorrect_methods.insert(mypair);
					printf("Demote list:\n");
					std::list<Method>::iterator it_m;
					for(it_m = vec_verify->demote_methods.begin(); it_m != vec_verify->demote_methods.end(); ++it_m)
					{
						printf("%d, Promote List: ", it_m->item_key);
						Item* vec_promote_item;
						if(it_m->item_key != INT_MIN)
							vec_promote_item = vector_items[it_m->item_key];
						else
							vec_promote_item = vector_items[KEY_RANGE];

						std::list<int>::iterator it_item1;
						for(it_item1 = vec_promote_item->promote_items.begin(); it_item1 != vec_promote_item->promote_items.end(); ++it_item1)
						{
							printf("%d ", *it_item1);
						}
						printf("\n");
					}
					printf("\n");		

	//#endif
				}
				//printf("Item %d, sum %.2lf\n", it_verify->second.key, it_verify->second.sum);

				if((ceil(vec_verify->sum) + vec_verify->sum_r) < 0)
				{
					outcome = false;
	#if DEBUG_
					//printf("WARNING: Item %d, sum_r %.2lf\n", it_verify->second.key, it_verify->second.sum_r);
					printf("WARNING: Item %d, sum_r %.2lf\n", vec_verify->key, vec_verify->sum_r);
	#endif
				}

				int N;
				if(vec_verify->sum_f == 0)
					N = 0;
				else
					N = -1;

				if(((ceil(vec_verify->sum) + vec_verify->sum_f) * N) < 0)
				{
					outcome = false;
	#if DEBUG_
					//printf("WARNING: Item %d, sum_f %.2lf\n", it_verify->second.key, it_verify->second.sum_f);
					printf("WARNING: Item %d, sum_f %.2lf\n", vec_verify->key, vec_verify->sum_f);
	#endif
				}


			}
		//}

		if(outcome == true)
		{
			*final_outcome = true;
#if DEBUG_
			printf("-------------Program Correct Up To This Point-------------\n");
#endif
		} else
		{
			*final_outcome = false;
#if DEBUG_
			printf("-------------Program Not Correct-------------\n");
#endif
		}
		

	}
}

//Should probably move vector sum computation to this function
void compare_vectors(Item** vector_items1, Item** vector_items2, bool* outcome_compare)
{
	for (int i = 0; i < KEY_RANGE+1; i++)
	{
		if(vector_items1[i] != NULL && vector_items2[i] == NULL)
		{
			printf("vector_items1[%d]->sum = %.2lf\n", i, vector_items1[i]->sum);
			*outcome_compare = false;
		} else if(vector_items1[i] == NULL && vector_items2[i] != NULL)
		{
			printf("vector_items2[%d]->sum = %.2lf\n", i, vector_items2[i]->sum);
			*outcome_compare = false;	
		} else if(vector_items1[i] != NULL && vector_items2[i] != NULL)
		{
			printf("vector_items1[%d]->sum = %.2lf, vector_items2[%d]->sum = %.2lf\n", i, vector_items1[i]->sum, i, vector_items2[i]->sum);
			if(vector_items1[i]->sum != vector_items2[i]->sum)	
				*outcome_compare = false;
		}
	}
}

long int get_elapsed_nanoseconds()
{
	//Get global start time
	auto start_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(start);
	auto start_time_epoch = start_time.time_since_epoch();

	//Get current start time
	std::chrono::time_point<std::chrono::high_resolution_clock> curr = std::chrono::high_resolution_clock::now();
	auto curr_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(curr);
	auto curr_time_epoch = curr_time.time_since_epoch();

	return curr_time_epoch.count() - start_time_epoch.count();
}

void create_method(int _iteration, int _operation, Method** m1, Method** m1_persist, int _process, int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status)
{
	int m_id = _process + 1 + (_iteration * TRANS_SIZE + _operation) * NUM_THRDS;
	m1[_operation] = new Method(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	m1_persist[_operation] = new Method(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
}

#if STRICT_SERIALIZABILITY
void insert_txn(Method** m1, Method** m1_persist, int _process, long int _txn_invocation, long int _txn_response)
{
	for(unsigned int j = 0; j < TRANS_SIZE; j++)
	{
		m1[j]->txn_invocation = _txn_invocation;
		m1[j]->txn_response = _txn_response;
		m1[j]->txn_id = m1[0]->id;
		thrd_lists[_process].push_back(*m1[j]);
	
		m1_persist[j]->txn_invocation = _txn_invocation; 
		m1_persist[j]->txn_response = _txn_response; 
		m1_persist[j]->txn_id = m1_persist[0]->id; 	
		thrd_lists_persist[_process].push_back(*m1_persist[j]);

		//printf("Thread %d: txn_id = %d, method_id = %d, txn_inv = %ld, txn_res = %ld \n", id, txn_id, (*it_list).id, txn_invocation, txn_response);
		
	}
	thrd_lists_size[_process].fetch_add(TRANS_SIZE);
	thrd_lists_size_persist[_process].fetch_add(TRANS_SIZE);

}
#endif

void insert_method(Method** m1, Method** m1_persist, int _process)
{
	for(unsigned int j = 0; j < TRANS_SIZE; j++)
	{
		thrd_lists[_process].push_back(*m1[j]);
		thrd_lists_persist[_process].push_back(*m1_persist[j]);

		//printf("Thread %d: txn_id = %d, method_id = %d, txn_inv = %ld, txn_res = %ld \n", id, txn_id, (*it_list).id, txn_invocation, txn_response);
		
	}
	thrd_lists_size[_process].fetch_add(TRANS_SIZE);
	thrd_lists_size_persist[_process].fetch_add(TRANS_SIZE);
}

void vsv_exit(int _process)
{
	done[_process].store(true);
}


void work_queue(int id)
{
	
	//int keyRange = 20;

	double wallTime = 0.0;
    
	timeval tod;
	gettimeofday(&tod,0);
	wallTime += static_cast<double>(tod.tv_sec);
	wallTime += static_cast<double>(tod.tv_usec) * 1e-6;

    //boost::mt19937 randomGenKey;
    boost::mt19937 randomGenOp;
    //randomGenKey.seed(wallTime + id);
    randomGenOp.seed(wallTime + id + 1000);
    //boost::uniform_int<int> randomDistKey(1, keyRange);
    boost::uniform_int<unsigned int> randomDistOp(1, 100);

	//int m_id = id;
	//int m_id = id + 1;
	
	//printf("Spawning thread %d, start = %ld\n", id, start_time_epoch.count());
	
	//auto start_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(start);
	//auto start_time_epoch = start_time.time_since_epoch();
	//std::chrono::time_point<std::chrono::high_resolution_clock> end;

	wait(NUM_THRDS);

	for(unsigned int i = 0; i < TEST_SIZE; i++) 
	{
		Method** m1 = new Method*[TRANS_SIZE];
		Method** m1_persist = new Method*[TRANS_SIZE];
#if STRICT_SERIALIZABILITY
	
		long int txn_invocation = get_elapsed_nanoseconds();

		for(unsigned int j = 0; j < TRANS_SIZE; j++)
		{
#endif
		Type type;
		int item_key = -1;
		bool res = true;
		
		uint32_t op_dist = randomDistOp(randomGenOp);

		long int invocation = get_elapsed_nanoseconds();

		//if(invocation.count() > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		if(invocation > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		{
#if DEBUG_
			printf("WARNING: TIME LIMIT REACHED! TERMINATING PROGRAM\n");
#endif
			break;
		}
		
		if(op_dist <= 50)
		//if(op_dist <= 33)
		{
			type = CONSUMER;
			int item_pop;
			long unsigned int* item_pop_ptr;
if (TBB_QUEUE) {
			res = queue.try_pop (item_pop);
} 
			if(res)
			{
if (TBB_QUEUE) {
				item_key = item_pop;
} 
			} else
				item_key = INT_MIN;
		} else {
			type = PRODUCER;
			//item_key = randomDistKey(randomGenKey);
			//item_key = m_id;
#if STRICT_SERIALIZABILITY
		item_key = id + 1 + (i * TRANS_SIZE + j) * NUM_THRDS;
#else
		item_key = id + 1 + i * NUM_THRDS;
#endif
if (TBB_QUEUE) {
			queue.push(item_key);
}
		}

		long int response = get_elapsed_nanoseconds();
		method_time[id] = method_time[id] + (response - invocation);

#if BUFFERED_LINEARIZABILITY
		if(id%NUM_THRDS == 0 && i%5 == 0)
			sync_time = response;
#endif
#if STRICT_SERIALIZABILITY
		
		create_method(i, j, m1, m1_persist, id, item_key, INT_MIN, FIFO, type, invocation, response, res);
		
		}
		
		long int txn_response = get_elapsed_nanoseconds();
		
		insert_txn(m1, m1_persist, id, txn_invocation, txn_response);
		
#else
		create_method(i, 0, m1, m1_persist, id, item_key, INT_MIN, FIFO, type, invocation, response, res);
		insert_method(m1, m1_persist, id);

#endif
	}
	
	vsv_exit(id);
}


void work_stack(int id)
{
	//int keyRange = 20;
	unsigned int testSize = TEST_SIZE;

	double wallTime = 0.0;
    
	timeval tod;
	gettimeofday(&tod,0);
	wallTime += static_cast<double>(tod.tv_sec);
	wallTime += static_cast<double>(tod.tv_usec) * 1e-6;

    //boost::mt19937 randomGenKey;
    boost::mt19937 randomGenOp;
    //randomGenKey.seed(wallTime + id);
    randomGenOp.seed(wallTime + id + 1000);
    //boost::uniform_int<int> randomDistKey(1, keyRange);
    boost::uniform_int<unsigned int> randomDistOp(1, 100);
	
	auto start_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(start);
	auto start_time_epoch = start_time.time_since_epoch();

	int m_id = id;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> end;

	wait(NUM_THRDS);
	for(unsigned int i = 0; i < testSize; i++)
	{
		Type type;

		int item_key = -1;

		bool res = true;

		uint32_t op_dist = randomDistOp(randomGenOp);
		
		end = std::chrono::high_resolution_clock::now();

		auto pre_function = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
		auto pre_function_epoch = pre_function.time_since_epoch();

		//auto invocation =
		//std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		//printf("Thread %d invocation: %ld nanoseconds\n", id, invocation.count());

		long int invocation = pre_function_epoch.count() - start_time_epoch.count();

		//if(invocation.count() > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		if(invocation > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		{
#if DEBUG_
			printf("WARNING: TIME LIMIT REACHED! TERMINATING PROGRAM\n");
#endif
			break;
		}
		
		if(op_dist <= 50)
		{	
			type = CONSUMER;
			int item_pop;
if (BOOST_STACK) {
			res = stack.pop (item_pop);
}
			if(res)
				item_key = item_pop;
			else 
				item_key = INT_MIN;
			
		} else {
			type = PRODUCER;
			item_key = m_id;
if (BOOST_STACK) {
			stack.push(item_key);
}
		}
				
		end = std::chrono::high_resolution_clock::now();

		auto post_function = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
		auto post_function_epoch = post_function.time_since_epoch();
		
		//printf("Thread %d, item %d: prefunction = %ld, postfunction = %ld\n", id, item_key, pre_function_epoch.count(), post_function_epoch.count());

		//auto response =
		//std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		//printf("Thread %d response: %ld nanoseconds\n", id, response.count());

		long int response = post_function_epoch.count() - start_time_epoch.count();

		//Method m1(m_id, id, item_key, LIFO, type, invocation.count(), response.count(), res);
		Method m1(m_id, id, item_key, INT_MIN, LIFO, type, invocation, response, res);
		Method m1_persist(m_id, id, item_key, INT_MIN, LIFO, type, invocation, response, res);
			
		m_id = m_id + NUM_THRDS;
		
		thrd_lists[id].push_back(m1);	
		thrd_lists_size[id].fetch_add(1);

		//Persist
		//if(op_dist >= 25) //25% chance that method call won't be persisted
		{
			thrd_lists_persist[id].push_back(m1_persist);
			thrd_lists_size_persist[id].fetch_add(1);
		}
		
		//method_time[id] = method_time[id] + (response.count() - invocation.count());
		method_time[id] = method_time[id] + (response - invocation);

#if BUFFERED_LINEARIZABILITY
		if(id%NUM_THRDS == 0 && i%5 == 0)
			sync_time = response;
#endif
		
	}
	
	done[id].store(true);
}

void work_map(int id)
{

	//int keyRange = 20;
	unsigned int testSize = TEST_SIZE;

	double wallTime = 0.0;
    
	timeval tod;
	gettimeofday(&tod,0);
	wallTime += static_cast<double>(tod.tv_sec);
	wallTime += static_cast<double>(tod.tv_usec) * 1e-6;

    //boost::mt19937 randomGenKey;
    boost::mt19937 randomGenOp;
    //randomGenKey.seed(wallTime + id);
    randomGenOp.seed(wallTime + id + 1000);
    //boost::uniform_int<int> randomDistKey(1, keyRange);
    boost::uniform_int<unsigned int> randomDistOp(1, 100);

	auto start_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(start);
	auto start_time_epoch = start_time.time_since_epoch();

	int m_id = id;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> end;

	wait(NUM_THRDS);
	for(unsigned int i = 0; i < testSize; i++) 
	{
		Type type;

		int item_key = -1;

		int item_val = -1;

		bool res = true;

		uint32_t op_dist = randomDistOp(randomGenOp);
		
		end = std::chrono::high_resolution_clock::now();

		auto pre_function = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
		auto pre_function_epoch = pre_function.time_since_epoch();

		//auto invocation =
		//std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		//printf("Thread %d invocation: %ld nanoseconds\n", id, invocation.count());

		long int invocation = pre_function_epoch.count() - start_time_epoch.count();

		//if(invocation.count() > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		if(invocation > (LONG_MAX - 10000000000)) //10 seconds until time exceeds limit
		{
#if DEBUG_
			printf("WARNING: TIME LIMIT REACHED! TERMINATING PROGRAM\n");
#endif
			break;
		}

//if (TBB_MAP) {
		tbb::concurrent_hash_map<int,int,MyHashCompare>::accessor a;
//} 

		if(op_dist <= 33)
		{
			type = CONSUMER;
			int item_erase = m_id - 2*NUM_THRDS;
if (TBB_MAP) {
			res = map.erase( item_erase );
}
			if(res)
			{
				item_key = item_erase;
			} else {
				item_key = INT_MIN;	
			}

		} else if (op_dist <= 66) {
			type = PRODUCER;
			item_key = m_id;
			item_val = m_id;
if (TBB_MAP) {
			map.insert(a, item_key);
			a->second = item_val;
}

		} else {
			type = READER;
			item_key = m_id - NUM_THRDS;
if (TBB_MAP) {
			res = map.find(a, item_key);
}
			if(res)
			{
if (TBB_MAP) {
				item_val = a->second;
}
			} else {
				item_key = INT_MIN;
				item_val = INT_MIN;
			}
		}
				
		end = std::chrono::high_resolution_clock::now();

		auto post_function = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
		auto post_function_epoch = post_function.time_since_epoch();
		
		//printf("Thread %d, item %d: prefunction = %ld, postfunction = %ld\n", id, item_key, pre_function_epoch.count(), post_function_epoch.count());

		//auto response =
		//std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		//printf("Thread %d response: %ld nanoseconds\n", id, response.count());

		long int response = post_function_epoch.count() - start_time_epoch.count();

		//Method m1(m_id, id, item_key, LIFO, type, invocation.count(), response.count(), res);
		Method m1(m_id, id, item_key, item_val, MAP, type, invocation, response, res);
		Method m1_persist(m_id, id, item_key, item_val, MAP, type, invocation, response, res);
			
		m_id = m_id + NUM_THRDS;
		
		thrd_lists[id].push_back(m1);	
		thrd_lists_size[id].fetch_add(1);

		//Persist
		//if(op_dist >= 25) //25% chance that method call won't be persisted
		{
			thrd_lists_persist[id].push_back(m1_persist);
			thrd_lists_size_persist[id].fetch_add(1);
		}
		
		//method_time[id] = method_time[id] + (response.count() - invocation.count());
		method_time[id] = method_time[id] + (response - invocation);
		
#if BUFFERED_LINEARIZABILITY
		if(id%NUM_THRDS == 0 && i%5 == 0)
			sync_time = response;
#endif

	}
	
	done[id].store(true);
}

//void verify_loop(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, std::vector<Item*>& vector_items, std::atomic<int> thrd_lists_size[], std::list<Method> thrd_lists[], int it_count[], std::list<Method>::iterator it[], long int& min, bool& stop)
//void verify_loop(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item* vector_items[], std::atomic<int> thrd_lists_size[], std::list<Method> thrd_lists[], int it_count[], std::list<Method>::iterator it[], long int& min, bool& stop)
//void verify_loop(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::atomic<int> thrd_lists_size[], std::list<Method> thrd_lists[], int it_count[], std::list<Method>::iterator it[], long int& min, bool& stop)
void verify_loop(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::atomic<int>* thrd_lists_size, std::list<Method>* thrd_lists, int* it_count, std::list<Method>::iterator* it, long int& min, bool& stop)
{

	for(int i = 0; i < NUM_THRDS; i++)
	{
		if(done[i].load() == false)
		{
			stop = false;
		}
		
		long int response_time = 0;
		while(it_count[i] < thrd_lists_size[i].load())
		{
			if(it_count[i] == 0)
			{
				it[i] = thrd_lists[i].begin();
			} else {
				++it[i];
			}
			
			Method m = *it[i];

			//if(m.item_key%500 == 0)
				//printf("Checking method %d\n", m.item_key);

			//pq_methods.push(m);

			//Consider doing a left shift by bits to store number of threads, and store thread id in lower bits
			std::map<long int,Method,bool(*)(long int,long int)>::iterator it_method;
			it_method = map_methods.find(m.response);
			while (it_method != map_methods.end())
			{
				m.response = m.response + 1;
				it_method = map_methods.find(m.response);
			}
			response_time = m.response;

			map_methods.insert ( std::pair<long int,Method>(m.response,m) );

#if DEBUG_
			if(m.type == PRODUCER)
				printf("PRODUCE ITEM %d\n", m.item_key);
			else if (m.type == CONSUMER)
				printf("CONSUME ITEM %d\n", m.item_key);
#endif

			//printf("Insert method %d\n", m.id);
			
			it_count[i] = it_count[i] + 1;

			//count_overall = count_overall + 1;

			//Item* item = new Item(m.item_key);
			//item->producer = map_methods.end();
			////map_items.insert(std::pair<int,Item>(m.item_key,item) );
			//if(m.item_key != INT_MIN)
				//vector_items[m.item_key] = item;
			//else
				//vector_items[KEY_RANGE] = item;
			////it_item = map_items.find(m.item_key);
			
			/*if(m.type == PRODUCER)
			{
				//TODO: If Item is ABSENT, set it to PRESENT
				if(it_item->second.status == ABSENT)
				{
					it_item->second.status = PRESENT;
				}
				//(it_item->second).sum = (it_item->second).sum + 1;
				it_item->second.add_int(1);
				
			} else if(m.type == CONSUMER)
			{
				//(it_item->second).sum = (it_item->second).sum - 1;
				it_item->second.sub_int(1);
			}*/
		}	

		if(response_time < min)
		{
			min = response_time;
		}
	}
}

void verify(bool* outcome, bool* outcome_persist, bool* outcome_compare)
{
	wait(NUM_THRDS);
	auto start_time = std::chrono::time_point_cast<std::chrono::nanoseconds>(start);
	auto start_time_epoch = start_time.time_since_epoch();

	std::chrono::time_point<std::chrono::high_resolution_clock> end;

	end = std::chrono::high_resolution_clock::now();

	auto pre_verify = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
	auto pre_verify_epoch = pre_verify.time_since_epoch();

	long int verify_start = pre_verify_epoch.count() - start_time_epoch.count();

	//std::priority_queue<Method,std::vector<Method>,Comparator> pq_methods;
	bool(*fn_pt)(long int,long int) = fncomp;
  	std::map<long int,Method,bool(*)(long int,long int)> map_methods (fn_pt);
	std::map<long int,Method,bool(*)(long int,long int)> map_methods_persist (fn_pt);

	//std::unordered_map<int,Item> map_items;
	//Item* vector_items[KEY_RANGE+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	Item** vector_items = (Item**) calloc((KEY_RANGE+1), sizeof(Item*)); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//std::vector<Item*> vector_items (KEY_RANGE+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//Item* vector_items_persist[KEY_RANGE+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	Item** vector_items_persist = (Item**) calloc((KEY_RANGE+1), sizeof(Item*)); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//std::vector<Item*> vector_items_persist (KEY_RANGE+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)

	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start;
	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start_persist;

	//std::list<Method>::iterator it[NUM_THRDS];
	std::list<Method>::iterator* it = new std::list<Method>::iterator[NUM_THRDS];
	//std::list<Method>::iterator it_persist[NUM_THRDS];
	std::list<Method>::iterator* it_persist = new std::list<Method>::iterator[NUM_THRDS];

	//int it_count[NUM_THRDS];
	int* it_count = new int[NUM_THRDS];
	//int it_count_persist[NUM_THRDS];
	int* it_count_persist = new int[NUM_THRDS];

	bool stop = false;

	long int min; 
	long int min_persist; 

	//long unsigned int count_overall = 0;

	long unsigned int count_iterated = 0;
	long unsigned int count_iterated_persist = 0;

	//min = LONG_MAX;

	//long int old_min;

	while(!stop)
	{
		stop = true;
		min = LONG_MAX;
		min_persist = LONG_MAX;

		verify_loop(map_methods, vector_items, thrd_lists_size, thrd_lists, it_count, it, min, stop);		
		verify_loop(map_methods_persist, vector_items_persist, thrd_lists_size_persist, thrd_lists_persist, it_count_persist, it_persist, min_persist, stop);

		//Decide here whether to use min time or sync() time
#if BUFFERED_LINEARIZABILITY
		if(sync_time < min)
			min = sync_time;
#endif
		
		/*for(int i = 0; i < NUM_THRDS; i++)
		{
			if(done[i].load() == false)
			{
				stop = false;
			}
			
			long int response_time = 0;
			while(it_count[i] < thrd_lists_size[i].load())
			{
				if(it_count[i] == 0)
				{
					it[i] = thrd_lists[i].begin();
				} else {
					++it[i];
				}
				
				Method m = *it[i];

				//if(m.item_key%500 == 0)
					//printf("Checking method %d\n", m.item_key);

				//pq_methods.push(m);

				//Consider doing a left shift by bits to store number of threads, and store thread id in lower bits
				std::map<long int,Method,bool(*)(long int,long int)>::iterator it_method;
				it_method = map_methods.find(m.response);
  				while (it_method != map_methods.end())
				{
					m.response = m.response + 1;
					it_method = map_methods.find(m.response);
				}
				response_time = m.response;

				map_methods.insert ( std::pair<long int,Method>(m.response,m) );

				//printf("Insert method %d\n", m.id);
				
				it_count[i] = it_count[i] + 1;

				//count_overall = count_overall + 1;

				Item* item = new Item(m.item_key);
				item->producer = map_methods.end();
				//map_items.insert(std::pair<int,Item>(m.item_key,item) );
				if(m.item_key != INT_MIN)
					vector_items[m.item_key] = item;
				else
					vector_items[KEY_RANGE] = item;
				//it_item = map_items.find(m.item_key);
				
				//if(m.type == PRODUCER)
				//{
					//TODO: If Item is ABSENT, set it to PRESENT
					//if(it_item->second.status == ABSENT)
					//{
						//it_item->second.status = PRESENT;
					//}
					////(it_item->second).sum = (it_item->second).sum + 1;
					//it_item->second.add_int(1);
					
				//} else if(m.type == CONSUMER)
				//{
					////(it_item->second).sum = (it_item->second).sum - 1;
					//it_item->second.sub_int(1);
				//}
			}	

			if(response_time < min)
			{
				min = response_time;
			}
		}*/
		//printf("min = %ld\n", min);

		printf("VERIFY CHECKPOINT\n");
		verify_checkpoint(map_methods, vector_items, it_start, count_iterated, min, true, outcome);
		printf("VERIFY CHECKPOINT PERSIST\n");
		verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, min, true, outcome_persist); //use min here instead of min_persist

		compare_vectors(vector_items, vector_items_persist, outcome_compare);

		/*if(min == old_min)
		{
			min = LONG_MAX;
		}*/

	}

#if !BUFFERED_LINEARIZABILITY
	printf("VERIFY CHECKPOINT\n");
	verify_checkpoint(map_methods, vector_items, it_start, count_iterated, LONG_MAX, false, outcome);
	printf("VERIFY CHECKPOINT PERSIST\n");
	verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, LONG_MAX, false, outcome_persist);

	compare_vectors(vector_items, vector_items_persist, outcome_compare);
#else
	printf("VERIFY CHECKPOINT\n");
	verify_checkpoint(map_methods, vector_items, it_start, count_iterated, sync_time, false, outcome);
	printf("VERIFY CHECKPOINT PERSIST\n");
	verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, sync_time, false, outcome_persist);

	compare_vectors(vector_items, vector_items_persist, outcome_compare);
#endif

#if DEBUG_
	//printf("Count overall = %lu, count iterated = %lu, map_methods.size() = %lu\n", count_overall, count_iterated, map_methods.size());
#endif

#if DEBUG_
	printf("All threads finished!\n");


	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_;
	for (it_=map_methods.begin(); it_!=map_methods.end(); ++it_)
	{
		std::unordered_map<int,Item>::iterator it_item;
		it_item = map_items.find(it_->second.item_key);
		if(it_->second.type == PRODUCER)
			printf("PRODUCER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
		else if ((it_->second).type == CONSUMER)
			printf("CONSUMER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
		else if ((it_->second).type == READER)
			printf("READER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
	}
#endif

	end = std::chrono::high_resolution_clock::now();

	auto post_verify = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);
	auto post_verify_epoch = post_verify.time_since_epoch();
	long int verify_finish = post_verify_epoch.count() - start_time_epoch.count();

	elapsed_time_verify = verify_finish - verify_start;

	free(vector_items);
	free(vector_items_persist);
}

void print_thread_lists()
{
	FILE *file1 = fopen("Visual/thrd_lists.txt", "w");

	fprintf(file1, "%d %d\n", NUM_THRDS, TEST_SIZE);
	
	for(int i = 0; i < NUM_THRDS; i++)
	{
		std::list<Method>::iterator m_it;
		for(m_it = thrd_lists[i].begin(); m_it != thrd_lists[i].end(); ++m_it)
		{
			fprintf(file1, "%lu %lu ", m_it->invocation, m_it->response);

			std::unordered_map<int,int>::const_iterator got = incorrect_methods.find (m_it->item_key);

			if(got == incorrect_methods.end() )
			{
				if(m_it->semantics == FIFO)
				{

					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "enq(%d) ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						fprintf(file1, "deq(%d) ", m_it->item_key);
					}
				} else if (m_it->semantics == LIFO)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "push(%d) ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "pop(%d) ", m_it->item_key);
						else
							fprintf(file1, "pop(NULL) ");
					}
				} //else if (m_it->semantics == MAP)
				//{
				//}
			} else {
				if(m_it->semantics == FIFO)
				{

					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "enq(%d):ERROR ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						fprintf(file1, "deq(%d):ERROR ", m_it->item_key);
					}
				} else if (m_it->semantics == LIFO)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "push(%d):ERROR ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "pop(%d):ERROR ", m_it->item_key);
						else
							fprintf(file1, "pop(NULL) ");
					}
				} //else if (m_it->semantics == MAP)
				//{
				//}
			}
			
		}	
		fprintf(file1, "\n");
	}
	fclose(file1);
}

void vsv_init()
{
	//pfile = fopen("output.txt", "a");

	final_outcome = true;
	final_outcome_persist = true;
	final_outcome_compare = true;

	method_count = 0;

#if BUFFERED_LINEARIZABILITY
	sync_time = 0;
#endif

	//Dynamically Allocate Arrays
	thrd_lists = new std::list<Method>[NUM_THRDS];
	thrd_lists_persist = new std::list<Method>[NUM_THRDS];
	thrd_lists_size = new std::atomic<int>[NUM_THRDS];
	thrd_lists_size_persist = new std::atomic<int>[NUM_THRDS];
	done = new std::atomic<bool>[NUM_THRDS];
	method_time = new long int[NUM_THRDS];
	t = new std::thread[NUM_THRDS];

	start = std::chrono::high_resolution_clock::now();

}

void vsv_startup()
{
	v = std::thread(verify, &final_outcome, &final_outcome_persist, &final_outcome_compare);
}

void vsv_shutdown()
{
	v.join();

	if(final_outcome == true)
	{
		printf("-------------Program Correct Up To This Point-------------\n");
	} else {
		printf("-------------Program Not Correct-------------\n");
	}

	if(final_outcome_persist == true)
	{
		printf("-------------Persisted Program Correct Up To This Point-------------\n");
	} else {
		printf("-------------Persisted Program Not Correct-------------\n");
	}

	if(final_outcome_compare == true)
	{
		printf("VECTOR SUMS EQUAL\n");
	} else {
		printf("VECTOR SUMS NOT EQUAL\n");
	}
			
	auto finish = std::chrono::high_resolution_clock::now();

	//Elapsed time
	auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
	double elapsed_time_double = elapsed_time.count()*0.000000001;
	printf("Total Time: %.15lf seconds\n", elapsed_time_double);
	
	//Elapsed method time
	long int elapsed_time_method = 0;
	for(int i = 0; i < NUM_THRDS; i++)
	{
		if(method_time[i] > elapsed_time_method)
			elapsed_time_method = method_time[i];
	}	
	double elapsed_time_method_double = elapsed_time_method*0.000000001;
	printf("Total Method Time: %.15lf seconds\n", elapsed_time_method_double);

	//Elapsed verification time
	double elapsed_time_verify_double = elapsed_time_verify*0.000000001;
	elapsed_time_verify_double = elapsed_time_verify_double - elapsed_time_method_double;	
	printf("Total Verification Time: %.15lf seconds\n", elapsed_time_verify_double);
	
	//fprintf(pfile, "%.15lf %.15lf\n", elapsed_time_method_double, elapsed_time_verify_double);
	//fclose(pfile);
}

int main(int argc,char* argv[]) 
{ 
	TBB_QUEUE = 0;
	BOOST_STACK = 0;
	TBB_MAP = 0;

	NUM_THRDS = 8;
	
	if( argc == 2 ) {
		printf("Test size = %d\n", atoi(argv[1]));
		TEST_SIZE = (unsigned int) atoi(argv[1]);
		TBB_QUEUE = 1;
		printf("Testing TBB_QUEUE\n");
#if LINEARIZABILITY
		TRANS_SIZE = 1;
#else
		TRANS_SIZE = 2;
#endif
	} else if (argc == 3) {
		printf("Test size = %d\n", atoi(argv[1]));
		TEST_SIZE = (unsigned int) atoi(argv[1]);
#if LINEARIZABILITY
		TRANS_SIZE = 1;
#else
		TRANS_SIZE = 2;
#endif
		if(atoi(argv[2]) == 0)
		{
			TBB_QUEUE = 1;
			printf("Testing TBB_QUEUE\n");
		} else if(atoi(argv[2]) == 1)
		{
			BOOST_STACK = 1;
			printf("Testing BOOST_STACK\n");
		} else if(atoi(argv[2]) == 2)
		{
			TBB_MAP = 1;
			printf("Testing TBB_MAP\n");
		} else { //default
			printf("Test size = 10\n");
			TEST_SIZE = 10;
			TBB_QUEUE = 1;
			printf("Testing TBB_QUEUE\n");
		}
		
	} else { //default
		printf("Test size = 10\n");
		TEST_SIZE = 10;
#if LINEARIZABILITY
		TRANS_SIZE = 1;
#else
		TRANS_SIZE = 2;
#endif
		TBB_QUEUE = 1;
		printf("Testing TBB_QUEUE\n");
	}

	KEY_RANGE = TEST_SIZE*NUM_THRDS*TRANS_SIZE;

	vsv_init();

	for(int i = 0; i < NUM_THRDS; i++)
	{
		if(TBB_QUEUE)
		{
			t[i] = std::thread(work_queue,i);
		} else if(BOOST_STACK)
		{
			t[i] = std::thread(work_stack,i);
		} else if(TBB_MAP)
		{
			t[i] = std::thread(work_map,i);
		} 
		//t[i] = std::thread(test_map,i);
	}
	
	vsv_startup();
	
	for(int i = 0; i < NUM_THRDS; i++)
	{
		t[i].join();
	}
	
	vsv_shutdown();
	
if (TBB_QUEUE) {	
	printf("Final Queue Configuration:\n");
	typedef tbb::concurrent_queue<int>::iterator iter;
    for(iter i(queue.unsafe_begin()); i!=queue.unsafe_end(); i++)
        printf("%d ", *i);
    printf("\n");
} else if (BOOST_STACK) {
	printf("Final Stack Configuration:\n");
	int stack_val;
	while(stack.pop(stack_val))
	{
		printf("%d ", stack_val);
	}
	printf("\n");
} else if (TBB_MAP) {
	printf("Final Map Configuration:\n");
	tbb::concurrent_hash_map<int,int,MyHashCompare>::iterator it;
	for( it=map.begin(); it!=map.end(); ++it )
    	printf("%d,%d ",it->first,it->second);
	printf("\n");
}

	print_thread_lists();

    return 0; 
} 
