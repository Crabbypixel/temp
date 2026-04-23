#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "world/World.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb/stb_perlin.h"

World::World()
{
    m_saveWorker = std::thread(&World::SaveWorker, this);
}

// ───── Coord helpers ─────────────────────────────────────────────────
// Get chunk coord from world coords
glm::ivec2 World::ChunkCoord(int worldX, int worldZ)
{
    return {
        (int)std::floor(worldX / (float)CX),
        (int)std::floor(worldZ / (float)CZ)
    };
}

// Get local chunk coord from world coords
glm::ivec3 World::ChunkLocalCoord(int worldX, int worldY, int worldZ)
{
    return {
        (worldX % CX + CX) % CX,
        worldY,
        (worldZ % CZ + CZ) % CZ
    };
}

// ───── Chunk access ──────────────────────────────────────────────────
Chunk* World::GetChunk(int worldX, int worldZ)
{
    auto it = chunks.find(ChunkCoord(worldX, worldZ));
    return (it != chunks.end()) ? it->second.get() : nullptr;
}

const Chunk* World::GetChunk(int worldX, int worldZ) const
{
    auto it = chunks.find(ChunkCoord(worldX, worldZ));
    return (it != chunks.end()) ? it->second.get() : nullptr;
}

// ───── Block access ──────────────────────────────────────────────────
bool World::IsSolid(int worldX, int worldY, int worldZ) const
{
    return GetBlock(worldX, worldY, worldZ) != BlockType::AIR;
}

BlockType World::GetBlock(int worldX, int worldY, int worldZ) const
{
    if (worldY < 0 || worldY >= CY)
        return BlockType::AIR;

    const Chunk* chunk = GetChunk(worldX, worldZ);
    if (!chunk) 
        return BlockType::AIR;
    
    auto l = ChunkLocalCoord(worldX, worldY, worldZ);
    return chunk->Get(l.x, l.y, l.z);
}

void World::SetBlock(int worldX, int worldY, int worldZ, BlockType type)
{
    if (worldY < 0 || worldY >= CY)
        return;
    
    Chunk* chunk = GetChunk(worldX, worldZ);
    if (!chunk) 
        return;

    auto l = ChunkLocalCoord(worldX, worldY, worldZ);
    chunk->Set(l.x, l.y, l.z, type);
}

// ───── World generation ────────────────────────────────────────────────────
float World::GetTerrainHeight(int wx, int wz)
{
    const float MIN_H = 20.0f;
    const float MAX_H = 120.0f;

    float fx = wx * 0.01f;
    float fz = wz * 0.01f;

    float n = stb_perlin_noise3(fx, 0.0f, fz, 0, 0, 0) * 0.5f + 0.5f;
    n += (stb_perlin_noise3(fx * 2.0f, 0.0f, fz * 2.0f, 0, 0, 0) * 0.5f + 0.5f) * 0.50f;
    n += (stb_perlin_noise3(fx * 4.0f, 0.0f, fz * 4.0f, 0, 0, 0) * 0.5f + 0.5f) * 0.25f;
    n /= 1.75f;
    n = pow(n, 2.0f);

    return MIN_H + n * (MAX_H - MIN_H);
}

// Invoked by worker
void World::FillChunkData(Chunk& chunk, glm::ivec2 coord)
{
    // Try disk first
    if (LoadChunkFromDisk(chunk, coord))
    {
        chunk.modified = false;
        return;
    }

    // Fresh perlin gen
    chunk.chunkPos = coord;
    int cx = coord.x;
    int cz = coord.y;

    for (int x = 0; x < CX; x++)
    {
        for (int z = 0; z < CZ; z++)
        {
            int worldX = cx * CX + x;
            int worldZ = cz * CZ + z;
            int height = (int)GetTerrainHeight(worldX, worldZ);

            float n = (height - 20.0f) / (100.0f - 20.0f);
            int   thickness = 4 + (int)((1.0f - n) * 10.0f);
            int   base = std::max(1, height - thickness);

            chunk.blocks[x][0][z] = BlockType::BEDROCK;
            for (int y = 1; y < base; y++) chunk.blocks[x][y][z] = BlockType::STONE;
            for (int y = base; y < height; y++) chunk.blocks[x][y][z] = BlockType::DIRT;
            chunk.blocks[x][height][z] = BlockType::GRASS;
        }
    }

    chunk.dirty = true;
    chunk.modified = false;
}

