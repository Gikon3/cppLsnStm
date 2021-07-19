#include "servo.h"
#include "tim.h"
#include <math.h>

static int8_t initFl = 0;
static double step = (2500 - 500) / 270.0; // pulseRange / 270
static TIM_HandleTypeDef* timer;
static uint32_t channel;
static const uint32_t zeroPoint = 499;

static inline void init()
{
  timer = &htim1;
  channel = TIM_CHANNEL_1;
  initFl = 1;
}

void servo_config()
{
  init();
  HAL_TIM_PWM_Start(timer, channel);
}

void servo_set_angle(float val)
{
  if (initFl) {
    double mod360 = fmod(val, 360);
    double angle = mod360 >= 0 ? mod360 : 360.0 + mod360;
    if (angle > 270.0) angle = 270.0;
    uint32_t pwm = angle * step + zeroPoint;
    __HAL_TIM_SET_COMPARE(timer, channel, pwm);
  }
}

float servo_angle()
{
  if (initFl) {
    uint32_t pwm = __HAL_TIM_GET_COMPARE(timer, channel);
    return (pwm - zeroPoint) / step;
  }
  return 0.0;
}
