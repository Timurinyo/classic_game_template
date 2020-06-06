#include <engine/pch.h>

#include <engine/assets.h>

namespace cgt
{

const char* GetGameRoot()
{
    static std::string root;
    if (root.empty())
    {
        root = SDL_GetBasePath();
        bool gameRootFound = false;
        const usize MAX_DEPTH = 10;
        for (usize i = 0; i < MAX_DEPTH; ++i)
        {
            std::string assetsPath = root + "/assets";
            if (std::filesystem::exists(assetsPath))
            {
                gameRootFound = true;
                break;
            }

            root += "/..";
        }

        if (!gameRootFound)
        {
            CGT_PANIC("Failed to find the game's root folder! Launched from: {}", SDL_GetBasePath());
        }

        // TODO: normalize path
    }

    return root.c_str();
}

std::string FormatPath(const char* path)
{
    // TODO: handle absolute paths or not?
    std::string formattedPath = fmt::format("{}/{}", GetGameRoot(), path);
    return formattedPath;
}

std::vector<u8> LoadFileInternal(const std::string formattedPath)
{
    // things are gonna be weird on esoteric platforms, so assert just in case
    static_assert(sizeof(char) == sizeof(u8));

    auto* fileRW = SDL_RWFromFile(formattedPath.c_str(), "rb");
    CGT_ASSERT_ALWAYS_MSG(fileRW != nullptr, "Failed to open a file at: {}\nSDL Error: {}", formattedPath.c_str(), SDL_GetError());

    usize fileSize = 0;
    void* fileData = SDL_LoadFile_RW(fileRW, &fileSize, true);
    CGT_ASSERT_ALWAYS_MSG(fileData != nullptr, "Failed to load a file at: {}\nSDL Error: {}", formattedPath.c_str(), SDL_GetError());

    std::vector<u8> data(fileSize);
    memcpy(data.data(), fileData, fileSize);
    SDL_free(fileData);

    return data;
}

std::vector<u8> LoadFile(const char* path)
{
    const std::string formattedPath = FormatPath(path);
    return LoadFileInternal(formattedPath);
}

tmx_map* LoadTiledMap(const char* path)
{
    const std::string formattedPath = FormatPath(path);
    tmx_map* map = tmx_load(formattedPath.c_str());
    CGT_ASSERT_ALWAYS_MSG(map != nullptr, "Failed to load a tiled map at: {}\nTMX error: {}", formattedPath.c_str(), tmx_strerr());

    return map;
}

Mix_Music* LoadMusic(const char* path)
{
    const std::string formattedPath = FormatPath(path);
    Mix_Music* music = Mix_LoadMUS(formattedPath.c_str());
    CGT_ASSERT_ALWAYS_MSG(music != nullptr, "Failed to load a music file at: {}\nSDL error: {}", formattedPath.c_str(), SDL_GetError());

    return music;
}

Mix_Chunk* LoadWav(const char* path)
{
    const std::string formattedPath = FormatPath(path);
    Mix_Chunk* chunk = Mix_LoadWAV(formattedPath.c_str());
    CGT_ASSERT_ALWAYS_MSG(chunk != nullptr, "Failed to load a WAV file at: {}\nSDL error: {}", formattedPath.c_str(), SDL_GetError());

    return chunk;
}

}
