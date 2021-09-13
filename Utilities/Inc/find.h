#ifndef INC_FIND_H_
#define INC_FIND_H_

#include "stm32f4xx_hal.h"

uint8_t* find_ar(uint8_t const* str, size_t sizeStr,
                 uint8_t const* subStr, size_t sizeSubStr);
uint8_t* find_circular(uint8_t const* buffer, size_t sizeBuf,
                       uint8_t const* str, size_t sizeStr,
                       uint8_t const* subStr, size_t sizeSubStr);

#endif /* INC_FIND_H_ */
