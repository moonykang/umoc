#pragma once

#include "common/util.h"
#include "model/loader.h"

namespace model
{
namespace predefined
{
class Loader : public model::Loader
{
  public:
    Object* load(platform::Context* context) override;

  private:
};
} // namespace predefined
} // namespace model