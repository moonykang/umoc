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
#include "ui.h"
#include "view.h"

namespace scene
{
SceneInfo::SceneInfo()
    : view(nullptr), lights(nullptr), ui(nullptr), renderTargets(nullptr), sceneDescriptorSet(nullptr), timer(0)
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

    lights = new Lights();
    try(lights->init(platformContext));

    ui = new UI();

    try(platformContext->getWindow()->getInput()->attach(view));

    renderTargets = new RenderTargets();
    try(renderTargets->init(platformContext));

    textures = new Textures();

    return postInit(platformContext);
}

Result SceneInfo::updateUI(platform::Context* platformContext)
{
    return Result::Continue;
}

Result SceneInfo::udpateScene(platform::Context* context)
{
    return Result::Continue;
}

Result SceneInfo::update(platform::Context* context)
{
    try(ui->startRender(context, this));
    try(updateUI(context));
    try(udpateScene(context));
    try(lights->updateUniformBuffer(context));
    try(view->updateUniformBuffer(context));
    return Result::Continue;
}

Result SceneInfo::postUpdate(platform::Context* context)
{
    try(ui->endRender());
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

    {
        delete ui;
        ui = nullptr;
    }
    TERMINATE(textures, context);
    TERMINATE(sceneDescriptorSet, context);
    TERMINATE(lights, platformContext);
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

    rhi::SubAllocatedBuffer* viewUniformBuffer = view->getUniformBuffer();
    rhi::SubAllocatedBuffer* lightUniformBuffer = lights->getUniformBuffer();

    std::vector<uint32_t> offsets;
    offsets.push_back(static_cast<uint32_t>(viewUniformBuffer->getOffset()));
    offsets.push_back(static_cast<uint32_t>(lightUniformBuffer->getOffset()));

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
