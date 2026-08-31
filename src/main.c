#include "fds.h"
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define TRACKS_FOLDER "tracks"

StringArray tracks;
int current_song = 0;
Sound track;

void next_song()
{
    current_song++;
    if((size_t)current_song >= tracks.size) current_song = 0;
    fds_log(FINFO, "Current track: %d", current_song);
}
void prev_song()
{
    current_song--;
    if(current_song >= (int)tracks.size) current_song = tracks.size-1;
    if(current_song < 0) current_song = 0;
    fds_log(FINFO, "Current track: %d", current_song);
}
void play_song()
{
    static int track_local;
    if(current_song != track_local)
    {
        UnloadSound(track);
        track = LoadSound(temp_arena_sprintf(temp_arena_get(), "%s/%s", TRACKS_FOLDER, tracks.data[current_song]));
        track_local = current_song;
    }
    PlaySound(track);
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    sa_new(&tracks, 64);
    InitWindow(800,600, "LeRadio");
    InitAudioDevice();
    SetTargetFPS(60);

    Font font = LoadFont("resources/fonts/segoeui.ttf");

    FdsDirIter it;
    if(fds_dir_iter_open(TRACKS_FOLDER, &it) == 0)
    {
        SV name;
        SV mp3 = sv_from_cstr("mp3");
        int is_dir;
        while(fds_dir_iter_next(&it, &name, &is_dir) == 0)
        {
            if(is_dir) continue;
            if(sv_ends_with(name, mp3))
            {
                fds_log(FINFO, "Find musick file: "SV_FMT, SV_ARGS(name));
                sa_push(&tracks, name.data);
            }
        }
        fds_dir_iter_close(&it);
    }

    // printf("All trecks: \n");
    // sa_print(&tracks);s

    track = LoadSound(temp_arena_sprintf(temp_arena_get(), "%s/%s", TRACKS_FOLDER, tracks.data[current_song]));

    fds_log(FINFO, "Song count: %d", tracks.size);
    fds_log(FINFO, "Current track: %d", current_song);
    while(!WindowShouldClose())
    {
        Vector2 window_size = {GetRenderWidth(), GetRenderHeight()};
        if(IsKeyPressed(KEY_Q)) play_song();
        if(IsKeyPressed(KEY_W)) StopSound(track);
        if(IsKeyPressed(KEY_N)) next_song();
        if(IsKeyPressed(KEY_P)) prev_song();
        BeginDrawing();

            for(int i = 0; i < tracks.size;++i)
            {   
                SV t = sv_from_cstr(tracks.data[i]);
                sv_remove_suffix(&t,4);
                DrawTextEx(
                    font,
                    temp_arena_sprintf(temp_arena_get(), "%d: "SV_FMT, i, SV_ARGS(t)),
                    (Vector2){.x = 10, .y = 80+(30*i)},
                    32, 1, (i == current_song ? GREEN : RED));
            }
            ClearBackground(GetColor(0x262626));

        EndDrawing();
    }
    
    CloseWindow();
    CloseAudioDevice();
    sa_free(&tracks);
    return 0;
}