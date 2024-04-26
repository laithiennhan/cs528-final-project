#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_OUTPUT_HANDLER_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_OUTPUT_HANDLER_H_

#include "tensorflow/lite/c/common.h"

// Called by the main loop to produce some output based on the x and y values
void HandleOutput(float x_value, float y_value);

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_HELLO_WORLD_OUTPUT_HANDLER_H_