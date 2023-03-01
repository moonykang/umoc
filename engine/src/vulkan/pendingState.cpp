#include "pendingState.h"
#include "context.h"
#include "core.h"

namespace vk
{
PendingState* Context::getPendingState()
{
    return pendingState;
}

PendingState::PendingState() : pipeline(nullptr)
{
}

Pipeline* PendingState::getPipeline()
{
    ASSERT(pipeline);
    return pipeline;
}

void PendingState::setPipeline(Pipeline* pipeline)
{
    this->pipeline = pipeline;
}
} // namespace vk