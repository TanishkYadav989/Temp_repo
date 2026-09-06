#include "afe.h"
#include <stdbool.h>
uint16_t cell_voltages_mv[15];
volatile uint16_t cuv_fault_flags = 0;
volatile bool timer_flag = false;
static uint8_t under_counter[15] = {0};
static uint8_t recovery_counter[15] = {0};
static uint8_t raw_buffer[30];

/*resetting flag, counters & voltage readings*/
void AFE_Init(){
    cuv_fault_flags = 0;
    for (int i = 0; i < TOTAL_CELLS; i++) {
        cell_voltages_mv[i] = 0;
        under_counter[i] = 0;
        recovery_counter[i] = 0;
    }
}
void Task_AFE_Read(){
    uint8_t i;
    uint16_t v_cell;
    /*Execute only on 100ms tick*/
    if (!timer_flag) {
        return; 
    }
    timer_flag = false;
    /*Fetch raw data via I2C*/
    i2c_read(AFE_I2C_ADDR, AFE_REG_VC1_HI, raw_buffer, RAW_BUFFER_SIZE);
    /* Parse data and convert to mV*/
    for (i = 0; i < TOTAL_CELLS; i++) {
        uint8_t high = raw_buffer[i * 2];
        uint8_t low = raw_buffer[(i * 2) + 1];
        
        uint16_t adc_raw = (((uint16_t)(high & 0x3F)) << 8) | (uint16_t)low;
        
        cell_voltages_mv[i] = (uint16_t)(((uint32_t)adc_raw * 380) / 1000);
    }

    /*temporal fault and recovery logic*/
    for (i = 0; i < TOTAL_CELLS; i++) {
        v_cell = cell_voltages_mv[i];

        if (v_cell < CUV_THRESHOLD_MV) {
            recovery_counter[i] = 0;

            if (under_counter[i] < 30) under_counter[i]++;

            if (under_counter[i] >= 30) cuv_fault_flags |= (1U << i);
        }
        else if (v_cell > CUV_RECOVERY_MV) {
            under_counter[i] = 0;

            if ((cuv_fault_flags & (1U << i)) != 0) {
                if (recovery_counter[i] < 60) recovery_counter[i]++;

                if (recovery_counter[i] >= 60) {
                    cuv_fault_flags &= ~(1U << i);
                    recovery_counter[i] = 0;
                }
            }
        }
        else {
            under_counter[i] = 0;
            recovery_counter[i] = 0;
        }
    }
}

/*100ms Timer ISR*/
void TIM_IRQHandler(){
   /*clearing flags & incrementing counter*/ 
   timer_flag=true; 
}