#include <cstdlib>
#include <cstdio>
#include <new>
#include "transmap.h"


#define SET_MARK(_p)    ((Node *)(((uintptr_t)(_p)) | 1))
#define CLR_MARK(_p)    ((Node *)(((uintptr_t)(_p)) & ~1))
#define CLR_MARKD(_p)    ((NodeDesc *)(((uintptr_t)(_p)) & ~1))
#define IS_MARKED(_p)     (((uintptr_t)(_p)) & 1)

__thread TransMap::HelpStack mapHelpStack;

PERSIST_CODE
(

inline bool TransMap::needPersistenceHelp(Desc* desc)
{
    return desc->persistStatus != PERSISTED;
}

inline void TransMap::persistDesc(Desc* desc)
{
    for(int i = 0; i < desc->size; i++) 
    {
        DTX::PERSIST_FLUSH_ONLY(&(desc->ops[i]), sizeof(Operator));
    }
    desc->txid = lastTxId++;
    DTX::PERSIST_FLUSH_ONLY(desc, sizeof(Desc));
    DTX::PERSIST_BARRIER_ONLY();
    desc->persistStatus = PERSISTED;
}

)


//TODO: make __thread std::vector<VALUE> toR; where each thread can put the results of its find operations and return them to the user
//TODO: this would have to be made to work even if other threads helped, similarly to the update mechanism
bool TransMap::ExecuteOps(Desc* desc, int threadId)//, std::vector<VALUE> &toR)
{
    mapHelpStack.Init();

    HelpOps(desc, 0, threadId);//, toR);

    bool ret = desc->status != MAP_ABORTED;

    ASSERT_CODE
    (
        if(ret)
        {
            for(uint32_t i = 0; i < desc->size; ++i)
            {
                if(desc->ops[i].type == MAP_INSERT)
                {
                    __sync_fetch_and_add(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_DELETE)
                {
                    __sync_fetch_and_sub(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_UPDATE)
                {
                    __sync_fetch_and_add(&g_count_upd, 1);
                }
                else
                	__sync_fetch_and_add(&g_count_fnd, 1);
            }
        }
    );

    return ret;
}

bool TransMap::ExecuteOpsDBBenchmark(Desc* desc, int threadId)
{
    mapHelpStack.Init();

    HelpOps(desc, 0, threadId);//, toR);

    bool ret = desc->status != MAP_ABORTED;

    ASSERT_CODE
    (
        if(ret)
        {
            for(uint32_t i = 0; i < desc->size; ++i)
            {
                if(desc->ops[i].type == MAP_INSERT)
                {
                    __sync_fetch_and_add(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_DELETE)
                {
                    __sync_fetch_and_sub(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_UPDATE)
                {
                    __sync_fetch_and_add(&g_count_upd, 1);
                }
                else
                	__sync_fetch_and_add(&g_count_fnd, 1);
            }
        }
    );

    return ret;    
}

bool TransMap::ExecuteOpsTATPBenchmark(Desc* desc, int threadId)
{
    mapHelpStack.Init();

    HelpOpsTATPBenchmark(desc, 0, threadId);//, toR);

    bool ret = desc->status != MAP_ABORTED;

    ASSERT_CODE
    (
        if(ret)
        {
            for(uint32_t i = 0; i < desc->size; ++i)
            {
                if(desc->ops[i].type == MAP_INSERT)
                {
                    __sync_fetch_and_add(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_DELETE)
                {
                    __sync_fetch_and_sub(&g_count, 1);
                }
                else if(desc->ops[i].type == MAP_UPDATE)
                {
                    __sync_fetch_and_add(&g_count_upd, 1);
                }
                else
                	__sync_fetch_and_add(&g_count_fnd, 1);
            }
        }
    );

    return ret;    
}

inline void TransMap::HelpOpsDBBenchmark(Desc* desc, uint8_t opid, int T)
{

    if(desc->status != MAP_ACTIVE)
    {
        bool needHelp = false;
        PERSIST_CODE
        (
            needHelp = needPersistenceHelp(desc);
        )

READ_ONLY_OPT_CODE
(
        if(desc->isReadOnly)
            needHelp = false;
)

        if(!needHelp)
            return;
    }

    

    //Cyclic dependcy check
    if(mapHelpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            PERSIST_CODE
            (
                if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                {
                    persistDesc(desc);
                }else if(desc->persistStatus == IN_PROGRESS)
                {
                    persistDesc(desc);
                }

            )             
            __sync_fetch_and_add(&g_count_abort, 1);
            __sync_fetch_and_add(&g_count_fake_abort, 1);
        }
        else 
        {
            PERSIST_CODE
            (
                if(needPersistenceHelp(desc)) 
                {
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                }
            )              
        }

        return;//return NULL;
    }

    bool ret = true;
    //td::vector<DataNode*> retVector;
    //std::vector<VALUE> foundValues;

    mapHelpStack.Push(desc);

    while(desc->status == MAP_ACTIVE && ret != false && opid < desc->size)
    {
        Operator& op = desc->ops[opid];

        if(op.type == MAP_INSERT)
        {
            ret = Insert(desc, opid, op.key, op.value, T);
            if(ret == false) {
                op.type = MAP_UPDATE;
                ret = Update(desc, opid, op.key, op.value, T);//, toRet);
            }
        }
        else if(op.type == MAP_DELETE)
        {
            //ret = Delete(op.key, desc, opid);
            ret = Delete(desc, opid, op.key, T);
        }
        else if(op.type == MAP_UPDATE)
        {
        	// this pointer is passed by reference to the update function
        	//DataNode* toRet;
        	ret = Update(desc, opid, op.key, op.value, T);//, toRet);
        	// the pointer is copied into the vector
        	//retVector.push_back(toRet);
        }
        else
        {
            // ret = Find(op.key, desc, opid);
            // if find is successful it returns a non-null value
            // std::cout << "before find " << op.key << std::endl;
            VALUE retVal = Find(desc, opid, op.key, T);

        	if (retVal == (VALUE)NULL)
        		ret = false;
        	else{
                // std::cout << "after find " << retVal << std::endl;
                desc->ops[opid].value = retVal;
                ret = true;
            }
        		
        	//if (retVal != 0)
        		//toR.push_back(retVal);//foundValues.push_back(retVal);
        	// TODO: edit helpops to return an array of values?
        }
        
        opid++;
    }

    mapHelpStack.Pop();

    if(ret != false)
    {
    	// // any concurrent txn will see that ours is live and not use/modify our nodes
    	// // there are now no more concurrent operations from our transaction to use/modify nodes we've previously updated
    	// // before commit update the node values if our transaction owns them via their desc aliasing that of our txn
     //    for(DataNode* x: retVector)
     //    {
     //    	// everything must have returned successfully to get here, so if the last operation was an update then we
     //    	// copy the new value in, if it was an insert we overwrite the value with itself, if it was a find
     //    	// NOTE: sometimes x->nodeDesc is NULL, should this be happening? is this a result of problems with the m_desc and m_nodedesc allocators? because x->nodedesc->desc can apparently also be null
     //    	// get rid of m_desc allocator and check for && x->nodeDesc->desc != NULL which was not hit before problem with not accessing memory arose
     //    	if (x != NULL && x->nodeDesc != NULL ) 
     //    	{
	    //     	if(x->nodeDesc->desc == desc)//&& x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE)
	    //     	{
	    //     		if (x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE || 
	    //     			(x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_FIND && x->nodeDesc->desc->ops[x->nodeDesc->opid].value != 0) )//nodeDesc->value != 0) )
	    //     		{
	    //     			x->value = x->nodeDesc->desc->ops[x->nodeDesc->opid].value;//x->nodeDesc->value;
	    //     		}
	    //     	}
	    //     }
     //    }



READ_ONLY_OPT_CODE        
(
        if(desc->isReadOnly) {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }else {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }

)

NO_READ_ONLY_OPT_CODE        
(

            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
) 
        // else
    	// {
    	// 	printf("999999999999999999999999\n");
    	// 	exit(9999);
    	// }
        	// NOTE: if this happens it means the updates need to be undone here
        //note: i think this might just mean that some other thread committed the transaction
        return;//return foundValues;
    }
    else
    {


READ_ONLY_OPT_CODE        
(
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            __sync_fetch_and_add(&g_count_abort, 1);    
        }
    }else {
        // never updated node values, so don't have to undo those; they'll be interpreted correctly
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        }
    }

)

NO_READ_ONLY_OPT_CODE        
(
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        } 
)           
            //MarkForDeletion(insNodes, insPredNodes, desc);
        return;//return NULL;
    }

}

