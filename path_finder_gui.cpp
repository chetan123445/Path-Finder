#include "raylib.h"
#include <vector>
#include <queue>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>

// --- Configuration ---
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
int GRID_ROWS = 20;
int GRID_COLS = 30;

const Color COLOR_EMPTY = LIGHTGRAY;
const Color COLOR_OBSTACLE = {40, 40, 40, 255};
const Color COLOR_PATH = GOLD;
const Color COLOR_START = GREEN;
const Color COLOR_TARGET = RED;
const Color COLOR_OPEN_SET = {173, 216, 230, 180}; // Light Blue, semi-transparent
const Color COLOR_CLOSED_SET = {255, 182, 193, 180}; // Light Pink, semi-transparent
const Color COLOR_GRID_LINES = GRAY;

// --- Data Structures ---

struct Point {
    int row, col;

    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

struct VisStep {
    Point current_node = {-1, -1};
    Point neighbor_node = {-1, -1};
    std::vector<Point> open_set_snapshot;
    std::vector<Point> closed_set_snapshot;
};

struct Node {
    Point pos;
    Point parent;
    float g; // Cost from start to current node
    float h; // Heuristic: estimated cost from current node to end
    float f; // g + h

    bool operator>(const Node& other) const {
        return f > other.f;
    }
};

// --- Main Application Logic ---

enum class AppState {
    AWAITING_INPUT,
    EDITING,
    VISUALIZING,
    FINISHED
};
// --- A* Pathfinding Algorithm ---

bool IsValid(int r, int c) {
    return r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS;
}

float CalculateHeuristic(Point a, Point b) {
    // Manhattan Distance
    return static_cast<float>(abs(a.row - b.row) + abs(a.col - b.col));
}

std::vector<Point> AStarSearch(const std::vector<std::vector<int>>& grid, Point start, Point target, std::vector<VisStep>& visSteps) {
    std::vector<Point> path;
    if (!IsValid(start.row, start.col) || !IsValid(target.row, target.col) || grid[start.row][start.col] == 0 || grid[target.row][target.col] == 0) {
        return path; // Return empty path if start/target is invalid or an obstacle
    }

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::map<int, Node> allNodes; // Using a map for efficient lookup and updates

    std::vector<Point> open_set_for_vis;
    std::vector<Point> closed_set_for_vis;

    Node startNode = {start, {-1, -1}, 0.0f, CalculateHeuristic(start, target), 0.0f};
    startNode.f = startNode.g + startNode.h;
    openSet.push(startNode);
    allNodes[start.row * GRID_COLS + start.col] = startNode;

    int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1}; // 8 directions
    int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        closed_set_for_vis.push_back(current.pos);
        // Remove from open set vis list
        open_set_for_vis.erase(std::remove(open_set_for_vis.begin(), open_set_for_vis.end(), current.pos), open_set_for_vis.end());

        // --- Visualization Step: Node picked from Open Set ---
        visSteps.push_back({current.pos, {-1, -1}, open_set_for_vis, closed_set_for_vis});

        if (current.pos == target) {
            Point temp = target;
            while (temp.row != -1 && temp.col != -1) {
                path.push_back(temp);
                temp = allNodes[temp.row * GRID_COLS + temp.col].parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 8; ++i) {
            int nextRow = current.pos.row + dr[i];
            int nextCol = current.pos.col + dc[i];

            // --- Visualization Step: Checking a neighbor ---
            visSteps.push_back({current.pos, {nextRow, nextCol}, open_set_for_vis, closed_set_for_vis});

            if (!IsValid(nextRow, nextCol) || grid[nextRow][nextCol] == 0) {
                continue;
            }

            // Diagonal movement check
            if (i >= 4) { // If it's a diagonal move
                int r1 = current.pos.row + dr[i];
                int c1 = current.pos.col;
                int r2 = current.pos.row;
                int c2 = current.pos.col + dc[i];
                if (!IsValid(r1, c1) || !IsValid(r2, c2) || grid[r1][c1] == 0 || grid[r2][c2] == 0) {
                    continue; // Blocked by adjacent walls
                }
            }

            float moveCost = (i < 4) ? 1.0f : 1.414f; // 1 for straight, sqrt(2) for diagonal
            float newG = current.g + moveCost;

            int nextNodeId = nextRow * GRID_COLS + nextCol;
            if (allNodes.find(nextNodeId) == allNodes.end() || newG < allNodes[nextNodeId].g) {
                Node successor;
                successor.pos = {nextRow, nextCol};
                successor.parent = current.pos;
                successor.g = newG;
                successor.h = CalculateHeuristic(successor.pos, target);
                successor.f = successor.g + successor.h;

                openSet.push(successor);
                allNodes[nextNodeId] = successor;
                open_set_for_vis.push_back(successor.pos);
            }
        }
    }

