/**
 * @file sp3485.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief
 * 该驱动简单封装函数注入的方式，以隔离处理逻辑和底层硬件，抹平平台差异。本身相当简单
 * @version 0.1
 * @date 2026-04-06
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef SP3485_H
#define SP3485_H

#include <stdint.h>

/*SP3485错误类型*/
typedef enum {
  SP3485_Ok = 0,
  SP3485_Absent,
  SP3485_Error,
} SP3485ErrorType;

/*收发模式控制枚举*/
typedef enum {
  SP3485_RxOnly, // 低电平有效
  SP3485_TxOnly, // 高电平有效
} SP3485RxTxControlType;

/*SP3485对象类型*/
typedef struct {
  uint8_t (*TxRxMode)(SP3485RxTxControlType state); // 发送使能控制
  uint8_t (*TxData)(uint8_t *txBuf,
                    uint8_t len); // 绑定串口发送多个字节的函数到设备SP3485
  uint8_t (*RxData)(uint8_t *rxBuf,
                    uint8_t len); // 绑定串口接收多个字节的函数到设备SP3485
} SP3485ObjectType;

/*需要实现的接口*/
typedef uint8_t (*SP3485_TxRxMode)(
    SP3485RxTxControlType state); // 声明发送使能控制函数
typedef uint8_t (*SP3485_TxData)(uint8_t *txBuf,
                                 uint8_t len); // 声明使用串口发送多个字节的函数
typedef uint8_t (*SP3485_RxData)(uint8_t *rxBuf,
                                 uint8_t len); // 声明使用串口接收多个字节的

/*函数声明*/
SP3485ErrorType SP3485_ObjectInit(SP3485ObjectType *sp3485, SP3485_TxRxMode mode,
                                  SP3485_TxData tx, SP3485_RxData rx);
SP3485ErrorType SP3485_TransmitData(SP3485ObjectType *sp3485, uint8_t *txBuf,
                                    uint8_t len);
SP3485ErrorType SP3485_ReceiveData(SP3485ObjectType *sp3485, uint8_t *rxBuf,
                                   uint8_t len);
#endif // SP3485_H