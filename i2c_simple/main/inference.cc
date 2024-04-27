
/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "inference.h"
#include "model.h"
#include "output_handler.h"
#include <iostream>
#include <cstdlib> // For rand() function
#include <ctime>   // For time() function

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  TfLiteTensor *output = nullptr;
  int inference_count = 100;

  constexpr int kTensorArenaSize = 2000;
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

// The name of this function is important for Arduino compatibility.
extern "C"
{
  void setup()
  {
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
      MicroPrintf("Model provided is schema version %d not equal to supported "
                  "version %d.",
                  model->version(), TFLITE_SCHEMA_VERSION);
      return;
    }

    static tflite::MicroMutableOpResolver<4> resolver;
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddReshape();
    resolver.AddRelu();

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
      MicroPrintf("AllocateTensors() failed");
      return;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);

    


    // Keep track of how many inferences we have performed.
    inference_count = 0;
  }

  // The name of this function is important for Arduino compatibility.
  void loop()
  {
    // Calculate an x value to feed into the model. We compare the current
    // inference_count to the number of inferences per cycle to determine
    // our position within the range of possible x values the model was
    // trained on, and use this to calculate a value.
    const int kInferencesPerCycle = 20;
    inference_count = 100;
    const float kXrange = 2.f * 3.14159265359f;
    float position = static_cast<int>(inference_count) /
                     static_cast<int>(kInferencesPerCycle);
    float x = position * kXrange;
    
    float fallData[20][6];
    
    for (int i = 0; i < 20; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        fallData[i][j] = static_cast<float>(rand() % 21 - 10);
      }

      // Generating random gyroscope values in the range [-5, 5]
      for (int j = 3; j < 6; ++j)
      {
        fallData[i][j] = static_cast<float>(rand() % 11 - 5);
      }
    }
    // // Quantize the input from floating-point to integer
    // int8_t x_quantized = x / input->params.scale + input->params.zero_point;
    // // Place the quantized input in the model's input tensor
    // input->data.int8[0] = x_quantized;
    for(int i = 0 ; i < 20 ; ++i){
      for(int j = 0; j < 6; ++j){
        input->data.int8[i * 20 + j] = fallData[i][j];
        
      }
    }
    

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
      MicroPrintf("Invoke failed on x: %f\n",
                  static_cast<double>(x));
      return;
    }
    MicroPrintf("y_value: %f", static_cast<double>(output->data.int8[0]));
    // Get the input tensor shape
    int input_height = input->dims->data[1];
    int input_width = input->dims->data[2];
    int input_channels = input->dims->data[3];

    int output_height = output->dims->data[1];
    int output_width = output->dims->data[2];
    int output_channels = output->dims->data[3];

    std::cout << "Input shape: " << input_height << "x" << input_width << "x" << input_channels << std::endl;
    std::cout << "Onput shape: " << output_height << "x" << output_width << "x" << output_channels << std::endl;

    
  }
}
