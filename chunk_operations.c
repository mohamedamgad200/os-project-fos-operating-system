/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");
	uint32 size=num_of_pages*PAGE_SIZE;
	uint32 source_eva = ROUNDUP(source_va+size,PAGE_SIZE);
	source_va=ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 dest_eva = ROUNDUP(dest_va+size,PAGE_SIZE);
	dest_va=ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 *check_page_tableS=NULL;
	uint32 *check_page_tableD=NULL;
	for(int i=dest_va;i<dest_eva;i+=PAGE_SIZE)
	{
		struct FrameInfo *ptr_frame_infoD= get_frame_info(page_directory,i,&check_page_tableD);
		if(ptr_frame_infoD!=NULL)
		{
			return -1;
		}
	}
	for(int i=dest_va,j=source_va;i<dest_eva,j<source_eva;i+=PAGE_SIZE,j+=PAGE_SIZE)
	{
		get_page_table(page_directory,i,&check_page_tableD);
		struct FrameInfo *ptr_frame_infoS= get_frame_info(page_directory,j,&check_page_tableS);
		if(check_page_tableD==NULL)
		{
			check_page_tableD=create_page_table(page_directory,i);
		}
		int perms= check_page_tableS[PTX(j)];
		perms=perms<<20;
		perms=perms>>20;
		int mapped=map_frame(page_directory,ptr_frame_infoS,i,perms);
		unmap_frame(page_directory,j);
	}
	return 0;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
