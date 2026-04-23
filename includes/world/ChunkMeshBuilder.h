#pragma once
#include "world/Chunk.h"
#include "rendering/ChunkMesh.h"

class World;   // forward declaration

class ChunkMeshBuilder
{
public:
    // Builds mesh for chunk, queries world for cross-chunk neighbors
    static void Build(
        const Chunk& chunk,
        const World& world,
        std::vector<ChunkMesh::Vertex>& outVertices
    );

private:
    static enum Face {
        TOP = 0,
        BOTTOM = 1,
        POS_X = 2,
        NEG_X = 3,
        POS_Z = 4,
        NEG_Z = 5
    };

    static void AddFace(
        std::vector<ChunkMesh::Vertex>& verts,
        const glm::vec3& pos,
        Face face,
        BlockType type
    );
};