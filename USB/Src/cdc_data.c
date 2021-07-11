#include "cdc_data.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#undef errno
extern int32_t errno;

static struct {
  uint16_t head;
  uint16_t tail;
  uint8_t buffer[CDC_DATA_BUFIN_SIZE + 1];
} data_if_in;

static struct {
  uint16_t head;
  uint16_t tail;
  uint8_t buffer[CDC_DATA_BUFOUT_SIZE + 1];
} data_if_out;

static void data_if_open(void* itf, USBD_CDC_LineCodingType * lc);
static void data_if_out_cmplt(void* itf, uint8_t* pbuf, uint16_t length);
static void data_if_in_cmplt(void* itf, uint8_t* pbuf, uint16_t length);
static void data_if_recv();
static void data_if_send();

static const USBD_CDC_AppType data_app =
{
  .Name         = "VSP for data",
  .Open         = data_if_open,
  .Received     = data_if_out_cmplt,
  .Transmitted  = data_if_in_cmplt
};

USBD_CDC_IfHandleType data_if_ =
{
  .App            = &data_app,
  .Base.AltCount  = 1
};
USBD_CDC_IfHandleType* const data_if = &data_if_;


static void data_if_open(void* itf, USBD_CDC_LineCodingType * lc)
{
  data_if_in.head = data_if_in.tail = 0;
  data_if_out.head = data_if_out.tail = 0;
  data_if_recv();
}

static void data_if_in_cmplt(void* itf, uint8_t * pbuf, uint16_t length)
{
  if (data_if_in.tail < CDC_DATA_BUFIN_SIZE) {
    data_if_in.tail += length;
  }
  else {
    data_if_in.tail = length - 1;
  }
  data_if_send();
}

static void data_if_send()
{
  uint16_t head = data_if_in.head;
  uint16_t tail = data_if_in.tail;
  uint16_t start = tail + 1;
  uint16_t length;

  if (tail <= head) {
      length = head - tail;
  }
  else if (tail < CDC_DATA_BUFIN_SIZE) {
      length = CDC_DATA_BUFIN_SIZE - tail;
  }
  else {
      length = head + 1;
      start = 0;
  }

  if (length > 0) {
    USBD_CDC_Transmit(data_if, &data_if_in.buffer[start], length);
  }
}

int cdc_data_write(int32_t file, uint8_t* ptr, int32_t len)
{
  int retval = -1;
  uint16_t head = data_if_in.head;
  uint16_t tail = data_if_in.tail;

  if (data_if->LineCoding.DataBits == 0) {
    errno = -EIO;
  }
  else if (((tail > head) ?
      (tail - head - 1) : (CDC_DATA_BUFIN_SIZE - (head - tail))) < len) {
    errno = -ENOMEM;
  }
  else {
    uint16_t len1 = 0;
    uint16_t len2 = 0;

    if (tail > head) {
      /* continuous */
      len1 = tail - head - 1;
      if (len < len1) {
        len1 = len;
      }
    }
    else {
      /* two chunks */
      len1 = CDC_DATA_BUFIN_SIZE - head;

      if (len <= len1) {
        len1 = len;
      }
      else if (len < (len1 + tail)) {
        len2 = len - len1;
      }
      else {
        len2 = tail;
      }
    }

    /* first chunk is copied starting from current head */
    memcpy(&data_if_in.buffer[head + 1], ptr, len1);
    data_if_in.head += len1;
    ptr += len1;

    /* the remaining chunk is copied from the buffer start */
    if (len2 > 0) {
      memcpy(&data_if_in.buffer[0], ptr, len2);
      data_if_in.head = len2 - 1;
    }

    retval = len1 + len2;
    data_if_send();
  }

  return retval;
}

static void data_if_out_cmplt(void* itf, uint8_t* pbuf, uint16_t length)
{
  if (data_if_out.head < CDC_DATA_BUFOUT_SIZE) {
    data_if_out.head += length;
  }
  else {
    data_if_out.head = length - 1;
  }
  data_if_recv();
}

static void data_if_recv()
{
  uint16_t tail = data_if_out.tail;
  uint16_t head = data_if_out.head;
  uint16_t start = head + 1;
  uint16_t length;

  if (tail > head) {
    length = tail - head - 1;
  }
  else if (head < CDC_DATA_BUFOUT_SIZE) {
    length = CDC_DATA_BUFOUT_SIZE - head;
  }
  else {
    length = tail;
    start = 0;
  }

  if (length > 0) {
    USBD_CDC_Receive(data_if, &data_if_out.buffer[start], length);
  }
}

int cdc_data_read(int32_t file, uint8_t* ptr, int32_t len)
{
    int retval = -1;
    uint16_t tail = data_if_out.tail;
    uint16_t head = data_if_out.head;

    if (data_if->LineCoding.DataBits == 0) {
      errno = -EIO;
    }
    else {
      uint16_t len1, len2 = 0;

      if (tail <= head) {
        /* continuous */
        len1 = head - tail;
        if (len < len1) {
          len1 = len;
        }
      }
      else {
        /* two chunks */
        len1 = CDC_DATA_BUFOUT_SIZE - tail;

        if (len <= len1) {
          len1 = len;
        }
        else if (len < (len1 + head + 1)) {
          len2 = len - len1;
        }
        else {
          len2 = head + 1;
        }
      }

      /* first chunk is copied starting from current tail */
      memcpy(ptr, &data_if_out.buffer[tail + 1], len1);
      data_if_out.tail += len1;
      ptr += len1;

      /* the remaining chunk is copied from the buffer start */
      if (len2 > 0) {
        memcpy(ptr, &data_if_out.buffer[0], len2);
        data_if_out.tail = len2 - 1;
      }

      retval = len1 + len2;
      if (retval > 0) {
        data_if_recv();
      }
    }

    return retval;
}
