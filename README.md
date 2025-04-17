# CustomOS 64-bit Terminal v1.0

> A simple 64-bit operating system built from scratch using C and x86_64 Assembly. Built as a personal project to explore OS internals from the bootloader to a working terminal interface.

---

## ✨ Features

- 64-bit long mode kernel
- Custom hand-written bootloader
- Protected memory management
- Timer interrupt handler (100Hz)
- Keyboard input via polling
- Basic shell with built-in commands:
  - `help` — list available commands
  - `about` — OS information
  - `echo` — repeat input
  - `meminfo` — show memory information
  - `clear` — clear the terminal screen

---

## 🛠️ Build & Run

### 🔧 Requirements

- Linux (tested on Debian)
- `nasm`, `gcc`, `ld`, `make`
- `qemu-system-x86_64`

### ▶️ Run it:

```bash
make clean && make && make run
```

🔗 Featured on [LinkedIn](https://www.linkedin.com/posts/harikrishnan-kodakkad-414875294_osdev-assembly-linux-activity-7318582443366576128-RTsa?utm_source=share&utm_medium=member_desktop&rcm=ACoAAEdRuxABwzwUE2nAtof4sSYLVycoA0jPQgk)
