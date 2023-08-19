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
    // renderingOptions.setFinalTarget(renderTargets->getShadowDepth());

    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "deferred/geometry.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                          rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent);
    shaderParameters.pixelShader = context->allocatePixelShader("deferred/geometry.frag.spv");

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

    // Lights
    {
        lights->setLightNumber(1);
        auto& directionalLight = lights->getDirectionalLight();
        directionalLight.setProjection(45.0f, 1.0f, 1.0f, 96.0f);

        directionalLight.rotate(glm::vec3(90.0f, 0.0f, 0.f));
        directionalLight.translate(glm::vec3(1.f, 25.f, 0.f));
    }
    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result SponzaScene::udpateScene(platform::Context* context)
{
    timer++;

    auto frameTimer = context->getTimer().getCurrentFrameTimer();
    float speed = timer * 0.001f;

    // Lights
    {
        auto& directionalLight = lights->getDirectionalLight();
        // directionalLight.rotate(glm::vec3(1.0f, 0.0f, 1.f));
    }
    try(lights->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene
