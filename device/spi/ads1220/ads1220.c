#include "ads1220.h"
#include "ads1220_def.h"
#include <stddef.h>
#include <stdint.h>

// 发送单字节命令
static ADS1220ErrorType ads1220_send_cmd(ADS1220ObjectType *ads, uint8_t cmd);
// SPI交换数据（含错误记录）
static ADS1220ErrorType ads1220_swap(ADS1220ObjectType *ads, uint8_t *txBuff,
                                     uint8_t txLen, uint8_t *rxBuff,
                                     uint8_t rxLen);
// 写单个寄存器
static void ads1220_write_reg(ADS1220ObjectType *ads, uint8_t reg,
                              uint8_t value);
// 读单个寄存器
static uint8_t ads1220_read_reg(ADS1220ObjectType *ads, uint8_t reg);

/**
 * @brief 初始化ADS1220对象并完成硬件复位
 *
 * @param ads       ADS1220句柄
 * @param swap      SPI交换数据函数注入
 * @param chipselect SPI片选函数注入，可传NULL（硬件片选）
 * @param delayMs   毫秒延时函数注入
 * @param delayUs   微秒延时函数注入
 * @return ADS1220ErrorType 错误码
 */
ADS1220ErrorType ADS1220_ObjectInit(ADS1220ObjectType *ads,
                                    ADS1220_SPISwap swap,
                                    ADS1220_SPIChipSelect chipselect,
                                    ADS1220_DelayMs delayMs,
                                    ADS1220_DelayUs delayUs) {
  if (ads == NULL || swap == NULL || delayMs == NULL || delayUs == NULL) {
    return ADS1220_ERROR_UNKNOWN;
  }

  ads->Swap = swap;
  ads->ChipSelect = chipselect; /* 允许NULL（硬件片选） */
  ads->DelayMs = delayMs;
  ads->DelayUs = delayUs;
  ads->lastError = ADS1220_ERROR_NONE;

  ads->DelayMs(10); /* 等待上电稳定 */
  ADS1220_Reset(ads);

  return ADS1220_ERROR_NONE;
}

/**
 * @brief 复位ADS1220，所有寄存器恢复默认值
 *
 * @param ads ADS1220句柄
 */
void ADS1220_Reset(ADS1220ObjectType *ads) {
  ads1220_send_cmd(ads, ADS1220_CMD_RESET);
  ads->DelayMs(2);
}

/**
 * @brief 发送START/SYNC命令，启动单次转换
 *
 * @param ads ADS1220句柄
 */
void ADS1220_StartSync(ADS1220ObjectType *ads) {
  ads1220_send_cmd(ads, ADS1220_CMD_START);
}

/**
 * @brief 使芯片进入掉电模式
 *
 * @param ads ADS1220句柄
 */
void ADS1220_PowerDown(ADS1220ObjectType *ads) {
  ads1220_send_cmd(ads, ADS1220_CMD_POWERDOWN);
}

/**
 * @brief 连续写入4个配置寄存器
 *
 * @param ads    ADS1220句柄
 * @param config 配置结构体指针
 */
void ADS1220_WriteConfig(ADS1220ObjectType *ads, ADS1220ConfigType *config) {
  ads1220_write_reg(ads, ADS1220_REG0, config->reg0);
  ads1220_write_reg(ads, ADS1220_REG1, config->reg1);
  ads1220_write_reg(ads, ADS1220_REG2, config->reg2);
  ads1220_write_reg(ads, ADS1220_REG3, config->reg3);
}

/**
 * @brief 读取全部4个配置寄存器到结构体
 *
 * @param ads    ADS1220句柄
 * @param config 配置结构体指针（输出）
 */
void ADS1220_ReadConfig(ADS1220ObjectType *ads, ADS1220ConfigType *config) {
  config->reg0 = ads1220_read_reg(ads, ADS1220_REG0);
  config->reg1 = ads1220_read_reg(ads, ADS1220_REG1);
  config->reg2 = ads1220_read_reg(ads, ADS1220_REG2);
  config->reg3 = ads1220_read_reg(ads, ADS1220_REG3);
}

/**
 * @brief 获取推荐默认配置
 *        AIN0-AIN1差分，增益=1，20SPS，正常模式单次转换，内部2.048V基准
 *
 * @param config 配置结构体指针（输出）
 */
void ADS1220_GetDefaultConfig(ADS1220ConfigType *config) {
  config->reg0 = ADS1220_MUX_AIN0_AIN1 | ADS1220_GAIN_1 | ADS1220_PGA_ENABLED;
  config->reg1 = ADS1220_DR_20SPS | ADS1220_MODE_NORMAL | ADS1220_CM_SINGLE;
  config->reg2 = ADS1220_VREF_INT | ADS1220_FIR_NONE;
  config->reg3 = 0x00;
}

