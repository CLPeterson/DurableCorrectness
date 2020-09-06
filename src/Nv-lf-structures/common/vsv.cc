#include <list>

#include "vsv.h"

#include <string>
#include <chrono>
#include <stdio.h>
#include <thread>
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

//Program Input
unsigned int NUM_THRDS;
unsigned int TEST_SIZE;
unsigned int TRANS_SIZE;
unsigned int KEY_RANGE_; //Replace TEST_SIZE*NUM_THRDS*TRANS_SIZE
Semantics SEMANTICS;

struct MyHashCompare {
    //static size_t hash( int x ) { {
	//	return x;
	//}
	static size_t hash(std::thread::id x ) {
		std::ostringstream ss;
		ss << x;
		std::string idstr = ss.str();
        size_t h = 0;
        for( const char* s = idstr.c_str(); *s; ++s )
            h = (h*17)^*s;
        return h;
    }
    //! True if strings are equal
    //static bool equal( int x, int y ) {
	static bool equal( std::thread::id x, std::thread::id y ) {
        return x==y;
    }
};

typedef enum ItemStatus
{
	PRESENT,
	ABSENT
}ItemStatus; 


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

//Utility Variables
FILE *pfile;

std::thread* t;
std::thread v;

std::atomic<int>* thrd_lists_size;
std::atomic<int>* thrd_lists_size_persist;
std::atomic<bool>* done;
//std::atomic<int> barrier;

std::unordered_map<int,int> incorrect_methods;

std::unordered_map<void*,Method>* persist_map;

std::chrono::time_point<std::chrono::high_resolution_clock> start;
long int* method_time;
int* method_id;
int* method_id_persist;
long int elapsed_time_verify;

unsigned int method_count;

bool final_outcome;
bool final_outcome_persist;
bool final_outcome_compare;

std::list<Method>* thrd_lists;
std::list<Method>* thrd_lists_persist;

std::list<Method>::iterator* thrd_lists_itr;
std::list<Method>::iterator* thrd_lists_itr_persist;

//std::unordered_map<std::thread::id,int>* thread_map;
//std::mutex process_map_lock;
tbb::concurrent_hash_map<std::thread::id,int,MyHashCompare> thread_map;
std::atomic<int> thread_ctr(0);


#if BUFFERED_LINEARIZABILITY
long int sync_time;
#endif

bool fncomp (long int lhs, long int rhs) {return lhs < rhs;}

