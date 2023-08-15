#pragma once

#include "util.h"
#include <mutex>

namespace util
{
class Transform
{
  public:
    Transform() : transform(1.0f), position(0.f), vRotate(0.f), vScale(1.f), dirty(false)
    {
    }

    Transform(glm::mat4 transform) : transform(transform), position(0.f), vRotate(0.f), vScale(1.f), dirty(false)
    {
    }

    ~Transform()
    {
    }

    void translate(glm::vec3 v)
    {
        std::lock_guard<std::mutex> lock(mutex);
        position += v;
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

            transform = glm::scale(rotM * glm::translate(glm::mat4(1.0f), position), vScale);
        }

        return transform;
    }

    size_t size()
    {
        return transformSize;
    }

    glm::vec3& getPosition()
    {
        return position;
    }

    glm::vec3& getRotation()
    {
        return vRotate;
    }
    glm::vec3& getScale()
    {
        return vScale;
    }

    void updateExternal()
    {
        dirty = true;
    }

    void debug()
    {
        LOGD("Position %f %f %f", position.x, position.y, position.z);
        LOGD("Rotate %f %f %f", vRotate.x, vRotate.y, vRotate.z);
        LOGD("Scale %f %f %f", vScale.x, vScale.y, vScale.z);

        LOGD("Transform Matrix");
        LOGD("%f %f %f %f", transform[0][0], transform[0][1], transform[0][2], transform[0][3]);
        LOGD("%f %f %f %f", transform[1][0], transform[1][1], transform[1][2], transform[1][3]);
        LOGD("%f %f %f %f", transform[2][0], transform[2][1], transform[2][2], transform[2][3]);
        LOGD("%f %f %f %f", transform[3][0], transform[3][1], transform[3][2], transform[3][3]);
    }

  public:
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 vRotate;
    glm::vec3 vScale;

    bool dirty;
    std::mutex mutex;

    static constexpr size_t transformSize = sizeof(glm::mat4);
};
} // namespace util