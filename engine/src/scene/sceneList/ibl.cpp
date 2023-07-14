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
        transform.rotate(glm::vec3(0, 180.0f, 0));
        transform.scale(glm::vec3(15.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true, true);
    }

    {
        model::Material* material = new model::Material();
        try(material->init(context));
        // Irradiance
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getIrradianceCube());
        }
        // BRDF
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getBrdfLutTexture());
        }
        // prefilter
        {
            material->updateTexture(model::MaterialFlag::External, renderTargets->getPreFilterCube());
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
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .setShaderParameters(&shaderParameters)
                          .addExternalMaterial(material)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);

        for (float i = -1; i <= 1; i += 0.3f)
        {
            for (float j = -1; j <= 1; j += 0.3f)
            {
                util::Transform transform;
                transform.scale(glm::vec3(0.14f));
                transform.translate(glm::vec3(i, j, 0));
                model::Instance* instance = object->instantiate(context, transform.get(), true);
            }
        }
    }

    view->setView(glm::vec3(1.950f, 0.1f, -3.7f), glm::vec3(-1.10f, 25.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 512.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLightPosition(0, glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result IBLScene::udpate(platform::Context* context)
{
    timer++;

    // White
    light->setLightPosition(
        0, glm::vec4(sin(glm::radians(360.0f * timer)) * 5.0f, cos(glm::radians(360.0f * timer)) * 5.0f, -10.0f, 0.0f));
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