#ifndef ROMULUSSETADAPTOR_H
#define ROMULUSSETADAPTOR_H

#include <cmath>
#include <vector>


#include "../romulus/datastructures/TMLinkedListSet.hpp"
#include "../romulus/datastructures/TMHashMap.hpp"
#include "../romulus/datastructures/TMSkipList.hpp"
#include "../romulus/datastructures/TMMDListSet.hpp"


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



template<>
class SetAdaptor<TMLinkedListSet<int>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
        romulusSet = nullptr;
        TM_WRITE_TRANSACTION([&] () {
            romulusSet = TM_ALLOC<TMLinkedListSet<int>>();
        });        
    }

    ~SetAdaptor() {
        TM_WRITE_TRANSACTION([&] () {
            TM_FREE(romulusSet);
        });        
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);

		FILE* pfile = fopen("output.txt", "a"); //CORRECTNESS ANNOTATIONS
		fprintf(pfile, "%u %u\n", g_count_commit, g_count_abort); //CORRECTNESS ANNOTATIONS
		fclose(pfile); //CORRECTNESS ANNOTATIONS
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

    bool ExecuteOps(const SetOpArray& ops)
    {
        bool ret = true;

        // TM_BEGIN_TRANSACTION();
        TM_WRITE_TRANSACTION([&] () {
 
        if(ret == true)
        {
			long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = romulusSet->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = romulusSet->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = romulusSet->remove(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }

                if(ret == false)
                {
                    //stm::restart();
                    // tx->tmabort(tx);
                    // std::cout << "Figure out how to abort!\n\r" << std::endl;
                    break;
                }
            }
			
			if(ret)
		    {
				long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				rollback_txn(); //CORRECTNESS ANNOTATIONS
				//rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
		    }
        }
        // TM_END_TRANSACTION();
        });   		

		//Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			for(int i = 0; i < ops.size(); i++) 
			{
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
				long int response; //CORRECTNESS ANNOTATIONS

				if(ops[i].type == INSERT)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else if(ops[i].type == DELETE)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
				} 

			}
			long int txn_response_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn_persist(txn_invocation_persist, txn_response_persist, ops.size()); //CORRECTNESS ANNOTATIONS

        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
			//rollback_txn_persist(); //Don't need rollback because we only insert successful persisted operations

        }
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

		/*std::string transaction;
		transaction.append("Transaction");
		
		for(int i = 0; i < ops.size(); i++)
		{
			if(ops[i].type == INSERT)
			{
				char str[80];
				sprintf(str, " P %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == DELETE)
			{
				char str[80];
				sprintf(str, " C %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == FIND)
			{
				char str[80];
				sprintf(str, " F %d", ops[i].key);
				transaction.append(str); 
			}
		}
		if(ret)
			transaction.append(":Commit"); 
		else
			transaction.append(":Abort");
		printf("%s\n", transaction.c_str());*/

        return ret;

        // return m_list->ExecuteOps(desc);
    }

private:

    TMLinkedListSet<int>* romulusSet;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};

template<>
class SetAdaptor<TMSkipList<int>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
        romulusSkipList = nullptr;
        TM_WRITE_TRANSACTION([&] () {
            romulusSkipList = TM_ALLOC<TMSkipList<int>>();
        });        
    }

    ~SetAdaptor() {
        TM_WRITE_TRANSACTION([&] () {
            TM_FREE(romulusSkipList);
        });        
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);

		FILE* pfile = fopen("output.txt", "a"); //CORRECTNESS ANNOTATIONS
		fprintf(pfile, "%u %u\n", g_count_commit, g_count_abort); //CORRECTNESS ANNOTATIONS
		fclose(pfile); //CORRECTNESS ANNOTATIONS
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

    bool ExecuteOps(const SetOpArray& ops)
    {
        bool ret = true;
        // TM_BEGIN_TRANSACTION();
        TM_WRITE_TRANSACTION([&] () {

     
        if(ret == true)
        {
			long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = romulusSkipList->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = romulusSkipList->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = romulusSkipList->remove(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }

                if(ret == false)
                {
                    //stm::restart();
                    // tx->tmabort(tx);
                    // std::cout << "Figure out how to abort!\n\r" << std::endl;
                    break;
                }
            }

			if(ret)
		    {
				long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				rollback_txn(); //CORRECTNESS ANNOTATIONS
		    }
        }
        // TM_END_TRANSACTION();
        });   

		//Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			for(int i = 0; i < ops.size(); i++) 
			{
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
				long int response; //CORRECTNESS ANNOTATIONS

				if(ops[i].type == INSERT)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else if(ops[i].type == DELETE)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
				} 

			}
			long int txn_response_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn_persist(txn_invocation_persist, txn_response_persist, ops.size()); //CORRECTNESS ANNOTATIONS
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
			//rollback_txn_persist(); //Don't need rollback because we only insert successful persisted operations

        }
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

		/*std::string transaction;
		transaction.append("Transaction");
		
		for(int i = 0; i < ops.size(); i++)
		{
			if(ops[i].type == INSERT)
			{
				char str[80];
				sprintf(str, " P %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == DELETE)
			{
				char str[80];
				sprintf(str, " C %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == FIND)
			{
				char str[80];
				sprintf(str, " F %d", ops[i].key);
				transaction.append(str); 
			}
		}
		if(ret)
			transaction.append(":Commit"); 
		else
			transaction.append(":Abort");
		printf("%s\n", transaction.c_str());*/

        return ret;

        // return m_list->ExecuteOps(desc);
    }

