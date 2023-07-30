#include "pbr.h"
#include "common/util.h"
#include "model/gltf/loader.h"
#include "model/gltf/material.h"
#include "model/instance.h"
#include "model/object.h"
#include "model/predefined/loader.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "scene/light.h"
#include "scene/rendertargets.h"
#include "scene/textures.h"
#include "scene/view.h"

namespace scene
{
namespace pbr
{
struct PushBlock
{
    float roughness;
    float metallic;
    float specular;
    float r;
    float g;
    float b;
} pushBlock;
} // namespace pbr

Result PBRScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    renderingOptions.enableForwardRendering();
    renderingOptions.enableBloom();

    // if (false)
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "brickwall/pbr.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                          rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent);
        shaderParameters.pixelShader = context->allocatePixelShader("brickwall/pbr.frag.spv");

        auto loader = model::gltf::Loader::Builder()
                          .setPath("")
                          .setFileName("sphere.gltf")
                          .setMaterialFlags(model::MaterialFlag::NONE)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .setShaderParameters(&shaderParameters)
                          .addExternalMaterial(material)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);

        float specular[7] = {1, 2, 4, 8, 16, 32, 64};
        pbr::PushBlock pushblocks[25];
        for (int i = 0; i < 5; i++)
        {
            float x = -1 + (i * 0.5f);

            for (int j = 0; j < 5; j++)
            {
                float y = -1 + (j * 0.5f);

                pbr::PushBlock& pushblock = pushblocks[i * 7 + j];
                pushblock.roughness = i * 0.2f + 0.1f;
                pushblock.metallic = j * 0.2f + 0.1f;
                pushblock.specular = specular[j];
                pushblock.r = 0.8f;
                pushblock.b = 0.8f;
                pushblock.g = 0.8f;

                util::Transform transform;
                transform.scale(glm::vec3(0.14f));
                transform.translate(glm::vec3(x, y, 0));
                model::Instance* instance = object->instantiate(context, transform.get(), true);
                instance->getPushConstantBlock().udpate(rhi::ShaderStage::Pixel, sizeof(pbr::PushBlock), &pushblock);
            }
        }
    }

    view->setView(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    lights->setLightPosition(0, glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result PBRScene::udpate(platform::Context* context)
{
    timer++;

    glm::vec3 lightPos;
    lightPos.x = cos(glm::radians(timer * 1.0f)) * 5.0f;
    lightPos.y = sin(glm::radians(timer * 1.0f)) * 5.0f;
    lightPos.z = -10.0f;

    lights->setLightPosition(0, glm::vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f));

    try(lights->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));
    return Result::Continue;
}
} // namespace scene
