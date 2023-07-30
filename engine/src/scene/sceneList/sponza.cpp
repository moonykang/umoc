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
    // renderingOptions.enableSSAO();
    // renderingOptions.setFinalTarget(renderTargets->getSSAOBlur());

    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "sponza/geometry.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal);
    shaderParameters.pixelShader = context->allocatePixelShader("sponza/geometry.frag.spv");

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

    lights->setLightPosition(0, glm::vec4(-1.0f, -3.0f, 0.0f, 1.0f));

    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result SponzaScene::udpate(platform::Context* context)
{
    timer++;
    lights->setNumLights(6);
    {
        glm::vec3 direction;
        direction.x = sin(glm::radians(360.0f * timer));
        direction.y = 1.0f;
        direction.z = cos(glm::radians(360.0f * timer));

        auto& light = lights->getLight(0);
        light.set_light_type(scene::LightType::LIGHT_TYPE_DIRECTIONAL);
        light.set_light_intensity(1);
        light.set_light_direction(glm::normalize(direction));
    }

    {
        glm::vec3 position;
        position.x = -4.0f + sin(glm::radians(360.0f * timer) + 45.0f) * 2.0f;
        position.y = 2.0f;
        position.z = 0.0f + cos(glm::radians(360.0f * timer) + 45.0f) * 2.0f;

        auto& light = lights->getLight(1);
        light.set_light_type(scene::LightType::LIGHT_TYPE_POINT);
        light.set_light_position(position);
        light.set_light_radius(10.0f);
        light.set_light_color(glm::vec3(1.0f, 0.0f, 0.0f));
    }

    // Blue
    lights->setLightPosition(2, glm::vec4(4.0f + sin(glm::radians(360.0f * timer)) * 2.0f, 2.0f,
                                          0.0f + cos(glm::radians(360.0f * timer)) * 2.0f, 0.0f));
    lights->setLightColor(2, glm::vec3(0.0f, 0.0f, 2.5f));
    lights->setLightRadius(2, 5.0f);
    // Yellow
    lights->setLightPosition(3, glm::vec4(0.0f + sin(glm::radians(360.0f * timer + 90.0f)) * 5.0f, 20.0f,
                                          0.0f - cos(glm::radians(360.0f * timer + 45.0f)) * 5.0f, 0.0f));
    lights->setLightColor(3, glm::vec3(1.0f, 1.0f, 0.0f));
    lights->setLightRadius(3, 5.0f);
    // Green
    lights->setLightPosition(4, glm::vec4(0.0f + sin(glm::radians(-360.0f * timer + 135.0f)) * 10.0f, 2.5f,
                                          0.0f - cos(glm::radians(-360.0f * timer - 45.0f)) * 10.0f, 0.0f));
    lights->setLightColor(4, glm::vec3(0.0f, 1.0f, 0.2f));
    lights->setLightRadius(4, 15.0f);

    // Yellow
    lights->setLightPosition(5, glm::vec4(0.0f, 3.0f, 0.0f, 0.0f));
    lights->setLightColor(5, glm::vec3(1.0f, 0.7f, 0.3f));
    lights->setLightRadius(5, 25.0f);

    try(lights->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene