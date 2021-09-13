#include "stm32f4xx_hal.h"

#include "xpd_usb.h"
#include "usb_device.h"

void HAL_USB_OTG_FS_MspInit(void* handle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**USB_OTG_FS GPIO Configuration
  PA8     ------> USB_OTG_FS_SOF
  PA9     ------> USB_OTG_FS_VBUS
  PA11     ------> USB_OTG_FS_DM
  PA12     ------> USB_OTG_FS_DP
  */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

void HAL_USB_OTG_FS_MspDeInit(void* handle)
{
  /* Peripheral clock disable */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /**USB_OTG_FS GPIO Configuration
  PA8     ------> USB_OTG_FS_SOF
  PA9     ------> USB_OTG_FS_VBUS
  PA11     ------> USB_OTG_FS_DM
  PA12     ------> USB_OTG_FS_DP
  */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12);

  /* Peripheral interrupt Deinit*/
  HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
}

void HAL_USBD_Setup(void)
{
    USB_INST2HANDLE(UsbDevice, USB_OTG_FS);
    UsbDevice->Callbacks.DepInit = HAL_USB_OTG_FS_MspInit;
    UsbDevice->Callbacks.DepDeinit = HAL_USB_OTG_FS_MspDeInit;
}

void OTG_FS_IRQHandler(void)
{
    USB_vIRQHandler(UsbDevice);
}
