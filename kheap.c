#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//
void initialize_dyn_block_system()
{
        LIST_INIT(&FreeMemBlocksList);
	LIST_INIT(&AllocMemBlocksList);
#if STATIC_MEMBLOCK_ALLOC
      //DO NOTHING
#else
	MAX_MEM_BLOCK_CNT = (KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE;
	//uint32 sizeee = sizeof(MemBlockNodes); //The size of MemBlocknodes array
	// round up the size
       // start of memblocknode in kheap start
     //	uint32 final = sizeof(MemBlockNodes[0]); //size of one entry
   	uint32 r = ROUNDUP(MAX_MEM_BLOCK_CNT * sizeof(struct MemBlock), PAGE_SIZE);
     //	uint32 r = s * final; //size= number of entries*size of entry
	allocate_chunk(ptr_page_directory, KERNEL_HEAP_START, r, PERM_WRITEABLE);
	MemBlockNodes = (struct MemBlock *)KERNEL_HEAP_START;
	initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
      // initially make the  memblocknode list is available
	struct MemBlock *tmp = LIST_FIRST(&AvailableMemBlocksList); //here we take from the avilable
	LIST_REMOVE(&AvailableMemBlocksList, tmp);
	tmp->size = (KERNEL_HEAP_MAX - KERNEL_HEAP_START) - (r);
	tmp->sva = KERNEL_HEAP_START + r;
	LIST_INSERT_HEAD(&FreeMemBlocksList, tmp);
#endif
}
void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	//change this "return" according to your answer
	//WE need to allocate a block with size equals size;
		//then we need to check if there is a free frame in the free frame list
		//in case,there is a free frame we need to get the physical address of this frame
		//at last we need to make the kernel heap pointing to the physical memory
		//insert in the alloflist
		//then we need to call allocate chunk
	 size=ROUNDUP(size, PAGE_SIZE);
	if(isKHeapPlacementStrategyFIRSTFIT())
		{
		     struct MemBlock*blo=alloc_block_FF(size);
			if(blo!=NULL)
			{
			insert_sorted_allocList(blo); //insert in the allocated from free
				allocate_chunk(ptr_page_directory, blo->sva, size, PERM_WRITEABLE);
				return (void*)blo->sva;
		}
			else{
			return NULL;
			     }
		}
	else if(isKHeapPlacementStrategyBESTFIT())
	{
			 struct MemBlock*blo=alloc_block_BF(size);
			 if(blo!=NULL)
			 {
				 insert_sorted_allocList(blo); //insert in the allocated from free
				 allocate_chunk(ptr_page_directory, blo->sva, size, PERM_WRITEABLE);
				 return (void*)blo->sva;
			 }
             else
	          {
	            return NULL;
	          }

		}
	else if(isKHeapPlacementStrategyNEXTFIT())
	{
	   struct MemBlock*blo=alloc_block_NF(size);
		if(blo!=NULL)
		 {
			 insert_sorted_allocList(blo); //insert in the allocated from free
		 allocate_chunk(ptr_page_directory, blo->sva, size, PERM_WRITEABLE);
		return (void*)blo->sva;
		 }
	     else
	     {
		  return NULL;
		 }
	}
		return NULL;
}
void kfree(void* virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
    uint32 va=(uint32)virtual_address;
	struct MemBlock *blockFIND=find_block(&(AllocMemBlocksList),va);
	if(blockFIND!=NULL)
	{
		LIST_REMOVE(&AllocMemBlocksList,blockFIND);
		uint32 size = ROUNDUP(blockFIND->sva+blockFIND->size,PAGE_SIZE);
		uint32 vablock=ROUNDDOWN(blockFIND->sva,PAGE_SIZE);
		for(uint32 i=vablock;i<size;i+=PAGE_SIZE)
		{
			unmap_frame(ptr_page_directory,i);
		}
		insert_sorted_with_merge_freeList(blockFIND);
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	//struct FrameInfo *ptr_frame_info=to_frame_info(physical_address);
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
        struct FrameInfo*m=to_frame_info(physical_address);
	return m->va ;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
        int kvtop= virtual_to_physical(ptr_page_directory,virtual_address);
	    return kvtop;

}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code
	panic("krealloc() is not implemented yet...!!");
}
