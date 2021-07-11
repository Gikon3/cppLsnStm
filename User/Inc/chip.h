#ifndef INC_CHIP_H_
#define INC_CHIP_H_

#include "stm32f4xx_hal.h"

typedef enum ChipReset_
{
  chipReset,
  chipUnreset
} ChipReset;

void chip_config();
void chip_rst_ctrl(ChipReset reset);
void chip_msg_proc();

#endif /* INC_CHIP_H_ */
