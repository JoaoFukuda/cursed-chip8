# Cursed CHIP-8

A terminal based CHIP-8 emulator as per these resources:

Resources | link
--------- | -----
mattmikolay/chip-8 | [https://github.com/mattmikolay/chip-8](https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908)
chip-8.com | [https://chip-8.com/info](https://storage.googleapis.com/wzukusers/user-34724694/documents/5ddc61256d08cO18xs1R/CHIP-8%20Instruction%20Set%20%28Classic%29.pdf)

> It only works in Linux because of library dependencies :V

## How to run

First, clone and compile it with `cmake`:

```bash
git clone https://github.com/JoaoFukuda/cursed-chip8.git
cd cursed-chip8
mkdir build && cmake -B build/ .
cmake --build build/
```

Then, just run it with one of the two example files (or copy and make one your own).

* `banana.c8`
: Prints *banana* to the screen
* `hex2dec.c8`
: Translates the value stored in register `3`.

```bash
./build/cc8 examples/banana.c8
```

