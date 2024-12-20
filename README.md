[![CI - Build and Test](https://github.com/Dark-Folt/6502-cpu-emulator/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/Dark-Folt/6502-cpu-emulator/actions/workflows/c-cpp.yml)

# **6502 CPU Emulator**

This project is a simple 6502 CPU emulator written in C. The goal is to understand how the 6502 processor works by implementing its core functionality and executing tests to validate the behavior.

## **Project Structure**
- **`src/`**: Contains the source code for the emulator.
- **`include/`**: Contains the header files.
- **`obj/`**: Stores the compiled object files.
- **`bin/`**: Stores the final compiled binary (`main`).
- **`tests/`**: Contains unit tests for the emulator.
- **`tests/bin/`**: Stores the compiled test binaries.

## **Dependencies**
This project requires:
- A C compiler (e.g., `gcc`).
- GNU Make (for building the project).
- [Criterion](https://github.com/Snaipe/Criterion) (a C testing framework).

### **On Linux**
Install the dependencies with:
```bash
sudo apt-get update
sudo apt-get install -y gcc make libcriterion-dev
```

### **On macOS**
Install the dependencies with [Homebrew](https://brew.sh/):
```bash
brew install gcc make criterion
```

## **How to Build and Run**
The project uses a `Makefile` to simplify the build process.

### **Build the Emulator**
Run the following command to compile the emulator:
```bash
make all
```
The compiled binary will be located in the `bin/` directory:
```bash
bin/main
```

### **Run the Emulator**
To run the emulator:
```bash
./bin/main
```

### **Run Tests**
The `Makefile` also provides a `test` target to compile and execute the unit tests. To run the tests:
```bash
make test
```

The tests are compiled into the `tests/bin/` directory, and each test binary is executed sequentially.

### **Clean the Project**
To clean up all compiled files:
```bash
make clean
```

---

## Sources :
https://cx16.dk/6502/reference.html  
https://www.masswerk.at/6502/6502_instruction_set.html  
https://en.wikibooks.org/wiki/6502_Assembly  
https://www.youtube.com/playlist?list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37  
