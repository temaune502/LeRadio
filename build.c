#define SHORT_LOT
#undef  FDS_REBUILD_CFLAGS
#undef  FDS_REBUILD_CC
#define FDS_REBUILD_CFLAGS "-g -Wall -Wextra -pedantic -pipe"
#define FDS_REBUILD_CC "gcc"
#define FDS_IMPL
#include "src\fds.h"

#define standart_flags(cmd) \
    sa_push((&cmd), "clang");   \
    sa_pushm((&cmd), "-Wall", "-Wextra", "-g", "-pedantic", "-Wno-language-extension-token");

#define fds_cmd_run(cmd)    \
    fds_cmd_run_ext((cmd)); \
    fds_log(FINFO, "Cmd: %s", (cmd));

int main(int argc, char **argv)
{
    FDS_REBUILD_YOURSELF(argc, argv);
    fds_cmd_result result = {0};
    FlagSet *fl = flagset_new();

    bool run_program = false;

    flagset_bool(fl, &run_program, "run", false, "Run program after start");
    flagset_parse(fl, argc, argv);

    StringArray cmd = {0};
    standart_flags(cmd);
    sa_pushm(&cmd, "-O0", "-pipe", "-Wno-unused-function");
    sa_pushm(&cmd, "src/main.c");
    sa_push(&cmd, "-I raylib/include");
    sa_push(&cmd, "raylib/lib/libraylib.a");
    sa_push(&cmd, "-lgdi32");
    sa_push(&cmd, "-lwinmm");
    sa_pushm(&cmd, "build/fds_separate_unit.o");
    sa_pushm(&cmd, "src/gui.c");
    sa_pushm(&cmd, "-o", "build/main.exe");

    char *build_command = sa_join(&cmd, " ");
    sa_clear(&cmd);
    standart_flags(cmd);
    sa_pushm(&cmd, "-Wno-unused-function");
    sa_push(&cmd, "-pipe");
    sa_push(&cmd, "-c");
    sa_push(&cmd, "-O0");
    sa_push(&cmd, "src/fds_separate_unit.c");
    sa_pushm(&cmd, "-o", "build/fds_separate_unit.o");

    char *fds_build = sa_join(&cmd, " ");

    // fds_log(FINFO, "Build command: %s",     fds_build);
    // fds_log(FINFO, "Unit build commad: %s", build_command);

    if (fds_dir_exists("build"))
    {
        fds_log(FINFO, "Build dir dont exist creating...");
        fds_dir_create("build");
    }

    // Check fucking separate_unit!
    if(!fds_file_exists("build/fds_separate_unit.o"))
    {
        time_t source_time = fds_get_file_mtime("src/fds_separate_unit.c");
        time_t program_time = fds_get_file_mtime("build/fds_separate_unit.o");
        time_t fds_source = fds_get_file_mtime("src/fds.h");
        if (source_time > program_time || fds_source > program_time)
        {
            result = fds_cmd_run(fds_build);
            if (result.stdout_len > 0)
                printf("%s", result.stdout_data);
            if (result.stderr_len > 0)
                printf("%s", result.stderr_data);

            if (result.exit_code != 0)
                fds_log(FERROR, "Some probles cant build!\n Stderr: %s\n Stdout %s\n Exit code: %d", result.stderr_data, result.stdout_data, result.exit_code);
            fds_cmd_result_free(&result);
        }
    }
    else
    {
        result = fds_cmd_run(fds_build);
        if (result.stdout_len)
            printf("%s", result.stdout_data);
        if (result.stderr_len)
            printf("%s", result.stderr_data);

        if (result.exit_code != 0)
            fds_log(FERROR, "Some probles cant build!\n Stderr: %s\n Stdout %s\n Exit code: %d", result.stderr_data, result.stdout_data, result.exit_code);
        fds_cmd_result_free(&result);
    }

    // Check if exist main.o file
    if (!fds_file_exists("build/main.exe"))
    {
        time_t source_time = fds_get_file_mtime("src/main.c");
        time_t program_time = fds_get_file_mtime("build/main.exe");
        if (source_time > program_time)
        {
            result = fds_cmd_run(build_command);
            if (result.stdout_len > 0)
                printf("%s", result.stdout_data);
            if (result.stderr_len > 0)
                printf("%s", result.stderr_data);

            if (result.exit_code != 0)
                fds_log(FERROR, "Some probles cant build!\n Stderr: %s\n Stdout %s\n Exit code: %d", result.stderr_data, result.stdout_data, result.exit_code);
        }
    }
    else
    {
        result = fds_cmd_run(build_command);
        if (result.stdout_len > 0)
            printf("%s", result.stdout_data);
        if (result.stderr_len > 0)
            printf("%s", result.stderr_data);

        if (result.exit_code != 0)
            fds_log(FERROR, "Some probles cant build!\n Stderr: %s\n Stdout %s\n Exit code: %d", result.stderr_data, result.stdout_data, result.exit_code);
    }


    if (result.exit_code == 0 && run_program)
    {
        fds_cmd_run_detached("build/main.exe");
    }

    fds_cmd_result_free(&result);
    free(build_command);
    free(fds_build);
    sa_free(&cmd);
    flagset_free(fl);
}