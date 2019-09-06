#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "error.h"

struct mem_info {
	struct {
		size_t len;
		const char *owner;
		struct mem_info *next;
	} header;
	uint8_t ptr[0];
};

static struct mem_info *head;

error_code _mem_new(void **ptr, size_t len, const char* caller)
{
	struct mem_info *mem;

	mem = malloc(sizeof(mem->header) + len);
	if (!mem)
		return ERROR_MEM;

	memset(mem, 0, sizeof(mem->header) + len);
	mem->header.len = len;
	mem->header.owner = caller;
	mem->header.next = NULL;
	if (!head) {
		head = mem;
	} else {
		struct mem_info *current;

		current = head;
		while (current->header.next != NULL) {
			current = current->header.next;
		}
		current->header.next = mem;
	}

	*ptr = mem->ptr;

	return SUCCESS;
}

error_code mem_free(void *ptr)
{
	if (!ptr)
		return ERROR_ALREADY;

	if (!head)
		return ERROR_NOT_FOUND;

	struct mem_info *current, *prev;

	current = head;
	prev = NULL;


	if (ptr == current->ptr)
	{
		head = current->header.next;
		printf("freeing %p head %p", current, head);
		free(current);
		return SUCCESS;
	}

	while (current->header.next != NULL && ptr != current->ptr) {

		prev = current;
		current = current->header.next;
	}

	if (!current)
		return ERROR_NOT_FOUND;


	prev->header.next = current->header.next;


	printf("freeing %p head %p", current, head);
	free(current);

	return SUCCESS;

}

static void mem_print(struct mem_info *mem)
{
	printf("%s allocates %ld bytes @ %p\n",
			mem->header.owner, mem->header.len, mem->ptr);
}

void mem_info(void)
{
	if (!head) {
		printf("no memory allocated\n");
		return;
	}
	struct mem_info *current;

	current = head;
	while (current->header.next != NULL) {
		mem_print(current);
		current = current->header.next;
	}
	mem_print(current);
}
