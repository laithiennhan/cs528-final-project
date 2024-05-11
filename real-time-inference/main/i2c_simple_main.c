#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "mpu6050.h"
#include <time.h>
#include "inference.h"


static const char *TAG = "i2c-simple-example";
static mpu6050_handle_t mpu6050 = NULL;

#define I2C_MASTER_SCL_IO 1         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 0         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000



static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
        .scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    // return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

static void i2c_sensor_mpu6050_init(void)
{
    i2c_master_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050);
}

void app_main(void)
{
    uint8_t mpu6050_deviceid;

    i2c_sensor_mpu6050_init();
    ESP_LOGI(TAG, "I2C initialized successfully");
    mpu6050_get_deviceid(mpu6050, &mpu6050_deviceid);
    printf("Device id: %X\n", mpu6050_deviceid);

    setup();
    
    int count[7];
    for(int i = 0; i < 7; i++) {
        count[i] = 0; 
    }

    for (int i = 0; i < 20; i++)
    {
        mpu6050_acce_value_t acce;
        mpu6050_gyro_value_t gyro;
        mpu6050_get_acce(mpu6050, &acce);
        mpu6050_get_gyro(mpu6050, &gyro);

        printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z, gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
        printf("xxx\n");
        
        float inputs[6];
        inputs[0] = acce.acce_x;
        inputs[1] = acce.acce_y;
        inputs[2] = acce.acce_z;
        inputs[3] = gyro.gyro_x;
        inputs[4] = gyro.gyro_y;
        inputs[5] = gyro.gyro_z;
        int argmax = infer(inputs);
        count[argmax] ++;

        printf("Argmax: %d\n", argmax);
        printf("xxx\n");
    }
    
    for(int i = 0; i < 7; i++){
        printf("Lable Count: %d\n", count[i]);
    }
    
    mpu6050_delete(mpu6050);
    i2c_driver_delete(I2C_MASTER_NUM);
    ESP_LOGI(TAG, "I2C de-initialized successfully");
  }
