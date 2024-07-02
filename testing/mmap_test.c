#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef struct {
  void *address;
  size_t allocated;
  int mmap_p : 1;
} allocator_chunk_t;

allocator_chunk_t *allocator_list = NULL;
size_t allocator_list_allocated = 0, allocator_list_length = 0;

size_t allocator_used = 0;

static const size_t allocator_max_malloc = 1024 /* bytes */;

void *allocator_alloc(
  size_t size
) {
  if (allocator_list == NULL)
    allocator_list = malloc(0);

  if (allocator_list_allocated <= allocator_list_length) {
    allocator_list_allocated = (allocator_list_allocated + 1) * 3 / 2;
    allocator_list = realloc(allocator_list, sizeof(*allocator_list) * allocator_lost_allocated);
  }

  if (allocator_used > allocator_max_malloc) {
    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
  } else {
    // use malloc
  }
}

int allocator_free(
  void *pointer
) {
  for (size_t position = 0; position < allocator_list_length; ++position) {
    allocator_chunk_t current = allocator_list[position];

    if (current.address == pointer) {
      if (current.mmap_p)
        munmap(current.address, current.allocated);
      else
        free(current.address);
    }

    allocator_used -= current.allocated;

    allocator_list[position] = (allocator_chunk_t){ NULL, 0, 0 };
    --allocator_list_length;

    return 1;
  }

  return 0;
}
