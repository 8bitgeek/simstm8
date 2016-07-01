#ifndef MCU_Operation_H
#define MCU_Operation_H
#include <stdint.h>
#include "CPUConfig.h"
#include "Memory.h"


uint16_t getBigEndianWord(uint8_t mostByte, uint8_t leastByte);
void setBigEndianWord(uint8_t *mostByte, uint8_t *leastByte, uint16_t fullByte);
void setBigEndianLSB(uint8_t *mostByte, uint8_t *leastByte, uint16_t fullByte);
void setBigEndianMSB(uint8_t *mostByte, uint8_t *leastByte, uint16_t fullByte);

void mcu_add(uint8_t value);
void mcu_adc(uint8_t value);
void mcu_addw(uint8_t *mostByte, uint8_t *leastByte, uint16_t value);

#endif // MCU_Operation_H
