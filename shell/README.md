# Ruby Shell

This project implements a simple command-line shell in Ruby. It supports parsing and executing commands, including pipelines (`|`), using `Parslet` for parsing and `IO.pipe` for connecting commands. The project demonstrates fundamental concepts in command-line interface design and system programming, such as process management and inter-process communication.

---

### Example Commands

1. Single Command:
   ```plaintext
   >> echo 'Hello, world!'
   ```
   Output:
   ```plaintext
   Hello, world!
   ```

2. Command Pipeline:
   ```plaintext
   >> echo 'Hello, world!' | grep Hello
   ```
   Output:
   ```plaintext
   Hello, world!
   ```

3. Chaining Commands:
   ```plaintext
   >> echo 'foo' | grep f | wc -c
   ```
   Output:
   ```plaintext
   4
   ```

### Exit the Shell
Type `Ctrl+C` or use `exit`.

---

## Resources

- [destroyallsoftware](https://www.destroyallsoftware.com/)

