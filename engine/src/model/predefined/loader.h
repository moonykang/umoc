#pragma once

#include "common/util.h"
#include "model/loader.h"
#include <map>
#include <memory>

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

        Builder& setShaderParameters(rhi::ShaderParameters* shaderParameters);

        std::shared_ptr<Loader> build();

      private:
        Material* material;
        rhi::ShaderParameters* shaderParameters;
    };

  public:
    Loader(Material* material, rhi::ShaderParameters* shaderParameters);

    Object* load(platform::Context* context, scene::SceneInfo* sceneInfo) override;

  private:
    Material* material;
    rhi::ShaderParameters* shaderParameters;
};
} // namespace predefined
} // namespace model