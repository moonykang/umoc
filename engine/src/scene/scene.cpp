#include "scene.h"
#include "light.h"
#include "model/object.h"
#include "platform/context.h"
#include "platform/input.h"
#include "platform/window.h"
#include "rendertargets.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "textures.h"
#include "view.h"

namespace scene
{
SceneInfo::SceneInfo() : view(nullptr), light(nullptr), renderTargets(nullptr), sceneDescriptorSet(nullptr), timer(0)
{
}

Result SceneInfo::postInit(platform::Context* context)
{
    return Result::Continue;
}

Result SceneInfo::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    sceneDescriptorSet = context->allocateDescriptorSet();

    uint32_t binding = 0;
    rhi::DescriptorInfoList descriptorInfoList;
    descriptorInfoList.push_back({binding++, rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel,
                                  rhi::DescriptorType::Uniform_Buffer_Dynamic}); // view
    descriptorInfoList.push_back({binding++, rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel,
                                  rhi::DescriptorType::Uniform_Buffer_Dynamic}); // light

    try(sceneDescriptorSet->init(context, descriptorInfoList));

    view = new View();
    try(view->init(platformContext));

    light = new Light();
    try(light->init(platformContext));

    try(platformContext->getWindow()->getInput()->attach(view));

    renderTargets = new RenderTargets();
    try(renderTargets->init(platformContext));

    textures = new Textures();

    return postInit(platformContext);
}

Result SceneInfo::udpate(platform::Context* context)
{
    try(view->updateUniformBuffer(context));
    return Result::Continue;
}

void SceneInfo::preTerminate(platform::Context* context)
{
}

void SceneInfo::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    preTerminate(platformContext);
    platformContext->getWindow()->getInput()->dettach();

    TERMINATE(textures, context);
    TERMINATE(sceneDescriptorSet, context);
    TERMINATE(light, platformContext);
    TERMINATE(view, platformContext);
    TERMINATE(renderTargets, platformContext);

    for (auto& model : models)
    {
        TERMINATE(model, platformContext);
    }

    postTerminate(platformContext);
}

void SceneInfo::postTerminate(platform::Context* context)
{
}

void SceneInfo::registerObject(platform::Context* context, model::Object* object)
{
    models.push_back(object);
}

Result SceneInfo::updateDescriptor(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    rhi::UniformBuffer* viewUniformBuffer = view->getUniformBuffer();
    rhi::UniformBuffer* lightUniformBuffer = light->getUniformBuffer();

    std::vector<uint32_t> offsets;
    offsets.push_back(viewUniformBuffer->getOffset());
    offsets.push_back(lightUniformBuffer->getOffset());

    uint32_t binding = 0;
    rhi::DescriptorList descriptorList;
    descriptorList.push_back(
        {{binding++, rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic},
         viewUniformBuffer->getBufferDescriptor()});
    descriptorList.push_back(
        {{binding++, rhi::ShaderStage::Vertex | rhi::ShaderStage::Pixel, rhi::DescriptorType::Uniform_Buffer_Dynamic},
         lightUniformBuffer->getBufferDescriptor()});
    try(sceneDescriptorSet->update(context, descriptorList, offsets));

    return Result::Continue;
}

rhi::DescriptorSet* SceneInfo::getDescriptorSet()
{
    return sceneDescriptorSet;
}
} // namespace scene
