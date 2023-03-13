#include "loader.h"
#include "material.h"
#include "object.h"

namespace model
{
Result Loader::loadMaterial(platform::Context* context, Object* object)
{
    Material* material = new Material();

    try(material->init(context));

    object->addMaterial(material);

    return Result::Continue;
}
} // namespace model