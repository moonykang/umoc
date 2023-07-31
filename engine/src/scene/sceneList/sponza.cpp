#include "sponza.h"
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
Result SponzaScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    renderingOptions.enableDeferredRendering();
    renderingOptions.setFinalTarget(renderTargets->getSceneColor());

    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "ssao/geometry.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
    shaderParameters.pixelShader = context->allocatePixelShader("ssao/geometry.frag.spv");
    /*
    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "forward.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
    shaderParameters.pixelShader = context->allocatePixelShader("forward.frag.spv");
    */
    // Sponza
    {
        auto loader = model::gltf::Loader::Builder()
                          .setPath("sponza/")
                          .setFileName("Sponza.gltf")
                          .setMaterialFlags(model::MaterialFlag::All)
                          .setShaderParameters(&shaderParameters)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .setForcedTextureExt("png")
                          .build();

        model::Object* object = loader->load(platformContext, this);
        registerObject(context, object);

        util::Transform transform;
        transform.scale(glm::vec3(1.0f));
        model::Instance* instance = object->instantiate(context, transform.get(), true);
    }

    view->setView(glm::vec3(5.0f, 1.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLightPosition(0, glm::vec4(-1.0f, -3.0f, 0.0f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result SponzaScene::udpate(platform::Context* context)
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
    light->setLightRadius(1, 10.0f);
    // Blue
    light->setLightPosition(2, glm::vec4(4.0f + sin(glm::radians(360.0f * timer)) * 2.0f, 2.0f,
                                         0.0f + cos(glm::radians(360.0f * timer)) * 2.0f, 0.0f));
    light->setLightColor(2, glm::vec3(0.0f, 0.0f, 2.5f));
    light->setLightRadius(2, 5.0f);
    // Yellow
    light->setLightPosition(3, glm::vec4(0.0f + sin(glm::radians(360.0f * timer + 90.0f)) * 5.0f, 20.0f,
                                         0.0f - cos(glm::radians(360.0f * timer + 45.0f)) * 5.0f, 0.0f));
    light->setLightColor(3, glm::vec3(1.0f, 1.0f, 0.0f));
    light->setLightRadius(3, 5.0f);
    // Green
    light->setLightPosition(4, glm::vec4(0.0f + sin(glm::radians(-360.0f * timer + 135.0f)) * 10.0f, 2.5f,
                                         0.0f - cos(glm::radians(-360.0f * timer - 45.0f)) * 10.0f, 0.0f));
    light->setLightColor(4, glm::vec3(0.0f, 1.0f, 0.2f));
    light->setLightRadius(4, 15.0f);

    // Yellow
    light->setLightPosition(5, glm::vec4(0.0f, 3.0f, 0.0f, 0.0f));
    light->setLightColor(5, glm::vec3(1.0f, 0.7f, 0.3f));
    light->setLightRadius(5, 25.0f);

    try(light->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene