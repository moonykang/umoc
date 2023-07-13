#pragma once

#include "util.h"
#include <mutex>

namespace util
{
class Transform
{
  public:
    Transform() : transform(1.0f), vPosition(0.f), vRotate(0.f), vScale(1.f), dirty(false)
    {
    }

    Transform(glm::mat4 transform) : transform(transform), vPosition(0.f), vRotate(0.f), vScale(1.f), dirty(false)
    {
    }

    ~Transform()
    {
    }

    void translate(glm::vec3 v)
    {
        std::lock_guard<std::mutex> lock(mutex);
        vPosition += v;
        dirty = true;
    }

    void rotate(glm::vec3 v)
    {
        std::lock_guard<std::mutex> lock(mutex);
        vRotate += v;
        dirty = true;
    }

    void scale(glm::vec3 v)
    {
        std::lock_guard<std::mutex> lock(mutex);
        vScale *= v;
        dirty = true;
    }

    glm::mat4& get()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (dirty)
        {
            glm::mat4 rotM = glm::mat4(1.0f);
            rotM = glm::rotate(rotM, glm::radians(vRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
            rotM = glm::rotate(rotM, glm::radians(vRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotM = glm::rotate(rotM, glm::radians(vRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));

            transform = glm::scale(rotM * glm::translate(glm::mat4(1.0f), vPosition), vScale);
        }

        return transform;
    }

    size_t size()
    {
        return transformSize;
    }

  private:
    glm::mat4 transform;
    glm::vec3 vPosition;
    glm::vec3 vRotate;
    glm::vec3 vScale;

    bool dirty;
    std::mutex mutex;

    static constexpr size_t transformSize = sizeof(glm::mat4);
};
} // namespace util