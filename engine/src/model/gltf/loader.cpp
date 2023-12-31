#define TINYGLTF_IMPLEMENTATION
#include "loader.h"
#include "material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"
#include "platform/defines.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/image.h"
#include "scene/scene.h"
#include "scene/textures.h"
#include <sstream>

namespace model
{
namespace gltf
{
bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning,
                            int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
    // This function will be used for samples that don't require images to be loaded
    return true;
}

Loader::Builder::Builder()
    : path(""), gltfLoadingFlags(0), materialFlags(0), externalMaterial(nullptr), forcedTextureExt("")
{
}

Loader::Builder& Loader::Builder::setPath(std::string path)
{
    this->path = path;
    return *this;
}

Loader::Builder& Loader::Builder::setFileName(std::string name)
{
    this->fileName = name;
    return *this;
}

Loader::Builder& Loader::Builder::setGltfLoadingFlags(GltfLoadingFlags gltfLoadingFlags)
{
    this->gltfLoadingFlags = gltfLoadingFlags;
    return *this;
}

Loader::Builder& Loader::Builder::setMaterialFlags(MaterialFlags materialFlags)
{
    this->materialFlags = materialFlags;
    return *this;
}

Loader::Builder& Loader::Builder::addExternalMaterial(model::Material* material)
{
    externalMaterial = material;
    return *this;
}

Loader::Builder& Loader::Builder::setShaderParameters(rhi::ShaderParameters* shaderParameters)
{
    this->shaderParameters = shaderParameters;
    return *this;
}

Loader::Builder& Loader::Builder::setForcedTextureExt(std::string forcedTextureExt)
{
    this->forcedTextureExt = forcedTextureExt;
    return *this;
}

std::shared_ptr<Loader> Loader::Builder::build()
{
    return std::make_shared<Loader>(path, fileName, gltfLoadingFlags, materialFlags, externalMaterial, shaderParameters,
                                    forcedTextureExt);
}

Loader::Loader(std::string path, std::string fileName, GltfLoadingFlags gltfLoadingFlags, MaterialFlags materialFlags,
               model::Material* externalMaterial, rhi::ShaderParameters* shaderParameters, std::string forcedTextureExt)
    : path(path), fileName(fileName), gltfLoadingFlags(gltfLoadingFlags), materialFlags(materialFlags),
      externalMaterial(externalMaterial), shaderParameters(shaderParameters), forcedTextureExt(forcedTextureExt)
{
}

Object* Loader::load(platform::Context* platformContext, scene::SceneInfo* sceneInfo)
{
    Object* newObject = new Object();
    try_call(newObject->init(platformContext));

    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
    std::string error, warning;

    std::string gltfFileName = platformContext->getAssetManager()->getAssetPath() + "/" + path + fileName;

    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, gltfFileName);

    ASSERT(fileLoaded);

    if (!(gltfLoadingFlags & GltfLoadingFlag::DontLoadImages))
    {
        try_call(loadTextures(platformContext, sceneInfo, newObject));
    }

    // Use external material if is set.
    if (!externalMaterial)
    {
        try_call(loadMaterials(platformContext, sceneInfo, newObject));
    }
    else
    {
        if (shaderParameters)
        {
            externalMaterial->setShaderParameters(shaderParameters);
        }
        newObject->addMaterial(externalMaterial);
    }

    const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
    for (size_t i = 0; i < scene.nodes.size(); i++)
    {
        const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
        try_call(loadNode(nullptr, node, scene.nodes[i], 1.f, newObject));
    }

    try_call(newObject->getVertexInput()->loadVertexBuffer(platformContext, vertices));
    try_call(newObject->getVertexInput()->loadIndexBuffer(platformContext, indices));

    return newObject;
}

Result Loader::loadTextures(platform::Context* platformContext, scene::SceneInfo* sceneInfo, Object* object)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    for (tinygltf::Image& image : gltfModel.images)
    {
        bool isKtx = false;
        std::string name = image.uri;

        scene::Textures* sceneTextures = sceneInfo->getTextures();

        if (forcedTextureExt != "")
        {
            std::stringstream ss;

            size_t found = name.find_last_of(".");
            std::string prefix = name.substr(0, found);

            ss << prefix << "." << forcedTextureExt;
            std::string newName = ss.str();

            auto [id, texture] = sceneTextures->get(platformContext, newName, path + newName);
            object->addTexture(id);
        }
        else
        {
            auto [id, texture] = sceneTextures->get(platformContext, name, path + image.uri);
            object->addTexture(id);
        }
    }
    return Result::Continue;
}

