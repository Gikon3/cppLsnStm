#include "string_data.h"
#include <string.h>
#include "cmsis_os.h"

String make_str(char const* str)
{
  String tmp;
  tmp.len = strlen(str);
  tmp.data = pvPortMalloc(tmp.len + 1);
  strcpy(tmp.data, str);
  return tmp;
}

String make_str_arr(char const* begin, char const* end)
{
  String str;
  str.len = end - begin;
  str.data = pvPortMalloc(str.len + 1);
  memcpy(str.data, begin, str.len);
  str.data[str.len] = '\0';
  return str;
}

String make_str_circ(char const* buffer, size_t size, char const* begin, char const* end)
{
  String str;
  str.len = (end > begin) ? end - begin: size - (begin - end);
  str.data = pvPortMalloc(str.len + 1);
  if (end > begin) {
    memcpy(str.data, begin, str.len);
  }
  else {
    size_t tailSize = buffer + size - begin;
    size_t headSize = end - buffer;
    memcpy(str.data, begin, tailSize);
    memcpy(str.data + tailSize, buffer, headSize);
  }
  str.data[str.len] = '\0';
  return str;
}

String str_copy(String const* str)
{
  String tmp;
  tmp.len = str->len;
  tmp.data = pvPortMalloc(tmp.len + 1);
  strcpy(tmp.data, str->data);
  return tmp;
}

void str_free(String* str)
{
  vPortFree(str->data);
  str->data = NULL;
  str->len = 0;
}

void str_append(String* str, char const* src)
{
  size_t srcLen = strlen(src);
  if (!srcLen) {
    return;
  }

  if (str->data) {
    size_t newLen = str->len + srcLen;
    char* dataTemp = pvPortMalloc(newLen + 1);
    strcpy(dataTemp, str->data);
    strcpy(&dataTemp[str->len], src);
    vPortFree(str->data);
    str->data = dataTemp;
    str->len = newLen;
  }
  else {
    str->len = strlen(src);
    str->data = pvPortMalloc(str->len + 1);
    strcpy(str->data, src);
  }
}

void str_append_arr(String* str, char const* begin, char const* end)
{
  size_t srcLen = end - begin;
  if (!srcLen) {
    return;
  }

  if (str->data) {
    size_t newLen = str->len + srcLen;
    char* dataTemp = pvPortMalloc(newLen + 1);
    strcpy(dataTemp, str->data);
    memcpy(&dataTemp[str->len], begin, srcLen);
    dataTemp[newLen] = '\0';
    vPortFree(str->data);
    str->data = dataTemp;
    str->len = newLen;
  }
  else {
    str->len = srcLen;
    str->data = pvPortMalloc(str->len + 1);
    memcpy(str->data, begin, srcLen);
  }
}

void str_append_char(String* str, char src)
{
  if (str->data) {
    size_t newLen = str->len + 1;
    char* dataTemp = pvPortMalloc(newLen + 1);
    strcpy(dataTemp, str->data);
    dataTemp[str->len] = src;
    dataTemp[str->len+1] = '\0';
    vPortFree(str->data);
    str->data = dataTemp;
    str->len = newLen;
  }
  else {
    str->len = 1;
    str->data = pvPortMalloc(str->len + 1);
    str->data[0] = src;
    str->data[1] = '\0';
  }
}

char str_pop_back(String* str)
{
  if (str->data) {
    --str->len;
    char temp = str->data[str->len];
    str->data[str->len] = '\0';
    return temp;
  }
  return '\0';
}
