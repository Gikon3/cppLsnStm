#ifndef INC_CHIP_H_
#define INC_CHIP_H_

#include "stm32f4xx_hal.h"

#define CHIP_ERRORS_MAX   64

typedef enum ChipReset_
{
  chipReset,
  chipUnreset
} ChipReset;

typedef enum ChipReconfig_
{
  chipReconfYes,
  chipReconfNo
} ChipReconfig;

void chip_config_force();
void chip_config();
void chip_rst_ctrl(ChipReset reset);
void chip_msg_proc();
void chip_reconfig_ctrl(ChipReconfig ctrl);
void chip_reconfig();

#endif /* INC_CHIP_H_ */
