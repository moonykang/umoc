#include "testScene.h"
#include "common/util.h"
#include "model/gltf/loader.h"
#include "model/instance.h"
#include "model/object.h"
#include "model/predefined/loader.h"
#include "platform/context.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/image.h"
#include "scene/view.h"

namespace scene
{
Result TestScene::postInit(platform::Context* context)
{
    return Result::Continue;
}

Result TestScene::load(platform::Context* context)
{
    auto loader = model::gltf::Loader::Builder()
                      .setPath("sponza/")
                      .setFileName("sponza.gltf")
                      .setMaterialFlags(model::MaterialFlag::BaseColorTexture)
                      .build();

    model::Object* object = loader->load(context);
    registerObject(context, object);

    object->instantiate(context, glm::mat4(1.0f));

    view->setView(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));
    try(view->updateDescriptor(context));

    return Result::Continue;
}

Result TestScene::udpate(platform::Context* context)
{
    try(view->updateUniformBuffer(context));
    return Result::Continue;
}

void TestScene::preTerminate(platform::Context* context)
{
}

void TestScene::postTerminate(platform::Context* context)
{
}
} // namespace scene
