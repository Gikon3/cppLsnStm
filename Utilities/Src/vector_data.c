#include "vector_data.h"
#include "cmsis_os.h"
#include <string.h>

void (*mem_free_fp)(void*) = vPortFree;
void* (*mem_malloc_fp)(size_t) = pvPortMalloc;

Vector make_vector()
{
  Vector vec;
  vec.data = NULL;
  vec.size = 0;
  vec.allocSize = 0;
  return vec;
}

Vector make_vector_sz(size_t size)
{
  if (!size) return make_vector();
  Vector vec;
  vec.data = mem_malloc_fp(size);
  vec.size = 0;
  vec.allocSize = size;
  return vec;
}

Vector make_vector_ar(uint8_t const* begin, uint8_t const* end)
{
  const size_t size = end - begin;
  Vector vec;
  vec.data = mem_malloc_fp(size);
  memcpy(vec.data, begin, size);
  vec.size = size;
  vec.allocSize = size;
  return vec;
}

Vector make_vector_buf_circ(uint8_t const* buf, size_t bufSize, uint8_t const* begin, uint8_t const* end)
{
  size_t size = end >= begin ? end - begin : bufSize - (begin - end);
  if (!size) return make_vector();

  if (end > begin) return make_vector_ar(begin, end);
  Vector vec = make_vector_sz(size);
  vector_append_ar(&vec, begin, buf + bufSize);
  vector_append_ar(&vec, buf, end);
  return vec;
}

void vector_free(Vector* vec)
{
  mem_free_fp(vec->data);
  vec->data = NULL;
  vec->size = 0;
  vec->allocSize = 0;
}

uint8_t vector_at(Vector const* vec, size_t id, int8_t* stat)
{
  if (id < vec->size) {
    *stat = 0;
    return vec->data[id];
  }
  *stat = -1;
  return 0;
}

void vector_resize(Vector* vec, size_t newSize)
{
  if (vec->allocSize == newSize) return;

  uint8_t* temp = mem_malloc_fp(newSize);
  const size_t copyNumber = vec->size < newSize ? vec->size : newSize;
  memcpy(temp, vec->data, copyNumber);
  mem_free_fp(vec->data);
  vec->data = temp;
  vec->size = copyNumber;
  vec->allocSize = newSize;
}

void vector_append(Vector* vec, uint8_t data)
{
  if (vec->size == vec->allocSize) {
    vector_resize(vec, vec->allocSize * 2);
  }
  vec->data[vec->size] = data;
  ++vec->size;
}

void vector_append_ar(Vector* vec, uint8_t const* begin, uint8_t const* end)
{
  size_t sizeAppend = end - begin;
  if (!sizeAppend) return;

  size_t requiredSize = vec->size + sizeAppend;
  if (requiredSize > vec->allocSize) {
    size_t newAllocSize = 0;
    do {
      newAllocSize = vec->allocSize * 2;
    } while (newAllocSize >= requiredSize);
    vector_resize(vec, newAllocSize);
  }

  memcpy(vec->data + vec->size, begin, sizeAppend);
  vec->size += sizeAppend;
}

void vector_append_vct(Vector* vec, Vector const* src)
{
  if (!src->size) return;

  size_t requiredSize = vec->size + src->size;
  if (requiredSize > vec->allocSize) {
    size_t newAllocSize = 0;
    do {
      newAllocSize = vec->allocSize * 2;
    } while (newAllocSize >= requiredSize);
    vector_resize(vec, newAllocSize);
  }

  memcpy(vec->data + vec->size, src->data, src->size);
  vec->size += src->size;
}
