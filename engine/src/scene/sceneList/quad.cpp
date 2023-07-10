#include "quad.h"
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
Result TriangleScene::load(platform::Context* context)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    {

        rhi::ShaderParameters shaderParameters;
        shaderParameters.vertexShader = context->allocateVertexShader(
            "quad/triangle.vert.spv", rhi::VertexChannel::Position | rhi::VertexChannel::Color);
        shaderParameters.pixelShader = context->allocatePixelShader("quad/triangle.frag.spv");

        auto loader = model::predefined::Loader::Builder()
                          .setPredefineModelType(model::PredefinedModel::Quad)
                          .setShaderParameters(&shaderParameters)
                          //.addExternalMaterial(material)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);
        util::Transform transform;
        model::Instance* instance = object->instantiate(context, transform.get(), true);
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
} // namespace scene