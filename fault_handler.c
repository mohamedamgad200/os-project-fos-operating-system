/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault

void page_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//TODO: [PROJECT MS3] [FAULT HANDLER] page_fault_handler
	// Write your code here, remove the panic and write your code
	//panic("page_fault_handler() is not implemented yet...!!");

	//refer to the project presentation and documentation for details
	uint32 WS_size=env_page_ws_get_size(curenv);
			struct FrameInfo *ptr_frame_info = NULL;
			uint32 *ptr_page_table = NULL;
			if(WS_size > curenv->page_WS_max_size){
				allocate_frame(&ptr_frame_info);
				map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_USER|PERM_WRITEABLE);
				int ret= pf_read_env_page(curenv,(void*)fault_va);
				if(ret== E_PAGE_NOT_EXIST_IN_PF){
			   if((USER_HEAP_MAX >= fault_va && USER_HEAP_START <=fault_va )|| (fault_va <=USTACKTOP && USTACKBOTTOM <=fault_va)){
								}
				       else {
						panic ("ILLEGAL MEMORY ACCESS");
						}
					}
				for(int i = 0 ; i<curenv->page_WS_max_size ; i++){
						if(curenv->ptr_pageWorkingSet[i].empty == 1){
						env_page_ws_set_entry(curenv, i, fault_va);
						i++;
				        if(i == curenv->page_WS_max_size){
						      i = 0;
				        }
							}
						         }
				curenv->page_last_WS_index++;
		        if(curenv->page_last_WS_index == (curenv->page_WS_max_size-1)){
				      curenv->page_last_WS_index = 0;
				                 }
				            }
			else //replacement
					{

						struct FrameInfo * F=NULL;
						allocate_frame(&F);
						map_frame(curenv->env_page_directory,F,fault_va,PERM_USER| PERM_WRITEABLE );
						uint32 * ptr_page_table=NULL;
						get_page_table(curenv->env_page_directory,fault_va,&ptr_page_table);
						ptr_page_table[PTX(fault_va)] = ptr_page_table[PTX(fault_va)] | (PERM_USED );
						uint32 * ptr_page_table2=NULL;
						int i=0;
						int ret = pf_read_env_page(curenv,(void *)fault_va);
								if (ret == E_PAGE_NOT_EXIST_IN_PF){
								if((fault_va >= USER_HEAP_START && fault_va <= USER_HEAP_MAX) ||(fault_va <= USTACKTOP && fault_va >=USTACKBOTTOM)){

								}
								else{
									//unmap_frame(curenv->env_page_directory,fault_va);
									panic(" ILLEGAL MEMORY ACCESS");
								}
								}

					//env_page_ws_print(curenv);
					for (; ; ) {

					//the first loop if we find used=1 replace it else make used bit=0
					{
						if(curenv->page_last_WS_index==curenv->page_WS_max_size)
						{
							curenv->page_last_WS_index=0;
						}
						//env_page_ws_print(curenv);

					uint32 index_va=env_page_ws_get_virtual_address(curenv,curenv->page_last_WS_index);

					get_page_table(curenv->env_page_directory,index_va,&ptr_page_table2);
					uint32 e=ptr_page_table2[PTX(index_va)];
					uint32 e2=e&(PERM_MODIFIED);
					e=e&(PERM_USED);
					if(e==0)
					{
						    if(e2!=0)
						    {
						    	uint32 * ptr_page_table2=NULL;
						    	get_page_table(curenv->env_page_directory,index_va,&ptr_page_table2);
						    	struct FrameInfo * frame=get_frame_info(curenv->env_page_directory,index_va,&ptr_page_table2);
						    	pf_update_env_page(curenv,index_va,frame);

						    }

						    env_page_ws_clear_entry(curenv,curenv->page_last_WS_index);
							env_page_ws_set_entry(curenv, curenv->page_last_WS_index, fault_va);
							unmap_frame(curenv->env_page_directory,index_va);
							curenv->page_last_WS_index++;
							break;
					}
					else if(e!=0)
					{
					ptr_page_table2[PTX(index_va)] = ptr_page_table2[PTX(index_va)] & (~PERM_USED);
					curenv->page_last_WS_index++;
					}
					}
					}
			}
}
void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	// Write your code here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");


}
