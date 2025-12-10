# 🗺️ Path-Finder GUI

A C++ application that visualizes pathfinding algorithms on a grid, built with the simple and easy-to-use [Raylib](https://www.raylib.com/) library. This tool allows users to create custom mazes, select start and end points, and watch as the algorithm finds a path from the source to the target.

https://drive.google.com/drive/u/0/folders/1yOgiDWgXpYm76Z8-vtaR58fy8TwyNxPp?ths=true

---

## ✨ Features

- **Interactive Grid**: A clean graphical user interface for creating a grid with obstacles.
- **Customizable Mazes**: Left-click on any cell to toggle it between a clear path and an obstacle.
- **Source & Target Selection**: Easily define the starting and ending nodes for the pathfinding algorithm.
- **Path Visualization**: Watch the algorithm explore the grid and find the optimal route.
- **Clear Path Display**: Once the target is reached, the application highlights the discovered path.
- **User Controls**: Simple buttons to start the search, reset the grid, or clear all obstacles.

---

## 🚀 Getting Started

Follow these instructions to compile and run the application on your local machine.

### Prerequisites

- A C++ compiler that supports C++17 (like MinGW g++ on Windows).
- The **Raylib** library files.

### Compilation

1.  **File Structure**: Ensure your project directory is set up correctly. The necessary Raylib files should be placed in `include` and `lib` folders relative to your source code:
    ```
    Path-Finder/
    ├── include/
    │   └── raylib.h
    ├── lib/
    │   └── libraylib.a  (or other raylib library file)
    └── path_finder_gui.cpp
    ```

2.  **Compile Command**: Open a terminal (like Git Bash or PowerShell) in the `Path-Finder` directory and run the following command:

    ```bash
    g++ path_finder_gui.cpp -o path_finder.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -std=c++17
    ```
    This command compiles your C++ code, links it against the Raylib library, and creates an executable file named `path_finder.exe`.

### Running the Application

After successful compilation, run the program from the same terminal:

```bash
./path_finder.exe
```

---

## 📖 How to Use the Application

1.  **Create a Maze**:
    - The application starts with a clear grid.
    - **Left-click** on any cell to turn it into an obstacle (a wall).
    - Click on an obstacle again to turn it back into a clear path.

2.  **Set Source and Target**:
    - **Right-click** on a clear cell to set it as the **Source Node** (start point).
    - **Right-click** on another clear cell to set it as the **Target Node** (end point).
    - You can change the source or target at any time by right-clicking on a different cell.

3.  **Use the Control Buttons**:
    - **Find Path**: Starts the pathfinding algorithm to find a route from the source to the target.
    - **Clear Path**: Removes the visualized search and the final path, but keeps your maze layout.
    - **Clear Grid**: Resets the entire grid, removing all obstacles, the source, and the target.

---

## 🧠 Core Logic: Pathfinding Algorithm

The solver is based on a pathfinding algorithm (such as A* or Dijkstra's) that efficiently explores the grid to find the shortest path between two points while avoiding obstacles.

1.  **Exploration**: The algorithm starts at the **Source Node** and explores adjacent, clear cells. It keeps track of the cells it has visited to avoid cycles.

2.  **Finding the Best Route**: It prioritizes cells that are closer to the target, making the search more efficient. It expands outwards, continuously seeking the most promising cell to explore next.

3.  **Path Construction**: As it explores, the algorithm remembers the "parent" of each cell (i.e., the cell from which it came). Once the **Target Node** is reached, it can reconstruct the path by backtracking from the target to the source through the chain of parents.

4.  **No Path Found**: If the algorithm explores all reachable cells and cannot find the target, it means the target is blocked off and no path exists.

This visualization makes it easy to understand how these powerful algorithms navigate complex spaces to find an optimal solution.

---