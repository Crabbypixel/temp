#include "world/ChunkMeshBuilder.h"
#include "world/World.h"
#include <glm/glm.hpp>

// UV rect per face
struct UVRect { glm::vec2 min, max; };

// Atlas constants
static constexpr float TW = 16.0f / 256.0f;   // 0.0625 — tile width
static constexpr float TH = 16.0f / 256.0f;   // 0.0625 — tile height
static constexpr float TV0 = 1.0f - TH;       // 0.9375 — v bottom of tile row
static constexpr float TV1 = 1.0f;            // v top

// Tile index -> UVRect
static UVRect Tile(int i)
{
    return { { i * TW, TV0 }, { (i + 1) * TW, TV1 } };
}

// Default grass tint
static const glm::vec3 GRASS_TINT = { 0.55f, 0.78f, 0.28f };
//static const glm::vec3 GRASS_TINT = { 0.72f, 0.74f, 0.30f };

// Tile indices
static constexpr int TEXTURE_DIRT = 0;
static constexpr int TEXTURE_GRASS_TOP = 1;
static constexpr int TEXTURE_GRASS_SIDE_OVERLAY = 2;
static constexpr int TEXTURE_STONE = 3;
static constexpr int TEXTURE_BEDROCK = 4;


// Face order: +Y -Y +X -X +Z -Z
static UVRect GetBlockFaceUV(BlockType type, int face)
{
    switch (type)
    {
    case BlockType::GRASS:
        if (face == 0) return Tile(TEXTURE_GRASS_TOP);    // +Y
        if (face == 1) return Tile(TEXTURE_DIRT);         // -Y
        return Tile(TEXTURE_DIRT);          // sides

    case BlockType::DIRT:
        return Tile(TEXTURE_DIRT);

    case BlockType::STONE:
        return Tile(TEXTURE_STONE);

    case BlockType::BEDROCK:
        return Tile(TEXTURE_BEDROCK);

    default:
        return Tile(TEXTURE_DIRT);
    }
}

// 6 faces: +Y -Y +X -X +Z -Z
// Each face = 4 verts -> 6 indices (2 tris) baked as 6 verts
static const glm::vec3 NORMALS[6] = {
    { 0, 1, 0}, { 0,-1, 0},
    { 1, 0, 0}, {-1, 0, 0},
    { 0, 0, 1}, { 0, 0,-1}
};

// Quad verts per face (local offsets from block origin)
static const glm::vec3 FACE_VERTS[6][4] = {
    // +Y top
    {{0,1,0},{1,1,0},{1,1,1},{0,1,1}},
    // -Y bottom
    {{0,0,1},{1,0,1},{1,0,0},{0,0,0}},

    // +X right
    {{1,0,0},{1,0,1},{1,1,1},{1,1,0}},
    // -X left
    {{0,0,1},{0,0,0},{0,1,0},{0,1,1}},

    // +Z front
    {{0,0,1},{1,0,1},{1,1,1},{0,1,1}},
    // -Z back
    {{1,0,0},{0,0,0},{0,1,0},{1,1,0}}
};

// Neighbor offsets per face: +Y -Y +X -X +Z -Z
static const glm::ivec3 DIRS[6] = {
    {0,1,0},{0,-1,0},{1,0,0},{-1,0,0},{0,0,1},{0,0,-1}
};

// Quad -> 2 tris (indices into 4-vert quad)
static const int TRI_IDX[6] = { 0,1,2, 0,2,3 };

