# Lox: A Dynamically Typed, Interpreted Language

Lox is a simple, dynamically typed programming language designed to help users explore and understand the core concepts of language implementation. Whether you're a curious learner or an aspiring language developer, Lox provides an approachable way to dive into the mechanics of modern programming languages.

---

## Key Features

- **Dynamic Typing**  
  Variables don't have fixed data types. Their type is determined at runtime based on the value assigned to them.

- **Garbage Collection**  
  Automatic memory management handles memory allocation and deallocation, freeing the programmer from manual memory management.

- **Simple Syntax**  
  Lox boasts a clean and concise syntax, making it easy to learn and write.

- **First-Class Functions**  
  Functions in Lox are first-class citizens, meaning they can be assigned to variables, passed as arguments, and returned from other functions.

- **Classes and Inheritance**  
  Lox supports object-oriented programming with classes and inheritance, allowing users to build reusable and modular code.

---

## Language Architecture

Lox's architecture is built around three main components:

1. **Lexer**  
   The lexer (or scanner) breaks the source code into tokens, such as keywords, identifiers, operators, and literals. This is the first step in transforming the source code into a structured format.

2. **Parser**  
   The parser constructs an Abstract Syntax Tree (AST) from the token stream. The AST represents the syntactic structure of the program in a hierarchical form.

3. **Interpreter**  
   The interpreter traverses the AST, executing statements and evaluating expressions to bring the program to life.

---

## Example Code

Here are some examples to showcase the versatility and simplicity of Lox:

### Factorial Using Recursion
```javascript
fun factorial(n) {
  if (n <= 1) return 1;
  return n * factorial(n - 1);
}

print factorial(5); // Output: 120
```

### Inheritance and Method Overriding
```javascript
class Animal {
  speak() {
    print "Some generic animal sound";
  }
}

class Dog < Animal {
  speak() {
    print "Woof!";
  }
}

var myDog = Dog();
myDog.speak(); // Output: Woof!
```

### Closures
```javascript
fun counter() {
  var count = 0;
  fun increment() {
    count = count + 1;
    return count;
  }
  return increment;
}

var myCounter = counter();
print myCounter(); // Output: 1
print myCounter(); // Output: 2
```

### Loops and Printing
```javascript
for (var i = 1; i < 5; i = i + 1) {
  print i * i;
}

// Output:
// 1
// 4
// 9
// 16
```

### Functional Programming with Higher-Order Functions
```javascript
fun makeAdder(n) {
  fun adder(i) {
    return n + i;
  }
  return adder;
}

var add5 = makeAdder(5);
print add5(10); // Output: 15
print add5(20); // Output: 25
```

---

## Resources

- **[Crafting Interpreters](https://craftinginterpreters.com/)**  
  A fantastic resource by Robert Nystrom that walks through the implementation of Lox in two parts: an interpreter in Java and a virtual machine in C.