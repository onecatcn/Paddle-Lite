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

#include "core/operation/elementwise.h"
#include "driver/kunlunxin_xtcl/converter/converter.h"
#include "utility/debug.h"
#include "utility/logging.h"

namespace nnadapter {
namespace kunlunxin_xtcl {

int ConvertElementwise(Converter* converter, hal::Operation* operation) {
  ELEMENTWISE_OPERATION_EXTRACT_INPUTS_OUTPUTS

  // Convert to XTCL exprs
  auto input0_expr = converter->GetMappedExpr(input0_operand);
  if (!input0_expr.defined()) {
    input0_expr = converter->ConvertOperand(input0_operand);
  }
  auto input1_expr = converter->GetMappedExpr(input1_operand);
  if (!input1_expr.defined()) {
    input1_expr = converter->ConvertOperand(input1_operand);
  }
  xtcl::xExpr eltwise_expr;
  switch (operation->type) {
#define CONVERT_ELEMENTWISE(type, func)                     \
  case NNADAPTER_##type: {                                  \
    eltwise_expr = converter->builder()->func;              \
    converter->UpdateExprMap(output_operand, eltwise_expr); \
  } break;
    CONVERT_ELEMENTWISE(ADD, CreateBinaryOp("add", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(SUB,
                        CreateBinaryOp("subtract", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(MUL,
                        CreateBinaryOp("multiply", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(DIV,
                        CreateBinaryOp("divide", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(MAX,
                        CreateBinaryOp("maximum", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(MIN,
                        CreateBinaryOp("minimum", input0_expr, input1_expr));
    CONVERT_ELEMENTWISE(POW, CreateBinaryOp("power", input0_expr, input1_expr));
#undef CONVERT_ELEMENTWISE
    default:
      NNADAPTER_LOG(FATAL) << "Unsupported element-wise operation type "
                           << OperationTypeToString(operation->type)
                           << " is found.";
      break;
  }
  // fuse activations ?
  switch (fuse_code) {
#define CONVERT_UNARY_ACTIVATION(type, func)                              \
  case NNADAPTER_FUSED_##type:                                            \
    converter->UpdateExprMap(output_operand, converter->builder()->func); \
    break;
    CONVERT_UNARY_ACTIVATION(RELU, CreateRelu(eltwise_expr));
    CONVERT_UNARY_ACTIVATION(RELU6, CreateRelu6(eltwise_expr));
#undef CONVERT_UNARY_ACTIVATION
    case NNADAPTER_FUSED_NONE:
      break;
    default:
      NNADAPTER_LOG(FATAL) << "Unsupported fuse_code(" << fuse_code
                           << ") is found.";
      break;
  }
  return NNADAPTER_NO_ERROR;
}

}  // namespace kunlunxin_xtcl
}  // namespace nnadapter
