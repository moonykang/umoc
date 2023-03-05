#include "scene.h"
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

    return postInit(context);
}

void SceneInfo::terminate(platform::Context* context)
{
    preTerminate(context);

    TERMINATE(view, context);

    postTerminate(context);
}

model::VertexInput* SceneInfo::registerObject(platform::Context* context, model::VertexInput* object)
{
    return nullptr;
}
} // namespace scene