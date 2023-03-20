#include "scene.h"
#include "model/object.h"
#include "platform/context.h"
#include "platform/input.h"
#include "platform/window.h"
#include "rendertargets.h"
#include "view.h"

namespace scene
{
SceneInfo::SceneInfo() : view(nullptr), renderTargets(nullptr)
{
}

Result SceneInfo::init(platform::Context* context)
{
    view = new View();
    try(view->init(context));

    try(context->getWindow()->getInput()->attach(view));

    renderTargets = new RenderTargets();
    try(renderTargets->init(context));

    return postInit(context);
}

void SceneInfo::terminate(platform::Context* context)
{
    preTerminate(context);
    context->getWindow()->getInput()->dettach();

    TERMINATE(view, context);
    TERMINATE(renderTargets, context);

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