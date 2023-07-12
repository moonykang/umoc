#include "deferred.h"
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
Result DeferredScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    const bool bDeferred = true;
    // Floor
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // albedo 0
        {
            auto [id, texture] =
                textures->get(context, "Stone floor diffuse", "stonefloor/stonefloor01_color_rgba.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }
        // normal 1
        {
            auto [id, texture] =
                textures->get(context, "Stone floor normal", "stonefloor/stonefloor01_normal_rgba.ktx");
            material->updateTexture(model::MaterialFlag::NormalTexture, texture);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;

        if (bDeferred)
        {
            shaderParameters.vertexShader = context->allocateVertexShader(
                "deferred/geometry.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                                  rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent |
                                                  rhi::VertexChannel::Bitangent);
            shaderParameters.pixelShader = context->allocatePixelShader("deferred/geometry.frag.spv");
        }
        else
        {

            shaderParameters.vertexShader = context->allocateVertexShader(
                "normal/normal.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                              rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent |
                                              rhi::VertexChannel::Bitangent);
            shaderParameters.pixelShader = context->allocatePixelShader("normal/normal.frag.spv");
        }

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Quad)
                          .setMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .setUvScale(20.f)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);
        util::Transform transform;
        transform.translate(glm::vec3(0.0f, -10.0f, 0.0f));
        transform.rotate(glm::vec3(90.0f, 0.0f, 0.0f));
        transform.scale(glm::vec3(50.0f, 50.0f, 1.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true);
    }

    // armor
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // albedo 0
        {
            auto [id, texture] = textures->get(context, "Armor(albedo)", "armor/albedo.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }
        // normal 1
        {
            auto [id, texture] = textures->get(context, "Armor(normal)", "armor/normal.ktx");
            material->updateTexture(model::MaterialFlag::NormalTexture, texture);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        if (bDeferred)
        {
            shaderParameters.vertexShader = context->allocateVertexShader(
                "deferred/geometry.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                                  rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent |
                                                  rhi::VertexChannel::Bitangent);
            shaderParameters.pixelShader = context->allocatePixelShader("deferred/geometry.frag.spv");
        }
        else
        {

            shaderParameters.vertexShader = context->allocateVertexShader(
                "normal/normal.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                              rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent |
                                              rhi::VertexChannel::Bitangent);
            shaderParameters.pixelShader = context->allocatePixelShader("normal/normal.frag.spv");
        }

        auto loader = model::gltf::Loader::Builder()
                          .setPath("armor/")
                          .setFileName("armor.gltf")
                          .setMaterialFlags(model::MaterialFlag::NONE)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .setShaderParameters(&shaderParameters)
                          .addExternalMaterial(material)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);
        util::Transform transform;
        transform.rotate(glm::vec3(-90.0f, 0.0f, 0.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true);
    }

    view->setView(glm::vec3(-9.0f, 4.5f, -5.5f), glm::vec3(-9.9f, -70.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLightPosition(0, glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result DeferredScene::udpate(platform::Context* context)
{
    timer++;

    // White
    light->setLightPosition(
        0, glm::vec4(sin(glm::radians(360.0f * timer)) * 5.0f, 2.0f, cos(glm::radians(360.0f * timer)) * 5.0f, 0.0f));
    light->setLightColor(0, glm::vec3(1.5f));
    light->setLightRadius(0, 15.0f);

    // Red
    light->setLightPosition(1, glm::vec4(-4.0f + sin(glm::radians(360.0f * timer) + 45.0f) * 2.0f, 2.0f,
                                         0.0f + cos(glm::radians(360.0f * timer) + 45.0f) * 2.0f, 0.0f));
    light->setLightColor(1, glm::vec3(1.0f, 0.0f, 0.0f));
    light->setLightRadius(1, 30.0f);
    // Blue
    light->setLightPosition(2, glm::vec4(4.0f + sin(glm::radians(360.0f * timer)) * 2.0f, 2.0f,
                                         0.0f + cos(glm::radians(360.0f * timer)) * 2.0f, 0.0f));
    light->setLightColor(2, glm::vec3(0.0f, 0.0f, 2.5f));
    light->setLightRadius(2, 25.0f);
    // Yellow
    light->setLightPosition(3, glm::vec4(0.0f + sin(glm::radians(360.0f * timer + 90.0f)) * 5.0f, 20.0f,
                                         0.0f - cos(glm::radians(360.0f * timer + 45.0f)) * 5.0f, 0.0f));
    light->setLightColor(3, glm::vec3(1.0f, 1.0f, 0.0f));
    light->setLightRadius(3, 15.0f);
    // Green
    light->setLightPosition(4, glm::vec4(0.0f + sin(glm::radians(-360.0f * timer + 135.0f)) * 10.0f, 2.5f,
                                         0.0f - cos(glm::radians(-360.0f * timer - 45.0f)) * 10.0f, 0.0f));
    light->setLightColor(4, glm::vec3(0.0f, 1.0f, 0.2f));
    light->setLightRadius(4, 45.0f);

    // Yellow
    light->setLightPosition(5, glm::vec4(0.0f, 3.0f, 0.0f, 0.0f));
    light->setLightColor(5, glm::vec3(1.0f, 0.7f, 0.3f));
    light->setLightRadius(5, 25.0f);

    try(light->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene