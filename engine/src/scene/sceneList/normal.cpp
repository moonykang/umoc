#include "normal.h"
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
Result NormalScene::load(platform::Context* platformContext)
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
        try(material->update(context));

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "normal/normal.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Uv |
                                          rhi::VertexChannel::Normal | rhi::VertexChannel::Tangent |
                                          rhi::VertexChannel::Bitangent);
        shaderParameters.pixelShader = context->allocatePixelShader("normal/normal.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Quad)
                          .setMaterial(material)
                          .setShaderParameters(&shaderParameters)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);
        util::Transform transform;
        model::Instance* instance = object->instantiate(context, transform.get(), true);
    }

    view->setView(glm::vec3(1.5f, 0.0f, -3.0f), glm::vec3(0.0f, 25.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}

Result NormalScene::udpate(platform::Context* context)
{
    timer++;

    glm::vec3 lightPos;

    float rotation = timer / 1;
    lightPos.x = cos(glm::radians(rotation * 1.0f)) * 4.0f;
    lightPos.y = sin(glm::radians(rotation * 1.0f)) * 4.0f;
    lightPos.z = -4.0f;

    light->setLight(glm::vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f));

    try(light->updateUniformBuffer(context));

    try(view->updateUniformBuffer(context));

    return Result::Continue;
}
} // namespace scene