#include "w25qxx.h"
#include "w25qxx_def.h"
#include <stddef.h>
#include <stdint.h>

// 交换数据操作
static W25QXXErrorType W25QXX_SwapData(W25QXXObjectType *w25qxx,
                                       uint8_t *txBuff, uint16_t txLen,
                                       uint8_t *rxBuff, uint16_t rxLen);
// 写使能
static W25QXXErrorType W25QXX_WriteEnable(W25QXXObjectType *w25qxx);
// 忙等待
static W25QXXErrorType W25QXX_WaitBusy(W25QXXObjectType *w25qxx);

/**
 * @brief 初始化W25QXX对象
 *
 * @param w25qxx w25qxx句柄
 * @param model 手动标记W25QXX型号
 * @param swap SPI交换数据函数注入
 * @param chipselect SPI片选函数注入，可选硬件片选，传入NULL即可
 * @param delay 延时函数注入
 * @return W25QXXErrorType 错误值返回
 */
W25QXXErrorType W25QXX_ObjectInit(W25QXXObjectType *w25qxx,
                                  W25QXXModelType model, W25QXX_SPISwap swap,
                                  W25QXX_SPIChipSelect chipselect) {
  /*检查所需函数是否存在*/
  if (w25qxx == NULL || swap == NULL ) {
    return W25QXX_ERROR_UNKNOWN;
  }

  /*检查可能不需要控制的选项*/
  if (chipselect == NULL) {
    w25qxx->ChipSelect = NULL;
  }

  /*函数注入*/
  w25qxx->Swap = swap;
  w25qxx->ChipSelect = chipselect;

  /*属性参数写入*/
  w25qxx->model = model;

  return W25QXX_ERROR_NONE;
}

/**
 * @brief 读取W25QXX芯片ID
 *
 * @param w25qxx w25qxx句柄
 * @param id 接收到的ID，3Bytes
 */
void W25QXX_ReadID(W25QXXObjectType *w25qxx, uint8_t *id) {
  uint8_t cmd = W25QXX_COMMAND_JEDEC_ID;
  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXX_SwapData(w25qxx, &cmd, 1, id, 3);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);
  // 存入ID到句柄
  w25qxx->id[0] = id[0];
  w25qxx->id[1] = id[1];
  w25qxx->id[2] = id[2];
}

/**
 * @brief 读取W25QXX数据
 *
 * @param w25qxx w25qxx句柄
 * @param block 块号，W25Q64：0~127，W25Q128：0~255
 * @param sector 块内扇区号，范围：0~15
 * @param page 扇区内页号，范围：0~15
 * @param offset 页内字节偏移，范围：0~255
 * @param dataBuff 用于接收读取数据的缓冲区，通过输出参数返回
 * @param count 要读取数据的数量
 * @return W25QXXErrorType 错误值返回
 */
W25QXXErrorType W25QXX_ReadData(W25QXXObjectType *w25qxx, uint32_t block,
                                uint8_t sector, uint8_t page, uint8_t offset,
                                uint8_t *dataBuff, uint32_t count) {
  /* 检查参数是否超出范围 */
  if (sector >= W25QXX_SECTORS_PER_BLOCK || page >= W25QXX_PAGES_PER_SECTOR) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q64_MODEL && block > 127) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q128_MODEL && block > 255) {
    return W25QXX_ERROR_Sector;
  }

  uint32_t address = W25QXX_ADDR(block, sector, page, offset);
  uint8_t cmd[4] = {
      W25QXX_COMMAND_READ_DATA,
      (uint8_t)(address >> 16),
      (uint8_t)(address >> 8),
      (uint8_t)(address),
  };

  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXX_SwapData(w25qxx, cmd, 4, NULL, 0);
  W25QXX_SwapData(w25qxx, NULL, 0, dataBuff, count);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);

  return W25QXX_ERROR_NONE;
}

/**
 * @brief 页编程
 *
 * @param w25qxx w25qxx句柄
 * @param block 块号，W25Q64：0~127，W25Q128：0~255
 * @param sector 块内扇区号，范围：0~15
 * @param page 扇区内页号，范围：0~15
 * @param dataBuff 用于写入数据的缓冲区
 * @param count 要写入数据的数量，范围：0~256
 * @return W25QXXErrorType 错误值返回
 */
