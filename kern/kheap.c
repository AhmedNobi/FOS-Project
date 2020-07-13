#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>


void* kmalloc(unsigned int size)
{
	panic("this function is not required...!!");
	return NULL;
}

void kfree(void* virtual_address)
{
	panic("this function is ffffnot required...!!");
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	panic("this function is sssnot required...!!");
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	panic("this function is dddnot required...!!");
	return 0;
}