    return path; // Return empty path if no path found
}

// --- GUI Drawing ---

void DrawGrid(int gridWidth, int gridHeight, int cellWidth, int cellHeight) {
    for (int i = 0; i <= GRID_ROWS; i++) {
        DrawLine(0, i * cellHeight, gridWidth, i * cellHeight, COLOR_GRID_LINES);
    }
    for (int i = 0; i <= GRID_COLS; i++) {
        DrawLine(i * cellWidth, 0, i * cellWidth, gridHeight, COLOR_GRID_LINES);
    }
}

void DrawCells(const std::vector<std::vector<int>>& grid, int cellWidth, int cellHeight) {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            Color color = (grid[r][c] == 1) ? COLOR_EMPTY : COLOR_OBSTACLE;
            DrawRectangle(c * cellWidth, r * cellHeight, cellWidth, cellHeight, color);
        }
    }
}

void DrawPoint(Point p, Color color, int cellWidth, int cellHeight) {
    if (IsValid(p.row, p.col)) {
        DrawRectangle(p.col * cellWidth, p.row * cellHeight, cellWidth, cellHeight, color);
    }
}

void DrawPath(const std::vector<Point>& path, int cellWidth, int cellHeight) {
    for (size_t i = 0; i < path.size(); ++i) {
        DrawPoint(path[i], COLOR_PATH, cellWidth, cellHeight);
        // Draw numbers on path
        std::string num = std::to_string(i);
        int textWidth = MeasureText(num.c_str(), 10);
        DrawText(num.c_str(), path[i].col * cellWidth + cellWidth/2 - textWidth/2, path[i].row * cellHeight + cellHeight/2 - 5, 10, BLACK);
    }
}

void DrawVisualizationSets(const std::vector<Point>& openSet, const std::vector<Point>& closedSet, Point currentNode, Point neighborNode, int cellWidth, int cellHeight) {
    for(const auto& p : closedSet) {
        DrawPoint(p, COLOR_CLOSED_SET, cellWidth, cellHeight);
    }
    for(const auto& p : openSet) {
        DrawPoint(p, COLOR_OPEN_SET, cellWidth, cellHeight);
    }
    // Highlight the node currently being processed
    DrawPoint(currentNode, {255, 165, 0, 255}, cellWidth, cellHeight); // Orange
    // Highlight the neighbor being checked
    DrawPoint(neighborNode, {128, 0, 128, 255}, cellWidth, cellHeight); // Purple
}

