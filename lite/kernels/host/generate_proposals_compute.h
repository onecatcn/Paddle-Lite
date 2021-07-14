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

#pragma once
#include "lite/core/kernel.h"
#include "lite/operators/generate_proposals_op.h"

namespace paddle {
namespace lite {
namespace kernels {
namespace host {

class GenerateProposalsCompute
    : public KernelLite<TARGET(kHost), PRECISION(kFloat)> {
 public:
  using param_t = operators::GenerateProposalsParam;

  void Run() override;

  virtual ~GenerateProposalsCompute() = default;
};

}  // namespace host
}  // namespace kernels
}  // namespace lite
}  // namespace paddle