// ───── File IO ──────────────────────────────────────────────────
std::string World::ChunkFilePath(glm::ivec2 coord)
{
    return "saves/" + std::to_string(coord.x) + "_" + std::to_string(coord.y) + ".bin";
}

void World::SaveChunk(const Chunk& chunk)
{
    std::filesystem::create_directories("saves");
    auto path = ChunkFilePath(chunk.chunkPos);

    FILE* f = nullptr;
    fopen_s(&f, path.c_str(), "wb");

    if (!f)
        return;

    fwrite(chunk.blocks, sizeof(chunk.blocks), 1, f);
    fclose(f);
}

bool World::LoadChunkFromDisk(Chunk& chunk, glm::ivec2& coord)
{
    auto path = ChunkFilePath(coord);

    FILE* f = nullptr;
    fopen_s(&f, path.c_str(), "rb");
    if (!f)
        return false;

    fread(chunk.blocks, sizeof(chunk.blocks), 1, f);
    fclose(f);

    chunk.chunkPos = coord;
    chunk.dirty = true;

    return true;
}

void World::UnloadChunks()
{
    // Save modified chunks
    for (auto& [coord, chunk] : chunks)
    {
        if (chunk->modified)
        {
            SaveChunk(*chunk);
            chunk->modified = false;
        }
    }

    // Unload chunks from memory
    for (auto& [coord, chunk] : chunks)
    {
        m_chunkMeshes[coord].Destroy();
        m_chunkMeshes.erase(coord);
    }

    chunks.clear();
    m_chunkMeshes.clear();

    std::cout << "World saved and unloaded.\n";

}

// ───── Raycast CRUD ──────────────────────────────────────────────────
bool World::PlaceBlock(const RaycastHit& hit, BlockType type)
{
    glm::ivec3 target = hit.blockPos + hit.normal;

    if (IsSolid(target.x, target.y, target.z))
        return false;

    SetBlock(target.x, target.y, target.z, type);

    // Mark this chunk as dirty (as we updated the chunk)
    Chunk* c = GetChunk(target.x, target.z);
    if (c)
        c->dirty = true;

    MarkNeighbourChunksDirty(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z);

    return true;
}

bool World::BreakBlock(const RaycastHit& hit)
{
    if (!hit.hit)
        return false;

    SetBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, BlockType::AIR);

    // Mark this chunk as dirty (as we updated the chunk)
    Chunk* c = GetChunk(hit.blockPos.x, hit.blockPos.z);
    if (c) 
        c->dirty = true;

    MarkNeighbourChunksDirty(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z);

    return true;
}

void World::MarkNeighbourChunksDirty(int wx, int wy, int wz)
{
    glm::vec3 localPos = ChunkLocalCoord(wx, wy, wz);
    int lx = localPos.x;
    int lz = localPos.z;

    if (lx == 0) { Chunk* c = GetChunk(wx - 1, wz); if (c) c->dirty = true; }
    if (lx == CX - 1) { Chunk* c = GetChunk(wx + 1, wz); if (c) c->dirty = true; }
    if (lz == 0) { Chunk* c = GetChunk(wx, wz - 1); if (c) c->dirty = true; }
    if (lz == CZ - 1) { Chunk* c = GetChunk(wx, wz + 1); if (c) c->dirty = true; }
}

// ───── Sync ──────────────────────────────────────────────────────────
void World::SyncRenderer()
{
    // Iterate over every chunk
    for (auto& [chunkPos, chunk] : chunks)
    {
        if (!chunk->dirty)       // If chunk is not dirty (unchanged), no need to sync
            continue;
            
        // Build mesh geometry on CPU   
        std::vector<ChunkMesh::Vertex> verts;
        ChunkMeshBuilder::Build(*chunk, *this, verts);          // Caution! - This operation involves reading chunk data

        // Upload to GPU
        m_chunkMeshes[chunkPos].Build(verts);

        chunk->dirty = false;
    }
}

// ───── Textures & Draw ───────────────────────────────────────────────
void World::SetChunkShader(Shader& shader)
{
    m_chunkShader = &shader;
    shader.use();
    shader.setInt("u_atlas", 2);   // single sampler, slot 2
}

void World::LoadAtlasTexture(const char* path)
{
    stbi_set_flip_vertically_on_load(true);   // GL origin = bottom-left

    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 0);
    if (!data) {
        std::cout << "Atlas load failed: " << path << '\n';
        return;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Nearest-neighbour — keeps pixel art crisp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

    m_atlasTexture = id;
}

