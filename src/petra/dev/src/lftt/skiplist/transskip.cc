/******************************************************************************
 * skip_cas.c
 * 
 * Skip lists, allowing concurrent update by use of CAS primitives. 
 * 
 * Copyright (c) 2001-2003, K A Fraser
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define __SET_IMPLEMENTATION__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
extern "C"
{
#include "../../common/fraser/portable_defns.h"
#include "../../common/fraser/ptst.h"
}
#include "transskip.h"
#include "../../durabletxn/dtx.h"


#define SET_MARK(_p)    ((node_t *)(((uintptr_t)(_p)) | 1))
#define CLR_MARKD(_p)    ((NodeDesc *)(((uintptr_t)(_p)) & ~1))
#define IS_MARKED(_p)     (((uintptr_t)(_p)) & 1)

struct HelpStack
{
    void Init()
    {
        index = 0;
    }

    void Push(Desc* desc)
    {
        ASSERT(index < 255, "index out of range");

        helps[index++] = desc;
    }

    void Pop()
    {
        ASSERT(index > 0, "nothing to pop");

        index--;
    }

    bool Contain(Desc* desc)
    {
        for(uint8_t i = 0; i < index; i++)
        {
            if(helps[i] == desc)
            {
                return true;
            }
        }

        return false;
    }

    Desc* helps[256];
    uint8_t index;
};

static __thread HelpStack helpStack;

enum OpStatus
{
    LIVE = 0,
    COMMITTED,
    ABORTED
};

enum OpType
{
    FIND = 0,
    INSERT,
    DELETE
};

enum PersistStatus
{
    MAYBE = 0,
    IN_PROGRESS,
    PERSISTED,
};    

static int gc_id[NUM_LEVELS];

static uint32_t g_count_commit = 0;
static uint32_t g_count_abort = 0;
static uint32_t g_count_fake_abort = 0;

static std::atomic<uint32_t> lastTxId;

/*
 * PRIVATE FUNCTIONS
 */

static bool help_ops(trans_skip* l, Desc* desc, uint8_t opid);
static bool help_ops_rec(trans_skip* l, Desc* desc, uint8_t opid);

