#ifndef INC_CHIP_H_
#define INC_CHIP_H_

#include "stm32f4xx_hal.h"

#define CHIP_ERRORS_MAX   64

typedef enum {
  chipReset,
  chipUnreset
} ChipReset;

typedef enum {
  chipReconfYes,
  chipReconfNo
} ChipReconfig;

void chip_config_force();
void chip_config();
void chip_rst_ctrl(ChipReset reset);
void chip_msg_proc();
void chip_reconfig_ctrl(ChipReconfig ctrl);
ChipReconfig chip_reconfig_enable();
void chip_reconfig();

#endif /* INC_CHIP_H_ */