void DrawUI(const char* statusText, AppState state, size_t pathLength, size_t openSetSize, size_t closedSetSize, Rectangle detailedButton, Rectangle finalButton) {
    // Draw a semi-transparent panel on the right
    DrawRectangle(SCREEN_WIDTH - 280, 0, 280, SCREEN_HEIGHT, {20, 20, 20, 200});
    DrawText("Path-Finder Control", SCREEN_WIDTH - 260, 20, 20, RAYWHITE);

    DrawText("Controls:", SCREEN_WIDTH - 260, 60, 16, LIGHTGRAY);
    DrawText("- Left Click: Place Obstacle", SCREEN_WIDTH - 250, 85, 14, LIGHTGRAY);
    DrawText("- Right Click: Remove Obstacle", SCREEN_WIDTH - 250, 105, 14, LIGHTGRAY);
    DrawText("- S Key: Set Start Point", SCREEN_WIDTH - 250, 125, 14, LIGHTGRAY);
    DrawText("- T Key: Set Target Point", SCREEN_WIDTH - 250, 145, 14, LIGHTGRAY);
    DrawText("- C Key: Clear Grid", SCREEN_WIDTH - 250, 165, 14, LIGHTGRAY);
    DrawText("- R Key: Reset Path", SCREEN_WIDTH - 250, 185, 14, LIGHTGRAY);

    DrawText("Actions:", SCREEN_WIDTH - 260, 210, 16, LIGHTGRAY);
    // Draw Detailed Button
    DrawRectangleRec(detailedButton, CheckCollisionPointRec(GetMousePosition(), detailedButton) ? GOLD : RAYWHITE);
    DrawText("Detailed Visualization", detailedButton.x + 10, detailedButton.y + 12, 18, BLACK);

    // Draw Final Path Button
    DrawRectangleRec(finalButton, CheckCollisionPointRec(GetMousePosition(), finalButton) ? GOLD : RAYWHITE);
    DrawText("Show Final Path", finalButton.x + 35, finalButton.y + 12, 18, BLACK);



    DrawText("Status:", SCREEN_WIDTH - 260, 400, 16, LIGHTGRAY);
    DrawText(statusText, SCREEN_WIDTH - 250, 425, 14, YELLOW);

    if (state == AppState::FINISHED) {
        DrawText("Search Statistics:", SCREEN_WIDTH - 260, 470, 16, LIGHTGRAY);
        if (pathLength > 0) {
            DrawText(TextFormat("- Path Length: %zu steps", pathLength), SCREEN_WIDTH - 250, 495, 14, LIGHTGRAY);
        }
        DrawText(TextFormat("- Cells Visited (Closed Set): %zu", closedSetSize), SCREEN_WIDTH - 250, 515, 14, LIGHTGRAY);
        DrawText(TextFormat("- Cells Considered (Open Set): %zu", openSetSize), SCREEN_WIDTH - 250, 535, 14, LIGHTGRAY);
    }
}

