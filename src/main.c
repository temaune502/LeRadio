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




StringArray tracks;
int current_song = 0;
Sound track;

void next_song()
{
    current_song++;
    if ((size_t)current_song >= tracks.size)
        current_song = 0;
    // fds_log(FINFO, "Current track: %d", current_song);
}

void prev_song()
{
    current_song--;
    if (current_song < 0)
    {
        current_song = (int)tracks.size - 1;
    }
//     fds_log(FINFO, "Current track: %d", current_song);
}
void play_song()
{
    static int track_local;
    
    if (current_song != track_local)
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
    InitWindow(800, 600, "LeRadio");
    InitAudioDevice();
    SetTargetFPS(60);


    float volume = 0.5f;
    float trackProgress = 45.0f; 
    bool shuffle = false;



    Font font = LoadFontEx("resources/fonts/segoeui.ttf", 32, 0, 255);

    FdsDirIter it;
    if (fds_dir_iter_open(TRACKS_FOLDER, &it) == 0)
    {
        SV name;
        SV mp3 = sv_from_cstr("mp3");
        int is_dir;
        while (fds_dir_iter_next(&it, &name, &is_dir) == 0)
        {
            if (is_dir)
                continue;
            if (sv_ends_with(name, mp3))
            {
                // fds_log(FINFO, "Find musick file: " SV_FMT, SV_ARGS(name));
                sa_push(&tracks, name.data);
            }
        }
        fds_dir_iter_close(&it);
    }

    // printf("All trecks: \n");
    // sa_print(&tracks);s

    track = LoadSound(temp_arena_sprintf(temp_arena_get(), "%s/%s", TRACKS_FOLDER, tracks.data[current_song]));

    // fds_log(FINFO, "Song count: %d", tracks.size);
    // fds_log(FINFO, "Current track: %d", current_song);
    while (!WindowShouldClose())
    {
        Vector2 window_size = {GetRenderWidth(), GetRenderHeight()};
        if (IsKeyPressed(KEY_Q))
            play_song();
        if (IsKeyPressed(KEY_W))
        {
            if(!IsSoundPlaying(track))
            {
                ResumeSound(track);
            }
            else
            {
                PauseSound(track);
            }
        }
        if (IsKeyPressed(KEY_N))
            next_song();
        if (IsKeyPressed(KEY_P))
            prev_song();
        BeginDrawing();

        // for (int i = 0; i < tracks.size; ++i)
        // {
        //     SV t = sv_from_cstr(tracks.data[i]);
        //     sv_remove_suffix(&t, 4);
        //     DrawTextEx(
        //         font,
        //         temp_arena_sprintf(temp_arena_get(), "%d: " SV_FMT, i, SV_ARGS(t)),
        //         (Vector2){.x = 10, .y = 80 + (30 * i)},
        //         32, 1, (i == current_song ? GREEN : RED));
        // }
        ClearBackground(GetColor(0x262626));




        Rectangle screenArea = { 10, 10, GetScreenWidth() - 20, GetScreenHeight() - 20 };
        Layout mainVBox = LayoutBegin(screenArea, LAYOUT_VERTICAL, 10.0f);

        // 1. Заголовок
        GuiLabel(LayoutNext(&mainVBox, 30.0f), "My C Player", WHITE);

        // 2. Центральна зона (Playlist + Album Art)
        float centerHeight = (screenArea.height - mainVBox.cursor) - 90.0f; 
        Layout centerHBox = LayoutBegin(LayoutNext(&mainVBox, centerHeight), LAYOUT_HORIZONTAL, 10.0f);
        
        Rectangle playlistArea = LayoutNext(&centerHBox, -0.7f); // 70% ширини
        DrawRectangleLinesEx(playlistArea, 1, DARKGRAY); // Заглушка під список
        
        Rectangle coverArea = LayoutNext(&centerHBox, 0.0f);     // Залишок 30%
        DrawRectangleRec(coverArea, DARKBLUE);

        // 3. Прогрес-бар треку (імітація руху для тесту)
        trackProgress += 0.05f;
        if (trackProgress > 100.0f) trackProgress = 0.0f;
        GuiProgressBar(LayoutNext(&mainVBox, 10.0f), trackProgress, 0.0f, 100.0f);

        // 4. Панель керування та налаштувань
        Layout controlsHBox = LayoutBegin(LayoutNext(&mainVBox, 50.0f), LAYOUT_HORIZONTAL, 10.0f);
        
        if (GuiButton(LayoutNext(&controlsHBox, 50.0f), "|<")) { /* Попередній */ }
        if (GuiButton(LayoutNext(&controlsHBox, 80.0f), "PLAY")) { /* Старт/Стоп */ }
        if (GuiButton(LayoutNext(&controlsHBox, 50.0f), ">|")) { /* Наступний */ }
        
        // Чекбокс під шафл (виділяємо квадрат 30x30, текст малюється збоку)
        Rectangle shuffleRect = LayoutNext(&controlsHBox, 30.0f);
        shuffleRect.height = 30; // Зменшуємо висоту чекбокса по центру
        shuffleRect.y += 10;
        GuiCheckbox(shuffleRect, "Shuffle", &shuffle);

        // Порожній простір, щоб відштовхнути гучність вправо
        LayoutNext(&controlsHBox, 40.0f); 

        // Слайдер гучності займає весь залишок
        volume = GuiSlider(LayoutNext(&controlsHBox, 0.0f), volume, 0.0f, 1.0f);




















        EndDrawing();
    }
    UnloadFont(font);
    CloseWindow();
    CloseAudioDevice();
    sa_free(&tracks);
    return 0;
}