Result Loader::loadMaterials(platform::Context* context, scene::SceneInfo* sceneInfo, Object* object)
{
    for (tinygltf::Material& mat : gltfModel.materials)
    {
        Material* material = new Material();

        try(material->init(context));

        if (shaderParameters)
        {
            material->setShaderParameters(shaderParameters);
        }

        if ((materialFlags & MaterialFlag::BaseColorTexture) != 0 &&
            mat.values.find("baseColorTexture") != mat.values.end())
        {
            TextureID textureID =
                object->getTexture(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
            material->updateTexture(MaterialFlag::BaseColorTexture, sceneInfo->getTextures()->get(textureID),
                                    rhi::ShaderStage::Pixel);
        }

        if ((materialFlags & MaterialFlag::MetalicRoughnessTexture) != 0 &&
            mat.values.find("metallicRoughnessTexture") != mat.values.end())
        {
            TextureID textureID =
                object->getTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
            material->updateTexture(MaterialFlag::MetalicRoughnessTexture, sceneInfo->getTextures()->get(textureID),
                                    rhi::ShaderStage::Pixel);
        }

        if ((materialFlags & MaterialFlag::NormalTexture) != 0 && mat.values.find("normalTexture") != mat.values.end())
        {
            TextureID textureID =
                object->getTexture(gltfModel.textures[mat.values["normalTexture"].TextureIndex()].source);
            material->updateTexture(MaterialFlag::NormalTexture, sceneInfo->getTextures()->get(textureID),
                                    rhi::ShaderStage::Pixel);
        }

        if ((materialFlags & MaterialFlag::EmissiveTexture) != 0 &&
            mat.values.find("emissiveTexture") != mat.values.end())
        {
            TextureID textureID =
                object->getTexture(gltfModel.textures[mat.values["emissiveTexture"].TextureIndex()].source);
            material->updateTexture(MaterialFlag::EmissiveTexture, sceneInfo->getTextures()->get(textureID),
                                    rhi::ShaderStage::Pixel);
        }

        if ((materialFlags & MaterialFlag::OcclusionTexture) != 0 &&
            mat.values.find("occlusionTexture") != mat.values.end())
        {
            TextureID textureID =
                object->getTexture(gltfModel.textures[mat.values["occlusionTexture"].TextureIndex()].source);
            material->updateTexture(MaterialFlag::OcclusionTexture, sceneInfo->getTextures()->get(textureID),
                                    rhi::ShaderStage::Pixel);
        }

        if (mat.values.find("roughnessFactor") != mat.values.end())
        {
            material->setRoughnessFactor(static_cast<float>(mat.values["roughnessFactor"].Factor()));
        }

        if (mat.values.find("metallicFactor") != mat.values.end())
        {
            material->setMetallicFactor(static_cast<float>(mat.values["metallicFactor"].Factor()));
        }

        if (mat.values.find("baseColorFactor") != mat.values.end())
        {
            material->setBaseColorFactor(glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data()));
        }

        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
        {
            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
            if (param.string_value == "BLEND")
            {
                material->setAlphaMode(AlphaMode::Blend);
            }
            if (param.string_value == "MASK")
            {
                material->setAlphaMode(AlphaMode::Mask);
            }
        }

        if (mat.values.find("alphaCutoff") != mat.values.end())
        {
            material->setAlphaCutoff(static_cast<float>(mat.values["alphaCutoff"].Factor()));
        }

        try(material->update(context));

        object->addMaterial(material);
    }

    try(loadMaterial(context, object));

    return Result::Continue;
}

