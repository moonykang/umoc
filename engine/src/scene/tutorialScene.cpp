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
Result TutorialScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    // if (false)
    {
        model::Material* material = new model::Material();
        try(material->init(context));
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "brickwall/pbr.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                          rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent);
        shaderParameters.pixelShader = context->allocatePixelShader("brickwall/pbr.frag.spv");

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

        for (float i = -1; i <= 1; i += 0.5f)
        {
            for (float j = -1; j <= 1; j += 0.5f)
            {
                util::Transform transform;
                transform.scale(glm::vec3(0.2f));
                transform.translate(glm::vec3(i, j, 0));
                model::Instance* instance = object->instantiate(context, transform.get(), true);
            }
        }
    }

    view->setView(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result TutorialScene::udpate(platform::Context* context)
{
    timer++;

    glm::vec3 lightPos;
    lightPos.x = cos(glm::radians(timer * 1.0f)) * 5.0f;
    lightPos.y = sin(glm::radians(timer * 1.0f)) * 5.0f;
    lightPos.z = -10.0f;

    light->setLight(glm::vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f));

    try(light->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));
    return Result::Continue;
}
} // namespace scene
