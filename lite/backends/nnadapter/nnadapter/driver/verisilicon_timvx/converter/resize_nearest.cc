// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
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

#include "core/operation/resize_nearest.h"
#include "driver/verisilicon_timvx/converter/converter.h"
#include "utility/debug.h"
#include "utility/logging.h"

namespace nnadapter {
namespace verisilicon_timvx {

int ConvertResizeNearest(Converter* converter, hal::Operation* operation) {
  RESIZE_NEAREST_OPERATION_EXTRACT_INPUTS_OUTPUTS

  // Convert to tim-vx tensors and operators
  auto input_tensor = converter->GetMappedTensor(input_operand);
  if (!input_tensor) {
    input_tensor = converter->ConvertOperand(input_operand);
  }
  auto output_tensor = converter->ConvertOperand(output_operand);

  float factor = reinterpret_cast<float*>(input_operands[2]->buffer)[0];
  auto resize_op = converter->graph()->CreateOperation<tim::vx::ops::Resize>(
      tim::vx::ResizeType::NEAREST_NEIGHBOR,
      factor,
      align_corners,
      false,
      output_operand->type.dimensions.data[2],
      output_operand->type.dimensions.data[3]);
  resize_op->BindInputs({input_tensor});
  resize_op->BindOutputs({output_tensor});
  return NNADAPTER_NO_ERROR;
}

}  // namespace verisilicon_timvx
}  // namespace nnadapter
