#include "stm_ctrl.h"
#include "usb_device.h"
#include "chip.h"
#include "servo.h"
#include "find.h"
#include "string_data.h"
#include "cmsis_os.h"
#include "queue.h"
#include <string.h>
#include <stdlib.h>

typedef char* (*find_fp_t)(char const*, char const*, size_t);
typedef void (*proc_fp_t)(String const*);

extern osMessageQueueId_t qCmdServoHandle;

static inline char* find_begin(char const* buffer, char const* str, size_t size)
{
  return (char*)find_ar((uint8_t const*)str, size, (uint8_t const*)"#", sizeof("#") - 1);
}

static inline char* find_end(char const* buffer, char const* str, size_t size)
{
  return (char*)find_ar((uint8_t const*)str, size, (uint8_t const*)"\r\n", sizeof("\r\n") - 1);
}

static inline void data_process(String const* cmd)
{
  char* pos = NULL;
  if ((pos = strstr(cmd->data, "angle="))) {
    char* anglePos = pos + sizeof("angle=") - 1;
    ServoControl servo;
    servo.cmd = servoAngle;
    servo.val = atof(anglePos);
    xQueueSendToBack(qCmdServoHandle, &servo, portMAX_DELAY);
  }
  else if ((pos = strstr(cmd->data, "rotate="))) {
    char* rotatePos = pos + sizeof("rotate=") - 1;
    ServoControl servo;
    servo.cmd = servoRotate;
    servo.val = atof(rotatePos);
    xQueueSendToBack(qCmdServoHandle, &servo, portMAX_DELAY);
  }
  else if ((pos = strstr(cmd->data, "chipreconfig="))) {
    char* chipResetPos = pos + sizeof("chipreconfig=") - 1;
    if (*chipResetPos == 'y' && *(chipResetPos+1) == 'e' && *(chipResetPos+2) == 's') {
      chip_reconfig_ctrl(chipReconfYes);
    }
    else if (*chipResetPos == 'n' && *(chipResetPos+1) == 'o') {
      chip_reconfig_ctrl(chipReconfNo);
    }
  }
  else if (strstr(cmd->data, "chipreconfig")) {
    chip_reconfig();
  }
}

static inline char* cmd_process(char* usbBuffer, size_t usbBufferSize, char* bufferPoint,
                                find_fp_t find_begin_fp, find_fp_t find_end_fp, proc_fp_t proc_fp)
{
  size_t freeSpaceLeft = usbBuffer + usbBufferSize - bufferPoint;
  int status = usb_recv_cmd((uint8_t*)bufferPoint, freeSpaceLeft);
  size_t lenRxData = status > 0 ? status : 0;

  char* startPosition = bufferPoint;
  char* endPosition;
  char* startFind = bufferPoint;
  size_t lenTail = lenRxData;

  while (1) {
    endPosition = find_end_fp(usbBuffer, startFind, lenTail);
    if (endPosition) {
      size_t sizeForFindBegin = endPosition - startFind;
      char* beginPosition = find_begin_fp(usbBuffer, startFind, sizeForFindBegin);

      if (beginPosition) {
        String message = make_str_arr(startPosition, endPosition);
        lenTail -= message.len;

        proc_fp(&message);
        str_free(&message);
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
  static char* bufferPoint = usbCmdBuffer;
  bufferPoint = cmd_process(usbCmdBuffer, USB_BUFFER_SIZE, bufferPoint,
                            find_begin, find_end, data_process);
}