private:

    TMSkipList<int>* romulusSkipList;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};

template<>
class SetAdaptor<TMMDListSet<int>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
        romulusMDList = nullptr;
        TM_WRITE_TRANSACTION([&] () {
            romulusMDList = TM_ALLOC<TMMDListSet<int>>();
        });        
    }

    ~SetAdaptor() {
        TM_WRITE_TRANSACTION([&] () {
            TM_FREE(romulusMDList);
        });        
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);
		
		FILE* pfile = fopen("output.txt", "a"); //CORRECTNESS ANNOTATIONS
		fprintf(pfile, "%u %u\n", g_count_commit, g_count_abort); //CORRECTNESS ANNOTATIONS
		fclose(pfile); //CORRECTNESS ANNOTATIONS
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

    bool ExecuteOps(const SetOpArray& ops)
    {
        bool ret = true;
        // TM_BEGIN_TRANSACTION();
        TM_WRITE_TRANSACTION([&] () {

     
        if(ret == true)
        {
			long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = romulusMDList->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = romulusMDList->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = romulusMDList->remove(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }

                if(ret == false)
                {
                    //stm::restart();
                    // tx->tmabort(tx);
                    // std::cout << "Figure out how to abort!\n\r" << std::endl;
                    break;
                }
            }
			if(ret)
		    {
				long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				rollback_txn(); //CORRECTNESS ANNOTATIONS
		    }
        }
        // TM_END_TRANSACTION();
        });   

		//Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			for(int i = 0; i < ops.size(); i++) 
			{
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
				long int response; //CORRECTNESS ANNOTATIONS

				if(ops[i].type == INSERT)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else if(ops[i].type == DELETE)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
				} 

			}
			long int txn_response_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn_persist(txn_invocation_persist, txn_response_persist, ops.size()); //CORRECTNESS ANNOTATIONS
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
			//rollback_txn_persist(); //Don't need rollback because we only insert successful persisted operations
        }
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

		/*std::string transaction;
		transaction.append("Transaction");
		
		for(int i = 0; i < ops.size(); i++)
		{
			if(ops[i].type == INSERT)
			{
				char str[80];
				sprintf(str, " P %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == DELETE)
			{
				char str[80];
				sprintf(str, " C %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == FIND)
			{
				char str[80];
				sprintf(str, " F %d", ops[i].key);
				transaction.append(str); 
			}
		}
		if(ret)
			transaction.append(":Commit"); 
		else
			transaction.append(":Abort");
		printf("%s\n", transaction.c_str());*/

        return ret;

        // return m_list->ExecuteOps(desc);
    }

private:

    TMMDListSet<int>* romulusMDList;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};

template<typename T>
class MapAdaptor
{
};

enum MapOpType
{
    MAP_FIND = 0,
    MAP_INSERT,
    MAP_DELETE,
    MAP_UPDATE
};

struct MapOperator
{
    uint8_t type;
    uint32_t key;
    uint32_t value;
    uint32_t expected;
    uint32_t threadId;
};

enum MapOpStatus
{
    MAP_ACTIVE = 0,
    MAP_COMMITTED,
    MAP_ABORTED
};

typedef std::vector<MapOperator> MapOpArray;

template<>
class MapAdaptor<TMHashMap<int,int>>
{
public:


    MapAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    { 
        cap = std::max(cap,(uint64_t)keyRange);
        int mapHeadSize				=std::pow(2, ((int)std::ceil(std::log2(cap))));
        cap = mapHeadSize;// = cap;
        romulusMap = nullptr;
        TM_WRITE_TRANSACTION([&] () {
            romulusMap = TM_ALLOC<TMHashMap<int,int>>();
        });            


    }

    ~MapAdaptor() {
        TM_WRITE_TRANSACTION([&] () {
            TM_FREE(romulusMap);
        });        
        printf("Total commit %u, abort (total/fake) %u/0\n", g_count_commit, g_count_abort);

		FILE* pfile = fopen("output.txt", "a"); //CORRECTNESS ANNOTATIONS
		fprintf(pfile, "%u %u\n", g_count_commit, g_count_abort); //CORRECTNESS ANNOTATIONS
		fclose(pfile); //CORRECTNESS ANNOTATIONS
    }    

    void Init()
    {
        g_count_commit = 0;
        g_count_abort = 0;
        g_count_stm_abort = 0;
    }

    void Uninit(){
        // delete romulusMap;
    }

    bool ExecuteOps(const MapOpArray& ops, int threadId)
    {
        bool ret = true;
// #ifndef PMDK_PTM
//         TM_BEGIN_TRANSACTION();
// #else        
        TM_WRITE_TRANSACTION([&] () {
// #endif
            if(ret == true)
            {
				long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
                for(uint32_t i = 0; i < ops.size(); ++i)
                {
					long int response; //CORRECTNESS ANNOTATIONS
					long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                    uint32_t val = ops[i].key;
                    if(ops[i].type == MAP_FIND)
                    {
                        ret = romulusMap->contains(val);
						response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
						create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                    }
                    else if(ops[i].type == MAP_INSERT || ops[i].type == MAP_UPDATE)
                    {
                        ret = romulusMap->add(val);
						response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
						create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                    }
                    else
                    {
                        ret = romulusMap->remove(val);
						response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
						create_method(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                    }

                    if(ret == false)
                    {
                        //stm::restart();
                        // tx->tmabort(tx);
                        // std::cout << "Figure out how to abort: " << g_count_abort << std::endl;
                        break;
                    }
                }
				if(ret)
				{
					long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
				}
				else
				{
					rollback_txn(); //CORRECTNESS ANNOTATIONS
				}
            }
        // TM_END_TRANSACTION();
// #ifndef PMDK_PTM        
        // TM_END_TRANSACTION();
// #else
   }); 
// #endif         
		//Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			for(int i = 0; i < ops.size(); i++) 
			{
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
				long int response; //CORRECTNESS ANNOTATIONS

				if(ops[i].type == MAP_INSERT || ops[i].type == MAP_UPDATE)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else if(ops[i].type == MAP_DELETE)
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				}
				else
				{
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method_persist(ops[i].key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
				} 

			}
			long int txn_response_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn_persist(txn_invocation_persist, txn_response_persist, ops.size()); //CORRECTNESS ANNOTATIONS
			
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
			//rollback_txn_persist(); //Don't need rollback because we only insert successful persisted operations
			
        }
        // printf("commit %u, abort %u\n\r", g_count_commit, g_count_abort);

		/*std::string transaction;
		transaction.append("Transaction");
		
		for(int i = 0; i < desc->size; i++)
		{
			if(desc->ops[i].type == MAP_INSERT)
			{
				char str[80];
				sprintf(str, " P %d", desc->ops[i].key);
				transaction.append(str); 
			} else if (desc->ops[i].type == MAP_DELETE)
			{
				char str[80];
				sprintf(str, " C %d", desc->ops[i].key);
				transaction.append(str); 
			} else if (desc->ops[i].type == MAP_FIND)
			{
				char str[80];
				sprintf(str, " F %d", desc->ops[i].key);
				transaction.append(str); 
			} else if (desc->ops[i].type == MAP_UPDATE)
			{
				char str[80];
				sprintf(str, " U %d", desc->ops[i].key);
				transaction.append(str); 
			}

		}
		if(desc->status == COMMITTED)
			transaction.append(":Commit"); 
		else
			transaction.append(":Abort");
		printf("%s\n", transaction.c_str());*/

        return ret;
    }

private:
    TMHashMap<int,int>* romulusMap;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;  

};




#endif /* end of include guard: ROMULUSSETADAPTOR_H */
