#pragma once
#include "common/hash.h"
#include "common/util.h"
#include "resources.h"
#include <mutex>
#include <unordered_map>

namespace rhi
{
class Context;

class ResourceMap
{
  public:
    Result insert(ResourceID key, Resource* resource)
    {
        if (auto it = resources.find(key); it != resources.end())
        {
            it->second.second++;
        }

        return Result::Continue;
    }

    std::pair<Result, Resource*> release(ResourceID key)
    {
        if (auto it = resources.find(key); it != resources.end())
        {
            Resource* resource = it->second.first;
            it->second.second--;

            if (it->second.second <= 0)
            {
                return {Result::Continue, resource};
            }
            else
            {
                return {Result::Fail, resource};
            }
        }
        else
        {
            return {Result::Fail, nullptr};
        }
    }

    Resource* get(ResourceID key)
    {
        if (auto it = resources.find(key); it != resources.end())
        {
            return it->second.first;
        }
        return nullptr;
    }

  private:
    std::unordered_map<ResourceID, std::pair<Resource*, uint32_t>> resources;
    std::mutex mutex;
};
} // namespace rhi