11,12c11
< //#include <queue>          // std::priority_queue
< #include <vector>         // std::vector
---
> #include <vector> 
17c16
< #include <stack>          // std::stack
---
> #include <stack>  
29c28
< unsigned int KEY_RANGE_; //Replace TEST_SIZE*NUM_THRDS*TRANS_SIZE
---
> unsigned int KEY_RANGE_;
30a30,32
> bool VERBOSE;
> unsigned int LINEARIZABILITY;
> unsigned int STRICT_SERIALIZABILITY;
75d76
< 	//std::stack<int> promote_items;
138d138
< 		//printf("add_num = %ld, numerator/denominator = %ld\n", add_num, numerator/denominator);
145d144
< 		//printf("sub_num = %ld, numerator/denominator = %ld\n", sub_num, numerator/denominator);
150d148
< 		//printf("num = %ld, den = %ld, numerator = %ld, denominator = %ld\n", num, den, numerator, denominator);
173,174d170
< 		//if(sum < 0)
< 			//printf("WARNING: ADD sum < 0\n");
207d202
< 		//printf("denominator = %ld\n", den);
223d217
< 		//printf("denominator = %ld\n", den);
283d276
< 		//printf("denominator = %ld\n", den);
290d282
< 		//printf("denominator = %ld\n", den);
330d321
< 		//printf("denominator = %ld\n", den);
337d327
< 		//printf("denominator = %ld\n", den);
403d392
< 		//if(it_0->second.response < it->second.invocation)
406,409c395
< 		{
< 			//std::unordered_map<int,Item>::iterator it_item_0;
< 			//it_item_0 = map_items.find(it_0->second.item_key);
< 			
---
> 		{		
432d417
< 		//if(it_0->second.response < it->second.invocation)
436,438d420
< 			//std::unordered_map<int,Item>::iterator it_item_0;
< 			//it_item_0 = map_items.find(it_0->second.item_key);
< 			
455,456d436
< 	//bool is_quiescent = true; //ADD to VectorSpace
< 
463d442
< 		//bool reset_it_start = true;
470,471c449
< 		//} else if(it != map_methods.end()) {
< 		} else if(it_start != map_methods.end()) { //ADD to VectorSpace
---
> 		} else if(it_start != map_methods.end()) {
479,502d456
< 			/*if(it->second.response > min) //ADD to VectorSpace
< 			{
< 				long int invoc[NUM_THRDS] = {};
< 				std::map<long int,Method,bool(*)(long int,long int)>::iterator q_check;
< 				int q_count = 0;
< 				for(q_check = it; q_check != map_methods.end(); ++q_check)
< 				{
< 					if(invoc[q_check->second.process] != 0)
< 					{
< 						invoc[q_check->second.process] = q_check->second.invocation;
< 						if(q_check->second.invocation < min)
< 						{
< 							is_quiescent = false;
< 							break;
< 						}
< 
< 						q_count = q_count + 1;
< 						if(q_count >= (NUM_THRDS -1))
< 							break;
< 					}
< 				}
< 
< 				break;
< 			}*/
504,506d457
< 			//if(vec_item->status != PRESENT)
< 				//printf("WARNING: Current item not present!\n");
< 
513d463
< 
519c469,470
< 			if(method_count%5000 == 0)
---
> if(VERBOSE) {
> 			if(method_count%5000 == 0 && method_count != 0)
520a472
> }
527d478
< 			//printf("it->second.item_key = %d, KEY_RANGE_ = %d, INT_MIN = %d\n", it->second.item_key, KEY_RANGE_, INT_MIN);
547d497
< 				//vec_item->producer = map_methods.end();
557,562d506
< 		
< 					/*if(vec_item->status == ABSENT)
< 					{				
< 						vec_item->status = PRESENT;
< 						vec_item->demote_methods.clear();
< 					}*/
564c508
< 					if(vec_item->status == PRESENT) //ADD to VectorSpace
---
> 					if(vec_item->status == PRESENT)
574d517
< 								//if(it_0->second.response < it->second.invocation)
589d531
< 										//vec_item_0->promote_items.push(vec_item->key);
599d540
< 										//vec_item->promote_items.push(vec_item_0->key);
622d562
< 					//TODO: Probably should handle the same as a failed consumer
628d567
< 			//if(((it->second).type == CONSUMER) && ((it->second).semantics == FIFO))
631d569
< 				//printf("Consume Item %d\n", it->second.item_key);
643,644c581
< 
< 					
---
> 				
647d583
< 						//printf("WARNING: Sum < 0\n");
651,652d586
< 							//printf("Item %d considers promoting Item %d\n", it_method->item_key, vec_item->key);
< 
656,659c590
< 								//printf("NOTE: Methods do not overlap\n");
< 							} else {
< 								//printf("NOTE: CONSUME Item %d overlaps with PRODUCE Item %d\n", it->second.item_val, it_method->item_val);
< 								
---
> 							} else {							
661d591
< 
666,667c596
< 								//Need to remove from promote list
< 								
---
> 								//Need to remove from promote list							
674d602
< 
686d613
< 
691,692c618
< 
< 					//map_methods.erase(it); //TODO: Dangerous, make sure this is correct
---
> 					//map_methods.erase(it); //TODO: Handled in !stack_finished_methods.empty() loop
694d619
< 					//printf("stack_finished_methods.push(Consume(%d))\n", it->second.item_key);
697,698c622
< 					{
< 						
---
> 					{					
700d623
< 						//printf("stack_finished_methods.push(Produce(%d))\n", vec_item->producer->second.item_key);
759d681
< 		//printf("Handling stack_finished_method\n");
763,767d684
< 			
< 			//if(methods_top->second.type == PRODUCER)
< 				//printf("Erase PRODUCER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
< 			//else if (methods_top->second.type == CONSUMER)
< 				//printf("Erase CONSUMER key %d, it_start->second.item_key = %d\n", methods_top->second.item_key, it_start->second.item_key);
782,787c699
< 				} else {
< 					/*if(methods_top->second.type == PRODUCER)
< 						printf("PRODUCER key %d, response = %ld found in method map\n", methods_top->second.item_key, methods_top->second.response);
< 					else if (methods_top->second.type == CONSUMER)
< 						printf("CONSUMER key %d, response = %ld found in method map\n", methods_top->second.item_key, methods_top->second.response);*/
< 				}
---
> 				} 
798,800c710
< 
< 					map_methods.erase(methods_top); //TODO: Dangerous, make sure this is correct, fails with the persistent verification	
< 					//printf("Erase return\n");	
---
> 					map_methods.erase(methods_top); //TODO: Dangerous, make sure this is correct, fails with the persistent verification
805d714
< 		//printf("Finished handling stack_finished_method\n");
808,815c717
< 		bool outcome = true; //ADD to VectorSpace
< 
< 		//if(is_quiescent) //ADD to VectorSpace
< 		//{
< 			Item* vec_verify;
< 			for (int i = 0; i < KEY_RANGE_+1; i++)
< 			{
< 				vec_verify = vector_items[i];
---
> 		bool outcome = true; 
817c719,722
< 				if(vec_verify == NULL) continue;
---
> 		Item* vec_verify;
> 		for (int i = 0; i < KEY_RANGE_+1; i++)
> 		{
> 			vec_verify = vector_items[i];
819,836c724
< 				if(vec_verify->sum < 0)
< 				{
< 					outcome = false;
< 	//#if DEBUG_
< 					//printf("WARNING: Item %d, sum %.2lf\n", it_verify->second.key, it_verify->second.sum);
< 					//printf("WARNING: Item %d, sum %.2lf\n", vec_verify->key, vec_verify->sum);
< 					std::pair<int,int> mypair (vec_verify->key,vec_verify->key);
< 					incorrect_methods.insert(mypair);
< 					/*printf("Demote list:\n");
< 					std::list<Method>::iterator it_m;
< 					for(it_m = vec_verify->demote_methods.begin(); it_m != vec_verify->demote_methods.end(); ++it_m)
< 					{
< 						printf("%d, Promote List: ", it_m->item_key);
< 						Item* vec_promote_item;
< 						if(it_m->item_key != INT_MIN)
< 							vec_promote_item = vector_items[it_m->item_key];
< 						else
< 							vec_promote_item = vector_items[KEY_RANGE_];
---
> 			if(vec_verify == NULL) continue;
838,845c726,728
< 						std::list<int>::iterator it_item1;
< 						for(it_item1 = vec_promote_item->promote_items.begin(); it_item1 != vec_promote_item->promote_items.end(); ++it_item1)
< 						{
< 							printf("%d ", *it_item1);
< 						}
< 						printf("\n");
< 					}
< 					printf("\n");	*/	
---
> 			if(vec_verify->sum < 0)
> 			{
> 				outcome = false;
847,849c730,742
< 	//#endif
< 				}
< 				//printf("Item %d, sum %.2lf\n", it_verify->second.key, it_verify->second.sum);
---
> 				std::pair<int,int> mypair (vec_verify->key,vec_verify->key);
> 				incorrect_methods.insert(mypair);
> //#if DEBUG_
> 				/*printf("Demote list:\n");
> 				std::list<Method>::iterator it_m;
> 				for(it_m = vec_verify->demote_methods.begin(); it_m != vec_verify->demote_methods.end(); ++it_m)
> 				{
> 					printf("%d, Promote List: ", it_m->item_key);
> 					Item* vec_promote_item;
> 					if(it_m->item_key != INT_MIN)
> 						vec_promote_item = vector_items[it_m->item_key];
> 					else
> 						vec_promote_item = vector_items[KEY_RANGE_];
851,857c744,749
< 				if((ceil(vec_verify->sum) + vec_verify->sum_r) < 0)
< 				{
< 					outcome = false;
< 	#if DEBUG_
< 					//printf("WARNING: Item %d, sum_r %.2lf\n", it_verify->second.key, it_verify->second.sum_r);
< 					printf("WARNING: Item %d, sum_r %.2lf\n", vec_verify->key, vec_verify->sum_r);
< 	#endif
---
> 					std::list<int>::iterator it_item1;
> 					for(it_item1 = vec_promote_item->promote_items.begin(); it_item1 != vec_promote_item->promote_items.end(); ++it_item1)
> 					{
> 						printf("%d ", *it_item1);
> 					}
> 					printf("\n");
858a751,753
> 				printf("\n");	*/	
> //#endif
> 			}
860,873c755,764
< 				int N;
< 				if(vec_verify->sum_f == 0)
< 					N = 0;
< 				else
< 					N = -1;
< 
< 				if(((ceil(vec_verify->sum) + vec_verify->sum_f) * N) < 0)
< 				{
< 					outcome = false;
< 	#if DEBUG_
< 					//printf("WARNING: Item %d, sum_f %.2lf\n", it_verify->second.key, it_verify->second.sum_f);
< 					printf("WARNING: Item %d, sum_f %.2lf\n", vec_verify->key, vec_verify->sum_f);
< 	#endif
< 				}
---
> 			if((ceil(vec_verify->sum) + vec_verify->sum_r) < 0)
> 			{
> 				outcome = false;
> if(VERBOSE) {
> 			if(LINEARIZABILITY)
> 				printf("WARNING: Non-Linearizable Read, Item %d, sum_r %.2lf\n", vec_verify->key, vec_verify->sum_r);
> 			else if(STRICT_SERIALIZABILITY)
> 				printf("WARNING: Non-Serializable Read, Item %d, sum_r %.2lf\n", vec_verify->key, vec_verify->sum_r);
> }
> 			}
874a766,770
> 			int N;
> 			if(vec_verify->sum_f == 0)
> 				N = 0;
> 			else
> 				N = -1;
875a772,780
> 			if(((ceil(vec_verify->sum) + vec_verify->sum_f) * N) < 0)
> 			{
> 				outcome = false;
> if(VERBOSE) {
> 			if(LINEARIZABILITY)
> 				printf("WARNING: Non-Linearizable Failed Method Call, Item %d, sum_f %.2lf\n", vec_verify->key, vec_verify->sum_f);
> 			else if(STRICT_SERIALIZABILITY)
> 				printf("WARNING: Non-Serializable Failed Method Call, Item %d, sum_f %.2lf\n", vec_verify->key, vec_verify->sum_f);
> }
877c782
< 		//}
---
> 		}
882,884c787,792
< #if DEBUG_
< 			printf("-------------Program Correct Up To This Point-------------\n");
< #endif
---
> if(VERBOSE) {
> 		if(LINEARIZABILITY)
> 			printf("-------------Execution Linearizable Up To This Time Step------\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Execution Serializable Up To This Time Step------\n");
> }
888,890c796,801
< #if DEBUG_
< 			printf("-------------Program Not Correct-------------\n");
< #endif
---
> if(VERBOSE) {
> 		if(LINEARIZABILITY)
> 			printf("-------------Execution Not Linearizable Up To This Time Step--\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Execution Not Serializable Up To This Time Step--\n");
> }
892,893d802
< 		
< 
899a809
> 	std::stack<int> incorrect_items;
904,905c814,818
< 			//printf("vector_items1[%d]->sum = %.2lf\n", i, vector_items1[i]->sum);
< 			if(vector_items1[i]->sum != 0)
---
> if(VERBOSE) {
> 			printf("Items[%d]->sum = %.2lf\n", i, vector_items1[i]->sum);
> }
> 			if(vector_items1[i]->sum != 0) 
> 			{
906a820,821
> 				incorrect_items.push(i);
> 			}
909c824,826
< 			//printf("vector_items2[%d]->sum = %.2lf\n", i, vector_items2[i]->sum);
---
> if(VERBOSE) {
> 			printf("Items_Durable[%d]->sum = %.2lf\n", i, vector_items2[i]->sum);
> }
911c828,831
< 				*outcome_compare = false;	
---
> 			{
> 				*outcome_compare = false;
> 				incorrect_items.push(i);	
> 			}
914c834,836
< 			//printf("vector_items1[%d]->sum = %.2lf, vector_items2[%d]->sum = %.2lf\n", i, vector_items1[i]->sum, i, vector_items2[i]->sum);
---
> if(VERBOSE) {
> 			printf("Items[%d]->sum = %.2lf, Items_Durable[%d]->sum = %.2lf\n", i, vector_items1[i]->sum, i, vector_items2[i]->sum);
> }
915a838
> 			{
916a840,845
> 				incorrect_items.push(i);
> 			}
> 		} else if(vector_items1[i] == NULL && vector_items2[i] == NULL) {
> if(VERBOSE) {
> 			printf("Items[%d]->sum = 0.00, Items_Durable[%d]->sum = 0.00\n", i, i);
> }
918a848,855
> if(VERBOSE) {
> 	while(!incorrect_items.empty())
> 	{
> 		int top = incorrect_items.top();
> 			
> 		printf("WARNING: NVM not consistent with cache-side Item %d\n", top);
> 		incorrect_items.pop();
> 	}
919a857
> } //end compare_vectors
943,947d880
< 			//if(m.item_key%500 == 0)
< 				//printf("Checking method %d\n", m.item_key);
< 
< 			//pq_methods.push(m);
< 
971,972d903
< 
< 			//printf("Insert method %d\n", m.id);
976,984d906
< 			//Item* item = new Item(m.item_key);
< 			//item->producer = map_methods.end();
< 			////map_items.insert(std::pair<int,Item>(m.item_key,item) );
< 			//if(m.item_key != INT_MIN)
< 				//vector_items[m.item_key] = item;
< 			//else
< 				//vector_items[KEY_RANGE_] = item;
< 			////it_item = map_items.find(m.item_key);
< 
1008d929
< 	//std::priority_queue<Method,std::vector<Method>,Comparator> pq_methods;
1013,1014d933
< 	//std::unordered_map<int,Item> map_items;
< 	//Item* vector_items[KEY_RANGE_+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
1016,1017d934
< 	//std::vector<Item*> vector_items (KEY_RANGE_+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
< 	//Item* vector_items_persist[KEY_RANGE_+1] = {}; //Plus 1 to handle case of key = INT_MIN (For failed consumers)
1019d935
< 	//std::vector<Item*> vector_items_persist (KEY_RANGE_+1,0); //Plus 1 to handle case of key = INT_MIN (For failed consumers)
1021,1022c937,938
< 	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start = map_methods.end(); //ADDED RECENTLY
< 	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start_persist = map_methods_persist.end(); //ADDED RECENTLY
---
> 	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start = map_methods.end(); 
> 	std::map<long int,Method,bool(*)(long int,long int)>::iterator it_start_persist = map_methods_persist.end();
1027d942
< 	//int* it_count = new int[NUM_THRDS];
1029d943
< 	//int* it_count_persist = new int[NUM_THRDS];
1055d968
< 		//printf("VERIFY CHECKPOINT\n");
1057d969
< 		//printf("VERIFY CHECKPOINT PERSIST\n");
1060,1061c972
< 		//compare_vectors(vector_items, vector_items_persist, outcome_compare);
< 
---
> 		//compare_vectors(vector_items, vector_items_persist, outcome_compare); //Compare vectors at every time step
1065d975
< 	//printf("VERIFY CHECKPOINT\n");
1067d976
< 	//printf("VERIFY CHECKPOINT PERSIST\n");
1072d980
< 	//printf("VERIFY CHECKPOINT\n");
1074d981
< 	//printf("VERIFY CHECKPOINT PERSIST\n");
1083,1096d989
< 
< 
< 	/*std::map<long int,Method,bool(*)(long int,long int)>::iterator it_;
< 	for (it_=map_methods.begin(); it_!=map_methods.end(); ++it_)
< 	{
< 		std::unordered_map<int,Item>::iterator it_item;
< 		it_item = map_items.find(it_->second.item_key);
< 		if(it_->second.type == PRODUCER)
< 			printf("PRODUCER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
< 		else if ((it_->second).type == CONSUMER)
< 			printf("CONSUMER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
< 		else if ((it_->second).type == READER)
< 			printf("READER inv %ld, res %ld, item %d, sum %.2lf, sum_r = %.2lf, sum_f = %.2lf, tid = %d\n", it_->second.invocation, it_->second.response, it_->second.item_key, it_item->second.sum, it_item->second.sum_r, it_item->second.sum_f, it_->second.process);
< 	}*/
1376c1269
< 	printf("VSV_Init()\n");
---
> 	//printf("VSV_Init()\n");
1413c1306
< 	printf("VSV_Init() Finished\n");
---
> 	//printf("VSV_Init() Finished\n");
1430c1323,1326
< 		printf("-------------Program Correct Up To This Point-------------\n");
---
> 		if(LINEARIZABILITY)
> 			printf("-------------Execution Linearizable---------------------------\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Execution Serializable---------------------------\n");
1432c1328,1331
< 		printf("-------------Program Not Correct-------------\n");
---
> 		if(LINEARIZABILITY)
> 			printf("-------------Execution Not Linearizable-----------------------\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Execution Not Serializable-----------------------\n");
1437c1336,1339
< 		printf("-------------Persisted Program Correct Up To This Point-------------\n");
---
> 		if(LINEARIZABILITY)
> 			printf("-------------Persisted Execution Linearizable-----------------\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Persisted Execution Serializable-----------------\n");
1439c1341,1344
< 		printf("-------------Persisted Program Not Correct-------------\n");
---
> 		if(LINEARIZABILITY)
> 			printf("-------------Persisted Execution Not Linearizable-------------\n");
> 		else if(STRICT_SERIALIZABILITY)
> 			printf("-------------Persisted Execution Not Serializable-------------\n");
1444c1349,1352
< 		printf("VECTOR SUMS EQUAL\n");
---
> 		if(final_outcome && final_outcome_persist)
> 			printf("VECTOR SUMS EQUAL => Execution Durable Linearizable\n");
> 		else
> 			printf("VECTOR SUMS EQUAL\n");
1446c1354
< 		printf("VECTOR SUMS NOT EQUAL\n");
---
> 		printf("VECTOR SUMS NOT EQUAL => Execution Not Durable Linearizable\n");
1454c1362
< 	printf("Total Time: %.15lf seconds\n", elapsed_time_double);
---
> 	printf("Verification Time: %.15lf seconds\n", elapsed_time_double);
1464c1372
< 	printf("Total Method Time: %.15lf seconds\n", elapsed_time_method_double);
---
> 	//printf("Total Method Time: %.15lf seconds\n", elapsed_time_method_double);
1469c1377
< 	printf("Total Verification Time: %.15lf seconds\n", elapsed_time_verify_double);
---
> 	//printf("Total Verification Time: %.15lf seconds\n", elapsed_time_verify_double);
1472c1380
< 	fprintf(pfile, "%.15lf\n", elapsed_time_double);
---
> 	fprintf(pfile, "%.15lf ", elapsed_time_double);
1557a1466,1486
> 	} else {
> 		
> 		if((got->second.item_key == _item_key) && (got->second.type == PRODUCER && _type == CONSUMER))
> 		{
> 			//Producer followed by Consumer eliminate each other
> 			persist_map[_process].erase(got);
> 		} else if ((got->second.item_key == _item_key) && (got->second.type == CONSUMER && _type == PRODUCER))
> 		{
> 			//Item exists in the persisted state
> 			persist_map[_process].erase(got);
> 			Method m1_persist(-1, _process, _item_key, _item_val, _semantics, _type, 0, 0, true);
> 			std::pair<void*,Method> pair (ptr,m1_persist);
> 			persist_map[_process].insert(pair);
> 		}
> 		/*else if((got->second.item_key == _item_key) && (got->second.type == PRODUCER && _type == WRITER)) {
> 			//Writer overwrite Producer
> 			persist_map[_process].erase(got);
> 			Method m1_persist(-1, _process, _item_key, _item_val, _semantics, _type, 0, 0, true);
> 			std::pair<void*,Method> pair (ptr,m1_persist);
> 			persist_map[_process].insert(pair);
> 		}*/
1575a1505,1506
> 	} else {
> 		//printf("Thread %d: handle_PWB did not find Node %p\n", _process, ptr);
1578a1510
> 
1580c1512
< #if STRICT_SERIALIZABILITY
---
> 
1820,1821d1751
< #endif
< 
1873a1804,1809
> 	VERBOSE = false;
> 	LINEARIZABILITY = 1;
> 	STRICT_SERIALIZABILITY = 0;
> 
> 	char vflag[] = "-v";
> 
1881a1818,1832
> 	if(argc > 8) {
> 		if(strcmp (vflag,argv[8]) == 0)
> 		{
> 			VERBOSE = true;
> 			//printf("VERBOSE flag toggled!\n");
> 		} else {
> 			//printf("vflag = %s, argv[8] = %s\n", vflag, argv[8]);
> 		}
> 		
> 	}
> 
> 	if(TRANS_SIZE > 1) {
> 		LINEARIZABILITY = 0;
> 		STRICT_SERIALIZABILITY = 1;
> 	}
1883c1834
< 	printf("NUM_THRDS = %d, TEST_SIZE = %d, TRANS_SIZE = %d, KEY_RANGE_ = %d\n", NUM_THRDS, TEST_SIZE, TRANS_SIZE, KEY_RANGE_);
---
> 	//printf("NUM_THRDS = %d, TEST_SIZE = %d, TRANS_SIZE = %d, KEY_RANGE_ = %d\n", NUM_THRDS, TEST_SIZE, TRANS_SIZE, KEY_RANGE_);
