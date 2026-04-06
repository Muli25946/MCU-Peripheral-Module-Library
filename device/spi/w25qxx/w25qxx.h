/**
 * @file w25qxx.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief w25qxx系列SPI闪存驱动头文件，支持w25q64/w25q128型号
 * @version 0.1
 * @date 2026-04-06
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _W25QXX_H
#define _W25QXX_H

#include <stdint.h>

/*W25QXX错误类型*/
typedef enum {
  W25QXX_ERROR_NONE = 0,
  W25QXX_ERROR_SWAPDATA,
  W25QXX_ERROR_TIMEOUT,
  W25QXX_ERROR_Sector,
  W25QXX_ERROR_UNKNOWN,
} W25QXXErrorType;

/*W25QXX片选控制*/
typedef enum {
  W25QXX_CS_ENABLE,
  W25QXX_CS_DISABLE,
} W25QXXCSEnableType;

/*W25QXX型号*/
typedef enum {
  W25QXX_W25Q64_MODEL,
  W25QXX_W25Q128_MODEL,
} W25QXXModelType;

/*W25QXXS对象*/
typedef struct {
  /*data*/
  W25QXXModelType model; // 型号
  uint8_t id[3];         // 存储ID
  /*function*/
  uint8_t (*Swap)(uint8_t *txBuff, uint8_t txLen, uint8_t *rxBuff,
                  uint8_t rxLen);                   // SPI交换数据
  uint8_t (*ChipSelect)(W25QXXCSEnableType status); // SPI片选操作
  void (*DelayMs)(uint32_t ms);                     // 延时函数
} W25QXXObjectType;

/*需要实现的接口*/
typedef uint8_t (*W25QXX_SPISwap)(uint8_t *txBuff, uint8_t txLen,
                                  uint8_t *rxBuff,
                                  uint8_t rxLen); // W25QXX的SPI交换数据操作
typedef uint8_t (*W25QXX_SPIChipSelect)(
    W25QXXCSEnableType
        status); // W25QXX的片选操作,硬件片选可忽略同时传入初始化参数NULL即可
typedef void (*W25QXX_DelayMs)(uint32_t ms); // W25QXX的延时函数

/*函数声明*/
W25QXXErrorType W25QXX_ObjectInit(W25QXXObjectType *w25qxx,
                                  W25QXXModelType model, W25QXX_SPISwap swap,
                                  W25QXX_SPIChipSelect chipselect,
                                  W25QXX_DelayMs delay);
void W25QXX_ReadID(W25QXXObjectType *w25qxx, uint8_t *id);
W25QXXErrorType W25QXX_ReadData(W25QXXObjectType *w25qxx,
                                uint32_t block, uint8_t sector,
                                uint8_t page, uint8_t offset,
                                uint8_t *dataBuff, uint32_t count);
W25QXXErrorType W25QXX_PageProgram(W25QXXObjectType *w25qxx,
                                   uint32_t block, uint8_t sector,
                                   uint8_t page,
                                   uint8_t *dataBuff, uint16_t count);
W25QXXErrorType W25QXX_SectorErase(W25QXXObjectType *w25qxx,
                                   uint32_t block, uint8_t sector);

#endif
