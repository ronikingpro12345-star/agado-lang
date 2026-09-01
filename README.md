# Agado Programming Language (`agado-lang`)
> **An interpreted programming language engineered with a custom memory snapshot system, execution isolation, and directional rollback error handling.**

> **Developer Workflow:** *This project is in active development using an AI-assisted "vibe coding" workflow. I designed the core system architecture (e.g., the execution isolation boundaries, snapshot memory model) and utilized AI agents to accelerate syntax generation and rapid prototyping.*

---

## 🏗️ Core Architecture
Agado is designed to handle complex state management directly at the language runtime level. 

* **Execution Isolation:** Sandboxes variable scopes to prevent unintended side effects during recursive or high-risk execution loops.
* **The Snapshot System:** Implements persistent state tracking via custom memory checkpoints and explicit locator markers.
* **Directional Rollback:** Allows developers to trigger conditional rollback blocks, automatically reverting runtime state to a prior checkpoint without crashing the interpreter.

---

## 🛠️ Tech Stack & Requirements
* **Language:** C++ (Standard C++17/20)
* **Build System:** CMake & Ninja
* **Toolchain:** LLVM MinGW

---

## 🗺️ Roadmap & Current Status
- [x] Build core Abstract Syntax Tree (AST) parser.
- [x] Implement variable assignment, scopes, and lexical analysis.
- [ ] **[In Progress]** Engineer state checkpointing and locator marker logic.
- [ ] **[Planned]** Finalize directional rollback error handling blocks.

---

## ⚙️ How to Build & Run
bash
git clone https://github.com/ronikingpro12345-star/agado-lang.git
cd agado-lang
mkdir build && cd build
cmake -G "Ninja" ..
ninja


---

## 📄 License
Available under the [MIT License](LICENSE).