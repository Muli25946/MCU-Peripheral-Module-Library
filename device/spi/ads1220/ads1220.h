/**
 * @file ads1220.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief ADS1220 24位高精度ADC驱动头文件
 * @details TI ADS1220内置PGA和基准电压源，通过SPI Mode 1通信。
 *          驱动与平台无关，通过函数指针注入SPI、片选和延时操作。
 * @note
 * 该驱动基于https://github.com/ahhh11111/ADS1220-PT100-STM32/tree/main开发，修改过程可能引入bug,如有错误请以原版为准
 * @version 0.1
 * @date 2026-04-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef ADS1220_H
#define ADS1220_H

#include <stdint.h>

/* ADS1220错误类型 */
typedef enum {
  ADS1220_ERROR_NONE = 0,
  ADS1220_ERROR_TIMEOUT,
  ADS1220_ERROR_INVALID,
  ADS1220_ERROR_UNKNOWN,
} ADS1220ErrorType;

/* ADS1220片选控制 */
typedef enum {
  ADS1220_CS_ENABLE,
  ADS1220_CS_DISABLE,
} ADS1220CSEnableType;

/* ADC转换状态（用于非阻塞状态机） */
typedef enum {
  ADS1220_CONV_IDLE = 0,
  ADS1220_CONV_WAITING,
  ADS1220_CONV_READY,
  ADS1220_CONV_TIMEOUT,
  ADS1220_CONV_ERROR,
} ADS1220ConvStateType;

/* ADS1220配置寄存器（4字节） */
typedef struct {
  uint8_t reg0; /**< 输入MUX和增益 */
  uint8_t reg1; /**< 数据速率和工作模式 */
  uint8_t reg2; /**< 基准电压和IDAC */
  uint8_t reg3; /**< IDAC路由 */
} ADS1220ConfigType;

/* ADS1220对象 */
typedef struct {
  ADS1220ConfigType config;
  /*注入函数*/
  uint8_t (*Swap)(uint8_t *txBuff, uint8_t txLen, uint8_t *rxBuff,
                  uint8_t rxLen);                    /**< SPI交换数据 */
  uint8_t (*ChipSelect)(ADS1220CSEnableType status); /**< SPI片选操作 */
  uint8_t (*IsDataReady)(void); /**< 读取DRDY引脚（低=就绪） */
  void (*DelayMs)(uint32_t ms); /**< 毫秒延时 */
  void (*DelayUs)(uint32_t us); /**< 微秒延时 */
  /*内部状态*/
  ADS1220ErrorType lastError;
} ADS1220ObjectType;

/* 需要实现的接口类型 */
typedef uint8_t (*ADS1220_SPISwap)(uint8_t *txBuff, uint8_t txLen,
                                   uint8_t *rxBuff, uint8_t rxLen);
typedef uint8_t (*ADS1220_SPIChipSelect)(ADS1220CSEnableType status);
typedef uint8_t (*ADS1220_IsDataReady)(void);
typedef void (*ADS1220_DelayMs)(uint32_t ms);
typedef void (*ADS1220_DelayUs)(uint32_t us);

/* 函数声明 */
ADS1220ErrorType ADS1220_ObjectInit(ADS1220ObjectType *ads,
                                    ADS1220_SPISwap swap,
                                    ADS1220_SPIChipSelect chipselect,
                                    ADS1220_IsDataReady isDataReady,
                                    ADS1220_DelayMs delayMs,
                                    ADS1220_DelayUs delayUs);

void ADS1220_Reset(ADS1220ObjectType *ads);
void ADS1220_StartSync(ADS1220ObjectType *ads);
void ADS1220_PowerDown(ADS1220ObjectType *ads);

void ADS1220_WriteConfig(ADS1220ObjectType *ads, ADS1220ConfigType *config);
void ADS1220_ReadConfig(ADS1220ObjectType *ads, ADS1220ConfigType *config);
void ADS1220_GetDefaultConfig(ADS1220ConfigType *config);

int32_t ADS1220_ReadData(ADS1220ObjectType *ads);
ADS1220ConvStateType ADS1220_ReadDataWithTimeout(ADS1220ObjectType *ads,
                                                 uint32_t timeoutMs,
                                                 int32_t *data);
float ADS1220_ReadVoltage(ADS1220ObjectType *ads, uint8_t gain, float vref);
int32_t ADS1220_ReadVoltageInt(ADS1220ObjectType *ads, uint8_t gain,
                               int32_t vrefUnit);
int16_t ADS1220_ReadTemperature(ADS1220ObjectType *ads);

void ADS1220_SetInputMux(ADS1220ObjectType *ads, uint8_t mux);
void ADS1220_SetGain(ADS1220ObjectType *ads, uint8_t gain);
void ADS1220_SetDataRate(ADS1220ObjectType *ads, uint8_t rate);
void ADS1220_SetConversionMode(ADS1220ObjectType *ads, uint8_t mode);
void ADS1220_SetVref(ADS1220ObjectType *ads, uint8_t vref);

ADS1220ErrorType ADS1220_GetLastError(ADS1220ObjectType *ads);
void ADS1220_ClearError(ADS1220ObjectType *ads);

#endif /* ADS1220_H */
