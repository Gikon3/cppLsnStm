#ifndef INC_CDC_CMD_H_
#define INC_CDC_CMD_H_

#include <usbd_cdc.h>

#define CDC_CMD_BUFIN_SIZE  512
#define CDC_CMD_BUFOUT_SIZE 512

extern USBD_CDC_IfHandleType* const cmd_if;

int cdc_cmd_write(int32_t file, uint8_t* ptr, int32_t len);
int cdc_cmd_read(int32_t file, uint8_t* ptr, int32_t len);

#endif /* INC_CDC_CMD_H_ */
