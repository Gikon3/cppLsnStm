#include "stm_ctrl.h"
#include "usb_device.h"
#include "find.h"
#include "vector_data.h"
#include <string.h>

typedef uint8_t* (*find_fp_t)(uint8_t const*, uint8_t const*, size_t);
typedef void (*proc_fp_t)(Vector const*);

static inline uint8_t* find_begin(uint8_t const* buffer, uint8_t const* str, size_t size)
{
  return find_ar(str, size, (uint8_t*)"#", sizeof("#") - 1);
}

static inline uint8_t* find_end(uint8_t const* buffer, uint8_t const* str, size_t size)
{
  return find_ar(str, size, (uint8_t*)"\r\n", sizeof("\r\n") - 1);
}

static inline void data_process(Vector const* cmd)
{

}

static inline uint8_t* cmd_process(uint8_t* usbBuffer, size_t usbBufferSize, uint8_t* bufferPoint,
                                   find_fp_t find_begin_fp, find_fp_t find_end_fp, proc_fp_t proc_fp)
{
  size_t freeSpaceLeft = usbBuffer + usbBufferSize - bufferPoint;
  int status = usb_recv_cmd(bufferPoint, freeSpaceLeft);
  size_t lenRxData = status > 0 ? status : 0;

  uint8_t* startPosition = bufferPoint;
  uint8_t* endPosition;
  uint8_t* startFind = bufferPoint;
  size_t lenTail = lenRxData;

  while (1) {
    endPosition = find_end_fp(usbBuffer, startFind, lenTail);
    if (endPosition) {
      size_t sizeForFindBegin = endPosition - startFind;
      uint8_t* beginPosition = find_begin_fp(usbBuffer, startFind, sizeForFindBegin);

      if (beginPosition) {
        Vector message = make_vector_ar(startPosition, endPosition);
        lenTail -= message.size;

        proc_fp(&message);
      }

      startPosition = endPosition;
      startFind = endPosition;
      continue;
    }

    memcpy(usbBuffer, startFind, lenTail);
    startPosition = usbBuffer;
    startFind = usbBuffer;
    break;
  }

  return startFind;
}

void ctrl_cmd_proc()
{
  static uint8_t* bufferPoint = usbCmdBuffer;
  bufferPoint = cmd_process(usbCmdBuffer, USB_BUFFER_SIZE, bufferPoint,
                            find_begin, find_end, data_process);
}
