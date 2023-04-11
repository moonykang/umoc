#include "platform/glfw/asset.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ktx.h>
#include <ktxvulkan.h>

#include "rhi/resources/glFormats.h"

namespace platform
{

Asset* Asset::createPlatformAsset()
{
    return new glfw::Asset();
}

namespace glfw
{
Asset::Asset()
{
}

std::string Asset::getAssetPath()
{
    return std::string(MODEL_PATH);
}

Result Asset::loadShader(std::string path, util::MemoryBuffer& buffer)
{
    std::string assetPath = std::string(SHADER_PATH);

    std::string filename = assetPath + "/" + path;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        return Result::Fail;
    }

    size_t fileSize = (size_t)file.tellg();
    buffer.resize(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    file.close();

    return Result::Continue;
}

Result loadImageSTB(std::string path, rhi::Format& format, rhi::Extent2D& extent, uint32_t& numLevels,
                    uint32_t& numLayers, std::vector<std::vector<size_t>>& offsets, util::MemoryBuffer& buffer)
{
    std::string assetPath = std::string(TEXTURE_PATH);

    std::string filename = assetPath + "/" + path;

    int texWidth, texHeight, texChannels;
    unsigned char* data = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!data)
    {
        throw std::runtime_error("failed to open image file!");
    }
    ASSERT(data);

    // TODO: set channel from Format
    extent.width = static_cast<uint32_t>(texWidth);
    extent.height = static_cast<uint32_t>(texHeight);

    numLevels = 1;
    offsets.push_back({0});

    size_t fileSize = (extent.width) * (extent.height) * STBI_rgb_alpha;

    buffer.resize(fileSize);
    memcpy(buffer.data(), data, fileSize);

    // TODO
    format = rhi::Format::R8G8B8A8_UNORM;

    return Result::Continue;
}

Result loadImageKTX(std::string path, rhi::Format& format, rhi::Extent2D& extent, uint32_t& numLevels,
                    uint32_t& numLayers, std::vector<std::vector<size_t>>& offsets, util::MemoryBuffer& buffer)
{
    std::string assetPath = std::string(TEXTURE_PATH);

    std::string filename = assetPath + "/" + path;

    ktxTexture* ktxTexture;
    ktxResult result =
        ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

    ASSERT(result == KTX_SUCCESS);
    ktx_size_t ktxSize = ktxTexture_GetSize(ktxTexture);
    ktx_uint8_t* ktxImage = ktxTexture_GetData(ktxTexture);

    extent.width = static_cast<uint32_t>(ktxTexture->baseWidth);
    extent.height = static_cast<uint32_t>(ktxTexture->baseHeight);
    numLevels = static_cast<uint32_t>(ktxTexture->numLevels);
    numLayers = static_cast<uint32_t>(ktxTexture->numFaces > ktxTexture->numLayers ? ktxTexture->numFaces
                                                                                   : ktxTexture->numLayers);

    buffer.resize(ktxSize);
    memcpy(buffer.data(), ktxImage, ktxSize);

    for (uint32_t layer = 0; layer < numLayers; layer++)
    {
        auto& layerOffsets = offsets.emplace_back();

        for (uint32_t level = 0; level < numLevels; level++)
        {
            ktx_size_t offset;
            KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, level, 0, layer, &offset);
            assert(result == KTX_SUCCESS);
            layerOffsets.push_back(offset);
        }
    }

    switch (ktxTexture->glInternalformat)
    {
    case GL_RGBA16F:
        format = rhi::Format::R16G16B16A16_FLOAT;
        break;
    case GL_R8:
        format = rhi::Format::R8_UNORM;
        break;
    default:
        format = rhi::Format::R8G8B8A8_UNORM;
        break;
    }

    ktxTexture_Destroy(ktxTexture);

    return Result::Continue;
}

Result Asset::loadImage(ImageLoader loader, std::string path, rhi::Format& format, rhi::Extent2D& extent,
                        uint32_t& numLevels, uint32_t& numLayers, std::vector<std::vector<size_t>>& offsets,
                        util::MemoryBuffer& buffer)
{
    switch (loader)
    {
    case ImageLoader::KTX:
        return loadImageKTX(path, format, extent, numLevels, numLayers, offsets, buffer);
    case ImageLoader::STB:
        return loadImageSTB(path, format, extent, numLevels, numLayers, offsets, buffer);
    default:
        UNREACHABLE();
        return Result::Fail;
    }
}
} // namespace glfw
} // namespace platform
