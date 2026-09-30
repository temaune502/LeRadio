#include "fds.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
// #include "gui.h"


#define TRACKS_FOLDER "tracks"




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
    core->track.looping = 0;
    if(!IsMusicValid(core->track)) fds_log(FERROR, "Music stream invalid!");
    PlayMusicStream(core->track);
}

int main(void)
{
    Core core zeroe;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN); // |FLAG_MSAA_4X_HINT
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
            if (is_dir) continue;
            if (sv_ends_with(name, mp3)) sa_push(&core.tracks, name.data);
            else fds_log(FWARN, "Warning unsuported file type pleas convert to mp3: "SV_FMT, SV_ARGS(name));
        }
        fds_dir_iter_close(&it);
    }

    Camera2D camera zeroe;
    camera.zoom = 1;
    camera.target = (Vector2){.x = 0, .y = 0};

    // Automatic sorting all tracks by alphabet.
    sa_sort(&core.tracks, NULL);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(core.track);
        core.window_size.x = GetRenderWidth();
        core.window_size.y = GetRenderHeight();
        camera.offset = (Vector2){.x = 0, .y = 0};
        // camera.offset = (Vector2){.x = core.window_size.x/2, .y = core.window_size.y/2};
        camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
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
        if((int)GetMusicTimePlayed(core.track) >= (int)GetMusicTimeLength(core.track)) {next_song(&core); play_song(&core);}
        if (IsKeyPressed(KEY_N)) next_song(&core);
        if (IsKeyPressed(KEY_P)) prev_song(&core);
        if (IsKeyPressed(KEY_E)) {SeekMusicStream(core.track, GetMusicTimeLength(core.track)-2);}
        
        BeginDrawing();
        
        BeginMode2D(camera);

        //Drawing the bar
        float norm = GetMusicTimePlayed(core.track)/GetMusicTimeLength(core.track);
        DrawRectangle(12, 12, 200, 10, BLACK);
        DrawRectangle(10, 10, norm*200, 10, RED);

        for (int i = 0; i < (int)core.tracks.size; ++i)
         {
             SV t = sv_from_cstr(core.tracks.data[i]);
             sv_remove_suffix(&t, 4);
             DrawTextEx(
                 core.font,
                 temp_arena_sprintf(&core.frame_arena, "%d: " SV_FMT, i, SV_ARGS(t)),
                 (Vector2){.x = 20, .y = 20 + (30 * i)},
                 32, 1, (i == core.current_song ? GREEN : RAYWHITE));
         }
        ClearBackground(GetColor(0x120817));

        core.frame_arena.offset = 0;
        EndMode2D();
        EndDrawing();
    }
    UnloadFont(core.font);
    CloseWindow();
    CloseAudioDevice();
    sa_free(&core.tracks);
    return 0;
}