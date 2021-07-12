#ifndef INC_STRING_DATA_H_
#define INC_STRING_DATA_H_

#include "stm32f4xx_hal.h"

typedef struct String_
{
  char* data;
  size_t len;
} String;

String make_str(char const* str);
String make_str_arr(char const* begin, char const* end);
String make_str_circ(char const* buffer, size_t size, char const* begin, char const* end);
String str_copy(String const* str);
void str_free(String* str);
void str_append(String* str, char const* src);
void str_append_arr(String* str, char const* begin, char const* end);
void str_append_char(String* str, char src);
char str_pop_back(String* str);

#endif /* INC_STRING_DATA_H_ */
