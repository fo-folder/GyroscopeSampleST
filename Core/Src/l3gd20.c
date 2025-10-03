#include "l3gd20.h"

static void L3GD20_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value);
static uint8_t L3GD20_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg);

uint8_t L3GD20_Init(SPI_HandleTypeDef *hspi) {
    uint8_t id = 0;
    
    /* Select the chip */
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    
    /* Read WHO_AM_I register */
    id = L3GD20_ReadReg(hspi, L3GD20_WHO_AM_I);
    
    if (id != L3GD20_ID) {
        return 0; /* Device not found */
    }
    
    /* Configure CTRL_REG1: Power on, 100Hz ODR, enable X,Y,Z */
    L3GD20_WriteReg(hspi, L3GD20_CTRL_REG1, 0x0F);
    
    /* Configure CTRL_REG4: 250 dps full scale */
    L3GD20_WriteReg(hspi, L3GD20_CTRL_REG4, 0x00);
    
    return 1;
}

void L3GD20_ReadGyro(SPI_HandleTypeDef *hspi, L3GD20_Data *data) {
    uint8_t buffer[6];
    
    /* Select the chip */
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    
    /* Send read command with auto-increment */
    uint8_t cmd = L3GD20_OUT_X_L | L3GD20_READ | L3GD20_MULTIPLEBYTE;
    HAL_SPI_Transmit(hspi, &cmd, 1, 100);
    
    /* Read 6 bytes (X, Y, Z - 2 bytes each) */
    HAL_SPI_Receive(hspi, buffer, 6, 100);
    
    /* Deselect the chip */
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_SET);
    
    /* Combine bytes */
    data->x = (int16_t)((buffer[1] << 8) | buffer[0]);
    data->y = (int16_t)((buffer[3] << 8) | buffer[2]);
    data->z = (int16_t)((buffer[5] << 8) | buffer[4]);
}

void L3GD20_UpdateAngles(L3GD20_Data *data, L3GD20_Angle *angle, float dt) {
    /* Convert raw data to degrees per second */
    float gyro_x = data->x * L3GD20_SENSITIVITY_250DPS / 1000.0f;
    float gyro_y = data->y * L3GD20_SENSITIVITY_250DPS / 1000.0f;
    float gyro_z = data->z * L3GD20_SENSITIVITY_250DPS / 1000.0f;
    
    /* Integrate to get angles (simple integration) */
    angle->x += gyro_x * dt;
    angle->y += gyro_y * dt;
    angle->z += gyro_z * dt;
    
    /* Optional: Limit angle drift with a high-pass filter */
    angle->x *= 0.98f; /* Slight decay to prevent drift */
    angle->y *= 0.98f;
    angle->z *= 0.98f;
}

static void L3GD20_WriteReg(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value) {
    uint8_t data[2];
    
    data[0] = reg & 0x7F; /* Write command */
    data[1] = value;
    
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, data, 2, 100);
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_SET);
}

static uint8_t L3GD20_ReadReg(SPI_HandleTypeDef *hspi, uint8_t reg) {
    uint8_t cmd = reg | L3GD20_READ;
    uint8_t value = 0;
    
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &cmd, 1, 100);
    HAL_SPI_Receive(hspi, &value, 1, 100);
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin, GPIO_PIN_SET);
    
    return value;
}