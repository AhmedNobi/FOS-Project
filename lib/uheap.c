#include <inc/lib.h>

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//uint32 neededSize=0;
//int counter_malloc=0;
int arr_size=(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;

//int best_arr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE];
int last_index_best=0;
struct Best_maloc
{
	uint32 va;
	int NumofPages;
	int empty;
	uint32 size;

};

struct Block
{
	//int size;
	int NumOfBlocks;
	bool empty;
	uint32 size;

};
struct Block arr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE];
struct Best_maloc best_arr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE];
int best_arr_size=10000;

//uint32 start_address_malloc;
void* malloc(uint32 size)
{

	//cprintf("hi");

	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side malloc]
	// Write your code here, remove the panic and write your code


	//refer to the documentation for details

	//This function should find the space of the required range by either:
	//1) FIRST FIT strategy
	//2) BEST FIT strategy
	//int sizeinKiloByte=size/1024;
	ROUNDUP(size,PAGE_SIZE);
	int neededPages=0;
	if(size%4096==0)
		neededPages=size/4096;
	else
		neededPages=(size/4096)+1;

	int c=0;
	if(sys_isUHeapPlacementStrategyFIRSTFIT()==1)
	{
		for(int i=0;i<arr_size;i++)
		{

			if(arr[i].empty==1)
			{
				c=0;
				i+=arr[i].NumOfBlocks-1;
			}
			else
			{
				c++;
				if(c==neededPages)
				{
					arr[i-c+1].NumOfBlocks=c;
					arr[i-c+1].empty=1;
					arr[i-c+1].size=size;
					uint32 va=USER_HEAP_START+((i-c+1)*PAGE_SIZE);
					sys_allocateMem(va,size);
					return (uint32*)va;
				}
			}
		}
		return NULL;
	}
	else if(sys_isUHeapPlacementStrategyBESTFIT()==1)
	{
		for(int i=0;i<last_index_best;i++)
		{
			for(int j=0;j<(last_index_best-i-1);j++)
			{
				struct Best_maloc tmp;
				if(best_arr[j].va>best_arr[j+1].va)
				{
					tmp=best_arr[j];
					best_arr[j]=best_arr[j+1];
					best_arr[j+1]=tmp;
				}

			}
		}
		//best_arr
		int best_number_pages=99999999;
		int min;
		int last_index;
		uint32 va=USER_HEAP_START;
		int b=0;
		int b_indx=0;
		bool flag=0;
		for(int i=0;i<last_index_best;i++)
		{
			if(best_arr[i].empty==1)
			{
				if(b==0)
					b_indx=i;
				if(best_arr[i].NumofPages+b>=neededPages)
					flag=1;
				b+=best_arr[i].NumofPages;
			}
			else
			{
				if(flag)
				{
					if(best_number_pages>b)
					{
						last_index=b_indx;
						best_number_pages=b;
					}
					flag=0;
				}
				b=0;
			}
			if(best_arr[i].va+(best_arr[i].NumofPages*PAGE_SIZE)>va)
				va=best_arr[i].va+(best_arr[i].NumofPages*PAGE_SIZE);
		}
		if(best_number_pages!=99999999)
		{
			best_arr[last_index].NumofPages=best_arr[last_index].NumofPages-neededPages;
			uint32 aq=best_arr[last_index].va;
			best_arr[last_index].va=best_arr[last_index].va+(neededPages*PAGE_SIZE);
			best_arr[last_index_best].NumofPages=neededPages;
			best_arr[last_index_best].va=aq;
			best_arr[last_index_best].empty=0;
			best_arr[last_index_best].size=size;
			sys_allocateMem(best_arr[last_index_best].va,size);
			last_index_best++;
			return (uint32*)(ROUNDDOWN(aq,PAGE_SIZE));
		}
		else
		{
			if((USER_HEAP_MAX-va)<size)
				return NULL;
			best_arr[last_index_best].NumofPages=neededPages;
			best_arr[last_index_best].va=va;
			best_arr[last_index_best].empty=0;
			best_arr[last_index_best].size=size;
			sys_allocateMem(va,size);
			last_index_best++;
			return (uint32*)ROUNDDOWN(va,PAGE_SIZE);
		}

	}
	return NULL;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side free]
	// Write your code here, remove the panic and write your code
	if(sys_isUHeapPlacementStrategyFIRSTFIT()==1)
	{
		int index=((uint32)virtual_address-USER_HEAP_START)/PAGE_SIZE;
		//cprintf("index is %d is ",index)
		arr[index].empty=0;
		arr[index].NumOfBlocks=0;
		sys_freeMem((uint32)virtual_address,arr[index].size);
		arr[index].size=0;
		for(int i=0;i<last_index_best;i++)
		{
			if(best_arr[i].va==(uint32)virtual_address)
			{
				best_arr[i].empty=1;
			}
		}
	}
	else
	{
		uint32 size=0;
		for(int i=0;i<last_index_best;i++)
		{
			if(best_arr[i].va==(uint32)virtual_address)
			{
				best_arr[i].empty=1;
				size=best_arr[i].size;
				break;
			}
		}
 		sys_freeMem((uint32)virtual_address,size);
	}
	//you should get the size of the given allocation using its address
	//refer to the documentation for details
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//
//===============
// [1] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}


//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}
