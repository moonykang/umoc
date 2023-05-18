#include "tutorialScene.h"
#include "common/util.h"
#include "model/gltf/loader.h"
#include "model/gltf/material.h"
#include "model/instance.h"
#include "model/object.h"
#include "model/predefined/loader.h"
#include "platform/context.h"
#include "rendertargets.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "scene/light.h"
#include "scene/view.h"
#include "textures.h"

namespace scene
{
Result TutorialScene::postInit(platform::Context* context)
{
    return Result::Continue;
}

Result TutorialScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    {
        model::Material* material = new model::Material();
        try(material->init(context));

        // albedo 0
        {
            auto [id, texture] = textures->get(context, "brickwall diffuse", "brickwall/diffuse.png");
            material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
        }

        // normal 1
        {
            auto [id, texture] = textures->get(context, "brickwall normal", "brickwall/normal.png");
            material->updateTexture(model::MaterialFlag::NormalTexture, texture);
        }

        try(material->init(context));
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "brickwall/phong.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                            rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent);
        shaderParameters.pixelShader = context->allocatePixelShader("brickwall/phong.frag.spv");

        LOGD("shaderParameters %p VS %p PS %p", &shaderParameters, shaderParameters.vertexShader,
             shaderParameters.pixelShader);

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Quad)
                          .setMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(context, this);
        LOGD("object %p", object);
        registerObject(context, object);

        object->instantiate(context, glm::mat4(1.0f), true);
    }

    view->setView(glm::vec3(3.0f, 1.0f, -3.0f), glm::vec3(-10.0f, 30.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result TutorialScene::udpate(platform::Context* context)
{
    timer = timer++;

    glm::vec3 lightPos;
    lightPos.x = cos(glm::radians(timer * 360.0f)) * 40.0f;
    lightPos.y = -50.0f + sin(glm::radians(timer * 360.0f)) * 20.0f;
    lightPos.z = 25.0f + sin(glm::radians(timer * 360.0f)) * 5.0f;

    // light->setLight(glm::vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f));

    try(light->updateUniformBuffer(context));

    LOGD("%f %f %f", lightPos.x, lightPos.y, lightPos.z);
    try(view->updateUniformBuffer(context));
    return Result::Continue;
}

void TutorialScene::preTerminate(platform::Context* context)
{
}

void TutorialScene::postTerminate(platform::Context* context)
{
}
} // namespace scene