/*void wait(unsigned int num_thrds)
{
	barrier.fetch_add(1);
	while(barrier.load() < num_thrds) { }
}*/

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
				vec_item_0 = vector_items[KEY_RANGE_];
			
			if(it_0->second.type == PRODUCER && vec_item->status == PRESENT && (it_0->second.semantics == FIFO || it_0->second.semantics == LIFO || it->second.item_key == it_0->second.item_key))
			{
				stack_failed.push(vec_item_0);
			}
		}
	}
}

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
				vec_item_0 = vector_items[KEY_RANGE_];
			
			if(it_0->second.type == PRODUCER && vec_item->status == PRESENT && it->second.item_key == it_0->second.item_key)
			{
				stack_failed.push(vec_item_0);
			}
		}
	}
}

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

			//printf("it->second.item_key = %d, KEY_RANGE_ = %d, INT_MIN = %d\n", it->second.item_key, KEY_RANGE_, INT_MIN);
			if(it->second.item_key != INT_MIN && vector_items[it->second.item_key] == NULL)
			{
				Item* item = new Item(it->second.item_key);
				if(item == NULL) printf("Item = NULL\n");
				item->producer = map_methods.end();
				vector_items[it->second.item_key] = item;
			} else if(it->second.item_key == INT_MIN && vector_items[KEY_RANGE_] == NULL)
			{
				Item* item = new Item(it->second.item_key);
				if(item == NULL) printf("Item = NULL\n");
				item->producer = map_methods.end();
				vector_items[KEY_RANGE_] = item;
			}
		
			Item* vec_item = NULL;
			if(it->second.item_key != INT_MIN) {
				vec_item = vector_items[it->second.item_key];
			} else {	
				vec_item = vector_items[KEY_RANGE_];
				//vec_item->producer = map_methods.end();
			}

			if(it->second.type == PRODUCER)
			{
				if(it->second.status == true)
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
										vec_item_0 = vector_items[KEY_RANGE_];
								
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
				else {
					//handle_failed_producer(); //Not implemented yet
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
									vec_method_item = vector_items[KEY_RANGE_];
								

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
#if DEBUG_
				printf("item_promote = %d, KEY_RANGE_ = %d, INT_MIN = %d\n", item_promote, KEY_RANGE_, INT_MIN);
#endif
				if(item_promote != INT_MIN)
					vec_promote_item = vector_items[item_promote];
				else
					vec_promote_item = vector_items[KEY_RANGE_];
				vec_promote_item->promote();
#if DEBUG_
				printf("Item %d promotes Item %d\n", it_top->key, item_promote);
#endif
			}

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

		//TODO: insert methods_top in a map and check for duplicates
	
		//printf("Handling stack_finished_method\n");
		while(!stack_finished_methods.empty())
		{
			std::map<long int,Method,bool(*)(long int,long int)>::iterator methods_top = stack_finished_methods.top();
			
			//if(methods_top->second.type == PRODUCER)
				//printf("Erase PRODUCER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
			//else if (methods_top->second.type == CONSUMER)
				//printf("Erase CONSUMER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
				
			if(methods_top->second.item_key != it_start->second.item_key) 
			{

				std::map<long int,Method,bool(*)(long int,long int)>::iterator it_method;
				it_method = map_methods.find(methods_top->second.response);
				if (it_method == map_methods.end())
				{
#if DEBUG_
					if(methods_top->second.type == PRODUCER)
						printf("PRODUCER key %d, response = %ld not found in method map\n", methods_top->second.item_key, methods_top->second.response);
					else if (methods_top->second.type == CONSUMER)
						printf("CONSUMER key %d, response = %ld not found in method map\n", methods_top->second.item_key, methods_top->second.response);
#endif
				} else {
					/*if(methods_top->second.type == PRODUCER)
						printf("PRODUCER key %d, response = %ld found in method map\n", methods_top->second.item_key, methods_top->second.response);
					else if (methods_top->second.type == CONSUMER)
						printf("CONSUMER key %d, response = %ld found in method map\n", methods_top->second.item_key, methods_top->second.response);*/
				}

				//if(methods_top != map_methods.end() && methods_top != map_methods.begin())
				if (it_method != map_methods.end())
				{
#if DEBUG_
					if(methods_top->second.type == PRODUCER)
						printf("Erase PRODUCER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
					else if (methods_top->second.type == CONSUMER)
						printf("Erase CONSUMER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
#endif

					map_methods.erase(methods_top); //TODO: Dangerous, make sure this is correct, fails with the persistent verification	
					//printf("Erase return\n");	
				}
			}
			stack_finished_methods.pop();
		}
		//printf("Finished handling stack_finished_method\n");
		
		//Verify Sums 
		bool outcome = true; //ADD to VectorSpace

		//if(is_quiescent) //ADD to VectorSpace
		//{
			Item* vec_verify;
			for (int i = 0; i < KEY_RANGE_+1; i++)
			{
				vec_verify = vector_items[i];

				if(vec_verify == NULL) continue;

				if(vec_verify->sum < 0)
				{
					outcome = false;
	//#if DEBUG_
					//printf("WARNING: Item %d, sum %.2lf\n", it_verify->second.key, it_verify->second.sum);
					//printf("WARNING: Item %d, sum %.2lf\n", vec_verify->key, vec_verify->sum);
					std::pair<int,int> mypair (vec_verify->key,vec_verify->key);
					incorrect_methods.insert(mypair);
					/*printf("Demote list:\n");
					std::list<Method>::iterator it_m;
					for(it_m = vec_verify->demote_methods.begin(); it_m != vec_verify->demote_methods.end(); ++it_m)
					{
						printf("%d, Promote List: ", it_m->item_key);
						Item* vec_promote_item;
						if(it_m->item_key != INT_MIN)
							vec_promote_item = vector_items[it_m->item_key];
						else
							vec_promote_item = vector_items[KEY_RANGE_];

						std::list<int>::iterator it_item1;
						for(it_item1 = vec_promote_item->promote_items.begin(); it_item1 != vec_promote_item->promote_items.end(); ++it_item1)
						{
							printf("%d ", *it_item1);
						}
						printf("\n");
					}
					printf("\n");	*/	

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
	for (int i = 0; i < KEY_RANGE_+1; i++)
	{
		if(vector_items1[i] != NULL && vector_items2[i] == NULL)
		{
			//printf("vector_items1[%d]->sum = %.2lf\n", i, vector_items1[i]->sum);
			if(vector_items1[i]->sum != 0)
				*outcome_compare = false;
		} else if(vector_items1[i] == NULL && vector_items2[i] != NULL)
		{
			//printf("vector_items2[%d]->sum = %.2lf\n", i, vector_items2[i]->sum);
			if(vector_items2[i]->sum != 0)
				*outcome_compare = false;	
		} else if(vector_items1[i] != NULL && vector_items2[i] != NULL)
		{
			//printf("vector_items1[%d]->sum = %.2lf, vector_items2[%d]->sum = %.2lf\n", i, vector_items1[i]->sum, i, vector_items2[i]->sum);
			if(vector_items1[i]->sum != vector_items2[i]->sum)	
				*outcome_compare = false;
		}
	}
}

void verify_loop(std::map<long int,Method,bool(*)(long int,long int)>& map_methods, Item** vector_items, std::atomic<int>* _thrd_lists_size, std::list<Method>* thrd_lists, int* it_count, std::list<Method>::iterator* it, long int& min, bool& stop)
{
	for(int i = 0; i < NUM_THRDS; i++)
	{
		if(done[i].load() == false)
		{
			stop = false;
		}
		
		long int response_time = 0;
		int _thrd_size = _thrd_lists_size[i].load();
		while(it_count[i] < _thrd_size)
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
				printf("Thread %d: PRODUCE ITEM %d, _thrd_lists_size[%d] = %d\n", i, m.item_key, i, _thrd_size);
			else if (m.type == CONSUMER)
				printf("Thread %d: CONSUME ITEM %d, _thrd_lists_size[%d] = %d\n", i, m.item_key, i, _thrd_size);
			else if (m.type == READER)
				printf("Thread %d: READ ITEM %d, _thrd_lists_size[%d] = %d\n", i, m.item_key, i, _thrd_size);
			else if (m.type == WRITER)
				printf("Thread %d: WRITE ITEM %d, _thrd_lists_size[%d] = %d\n", i, m.item_key, i, _thrd_size);
			else printf("Thread %d: STRAY ITEM %d, _thrd_lists_size[%d] = %d\n", i, m.item_key, i, _thrd_size);
#endif

			//printf("Insert method %d\n", m.id);
			
			it_count[i] = it_count[i] + 1;

			//Item* item = new Item(m.item_key);
			//item->producer = map_methods.end();
			////map_items.insert(std::pair<int,Item>(m.item_key,item) );
			//if(m.item_key != INT_MIN)
				//vector_items[m.item_key] = item;
			//else
				//vector_items[KEY_RANGE_] = item;
			////it_item = map_items.find(m.item_key);

			_thrd_size = _thrd_lists_size[i].load();
			
		}	

		if(response_time < min)
		{
			min = response_time;
		}
	}
}

void verify(bool* outcome, bool* outcome_persist, bool* outcome_compare)
{
	printf("Begin Verification\n");
	//wait(NUM_THRDS);
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
	//Item* vector_items[KEY_RANGE_+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	Item** vector_items = (Item**) calloc((KEY_RANGE_+1), sizeof(Item*)); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//std::vector<Item*> vector_items (KEY_RANGE_+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//Item* vector_items_persist[KEY_RANGE_+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	Item** vector_items_persist = (Item**) calloc((KEY_RANGE_+1), sizeof(Item*)); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
	//std::vector<Item*> vector_items_persist (KEY_RANGE_+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)

	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start = map_methods.end(); //ADDED RECENTLY
	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start_persist = map_methods_persist.end(); //ADDED RECENTLY

	std::list<Method>::iterator* it = new std::list<Method>::iterator[NUM_THRDS];
	std::list<Method>::iterator* it_persist = new std::list<Method>::iterator[NUM_THRDS];

	//int* it_count = new int[NUM_THRDS];
	int* it_count = (int*) calloc (NUM_THRDS, sizeof(int));
	//int* it_count_persist = new int[NUM_THRDS];
	int* it_count_persist = (int*) calloc (NUM_THRDS, sizeof(int));

	bool stop = false;

	long int min; 
	long int min_persist; 

	long unsigned int count_iterated = 0;
	long unsigned int count_iterated_persist = 0;

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

		//printf("VERIFY CHECKPOINT\n");
		verify_checkpoint(map_methods, vector_items, it_start, count_iterated, min, true, outcome);
		//printf("VERIFY CHECKPOINT PERSIST\n");
		verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, min, true, outcome_persist); //use min here instead of min_persist

		//compare_vectors(vector_items, vector_items_persist, outcome_compare);

	}

#if !BUFFERED_LINEARIZABILITY
	//printf("VERIFY CHECKPOINT\n");
	verify_checkpoint(map_methods, vector_items, it_start, count_iterated, LONG_MAX, false, outcome);
	//printf("VERIFY CHECKPOINT PERSIST\n");
	verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, LONG_MAX, false, outcome_persist);

	compare_vectors(vector_items, vector_items_persist, outcome_compare);
#else
	//printf("VERIFY CHECKPOINT\n");
	verify_checkpoint(map_methods, vector_items, it_start, count_iterated, sync_time, false, outcome);
	//printf("VERIFY CHECKPOINT PERSIST\n");
	verify_checkpoint(map_methods_persist, vector_items_persist, it_start_persist, count_iterated_persist, sync_time, false, outcome_persist);

	compare_vectors(vector_items, vector_items_persist, outcome_compare);
#endif


#if DEBUG_
	printf("All threads finished!\n");


	/*std::map<long int,Method,bool(*)(long int,long int)>::iterator it_;
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
	}*/
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
	FILE *file1 = fopen("thrd_lists.txt", "w");

	fprintf(file1, "%d\n", NUM_THRDS);
	
	for(int i = 0; i < NUM_THRDS; i++)
	{
		fprintf(file1, "%lu ", thrd_lists[i].size());
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
				} else if (m_it->semantics == SET)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "ins(%d) ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "rem(%d) ", m_it->item_key);
						else
							fprintf(file1, "rem(NULL) ");
					}
				}

				//else if (m_it->semantics == MAP)
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
				} else if (m_it->semantics == SET)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "ins(%d):ERROR ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "rem(%d):ERROR ", m_it->item_key);
						else
							fprintf(file1, "rem(NULL) ");
					}
				}
				//else if (m_it->semantics == MAP)
				//{
				//}
			}
			
		}	
		fprintf(file1, "\n");
	}
	fclose(file1);
}