//	panic("copy_paste_chunk() is not implemented yet...!!");
	uint32 *check_page_tableS = NULL;
	uint32 source_eva = source_va + size;
	uint32 dest_eva = dest_va + size;
	uint32 *check_page_tableD = NULL;
	uint32 temp_end = dest_va + size;
	uint32 dest1 = dest_va ;
	for (int i = dest1; i < temp_end; i += PAGE_SIZE)
	{
		int x = pt_get_page_permissions(page_directory, i) & (PERM_WRITEABLE);
		if (x != PERM_WRITEABLE)
		{
			return -1;
		}
	}
	for(int i=dest_va,j=source_va;i<dest_eva,j<source_eva;i+=PAGE_SIZE,j+=PAGE_SIZE)
	{
		get_page_table(page_directory,i,&check_page_tableD);
		if(check_page_tableD==NULL)
		{
			create_page_table(page_directory,i);
		}
		struct FrameInfo *ptr_frame_infoS= get_frame_info(page_directory,j,&check_page_tableS);
		struct FrameInfo *ptr_frame_infod= get_frame_info(page_directory,i,&check_page_tableD);
		if(ptr_frame_infod==NULL)
		{
		   int perms= check_page_tableS[PTX(j)];
			int ret  = allocate_frame(&ptr_frame_infod) ;
			if (ret != E_NO_MEM)
			{
				perms=perms<<20;
				perms=perms>>20;
				perms=perms&PERM_USER;

			    map_frame(ptr_page_directory, ptr_frame_infod, i, perms|PERM_WRITEABLE);
			}
		}
	}
	uint8 *src_ptr = (uint8 *)(source_va) ;
	uint8 *dst_ptr = (uint8 *) dest_va;
	for (int i = source_va; i < source_eva; i++)
	{
		*dst_ptr = *src_ptr;
		dst_ptr++;
		src_ptr++;
	}

	return 0;
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	//panic("share_chunk() is not implemented yet...!!");
	uint32 source_eva = ROUNDUP(source_va+size,PAGE_SIZE);
	source_va=ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 dest_eva = ROUNDUP(dest_va+size,PAGE_SIZE);
	dest_va=ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 *check_page_tableS=NULL;
	uint32 *check_page_tableD=NULL;
	for(int i=dest_va;i<dest_eva;i+=PAGE_SIZE)
	{
		struct FrameInfo *ptr_frame_infoD= get_frame_info(page_directory,i,&check_page_tableD);
		if(ptr_frame_infoD!=NULL)
		{
			return -1;
		}
	}
	for(int i=dest_va,j=source_va;i<dest_eva,j<source_eva;i+=PAGE_SIZE,j+=PAGE_SIZE)
	{
		get_page_table(page_directory,i,&check_page_tableD);
		struct FrameInfo *ptr_frame_infoS= get_frame_info(page_directory,j,&check_page_tableS);
		if(check_page_tableD==NULL)
		{
			check_page_tableD=create_page_table(page_directory,i);
		}
		int mapped=map_frame(page_directory,ptr_frame_infoS,i,perms);
	}
	return 0;
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");
	//va=ROUNDDOWN(va,PAGE_SIZE);
	//size+=va;
	//size=ROUNDUP(size,PAGE_SIZE);

	size = ROUNDUP(va+size,PAGE_SIZE);
	va=ROUNDDOWN(va,PAGE_SIZE);

	uint32 *check_page_table=NULL;
	for(int i=va;i<size;i+=PAGE_SIZE)
	{
		struct FrameInfo *ptr_frame_info= get_frame_info(page_directory,i,&check_page_table);
		if(ptr_frame_info!=NULL)
		{
			return -1;
	 	}
	}
	for(int i=va;i<size;i+=PAGE_SIZE)
	{
		get_page_table(page_directory,i,&check_page_table);
		if(check_page_table==NULL)
		{
			check_page_table=create_page_table(page_directory,i);
		}
		struct FrameInfo *ptr;
		int allocated=allocate_frame(&ptr);
		int mapped=map_frame(page_directory,ptr,i,perms);
                ptr->va=i;
		}
	/*
	cprintf("c=%d d=%d",c,d);

	int h_num_frames = (size - va)/ PAGE_SIZE ;

	cprintf ("the expected number of frames :  %d" ,  h_num_frames) ;
	cprintf("the start address after round down is :  %x" , va) ;
	cprintf("the end address after round up is :  %x" , size) ;

	*/
	return 0;
}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");
	eva= ROUNDUP(eva,PAGE_SIZE);
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	uint32 arr[PAGE_SIZE*5]={0},p=0,f=0;
	uint32 table;
	for(uint32 i=sva;i<eva;i+=PAGE_SIZE)
	{
		uint32 *check_page_table=NULL;
		struct FrameInfo *ptr_frame_info= get_frame_info(page_directory,i,&check_page_table);
		get_page_table(page_directory,i,&check_page_table);
		if(check_page_table!=NULL)
		{
			f=0;
			table=(uint32)check_page_table[PDX(i)];
			table=table>>12;
			for(int j=0;j<p;j++)
			{
			if(arr[j]==table)
			{
				f=1;
				break;
			}
			}
			if(f==0)
			{
				arr[p]=table;
				p++;
			   (*num_tables)++;
			}
			if(ptr_frame_info!=NULL)
			{
				(*num_pages)++;
			}
		}

    }
	//cprintf("num_tables %d num_pages %d",*num_tables,*num_pages);
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");
	size = ROUNDUP(sva+size,PAGE_SIZE);
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	uint32 num_tables = 0;
	uint32 num_pages = 0;
	uint32 arr[PAGE_SIZE*5]={0},p=0,f=0;
	uint32 table;
	uint32 *check_page_table=NULL;
	for(int i=sva;i<size;i+=PAGE_SIZE)
		{
			struct FrameInfo *ptr_frame_info= get_frame_info(page_directory,i,&check_page_table);
			get_page_table(page_directory,i,&check_page_table);
			if(ptr_frame_info==NULL)
			{
				num_pages++;
		 	}
			if(check_page_table==NULL)
			{
				f=0;
			table=i;
			table=table>>22;
			for(int j=0;j<p;j++)
			{
			if(arr[j]==table)
			{
				f=1;
				break;
			}
			}
			if(f==0)
			{
				arr[p]=table;
				p++;
				num_tables++;
			}
			}
		}
	return num_pages+num_tables;
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
	uint32 sizerounding = ROUNDUP((virtual_address + size), PAGE_SIZE);
		uint32 va1 =virtual_address;
		uint32 va2 = virtual_address;
		uint32 va3 = virtual_address;
		int f;
		int maxsizeofworkingset = e->page_WS_max_size;
		int numberofpages = size / PAGE_SIZE;
		int counter;
		int numberofinpagetable = 1024;
		int i;
		for (int i = 0; i < numberofpages; i++)
		{
			pf_remove_env_page(e, va1);
			env_page_ws_invalidate(e, va1);
			unmap_frame(e->env_page_directory, va1);
			uint32 *wla = NULL;
			get_page_table(e->env_page_directory, va1, &wla);

			uint32 flag = 1;
			if (wla != NULL) {

				for (counter = 0; counter < numberofinpagetable; counter++)
				{
					if (wla[counter] != 0)
					{
						flag = 0;
						break;
					}

				}


			}
			if (flag == 1)
			{
				e->env_page_directory[PDX(va1)]=0;
				kfree((void*) wla);

			}
			va1 += PAGE_SIZE;

		}
	}


//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

