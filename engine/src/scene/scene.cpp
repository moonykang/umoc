#include "scene.h"
#include "model/object.h"
#include "platform/context.h"
#include "platform/input.h"
#include "platform/window.h"
#include "view.h"

namespace scene
{
SceneInfo::SceneInfo() : view(nullptr)
{
}

Result SceneInfo::init(platform::Context* context)
{
    view = new View();
    try(view->init(context));

    try(context->getWindow()->getInput()->attach(view));

    return postInit(context);
}

void SceneInfo::terminate(platform::Context* context)
{
    preTerminate(context);
    context->getWindow()->getInput()->dettach();

    TERMINATE(view, context);

    for (auto& model : models)
    {
        TERMINATE(model, context);
    }

    postTerminate(context);
}

void SceneInfo::registerObject(platform::Context* context, model::Object* object)
{
    models.push_back(object);
}
} // namespace scene