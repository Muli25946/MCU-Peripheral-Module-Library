#include "sp3485.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief 初始化SP3485对象
 *
 * @param sp3485 sp3485句柄
 * @param txen 发送使能控制函数
 * @param tx 发送数据函数
 * @param rx 接收数据函数
 * @return SP3485ErrorType
 */
SP3485ErrorType SP3485_ObjectInit(SP3485ObjectType *sp3485, SP3485_TxRxMode txen,
                                  SP3485_TxData tx, SP3485_RxData rx) {
  /*检查注入函数*/
  if ((txen == NULL) || (tx == NULL) || (rx == NULL)) {
    return SP3485_Absent;
  }

  /*注入函数*/
  sp3485->TxRxMode = txen;
  sp3485->TxData = tx;
  sp3485->RxData = rx;

  return SP3485_Ok;
}

/**
 * @brief 发送数据
 *
 * @param sp3485 sp3485句柄
 * @param txBuf 发送数据缓冲区
 * @param len 发送数据长度
 * @return SP3485ErrorType
 */
SP3485ErrorType SP3485_TransmitData(SP3485ObjectType *sp3485, uint8_t *txBuf,
                                    uint8_t len) {
  uint8_t status = 0;
  sp3485->TxRxMode(SP3485_TxOnly); // 切换到发送模式
  status += sp3485->TxData(txBuf, len);
  sp3485->TxRxMode(SP3485_RxOnly); // 发送完成后切换回接收模式
  if (status != 0) {
    return SP3485_Error;
  }
  return SP3485_Ok;
}

/**
 * @brief 接收数据
 *
 * @param sp3485 sp3485句柄
 * @param rxBuf 接收数据缓冲区
 * @param len 接收数据长度
 * @return SP3485ErrorType
 */
SP3485ErrorType SP3485_ReceiveData(SP3485ObjectType *sp3485, uint8_t *rxBuf,
                                   uint8_t len) {
  uint8_t status = 0;
  sp3485->TxRxMode(SP3485_RxOnly); // 确保处于接收模式
  status += sp3485->RxData(rxBuf, len);
  if (status != 0) {
    return SP3485_Error;
  }
  return SP3485_Ok;
}
