ADDER = {
    ("B", "s1" ): ("(", "R", "s2"),
    ("B", "s2" ): ("1", "R", "s3"),
    ("B", "s3" ): ("1", "R", "s4"),
    ("B", "s4" ): ("+", "R", "s5"),
    ("B", "s5" ): ("1", "R", "s6"),
    ("B", "s6" ): ("1", "R", "s7"),
    ("B", "s7" ): ("1", "R", "s8"),
    ("B", "s8" ): (")", "R", "s9"),

    ("B", "s9" ): ("B", "L", "s9"),
    (")", "s9" ): (")", "L", "s9"),
    ("1", "s9" ): ("1", "L", "s9"),
    ("+", "s9" ): ("1", "R", "s10"),

    ("1", "s10" ): ("1", "R", "s10"),
    (")", "s10" ): ("B", "L", "s11"),

    ("1", "s11" ): (")", "R", "s12"),
    ("B", "s12" ): ("B", "R", "s12"),
}

def simulate(instructions):
    tape_size = len(instructions)
    tape  = ["B"] * tape_size
    head  = 0
    state = "s1"

    for _ in range(tape_size*2):
        print(state.rjust(4) + ": " + "".join(tape))
        print("      " + " " * head + "^")
        key = (tape[head], state)
        tape_sym, head_dir, new_state = instructions[key]
        tape[head] = tape_sym
        head += 1 if head_dir == "R" else -1
        state = new_state


simulate(ADDER)