PERSIST_CODE
(

inline bool needPersistenceHelp(Desc* desc)
{
    return desc->persistStatus != PERSISTED;
}

inline void persistDesc(Desc* desc)
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

static inline bool FinishPendingTxn(trans_skip* l, NodeDesc* nodeDesc, Desc* desc)
{
    // The node accessed by the operations in same transaction is always active 
    if(nodeDesc->desc == desc)
    {
        return true;
    }


    bool needHelp = false;

    if(desc->status != LIVE)
    {
        
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
            return true;
    } else {
        //help_ops(l, nodeDesc->desc, nodeDesc->opid + 1);
		help_ops_rec(l, nodeDesc->desc, nodeDesc->opid + 1); //CORRECTNESS ANNOTATIONS
    }   


    return true;
}

static inline bool IsNodeActive(NodeDesc* nodeDesc)
{

    bool ret = (nodeDesc->desc->status == COMMITTED);

    PERSIST_CODE
    (
        ret = ret && (nodeDesc->desc->persistStatus == PERSISTED);
    )

    return ret;
}

static inline bool IsKeyExist(NodeDesc* nodeDesc)
{
    bool isNodeActive = IsNodeActive(nodeDesc);
    uint8_t opType = nodeDesc->desc->ops[nodeDesc->opid].type;

    return (opType == FIND) || (isNodeActive && opType == INSERT) || (!isNodeActive && opType == DELETE);
}

static inline bool IsNodeActive_mod(NodeDesc* nodeDesc, Desc* desc_this)
{

    bool ret = (nodeDesc->desc->status == COMMITTED);

    PERSIST_CODE
    (
        ret = ret && (nodeDesc->desc->persistStatus == PERSISTED);
    )

	//BUG FIX
	ret = ret || (nodeDesc->desc == desc_this);

    return ret;
}

static inline bool IsKeyExist_mod(NodeDesc* nodeDesc, Desc* desc_this)
{
    bool isNodeActive = IsNodeActive_mod(nodeDesc, desc_this);
    uint8_t opType = nodeDesc->desc->ops[nodeDesc->opid].type;

    return (opType == FIND) || (isNodeActive && opType == INSERT) || (!isNodeActive && opType == DELETE);
}

static inline bool IsSameOperation(NodeDesc* nodeDesc1, NodeDesc* nodeDesc2)
{
    return nodeDesc1->desc == nodeDesc2->desc && nodeDesc1->opid == nodeDesc2->opid;
}


/*
 * Random level generator. Drop-off rate is 0.5 per level.
 * Returns value 1 <= level <= NUM_LEVELS.
 */
static int get_level(ptst_t *ptst)
{
    unsigned long r = rand_next(ptst);
    int l = 1;
    r = (r >> 4) & ((1 << (NUM_LEVELS-1)) - 1);
    while ( (r & 1) ) { l++; r >>= 1; }
    return(l);
}


/*
 * Allocate a new node, and initialise its @level field.
 * NB. Initialisation will eventually be pushed into garbage collector,
 * because of dependent read reordering.
 */
static node_t *alloc_node(ptst_t *ptst)
{

    

    int l;
    node_t *n;
    l = get_level(ptst);
    n = (node_t*)fr_gc_alloc(ptst, gc_id[l - 1]);
    n->level = l;

    return(n);
}


/* Free a node to the garbage collector. */
// static void free_node(ptst_t *ptst, node_t* n)
// {
//     fr_gc_free(ptst, (void *)n, gc_id[(n->level & LEVEL_MASK) - 1]);
// }


/*
 * Search for first non-deleted node, N, with key >= @k at each level in @l.
 * RETURN VALUES:
 *  Array @pa: @pa[i] is non-deleted predecessor of N at level i
 *  Array @na: @na[i] is N itself, which should be pointed at by @pa[i]
 *  MAIN RETURN VALUE: same as @na[0].
 */
static node_t* strong_search_predecessors(trans_skip *l, setkey_t k, node_t* *pa, node_t* *na)
{
    // printf("strong_search_predecessors begins!\n\r");
    node_t* x, *x_next, *old_x_next, *y, *y_next;
    setkey_t  y_k;
    int        i;

 retry:
    RMB();

    x = &l->head;
    for ( i = NUM_LEVELS - 1; i >= 0; i-- )
    {
        // printf("strong_search_predecessors outer loop %d!\n\r", i);
        /* We start our search at previous level's unmarked predecessor. */
        READ_FIELD(x_next, x->next[i]);
        /* If this pointer's marked, so is @pa[i+1]. May as well retry. */
        if ( is_marked_ref(x_next) ) goto retry;

        for ( y = x_next; ; y = y_next )
        {
            // printf("strong_search_predecessors inner loop %p \n\r", y);
            /* Shift over a sequence of marked nodes. */
            for ( ; ; )
            {
                // printf("strong_search_predecessors inner loop2 %p %p %p i=%d \n\r", y_next, y, y->next, i);
                // printf("y->next[%d]=%p\n\r", i, y->next[i]);
                READ_FIELD(y_next, y->next[i]);
                // printf("strong_search_predecessors inner loop2 y_next=%p \n\r", y_next);
                if ( !is_marked_ref(y_next) ) break;
                y = (node_t*)get_unmarked_ref(y_next);
            }

            READ_FIELD(y_k, y->k);
            // printf("strong_search_predecessors inner loop y_k=%d \n\r", y_k);
            if ( y_k >= k ) break;

            /* Update estimate of predecessor at this level. */
            x      = y;
            x_next = y_next;
        }

        // printf("strong_search_predecessors outer loop after inner loop \n\r");

        /* Swing forward pointer over any marked nodes. */
        if ( x_next != y )
        {
            old_x_next = CASPO(&x->next[i], x_next, y);
            // printf("strong_search_predecessors outer loop after CASPO \n\r");
            if ( old_x_next != x_next ) goto retry;
        }

        if ( pa ) pa[i] = x;
        if ( na ) na[i] = y;
    }

    // printf("strong_search_predecessors finishes!\n\r");
    return(y);
}


/* This function does not remove marked nodes. Use it optimistically. */
static node_t* weak_search_predecessors(trans_skip *l, setkey_t k, node_t* *pa, node_t* *na)
{
    // printf("weak_search_predecessors begins!\n\r");
    node_t* x, *x_next;
    setkey_t  x_next_k;
    int        i;

    x = &l->head;
    for ( i = NUM_LEVELS - 1; i >= 0; i-- )
    {
        // printf("weak_search_predecessors outer loop %d!\n\r", i);
        for ( ; ; )
        {
            // printf("weak_search_predecessors inner loop!\n\r");
            READ_FIELD(x_next, x->next[i]);
            // printf("weak_search_predecessors inner loop after READ_FIELD! x_next=%p\n\r", x_next);
            x_next = (node_t*)get_unmarked_ref(x_next);
            // printf("weak_search_predecessors inner loop after get_unmarked_ref x_next_k=%ul , x_next=%p, x_next->k=%ul!\n\r", x_next_k, x_next, x_next->k);

            READ_FIELD(x_next_k, x_next->k);
            // printf("weak_search_predecessors inner loop after READ_FIELD 2!\n\r");
            if ( x_next_k >= k ) break;

            x = x_next;
        }

        if ( pa ) pa[i] = x;
        if ( na ) na[i] = x_next;
    }
    // printf("weak_search_predecessors finishes!\n\r");

    return(x_next);
}


/*
 * Mark @x deleted at every level in its list from @level down to level 1.
 * When all forward pointers are marked, node is effectively deleted.
 * Future searches will properly remove node by swinging predecessors'
 * forward pointers.
 */
static void mark_deleted(node_t* x, int level)
{
    node_t* x_next;

    while ( --level >= 0 )
    {
        x_next = x->next[level];
        while ( !is_marked_ref(x_next) )
        {
            x_next = CASPO(&x->next[level], x_next, get_marked_ref(x_next));
        }
        WEAK_DEP_ORDER_WMB(); /* mark in order */
    }
}


static int check_for_full_delete(node_t* x)
{
    int level = x->level;
    return ((level & READY_FOR_FREE) ||
            (CASIO(&x->level, level, level | READY_FOR_FREE) != level));
}


static void do_full_delete(ptst_t *ptst, trans_skip *l, node_t* x, int level)
{
    int k = x->k;
#ifdef WEAK_MEM_ORDER
    node_t* preds[NUM_LEVELS];
    int i = level;
 retry:
    (void)strong_search_predecessors(l, k, preds, NULL);
    /*
     * Above level 1, references to @x can disappear if a node is inserted
     * immediately before and we see an old value for its forward pointer. This
     * is a conservative way of checking for that situation.
     */
    if ( i > 0 ) RMB();
    while ( i > 0 )
    {
        node_t *n = get_unmarked_ref(preds[i]->next[i]);
        while ( n->k < k )
        {
            n = get_unmarked_ref(n->next[i]);
            RMB(); /* we don't want refs to @x to "disappear" */
        }
        if ( n == x ) goto retry;
        i--; /* don't need to check this level again, even if we retry. */
    }
#else
    (void)strong_search_predecessors(l, k, NULL, NULL);
#endif
    // free_node(ptst, x);
}


/*
 * PUBLIC FUNCTIONS
 */

trans_skip *transskip_alloc(Allocator<Desc>* _descAllocator, Allocator<NodeDesc>* _nodeDescAllocator, Allocator<node_t>* _nodeAllocator)
{
    trans_skip *l;
    node_t *n;
    int i;

    n = (node_t*)malloc(sizeof(*n) + (NUM_LEVELS-1)*sizeof(node_t *));
    memset(n, 0, sizeof(*n) + (NUM_LEVELS-1)*sizeof(node_t *));
    n->k = SENTINEL_KEYMAX;

    /*
     * Set the forward pointers of final node to other than NULL,
     * otherwise READ_FIELD() will continually execute costly barriers.
     * Note use of 0xfe -- that doesn't look like a marked value!
     */
    memset(n->next, 0xfe, NUM_LEVELS*sizeof(node_t *));

    l = (trans_skip*)malloc(sizeof(*l) + (NUM_LEVELS-1)*sizeof(node_t *));
    l->head.k = SENTINEL_KEYMIN;
    l->head.level = NUM_LEVELS;
    for ( i = 0; i < NUM_LEVELS; i++ )
    {
        l->head.next[i] = n;
    }

    l->tail = n;

    l->descAllocator = _descAllocator;
    l->nodeDescAllocator = _nodeDescAllocator;
    l->nodeAllocator = _nodeAllocator;
    lastTxId.store(0);

    return(l);
}


bool transskip_insert(trans_skip *l, setkey_t k, Desc* desc, uint8_t opid, node_t*& n)
{
    // printf("transskip_insert begins! key=%d\n\r", k);
    n = NULL;
    bool ret = false;
    NodeDesc* nodeDesc = l->nodeDescAllocator->Alloc();
    nodeDesc->desc = desc;
    nodeDesc->opid = opid;

    ptst_t    *ptst;
    node_t* preds[NUM_LEVELS], *succs[NUM_LEVELS];
    node_t* pred, *succ, *new_node = NULL, *new_next, *old_next;
    int        i, level;

    k = CALLER_TO_INTERNAL_KEY(k);
    // printf("transskip_insert after CALLER_TO_INTERNAL_KEY!\n\r");

    ptst = fr_critical_enter();
    // printf("transskip_insert after fr_critical_enter!\n\r");

    succ = weak_search_predecessors(l, k, preds, succs);
    // printf("transskip_insert after weak_search_predecessors!\n\r");
    
 retry:
    // printf("transskip_insert retry loop begins!\n\r");
    if ( succ->k == k )
    {
        NodeDesc* oldCurrDesc = succ->nodeDesc;

        if(IS_MARKED(oldCurrDesc))
        {
            // printf("transskip_insert IS_MARKED if begins!\n\r");
            READ_FIELD(level, succ->level);
            // printf("transskip_insert IS_MARKED if after READ_FIELD!\n\r");
            mark_deleted(succ, level & LEVEL_MASK);
            // printf("transskip_insert IS_MARKED if after mark_deleted!\n\r");
            succ = strong_search_predecessors(l, k, preds, succs);
            // printf("transskip_insert IS_MARKED if finishes!\n\r");
            goto retry;
        }

        if(!FinishPendingTxn(l, oldCurrDesc, desc))
        {
            // printf("transskip_insert after FinishPendingTxn if!\n\r");
            // if ( new_node != NULL ) free_node(ptst, new_node);
            ret = false;
            goto out;
        }

        if(IsSameOperation(oldCurrDesc, nodeDesc))
        {
            // printf("transskip_insert after IsSameOperation if!\n\r");
            ret = true;
            goto out;
        }

        //if(!IsKeyExist(oldCurrDesc))
		if(!IsKeyExist_mod(oldCurrDesc, desc))
        {
            // printf("transskip_insert after IsKeyExist if!\n\r");
            NodeDesc* currDesc = succ->nodeDesc;

            if(desc->status != LIVE)
            {
                // if ( new_node != NULL ) free_node(ptst, new_node);
                ret = false;
                goto out;
            }

            //if(currDesc == oldCurrDesc)
            {
                //Update desc 
                currDesc = __sync_val_compare_and_swap(&succ->nodeDesc, oldCurrDesc, nodeDesc);

                if(currDesc == oldCurrDesc)
                {
                    // if ( new_node != NULL ) free_node(ptst, new_node);
                    n = succ;
                    ret = true;
                    // printf("transskip_insert was successsful!\n\r");
                    goto out;
                }
            }

            goto retry;
        }
        else
        {
            // if ( new_node != NULL ) free_node(ptst, new_node);
            ret = false;
            goto out;
        }
    }

#ifdef WEAK_MEM_ORDER
    /* Free node from previous attempt, if this is a retry. */
    if ( new_node != NULL ) 
    { 
        free_node(ptst, new_node);
        new_node = NULL;
    }
#endif

    /* Not in the list, so initialise a new node for insertion. */
    if ( new_node == NULL )
    {
        // printf("transskip_insert initing new node!\n\r");
        new_node    = l->nodeAllocator->Alloc();//alloc_node(ptst);
        new_node->level = get_level(ptst);
        new_node->k = k;
        new_node->v = (void*)0xf0f0f0f0;
        new_node->nodeDesc = nodeDesc;
        // new_node->next = l->nodeAllocator->Alloc();
    }
    level = new_node->level;

    /* If successors don't change, this saves us some CAS operations. */
    for ( i = 0; i < level; i++ )
    {
        new_node->next[i] = succs[i];
    }
    // printf("transskip_insert after assigning next!\n\r");

    /* We've committed when we've inserted at level 1. */
    WMB_NEAR_CAS(); /* make sure node fully initialised before inserting */
    // printf("transskip_insert after WMB_NEAR_CAS!\n\r");

    if(desc->status != LIVE)
    {
        ret = false;
        goto out;
    }
    // printf("transskip_insert after checking the status!\n\r");

    old_next = CASPO(&preds[0]->next[0], succ, new_node);
    // printf("transskip_insert after CASPO!\n\r");
    if ( old_next != succ )
    {
        succ = strong_search_predecessors(l, k, preds, succs);
        // printf("transskip_insert after strong_search_predecessors!\n\r");
        goto retry;
    }

    /* Insert at each of the other levels in turn. */
    i = 1;
    while ( i < level )
    {
        // printf("transskip_insert insert new node at level %d!\n\r", i);
        pred = preds[i];
        succ = succs[i];

        /* Someone *can* delete @new under our feet! */
        new_next = new_node->next[i];
        if ( is_marked_ref(new_next) ) goto success;

        /* Ensure forward pointer of new node is up to date. */
        if ( new_next != succ )
        {
            old_next = CASPO(&new_node->next[i], new_next, succ);
            if ( is_marked_ref(old_next) ) goto success;
            assert(old_next == new_next);
        }

        /* Ensure we have unique key values at every level. */
        if ( succ->k == k ) goto new_world_view;
        assert((pred->k < k) && (succ->k > k));

        /* Replumb predecessor's forward pointer. */
        old_next = CASPO(&pred->next[i], succ, new_node);
        if ( old_next != succ )
        {
        new_world_view:
            RMB(); /* get up-to-date view of the world. */
            (void)strong_search_predecessors(l, k, preds, succs);
            continue;
        }

        /* Succeeded at this level. */
        i++;
    }

 success:
    /* Ensure node is visible at all levels before punting deletion. */
    WEAK_DEP_ORDER_WMB();
    if ( check_for_full_delete(new_node) ) 
    {
        MB(); /* make sure we see all marks in @new. */
        do_full_delete(ptst, l, new_node, level - 1);
    }

    n = new_node;
    ret = true;
    // printf("transskip_insert finishes!\n\r");

 out:
    fr_critical_exit(ptst);
    return ret;
}

bool transskip_delete(trans_skip *l, setkey_t k, Desc* desc, uint8_t opid, node_t*& n)
{
    // printf("transskip_delete begins!\n\r");
    n = NULL;
    bool ret = false;
    NodeDesc* nodeDesc = NULL;

    ptst_t    *ptst;
    node_t    *succ;

    k = CALLER_TO_INTERNAL_KEY(k);

    ptst = fr_critical_enter();

    succ = weak_search_predecessors(l, k, NULL, NULL);
    
 retry:

    if ( succ->k == k )
    {
        NodeDesc* oldCurrDesc = succ->nodeDesc;

        if(IS_MARKED(oldCurrDesc))
        {
            ret = false;
            goto out;
            //READ_FIELD(level, succ->level);
            //mark_deleted(succ, level & LEVEL_MASK);
            //succ = strong_search_predecessors(l, k, preds, succs);
            //goto retry;
        }

        if(!FinishPendingTxn(l, oldCurrDesc, desc))
        {
            ret = false;
            goto out;
        }

        if(nodeDesc == NULL)
        {
            nodeDesc = l->nodeDescAllocator->Alloc();
            nodeDesc->desc = desc;
            nodeDesc->opid = opid;
        }

        if(IsSameOperation(oldCurrDesc, nodeDesc))
        {
            ret = true;
            goto out;
        }

        //if(IsKeyExist(oldCurrDesc))
		if(IsKeyExist_mod(oldCurrDesc, desc))
        {
            NodeDesc* currDesc = succ->nodeDesc;

            if(desc->status != LIVE)
            {
                ret = false;
                goto out;
            }

            //if(currDesc == oldCurrDesc)
            {
                //Update desc 
                currDesc = __sync_val_compare_and_swap(&succ->nodeDesc, oldCurrDesc, nodeDesc);

                if(currDesc == oldCurrDesc)
                {
                    n = succ;
                    ret = true;
                    goto out;
                }
            }

            goto retry;
        }
        else
        {
            ret = false;
            goto out;
        }
    }
    else
    {
        ret = false;
        goto out;
    }

out:
    fr_critical_exit(ptst);
    // printf("transskip_delete finishes!\n\r");
    return ret;
}

setval_t transskip_delete_org(trans_skip *l, setkey_t k)
{
    setval_t  v = NULL;
    ptst_t    *ptst;
    node_t* preds[NUM_LEVELS], *x;
    int        level, i;

    //k = CALLER_TO_INTERNAL_KEY(k);

    ptst = fr_critical_enter();

    x = weak_search_predecessors(l, k, preds, NULL);

    if ( x->k > k ) goto out;

    READ_FIELD(level, x->level);
    level = level & LEVEL_MASK;

    /* Once we've marked the value field, the node is effectively deleted. */
    //new_v = x->v;
    //do {
        //v = new_v;
        //if ( v == NULL ) goto out;
    //}
    //while ( (new_v = CASPO(&x->v, v, NULL)) != v );

    /* Committed to @x: mark lower-level forward pointers. */
    WEAK_DEP_ORDER_WMB(); /* enforce above as linearisation point */
    mark_deleted(x, level);

    /*
     * We must swing predecessors' pointers, or we can end up with
     * an unbounded number of marked but not fully deleted nodes.
     * Doing this creates a bound equal to number of threads in the system.
     * Furthermore, we can't legitimately call 'free_node' until all shared
     * references are gone.
     */
    for ( i = level - 1; i >= 0; i-- )
    {
        if ( CASPO(&preds[i]->next[i], x, get_unmarked_ref(x->next[i])) != x )
        {
            if ( (i != (level - 1)) || check_for_full_delete(x) )
            {
                MB(); /* make sure we see node at all levels. */
                do_full_delete(ptst, l, x, i);
            }
            goto out;
        }
    }

    // free_node(ptst, x);

 out:
    fr_critical_exit(ptst);
    return(v);
}

bool transskip_find(trans_skip* l, setkey_t k, Desc* desc, uint8_t opid)
{
    NodeDesc* nodeDesc = NULL;

    bool ret;
    ptst_t *ptst;
    node_t *x;

    k = CALLER_TO_INTERNAL_KEY(k);

    ptst = fr_critical_enter();

    x = weak_search_predecessors(l, k, NULL, NULL);

retry:
    if ( x->k == k )
    {
        NodeDesc* oldCurrDesc = x->nodeDesc;

        if(IS_MARKED(oldCurrDesc))
        {
            ret = false;
            goto out;
            //READ_FIELD(level, x->level);
            //mark_deleted(x, level & LEVEL_MASK);
            //x = strong_search_predecessors(l, k, NULL, NULL);
            //goto retry;
        }

        if(!FinishPendingTxn(l, oldCurrDesc, desc))
        {
            ret = false;
            goto out;
        }

        if(nodeDesc == NULL)
        {
            nodeDesc = l->nodeDescAllocator->Alloc();
            nodeDesc->desc = desc;
            nodeDesc->opid = opid;
        }

        if(IsSameOperation(oldCurrDesc, nodeDesc))
        {
            ret = true;
            goto out;
        }

        //if(IsKeyExist(oldCurrDesc))
		if(IsKeyExist_mod(oldCurrDesc, desc))
        {
            NodeDesc* currDesc = x->nodeDesc;

            if(desc->status != LIVE)
            {
                ret = false;
                goto out;
            }
// READ_ONLY_OPT_CODE
// (
//         if(desc->isReadOnly) {
//             ret = true;
//             goto out;
//         }
// )             

            //if(currDesc == oldCurrDesc)
            {
                //Update desc 
                currDesc = __sync_val_compare_and_swap(&x->nodeDesc, oldCurrDesc, nodeDesc);

                if(currDesc == oldCurrDesc)
                {
                    ret = true;
                    goto out;
                }
            }

            goto retry;
        }
        else
        {
            ret = false;
            goto out;
        }
    }
    else
    {
        ret = false;
        goto out;
    }

out:
    fr_critical_exit(ptst);
    return ret;
}

setval_t transskip_find_original(trans_skip *l, setkey_t k)
{
    setval_t  v = NULL;
    ptst_t    *ptst;
    node_t* x;

    k = CALLER_TO_INTERNAL_KEY(k);

    ptst = fr_critical_enter();

    x = weak_search_predecessors(l, k, NULL, NULL);
    if ( x->k == k ) READ_FIELD(v, x->v);

    fr_critical_exit(ptst);
    return(v);
}

void init_transskip_subsystem(void)
{
    int i;

    fr_init_ptst_subsystem();
    fr_init_gc_subsystem();

    for ( i = 0; i < NUM_LEVELS; i++ )
    {
        gc_id[i] = fr_gc_add_allocator(sizeof(node_t) + i*sizeof(node_t *));
    }
}

void destroy_transskip_subsystem(void)
{
    fr_destroy_gc_subsystem();
}

static inline bool help_ops(trans_skip* l, Desc* desc, uint8_t opid)
{
    bool ret = true;
    // For less than 1 million nodes, it is faster not to delete nodes
    //std::vector<node_t*> deletedNodes;
    //std::vector<node_t*> insertedNodes;

    //Cyclic dependcy check
    if(helpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
        } else
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
        
        return false;
    }

    helpStack.Push(desc);

	long int txn_invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS

    while(desc->status == LIVE && ret && opid < desc->size)
    {
		long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
		long int response; //CORRECTNESS ANNOTATIONS

        const Operator& op = desc->ops[opid];

        if(op.type == INSERT)
        {
            node_t* n;
            ret = transskip_insert(l, op.key, desc, opid, n);
            //insertedNodes.push_back(n);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, PRODUCER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS
        }
        else if(op.type == DELETE)
        {
            node_t* n;
            ret = transskip_delete(l, op.key, desc, opid, n);
            //deletedNodes.push_back(n);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, CONSUMER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS
        }
        else
        {
            ret = transskip_find(l, op.key, desc, opid);

			response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
			bool method_ret = true; //CORRECTNESS ANNOTATIONS
			if(ret == false) method_ret = false; //CORRECTNESS ANNOTATIONS
			create_method(op.key, INT_MIN, SET, READER, invocation, response, method_ret); //CORRECTNESS ANNOTATIONS
        }

        opid++;
    }

    helpStack.Pop();

    if(ret == true)
    {

READ_ONLY_OPT_CODE        
(
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
        {   
            __sync_fetch_and_add(&g_count_commit, 1);   
        }     
    }else {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
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
            __sync_fetch_and_add(&g_count_commit, 1);
        }
    }              

)

