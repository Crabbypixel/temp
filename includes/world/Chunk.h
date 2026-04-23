#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <iostream>

constexpr int CX = 16;
constexpr int CY = 256;
constexpr int CZ = 16;

enum class BlockType : uint8_t
{
	AIR = 0,
	BEDROCK = 1,
	STONE = 2,
	DIRT = 3,
	GRASS = 4
};

struct BlockInstance
{
	glm::vec3 pos;
	BlockType type;
};

class Chunk
{
public:
	Chunk() = default;

	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	Chunk(Chunk&&) noexcept = default;
	Chunk& operator=(Chunk&&) noexcept = default;

	BlockType blocks[CX][CY][CZ];
	glm::ivec2 chunkPos;
	bool dirty = true;		// needs mesh rebuild
	bool modified = false;  // has unsaved changes (never cleared except after SaveChunk)

	BlockType Get(int x, int y, int z) const;
	void Set(int x, int y, int z, BlockType type);

	// Range check
	static bool InBounds(int x, int y, int z);
};