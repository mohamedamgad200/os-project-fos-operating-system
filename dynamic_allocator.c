/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	// Write your code here, remove the panic and write your code
	//panic("initialize_MemBlocksList() is not implemented yet...!!");
	LIST_INIT(&AvailableMemBlocksList);
	for(int i=0;i<numOfBlocks;i++)
	{
		LIST_INSERT_TAIL(&AvailableMemBlocksList,&MemBlockNodes[i]);
	}
}
//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	// Write your code here, remove the panic and write your code
	//panic("find_block() is not implemented yet...!!");
	struct MemBlock *element;
		LIST_FOREACH(element,blockList)
	   {
			if(element->sva==va)
			{
	     		return element;
			}
		}
		return NULL;
}
//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
	// Write your code here, remove the panic and write your code
	//panic("insert_sorted_allocList() is not implemented yet...!!");
	int size = LIST_SIZE(&AllocMemBlocksList);
			struct MemBlock* current = LIST_FIRST(&AllocMemBlocksList);
			struct MemBlock* firstElement = LIST_FIRST(&AllocMemBlocksList);
			struct MemBlock* lastElement = LIST_LAST(&AllocMemBlocksList);
			struct MemBlock *it;
			if(size == 0)
			{
				LIST_INSERT_HEAD(&AllocMemBlocksList, blockToInsert);
			}
			else if((lastElement->sva )<blockToInsert->sva){
				LIST_INSERT_TAIL(&AllocMemBlocksList, blockToInsert);
			}
			else if ((firstElement->sva) > blockToInsert->sva) {
				LIST_INSERT_HEAD(&AllocMemBlocksList, blockToInsert);
			    }
			else
			{
				LIST_FOREACH(it, &(AllocMemBlocksList)){
					if((it->sva )<blockToInsert->sva)
					{
						if(it->prev_next_info.le_next->sva>blockToInsert->sva )
						{
							LIST_INSERT_AFTER(&AllocMemBlocksList, it, blockToInsert);
						}

					}
				}
			}


}
//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_FF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_FF() is not implemented yet...!!");
	        struct MemBlock *it;
			bool is_found=0;
			LIST_FOREACH(it,&(FreeMemBlocksList))
			{
				struct MemBlock*tmp=it;
				if(it->size == size)
				{
					is_found=1;
					LIST_REMOVE(&FreeMemBlocksList,it);
					return tmp;
				}
				else if(it->size > size)
				{
					is_found=1;
					tmp=LIST_FIRST(&AvailableMemBlocksList);
					LIST_REMOVE(&AvailableMemBlocksList,tmp);
					tmp->sva=it->sva;
					tmp->size=size;
					it->size-=size;
					it->sva+=size;
					return tmp;
				}
			}
			return NULL;
}
//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	struct MemBlock*point;
	bool flag=0;
	uint32 diff = UINT_MAX;
	struct MemBlock* LAST_BLK;
	LIST_FOREACH(point,&((FreeMemBlocksList)))
	{
		struct MemBlock*temp=point;
		if(temp->size >= size)
		{
			flag=1;
			if(temp->size-size < diff)
			{
				diff=(temp->size)-(size);
				LAST_BLK = point;
			}
		}
	}
	if(flag && LAST_BLK->size == size)
	{
		LIST_REMOVE(&FreeMemBlocksList,LAST_BLK);
		return LAST_BLK;
	}
	else if(flag && LAST_BLK->size > size)
	{
		point=LIST_FIRST(&AvailableMemBlocksList);
		LIST_REMOVE(&AvailableMemBlocksList,point);
		point->sva=LAST_BLK->sva;
		point->size=size;
		LAST_BLK->size-=size;
		LAST_BLK->sva+=size;
		return point;
	}
  	 return NULL;
}
struct MemBlock *LastAlloc;
//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *alloc_block_NF(uint32 size)
{
	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	// Write y our code here,remove the panic and write your code
	//panic("alloc_block_NF() is not implemented yet...!!");
	bool found=0;
	struct MemBlock*it;
	struct MemBlock*it2;
	LIST_FOREACH(it,&((FreeMemBlocksList)))
	{
		if(it->sva==0)
		{
			LastAlloc=it;
			struct MemBlock*tmp=it;
			if(it->size == size)
			{
				found=1;
				LIST_REMOVE(&FreeMemBlocksList,it);
				LastAlloc=tmp;
				return tmp;
			}
			else if(it->size > size)
			{
				found=1;
				tmp=LIST_FIRST(&AvailableMemBlocksList);
				LIST_REMOVE(&AvailableMemBlocksList,tmp);
				tmp->sva=it->sva;
				tmp->size=size;
				it->size-=size;
				it->sva+=size;
				LastAlloc=tmp;
				return tmp;
			}
		}
		if(it->sva>LastAlloc->sva)
		{
		struct MemBlock*tmp=it;
		if(it->size == size)
		{
		    found=1;
			LIST_REMOVE(&FreeMemBlocksList,it);
			LastAlloc=tmp;
			//LastAlloc=LIST_NEXT(tmp);
			return tmp;
		}
		else if(it->size > size)
		{
			found=1;
			tmp=LIST_FIRST(&AvailableMemBlocksList);
			LIST_REMOVE(&AvailableMemBlocksList,tmp);
			tmp->sva=it->sva;
			tmp->size=size;
			it->size-=size;
			it->sva+=size;
			LastAlloc=tmp;
			//LastAlloc=LIST_NEXT(tmp);
			return tmp;
		}
	}
	}
	LIST_FOREACH(it2,&((FreeMemBlocksList)))
	{
			if(it2->sva<LastAlloc->sva)
			{
			struct MemBlock*tmp2=it2;
			if(it2->size == size)
			{
			    found=1;
				LIST_REMOVE(&FreeMemBlocksList,it2);
				LastAlloc=tmp2;;
				//LastAlloc=LIST_NEXT(tmp2);;
				return tmp2;
			}
			else if(it2->size > size)
			{
				found=1;
				tmp2=LIST_FIRST(&AvailableMemBlocksList);
				LIST_REMOVE(&AvailableMemBlocksList,tmp2);
				tmp2->sva=it2->sva;
				tmp2->size=size;
				it2->size-=size;
				it2->sva+=size;
				LastAlloc=tmp2;
				//LastAlloc=LIST_NEXT(tmp2);
				return tmp2;
			}
		}
	}
	return NULL;
}
//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
//	cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
//	print_mem_block_lists() ;

	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_with_merge_freeList
	// Write your code here, remove the panic and write your code
	//panic("insert_sorted_with_merge_freeList() is not implemented yet...!!");
	int size = LIST_SIZE(&FreeMemBlocksList);
	struct MemBlock* current = LIST_FIRST(&FreeMemBlocksList);
	struct MemBlock* firstElement = LIST_FIRST(&FreeMemBlocksList);
	struct MemBlock* lastElement = LIST_LAST(&FreeMemBlocksList);
	if(size == 0)
	{
	   LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
	}
	else
	{
		if((blockToInsert->sva) < firstElement->sva)
		{
			if((blockToInsert->sva+blockToInsert->size)==firstElement->sva)
			{
				firstElement->sva=blockToInsert->sva;
				firstElement->size+=blockToInsert->size;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
				return;
			}
			else
			{
				LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
				return;
			}
	}
	else if((blockToInsert->sva) >lastElement->sva)
	{
		if(((lastElement->sva)+(lastElement->size))==blockToInsert->sva)
		{
			lastElement->size+=blockToInsert->size;
			blockToInsert->sva=0;
			blockToInsert->size=0;
			LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
		}
	    else
	    {
		   LIST_INSERT_TAIL(&FreeMemBlocksList, blockToInsert);
	    }
	}
	else
	{
		struct MemBlock *element;
		LIST_FOREACH(element,&((FreeMemBlocksList)))
		{
			struct MemBlock* NextBlock = LIST_NEXT(element);
			struct MemBlock* PreviElement = LIST_PREV(element);

			 if((blockToInsert->sva+blockToInsert->size)==NextBlock->sva&&((element->sva+element->size)==blockToInsert->sva))
			{
				int newsize=blockToInsert->size+NextBlock->size;
				element->size+=newsize;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				NextBlock->sva=0;
				NextBlock->size=0;
				LIST_REMOVE(&FreeMemBlocksList,NextBlock);
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
				LIST_INSERT_HEAD(&AvailableMemBlocksList, NextBlock );
				break;
			}
			else if((blockToInsert->sva+blockToInsert->size)==NextBlock->sva)
			{
				//next
				NextBlock->sva=blockToInsert->sva;
				NextBlock->size+=blockToInsert->size;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
				break;
			}
			else if((element->sva+element->size)==blockToInsert->sva)
			{
				//previous
				element->size+=blockToInsert->size;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
				break;
			}
			else
			{

				if(NextBlock->sva>blockToInsert->sva)
				{
					LIST_INSERT_AFTER(&FreeMemBlocksList,element, blockToInsert);
					break;
				}
			}
		}
	}
	}
//	cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
//	print_mem_block_lists();

}
