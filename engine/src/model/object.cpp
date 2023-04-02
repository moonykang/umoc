#include "object.h"
#include "instance.h"
#include "material.h"
#include "node.h"
#include "rhi/context.h"
#include "rhi/image.h"
#include "vertexInput.h"

namespace model
{
Object::Object() : vertexInput(nullptr)
{
}

Result Object::init(platform::Context* context)
{
    vertexInput = new VertexInput();

    return Result::Continue;
}

void Object::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    TERMINATE(vertexInput, platformContext);

    for (auto& material : materials)
    {
        TERMINATE(material, platformContext);
    }

    materials.clear();
    textures.clear();

    for (auto& instance : linearInstances)
    {
        TERMINATE(instance, platformContext);
    }
    linearInstances.clear();
}

void Object::draw(platform::Context* context)
{
    vertexInput->bind(context);
}

VertexInput* Object::getVertexInput()
{
    ASSERT(vertexInput);
    return vertexInput;
}

void Object::addTexture(TextureID texture)
{
    textures.push_back(texture);
}

TextureID Object::getTexture(uint32_t index)
{
    ASSERT(textures.size() > index);
    return textures[index];
}

void Object::addMaterial(Material* material)
{
    materials.push_back(material);
}

Material* Object::getMaterial(int32_t index)
{
    // ASSERT(materials.size() > index);
    return index > -1 ? materials[index] : materials.back();
}

void Object::addNode(Node* node)
{
    nodes.push_back(node);
}

void Object::addLinearNode(Node* node)
{
    linearNodes.push_back(node);
}

Instance* Object::instantiate(platform::Context* context, glm::mat4 transform, bool initDescriptor)
{
    Instance* prevInstance = nullptr;

    for (Node* node : linearNodes)
    {
        if (node->hasMesh())
        {
            const glm::mat4 localMatrix = transform * node->getTransform();

            for (Primitive* primitive : node->getPrimitives())
            {
                Instance* newInstance =
                    new Instance(this, primitive->material, primitive->firstIndex, primitive->indexCount,
                                 primitive->firstVertex, primitive->vertexCount, localMatrix);
                prevInstance = newInstance;

                try_call(newInstance->init(context, initDescriptor));

                if (initDescriptor)
                {
                    try_call(newInstance->updateUniformBuffer(context));
                }

                linearInstances.push_back(newInstance);
            }
        }
    }

    ASSERT(prevInstance != nullptr);

    return prevInstance;
}
std::vector<Instance*>& Object::getInstances()
{
    return linearInstances;
}
} // namespace model
