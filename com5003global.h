// globale definition

#ifndef COM5003GOBAL_H
#define COM5003GLOBAL_H

#define CheckSumOffset 56
#define FPGADeviceNode "/dev/zFPGA1reg"
#define I2CDeviceNode "/dev/i2c-3"
#define I2CMasterAdress 0x20
#define I2CSlaveAdress 0x21
#define LeiterkartenName "com5003"
#define ServerName "com5003d"
#define InpBufSize 4096
#define ServerVersion "V1.00"


// wenn COM5003DEBUG -> kein fork()
//#define COM5003DEBUG 1

#endif