void ChunkMeshBuilder::AddFace(std::vector<ChunkMesh::Vertex>& verts, const glm::vec3& pos, Face face, BlockType type)
{
    bool isGrassSide = (type == BlockType::GRASS && face > 1);  // Only sides

    const UVRect baseRect = isGrassSide ? Tile(TEXTURE_DIRT) : GetBlockFaceUV(type, face);
    const UVRect overlayRect = isGrassSide ? Tile(TEXTURE_GRASS_SIDE_OVERLAY) : Tile(0);

    // Map quad corners to atlas sub-region
    glm::vec2 baseUVs[4] = {
        {baseRect.min.x, baseRect.min.y},   // v0 bottom-left
        {baseRect.max.x, baseRect.min.y},   // v1 bottom-right
        {baseRect.max.x, baseRect.max.y},   // v2 top-right
        {baseRect.min.x, baseRect.max.y},   // v3 top-left
    };

    glm::vec2 overlayUVs[4] = {
        {overlayRect.min.x, overlayRect.min.y},   // v0 bottom-lefthu
        {overlayRect.max.x, overlayRect.min.y},   // v1 bottom-right
        {overlayRect.max.x, overlayRect.max.y},   // v2 top-right
        {overlayRect.min.x, overlayRect.max.y},   // v3 top-left
    };

    for (int i : TRI_IDX)
    {
        verts.emplace_back(ChunkMesh::Vertex{
            pos + FACE_VERTS[face][i],
            baseUVs[i],                      // <- atlas sub-region now
            overlayUVs[i],
            NORMALS[face],
            pos,
            (type == BlockType::GRASS && face != Face::BOTTOM) ? GRASS_TINT : glm::vec3(1.0f),
            isGrassSide ? 1.0f : 0.0f
            });
    }
}

void ChunkMeshBuilder::Build(const Chunk& chunk, const World& world, std::vector<ChunkMesh::Vertex>& outVertices)
{
    outVertices.clear();

    // Chunk world coordinates (not local world coordinates)
    int wx0 = chunk.chunkPos.x * CX;
    int wz0 = chunk.chunkPos.y * CZ;

    auto getChunkSafe = [&](int cx, int cz) -> const Chunk* {
        auto it = world.chunks.find({cx, cz});
        return (it != world.chunks.end()) ? it->second.get() : nullptr;
        };

    // Cache neighbouring chunks for faster querying
    const Chunk* nPX = getChunkSafe(chunk.chunkPos.x + 1, chunk.chunkPos.y);
    const Chunk* nNX = getChunkSafe(chunk.chunkPos.x - 1, chunk.chunkPos.y);
    const Chunk* nPZ = getChunkSafe(chunk.chunkPos.x, chunk.chunkPos.y + 1);
    const Chunk* nNZ = getChunkSafe(chunk.chunkPos.x, chunk.chunkPos.y - 1);

    // Iterate over every block
    for (int x = 0; x < CX; x++)
        for (int y = 0; y < CY; y++)
            for (int z = 0; z < CZ; z++)
            {
                BlockType blockType = chunk.Get(x, y, z);
                if (blockType == BlockType::AIR)        // If air, continue
                    continue;

                // Local world coordinates
                glm::vec3 worldPos = glm::vec3(wx0 + x, y, wz0 + z);

                //// Check all six faces
                for (int face = 0; face < 6; face++)
                {
                    int nx = x + DIRS[face].x;
                    int ny = y + DIRS[face].y;
                    int nz = z + DIRS[face].z;

                    bool isNeighborSolid = false;

                    if (Chunk::InBounds(nx, ny, nz))
                    {
                        // Neighbor is inside this chunk — safe direct access
                        isNeighborSolid = chunk.Get(nx, ny, nz) != BlockType::AIR;
                    }
                    else
                    {
                        // Out of chunk bounds — query neighbor chunk
                        if (nx < 0 && nNX) isNeighborSolid = nNX->Get(CX - 1, ny, nz) != BlockType::AIR;
                        else if (nx >= CX && nPX) isNeighborSolid = nPX->Get(0, ny, nz) != BlockType::AIR;
                        else if (nz < 0 && nNZ) isNeighborSolid = nNZ->Get(nx, ny, CZ - 1) != BlockType::AIR;
                        else if (nz >= CZ && nPZ) isNeighborSolid = nPZ->Get(nx, ny, 0) != BlockType::AIR;
                       
                        // null neighbor -> isNeighborSolid stays false -> face renders (correct — exposed to unloaded chunk)
                    }

                    if (!isNeighborSolid)
                        AddFace(outVertices, worldPos, (Face)face, blockType);
                }
            }
}