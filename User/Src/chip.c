#include "chip.h"
#include "main.h"
#include "spi.h"
#include "servo.h"
#include "vector_data.h"
#include "find.h"
#include "cmsis_os.h"
#include "queue.h"
#include <string.h>
#include <math.h>

#define SILENCE_THR   (500 / portTICK_PERIOD_MS)

extern osMessageQueueId_t qDataHandle;

typedef enum {
  checkOk,
  checkOrderFail,
  checkHashFail,
  checkTimeFail,
  checkNone
} Check;
typedef enum {
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
static ChipReconfig autoReconfigFl = chipReconfYes;
static int8_t pendingReconfig = 0;

static uint8_t msgEnd[]        = {0xF0, 0xDA, 0x0E, 0xFF};
static uint8_t codeOrderFail[] = {0xF0, 0xDA, 0x0F, 0x00};
static uint8_t codeHashFail[]  = {0xF0, 0xDA, 0x0F, 0x01};
static uint8_t codeSilence[]   = {0xF0, 0xDA, 0x0F, 0x02};

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
  if (autoReconfigFl == chipReconfNo) return;
  chip_config_force();
}

void chip_rst_ctrl(ChipReset reset)
{
  switch (reset) {
    case chipReset: HAL_GPIO_WritePin(CPP_nRST_POR_GPIO_Port, CPP_nRST_POR_Pin, GPIO_PIN_RESET); break;
    case chipUnreset: HAL_GPIO_WritePin(CPP_nRST_POR_GPIO_Port, CPP_nRST_POR_Pin, GPIO_PIN_SET); break;
  }
}

static inline uint8_t* find_end(uint8_t const* buffer, uint8_t const* str, size_t size)
{
  return find_circular(buffer, SPI_BUFFER_SIZE, str, size, msgEnd, sizeof(msgEnd));
}

static inline Check check_hash(Vector const* msg)
{
  const size_t size = msg->size - 8;
  const uint32_t hash = msg->data[size] << 24 | msg->data[size+1] << 16 | msg->data[size+2] << 8 | msg->data[size+3];
  uint32_t calcHash = 0;
  for (size_t i = 0; i < size; i += 4) {
    const uint32_t word = msg->data[i] << 24 | msg->data[i+1] << 16 | msg->data[i+2] << 8 | msg->data[i+3];
    if (word == opStart) continue;
    calcHash ^= word;
  }
  if (calcHash != hash) return checkHashFail;
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
        else return checkOrderFail;
        break;
      case stIdBlock:
        if (word == opBegin || word == opEnd) return checkOrderFail;
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
        if (word == opBegin || word == opEnd) return checkOrderFail;
        else state = stError;
        break;
      case stError:
        if (word == opBegin || word == opEnd) {
          return checkOrderFail;
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
        if (check_hash(msg) != checkOk) return checkHashFail;
        state = stEnd;
        break;
      case stEnd:
        if (word != opEnd) return checkOrderFail;
        break;
    }
  }

  return checkOk;
}

static inline uint8_t* msg_process(SPI_HandleTypeDef* spi, DMA_HandleTypeDef* dmaRx,
                                  uint8_t* spiBuffer, size_t spiBufferSize, uint8_t* bufferPoint,
                                  find_fp_t find_end_fp, check_fp_t check_fp)
{
  static TickType_t lastMsgTime = 0;
  if (lastMsgTime == 0) lastMsgTime = xTaskGetTickCount();

  size_t dmaRxSize = spiBufferSize - dmaRx->Instance->NDTR;
  uint8_t* endRxDmaBuffer = spiBuffer + dmaRxSize;
  size_t lenRxData = (endRxDmaBuffer >= bufferPoint) ? endRxDmaBuffer - bufferPoint:
      dmaRxSize + spiBuffer + spiBufferSize - bufferPoint;

  uint8_t* startPosition = bufferPoint;
  uint8_t* endPosition;
  uint8_t* startFind = bufferPoint;
  size_t lenTail = lenRxData;

  Check status = checkNone;

  while (1) {
    endPosition = find_end_fp(spiBuffer, startFind, lenTail);
    if (endPosition) {
      lastMsgTime = xTaskGetTickCount();
      Vector message = make_vector_buf_circ(spiBuffer, spiBufferSize, startPosition, endPosition);
      lenTail -= message.size;

      status = check_fp(&message);
      if (status == checkOk) {
        uint8_t angleCode[4] = { 0 };
        uint16_t angle = (float)lrint(servo_angle());
        angleCode[2] = angle >> 8;
        angleCode[3] = angle;
        vector_append_ar(&message, angleCode, &angleCode[sizeof(angleCode)]);
      }
      else if (status == checkOrderFail) {
        vector_free(&message);
        message = make_vector_ar(codeOrderFail, &codeOrderFail[sizeof(codeOrderFail)]);
      }
      else if (status == checkHashFail) {
        vector_free(&message);
        message = make_vector_ar(codeHashFail, &codeHashFail[sizeof(codeHashFail)]);
      }
      xQueueSendToBack(qMessage, &message, portMAX_DELAY);

      startPosition = endPosition;
      startFind = endPosition;

      if (status == checkOk) continue;
    }
    else if (xTaskGetTickCount() - lastMsgTime > SILENCE_THR) {
      lastMsgTime = xTaskGetTickCount();
      Vector message = make_vector_ar(codeSilence, &codeSilence[sizeof(codeSilence)]);
      xQueueSendToBack(qMessage, &message, portMAX_DELAY);
      status = checkTimeFail;
    }

    if ((status != checkNone && status != checkOk) || pendingReconfig) {
      if (!pendingReconfig) chip_config();
      else chip_config_force();
      pendingReconfig = 0;

      return spiBuffer;
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
  autoReconfigFl = ctrl;
}

ChipReconfig chip_reconfig_enable()
{
  return autoReconfigFl;
}

void chip_reconfig()
{
  pendingReconfig = 1;
}
