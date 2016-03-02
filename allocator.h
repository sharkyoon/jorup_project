/*
 * allocator.h
 *
 *  Created on: 2016. 2. 24.
 *      Author: sharkyoon & sugeun
 */
#include <cstddef>

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

extern "C" {
  void * dlmalloc (size_t);
  void dlfree (void *);
  size_t dlmalloc_usable_size (void *);
}


#endif /* ALLOCATOR_H_ */
