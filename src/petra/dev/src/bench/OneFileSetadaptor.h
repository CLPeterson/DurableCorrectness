#ifndef ROMULUSSETADAPTOR_H
#define ROMULUSSETADAPTOR_H

#include <cmath>
#include <vector>


#include "../OneFile/pdatastructures/TMLinkedListSet.hpp"
#include "../OneFile/ptms/OneFilePTMLF.hpp"
#include "../OneFile/pdatastructures/TMHashMap.hpp"
#include "../OneFile/pdatastructures/TMSkipList.hpp"
#include "../OneFile/pdatastructures/OFMDListSet.hpp"

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


//TODO: Update Map Adaptor Template
template<>
class SetAdaptor<TMLinkedListSet<int,poflf::OneFileLF,poflf::tmtype>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    {
        onefileSet = nullptr;
        poflf::OneFileLF::template updateTx<bool>([&] () {
            onefileSet = poflf::OneFileLF::template tmNew<TMLinkedListSet<int,poflf::OneFileLF,poflf::tmtype>>();
            return true;
        });

    }

    ~SetAdaptor() {

        poflf::OneFileLF::template updateTx<bool>([=] () {
            poflf::OneFileLF::tmDelete(onefileSet);
            return true;
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

		long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
		long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

        ret = poflf::OneFileLF::template updateTx<bool>([=] () {
                bool ret = true;
        // TM_WRITE_TRANSACTION([&] () {

            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = onefileSet->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = onefileSet->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = onefileSet->remove(val);
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
				//long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				//insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				//rollback_txn(); //CORRECTNESS ANNOTATIONS
				//rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
		    }

            return ret;
        // TM_END_TRANSACTION();
        });   

        //Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			//long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			/*for(int i = 0; i < ops.size(); i++) 
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
			insert_txn_persist(txn_invocation_persist, txn_response_persist, ops.size()); //CORRECTNESS ANNOTATIONS*/
			
			long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS
			
        }
        else
        {
            __sync_fetch_and_add(&g_count_abort, 1);
			//rollback_txn_persist(); //Don't need rollback because we only insert successful persisted operations
			rollback_txn(); //CORRECTNESS ANNOTATIONS
			rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
			
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

	void validate_and_recovery() {
		printf("Starting recovery\n");
		poflf::OneFileLF::validate_and_recovery();
	}

private:

    TMLinkedListSet<int,poflf::OneFileLF,poflf::tmtype>* onefileSet;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;

};


#define NUM_LEVELS 20

template<>
class SetAdaptor<TMSkipList<int,poflf::OneFileLF,poflf::tmtype>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    { 

        onefileSkipList = nullptr;
        poflf::OneFileLF::template updateTx<bool>([&] () {
            onefileSkipList = poflf::OneFileLF::template tmNew<TMSkipList<int,poflf::OneFileLF,poflf::tmtype>>();
            return true;
        });
    }

    ~SetAdaptor()
    {

        // poflf::OneFileLF::template updateTx<bool>([=] () {
        //     poflf::OneFileLF::tmDelete(onefileSkipList);
        //     return true;
        // });
  
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

    void Uninit()
    {
        // destroy_transskip_subsystem(); 
    }

    bool ExecuteOps(const SetOpArray& ops)
    {
        //TransList::Desc* desc = m_list.AllocateDesc(ops.size());
bool ret = true;
        // TM_BEGIN_TRANSACTION();

		long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

        ret = poflf::OneFileLF::template updateTx<bool>([=] () {
                bool ret = true;
        // TM_WRITE_TRANSACTION([&] () {
			
            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = onefileSkipList->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = onefileSkipList->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = onefileSkipList->remove(val);
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
				//long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				//insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				rollback_txn(); //CORRECTNESS ANNOTATIONS
				rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
		    }

            return ret;
        // TM_END_TRANSACTION();
        });   

        //Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			/*long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
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
			*/
			
			long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS
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
    }

	void validate_and_recovery() {
		printf("Starting recovery\n");
		poflf::OneFileLF::validate_and_recovery();
	}

private:

    TMSkipList<int,poflf::OneFileLF,poflf::tmtype>* onefileSkipList;
    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;    
};


template<>
class SetAdaptor<OFMDListSet<int,poflf::OneFileLF,poflf::tmtype>>
{
public:
    SetAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    { 

        onefileMDList = nullptr;
        poflf::OneFileLF::template updateTx<bool>([&] () {
            onefileMDList = poflf::OneFileLF::template tmNew<OFMDListSet<int,poflf::OneFileLF,poflf::tmtype>>();
            return true;
        });
    }

