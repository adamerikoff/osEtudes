#include "./src/EvaLLVM.h"

int main(int argc, char *argv[]) {
    std::string program = R"(

        // Functors - callable objects

        (class Transformer null
            (begin

                (var factor 1)

                (def constructor (self factor) -> Transformer
                    (begin
                        (set (prop self factor) factor)
                        self))

                (def __call__ (self v)
                    (* (prop self factor) v))
            )
        )

        (var transform (new Transformer 5))

        (printf "(transformer 10) = %d\n" (transform 10))  // 50

        (def calculate (x (modify Transformer))
            (modify x))

        (printf "(calculate 10 transform) = %d\n" (calculate 10 transform))  // 50
    )";
    EvaLLVM vm;
    vm.exec(program);
    return 0;
}