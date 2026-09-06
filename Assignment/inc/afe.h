#ifndef BMS_AFE_H
#define BMS_AFE_H

#include <stdint.h>
#include <stdbool.h>

#define AFE_I2C_ADDR             0x10
#define AFE_REG_VC1_HI           0x0C
#define TOTAL_CELLS              15
#define RAW_BUFFER_SIZE          30
#define CUV_THRESHOLD_MV         2800
#define CUV_RECOVERY_MV          3000

extern uint16_t cell_voltages_mv[15];
extern volatile uint16_t cuv_fault_flags;
extern volatile bool timer_flag;

/*external I2C driver read function*/
extern void i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t len);
void AFE_Init();
void Task_AFE_Read();
#endif