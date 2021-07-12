#include "chip.h"
#include "main.h"
#include "spi.h"
#include "servo.h"
#include "vector_data.h"
#include "find.h"
#include "cmsis_os.h"
#include "queue.h"
#include <string.h>

extern osMessageQueueId_t qDataHandle;

typedef enum Check_
{
  checkOk,
  checkFail,
  checkHash
} Check;
typedef enum Fsm_
{
  stBegin,
  stIdBlock,
  stErrorNum,
  stAddress,
  stError,
  stHash,
  stEnd
} Fsm;
typedef uint8_t* (*find_fp_t)(uint8_t const*, uint8_t const*, size_t);
typedef Check (*check_fp_t)(Vector const*);

static SPI_HandleTypeDef* spi;
static DMA_HandleTypeDef* dmaRx;
static osMessageQueueId_t qMessage;
static int8_t initFl = 0;
static int8_t reconfigEnableFl = 0;
static int8_t pendingRecofig = 0;

//static uint8_t msgBegin[]       = {0xF0, 0xDA, 0x00, 0x00};
static uint8_t msgEnd[]         = {0xF0, 0xDA, 0x0E, 0xFF};
static uint8_t codeCheckFail[]  = {0xF0, 0xDA, 0x0F, 0x00};
static uint8_t codeCheckHash[]  = {0xF0, 0xDA, 0x0F, 0x01};

static uint32_t opBegin = 0xF0DA0000;
static uint32_t opEnd   = 0xF0DA0EFF;
static uint32_t opStart = 0xF0DAA000;

static inline void chip_init()
{
  spi = &hspi3;
  dmaRx = &hdma_spi3_rx;
  qMessage = qDataHandle;

  initFl = 1;
}

void chip_config_force()
{
  chip_init();
  chip_rst_ctrl(chipReset);
  HAL_SPI_DMAStop(spi);
  HAL_SPI_Receive_DMA(spi, dmaSpi3Buffer, SPI_BUFFER_SIZE);
  osDelay(10 / portTICK_PERIOD_MS);
  chip_rst_ctrl(chipUnreset);
}

void chip_config()
{
  if (reconfigEnableFl == chipReconfNo) return;

  chip_config_force();
}

void chip_rst_ctrl(ChipReset reset)
{
  switch (reset) {
    case chipReset: HAL_GPIO_WritePin(CPP_nRST_POR_GPIO_Port, CPP_nRST_POR_Pin, GPIO_PIN_RESET); break;
    case chipUnreset: HAL_GPIO_WritePin(CPP_nRST_POR_GPIO_Port, CPP_nRST_POR_Pin, GPIO_PIN_SET); break;
  }
}

//static inline uint8_t* find_begin(uint8_t const* buffer, uint8_t const* str, size_t size)
//{
//  return find_circular(buffer, SPI_BUFFER_SIZE, str, sizeof(msgBegin), msgBegin, sizeof(msgBegin));
//}

static inline uint8_t* find_end(uint8_t const* buffer, uint8_t const* str, size_t size)
{
  return find_circular(buffer, SPI_BUFFER_SIZE, str, size, msgEnd, sizeof(msgEnd));
}

static inline Check check_hash(Vector const* msg, size_t id)
{
  return checkOk;
}

static inline Check check_message(Vector const* msg)
{
  Fsm state = stBegin;
  size_t errorsNum = 0;
  size_t errorsCount = 0;
  for (size_t i = 0; i < msg->size; i += 4) {
    uint32_t word = msg->data[i] << 24 | msg->data[i+1] << 16 | msg->data[i+2] << 8 | msg->data[i+3];

    switch (state) {
      case stBegin:
        if (word == opStart) state = stBegin;
        else if (word == opBegin) state = stIdBlock;
        else return checkFail;
        break;
      case stIdBlock:
        if (word == opBegin || word == opEnd) return checkFail;
        else state = stErrorNum;
        break;
      case stErrorNum:
        if (word == 0 || word == opStart) {
          state = stHash;
        }
        else {
          state = stAddress;
          errorsNum = word <= CHIP_ERRORS_MAX ? word : CHIP_ERRORS_MAX;
        }
        break;
      case stAddress:
        if (word == opBegin || word == opEnd) return checkFail;
        else state = stError;
        break;
      case stError:
        if (word == opBegin || word == opEnd) {
          return checkFail;
        }
        else if (errorsCount < errorsNum) {
          state = stAddress;
          ++errorsCount;
        }
        if (errorsCount == errorsNum) {
          state = stHash;
        }
        break;
      case stHash:
        if (check_hash(msg, i) != checkOk) return checkHash;
        state = stEnd;
        break;
      case stEnd:
        if (word != opEnd) return checkFail;
        break;
    }
  }

  return checkOk;
}

static inline uint8_t* msg_process(SPI_HandleTypeDef* spi, DMA_HandleTypeDef* dmaRx,
                                  uint8_t* spiBuffer, size_t spiBufferSize, uint8_t* bufferPoint,
                                  find_fp_t find_end_fp, check_fp_t check_fp)
{
  size_t dmaRxSize = spiBufferSize - dmaRx->Instance->NDTR;
  uint8_t* endRxDmaBuffer = spiBuffer + dmaRxSize;
  size_t lenRxData = (endRxDmaBuffer >= bufferPoint) ? endRxDmaBuffer - bufferPoint:
      dmaRxSize + spiBuffer + spiBufferSize - bufferPoint;

  uint8_t* startPosition = bufferPoint;
  uint8_t* endPosition;
  uint8_t* startFind = bufferPoint;
  size_t lenTail = lenRxData;

  while (1) {
    endPosition = find_end_fp(spiBuffer, startFind, lenTail);
    if (endPosition) {
      Vector message = make_vector_buf_circ(spiBuffer, spiBufferSize, startPosition, endPosition);
      lenTail -= message.size;

      Check status = check_fp(&message);
      if (status == checkOk) {
        uint8_t angleCode[4] = { 0 };
        uint16_t angle = servo_angle();
        angleCode[1] = angle >> 8;
        angleCode[0] = angle;
        vector_append_ar(&message, angleCode, &angleCode[sizeof(angleCode)]);
      }
      else if (status == checkFail) {
        vector_free(&message);
        message = make_vector_ar(codeCheckFail, &codeCheckFail[sizeof(codeCheckFail)]);
      }
      else if (status == checkHash) {
        vector_free(&message);
        message = make_vector_ar(codeCheckHash, &codeCheckHash[sizeof(codeCheckHash)]);
      }
      xQueueSendToBack(qMessage, &message, portMAX_DELAY);

      if (status != checkOk || pendingRecofig) {
        pendingRecofig = 0;
        if (!pendingRecofig) chip_config();
        else chip_config_force();
        return spiBuffer;
      }

      startPosition = endPosition;
      startFind = endPosition;
      continue;
    }
    break;
  }
  return startFind;
}

void chip_msg_proc()
{
  static uint8_t* bufferPoint = dmaSpi3Buffer;
  if (initFl) {
    bufferPoint = msg_process(spi, dmaRx, dmaSpi3Buffer, SPI_BUFFER_SIZE, bufferPoint,
                              find_end, check_message);
  }
}

void chip_reconfig_ctrl(ChipReconfig ctrl)
{
  reconfigEnableFl = ctrl;
}

void chip_reconfig()
{
  pendingRecofig = 1;
}