    ~SetAdaptor()
    {
        // poflf::OneFileLF::template updateTx<bool>([=] () {
        //     poflf::OneFileLF::tmDelete(onefileMDList);
        //     return true;
        // });
  
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

    void Uninit()
    {
        // destroy_transskip_subsystem(); 
    }

    bool ExecuteOps(const SetOpArray& ops)
    {
        //TransList::Desc* desc = m_list.AllocateDesc(ops.size());
bool ret = true;
        // TM_BEGIN_TRANSACTION();

		long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

        ret = poflf::OneFileLF::template updateTx<bool>([=] () {
                bool ret = true;
        // TM_WRITE_TRANSACTION([&] () {
			
            for(uint32_t i = 0; i < ops.size(); ++i)
            {
				long int response; //CORRECTNESS ANNOTATIONS
				long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                uint32_t val = ops[i].key;
                if(ops[i].type == FIND)
                {
                    ret = onefileMDList->contains(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else if(ops[i].type == INSERT)
                {
                    ret = onefileMDList->add(val);
					response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                }
                else
                {
                    ret = onefileMDList->remove(val);
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
				//long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				//insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
		    }
		    else
		    {
				rollback_txn(); //CORRECTNESS ANNOTATIONS
				rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
		    }

            return ret;
        // TM_END_TRANSACTION();
        });   

        //Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			/*long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
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
			*/
			
			long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS
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
    }

	void validate_and_recovery() {
		printf("Starting recovery\n");
		poflf::OneFileLF::validate_and_recovery();
	}

private:

    OFMDListSet<int,poflf::OneFileLF,poflf::tmtype>* onefileMDList;
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
class MapAdaptor<TMHashMap<int,int, poflf::OneFileLF,poflf::tmtype>>
{
public:


    MapAdaptor(uint64_t cap, uint64_t threadCount, uint32_t transSize, uint32_t keyRange)
    { 
        cap = std::max(cap,(uint64_t)keyRange);
        int mapHeadSize				=std::pow(2, ((int)std::ceil(std::log2(cap))));
        cap = mapHeadSize;// = cap;
        onefileMap = nullptr;

        poflf::OneFileLF::template updateTx<bool>([&] () {
            onefileMap = poflf::OneFileLF::template tmNew<TMHashMap<int,int, poflf::OneFileLF,poflf::tmtype>>();
            return true;
        });          


    }

    ~MapAdaptor() {


        poflf::OneFileLF::template updateTx<bool>([=] () {
            poflf::OneFileLF::tmDelete(onefileMap);
            return true;
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

		long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

        ret = poflf::OneFileLF::template updateTx<bool>([=] () {
                bool ret = true;
            if(ret == true)
            {
				
                for(uint32_t i = 0; i < ops.size(); ++i)
                {
					long int response; //CORRECTNESS ANNOTATIONS
					long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	

                    uint32_t val = ops[i].key;
                    if(ops[i].type == MAP_FIND)
                    {
                        ret = onefileMap->contains(val);
						response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
						create_method(ops[i].key, INT_MIN, SET, READER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                    }
                    else if(ops[i].type == MAP_INSERT || ops[i].type == MAP_UPDATE)
                    {
                        ret = onefileMap->add(val);
						response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
						create_method(ops[i].key, INT_MIN, SET, PRODUCER, invocation, response, ret); //CORRECTNESS ANNOTATIONS
                    }
                    else
                    {
                        ret = onefileMap->remove(val);
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
					//long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
					//insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS	
				}
				else
				{
					rollback_txn(); //CORRECTNESS ANNOTATIONS
					rollback_txn_persist(); //CORRECTNESS ANNOTATIONS
				}
            }
            return ret;
        });

        //Transaction is persisted at this point
        if(ret)
        {
            __sync_fetch_and_add(&g_count_commit, 1);
			/*long int txn_invocation_persist = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
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
			*/
			
			long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			insert_txn(txn_invocation, txn_response, ops.size()); //CORRECTNESS ANNOTATIONS
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
			if(ops[i].type == MAP_INSERT)
			{
				char str[80];
				sprintf(str, " P %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == MAP_DELETE)
			{
				char str[80];
				sprintf(str, " C %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == MAP_FIND)
			{
				char str[80];
				sprintf(str, " F %d", ops[i].key);
				transaction.append(str); 
			} else if (ops[i].type == MAP_UPDATE)
			{
				char str[80];
				sprintf(str, " U %d", ops[i].key);
				transaction.append(str); 
			}

		}
		if(ret)
			transaction.append(":Commit"); 
		else
			transaction.append(":Abort");
		printf("%s\n", transaction.c_str());*/

        return ret;
    }

	void validate_and_recovery() {
		printf("Starting recovery\n");
		poflf::OneFileLF::validate_and_recovery();
	}

private:
    TMHashMap<int,int, poflf::OneFileLF,poflf::tmtype>* onefileMap;

    uint32_t g_count_commit = 0;
    uint32_t g_count_abort = 0;  
    uint32_t g_count_stm_abort = 0;  

};



#endif /* end of include guard: ROMULUSSETADAPTOR_H */