void print_thread_lists_persist()
{
	FILE *file1 = fopen("thrd_lists_persist.txt", "w");

	fprintf(file1, "%d\n", NUM_THRDS);
	
	for(int i = 0; i < NUM_THRDS; i++)
	{
		fprintf(file1, "%lu ", thrd_lists_persist[i].size());
		std::list<Method>::iterator m_it;
		for(m_it = thrd_lists_persist[i].begin(); m_it != thrd_lists_persist[i].end(); ++m_it)
		{
			fprintf(file1, "%lu %lu ", m_it->invocation, m_it->response);

			std::unordered_map<int,int>::const_iterator got = incorrect_methods.find (m_it->item_key); //FIX THIS

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
				} else if (m_it->semantics == SET)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "ins(%d) ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "rem(%d) ", m_it->item_key);
						else
							fprintf(file1, "rem(NULL) ");
					}
				}

				//else if (m_it->semantics == MAP)
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
				} else if (m_it->semantics == SET)
				{
					if(m_it->type == PRODUCER)
					{
						fprintf(file1, "ins(%d):ERROR ", m_it->item_key);
					} else if(m_it->type == CONSUMER)
					{
						if(m_it->item_key != INT_MIN)
							fprintf(file1, "rem(%d):ERROR ", m_it->item_key);
						else
							fprintf(file1, "rem(NULL) ");
					}
				}
				//else if (m_it->semantics == MAP)
				//{
				//}
			}
			
		}	
		fprintf(file1, "\n");
	}
	fclose(file1);
}


