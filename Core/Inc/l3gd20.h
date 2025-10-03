#ifndef L3GD20_H
#define L3GD20_H

#include "main.h"

/* L3GD20 Registers */
#define L3GD20_WHO_AM_I      0x0F
#define L3GD20_CTRL_REG1     0x20
#define L3GD20_CTRL_REG4     0x23
#define L3GD20_OUT_X_L       0x28
#define L3GD20_OUT_X_H       0x29
#define L3GD20_OUT_Y_L       0x2A
#define L3GD20_OUT_Y_H       0x2B
#define L3GD20_OUT_Z_L       0x2C
#define L3GD20_OUT_Z_H       0x2D

/* Read/Write commands */
#define L3GD20_READ          0x80
#define L3GD20_WRITE         0x00
#define L3GD20_MULTIPLEBYTE  0x40

/* Device ID */
#define L3GD20_ID            0xD4

/* Sensitivity */
#define L3GD20_SENSITIVITY_250DPS  8.75f  /* 8.75 mdps/digit */

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} L3GD20_Data;

typedef struct {
    float x;
    float y;
    float z;
} L3GD20_Angle;

/* Function prototypes */
uint8_t L3GD20_Init(SPI_HandleTypeDef *hspi);
void L3GD20_ReadGyro(SPI_HandleTypeDef *hspi, L3GD20_Data *data);
void L3GD20_UpdateAngles(L3GD20_Data *data, L3GD20_Angle *angle, float dt);

#endif /* L3GD20_H */