/**
 * @brief 读取24位ADC原始数据（符号扩展到32位）
 * @note  调用前需确认DRDY已就绪
 *
 * @param ads ADS1220句柄
 * @return int32_t 24位补码值，符号扩展到32位
 */
int32_t ADS1220_ReadData(ADS1220ObjectType *ads) {
  uint8_t cmd = ADS1220_CMD_RDATA;
  uint8_t buf[3] = {0, 0, 0};

  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_ENABLE);
  ads->DelayUs(2);
  ads1220_swap(ads, &cmd, 1, NULL, 0);
  ads1220_swap(ads, NULL, 0, buf, 3);
  ads->DelayUs(2);
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_DISABLE);

  int32_t data =
      ((int32_t)buf[0] << 16) | ((int32_t)buf[1] << 8) | (int32_t)buf[2];
  /* 符号扩展：最高位为1时填充高8位 */
  if (data & 0x800000) {
    data |= (int32_t)0xFF000000;
  }
  return data;
}

/**
 * @brief 阻塞等待转换完成并读取数据
 *
 * @param ads       ADS1220句柄
 * @param timeoutMs 超时时间（毫秒，基于轮询估算）
 * @param data      输出参数，存储ADC原始值
 * @return ADS1220ConvStateType CONV_READY=成功, CONV_TIMEOUT=超时,
 * CONV_ERROR=参数错误
 */
ADS1220ConvStateType ADS1220_ReadDataWithTimeout(ADS1220ObjectType *ads,
                                                 uint32_t timeoutMs,
                                                 int32_t *data) {
  if (data == NULL) {
    ads->lastError = ADS1220_ERROR_INVALID;
    return ADS1220_CONV_ERROR;
  }

  ADS1220_ClearError(ads);
  ADS1220_StartSync(ads);

  /* 每次轮询约1µs，timeoutMs*1000次 */
  uint32_t tries = timeoutMs * 1000;
  while (tries--) {
    if (ads->drdy) {
      *data = ADS1220_ReadData(ads);
      ads->drdy = 0;
      return ADS1220_CONV_READY;
    }
  }

  ads->lastError = ADS1220_ERROR_TIMEOUT;
  *data = 0x7FFFFFFF;
  return ADS1220_CONV_TIMEOUT;
}

/**
 * @brief 读取电压值（浮点）
 *        V = (ADC_Code / 2^23) * (Vref / Gain)
 *
 * @param ads  ADS1220句柄
 * @param gain 增益值（1/2/4/8/16/32/64/128）
 * @param vref 基准电压（V）
 * @return float 电压值（V）
 */
float ADS1220_ReadVoltage(ADS1220ObjectType *ads, uint8_t gain, float vref) {
  int32_t raw = ADS1220_ReadData(ads);
  return ((float)raw / (float)ADS1220_FULL_SCALE) * (vref / (float)gain);
}

/**
 * @brief 读取电压值（整数，无浮点）
 *        返回值单位与vrefUnit相同：vrefUnit=2048000时单位为µV
 *
 * @param ads      ADS1220句柄
 * @param gain     增益值（1/2/4/8/16/32/64/128）
 * @param vrefUnit 基准电压（调用者选择单位）
 * @return int32_t 电压值
 */
int32_t ADS1220_ReadVoltageInt(ADS1220ObjectType *ads, uint8_t gain,
                               int32_t vrefUnit) {
  int32_t raw = ADS1220_ReadData(ads);
  int64_t voltage =
      ((int64_t)raw * vrefUnit) / ((int64_t)ADS1220_FULL_SCALE * gain);
  return (int32_t)voltage;
}

/**
 * @brief 读取内部温度传感器值
 * @note  使用前需配置寄存器1的TS位为ADS1220_TS_ENABLED
 *        分辨率0.03125°C/LSB，结果为整数°C
 *
 * @param ads ADS1220句柄
 * @return int16_t 温度值（°C）
 */
int16_t ADS1220_ReadTemperature(ADS1220ObjectType *ads) {
  int32_t raw = ADS1220_ReadData(ads);
  return (int16_t)((raw * 3125) / 100000);
}

/**
 * @brief 设置输入多路复用器（仅修改REG0高4位）
 *
 * @param ads ADS1220句柄
 * @param mux MUX配置值，使用 ADS1220_MUX_* 宏
 */