Result Loader::loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, float globalscale, Object* object)
{
    Node* newNode = new Node(parent);
    newNode->index = nodeIndex;
    newNode->name = node.name;
    newNode->skinIndex = node.skin;
    newNode->matrix = glm::mat4(1.0f);

    // Generate local node matrix
    glm::vec3 translation = glm::vec3(0.0f);
    if (node.translation.size() == 3)
    {
        translation = glm::make_vec3(node.translation.data());
        newNode->translation = translation;
    }

    glm::mat4 rotation = glm::mat4(1.0f);
    if (node.rotation.size() == 4)
    {
        glm::quat q = glm::make_quat(node.rotation.data());
        newNode->rotation = glm::mat4(q);
    }

    glm::vec3 scale = glm::vec3(1.0f);
    if (node.scale.size() == 3)
    {
        scale = glm::make_vec3(node.scale.data());
        newNode->scale = scale;
    }

    if (node.matrix.size() == 16)
    {
        newNode->matrix = glm::make_mat4x4(node.matrix.data());
        if (globalscale != 1.0f)
        {
            // newNode->matrix = glm::scale(newNode->matrix, glm::vec3(globalscale));
        }
    };

    newNode->updateTransform();

    // Node with children
    if (node.children.size() > 0)
    {
        for (auto i = 0; i < node.children.size(); i++)
        {
            try(loadNode(newNode, gltfModel.nodes[node.children[i]], node.children[i], globalscale, object));
        }
    }

    // Node contains mesh data
    if (node.mesh > -1)
    {
        const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];
        Mesh* newMesh = new Mesh(mesh.name);
        newNode->setMesh(newMesh);

        for (size_t j = 0; j < mesh.primitives.size(); j++)
        {
            const tinygltf::Primitive& primitive = mesh.primitives[j];

            if (primitive.indices < 0)
            {
                continue;
            }

            uint32_t indexStart = static_cast<uint32_t>(indices.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertices.size());
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;
            glm::vec3 posMin{};
            glm::vec3 posMax{};
            bool hasSkin = false;

            // Vertices
            {
                const float* bufferPos = nullptr;
                const float* bufferNormals = nullptr;
                const float* bufferTexCoords = nullptr;
                const float* bufferColors = nullptr;
                const float* bufferTangents = nullptr;
                uint32_t numColorComponents;
                const uint16_t* bufferJoints = nullptr;
                const float* bufferWeights = nullptr;

                // Position attribute is required
                assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

                const tinygltf::Accessor& posAccessor =
                    gltfModel.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& posView = gltfModel.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const float*>(
                    &(gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& normAccessor =
                        gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView& normView = gltfModel.bufferViews[normAccessor.bufferView];
                    bufferNormals = reinterpret_cast<const float*>(
                        &(gltfModel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& uvAccessor =
                        gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView& uvView = gltfModel.bufferViews[uvAccessor.bufferView];
                    bufferTexCoords = reinterpret_cast<const float*>(
                        &(gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }

                if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& colorAccessor =
                        gltfModel.accessors[primitive.attributes.find("COLOR_0")->second];
                    const tinygltf::BufferView& colorView = gltfModel.bufferViews[colorAccessor.bufferView];
                    // Color buffer are either of type vec3 or vec4
                    numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
                    bufferColors = reinterpret_cast<const float*>(
                        &(gltfModel.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                }

                if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& tangentAccessor =
                        gltfModel.accessors[primitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView& tangentView = gltfModel.bufferViews[tangentAccessor.bufferView];
                    bufferTangents = reinterpret_cast<const float*>(
                        &(gltfModel.buffers[tangentView.buffer]
                              .data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                }

                // Skinning
                // Joints
                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& jointAccessor =
                        gltfModel.accessors[primitive.attributes.find("JOINTS_0")->second];
                    const tinygltf::BufferView& jointView = gltfModel.bufferViews[jointAccessor.bufferView];
                    bufferJoints = reinterpret_cast<const uint16_t*>(
                        &(gltfModel.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
                }

                if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& uvAccessor =
                        gltfModel.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                    const tinygltf::BufferView& uvView = gltfModel.bufferViews[uvAccessor.bufferView];
                    bufferWeights = reinterpret_cast<const float*>(
                        &(gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }

                hasSkin = (bufferJoints && bufferWeights);

                vertexCount = static_cast<uint32_t>(posAccessor.count);

                for (size_t v = 0; v < posAccessor.count; v++)
                {
                    rhi::Vertex vertex;
                    vertex.position = glm::make_vec3(&bufferPos[v * 3]);
                    vertex.normal = glm::normalize(
                        glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                    vertex.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);

                    if (bufferColors)
                    {
                        switch (numColorComponents)
                        {
                        case 3:
                            vertex.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
                        case 4:
                            vertex.color = glm::make_vec4(&bufferColors[v * 4]);
                        }
                    }
                    else
                    {
                        vertex.color = glm::vec4(1.0f);
                    }

                    glm::vec4 tangent =
                        bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
                    vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
                    vertex.bitangent =
                        bufferTangents ? glm::cross(vertex.normal, vertex.tangent) * tangent.w : glm::vec3(0.f);
                    vertex.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                    vertex.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);

                    if ((gltfLoadingFlags & model::GltfLoadingFlag::FlipY) != 0)
                    {
                        vertex.position.y *= -1.0f;
                        vertex.normal.y *= -1.0f;
                    }
                    vertices.push_back(vertex);
                }
            }

            // Indices
            {
                const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

                indexCount = static_cast<uint32_t>(accessor.count);

                switch (accessor.componentType)
                {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    uint32_t* buf = new uint32_t[accessor.count];
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                           accessor.count * sizeof(uint32_t));
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indices.push_back(buf[index] + vertexStart);
                    }
                    delete[] buf;
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    uint16_t* buf = new uint16_t[accessor.count];
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                           accessor.count * sizeof(uint16_t));
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indices.push_back(buf[index] + vertexStart);
                    }
                    delete[] buf;
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    uint8_t* buf = new uint8_t[accessor.count];
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                           accessor.count * sizeof(uint8_t));
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indices.push_back(buf[index] + vertexStart);
                    }
                    delete[] buf;
                    break;
                }
                default:
                    UNREACHABLE();
                    return Result::Fail;
                }
            }

            model::Material* material = externalMaterial ? externalMaterial : object->getMaterial(primitive.material);
            Primitive* newPrimitive = new Primitive(indexStart, indexCount, material);
            newPrimitive->firstVertex = vertexStart;
            newPrimitive->vertexCount = vertexCount;
            newMesh->addPrimitive(newPrimitive);
        }
    }

    if (parent)
    {
        parent->addChild(newNode);
    }
    else
    {
        object->addNode(newNode);
    }

    object->addLinearNode(newNode);

    return Result::Continue;
}
} // namespace gltf
} // namespace model
