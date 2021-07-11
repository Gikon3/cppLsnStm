#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "stm32f4xx_hal.h"

typedef enum ServoCmd_
{
  servoAngle,
  servoRotate
} ServoCmd;

typedef struct ServoControl_
{
  ServoCmd cmd;
  float val;
} ServoControl;

void servo_config();
void servo_set_angle(float val);
float servo_angle();

#endif /* INC_SERVO_H_ */