void ADS1220_SetInputMux(ADS1220ObjectType *ads, uint8_t mux) {
  uint8_t reg = ads1220_read_reg(ads, ADS1220_REG0);
  ads1220_write_reg(ads, ADS1220_REG0, (reg & 0x0F) | (mux & 0xF0));
}

/**
 * @brief 设置PGA增益（仅修改REG0的GAIN[2:0]位）
 *
 * @param ads  ADS1220句柄
 * @param gain 增益配置值，使用 ADS1220_GAIN_* 宏
 */
void ADS1220_SetGain(ADS1220ObjectType *ads, uint8_t gain) {
  uint8_t reg = ads1220_read_reg(ads, ADS1220_REG0);
  ads1220_write_reg(ads, ADS1220_REG0, (reg & 0xF1) | (gain & 0x0E));
}

/**
 * @brief 设置数据速率（仅修改REG1高3位）
 *
 * @param ads  ADS1220句柄
 * @param rate 速率配置值，使用 ADS1220_DR_* 宏
 */
void ADS1220_SetDataRate(ADS1220ObjectType *ads, uint8_t rate) {
  uint8_t reg = ads1220_read_reg(ads, ADS1220_REG1);
  ads1220_write_reg(ads, ADS1220_REG1, (reg & 0x1F) | (rate & 0xE0));
}

/**
 * @brief 设置转换模式（单次/连续，仅修改REG1的CM位）
 *
 * @param ads  ADS1220句柄
 * @param mode 转换模式，使用 ADS1220_CM_* 宏
 */
void ADS1220_SetConversionMode(ADS1220ObjectType *ads, uint8_t mode) {
  uint8_t reg = ads1220_read_reg(ads, ADS1220_REG1);
  ads1220_write_reg(ads, ADS1220_REG1, (reg & 0xFB) | (mode & 0x04));
}

/**
 * @brief 设置基准电压源（仅修改REG2高2位）
 *
 * @param ads  ADS1220句柄
 * @param vref 基准配置值，使用 ADS1220_VREF_* 宏
 */
void ADS1220_SetVref(ADS1220ObjectType *ads, uint8_t vref) {
  uint8_t reg = ads1220_read_reg(ads, ADS1220_REG2);
  ads1220_write_reg(ads, ADS1220_REG2, (reg & 0x3F) | (vref & 0xC0));
}

/**
 * @brief 获取最近一次错误码
 *
 * @param ads ADS1220句柄
 * @return ADS1220ErrorType 错误码
 */
ADS1220ErrorType ADS1220_GetLastError(ADS1220ObjectType *ads) {
  return ads->lastError;
}

/**
 * @brief 清除错误状态
 *
 * @param ads ADS1220句柄
 */
void ADS1220_ClearError(ADS1220ObjectType *ads) {
  ads->lastError = ADS1220_ERROR_NONE;
}

static ADS1220ErrorType ads1220_swap(ADS1220ObjectType *ads, uint8_t *txBuff,
                                     uint8_t txLen, uint8_t *rxBuff,
                                     uint8_t rxLen) {
  if (ads->Swap(txBuff, txLen, rxBuff, rxLen) != 0) {
    ads->lastError = ADS1220_ERROR_UNKNOWN;
    return ADS1220_ERROR_UNKNOWN;
  }
  return ADS1220_ERROR_NONE;
}

static ADS1220ErrorType ads1220_send_cmd(ADS1220ObjectType *ads, uint8_t cmd) {
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_ENABLE);
  ads->DelayUs(2);
  ADS1220ErrorType err = ads1220_swap(ads, &cmd, 1, NULL, 0);
  ads->DelayUs(2);
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_DISABLE);
  return err;
}

static void ads1220_write_reg(ADS1220ObjectType *ads, uint8_t reg,
                              uint8_t value) {
  uint8_t cmd[2] = {
      (uint8_t)(ADS1220_CMD_WREG | (reg << 2)),
      value,
  };
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_ENABLE);
  ads->DelayUs(2);
  ads1220_swap(ads, cmd, 2, NULL, 0);
  ads->DelayUs(2);
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_DISABLE);
}

static uint8_t ads1220_read_reg(ADS1220ObjectType *ads, uint8_t reg) {
  uint8_t cmd = (uint8_t)(ADS1220_CMD_RREG | (reg << 2));
  uint8_t value = 0;
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_ENABLE);
  ads->DelayUs(2);
  ads1220_swap(ads, &cmd, 1, NULL, 0);
  ads1220_swap(ads, NULL, 0, &value, 1);
  ads->DelayUs(2);
  if (ads->ChipSelect)
    ads->ChipSelect(ADS1220_CS_DISABLE);
  return value;
}
