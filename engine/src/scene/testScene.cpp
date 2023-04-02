#include "testScene.h"
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
#include "scene/view.h"
#include "textures.h"

namespace scene
{
Result TestScene::postInit(platform::Context* context)
{
    return Result::Continue;
}

Result TestScene::load(platform::Context* context)
{
    {
        // sponza
        auto loader = model::gltf::Loader::Builder()
                          .setPath("sponza/")
                          .setFileName("sponza.gltf")
                          //.setMaterialFlags(model::MaterialFlag::BaseColorTexture)
                          .setMaterialFlags(model::MaterialFlag::All)
                          .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                          .build();

        model::Object* object = loader->load(context, this);
        registerObject(context, object);

        object->instantiate(context, glm::mat4(1.0f), true);
    }

    /*
        {
            model::gltf::Material* material = new model::gltf::Material();
            try(material->init(context));

            // albedo
            {
                auto [id, texture] = textures->get(context, "cerberus_albedo", "cerberus/albedo.ktx");
                material->updateTexture(model::MaterialFlag::BaseColorTexture, texture);
            }

            // normal
            {
                auto [id, texture] = textures->get(context, "cerberus_normal", "cerberus/normal.ktx");
                material->updateTexture(model::MaterialFlag::NormalTexture, texture);
            }

            // ao
            {
                auto [id, texture] = textures->get(context, "cerberus_ao", "cerberus/ao.ktx");
                material->updateTexture(model::MaterialFlag::OcclusionTexture, texture);
            }

            // metallic
            {
                auto [id, texture] = textures->get(context, "cerberus_metallic", "cerberus/metallic.ktx");
                material->updateTexture(model::MaterialFlag::MetalicRoughnessTexture, texture);
            }

            // roughness (use diffuse)
            {
                auto [id, texture] = textures->get(context, "cerberus_roughness", "cerberus/roughness.ktx");
                material->updateTexture(model::MaterialFlag::DiffuseTexture, texture);
            }
            // ceberus (PBR)
            // auto loader = model::

            auto loader = model::gltf::Loader::Builder()
                              .setPath("cerberus/")
                              .setFileName("cerberus.gltf")
                              //.setMaterialFlags(model::MaterialFlag::BaseColorTexture)
                              .setMaterialFlags(model::MaterialFlag::All)
                              .addExternalMaterial(material)
                              .setGltfLoadingFlags(model::GltfLoadingFlag::FlipY)
                              .build();

            model::Object* object = loader->load(context, this);
            registerObject(context, object);

            object->instantiate(context, glm::mat4(1.0f), true);
        }
    */
    view->setView(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f));
    view->setPerspective(45.0f, 1, 0.1f, 64.f);
    view->updateViewMatrix();

    try(view->updateUniformBuffer(context));

    light->setLight(glm::vec4(-1.0f, -3.0f, 0.0f, 1.0f));

    try(light->updateUniformBuffer(context));

    try(updateDescriptor(context));

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