int get_process_id()
{
	std::thread::id this_id = std::this_thread::get_id();

	//printf("Thread::get_id() = %p\n", this_id);

	//std::unordered_map<std::thread::id,int>::iterator got = thread_map->find (this_id);
	tbb::concurrent_hash_map<std::thread::id,int,MyHashCompare>::accessor a;
	
	int _process;
	//if ( got == thread_map->end() )
	if(!thread_map.find(a, this_id))
	{
		_process = thread_ctr.fetch_add(1);

		//std::pair<std::thread::id,int> pair (this_id,_process);
		//process_map_lock.lock();
		//thread_mapinsert (pair);
		//process_map_lock.unlock();
		//printf("%d\n", _process);

		thread_map.insert(a, this_id);
		a->second = _process; //It's okay to set process in separate atomic step as insert since other threads will not access this field
	} else {
		//_process = got->second;
		_process = a->second;
	}
	return _process;
}

int get_method_id()
{
	int _process = get_process_id();
	if(method_id[_process] == 0)
	{
		method_id[_process] = _process + 1;
	} else {
		method_id[_process] = method_id[_process] + NUM_THRDS;
	}
	return method_id[_process];
}

int get_method_id_persist()
{
	int _process = get_process_id();
	if(method_id_persist[_process] == 0)
	{
		method_id_persist[_process] = _process + 1;
	} else {
		method_id_persist[_process] = method_id_persist[_process] + NUM_THRDS;
	}
	return method_id_persist[_process];
}

void vsv_init()
{
	printf("VSV_Init()\n");
	pfile = fopen("output.txt", "a");

	//barrier.store(0);

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
	thrd_lists_size = (std::atomic<int>*) calloc (NUM_THRDS, sizeof(std::atomic<int>));
	thrd_lists_size_persist = (std::atomic<int>*) calloc (NUM_THRDS, sizeof(std::atomic<int>));
	thrd_lists_itr = new std::list<Method>::iterator[NUM_THRDS];
	thrd_lists_itr_persist = new std::list<Method>::iterator[NUM_THRDS];
	for(int i = 0; i < NUM_THRDS; i++)
	{
		thrd_lists_itr[i] = thrd_lists[i].end(); //ADDED RECENTLY
		thrd_lists_itr_persist[i] = thrd_lists_persist[i].end(); //ADDED RECENTLY
	}
	done = (std::atomic<bool>*) calloc (NUM_THRDS, sizeof(std::atomic<bool>));
	method_time = (long int*) calloc (NUM_THRDS, sizeof(long int));
	method_id = (int*) calloc (NUM_THRDS, sizeof(int));
	method_id_persist = (int*) calloc (NUM_THRDS, sizeof(int));
	t = new std::thread[NUM_THRDS];
	persist_map = new std::unordered_map<void*,Method>[NUM_THRDS];

	//start = std::chrono::high_resolution_clock::now();

	//thread_map = new std::unordered_map<std::thread::id,int>();
	printf("VSV_Init() Finished\n");

}

void vsv_startup()
{
	start = std::chrono::high_resolution_clock::now();
	v = std::thread(verify, &final_outcome, &final_outcome_persist, &final_outcome_compare);
}

