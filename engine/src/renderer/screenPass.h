#pragma once

#include "base.h"

namespace renderer
{
class ScreenPass : public Base
{
public:
	Result render(platform::Context* context) override;
};
} // namespace renderer