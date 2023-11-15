#include "shadow.h"
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
Result ShadowScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    renderingOptions.enableForwardRendering();
    renderingOptions.setFinalTarget(renderTargets->getSceneColor());
    // renderingOptions.setFinalTarget(renderTargets->getShadowDepth());

    rhi::ShaderParameters shaderParameters;
    shaderParameters.vertexShader = context->allocateVertexShader(
        "forward/forward.vert.spv",
        rhi::VertexChannel::Position | rhi::VertexChannel::Uv | rhi::VertexChannel::Normal | rhi::VertexChannel::Color);
    shaderParameters.pixelShader = context->allocatePixelShader("forward/forward.frag.spv");

    if (true)
    {
        auto [id, redBlueTexture] = textures->get(context, "Red Blue Texture", "redblue.png");
        model::Material* material = new model::Material();
        try(material->init(platformContext));
        material->updateTexture(model::MaterialFlag::External, redBlueTexture, rhi::ShaderStage::Pixel);
        material->updateTexture(model::MaterialFlag::External, getRenderTargets()->getShadowDepth(),
                                rhi::ShaderStage::Pixel);
        try(material->update(platformContext));

        auto loader = model::gltf::Loader::Builder()
                          .setPath("")
                          .setFileName("vulkanscene_shadow.gltf")
                          .setMaterialFlags(model::MaterialFlag::All)
                          .addExternalMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(platformContext, this);
        registerObject(context, object);
        object->getPolygonState().update(rhi::CullMode::BACK_BIT);
        object->getPolygonState().update(rhi::PolygonMode::FILL);
        object->getPolygonState().update(rhi::FrontFace::COUNTER_CLOCKWISE);

        util::Transform transform;
        transform.scale(glm::vec3(1.f));
        model::Instance* instance = object->instantiate(context, transform.get(), true);
    }
    else
    {
        {
            auto [id, redBlueTexture] = textures->get(context, "Red Blue Texture", "redblue.png");
            model::Material* material = new model::Material();
            try(material->init(platformContext));
            material->updateTexture(model::MaterialFlag::External, redBlueTexture, rhi::ShaderStage::Pixel);
            material->updateTexture(model::MaterialFlag::External, getRenderTargets()->getShadowDepth(),
                                    rhi::ShaderStage::Pixel);
            try(material->update(platformContext));

            auto loader = model::gltf::Loader::Builder()
                              .setPath("")
                              .setFileName("sphere.gltf")
                              .setMaterialFlags(model::MaterialFlag::All)
                              .addExternalMaterial(material)
                              .setShaderParameters(&shaderParameters)
                              .build();

            model::Object* object = loader->load(platformContext, this);
            registerObject(context, object);
            object->getPolygonState().update(rhi::CullMode::FRONT_BIT);
            object->getPolygonState().update(rhi::PolygonMode::FILL);
            object->getPolygonState().update(rhi::FrontFace::CLOCKWISE);

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        util::Transform transform;
                        transform.scale(glm::vec3(0.5f));
                        transform.translate(glm::vec3(-2.f + (i * 2.f), -2.f + (j * 2.f), -2.f + (k * 2.f)));
                        model::Instance* instance = object->instantiate(context, transform.get(), true);
                    }
                }
            }
        }

        {
            auto [id, checkTexture] = textures->get(context, "Check Texture", "check.png");
            model::Material* material = new model::Material();
            try(material->init(platformContext));
            material->updateTexture(model::MaterialFlag::External, checkTexture, rhi::ShaderStage::Pixel);
            material->updateTexture(model::MaterialFlag::External, getRenderTargets()->getShadowDepth(),
                                    rhi::ShaderStage::Pixel);
            try(material->update(platformContext));

            auto loader = model::predefined::Loader::Builder()
                              .setMaterial(material)
                              .setPredefineModelType(model::PredefinedModel::Cube)
                              .setShaderParameters(&shaderParameters)
                              .setUvScale(25.f)
                              .build();

            model::Object* object = loader->load(platformContext, this);
            object->getPolygonState().update(rhi::CullMode::FRONT_BIT);
            object->getPolygonState().update(rhi::PolygonMode::FILL);
            object->getPolygonState().update(rhi::FrontFace::CLOCKWISE);

            registerObject(context, object);

            util::Transform transform;
            transform.scale(glm::vec3(100.f, 0.1f, 100.f));
            transform.translate(glm::vec3(0.f, 3.f, 0.f));
            model::Instance* instance = object->instantiate(context, transform.get(), true);
        }
    }

    view->setView(glm::vec3(0.1f, 0.0f, -1.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    // Lights
    {
        lights->setLightNumber(1);
        auto& directionalLight = lights->getDirectionalLight();
        directionalLight.setProjection(45.0f, 1.0f, 1.0f, 96.0f);

        directionalLight.rotate(glm::vec3(90.0f, 0.0f, 0.f));
        directionalLight.translate(glm::vec3(0.f, 10.f, 0.f));
    }
    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result ShadowScene::udpateScene(platform::Context* context)
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