inline void TransMap::HelpOps(Desc* desc, uint8_t opid, int T)//, std::vector<VALUE> &toR)
{
    if(desc->status != MAP_ACTIVE)
    {
        bool needHelp = false;
        PERSIST_CODE
        (
            needHelp = needPersistenceHelp(desc);
        )

READ_ONLY_OPT_CODE
(
        if(desc->isReadOnly)
            needHelp = false;
)

        if(!needHelp)
            return;
    }

    

    //Cyclic dependcy check
    if(mapHelpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            PERSIST_CODE
            (
                if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                {
                    persistDesc(desc);
                }else if(desc->persistStatus == IN_PROGRESS)
                {
                    persistDesc(desc);
                }

            )             
            __sync_fetch_and_add(&g_count_abort, 1);
            __sync_fetch_and_add(&g_count_fake_abort, 1);
        }
        else 
        {
            PERSIST_CODE
            (
                if(needPersistenceHelp(desc)) 
                {
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                }
            )              
        }

        return;//return NULL;
    }

    bool ret = true;
    //td::vector<DataNode*> retVector;
    //std::vector<VALUE> foundValues;

    mapHelpStack.Push(desc);

	long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

    while(desc->status == MAP_ACTIVE && ret != false && opid < desc->size)
    {
		long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
		long int response; //CORRECTNESS ANNOTATIONS

        const Operator& op = desc->ops[opid];

        if(op.type == MAP_INSERT)
        {
            ret = Insert(desc, opid, op.key, op.value, T);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, PRODUCER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS
        }
        else if(op.type == MAP_DELETE)
        {
            //ret = Delete(op.key, desc, opid);
            ret = Delete(desc, opid, op.key, T);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, CONSUMER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS
        }
        else if(op.type == MAP_UPDATE)
        {
        	// this pointer is passed by reference to the update function
        	//DataNode* toRet;
        	ret = Update(desc, opid, op.key, op.value, T);//, toRet);
        	// the pointer is copied into the vector
        	//retVector.push_back(toRet);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, WRITER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS	
        }
        else
        {
            // ret = Find(op.key, desc, opid);
            // if find is successful it returns a non-null value
            // std::cout << "before find " << op.key << std::endl;
            VALUE retVal = Find(desc, opid, op.key, T);

        	if (retVal == (VALUE)NULL)
        		ret = false;
        	else{
                // std::cout << "after find " << retVal << std::endl;
                desc->ops[opid].value = retVal;
                ret = true;
            }
        		
        	//if (retVal != 0)
        		//toR.push_back(retVal);//foundValues.push_back(retVal);
        	// TODO: edit helpops to return an array of values?

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, READER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS	
        }
        
        opid++;
    }

    mapHelpStack.Pop();

    if(ret != false)
    {
    	// // any concurrent txn will see that ours is live and not use/modify our nodes
    	// // there are now no more concurrent operations from our transaction to use/modify nodes we've previously updated
    	// // before commit update the node values if our transaction owns them via their desc aliasing that of our txn
     //    for(DataNode* x: retVector)
     //    {
     //    	// everything must have returned successfully to get here, so if the last operation was an update then we
     //    	// copy the new value in, if it was an insert we overwrite the value with itself, if it was a find
     //    	// NOTE: sometimes x->nodeDesc is NULL, should this be happening? is this a result of problems with the m_desc and m_nodedesc allocators? because x->nodedesc->desc can apparently also be null
     //    	// get rid of m_desc allocator and check for && x->nodeDesc->desc != NULL which was not hit before problem with not accessing memory arose
     //    	if (x != NULL && x->nodeDesc != NULL ) 
     //    	{
	    //     	if(x->nodeDesc->desc == desc)//&& x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE)
	    //     	{
	    //     		if (x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE || 
	    //     			(x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_FIND && x->nodeDesc->desc->ops[x->nodeDesc->opid].value != 0) )//nodeDesc->value != 0) )
	    //     		{
	    //     			x->value = x->nodeDesc->desc->ops[x->nodeDesc->opid].value;//x->nodeDesc->value;
	    //     		}
	    //     	}
	    //     }
     //    }



READ_ONLY_OPT_CODE        
(
        if(desc->isReadOnly) {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }else {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }

)

NO_READ_ONLY_OPT_CODE        
(

            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
) 
        // else
    	// {
    	// 	printf("999999999999999999999999\n");
    	// 	exit(9999);
    	// }
        	// NOTE: if this happens it means the updates need to be undone here
        //note: i think this might just mean that some other thread committed the transaction

        //return;//return foundValues; //CORRECTNESS ANNOTATIONS
    }
    else
    {


READ_ONLY_OPT_CODE        
(
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            __sync_fetch_and_add(&g_count_abort, 1);    
        }
    }else {
        // never updated node values, so don't have to undo those; they'll be interpreted correctly
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        }
    }

)

NO_READ_ONLY_OPT_CODE        
(
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        } 
)           
            //MarkForDeletion(insNodes, insPredNodes, desc);
        //return;//return NULL; //CORRECTNESS ANNOTATIONS
    }
	
	if(desc->status == MAP_COMMITTED) //CORRECTNESS ANNOTATIONS
	{
		long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
		//Create remaining methods
		while(opid < desc->size)
		{

			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
			long int response; //CORRECTNESS ANNOTATIONS

			const Operator& op = desc->ops[opid];

			if(op.type == MAP_INSERT)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				//create_method_persist(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
			}
			else if(op.type == MAP_DELETE)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				//create_method_persist(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
			}
			else if(op.type == MAP_UPDATE)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, WRITER, invocation, response, true); //CORRECTNESS ANNOTATIONS
			}
			else
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				//create_method_persist(op.key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
			}
			opid++;

		}

		insert_txn(txn_invocation, txn_response, desc->size); //CORRECTNESS ANNOTATIONS
		//insert_txn_persist(txn_invocation, txn_response, desc->size); //CORRECTNESS ANNOTATIONS
	} else {
		rollback_txn();
		//rollback_txn_persist();
	}

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

	if(desc->persistStatus == PERSISTED && desc->status == MAP_COMMITTED)
	{
		long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
		for(int i = 0; i < desc->size; i++) 
		{
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
			long int response; //CORRECTNESS ANNOTATIONS

			const Operator& op = desc->ops[i];

			if(op.type == MAP_INSERT)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
			}
			else if(op.type == MAP_DELETE)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
			}
			else if(op.type == MAP_UPDATE)
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, WRITER, invocation, response, true); //CORRECTNESS ANNOTATIONS	
			}
			else
			{
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, READER, invocation, response, true); //CORRECTNESS ANNOTATIONS		
			} 

		}
		insert_txn_persist(txn_invocation, txn_response, desc->size); //CORRECTNESS ANNOTATIONS
	} else {
		rollback_txn_persist();
	}

}