W25QXXErrorType W25QXX_PageProgram(W25QXXObjectType *w25qxx, uint32_t block,
                                   uint8_t sector, uint8_t page,
                                   uint8_t *dataBuff, uint16_t count) {
  /* 检查参数是否超出范围 */
  if (sector >= W25QXX_SECTORS_PER_BLOCK || page >= W25QXX_PAGES_PER_SECTOR) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q64_MODEL && block > 127) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q128_MODEL && block > 255) {
    return W25QXX_ERROR_Sector;
  }

  uint32_t address = W25QXX_ADDR(block, sector, page, 0);
  uint8_t cmd[4] = {
      W25QXX_COMMAND_PAGE_PROGRAM,
      (uint8_t)(address >> 16),
      (uint8_t)(address >> 8),
      (uint8_t)(address),
  };

  W25QXX_WriteEnable(w25qxx);

  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXX_SwapData(w25qxx, cmd, 4, NULL, 0);
  W25QXX_SwapData(w25qxx, dataBuff, count, NULL, 0);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);

  return W25QXX_WaitBusy(w25qxx);
}

/**
 * @brief 扇区擦除（4KB）
 *
 * @param w25qxx w25qxx句柄
 * @param block 块号，W25Q64：0~127，W25Q128：0~255
 * @param sector 块内扇区号，范围：0~15
 * @return W25QXXErrorType 错误值返回
 */
W25QXXErrorType W25QXX_SectorErase(W25QXXObjectType *w25qxx, uint32_t block,
                                   uint8_t sector) {
  /* 检查参数是否超出范围 */
  if (sector >= W25QXX_SECTORS_PER_BLOCK) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q64_MODEL && block > 127) {
    return W25QXX_ERROR_Sector;
  }
  if (w25qxx->model == W25QXX_W25Q128_MODEL && block > 255) {
    return W25QXX_ERROR_Sector;
  }

  uint32_t address = W25QXX_ADDR(block, sector, 0, 0);
  uint8_t cmd[4] = {
      W25QXX_COMMAND_SECTOR_ERASE_4K,
      (uint8_t)(address >> 16),
      (uint8_t)(address >> 8),
      (uint8_t)(address),
  };

  W25QXX_WriteEnable(w25qxx);

  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXX_SwapData(w25qxx, cmd, 4, NULL, 0);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);

  return W25QXX_WaitBusy(w25qxx);
}

/**
 * @brief 交换数据操作(读写检查)
 *
 * @param w25qxx w25qxx句柄
 * @param txBuff 传输数据缓冲区
 * @param txLen 传输数据长度
 * @param rxBuff 接收数据缓冲区
 * @param rxLen 接收数据长度
 * @return uint8_t 错误类型
 */
static W25QXXErrorType W25QXX_SwapData(W25QXXObjectType *w25qxx,
                                       uint8_t *txBuff, uint16_t txLen,
                                       uint8_t *rxBuff, uint16_t rxLen) {
  uint8_t status = 0;

  status += w25qxx->Swap(txBuff, txLen, rxBuff, rxLen);

  if (status != 0) {
    return W25QXX_ERROR_SWAPDATA;
  } else {
    return W25QXX_ERROR_NONE;
  }
}

/**
 * @brief 写使能
 *
 * @param w25qxx w25qxx句柄
 * @return W25QXXErrorType 错误值返回
 */
static W25QXXErrorType W25QXX_WriteEnable(W25QXXObjectType *w25qxx) {
  uint8_t cmd = W25QXX_COMMAND_WRITE_ENABLE;

  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXXErrorType err = W25QXX_SwapData(w25qxx, &cmd, 1, NULL, 0);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);

  return err;
}

/**
 * @brief 等待芯片忙状态结束
 *
 * @param w25qxx w25qxx句柄
 * @return W25QXXErrorType 错误值返回
 */
static W25QXXErrorType W25QXX_WaitBusy(W25QXXObjectType *w25qxx) {
  uint8_t cmd = W25QXX_COMMAND_READ_STATUS_REG1;
  uint8_t status = 0;
  uint32_t timeout = 100000;

  w25qxx->ChipSelect(W25QXX_CS_ENABLE);
  W25QXX_SwapData(w25qxx, &cmd, 1, NULL, 0);
  do {
    W25QXX_SwapData(w25qxx, NULL, 0, &status, 1);
    if (--timeout == 0) {
      w25qxx->ChipSelect(W25QXX_CS_DISABLE);
      return W25QXX_ERROR_TIMEOUT;
    }
  } while (status & 0x01);
  w25qxx->ChipSelect(W25QXX_CS_DISABLE);

  return W25QXX_ERROR_NONE;
}
