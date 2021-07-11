#include "servo.h"
#include "tim.h"
#include <math.h>

static int8_t initFl = 0;
static float step = 2.777;
static TIM_HandleTypeDef* timer;
static uint32_t channel;

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
    uint32_t pwm = fmod(fabsf(val), 360) * step + 999;
    __HAL_TIM_SET_COMPARE(timer, channel, pwm);
  }
}

float servo_angle()
{
  if (initFl) {
    uint32_t pwm = __HAL_TIM_GET_COMPARE(timer, channel);
    return (pwm - 999) / step;
  }
  return 0.0;
}