NO_READ_ONLY_OPT_CODE        
(
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
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
            __sync_fetch_and_add(&g_count_commit, 1);
        }            
)            
            // __sync_fetch_and_add(&g_count_commit, 1);

            // Mark nodes for physical deletion
            //for(uint32_t i = 0; i < deletedNodes.size(); ++i)
            //{
                //node_t* x = deletedNodes[i];
                //if(x == NULL) { continue; }

                //NodeDesc* nodeDesc = x->nodeDesc;
                //if(nodeDesc->desc != desc) { continue; }

                //if(__sync_bool_compare_and_swap(&x->nodeDesc, nodeDesc, SET_MARK(nodeDesc)))
                //{
                    //transskip_delete_org(l, x->k);
                //}
            //}
        // }
    }
    else
    {


READ_ONLY_OPT_CODE        
( 
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
        {  
            __sync_fetch_and_add(&g_count_abort, 1);
        }
    }else {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
            
            
            // Mark nodes for physical deletion
            //for(uint32_t i = 0; i < insertedNodes.size(); ++i)
            //{
                //node_t* x = insertedNodes[i];
                //if(x == NULL) { continue; }

                //NodeDesc* nodeDesc = x->nodeDesc;
                //if(nodeDesc->desc != desc) { continue; }

                //if(__sync_bool_compare_and_swap(&x->nodeDesc, nodeDesc, SET_MARK(nodeDesc)))
                //{
                    //transskip_delete_org(l, x->k);
                //}
            //}
        // }
    }

	if(desc->status == COMMITTED) //CORRECTNESS ANNOTATIONS
	{
		long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
		//Create remaining methods
		while(opid < desc->size)
		{

			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
			long int response; //CORRECTNESS ANNOTATIONS

			const Operator& op = desc->ops[opid];

		    if(op.type == INSERT)
		    {
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				//create_method_persist(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
		    }
		    else if(op.type == DELETE)
		    {
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
				//create_method_persist(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
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
		if(desc->ops[i].type == INSERT)
		{
			char str[80];
			sprintf(str, " P %d", desc->ops[i].key);
			transaction.append(str); 
		} else if (desc->ops[i].type == DELETE)
		{
			char str[80];
			sprintf(str, " C %d", desc->ops[i].key);
			transaction.append(str); 
		} else if (desc->ops[i].type == FIND)
		{
			char str[80];
			sprintf(str, " F %d", desc->ops[i].key);
			transaction.append(str); 
		}
	}
	if(desc->status == COMMITTED)
		transaction.append(":Commit"); 
	else
		transaction.append(":Abort");
	printf("%s\n", transaction.c_str());*/

	if(desc->persistStatus == PERSISTED && desc->status == COMMITTED)
	{
		long int txn_response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
		for(int i = 0; i < desc->size; i++) 
		{
			long int invocation = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS	
			long int response; //CORRECTNESS ANNOTATIONS

		    const Operator& op = desc->ops[i];

		    if(op.type == INSERT)
		    {
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, PRODUCER, invocation, response, true); //CORRECTNESS ANNOTATIONS
		    }
		    else if(op.type == DELETE)
		    {
				response = get_elapsed_nanoseconds(); //CORRECTNESS ANNOTATIONS
				create_method_persist(op.key, INT_MIN, SET, CONSUMER, invocation, response, true); //CORRECTNESS ANNOTATIONS
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

    return ret;
}

static inline bool help_ops_rec(trans_skip* l, Desc* desc, uint8_t opid) //CORRECTNESS ANNOTATIONS
{
    bool ret = true;
    // For less than 1 million nodes, it is faster not to delete nodes
    //std::vector<node_t*> deletedNodes;
    //std::vector<node_t*> insertedNodes;

    //Cyclic dependcy check
    if(helpStack.Contain(desc))
    {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
        } else
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
        
        return false;
    }

    helpStack.Push(desc);

    while(desc->status == LIVE && ret && opid < desc->size)
    {
        const Operator& op = desc->ops[opid];

        if(op.type == INSERT)
        {
            node_t* n;
            ret = transskip_insert(l, op.key, desc, opid, n);
            //insertedNodes.push_back(n);
        }
        else if(op.type == DELETE)
        {
            node_t* n;
            ret = transskip_delete(l, op.key, desc, opid, n);
            //deletedNodes.push_back(n);
        }
        else
        {
            ret = transskip_find(l, op.key, desc, opid);
        }

        opid++;
    }

    helpStack.Pop();

    if(ret == true)
    {

READ_ONLY_OPT_CODE        
(
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
        {   
            __sync_fetch_and_add(&g_count_commit, 1);   
        }     
    }else {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
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
            __sync_fetch_and_add(&g_count_commit, 1);
        }
    }              

)

NO_READ_ONLY_OPT_CODE        
(
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, COMMITTED))
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
            __sync_fetch_and_add(&g_count_commit, 1);
        }            
)            
            // __sync_fetch_and_add(&g_count_commit, 1);

            // Mark nodes for physical deletion
            //for(uint32_t i = 0; i < deletedNodes.size(); ++i)
            //{
                //node_t* x = deletedNodes[i];
                //if(x == NULL) { continue; }

                //NodeDesc* nodeDesc = x->nodeDesc;
                //if(nodeDesc->desc != desc) { continue; }

                //if(__sync_bool_compare_and_swap(&x->nodeDesc, nodeDesc, SET_MARK(nodeDesc)))
                //{
                    //transskip_delete_org(l, x->k);
                //}
            //}
        // }
    }
    else
    {


READ_ONLY_OPT_CODE        
( 
    if(desc->isReadOnly) {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
        {  
            __sync_fetch_and_add(&g_count_abort, 1);
        }
    }else {
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
        if(__sync_bool_compare_and_swap(&desc->status, LIVE, ABORTED))
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
            
            
            // Mark nodes for physical deletion
            //for(uint32_t i = 0; i < insertedNodes.size(); ++i)
            //{
                //node_t* x = insertedNodes[i];
                //if(x == NULL) { continue; }

                //NodeDesc* nodeDesc = x->nodeDesc;
                //if(nodeDesc->desc != desc) { continue; }

                //if(__sync_bool_compare_and_swap(&x->nodeDesc, nodeDesc, SET_MARK(nodeDesc)))
                //{
                    //transskip_delete_org(l, x->k);
                //}
            //}
        // }
    }

    return ret;
}



bool execute_ops(trans_skip* l, Desc* desc)
{
    helpStack.Init();

    bool ret = help_ops(l, desc, 0);

    return ret;
}

void transskip_reset_metrics()
{
    g_count_commit = 0;
    g_count_abort = 0;
    g_count_fake_abort = 0;
}

void transskip_print(trans_skip* l)
{
    node_t* curr = l->head.next[0];

    while(curr != l->tail)
    {
        printf("Node [%p] Key [%u] Status [%s]\n", curr, INTERNAL_TO_CALLER_KEY(curr->k), IsKeyExist(CLR_MARKD(curr->nodeDesc))? "Exist":"Inexist");
        curr = (node_t*)get_unmarked_ref(curr->next[0]); 
    }
}

void transskip_free(trans_skip* l)
{
    printf("Total commit %u, abort (total/fake) %u/%u\n", g_count_commit, g_count_abort, g_count_fake_abort);

	FILE* pfile = fopen("output.txt", "a"); //CORRECTNESS ANNOTATIONS
	fprintf(pfile, "%u %u\n", g_count_commit, g_count_abort); //CORRECTNESS ANNOTATIONS
	fclose(pfile); //CORRECTNESS ANNOTATIONS

    //transskip_print(l);
}
