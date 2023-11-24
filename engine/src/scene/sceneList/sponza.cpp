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
                          .setFileName("sponza.gltf")
                          .setMaterialFlags(model::MaterialFlag::All)
                          .setShaderParameters(&shaderParameters)
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
    lights->setLightNumber(5);
    {
        auto& directionalLight = lights->getDirectionalLight();
        directionalLight.setProjection(45.0f, 1.0f, 1.0f, 96.0f);

        directionalLight.rotate(glm::vec3(90.0f, 0.0f, 0.f));
        directionalLight.translate(glm::vec3(1.f, 25.f, 0.f));
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
            light.set_light_position(glm::vec3(-1.f, 1.f, 1.f));
            light.set_light_color(glm::vec3(1.0f, 1.0f, 0.6f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }

        // Green
        {
            auto& light = lights->getLight(4);
            light.set_light_position(glm::vec3(1.f, 1.f, 1.f));
            light.set_light_color(glm::vec3(0.0f, 1.0f, 0.2f));
            light.set_light_radius(30.f);
            light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        }
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
        /*
        glm::vec3 position;
        position.x = 0;
        // sin(glm::radians(speed * 360.f)) * 5.f;
        position.y = 17.f;
        position.z = 0; // cos(glm::radians(speed * 360.f)) * 5.f;
        directionalLight.setPosition(position);
        // directionalLight.rotate(glm::vec3(1.0f, 0.0f, 1.f));
*/
        {
            float x = cos(glm::radians(speed * 360.f)) * 2.f;
            float z = cos(glm::radians(speed * 360.f)) * 2.f;
            lights->setLightPosition(1, glm::vec3(x, 1.f, z));
        }
    }
    try(lights->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene
