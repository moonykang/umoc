#pragma once

namespace vk
{
class Pipeline;
class PendingState
{
  public:
    PendingState();

    ~PendingState() = default;

    Pipeline* getPipeline();

    void setPipeline(Pipeline* pipeline);

  private:
    Pipeline* pipeline;
};
} // namespace vk