inline void TransMap::HelpOps_rec(Desc* desc, uint8_t opid, int T)//, std::vector<VALUE> &toR) //CORRECTNESS ANNOTATIONS
{
    if(desc->status != MAP_ACTIVE)
    {
        bool needHelp = false;
        PERSIST_CODE
        (
            needHelp = needPersistenceHelp(desc);
        )

READ_ONLY_OPT_CODE
(
        if(desc->isReadOnly)
            needHelp = false;
)

        if(!needHelp)
            return;
    }

    

    //Cyclic dependcy check
    if(mapHelpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            PERSIST_CODE
            (
                if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                {
                    persistDesc(desc);
                }else if(desc->persistStatus == IN_PROGRESS)
                {
                    persistDesc(desc);
                }

            )             
            __sync_fetch_and_add(&g_count_abort, 1);
            __sync_fetch_and_add(&g_count_fake_abort, 1);
        }
        else 
        {
            PERSIST_CODE
            (
                if(needPersistenceHelp(desc)) 
                {
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                }
            )              
        }

        return;//return NULL;
    }

    bool ret = true;
    //td::vector<DataNode*> retVector;
    //std::vector<VALUE> foundValues;

    mapHelpStack.Push(desc);

    while(desc->status == MAP_ACTIVE && ret != false && opid < desc->size)
    {
        const Operator& op = desc->ops[opid];

        if(op.type == MAP_INSERT)
        {
            ret = Insert(desc, opid, op.key, op.value, T);
        }
        else if(op.type == MAP_DELETE)
        {
            //ret = Delete(op.key, desc, opid);
            ret = Delete(desc, opid, op.key, T);
        }
        else if(op.type == MAP_UPDATE)
        {
        	// this pointer is passed by reference to the update function
        	//DataNode* toRet;
        	ret = Update(desc, opid, op.key, op.value, T);//, toRet);
        	// the pointer is copied into the vector
        	//retVector.push_back(toRet);
        }
        else
        {
            // ret = Find(op.key, desc, opid);
            // if find is successful it returns a non-null value
            // std::cout << "before find " << op.key << std::endl;
            VALUE retVal = Find(desc, opid, op.key, T);

        	if (retVal == (VALUE)NULL)
        		ret = false;
        	else{
                // std::cout << "after find " << retVal << std::endl;
                desc->ops[opid].value = retVal;
                ret = true;
            }
        		
        	//if (retVal != 0)
        		//toR.push_back(retVal);//foundValues.push_back(retVal);
        	// TODO: edit helpops to return an array of values?
        }
        
        opid++;
    }

    mapHelpStack.Pop();

    if(ret != false)
    {
    	// // any concurrent txn will see that ours is live and not use/modify our nodes
    	// // there are now no more concurrent operations from our transaction to use/modify nodes we've previously updated
    	// // before commit update the node values if our transaction owns them via their desc aliasing that of our txn
     //    for(DataNode* x: retVector)
     //    {
     //    	// everything must have returned successfully to get here, so if the last operation was an update then we
     //    	// copy the new value in, if it was an insert we overwrite the value with itself, if it was a find
     //    	// NOTE: sometimes x->nodeDesc is NULL, should this be happening? is this a result of problems with the m_desc and m_nodedesc allocators? because x->nodedesc->desc can apparently also be null
     //    	// get rid of m_desc allocator and check for && x->nodeDesc->desc != NULL which was not hit before problem with not accessing memory arose
     //    	if (x != NULL && x->nodeDesc != NULL ) 
     //    	{
	    //     	if(x->nodeDesc->desc == desc)//&& x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE)
	    //     	{
	    //     		if (x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_UPDATE || 
	    //     			(x->nodeDesc->desc->ops[x->nodeDesc->opid].type == MAP_FIND && x->nodeDesc->desc->ops[x->nodeDesc->opid].value != 0) )//nodeDesc->value != 0) )
	    //     		{
	    //     			x->value = x->nodeDesc->desc->ops[x->nodeDesc->opid].value;//x->nodeDesc->value;
	    //     		}
	    //     	}
	    //     }
     //    }



READ_ONLY_OPT_CODE        
(
        if(desc->isReadOnly) {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }else {
            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
        }

)

NO_READ_ONLY_OPT_CODE        
(

            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            }
) 
        // else
    	// {
    	// 	printf("999999999999999999999999\n");
    	// 	exit(9999);
    	// }
        	// NOTE: if this happens it means the updates need to be undone here
        //note: i think this might just mean that some other thread committed the transaction
        return;//return foundValues;
    }
    else
    {


READ_ONLY_OPT_CODE        
(
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            __sync_fetch_and_add(&g_count_abort, 1);    
        }
    }else {
        // never updated node values, so don't have to undo those; they'll be interpreted correctly
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        }
    }

)

