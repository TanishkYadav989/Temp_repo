# Assumptions

* AFE module/IC allows auto-incrementing of data registers once a read sequence is initialised similar to MPU6050, TSL2561 or other I2C modules.

* A hardware timer ticks every 100ms, 1 tick = 100ms. Therefore 30 ticks = 3 sec for fault analysis & 60 ticks = 6 sec for recovery clearing.
  
* i2c_read() external function implemented correctly and handles start/stop conditions along with ACK signals.
