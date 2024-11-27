
#include "./includes.h"
#include "./vm/EvaVM.h"
#include "./vm/EvaValue.h"
#include "./parser/EvaParser.h"

void printHelp() {
    std::cout << "\nUsage: eva-vm [options]\n\n"
              << "Options:\n"
              << "   -e, --expression Expression to parse\n"
              << "   -f, --file       File to parse\n\n";
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printHelp();
        return 0;
    }
    std::string mode = argv[1];

    std::string program;

    if (mode == "-e") {
        program = argv[2];
    } else if (mode == "-f") {
        std::ifstream programFile(argv[2]);
        std::stringstream buffer;
        buffer << programFile.rdbuf() << "\n";

        program = buffer.str();
    } else {
        printHelp();
        return 0;
    }
    EvaVM vm;

    auto result = vm.exec(program);

    

    vm.DebugDumpStack(0);
    std::cout << std::endl;
    LOG(result);
    Traceable::printStats();
    Traceable::printStats();
    std::cout << std::endl;
    std::cout << "all passed!" << std::endl;
    return 0;
}