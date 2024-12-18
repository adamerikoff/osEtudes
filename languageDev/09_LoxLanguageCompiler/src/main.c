#include "./headers/common.h"
#include "./headers/chunk.h"
#include "./headers/debug.h"
#include "./headers/vm.h"

VM vm;

void repl() {
    char line[1024];
    memset(line, 0, sizeof(line));
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(&vm, line);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = (size_t)ftell(file);
    if (fileSize == (size_t)-1L) {
        fprintf(stderr, "Could not determine file size \"%s\".\n", path);
        exit(74);
    }
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void runFile(const char* path) {
    char* source = readFile(path);
    InterpretResult result = interpret(&vm, source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) {
        exit(65);
    }
    if (result == INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

int main(int argc, const char* argv[]) {
    initVM(&vm);

    if (argc == 1) {
        repl();
    }
    else if (argc == 2) {
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
    
    freeVM(&vm);
    return 0;
}