void World::DrawAll(const glm::mat4& proj, const glm::mat4& view)
{
    if (!m_chunkShader)
        return;

    m_chunkShader->use();

    // Bind textures once — shared across all chunk draw calls
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_atlasTexture);

    // Extract frustum planes
    m_frustum.Extract(proj * view);

    // Draw all meshes (computed earlier)
    for (auto& [chunkPos, mesh] : m_chunkMeshes)
    {
        glm::vec3 minP = { chunkPos.x * CX,    0,  chunkPos.y * CZ };
        glm::vec3 maxP = { chunkPos.x * CX + CX, CY, chunkPos.y * CZ + CZ };

        // Implement frustum culling
        if (!m_frustum.ContainsAABB(minP, maxP)) continue;

        mesh.Draw();
    }
}

// ───── Frustum Culling ───────────────────────────────────────────────
void Frustum::Extract(const glm::mat4& vp)
{
    planes[0] = glm::vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]); // left
    planes[1] = glm::vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]); // right
    planes[2] = glm::vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]); // bottom
    planes[3] = glm::vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]); // top
    planes[4] = glm::vec4(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]); // near
    planes[5] = glm::vec4(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]); // far

    for (auto& p : planes) p /= glm::length(glm::vec3(p));
}

bool Frustum::ContainsAABB(const glm::vec3& minP, const glm::vec3& maxP) const
{
    for (const auto& plane : planes)
    {
        glm::vec3 pv = {
            plane.x >= 0 ? maxP.x : minP.x,
            plane.y >= 0 ? maxP.y : minP.y,
            plane.z >= 0 ? maxP.z : minP.z
        };
        if (glm::dot(glm::vec3(plane), pv) + plane.w < 0.0f)
            return false;
    }
    return true;
}

// ───── Infinite World ───────────────────────────────────────────────
void World::UpdateStreaming(const glm::vec3& playerPos)
{
    glm::ivec2 playerChunkCoord = ChunkCoord((int)playerPos.x, (int)playerPos.z);

    if (playerChunkCoord == m_lastPlayerChunk)
    {
        bool hasAllChunksLoaded = true;
        std::lock_guard<std::mutex> lk(m_queuedMutex);
        {
            for (int dx = -m_viewDist; dx <= m_viewDist && hasAllChunksLoaded; dx++)
            {
                for (int dz = -m_viewDist; dz <= m_viewDist && hasAllChunksLoaded; dz++)
                {
                    if (dx * dx + dz * dz > m_viewDist * m_viewDist)
                        continue;

                    glm::ivec2 c = { playerChunkCoord.x + dx, playerChunkCoord.y + dz };

                    if (!chunks.count(c) && !m_queued.count(c))
                        hasAllChunksLoaded = false;
                }
            }
            if (hasAllChunksLoaded) return;
        }
    }
    else
    {
        m_lastPlayerChunk = playerChunkCoord;
    }

    // Make list of chunks to unload
    std::vector<glm::ivec2> chunksToUnload;
    for (auto& [chunkCoord, _] : chunks)
    {
        glm::ivec2 d = chunkCoord - playerChunkCoord;

        if (d.x * d.x + d.y * d.y > m_unloadDist * m_unloadDist)
            chunksToUnload.push_back(chunkCoord);
    }

    // Actually unload
    for (auto& chunkCoord : chunksToUnload)
    {
        auto it = chunks.find(chunkCoord);
        if (it != chunks.end())
        {
            std::lock_guard<std::mutex> lk(m_saveMutex);
            if (it->second->modified)
                m_saveQueue.push(std::move(it->second));
            m_saveCV.notify_all();
        }

        m_chunkMeshes[chunkCoord].Destroy();
        m_chunkMeshes.erase(chunkCoord);
        chunks.erase(chunkCoord);
    }

    // Make list of chunks to load
    std::vector<glm::ivec2> chunksToLoad;
    for (int dx = -m_viewDist; dx <= m_viewDist; dx++)
    {
        for (int dz = -m_viewDist; dz <= m_viewDist; dz++)
        {
            if (dx * dx + dz * dz > m_viewDist * m_viewDist)
                continue;

            glm::ivec2 chunkCoord = { playerChunkCoord.x + dx, playerChunkCoord.y + dz };

            // If chunk coord isn't present, load it
            if (!chunks.count(chunkCoord))
                chunksToLoad.push_back(chunkCoord);
        }
    }

    // Sort the list from nearest to load first then at the end
    std::sort(chunksToLoad.begin(), chunksToLoad.end(), [&](const glm::ivec2& a, const glm::ivec2& b) {
        glm::ivec2 da = a - playerChunkCoord, db = b - playerChunkCoord;
        return da.x * da.x + da.y * da.y < db.x * db.x + db.y * db.y;
        });

    // Actually load
    {
        std::lock_guard<std::mutex> lkQ(m_genQueueMutex);
        std::lock_guard<std::mutex> lkS(m_queuedMutex);

        int budget = 1;

        for (auto& chunkCoord : chunksToLoad)
        {
            if (m_queued.count(chunkCoord))
                continue;

            m_genQueue.push(chunkCoord);
            m_queued.insert(chunkCoord);

            //if (budget > 0)
            //{
            //    budget--;
            //    break;
            //}
            
            // Mark neighboring chunks dirty because faces at chunk borders depend on adjacent chunk data.
            // When a chunk changes, neighbors may need to rebuild meshes for correct face culling.
            auto it = chunks.find(chunkCoord + glm::ivec2{ 1,0 }); if (it != chunks.end()) it->second->dirty = true;
            it = chunks.find(chunkCoord + glm::ivec2{ 0,1 }); if (it != chunks.end()) it->second->dirty = true;
            it = chunks.find(chunkCoord + glm::ivec2{ -1,0 }); if (it != chunks.end()) it->second->dirty = true;
            it = chunks.find(chunkCoord + glm::ivec2{ 0,-1 }); if (it != chunks.end()) it->second->dirty = true;
        }
    }
    m_genQueueCV.notify_all();
}

