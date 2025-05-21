# Rush Hour Puzzle Solver

This project solves the classic **Rush Hour puzzle** using several informed search algorithms and provides a graphical interface for interactive puzzle creation and solution playback.

## 🚗 Features

- **Implemented Algorithms**:
  - **Uniform Cost Search (UCS)** – Expands nodes with the lowest total cost `g(n)`.
  - **A\* Search** – Uses `f(n) = g(n) + h(n)` to combine path cost and heuristic.
  - **Greedy Best-First Search** – Expands nodes based on heuristic `h(n)` only.
  - **Iterative Deepening A\* (IDA\*)** – Memory-efficient depth-first version of A\*.

- **Efficient C++ Engine** – Fast and optimized search implementation.
- **User-Friendly Java Swing GUI** – For puzzle input and step-by-step solution visualization.

## 🧱 Architecture

This project separates logic and interface for better performance and modularity:

- **C++** handles the core solving logic and search algorithms.
- **Java (Swing)** provides the GUI for puzzle building and interaction.
- Data is exchanged between the two via file-based input/output.

## 📦 Requirements

- C++ compiler (supporting C++17 or newer)
- `make`
- Java JDK (version 24 or newer)

## ▶️ How to Run

1. **Compile the C++ Solver**  
   From the root directory, run:
   ```bash
   make
   ```
2. **Start the GUI!**  
   From the root directory, run:
   ```bash
   java GUI.java
   ```

## 👥 Contributors

    Juan Sohuturon Arauna Siagian – 18222086
    Raudhah Yahya Kuddah – 13122003

   