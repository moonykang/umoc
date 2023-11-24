#include "ibl.h"
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
namespace ibl
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
} // namespace ibl

Result IBLScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    renderingOptions.enableForwardRendering();
    renderingOptions.enableEnvironmap();

    // Skybox
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // albedo 0
        {
            auto [id, texture] = textures->get(context, "Environment Cube Texture", "uffizi_cube.ktx");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture, rhi::ShaderStage::Pixel);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "skybox/skybox.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv);
        shaderParameters.pixelShader = context->allocatePixelShader("skybox/skybox.frag.spv");

        auto loader = model::gltf::Loader::Builder()
                          .setFileName("cube.gltf")
                          .addExternalMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(platformContext, this);
        object->getPolygonState().update(rhi::CullMode::BACK_BIT);
        object->getPolygonState().update(rhi::PolygonMode::FILL);
        object->getPolygonState().update(rhi::FrontFace::CLOCKWISE);
        registerObject(context, object);

        util::Transform transform;
        transform.rotate(glm::vec3(0, 180.0f, 0));
        transform.scale(glm::vec3(15.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true, true);
    }

    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // Irradiance
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getIrradianceCube(),
                                    rhi::ShaderStage::Pixel);
        }
        // BRDF
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getBrdfLutTexture(),
                                    rhi::ShaderStage::Pixel);
        }
        // prefilter
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getPreFilterCube(),
                                    rhi::ShaderStage::Pixel);
        }
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "ibl/pbr.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
        shaderParameters.pixelShader = context->allocatePixelShader("ibl/pbr.frag.spv");

        auto loader = model::gltf::Loader::Builder()
                          .setPath("")
                          .setFileName("sphere.gltf")
                          .setMaterialFlags(model::MaterialFlag::NONE)
                          .setShaderParameters(&shaderParameters)
                          .addExternalMaterial(material)
                          .build();

        model::Object* object = loader->load(context, this);
        object->getPolygonState().update(rhi::CullMode::BACK_BIT);
        object->getPolygonState().update(rhi::PolygonMode::FILL);
        object->getPolygonState().update(rhi::FrontFace::CLOCKWISE);
        registerObject(context, object);

        float specular[7] = {1, 2, 4, 8, 16, 32, 64};
        ibl::PushBlock pushblocks[49];
        for (int i = 0; i < 7; i++)
        {
            float x = -1 + (i * 0.3f);

            for (int j = 0; j < 7; j++)
            {
                float y = -1 + (j * 0.3f);

                ibl::PushBlock& pushblock = pushblocks[i * 7 + j];
                pushblock.roughness = i * 0.13f + 0.1f;
                pushblock.metallic = j * 0.13f + 0.1f;
                pushblock.specular = specular[j];
                pushblock.r = 0.8f;
                pushblock.b = 0.8f;
                pushblock.g = 0.8f;

                util::Transform transform;
                transform.scale(glm::vec3(0.14f));
                transform.translate(glm::vec3(x, y, 0));
                model::Instance* instance = object->instantiate(context, transform.get(), true);
                instance->getPushConstantBlock().udpate(rhi::ShaderStage::Pixel, sizeof(ibl::PushBlock), &pushblock);
            }
        }
    }

    view->setView(glm::vec3(1.56, 0.08f, -2.88f), glm::vec3(-1.10f, 25.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 512.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    // light->setLightPosition(0, glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    // Lights
    lights->setLightNumber(5);
    {
        auto& directionalLight = lights->getDirectionalLight();
        directionalLight.setProjection(45.0f, 1.0f, 1.0f, 96.0f);

        directionalLight.rotate(glm::vec3(90.0f, 0.0f, 0.f));
        directionalLight.translate(glm::vec3(40.f, 30.f, 25.f));
        directionalLight.get();

        // Red
        {
            auto& light = lights->getLight(1);
            light.set_light_position(glm::vec3(-1.f, 1.f, -1.f));
            light.set_light_color(glm::vec3(1.0f, 0.6f, 0.6f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }

        // Blue
        {
            auto& light = lights->getLight(2);
            light.set_light_position(glm::vec3(1.f, 1.f, -1.f));
            light.set_light_color(glm::vec3(0.7f, 0.7f, 1.0f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }

        // Yellow
        {
            auto& light = lights->getLight(3);
            light.set_light_position(glm::vec3(-1.f, -1.f, -1.f));
            light.set_light_color(glm::vec3(1.0f, 1.0f, 0.6f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }

        // Green
        {
            auto& light = lights->getLight(4);
            light.set_light_position(glm::vec3(1.f, -1.f, -1.f));
            light.set_light_color(glm::vec3(0.0f, 1.0f, 0.2f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }
    }
    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result IBLScene::udpateScene(platform::Context* context)
{
    timer++;
    try(lights->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));
    return Result::Continue;
}
} // namespace scene