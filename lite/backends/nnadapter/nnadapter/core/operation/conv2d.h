// Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "nnadapter.h"  // NOLINT

namespace nnadapter {
namespace operation {

#define CONV_2D_OPERATION_EXTRACT_INPUTS_OUTPUTS                               \
  auto& input_operands = operation->input_operands;                            \
  auto& output_operands = operation->output_operands;                          \
  auto input_count = input_operands.size();                                    \
  auto output_count = output_operands.size();                                  \
  NNADAPTER_CHECK_EQ(input_count, 9);                                          \
  NNADAPTER_CHECK_EQ(output_count, 1);                                         \
  /* Input */                                                                  \
  auto input_operand = input_operands[0];                                      \
  NNADAPTER_VLOG(5) << "input: " << OperandToString(input_operand);            \
  auto input_channel_size = input_operand->type.dimensions.data[1];            \
  /* Filter */                                                                 \
  auto filter_operand = input_operands[1];                                     \
  NNADAPTER_VLOG(5) << "filter: " << OperandToString(filter_operand);          \
  auto output_channel_size = filter_operand->type.dimensions.data[0];          \
  auto filter_channel_size = filter_operand->type.dimensions.data[1];          \
  auto filter_height = filter_operand->type.dimensions.data[2];                \
  auto filter_width = filter_operand->type.dimensions.data[3];                 \
  NNADAPTER_VLOG(5) << "filter dims = [" << output_channel_size << ","         \
                    << filter_channel_size << "," << filter_height << ","      \
                    << filter_width << "]";                                    \
  /* Bias */                                                                   \
  auto bias_operand = input_operands[2];                                       \
  NNADAPTER_VLOG(5) << "bias: " << OperandToString(bias_operand);              \
  /* Auto pad */                                                               \
  auto auto_pad = static_cast<NNAdapterAutoPadCode>(                           \
      *reinterpret_cast<int32_t*>(input_operands[3]->buffer));                 \
  NNADAPTER_VLOG(5) << "auto_pad: " << AutoPadCodeToString(auto_pad);          \
  /* Pads: Pads are transed according to auto_pad, so pads are used. */        \
  uint32_t pads_size =                                                         \
      input_operands[4]->length / static_cast<uint32_t>(sizeof(int32_t));      \
  NNADAPTER_CHECK_EQ(pads_size, 4U);                                           \
  auto pads_buffer = reinterpret_cast<int32_t*>(input_operands[4]->buffer);    \
  auto pad_height_top = pads_buffer[0];                                        \
  auto pad_height_bottom = pads_buffer[1];                                     \
  auto pad_width_left = pads_buffer[2];                                        \
  auto pad_width_right = pads_buffer[3];                                       \
  NNADAPTER_VLOG(5) << "paddings = [" << pad_height_top << ", "                \
                    << pad_height_bottom << ", " << pad_width_left << ", "     \
                    << pad_width_right << "]";                                 \
  /* Strides */                                                                \
  uint32_t strides_size =                                                      \
      input_operands[5]->length / static_cast<uint32_t>(sizeof(int32_t));      \
  NNADAPTER_CHECK_EQ(strides_size, 2U);                                        \
  auto strides_buffer = reinterpret_cast<int32_t*>(input_operands[5]->buffer); \
  auto stride_height = strides_buffer[0];                                      \
  auto stride_width = strides_buffer[1];                                       \
  NNADAPTER_VLOG(5) << "strides = [" << stride_height << ", " << stride_width  \
                    << "]";                                                    \
  /* Group */                                                                  \
  auto group = *reinterpret_cast<int32_t*>(input_operands[6]->buffer);         \
  NNADAPTER_VLOG(5) << "group = " << group;                                    \
  /* Dilations */                                                              \
  uint32_t dilations_size =                                                    \
      input_operands[7]->length / static_cast<uint32_t>(sizeof(int32_t));      \
  NNADAPTER_CHECK_EQ(dilations_size, 2U);                                      \
  auto dilations_buffer =                                                      \
      reinterpret_cast<int32_t*>(input_operands[7]->buffer);                   \
  auto dilation_height = dilations_buffer[0];                                  \
  auto dilation_width = dilations_buffer[1];                                   \
  NNADAPTER_VLOG(5) << "dilations = [" << dilation_height << ", "              \
                    << dilation_width << "]";                                  \
  /* Fuse code */                                                              \
  auto fuse_code = *reinterpret_cast<int32_t*>(input_operands[8]->buffer);     \
  NNADAPTER_VLOG(5) << "fuse_code = " << fuse_code;                            \
  /* Output */                                                                 \
  auto output_operand = output_operands[0];                                    \
  NNADAPTER_VLOG(5) << "output: " << OperandToString(output_operand);          \
  /* Check depthwise mode */                                                   \
  bool is_depthwise_mode = group != 1 && input_channel_size == group &&        \
                           output_channel_size % input_channel_size == 0;      \
  NNADAPTER_VLOG(5) << "depthwise mode(" << is_depthwise_mode << ").";

// Update the values of pad and dilation according to auto_pad and input_size
void UpdateConv2DPadAndDilation(int32_t input_size,
                                int32_t filter_height_or_width,
                                NNAdapterAutoPadCode auto_pad,
                                int32_t* pad_top_or_left,
                                int32_t* pad_bottom_or_right,
                                int32_t stride_height_or_width,
                                int32_t* dilation_height_or_width);
// Calculate the height or width of the output operand of Conv2D according to
// the pads, dilation, stride and etc.
int32_t CalcConv2DOutputSize(int32_t input_size,
                             int32_t filter_height_or_width,
                             NNAdapterAutoPadCode auto_pad,
                             int32_t pad_top_or_left,
                             int32_t pad_bottom_or_right,
                             int32_t stride_height_or_width,
                             int32_t dilation_height_or_width);

}  // namespace operation
}  // namespace nnadapter
