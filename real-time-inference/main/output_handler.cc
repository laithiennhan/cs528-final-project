#include "output_handler.h"
#include "tensorflow/lite/micro/micro_log.h"

void HandleOutput(float x_value, float y_value) {
  // Log the current X and Y values
  MicroPrintf("x_value: %f, y_value: %f", static_cast<double>(x_value),
              static_cast<double>(y_value));
}