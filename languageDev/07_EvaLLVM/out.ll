; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@VERSION = global i32 1, align 4
@0 = private unnamed_addr constant [6 x i8] c"HALLO\00", align 1
@string = global ptr @0, align 4

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  ret i32 0
}
