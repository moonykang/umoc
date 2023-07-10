#include "ibpbr.h"
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
Result IBPBRScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    {

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "forward.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("forward.frag.spv");
        // sponza
        auto loader = model::gltf::Loader::Builder()
                          .setPath("sponza/")
                          .setFileName("sponza.gltf")
                          //.setMaterialFlags(model::MaterialFlag::BaseColorTexture)
                          .setMaterialFlags(model::MaterialFlag::All)
                          .setShaderParameters(&shaderParameters)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);

        object->instantiate(context, glm::mat4(1.0f), true);
    }

    if (true)
    {
        model::Material* material = new model::Material();
        try(material->init(context));

        // albedo 0
        {
            auto [id, texture] = textures->get(context, "cerberus_albedo", "cerberus/albedo.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }

        // normal 1
        {
            auto [id, texture] = textures->get(context, "cerberus_normal", "cerberus/normal.ktx");
            material->updateTexture(model::MaterialFlag::NormalTexture, texture);
        }

        // ao 2
        {
            auto [id, texture] = textures->get(context, "cerberus_ao", "cerberus/ao.ktx");
            material->updateTexture(model::MaterialFlag::OcclusionTexture, texture);
        }

        // metallic 3
        {
            auto [id, texture] = textures->get(context, "cerberus_metallic", "cerberus/metallic.ktx");
            material->updateTexture(model::MaterialFlag::MetalicRoughnessTexture, texture);
        }

        // roughness (use diffuse) 4
        {
            auto [id, texture] = textures->get(context, "cerberus_roughness", "cerberus/roughness.ktx");
            material->updateTexture(model::MaterialFlag::DiffuseTexture, texture);
        }
        // ceberus (PBR)
        // auto loader = model::

        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getIrradianceCube());
            material->updateTexture(model::MaterialFlag::External, renderTargets->getBrdfLutTexture());
            material->updateTexture(model::MaterialFlag::External, renderTargets->getPreFilterCube());
        }

        try(material->init(context));
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader =
            context->allocateVertexShader("pbr.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                                              rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent);
        shaderParameters.pixelShader = context->allocatePixelShader("pbr.frag.spv");

        auto loader = model::gltf::Loader::Builder()
                          .setPath("cerberus/")
                          .setFileName("cerberus.gltf")
                          //.setMaterialFlags(model::MaterialFlag::BaseColorTexture)
                          .setMaterialFlags(model::MaterialFlag::All)
                          .addExternalMaterial(material)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);

        object->instantiate(context, glm::mat4(1.0f), true);
    }

    view->setView(glm::vec3(3.0f, 1.0f, -3.0f), glm::vec3(-10.0f, 30.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(-1.0f, -3.0f, 0.0f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result IBPBRScene::udpate(platform::Context* context)
{
    try(view->updateUniformBuffer(context));
    return Result::Continue;
}
} // namespace scene
