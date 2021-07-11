#include "usb_device.h"
#include "usbd_cdc.h"
#include "cdc_cmd.h"
#include "cdc_data.h"

uint8_t usbCmdBuffer[USB_BUFFER_SIZE];

const USBD_DescriptionType hdev_cfg = {
  .Vendor = {
    .Name           = "STMicroelectronics",
    .ID             = 0x0483,
  },
  .Product = {
    .Name           = "STM32 CPP LSN",
    .ID             = 0x5740,
    .Version.bcd    = 0x0100,
  },
  .Config = {
    .Name           = "STM32 CPP LSN config",
    .MaxCurrent_mA  = 100,
    .RemoteWakeup   = 0,
    .SelfPowered    = 0,
  },
}, *const dev_cfg = &hdev_cfg;

USBD_HandleType hUsbDevice, *const UsbDevice = &hUsbDevice;

void usb_init(void)
{
    /* All fields of Config have to be properly set up */
    data_if->Config.InEpNum  = 0x81;
    data_if->Config.OutEpNum = 0x01;
    data_if->Config.NotEpNum = 0x82;

    cmd_if->Config.InEpNum  = 0x83;
    cmd_if->Config.OutEpNum = 0x03;
    cmd_if->Config.NotEpNum = 0x84;

    /* Mount the interfaces to the device */
    USBD_CDC_MountInterface(data_if, UsbDevice);
    USBD_CDC_MountInterface(cmd_if, UsbDevice);

    /* Initialize the device */
    USBD_Init(UsbDevice, dev_cfg);

    /* The device connection can be made */
    USBD_Connect(UsbDevice);
}

int usb_send_cmd(uint8_t* buf, int32_t len)
{
  return cdc_cmd_write(0, buf, len);
}

int usb_recv_cmd(uint8_t* buf, int32_t len)
{
  return cdc_cmd_read(0, buf, len);
}

int usb_send_data(uint8_t* buf, int32_t len)
{
  return cdc_data_write(0, buf, len);
}

int usb_recv_data(uint8_t* buf, int32_t len)
{
  return cdc_data_read(0, buf, len);
}