void vsv_shutdown()
{
	vsv_exit();
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
	//elapsed_time_verify_double = elapsed_time_verify_double - elapsed_time_method_double;	
	printf("Total Verification Time: %.15lf seconds\n", elapsed_time_verify_double);
	
	//fprintf(pfile, "%.15lf %.15lf\n", elapsed_time_method_double, elapsed_time_verify_double);
	fprintf(pfile, "%.15lf\n", elapsed_time_double);
	fclose(pfile);
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

void update_method_time(long int _invocation, long int _response)
{
	int _process = get_process_id();
	method_time[_process] = method_time[_process] + (_response - _invocation);
}

//TODO: FIXE ME!!!! Major Bug here, method will be inserted in thrd_lists twice when insert_txn is called
void create_method(int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status)
{
	int _process = get_process_id();
	int m_id = get_method_id();
	//int m_id = _process + 1 + (_iteration * TRANS_SIZE + _operation) * NUM_THRDS;
	bool empty = thrd_lists[_process].empty();
	//bool empty_persist = thrd_lists_persist[_process].empty();
	//Method* m1 = new Method(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	Method m1(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	/*if(m1.type == PRODUCER)
		printf("Thread %d: new Method PRODUCE ITEM %d\n", _process, m1.item_key);
	else if (m1.type == CONSUMER)
		printf("Thread %d: new Method CONSUME ITEM %d\n", _process, m1.item_key);
	else if (m1.type == READER)
		printf("Thread %d: new Method READ ITEM %d\n", _process, m1.item_key);
	else if (m1.type == WRITER)
		printf("Thread %d: new Method WRITE ITEM %d\n", _process, m1.item_key);
	else printf("Thread %d: new Method STRAY ITEM %d\n", _process, m1.item_key);*/
	//Method* m1_persist = new Method(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	//Method m1_persist(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	//thrd_lists[_process].push_back(*m1);
	thrd_lists[_process].push_back(m1);
	//thrd_lists_persist[_process].push_back(*m1_persist);
	//thrd_lists_persist[_process].push_back(m1_persist);
	if(empty) thrd_lists_itr[_process] = thrd_lists[_process].begin();
	//if(empty_persist) thrd_lists_itr_persist[_process] = thrd_lists_persist[_process].begin();
	update_method_time(_invocation, _response);

}

void create_method_persist(int _item_key, int _item_val, Semantics _semantics, Type _type, long int _invocation, long int _response, bool _status)
{
	int _process = get_process_id();
	int m_id = get_method_id_persist();	
	bool empty_persist = thrd_lists_persist[_process].empty();
	Method m1_persist(m_id, _process, _item_key, _item_val, _semantics, _type, _invocation, _response, _status);
	/*if(m1_persist.type == PRODUCER)
		printf("Thread %d: new Method PRODUCE ITEM %d\n", _process, m1_persist.item_key);
	else if (m1_persist.type == CONSUMER)
		printf("Thread %d: new Method CONSUME ITEM %d\n", _process, m1_persist.item_key);
	else if (m1_persist.type == READER)
		printf("Thread %d: new Method READ ITEM %d\n", _process, m1_persist.item_key);
	else if (m1_persist.type == WRITER)
		printf("Thread %d: new Method WRITE ITEM %d\n", _process, m1_persist.item_key);
	else printf("Thread %d: new Method STRAY ITEM %d\n", _process, m1_persist.item_key);*/
	thrd_lists_persist[_process].push_back(m1_persist);
	if(empty_persist) thrd_lists_itr_persist[_process] = thrd_lists_persist[_process].begin();

}

void insert_persist_map(void* ptr, int _item_key, int _item_val, Semantics _semantics, Type _type)
{
	int _process = get_process_id();

	std::unordered_map<void*,Method>::iterator got = persist_map[_process].find (ptr);
	
	if ( got == persist_map[_process].end() )
	{
		Method m1_persist(-1, _process, _item_key, _item_val, _semantics, _type, 0, 0, true);
		std::pair<void*,Method> pair (ptr,m1_persist);
		persist_map[_process].insert(pair);
	} else {
		
		if((got->second.item_key == _item_key) && (got->second.type == PRODUCER && _type == CONSUMER))
		{
			//Producer followed by Consumer eliminate each other
			persist_map[_process].erase(got);
		} else if ((got->second.item_key == _item_key) && (got->second.type == CONSUMER && _type == PRODUCER))
		{
			//Item exists in the persisted state
			persist_map[_process].erase(got);
			Method m1_persist(-1, _process, _item_key, _item_val, _semantics, _type, 0, 0, true);
			std::pair<void*,Method> pair (ptr,m1_persist);
			persist_map[_process].insert(pair);
		}
		/*else if((got->second.item_key == _item_key) && (got->second.type == PRODUCER && _type == WRITER)) {
			//Writer overwrite Producer
			persist_map[_process].erase(got);
			Method m1_persist(-1, _process, _item_key, _item_val, _semantics, _type, 0, 0, true);
			std::pair<void*,Method> pair (ptr,m1_persist);
			persist_map[_process].insert(pair);
		}*/
	}
}

void handle_PWB(void* ptr, long int _invocation, long int _response)
{
	int _process = get_process_id();

	std::unordered_map<void*,Method>::iterator got = persist_map[_process].find (ptr);

	if ( got != persist_map[_process].end() )
	{
		/*if(got->second.type == PRODUCER)
			printf("Thread %d: handle_PWB PRODUCE ITEM %d\n", _process, got->second.item_key);
		else if (got->second.type == CONSUMER)
			printf("Thread %d: handle_PWB CONSUME ITEM %d\n", _process, got->second.item_key);*/

		create_method_persist(got->second.item_key, got->second.item_val, got->second.semantics, got->second.type, _invocation, _response, true);
		persist_map[_process].erase(got);
	} else {
		//printf("Thread %d: handle_PWB did not find Node %p\n", _process, ptr);
	}
}
//TODO: FIXE ME!!!! Major Bug here, method will be inserted in thrd_lists twice when insert_txn is called
#if STRICT_SERIALIZABILITY
void insert_txn(long int _txn_invocation, long int _txn_response, int size)
{
	int _process = get_process_id();

	//printf("Thread %d: Attempting to Insert_txn\n", _process);

	if(thrd_lists[_process].size() == 0) return; //ADDED RECENTLY

	std::list<Method>::iterator m1_it = thrd_lists_itr[_process];
	if(m1_it == thrd_lists[_process].end()) return; //ADDED RECENTLY
	//std::list<Method>::iterator m1_persist_it = thrd_lists_itr_persist[_process];

	std::list<Method>::iterator m1_it_prev;

	/*std::list<Method>::iterator m1_it_begin;

	m1_it_begin = thrd_lists[_process].begin();

	if(m1_it_begin->type == PRODUCER)
		printf("Thread %d: m1_it_begin set to PRODUCER ITEM %d\n", _process, m1_it_begin->item_key);
	else if(m1_it->type == CONSUMER)
		printf("Thread %d: m1_it_begin set to CONSUMER ITEM %d\n", _process, m1_it_begin->item_key);
	else if(m1_it->type == READER)
		printf("Thread %d: m1_it_begin set to READER ITEM %d\n", _process, m1_it_begin->item_key);
	else if(m1_it->type == WRITER)
		printf("Thread %d: m1_it_begin set to WRITER ITEM %d\n", _process, m1_it_begin->item_key);
	else
		printf("Thread %d: m1_it_begin set to STRAY ITEM %d\n", _process, m1_it_begin->item_key);*/

	//if(m1_it != thrd_lists[_process].begin()) ++m1_it;
	if(thrd_lists_size[_process].load() != 0) ++m1_it;
	if(m1_it == thrd_lists[_process].end()) return; //ADDED RECENTLY
	//if(m1_persist_it != thrd_lists_persist[_process].begin()) ++m1_persist_it;
	//if(thrd_lists_size_persist[_process].load() != 0) ++m1_persist_it;

	if(m1_it == thrd_lists[_process].end()) { 
		//printf("Thread %d: Iterator fail 1\n", _process); 
	} else {
		/*if(m1_it->type == PRODUCER)
			printf("Thread %d: m1_it 1 advanced to PRODUCER ITEM %d\n", _process, m1_it->item_key);
		else if(m1_it->type == CONSUMER)
			printf("Thread %d: m1_it 1 advanced to CONSUMER ITEM %d\n", _process, m1_it->item_key);
		else if(m1_it->type == READER)
			printf("Thread %d: m1_it 1 advanced to READER ITEM %d\n", _process, m1_it->item_key);
		else if(m1_it->type == CONSUMER)
			printf("Thread %d: m1_it 1 advanced to WRITER ITEM %d\n", _process, m1_it->item_key);
		else
			printf("Thread %d: m1_it 1 advanced to STRAY ITEM %d\n", _process, m1_it->item_key);*/
	}

	int txn_id = m1_it->id;
	//int txn_id_persist = m1_persist_it->id;
	//for(unsigned int j = 0; j < TRANS_SIZE; j++)
	
	int running_size = 0;

	for(unsigned int j = 0; j < size; j++)
	{
		m1_it->txn_invocation = _txn_invocation;
		m1_it->txn_response = _txn_response;
		m1_it->txn_id = txn_id;
		//thrd_lists[_process].push_back(*m1_it); //REMOVE LATER
	
		//m1_persist_it->txn_invocation = _txn_invocation; 
		//m1_persist_it->txn_response = _txn_response; 
		//m1_persist_it->txn_id = txn_id_persist; 	
		//thrd_lists_persist[_process].push_back(*m1_persist_it); //REMOVE LATER

		running_size = running_size + 1;

		//printf("Thread %d: txn_id = %d, method_id = %d, txn_inv = %ld, txn_res = %ld \n", id, txn_id, (*it_list).id, txn_invocation, txn_response);
		//if(j < (TRANS_SIZE - 1)) 
		m1_it_prev = m1_it;
		if(j < (size - 1))
		{
			++m1_it;
			if(m1_it == thrd_lists[_process].end()) { 
				//printf("Thread %d: Iterator fail 2\n", _process); 
				m1_it = m1_it_prev; break; 
			}
			else {
				/*if(m1_it->type == PRODUCER)
					printf("Thread %d: m1_it 2 advanced to PRODUCER ITEM %d\n", _process, m1_it->item_key);
				else if(m1_it->type == CONSUMER)
					printf("Thread %d: m1_it 2 advanced to CONSUMER ITEM %d\n", _process, m1_it->item_key);
				else if(m1_it->type == READER)
					printf("Thread %d: m1_it 2 advanced to READER ITEM %d\n", _process, m1_it->item_key);
				else if(m1_it->type == WRITER)
					printf("Thread %d: m1_it 2 advanced to WRITER ITEM %d\n", _process, m1_it->item_key);
				else
					printf("Thread %d: m1_it 2 advanced to STRAY ITEM %d\n", _process, m1_it->item_key);*/
			}	
		}
		
	}
	thrd_lists_itr[_process] = m1_it;
	//thrd_lists_itr_persist[_process] = m1_persist_it;

	//printf("Thread %d: size = %d, running_size = %d\n", _process, thrd_lists_size[_process].load(), running_size);

	//thrd_lists_size[_process].fetch_add(TRANS_SIZE);
	//thrd_lists_size[_process].fetch_add(size);
	thrd_lists_size[_process].fetch_add(running_size);
	//thrd_lists_size_persist[_process].fetch_add(TRANS_SIZE);
	//thrd_lists_size_persist[_process].fetch_add(size);
	//thrd_lists_size_persist[_process].fetch_add(running_size);


	//if(thrd_lists_size[_process].load() >= (TRANS_SIZE*TEST_SIZE)) printf("thread_list_size[%d] = %d\n", _process, thrd_lists_size[_process].load());
	//if(thrd_lists_size_persist[_process].load() >= (TRANS_SIZE*TEST_SIZE)) printf("thread_list_size_persist[%d] = %d\n", _process, thrd_lists_size_persist[_process].load());

}

void insert_txn_persist(long int _txn_invocation, long int _txn_response, int size)
{
	int _process = get_process_id();

	if(thrd_lists_persist[_process].size() == 0) return; //ADDED RECENTLY

	std::list<Method>::iterator m1_persist_it = thrd_lists_itr_persist[_process];
	if(m1_persist_it == thrd_lists_persist[_process].end()) return; //ADDED RECENTLY

	std::list<Method>::iterator m1_persist_it_prev;

	if(thrd_lists_size_persist[_process].load() != 0) ++m1_persist_it;
	if(m1_persist_it == thrd_lists_persist[_process].end()) return; //ADDED RECENTLY

	/*if(m1_persist_it == thrd_lists[_process].end()) { printf("Thread %d: Iterator fail 1\n", _process); }
	else {
		if(m1_persist_it->type == PRODUCER)
			printf("Thread %d: m1_it 1 advanced to PRODUCER ITEM %d\n", _process, m1_persist_it->item_key);
		else if(m1_persist_it->type == CONSUMER)
			printf("Thread %d: m1_it 1 advanced to CONSUMER ITEM %d\n", _process, m1_persist_it->item_key);
		else if(m1_persist_it->type == READER)
			printf("Thread %d: m1_it 1 advanced to READER ITEM %d\n", _process, m1_persist_it->item_key);
		else if(m1_persist_it->type == WRITER)
			printf("Thread %d: m1_it 1 advanced to WRITER ITEM %d\n", _process, m1_persist_it->item_key);
		else
			printf("Thread %d: m1_it 1 advanced to STRAY ITEM %d\n", _process, m1_persist_it->item_key);
	}*/

	int txn_id_persist = m1_persist_it->id;
	
	int running_size = 0;

	for(unsigned int j = 0; j < size; j++)
	{	
		m1_persist_it->txn_invocation = _txn_invocation; 
		m1_persist_it->txn_response = _txn_response; 
		m1_persist_it->txn_id = txn_id_persist; 	

		running_size = running_size + 1;

		m1_persist_it_prev = m1_persist_it;
		if(j < (size - 1))
		{
			++m1_persist_it;
			if(m1_persist_it == thrd_lists_persist[_process].end()) { 
				//printf("Thread %d: Persist Iterator fail 2\n", _process); 
				m1_persist_it = m1_persist_it_prev; break; 
			} else {
				/*if(m1_persist_it->type == PRODUCER)
					printf("Thread %d: m1_it 2 advanced to PRODUCER ITEM %d\n", _process, m1_persist_it->item_key);
				else if(m1_persist_it->type == CONSUMER)
					printf("Thread %d: m1_it 2 advanced to CONSUMER ITEM %d\n", _process, m1_persist_it->item_key);
				else if(m1_persist_it->type == READER)
					printf("Thread %d: m1_it 2 advanced to READER ITEM %d\n", _process, m1_persist_it->item_key);
				else if(m1_persist_it->type == WRITER)
					printf("Thread %d: m1_it 2 advanced to WRITER ITEM %d\n", _process, m1_persist_it->item_key);
				else
					printf("Thread %d: m1_it 2 advanced to STRAY ITEM %d\n", _process, m1_persist_it->item_key);*/
			}	
		}
		
	}
	//printf("Thread %d: size = %d, running_size = %d\n", _process, thrd_lists_size_persist[_process].load(), running_size);

	thrd_lists_itr_persist[_process] = m1_persist_it;	
	thrd_lists_size_persist[_process].fetch_add(running_size);
}

void rollback_txn()
{
	int _process = get_process_id();

	//printf("Thread %d: Attempting to Rollback_txn\n", _process);

	int size = thrd_lists_size[_process].load();
	//int size_persist = thrd_lists_size_persist[_process].load();
	
	//printf("Abort\n");
	while(thrd_lists[_process].size() > size)
	{
		Method m = thrd_lists[_process].back();
		/*if(m.type == PRODUCER)
			printf("Thread %d: m pop_back PRODUCE ITEM %d\n", _process, m.item_key);
		else if(m.type == CONSUMER)
			printf("Thread %d: m pop_back CONSUME ITEM %d\n", _process, m.item_key);
		else if(m.type == READER)
			printf("Thread %d: m pop_back READ ITEM %d\n", _process, m.item_key);
		else if(m.type == WRITER)
			printf("Thread %d: m pop_back WRITE ITEM %d\n", _process, m.item_key);
		else
			printf("Thread %d: m pop_back STRAY ITEM %d\n", _process, m.item_key);*/
		thrd_lists[_process].pop_back();
	}

	//while(thrd_lists_persist[_process].size() > size_persist)
	//{
	//	thrd_lists_persist[_process].pop_back();
	//}
}

void rollback_txn_persist()
{
	int _process = get_process_id();

	persist_map[_process].clear();

	int size_persist = thrd_lists_size_persist[_process].load();
	
	while(thrd_lists_persist[_process].size() > size_persist)
	{
		/*Method m = thrd_lists[_process].back();
		if(m.type == PRODUCER)
			printf("Thread %d: m pop_back PRODUCER ITEM %d\n", _process, m.item_key);
		else if(m.type == CONSUMER)
			printf("Thread %d: m pop_back CONSUMER ITEM %d\n", _process, m.item_key);
		else if(m.type == READER)
			printf("Thread %d: m pop_back READER ITEM %d\n", _process, m.item_key);
		else if(m.type == WRITER)
			printf("Thread %d: m pop_back WRITER ITEM %d\n", _process, m.item_key);
		else
			printf("Thread %d: m pop_back STRAY ITEM %d\n", _process, m.item_key);*/
		//printf("thrd_lists[%d].size() = %d, size = %d\n", _process, thrd_lists[_process].size(), size);
		thrd_lists_persist[_process].pop_back();
	}
}

#endif

void insert_method()
{
	int _process = get_process_id();

	if(thrd_lists[_process].size() == 0) return; //ADDED RECENTLY

	std::list<Method>::iterator m1_it = thrd_lists_itr[_process];
	//std::list<Method>::iterator m1_persist_it = thrd_lists_itr_persist[_process];

	//if(m1_it != thrd_lists[_process].begin()) ++m1_it;
	if(thrd_lists_size[_process].load() != 0) ++m1_it;

	//if(m1_persist_it != thrd_lists_persist[_process].begin()) ++m1_persist_it;
	//if(thrd_lists_size_persist[_process].load() != 0) ++m1_persist_it;
	
	thrd_lists_size[_process].fetch_add(1);
	//thrd_lists_size_persist[_process].fetch_add(1);

	//if(thrd_lists_size[_process].load() >= (TRANS_SIZE*TEST_SIZE)) printf("thread_list_size[%d] = %d\n", _process, thrd_lists_size[_process].load());
	//if(thrd_lists_size_persist[_process].load() >= (TRANS_SIZE*TEST_SIZE)) printf("thread_list_size_persist[%d] = %d\n", _process, thrd_lists_size_persist[_process].load());
}

void insert_method_persist()
{
	int _process = get_process_id();

	if(thrd_lists_persist[_process].size() == 0) return; //ADDED RECENTLY

	std::list<Method>::iterator m1_persist_it = thrd_lists_itr_persist[_process];

	if(thrd_lists_size_persist[_process].load() != 0) ++m1_persist_it;
	
	thrd_lists_size_persist[_process].fetch_add(1);

}

void vsv_exit()
{
	int _process = get_process_id();
	done[_process].store(true);
}


//This function must be configured by the user
void vsv_args(int argc, const char *argv[])
{
	SEMANTICS = SET;
	NUM_THRDS = 4;
	TEST_SIZE = 10;
	TRANS_SIZE = 2;
	KEY_RANGE_ = 100;

	if(argc > 1) {
		if(atoi(argv[1]) < 3) SEMANTICS = SET;
		else if (atoi(argv[1]) >= 3) SEMANTICS = MAP;
	}
	if(argc > 2) NUM_THRDS = atoi(argv[2]) + 1;
	if(argc > 3) TEST_SIZE = atoi(argv[3]);
	if(argc > 4) TRANS_SIZE = atoi(argv[4]);
	if(argc > 5) KEY_RANGE_ = atoi(argv[5]) + 2;

	printf("NUM_THRDS = %d, TEST_SIZE = %d, TRANS_SIZE = %d, KEY_RANGE_ = %d\n", NUM_THRDS, TEST_SIZE, TRANS_SIZE, KEY_RANGE_);

	/*if(argc > 1) setType = atoi(argv[1]);
    if(argc > 2) numThread = atoi(argv[2]);
    if(argc > 3) testSize = atoi(argv[3]);
    if(argc > 4) tranSize = atoi(argv[4]);
    if(argc > 5) keyRange = atoi(argv[5]);
    if(argc > 6) insertion = atoi(argv[6]);
    if(argc > 7) deletion = atoi(argv[7]);
    if(argc > 8) update = atoi(argv[8]);*/
}

void vsv_args2()
{
	printf("Hello\n");	
}

