#include "platform/glfw/asset.h"
#include <fstream>

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
} // namespace glfw
} // namespace platform