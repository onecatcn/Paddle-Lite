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

#include "core/operation/gelu.h"
#include "driver/kunlunxin_xtcl/converter/converter.h"
#include "utility/debug.h"
#include "utility/logging.h"

namespace nnadapter {
namespace kunlunxin_xtcl {

int ConvertGelu(Converter* converter, hal::Operation* operation) {
  GELU_OPERATION_EXTRACT_INPUTS_OUTPUTS
  NNADAPTER_CHECK_EQ(approximate, false)
      << "CANN only supports approximate=false.";

  // Convert to XTCL exprs
  auto input_expr = converter->GetMappedExpr(input_operand);
  if (input_expr.defined()) {
    input_expr = converter->ConvertOperand(input_operand);
  }
  auto gelu_expr = converter->builder()->CreateGelu(input_expr);
  converter->UpdateExprMap(output_operand, gelu_expr);
  return NNADAPTER_NO_ERROR;
}

}  // namespace kunlunxin_xtcl
}  // namespace nnadapter
