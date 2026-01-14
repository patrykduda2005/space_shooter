#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define RAYLIB_DIR "./build/external/raylib-master/src/"
#define PROJECT_OBJECTS_DIR "./build/build_files/obj/x64/Debug/projekt/"

Cmd cmd = {0};

void cflags(void) {
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-I./include/");
}

void gcc(void) {
    cmd_append(&cmd, "cc");
    cflags();
}

void gxx(void) {
    cmd_append(&cmd, "g++");
    cflags();
}

void libs(void) {
    cmd_append(&cmd, "-I"RAYLIB_DIR);
    cmd_append(&cmd, "-L"RAYLIB_DIR);
    cmd_append(&cmd, "-lraylib");
}

void build_raylib(void) {
    gcc();
    cmd_append(&cmd, RAYLIB_DIR"raudio.c");
    cmd_append(&cmd, RAYLIB_DIR"rcore.c");
    cmd_append(&cmd, RAYLIB_DIR"rglfw.c");
    cmd_append(&cmd, RAYLIB_DIR"rmodels.c");
    cmd_append(&cmd, RAYLIB_DIR"rshapes.c");
    cmd_append(&cmd, RAYLIB_DIR"rtext.c");
    cmd_append(&cmd, RAYLIB_DIR"rtextures.c");
    cmd_append(&cmd, RAYLIB_DIR"utils.c");
    cmd_append(&cmd, "-c");
    //cmd_append(&cmd, "-o", RAYLIB_DIR"libraylib.o");
    if (!nob_cmd_run(&cmd)) exit(1);
}

void build_if_needed(char *input, char *output) {
    if (needs_rebuild1(output, input)) {
        gxx();
        cflags();
        cmd_append(&cmd, input);
        libs();
        if (!nob_cmd_run(&cmd)) exit(1);
    };
}

void build_project(void) {
    build_if_needed("./src/ecs.cpp", PROJECT_OBJECTS_DIR"ecs.o");
    build_if_needed("./src/components.cpp", PROJECT_OBJECTS_DIR"components.o");
    build_if_needed("./src/resources.cpp", PROJECT_OBJECTS_DIR"resources.o");
    build_if_needed("./src/utils.cpp", PROJECT_OBJECTS_DIR"utils.o");
    if (needs_rebuild1("./bin/Debug/projekt", "./src/main.cpp")) {
        gxx();
        cmd_append(&cmd, PROJECT_OBJECTS_DIR"ecs.o");
        cmd_append(&cmd, PROJECT_OBJECTS_DIR"components.o");
        cmd_append(&cmd, PROJECT_OBJECTS_DIR"resources.o");
        cmd_append(&cmd, PROJECT_OBJECTS_DIR"utils.o");
        cmd_append(&cmd, "./src/main.cpp");
        cmd_append(&cmd, "-o", "./bin/Debug/projekt");
        libs();
        if (!nob_cmd_run(&cmd)) exit(1);
    }
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    //build_raylib();
    build_project();
    return 0;
}
