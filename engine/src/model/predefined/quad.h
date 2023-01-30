#pragma once

#include "../object.h"
#include "common/util.h"

namespace rhi
{
class Quad
{
	uint32_t indexStart = 0;
	uint32_t vertexStart = 0;

	const uint32_t slices = 2;
	const uint32_t stacks = 2;

	const uint32_t top_left = 0;
	const uint32_t top_right = 1;
	const uint32_t bottom_left = 2;
	const uint32_t bottom_right = 3;

	static const uint32_t num_angles = 4;
	const glm::vec3 positions[num_angles] =
	{
		{ -1.0f, -1.0f, 0.0f }, {  1.0f, -1.0f, 0.0f }, { -1.0f,  1.0f, 0.0f }, {  1.0f, 1.0f, 0.0f }
	};

	const glm::vec3 normals[num_angles] =
	{
		{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }
	};

	const glm::vec2 uvs[num_angles] =
	{
		{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }
	};

	const glm::vec3 colors[num_angles] =
	{
		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f }
	};

	for (uint32_t i = 0; i < num_angles; i++)
	{
		rhi::VertexData vertex;
		vertex.pos = positions[i];
		vertex.uv = uvs[i];
		vertexBuffer->append(vertex);
	}
}