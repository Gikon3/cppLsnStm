#ifndef INC_VECTOR_DATA_H_
#define INC_VECTOR_DATA_H_

#include "stm32f4xx_hal.h"

typedef struct Vector_
{
  uint8_t* data;
  size_t size;
  size_t allocSize;
} Vector;

Vector make_vector();
Vector make_vector_sz(size_t size);
Vector make_vector_ar(uint8_t const* begin, uint8_t const* end);
Vector make_vector_buf_circ(uint8_t const* buf, size_t bufSize, uint8_t const* begin, uint8_t const* end);
void vector_free(Vector* vec);
uint8_t vector_at(Vector const* vec, size_t id, int8_t* stat);
void vector_resize(Vector* vec, size_t newSize);
void vector_append(Vector* vec, uint8_t data);
void vector_append_ar(Vector* vec, uint8_t const* begin, uint8_t const* end);
void vector_append_vct(Vector* vec, Vector const* src);

#endif /* INC_VECTOR_DATA_H_ */
