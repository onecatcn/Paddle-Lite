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

#include "core/operation/dequantize.h"
#include "driver/huawei_ascend_npu/converter/converter.h"
#include "utility/debug.h"
#include "utility/logging.h"

namespace nnadapter {
namespace huawei_ascend_npu {

int ConvertDequantize(Converter* converter, hal::Operation* operation) {
  DEQUANTIZE_OPERATION_EXTRACT_INPUTS_OUTPUTS
  NNADAPTER_CHECK(input_operand->type.precision ==
                  NNADAPTER_QUANT_INT32_SYMM_PER_LAYER)
      << "Only support NNADAPTER_QUANT_INT32_SYMM_PER_LAYER, but received "
      << OperandPrecisionCodeToString(input_operand->type.precision);

  // Convert to GE operators
  auto input_operator = converter->GetMappedOperator(input_operand);
  if (input_operator == nullptr) {
    input_operator = converter->ConvertOperand(input_operand);
  }
  float scale = input_operand->type.symm_per_layer_params.scale;
  uint64_t scale_uint64 =
      static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&scale));
  auto scale_operator =
      converter->AddUInt64ConstantOperator(&scale_uint64, {1});
  auto dequantize_op =
      converter->AddOperator<ge::op::AscendDequant>(output_operand);
  SET_INPUT(dequantize_op, x, input_operator);
  SET_INPUT(dequantize_op, deq_scale, scale_operator);
  MAP_OUTPUT(dequantize_op, y, output_operand);
  return NNADAPTER_NO_ERROR;
}

}  // namespace huawei_ascend_npu
}  // namespace nnadapter
