#include "particles.h"

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
Result ParticleScene::load(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    view->setType(View::Type::LookAt);
    renderingOptions.enableForwardRendering();
    renderingOptions.enableParticleRendering();

    // view->setView(glm::vec3(0.f, 0.0f, -14.0f), glm::vec3(-26.0f, 75.0f, 0.0f));
    view->rotate(glm::vec3(-26.0f, 75.0f, 0.0f));
    view->translate(glm::vec3(0.0f, 0.0f, -14.0f));
    view->setPerspective(60.0f, 1, 0.1f, 512.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    lights->setLightPosition(0, glm::vec4(0.5f, -1.0f, 0.3f, 1.0f));

    try(lights->updateUniformBuffer(context));

    try(updateDescriptor(context));

    return Result::Continue;
}
} // namespace scene