
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
// #include "test_model.h"
#include "output_handler.h"
#include <iostream>
#include <cstdlib> // For rand() function
#include <ctime>   // For time() function
#include <vector>

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  TfLiteTensor *output = nullptr;
  int inference_count = 100;

  constexpr int kTensorArenaSize = 30 * 1024;
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
    // model = tflite::GetModel(test_model);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
      MicroPrintf("Model provided is schema version %d not equal to supported "
                  "version %d.",
                  model->version(), TFLITE_SCHEMA_VERSION);
      return;
    }
    

    static tflite::MicroMutableOpResolver<4> resolver;
    resolver.AddReshape();
    resolver.AddRelu();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    
    


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

    int input_height = input->dims->data[1];
    int input_width = input->dims->data[2];
    int input_channels = input->dims->data[3];

    int output_height = output->dims->data[1];
    int output_width = output->dims->data[2];
    int output_channels = output->dims->data[3];

    int randomshit = output->dims->data[0];

    /* std::cout << input->dims->size << std::endl; */
    /* std::cout << output->dims->size << std::endl; */
    /* std::cout << "Input shape: " << input_height << "x" << input_width << "x" << input_channels << std::endl; */
    /* std::cout << "Onput shape: " << output_height << "x" << output_width << "x" << output_channels << std::endl; */
    /* std::cout << randomshit << std::endl; */
    
    

    // Keep track of how many inferences we have performed.
    inference_count = 0;
  }

  // The name of this function is important for Arduino compatibility.
  int infer(float * inputs)
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

    
    float data[20][6] = {
        {-2.69, -10.3, -2.08, 0.03, -0.24, 0.09},
        {-3.04, -6.78, -0.27, -0.97, -0.7, 0.49},
        {-2.63, -10.14, 2.2, -0.2, -0.79, 0.55},
        {-2.33, -9.56, -1.96, 0.26, -0.61, 0.43},
        {-2.11, -8.86, -0.94, -0.31, -0.38, 0.49},
        {-1.81, -9.55, -0.04, -0.13, -0.31, 0.4},
        {-3.08, -10.04, 0.18, -0.04, -0.22, 0.15},
        {-3.72, -10.82, -0.58, 0.02, -0.75, -0.01},
        {-4.68, -8.4, -2.24, -0.52, -1.11, -0.65},
        {-9.8, -4.07, 1.78, -1.44, -0.79, -0.25},
        {-26.04, -13.38, 1.39, -0.67, -1.87, 0.35},
        {-22.14, -18.79, 15.83, -1.21, -0.46, 6.68},
        {-17.4, -24.56, -2.04, 1.82, -2.99, 5.89},
        {-7.17, -19.69, -1.58, 0.48, -1.16, 6.1},
        {-4.19, -10.46, -7.44, 2.37, -1.4, 5.09},
        {1.43, -4.98, -6.21, 0.87, -2.68, 5.11},
        {1.71, -1.96, -7.31, 0.89, -2.02, 4.3},
        {-0.39, -3.33, -2.24, 0.03, 0.31, 4.84},
        {4.54, -3.21, 0.62, 3.86, 2.1, 3.45},
        {0.14, -0.73, -3.03, 2.73, 3.03, 1.97}};
    float randomFallData[20][6];
    
    

    /* std::cout << "Type: " << input->type << std::endl; */

    for(int i = 0; i < 120; ++i){
      input->data.f[i] = inputs[i];
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
      MicroPrintf("Invoke failed on x: %f\n",
                  static_cast<double>(x));
      return -1;
    }

    int argmax = 0;
    float value = output->data.f[0];

    for(int i = 0; i < 7; i++){
      if(value < output->data.f[i]){
        argmax = i;
        value = output->data.f[i];
      }
    }
    
    /* std:: cout << "Value : " << output->data.f[0] + output->data.f[1] + output->data.f[2] + output->data.f[3] + output->data.f[4] + output->data.f[5] + output->data.f[6] << std::endl; */
    /* std:: cout << "Argmax :" << argmax << std::endl; */

    return argmax;


    // // // Get the input tensor shape
    // int input_height = input->dims->data[1];
    // int input_width = input->dims->data[2];
    // int input_channels = input->dims->data[3];

    // int output_height = output->dims->data[1];
    // int output_width = output->dims->data[2];
    // int output_channels = output->dims->data[3];

    

    // std::cout << "Input shape: " << input_height << "x" << input_width << "x" << input_channels << std::endl;
    // std::cout << "Onput shape: " << output_height << "x" << output_width << "x" << output_channels << std::endl;
  }
}
