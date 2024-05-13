#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "mpu6050.h"
#include <time.h>
#include "inference.h"
#include "math.h"

#define WINDOW_SIZE 40
#define OVERLAP 20
#define GRAVITY 9.8

// Thresholds as per your description
#define THRESHOLD_ALPHA 3.5
#define THRESHOLD_OMEGA 3
#define THRESHOLD_ANGLE 60
#define THRESHOLD_MOTION 9

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
  float ax, ay, az, gx, gy, gz;
  float alpha, omega, theta;
  float max_alpha = 0, min_alpha = INFINITY;
  float max_omega = 0, min_omega = INFINITY;
  int samples_collected = 0;

  mpu6050_acce_value_t acce;
  mpu6050_gyro_value_t gyro;
  while (1)
  {
    mpu6050_get_acce(mpu6050, &acce);
    mpu6050_get_gyro(mpu6050, &gyro);

    ax = acce.acce_x;
    ay = acce.acce_y;
    az = acce.acce_z;
    gx = gyro.gyro_x / 65.5;
    gy = gyro.gyro_y / 65.5;
    gz = gyro.gyro_z / 65.5;

    // Calculate magnitude of vectors
    alpha = sqrt(ax * ax + ay * ay + az * az);
    omega = sqrt(gx * gx + gy * gy + gz * gz);
    theta = acos(ay + 0.000001) * 180 / M_PI; // assuming ay is the vertical component

    // Update max and min over the window
    if (alpha > max_alpha)
      max_alpha = alpha;
    if (alpha < min_alpha)
      min_alpha = alpha;
    if (omega > max_omega)
      max_omega = omega;
    if (omega < min_omega)
      min_omega = omega;

    samples_collected++;

    // Check window size and apply thresholds
    if (samples_collected >= WINDOW_SIZE)
    {
      float delta_alpha = max_alpha - min_alpha;
      float delta_omega = max_omega - min_omega;

      ESP_LOGI(TAG, "Delta Alpha: %f, Delta Omega: %f, Theta: %f", delta_alpha, delta_omega, theta);

      if (delta_alpha >= THRESHOLD_ALPHA && delta_omega >= THRESHOLD_OMEGA && theta >= THRESHOLD_ANGLE)
      {
        ESP_LOGI(TAG, "Fall detected!");
        // Trigger notification logic here
      } else {
      }

      // Reset for next window with overlap
      samples_collected = OVERLAP;
      max_alpha = 0;
      min_alpha = INFINITY;
      max_omega = 0;
      min_omega = INFINITY;
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // Sampling rate about 20 Hz
  }
  // for(int i = 0; i < 7; i++){
  //     printf("Lable Count: %d\n", count[i]);
  // }

  mpu6050_delete(mpu6050);
  i2c_driver_delete(I2C_MASTER_NUM);
  ESP_LOGI(TAG, "I2C de-initialized successfully");
}
