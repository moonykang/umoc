#include "textures.h"
#include "common/hash.h"
#include "platform/defines.h"
#include "rhi/context.h"
#include "rhi/image.h"
#include "rhi/resources.h"

namespace scene
{
void Textures::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    for (auto& texture : textureMap)
    {
        TERMINATE(texture.second, context);
    }
    textureMap.clear();
}

rhi::Texture* Textures::get(model::TextureID id)
{
    return textureMap[id];
}

std::pair<model::TextureID, rhi::Texture*> Textures::get(platform::Context* platformContext, std::string name,
                                                         std::string fullpath)
{
    static const size_t keySize = 64; // name.size()
    name.reserve(keySize);
    model::TextureID id = util::computeGenericHash(name.data(), keySize);

    rhi::Texture* texture = get(id);

    if (!texture)
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

        std::string path;
        std::string file;

        // Splitting path / file
        {
            std::size_t found = fullpath.find_last_of("/");

            if (found == std::string::npos)
            {
                path = "";
                file = fullpath;
            }
            else
            {
                path = fullpath.substr(0, found);
                file = fullpath.substr(found + 1);
            }
        }

        std::string extension = fullpath.substr(fullpath.find_last_of(".") + 1);

        platform::ImageLoader imageLoader =
            extension == "ktx" ? platform::ImageLoader::KTX : platform::ImageLoader::STB;

        texture = new rhi::Texture(name);
        try_call(texture->init(context, fullpath, imageLoader));

        textureMap[id] = texture;
    }
    return {id, texture};
}

std::pair<model::TextureID, rhi::Texture*> Textures::get(platform::Context* platformContext, std::string name,
                                                         rhi::Format format, rhi::Extent3D extent, size_t size,
                                                         void* data)
{
    LOGD("name %s", name.c_str());

    static const size_t keySize = 64; // name.size()
    name.reserve(keySize);
    model::TextureID id = util::computeGenericHash(name.data(), keySize);

    rhi::Texture* texture = get(id);

    if (!texture)
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

        texture = new rhi::Texture(name);
        try_call(texture->init(context, format, extent, rhi::ImageUsage::SAMPLED | rhi::ImageUsage::TRANSFER_DST, size,
                               data));

        textureMap[id] = texture;
    }
    return {id, texture};
}
} // namespace scene
