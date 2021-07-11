#ifndef INC_CDC_DATA_H_
#define INC_CDC_DATA_H_

#include <usbd_cdc.h>

#define CDC_DATA_BUFIN_SIZE  2048
#define CDC_DATA_BUFOUT_SIZE 2048

extern USBD_CDC_IfHandleType* const data_if;

int cdc_data_write(int32_t file, uint8_t* ptr, int32_t len);
int cdc_data_read(int32_t file, uint8_t* ptr, int32_t len);

#endif /* INC_CDC_DATA_H_ */