void DrawInputScreen(const char* rowInput, const char* colInput, bool editingRows, const char* errorMsg) {
    ClearBackground(DARKGRAY);
    DrawText("Set Grid Dimensions", GetScreenWidth() / 2 - MeasureText("Set Grid Dimensions", 40) / 2, 150, 40, RAYWHITE);

    // Draw Row Input Box
    DrawText("Rows:", 450, 250, 20, LIGHTGRAY);
    DrawRectangle(520, 240, 200, 40, RAYWHITE);
    if (editingRows) DrawRectangleLines(520, 240, 200, 40, GOLD);
    DrawText(rowInput, 530, 250, 20, BLACK);

    // Draw Col Input Box
    DrawText("Cols:", 450, 300, 20, LIGHTGRAY);
    DrawRectangle(520, 290, 200, 40, RAYWHITE);
    if (!editingRows) DrawRectangleLines(520, 290, 200, 40, GOLD);
    DrawText(colInput, 530, 300, 20, BLACK);

    if (errorMsg) {
        DrawText(errorMsg, GetScreenWidth() / 2 - MeasureText(errorMsg, 20) / 2, 350, 20, RED);
    }

    DrawText("Use TAB to switch between fields.", GetScreenWidth() / 2 - MeasureText("Use TAB to switch between fields.", 20) / 2, 420, 20, LIGHTGRAY);
    DrawText("Press ENTER to create the grid.", GetScreenWidth() / 2 - MeasureText("Press ENTER to create the grid.", 20) / 2, 450, 20, LIGHTGRAY);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Path-Finder GUI");
    SetTargetFPS(60);

    // Grid-related variables, to be initialized after user input
    int gridDisplayWidth;
    int cellWidth;
    int cellHeight;
    std::vector<std::vector<int>> grid;
    Point start;
    Point target;

    std::vector<Point> path;
    std::vector<VisStep> visSteps;
    size_t visStepIndex = 0;
    float visTimer = 0.0f;

    AppState currentState = AppState::AWAITING_INPUT;
    const char* statusText = "Awaiting grid dimensions...";

    // Variables for input screen
    char rowInput[4] = "20";
    int rowInputLen = 2;
    char colInput[4] = "30";
    int colInputLen = 2;
    bool editingRows = true;
    const char* errorMsg = nullptr;

    // UI Button Rectangles
    Rectangle detailedButton = { (float)SCREEN_WIDTH - 260, 235, 240, 40 };
    Rectangle finalButton = { (float)SCREEN_WIDTH - 260, 285, 240, 40 };

    // Main game loop
    while (!WindowShouldClose()) {
        // --- Update ---
        if (currentState == AppState::AWAITING_INPUT) {
            if (IsKeyPressed(KEY_TAB)) {
                editingRows = !editingRows;
            }

            int key = GetKeyPressed();
            if (key >= KEY_ZERO && key <= KEY_NINE) {
                if (editingRows) {
                    if (rowInputLen < 3) rowInput[rowInputLen++] = (char)key;
                } else {
                    if (colInputLen < 3) colInput[colInputLen++] = (char)key;
                }
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (editingRows) {
                    if (rowInputLen > 0) rowInputLen--;
                } else {
                    if (colInputLen > 0) colInputLen--;
                }
            }
            // Null-terminate the strings
            rowInput[rowInputLen] = '\0';
            colInput[colInputLen] = '\0';

            if (IsKeyPressed(KEY_ENTER)) {
                int rows = atoi(rowInput);
                int cols = atoi(colInput);

                if (rows > 0 && cols > 0 && rows <= 100 && cols <= 100) {
                    GRID_ROWS = rows;
                    GRID_COLS = cols;

                    // Initialize grid and related variables
                    gridDisplayWidth = SCREEN_WIDTH - 280;
                    cellWidth = gridDisplayWidth / GRID_COLS;
                    cellHeight = SCREEN_HEIGHT / GRID_ROWS;
                    grid.assign(GRID_ROWS, std::vector<int>(GRID_COLS, 1));
                    start = {GRID_ROWS / 4, GRID_COLS / 4};
                    target = {GRID_ROWS * 3 / 4, GRID_COLS * 3 / 4};

                    currentState = AppState::EDITING;
                    statusText = "Editing Grid. Set Start (S) and Target (T).";
                    errorMsg = nullptr;
                } else {
                    errorMsg = "Invalid dimensions. Rows/Cols must be > 0 and <= 100.";
                }
            }
        } else { // Handle other states (EDITING, VISUALIZING, FINISHED)
            Vector2 mousePos = GetMousePosition();
            int mouseRow = mousePos.y / cellHeight;
            int mouseCol = mousePos.x / cellWidth;

            if (currentState == AppState::EDITING) {
            if (mousePos.x < gridDisplayWidth) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    if (IsValid(mouseRow, mouseCol)) {
                        grid[mouseRow][mouseCol] = 0; // Set obstacle
                    }
                }
                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                     if (IsValid(mouseRow, mouseCol)) {
                        grid[mouseRow][mouseCol] = 1; // Remove obstacle
                    }
                }

                if (IsKeyPressed(KEY_S)) {
                    if (IsValid(mouseRow, mouseCol) && grid[mouseRow][mouseCol] == 1) {
                        start = {mouseRow, mouseCol};
                        statusText = "Start point updated.";
                    } else {
                        statusText = "Cannot place point on an obstacle!";
                    }
                }

                if (IsKeyPressed(KEY_T)) {
                    if (IsValid(mouseRow, mouseCol) && grid[mouseRow][mouseCol] == 1) {
                        target = {mouseRow, mouseCol};
                        statusText = "Target point updated.";
                    } else {
                        statusText = "Cannot place point on an obstacle!";
                    }
                }
            }

            }
            // --- Button Click Handling ---
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePoint = GetMousePosition();

                // Check for Detailed Visualization button click
                if (CheckCollisionPointRec(mousePoint, detailedButton)) {
                    if (grid[start.row][start.col] == 0) {
                        statusText = "Start point is blocked! Press R to reset.";
                        currentState = AppState::FINISHED;
                    } else if (grid[target.row][target.col] == 0) {
                        statusText = "Target point is blocked! Press R to reset.";
                        currentState = AppState::FINISHED;
                    } else {
                        visSteps.clear();
                        path.clear();
                        visStepIndex = 0;
                        path = AStarSearch(grid, start, target, visSteps);

                        if (!visSteps.empty() && !path.empty()) {
                            currentState = AppState::VISUALIZING;
                            statusText = "Visualizing search... Press SPACE to skip.";
                        } else {
                            currentState = AppState::FINISHED;
                            statusText = "No path found! Press R to reset.";
                        }
                    }
                }

                // Check for Show Final Path button click
                if (CheckCollisionPointRec(mousePoint, finalButton)) {
                    if (grid[start.row][start.col] == 0) {
                        statusText = "Start point is blocked! Press R to reset.";
                        currentState = AppState::FINISHED;
                    } else if (grid[target.row][target.col] == 0) {
                        statusText = "Target point is blocked! Press R to reset.";
                        currentState = AppState::FINISHED;
                    } else {
                        visSteps.clear();
                        path.clear();
                        visStepIndex = 0;
                        path = AStarSearch(grid, start, target, visSteps);

                        currentState = AppState::FINISHED;
                        if (path.empty()) {
                            statusText = "No path found! Press R to reset.";
                        } else {
                            statusText = "Path found! Press R to reset.";
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_C)) {
                for (auto& row : grid) std::fill(row.begin(), row.end(), 1);
                start = {GRID_ROWS / 4, GRID_COLS / 4};
                target = {GRID_ROWS * 3 / 4, GRID_COLS * 3 / 4};
                statusText = "Grid cleared. Ready for editing.";
            }
            
            if (currentState == AppState::VISUALIZING) {
                // Animate the search process
                visTimer += GetFrameTime();

                // Update step every 1 second for a clear view
                if (visTimer >= 1.0f) {
                    visTimer = 0.0f;
                    if (visStepIndex < visSteps.size() - 1) visStepIndex++;
                }

                if (visStepIndex >= visSteps.size() - 1) {
                    currentState = AppState::FINISHED;
                    statusText = path.empty() ? "No path found!" : "Path found!";
                }
                // Allow skipping the animation
                if (IsKeyPressed(KEY_SPACE)) {
                    currentState = AppState::FINISHED;
                    statusText = "Visualization skipped. Path found!";
                }
            }

            if (IsKeyPressed(KEY_R)) {
                visSteps.clear();
                path.clear();
                currentState = AppState::EDITING;
                statusText = "Path reset. Ready for editing.";
            }
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == AppState::AWAITING_INPUT) {
            DrawInputScreen(rowInput, colInput, editingRows, errorMsg);
        } else {
            // Draw Grid and Cells
            DrawCells(grid, cellWidth, cellHeight);

            // Visualization Drawing
            if (currentState == AppState::VISUALIZING) {
                const auto& currentStep = visSteps[visStepIndex];
                DrawVisualizationSets(currentStep.open_set_snapshot, currentStep.closed_set_snapshot, currentStep.current_node, currentStep.neighbor_node, cellWidth, cellHeight);
            }

            if (currentState == AppState::FINISHED) {
                // Show the final state of the search
                if (!visSteps.empty()) {
                    const auto& lastStep = visSteps.back();
                    // Get the final open/closed sets from the last step of the visualization
                    size_t finalOpenCount = lastStep.open_set_snapshot.size();
                    size_t finalClosedCount = lastStep.closed_set_snapshot.size();
                    DrawVisualizationSets(lastStep.open_set_snapshot, lastStep.closed_set_snapshot, {-1,-1}, {-1,-1}, cellWidth, cellHeight);
                }
                if (!path.empty()) {
                    DrawPath(path, cellWidth, cellHeight);
                }
            }

            // Draw Start and Target points on top of everything
            DrawPoint(start, COLOR_START, cellWidth, cellHeight);
            DrawPoint(target, COLOR_TARGET, cellWidth, cellHeight);

            // Draw Grid lines on top of cells
            DrawGrid(gridDisplayWidth, SCREEN_HEIGHT, cellWidth, cellHeight);

            // Draw UI Panel
            size_t finalOpen = visSteps.empty() ? 0 : visSteps.back().open_set_snapshot.size();
            size_t finalClosed = visSteps.empty() ? 0 : visSteps.back().closed_set_snapshot.size();
            DrawUI(statusText, currentState, path.size(), finalOpen, finalClosed, detailedButton, finalButton);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

/*
--- HOW TO COMPILE AND RUN (using g++) ---

Prerequisites:
1. You have a C++ compiler (like g++ from MinGW on Windows).
2. You have downloaded Raylib and placed the 'lib' and 'include' folders
   from the Raylib release package into your project's root directory.
   Your project structure should look like this:
   - Your_Project_Folder/
     - path_finder_gui.cpp
     - include/ (contains raylib.h, etc.)
     - lib/ (contains libraylib.a)

Compilation Command (run in your terminal/command prompt from the project folder):

g++ path_finder_gui.cpp -o path_finder.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

Explanation of flags:
-o path_finder.exe   : Specifies the output executable file name.
-Iinclude            : Tells the compiler to look for header files in the 'include' directory.
-Llib                : Tells the linker to look for library files in the 'lib' directory.
-lraylib             : Links the Raylib library.
-lopengl32 -lgdi32 -lwinmm : These are system libraries that Raylib depends on (on Windows).

Run Command:

./path_finder.exe

*/


/*
--- ANALYSIS OF CHANGES AND IMPROVEMENTS ---

1.  Algorithm Correction & Enhancement:
    - Data Structure for Open Set: Your pseudo-code mentioned a 'Queue' for the open list. For an A* algorithm, a `std::priority_queue` is the correct and much more efficient data structure. It ensures that the node with the lowest 'f' score is always processed first, which is the core principle of A*. This change improves the algorithm's time complexity from O(N*M) in the worst-case of a simple queue (like BFS) to a much faster average case for A*.
    - Closed Set Handling: Instead of a 2D boolean array, I've used a `std::map` (`allNodes`) to store all nodes that have been encountered. This map stores the node's position, parent, and costs. This is more memory-efficient for sparse searches and makes it easier to check if a newly found path to an existing node is better (has a lower 'g' cost).
    - Diagonal Movement: The code correctly implements your rule: diagonal movement is blocked if the path is "squeezing" between two adjacent obstacles. It also uses a more accurate cost for diagonal moves (sqrt(2) ≈ 1.414) versus straight moves (1.0), leading to more optimal paths.

2.  Complete GUI Implementation with Raylib:
    - Interactive Grid: The program now has a fully interactive grid. You can draw obstacles with the left mouse button and set the start/target points using keyboard shortcuts ('S' and 'T').
    - Clear Visualization: Different colors are used to clearly distinguish between empty cells, obstacles, the start/target points, the final path, and the cells being explored (open/closed sets). This makes the algorithm's behavior easy to understand.
    - Stylish Obstacles: Obstacles are drawn in a dark color, providing a high-contrast, "stylish" look against the lighter grid. The path is a bright gold color to stand out.

3.  User-Selectable Visualization Modes:
    - Detailed vs. Final Path: As you requested, the user can toggle between two modes with the 'V' key.
      - Detailed View: Shows a step-by-step animation of the A* algorithm's search process, visualizing the expansion of the open and closed sets. This is excellent for learning how the algorithm works.
      - Final Path View: Instantly computes and displays the final path, providing a quick solution.

4.  Code Quality and Clarity:
    - Modularity: The code is organized into logical sections: data structures, the A* algorithm, GUI drawing functions, and the main application loop. This separation of concerns makes the code easier to read, understand, and maintain.
    - Clear Naming: Variables and functions have descriptive names (e.g., `AStarSearch`, `DrawCells`, `COLOR_OBSTACLE`) that make their purpose immediately clear.
    - State Management: An `enum class AppState` is used to manage the application's state (e.g., editing, visualizing, finished), which results in cleaner and less error-prone logic in the main loop compared to using multiple boolean flags.

*/