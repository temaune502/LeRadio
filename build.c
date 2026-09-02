#define FDS_IMPL
#include "src\fds.h"

#define standart_flags(cmd) \
    sa_push((&cmd), "gcc");   \
    sa_pushm((&cmd), "-Wall", "-Wextra", "-g", "-pedantic");

#define fds_cmd_run(cmd)    \
    fds_cmd_run_ext((cmd)); \
    fds_log(FINFO, "Cmd: %s", (cmd));

int main(int argc, char **argv)
{
    fds_cmd_result result = {0};
    FlagSet *fl = flagset_new();

    bool run_program = false;
    bool needs_rebuild = false;

    flagset_bool(fl, &run_program, "run", false, "Run program after start");
    flagset_parse(fl, argc, argv);

    // time_t source_time  = get_file_mtime("build.c");
    // time_t program_time = get_file_mtime("build.exe");
    // if(source_time > program_time)
    // {
    //     needs_rebuild = true;
    //     printf("Rebuild!!!\n");
    //     result = fds_cmd_run_ext("gcc build.c -o build.exe.new");
    //     if(result.exit_code != 0)
    //     {
    //         needs_rebuild = false;
    //         fds_log(FERROR, "Could not rebuild itself\n %s, %s", result.stderr_data, result.stdout_data);
    //     }
    //     fds_cmd_result_free(&result);
    // }

    StringArray cmd = {0};
    standart_flags(cmd);
    sa_pushm(&cmd, "-O0", "-pipe", "-Wno-unused-function");
    sa_pushm(&cmd, "src/main.c");
    sa_push(&cmd, "-I:raylib/include");
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
    sa_push(&cmd, "-c");
    sa_push(&cmd, "-O0");
    sa_push(&cmd, "src/fds_separate_unit.c");
    sa_pushm(&cmd, "-o", "build/fds_separate_unit.o");

    char *fds_build = sa_join(&cmd, " ");

    fds_log(FINFO, "Build command: %s",     fds_build);
    fds_log(FINFO, "Unit build commad: %s", build_command);

    if (fds_dir_exists("build"))
    {
        fds_log(FINFO, "Build dir dont exist creating...");
        fds_dir_create("build");
    }

    // Check fucking separate_unit!
    if(!fds_file_exists("build/fds_separate_unit.o"))
    {
        time_t source_time = get_file_mtime("src/fds_separate_unit.c");
        time_t program_time = get_file_mtime("build/fds_separate_unit.o");
        time_t fds_source = get_file_mtime("src/fds.h");
        if (source_time > program_time || fds_source > program_time)
        {
            result = fds_cmd_run(fds_build);
            if (result.stdout_len < 0)
                printf("%s", result.stdout_data);
            if (result.stderr_len < 0)
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
        time_t source_time = get_file_mtime("src/main.c");
        time_t program_time = get_file_mtime("build/main.exe");
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
        if (result.stdout_len < 0)
            printf("%s", result.stdout_data);
        if (result.stderr_len < 0)
            printf("%s", result.stderr_data);

        if (result.exit_code != 0)
            fds_log(FERROR, "Some probles cant build!\n Stderr: %s\n Stdout %s\n Exit code: %d", result.stderr_data, result.stdout_data, result.exit_code);
    }

    // Check main executabl if exist and needs rebuild!
    // if(!fds_file_exists("build/main.exe"))

    // printf("Result exit code: %d\n", result.exit_code);

    if (result.exit_code == 0 & run_program)
    {

        // result = fds_cmd_run("build/main.exe");
        fds_cmd_run_async("build/main.exe");

        // printf("%s\n%s\n", result.stdout_data, result.stderr_data);
    }

    fds_cmd_result_free(&result);
    free(build_command);
    free(fds_build);
    sa_free(&cmd);
    flagset_free(fl);
    // if(needs_rebuild) {fds_cmd_run_async("cmd /c rename.bat");}
}