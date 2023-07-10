#include "loader.h"
#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"
#include "quad.h"

namespace model
{
namespace predefined
{
Loader::Builder::Builder() : predefinedType(PredefinedModel::Quad), material(nullptr)
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

std::shared_ptr<Loader> Loader::Builder::build()
{
    return std::make_shared<Loader>(predefinedType, material, shaderParameters);
}

Loader::Loader(PredefinedModel predefinedType, Material* material, rhi::ShaderParameters* shaderParameters)
    : predefinedType(predefinedType), material(material), shaderParameters(shaderParameters)
{
}

Object* createObject(PredefinedModel predefinedType)
{
    switch (predefinedType)
    {
    case PredefinedModel::Quad:
        return new Quad();
    }

    return nullptr; // need dummy
}

model::Object* Loader::load(platform::Context* context, scene::SceneInfo* sceneInfo)
{
    Object* newObject = createObject(predefinedType);

    try_call(newObject->init(context));
    try_call(newObject->loadVertexBuffer(context));
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