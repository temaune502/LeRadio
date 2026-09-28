#include "fds.h"
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "gui.h"

// #include <winnls.h>
// #include <stringapiset.h>
// #include <fileapi.h>

#define TRACKS_FOLDER "tracks"

// wchar_t *fds_utf8_to_wide(const char *str)
// {
//     if (!str)
//         return NULL;
//     int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
//     if (len <= 0) return NULL;
//     wchar_t *out = malloc((size_t)len * sizeof(wchar_t));
//     if (!out) return NULL;
//     if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, out, len))
//     {
//         free(out);
//         return NULL;
//     }
//     return out;
// }





typedef struct 
{
    StringArray tracks;
    Music track;
    Font font;
    Vector2 window_size;
    FixedArena frame_arena;
    int current_song;
    float volume;
    float trackProgress; 
    bool shuffle;
} Core;



void next_song(Core* core)
{
    core->current_song++;
    if ((size_t)core->current_song >= core->tracks.size)  core->current_song = 0;
}

void prev_song(Core* core)
{
    core->current_song--;
    if (core->current_song < 0)
    {
        core->current_song = (int)core->tracks.size - 1;
    }
}
void play_song(Core* core)
{
    static int track_local;
    
    if (core->current_song != track_local)
    {        
        UnloadMusicStream(core->track);
        core->track = LoadMusicStream(temp_arena_sprintf(temp_arena_get(), "%s/%s", TRACKS_FOLDER, core->tracks.data[core->current_song]));
        track_local = core->current_song;
    }
    PlayMusicStream(core->track);
}

int main(void)
{
    Core core zeroe;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // |FLAG_MSAA_4X_HINT
    sa_new(&core.tracks, 64);
    InitWindow(800, 600, "LeRadio");
    InitAudioDevice();
    SetTargetFPS(60);


    core.volume = 0.5f;
    core.trackProgress = 45.0f; 
    core.shuffle = false;
    core.frame_arena = fixed_arena_create(64*KB);

    core.font = LoadFontEx("resources/fonts/segoeui.ttf", 32, 0, 255);

    FdsDirIter it;
    if (fds_dir_iter_open(TRACKS_FOLDER, &it) == 0)
    {
        SV name;
        SV mp3 = sv_from_parts("mp3", 3);
        int is_dir;
        while (fds_dir_iter_next(&it, &name, &is_dir) == 0)
        {
            if (is_dir)
                continue;
            if (sv_ends_with(name, mp3)) sa_push(&core.tracks, name.data);
            else fds_log(FWARN, "Warning unsuported file type pleas convert to mp3: "SV_FMT, SV_ARGS(name));
        }
        fds_dir_iter_close(&it);
    }
    // Automatic sorting all tracks by alphabet.
    sa_sort(&core.tracks, NULL);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(core.track);
        core.window_size.x = GetRenderWidth();
        core.window_size.y = GetRenderHeight();
        if (IsKeyPressed(KEY_Q))
            play_song(&core);
        if (IsKeyPressed(KEY_W))
        {
            if(!IsMusicStreamPlaying(core.track))
            {
                ResumeMusicStream(core.track);
            }
            else
            {
                PauseMusicStream(core.track);
            }
        }
        if (IsKeyPressed(KEY_N))
            next_song(&core);
        if (IsKeyPressed(KEY_P))
            prev_song(&core);
        BeginDrawing();
        
        DrawRectangleRect();

        for (int i = 0; i < (int)core.tracks.size; ++i)
         {
             SV t = sv_from_cstr(core.tracks.data[i]);
             sv_remove_suffix(&t, 4);
             DrawTextEx(
                 core.font,
                 temp_arena_sprintf(&core.frame_arena, "%d: " SV_FMT, i, SV_ARGS(t)),
                 (Vector2){.x = 20, .y = 20 + (30 * i)},
                 32, 1, (i == core.current_song ? GREEN : RED));
         }
        ClearBackground(GetColor(0x262626));

        core.frame_arena.offset = 0;
        EndDrawing();
    }
    UnloadFont(core.font);
    CloseWindow();
    CloseAudioDevice();
    sa_free(&core.tracks);
    return 0;
}