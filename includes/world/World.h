#pragma once

#include <glm/glm.hpp>

#include "world/Chunk.h"
#include "rendering/ChunkMesh.h"
#include "world/ChunkMeshBuilder.h"
#include "Raycast.h"

#include <cmath>
#include <array>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <shared_mutex>
#include <condition_variable>


struct IVec2Hash
{
    size_t operator()(const glm::ivec2& v) const
    {
        size_t h1 = std::hash<int>()(v.x);
        size_t h2 = std::hash<int>()(v.y);
        return h1 ^ (h2 * 2654435761u);
    }
};

struct Frustum
{
    std::array<glm::vec4, 6> planes;

    void Extract(const glm::mat4& vp);
    bool ContainsAABB(const glm::vec3& min, const glm::vec3& max) const;
};

class World
{
public:
    // Core data
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash> chunks;

    // Constructor
    World();

    // Block access
    BlockType GetBlock(int worldX, int worldY, int worldZ) const;
    void SetBlock(int worldX, int worldY, int worldZ, BlockType type);
    bool IsSolid(int worldX, int worldY, int worldZ) const;

    // Player interaction
    bool PlaceBlock(const RaycastHit& hit, BlockType type);
    bool BreakBlock(const RaycastHit& hit);

    // Rendering
    void SyncRenderer();
    void SetChunkShader(Shader& shader);
    void LoadAtlasTexture(const char* path);
    void DrawAll(const glm::mat4& proj, const glm::mat4& view);

    // Streaming
    void UpdateStreaming(const glm::vec3& playerPos);

    // Save chunks
    void UnloadChunks();

    // Helpers
    static glm::ivec2 ChunkCoord(int worldX, int worldZ);
    static glm::ivec3 ChunkLocalCoord(int worldX, int worldY, int worldZ);

    // Workers
    void StartWorkers(int count);
    void StopWorkers();
    void UploadReady();  // called on each frame, main thread only

private:
    // Internal chunk access
    Chunk* GetChunk(int worldX, int worldZ);
    const Chunk* GetChunk(int worldX, int worldZ) const;

    // Rendering data
    unsigned int m_atlasTexture = 0;
    Frustum m_frustum;
    std::unordered_map<glm::ivec2, ChunkMesh, IVec2Hash> m_chunkMeshes;
    Shader* m_chunkShader = nullptr;

    // Chunk updates
    void MarkNeighbourChunksDirty(int wx, int wy, int wz);

    // Returns height at location using Perlin noise
    static float GetTerrainHeight(int wx, int wz);

    // Streaming / disk
    static std::string ChunkFilePath(glm::ivec2 coord);
    static void SaveChunk(const Chunk& chunk);
    static bool LoadChunkFromDisk(Chunk& chunk, glm::ivec2& coord);

    // Streaming state
    int m_viewDist = 10;
    int m_unloadDist = 10;
    glm::ivec2 m_lastPlayerChunk = { INT_MAX, INT_MAX };

    // Multithreading
    std::vector<std::thread> m_workers;
    std::atomic<bool> m_shutdown{ false };

    // Job queue - main thread pushes coords to load, workers pop
    std::queue<glm::ivec2> m_genQueue;
    std::mutex m_genQueueMutex;
    std::condition_variable m_genQueueCV;

    // Staging - workers push, main promotes
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash> m_staging;
    std::mutex m_stagingMutex;

    // In-flight set - prevents duplicate queuing
    std::unordered_set<glm::ivec2, IVec2Hash> m_queued;
    std::mutex m_queuedMutex;

    // Save IO threading
    std::queue<std::unique_ptr<Chunk>> m_saveQueue;
    std::mutex m_saveMutex;
    std::condition_variable m_saveCV;
    std::thread m_saveWorker;
    void SaveWorker();

    //// Mesh threading - to be done later
    //std::unordered_map<glm::ivec2, std::vector<ChunkMesh::Vertex>, IVec2Hash> m_meshStaging;
    //std::mutex m_meshStagingMutex;

    //// Mesh job queue
    //std::queue<glm::ivec2> m_meshQueue;
    //std::mutex m_meshQueueMutex;
    //std::condition_variable m_meshQueueCV;

    //// In-flight mesh set - main thread must NOT unload these
    //std::unordered_set<glm::ivec2, IVec2Hash> m_meshQueued;
    //std::mutex m_meshQueuedMutex;

    //// Mesh workers
    //std::vector<std::thread> m_meshWorkers;

    //void MeshWorkerLoop();
    //void StartMeshWorkers(int count);
    //void StopMeshWorkers();

    // Pure CPU task - worker-safe, no data races
    void FillChunkData(Chunk& chunk, glm::ivec2 coord);
    void WorkerLoop();
};