// ───── Multithreading ───────────────────────────────────────────────
void World::StartWorkers(int count)
{
    m_shutdown = false;
    for (int i = 0; i < count; i++)
        m_workers.emplace_back([this] { WorkerLoop(); });
}

void World::StopWorkers()
{
    m_shutdown = true;

    m_genQueueCV.notify_all();   // wake all sleeping workers so they exit
    m_saveCV.notify_all();

    // Join the save worker
    if (m_saveWorker.joinable())
    {
        m_saveWorker.join();
        std::cout << "Save worker joined\n";
    }

    for (auto& t : m_workers)
    {
        if (t.joinable())
            t.join();
    }

    m_workers.clear();
}

void World::WorkerLoop()
{
    while (true)
    {
        glm::ivec2 coord;
        {
            std::unique_lock<std::mutex> lk(m_genQueueMutex);
            m_genQueueCV.wait(lk, [&] {         // Wakeup when queue is NOT empty or shutdown triggered
                return !m_genQueue.empty() || m_shutdown;
                });

            // Critical section
            if (m_shutdown && m_genQueue.empty())
                break;

            coord = m_genQueue.front();
            m_genQueue.pop();
        }

        // Fill into worker-local chunk
        auto chunk = std::make_unique<Chunk>();
        chunk->chunkPos = coord;

        FillChunkData(*chunk, coord);        // No need to lock this, no shared resource used in this function
        
        // Move the chunk to staged section and remove the coord from queue
        {
            std::lock_guard<std::mutex> lk(m_stagingMutex);
            m_staging[coord] = std::move(chunk);        // Move semantics, O(1) operation
        }
        {
            std::lock_guard<std::mutex> lk(m_queuedMutex);
            m_queued.erase(coord);
        }
    }
}

void World::SaveWorker()
{
    while (true)
    {
        std::unique_ptr<Chunk> chunk;

        {
            std::unique_lock<std::mutex> lk(m_saveMutex);
            m_saveCV.wait(lk, [&] {
                return !m_saveQueue.empty() || m_shutdown;
                });

            if (m_shutdown && m_saveQueue.empty())
                break;

            chunk = std::move(m_saveQueue.front());
            m_saveQueue.pop();
        }

        SaveChunk(*chunk);
    }
}

void World::UploadReady()
{
    // Swap chunks present in staging queue 
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash> queued;
    {
        std::lock_guard<std::mutex> lk(m_stagingMutex);
        queued.swap(m_staging);
    }

    // If nothing, return
    if (queued.empty())
        return;

    // Move chunks from queued to chunks
    for (auto& [coord, chunkPtr] : queued)
    {
        chunks[coord] = std::move(chunkPtr);        // Move semantics
        m_chunkMeshes[coord];
        chunks[coord]->dirty = true;
    }
}