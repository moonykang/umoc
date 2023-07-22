#include "loader.h"
#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"
#include "quad.h"
#include "storage.h"

namespace model
{
namespace predefined
{
Loader::Builder::Builder()
    : predefinedType(PredefinedModel::Quad), material(nullptr), uvScale(1.0f), storageBuffer({nullptr, 0})
{
}

Loader::Builder& Loader::Builder::setPredefineModelType(PredefinedModel predefinedType)
{
    this->predefinedType = predefinedType;
    return *this;
}

Loader::Builder& Loader::Builder::setMaterial(Material* material)
{
    this->material = material;
    return *this;
}

Loader::Builder& Loader::Builder::setShaderParameters(rhi::ShaderParameters* shaderParameters)
{
    this->shaderParameters = shaderParameters;
    return *this;
}

Loader::Builder& Loader::Builder::setUvScale(double uvScale)
{
    this->uvScale = uvScale;
    return *this;
}

Loader::Builder& Loader::Builder::setExternalVertexBuffer(std::pair<rhi::StorageBuffer*, uint32_t> storageBuffer)
{
    this->storageBuffer = storageBuffer;
    return *this;
}

std::shared_ptr<Loader> Loader::Builder::build()
{
    return std::make_shared<Loader>(predefinedType, material, shaderParameters, uvScale, storageBuffer);
}

Loader::Loader(PredefinedModel predefinedType, Material* material, rhi::ShaderParameters* shaderParameters,
               double uvScale, std::pair<rhi::StorageBuffer*, uint32_t> storageBuffer)
    : predefinedType(predefinedType), material(material), shaderParameters(shaderParameters), uvScale(uvScale),
      storageBuffer(storageBuffer)
{
}

Object* createObject(PredefinedModel predefinedType)
{
    switch (predefinedType)
    {
    case PredefinedModel::Quad:
        return new Quad();
    case PredefinedModel::Cube:
    case PredefinedModel::Sphere:
        return nullptr; // TODO
    case PredefinedModel::Storage:
        return new Storage(); // TODO
    }

    return nullptr; // need dummy
}

model::Object* Loader::load(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    Object* newObject = createObject(predefinedType);

    try_call(newObject->init(context));

    if (storageBuffer.first)
    {
        try_call(newObject->loadVertexBuffer(context, storageBuffer));
    }
    else
    {
        try_call(newObject->loadVertexBuffer(context, uvScale));
    }
    try_call(newObject->loadIndexBuffer(context));

    if (material)
    {
        // try_call(material->init(context));
        // try_call(material->update(context));
        newObject->addMaterial(material);
    }
    else
    {
        try_call(loadMaterial(context, newObject));
    }

    auto localMaterial = newObject->getMaterial(-1);

    if (shaderParameters)
    {
        localMaterial->setShaderParameters(shaderParameters);
    }

    try_call(newObject->loadMesh());

    return newObject;
}
} // namespace predefined
} // namespace model