NO_READ_ONLY_OPT_CODE        
(
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        } 
)           
            //MarkForDeletion(insNodes, insPredNodes, desc);
        return;//return NULL;
    }
}

inline void TransMap::HelpOpsTATPBenchmark(Desc* desc, uint8_t opid, int T)//, std::vector<VALUE> &toR)
{
    if(opid > 0)
        return;
    if(desc->status != MAP_ACTIVE)
    {
        bool needHelp = false;
        PERSIST_CODE
        (
            needHelp = needPersistenceHelp(desc);
        )

        if(!needHelp)
            return;
    }

    

    //Cyclic dependcy check
    if(mapHelpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
            PERSIST_CODE
            (
                if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                {
                    persistDesc(desc);
                }else if(desc->persistStatus == IN_PROGRESS)
                {
                    persistDesc(desc);
                }

            )             
            __sync_fetch_and_add(&g_count_abort, 1);
            __sync_fetch_and_add(&g_count_fake_abort, 1);
        }
        else 
        {
            PERSIST_CODE
            (
                if(needPersistenceHelp(desc)) 
                {
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS)) 
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                }
            )              
        }

        return;//return NULL;
    }

    bool ret = true;
    //td::vector<DataNode*> retVector;
    //std::vector<VALUE> foundValues;

    mapHelpStack.Push(desc);

    const Operator& op = desc->ops[0];
    ret = Update(desc, opid, op.key, op.value, T);//, toRet);

    mapHelpStack.Pop();

    if(ret != false)
    {

            if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_COMMITTED))
            {
                //MarkForDeletion(delNodes, delPredNodes, desc);
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }
                )
                __sync_fetch_and_add(&g_count_commit, 1);
            } 
        // else
    	// {
    	// 	printf("999999999999999999999999\n");
    	// 	exit(9999);
    	// }
        	// NOTE: if this happens it means the updates need to be undone here
        //note: i think this might just mean that some other thread committed the transaction
        return;//return foundValues;
    }
    else
    {



        if(__sync_bool_compare_and_swap(&desc->status, MAP_ACTIVE, MAP_ABORTED))
        {
                PERSIST_CODE
                (
                    if(__sync_bool_compare_and_swap(&desc->persistStatus, MAYBE, IN_PROGRESS))
                    {
                        persistDesc(desc);
                    }else if(desc->persistStatus == IN_PROGRESS)
                    {
                        persistDesc(desc);
                    }

                )
                __sync_fetch_and_add(&g_count_abort, 1);     
        } 
         
            //MarkForDeletion(insNodes, insPredNodes, desc);
        return;//return NULL;
    }
}
