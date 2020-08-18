#ifndef SETADAPTOR_H
#define SETADAPTOR_H

#include <cmath>
#include <vector>


//#include "../romulus/datastructures/TMLinkedListSet.hpp"
//#include "../romulus/datastructures/TMHashMap.hpp"
//#include "../romulus/datastructures/TMSkipList.hpp"
//#include "../romulus/datastructures/TMMDListSet.hpp"


//#include "../src/linkedlist/lf-linkedlist.h"
//#include "../src/skiplist/lf-skiplist.h"

//#include "../src/linkedlist/lf-linkedlist.h"

#include "../common/vsv.h" //CORRECTNESS ANNOTATIONS

enum SetOpType
{
    FIND = 0,
    INSERT,
    DELETE
};

struct SetOperator
{
    uint8_t type;
    uint32_t key;
};

enum SetOpStatus
{
    LIVE = 0,
    COMMITTED,
    ABORTED
};

enum SetPersistStatus
{
    MAYBE = 0,
    IN_PROGRESS,
    PERSISTED,
};

typedef std::vector<SetOperator> SetOpArray;

template<typename T>
class SetAdaptor
{
};


#ifdef LINKEDLIST_T
#include "../src/linkedlist/lf-linkedlist.h"
template<>
class SetAdaptor<linkedlist_t*>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
		lc = cache_create();
  		EpochGlobalInit(lc);
	
		EpochThread epoch = EpochThreadInit(threadCount);
		linkedList = new_linkedlist(epoch);  
    }

    ~SetAdaptor() {
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);
	delete_linkedlist(linkedList);	
    }

    void Init()
    {
        g_count_commit = 0;
        g_count_abort = 0;
        g_count_stm_abort = 0;
    }

    void Uninit(){
        // delete romulusSet;
    }

    //bool ExecuteOps(const SetOpArray& ops)
	bool ExecuteOps(const SetOpArray& ops, EpochThread epoch)
    {
        //bool ret = true;
		int ret = 1;
        
        for(uint32_t i = 0; i < ops.size(); ++i)
        {
			long int response; //CORRECTNESS ANNOTATIONS
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

            uint32_t val = ops[i].key;
            if(ops[i].type == FIND)
            {
				ret = linkedlist_find(linkedList, val, epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else if(ops[i].type == INSERT)
            {
				ret = linkedlist_insert(linkedList, val,(val+4),epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else
            {
				ret = linkedlist_remove(linkedList, val, epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
			insert_method(); //CORRECTNESS ANNOTATIONS

        }

        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
		}
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

        return ret;

        // return m_list->ExecuteOps(desc);
    }

	void Recover() 
	{
		
	}

private:

    //TMLinkedListSet<int>* romulusSet;
	linkedlist_t* linkedList;

	linkcache_t* lc;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};
#endif

#ifdef SKIPLIST_T
__thread unsigned long * seeds;

#include "../src/skiplist/lf-skiplist.h"
template<>
class SetAdaptor<skiplist_t*>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
		lc = cache_create();
  		EpochGlobalInit(lc);
	
		EpochThread epoch = EpochThreadInit(threadCount);
		skipList = new_skiplist(epoch);  
    }

    ~SetAdaptor() {
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);
	delete_skiplist(skipList);	
		
    }

    void Init()
    {
        g_count_commit = 0;
        g_count_abort = 0;
        g_count_stm_abort = 0;
    }

    void Uninit(){
        // delete romulusSet;
    }

    //bool ExecuteOps(const SetOpArray& ops)
	bool ExecuteOps(const SetOpArray& ops, EpochThread epoch)
    {
		seeds = seed_rand();
        //bool ret = true;
		int ret = 1;
        
        for(uint32_t i = 0; i < ops.size(); ++i)
        {
			long int response; //CORRECTNESS ANNOTATIONS
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

            uint32_t val = ops[i].key;
            if(ops[i].type == FIND)
            {
				ret = skiplist_find(skipList, val, epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else if(ops[i].type == INSERT)
            {
				ret = skiplist_insert(skipList, val,(val+4),epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else
            {
				ret = skiplist_remove(skipList, val, epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
			insert_method(); //CORRECTNESS ANNOTATIONS

        }

        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
		}
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

        return ret;

        // return m_list->ExecuteOps(desc);
    }

	void Recover() 
	{
		
	}

private:

    //TMLinkedListSet<int>* romulusSet;
	skiplist_t* skipList;

	linkcache_t* lc;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};
#endif

#ifdef BST_T

#include "../src/bst/lf-bst.h"
template<>
class SetAdaptor<NODE_PTR>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
		lc = cache_create();
  		EpochGlobalInit(lc);
	
		EpochThread epoch = EpochThreadInit(threadCount);
		bst = initialize_tree(epoch);
    }

    ~SetAdaptor() {
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);
	
    }

    void Init()
    {
        g_count_commit = 0;
        g_count_abort = 0;
        g_count_stm_abort = 0;
    }

    void Uninit(){
        // delete romulusSet;
    }

    //bool ExecuteOps(const SetOpArray& ops)
	bool ExecuteOps(const SetOpArray& ops, EpochThread epoch)
    {
		bst_init_local();
        //bool ret = true;
		int ret = 1;
        
        for(uint32_t i = 0; i < ops.size(); ++i)
        {
			long int response; //CORRECTNESS ANNOTATIONS
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

            uint32_t val = ops[i].key;
            if(ops[i].type == FIND)
            {
				ret = bst_search(val,bst,epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else if(ops[i].type == INSERT)
            {
				ret = bst_insert(val,(val+4),bst,epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else
            {
				ret = bst_remove(val, bst, epoch, lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
			insert_method(); //CORRECTNESS ANNOTATIONS

        }

        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
		}
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

        return ret;

        // return m_list->ExecuteOps(desc);
    }

	void Recover() 
	{
		
	}

private:

    //TMLinkedListSet<int>* romulusSet;
	NODE_PTR bst;

	linkcache_t* lc;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};
#endif

#ifdef HASHTABLE_T

__thread unsigned long * seeds;

#include "../src/hashtable/intset.h"
unsigned int maxhtlength;
template<>
class SetAdaptor<ht_intset_t*>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
		maxhtlength = cap;

		lc = cache_create();
  		EpochGlobalInit(lc);
	
		EpochThread epoch = EpochThreadInit(threadCount);
		hashtable = ht_new(epoch);
		if(hashtable == NULL) printf("ERROR: HASHTABLE IS NULL\n");
    }

    ~SetAdaptor() {
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);
	
    }

    void Init()
    {
        g_count_commit = 0;
        g_count_abort = 0;
        g_count_stm_abort = 0;
    }

    void Uninit(){
        // delete romulusSet;
    }

    //bool ExecuteOps(const SetOpArray& ops)
	bool ExecuteOps(const SetOpArray& ops, EpochThread epoch)
    {
		seeds = seed_rand();
        //bool ret = true;
		int ret = 1;
        
        for(uint32_t i = 0; i < ops.size(); ++i)
        {
			long int response; //CORRECTNESS ANNOTATIONS
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

            uint32_t val = ops[i].key;
            if(ops[i].type == FIND)
            {
				ret = ht_contains(hashtable,val,epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else if(ops[i].type == INSERT)
            {
				ret = ht_add(hashtable,val,(val+4),epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
            else
            {
				ret = ht_remove(hashtable,val,epoch,lc);
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
            }
			insert_method(); //CORRECTNESS ANNOTATIONS

        }

        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
		}
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

        return ret;

        // return m_list->ExecuteOps(desc);
    }

	void Recover() 
	{
		
	}

private:

    //TMLinkedListSet<int>* romulusSet;
	ht_intset_t* hashtable;

	linkcache_t* lc;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};
#endif


#endif /* end of include guard: SETADAPTOR_H */
