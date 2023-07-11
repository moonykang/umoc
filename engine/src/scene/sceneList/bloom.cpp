#include "bloom.h"
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
Result BloomScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    // Skybox
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // albedo 0
        {
            auto [id, texture] = textures->get(context, "Environment Cube Texture", "gcanyon_cube.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "skybox/skybox.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv);
        shaderParameters.pixelShader = context->allocatePixelShader("skybox/skybox.frag.spv");

        auto loader = model::gltf::Loader::Builder()
                          .setFileName("cube.gltf")
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .addExternalMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(platformContext, this);
        registerObject(context, object);

        util::Transform transform;
        transform.scale(glm::vec3(1.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true, true);
    }

    // Reflection sphere
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // albedo 0
        {
            auto [id, texture] = textures->get(context, "Environment Cube Texture", "gcanyon_cube.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "skybox/reflection.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("skybox/refraction.frag.spv");

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

        util::Transform transform;
        transform.scale(glm::vec3(1.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true, true);
    }

    view->setView(glm::vec3(4.11f, 1.12f, -6.73f), glm::vec3(-3.69f, 36.8f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}
} // namespace scene