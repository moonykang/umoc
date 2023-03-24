#pragma once

#include "common/util.h"
#include "model/loader.h"
#include <map>

namespace model
{
class Material;
namespace predefined
{
class Loader : public model::Loader
{
  public:
    class Builder
    {
      public:
        Builder();

        Builder& setMaterial(Material* material);

        std::shared_ptr<Loader> build();

      private:
        Material* material;
    };

  public:
    Loader(Material* material);

    Object* load(platform::Context* context) override;

  private:
    Material* material;
};
} // namespace predefined
} // namespace model