#include "driver/i2c.h"
#include "esp_log.h"
#include "mpu6050.h"
#include "unity.h"
#include <stdio.h>
#include <unistd.h>

#define I2C_MASTER_SCL_IO 1       /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 0       /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

static const char *TAG = "final-project";
static mpu6050_handle_t mpu6050 = NULL;

/**
 * @brief i2c master initialization
 */
static void i2c_bus_init(void) {
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
  conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

  ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));

  ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

/**
 * @brief i2c master initialization
 */
static void i2c_sensor_mpu6050_init(void) {

  i2c_bus_init();
  mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
  TEST_ASSERT_NOT_NULL_MESSAGE(mpu6050, "MPU6050 create returned NULL");

  ESP_ERROR_CHECK(mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS));

  ESP_ERROR_CHECK(mpu6050_wake_up(mpu6050));
}

void app_main() {
  mpu6050_acce_value_t acce;
  mpu6050_gyro_value_t gyro;
  i2c_sensor_mpu6050_init();
  int freq = 100;
  int duration = 4;
  int period = 1000000 / freq;
  int iterations = freq * duration;
  int index = 0;

  char ch;
  while (1) {
    scanf("%c", &ch);
    if (ch == 's') {
      for (int i = 0; i < iterations; i++) {
        ESP_ERROR_CHECK(mpu6050_get_acce(mpu6050, &acce));
        ESP_ERROR_CHECK(mpu6050_get_gyro(mpu6050, &gyro));
        ESP_LOGI(TAG, "%.2f %.2f %.2f %.2f %.2f %.2f", acce.acce_x, acce.acce_y,
                 acce.acce_z, gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
        usleep(period);
      }
      ESP_LOGI(TAG, "end %d", index++);
      ch = '\0';
    }
    vTaskDelay(1);
  }

  mpu6050_delete(mpu6050);
  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
}
