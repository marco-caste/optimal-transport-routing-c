# Optimal Transport Routing on Hexagonal Map

Final project for the "Algorithms and Data Structures" (2024-2025) course at Politecnico di Milano.

### 📝 Description
A C program developed for a logistics company to calculate optimal routes on a map tiled with hexagons. The system dynamically handles cost changes for land travel and the opening/closing of direct air routes between nodes.

### ✨ Key Features
- **Pathfinding Algorithm:** Implements a Dijkstra-based search to calculate the minimum travel cost between any two valid hexagonal tiles (`travel_cost` command).
- **Custom Min-Heap:** Utilizes a custom-built Min-Heap priority queue (`ModificaHeap`) to ensure efficient node extraction and graph traversal.
- **Dynamic Graph Updates:** Processes real-time updates to travel costs (`change_cost`) and network topology (`toggle_air_route`) using dynamic memory allocation.
  
### ⚠️ Technical Note: Performance over Readability
This codebase was developed under strict time and memory execution constraints for an automated academic evaluation. To achieve the maximum performance score, the code was heavily optimized at the expense of standard "clean code" practices (e.g., avoiding standard library abstractions, implementing a flat custom Min-Heap, and favoring inline operations). Variable names and internal comments remain in Italian to preserve the original logic.

### 🛠️ Tech Stack
- **Language:** C (Standard Library)
- **Concepts:** Graph Theory, Priority Queues, Dynamic Memory Allocation

### 🚀 How to Run
Compile the source code using GCC and provide a sequence of commands via standard input:
```bash
gcc main.c -o routing
./routing
```
