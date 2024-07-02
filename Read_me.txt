-->Algorithm for Path Finder:

-->We create two lists – Open List and Closed List  

-->Open List: It contains all those vertices or nodes or cells which are unvisited and has to be visit and they are marked as 1 in close list.

-->Close list: It contains all the visited cells and marked as 0.


-->Data Structures used:
1: 2D arrays: Close List, Input Grid
2: Queue: Open List
3: Stack: Store Path
4: Structure to 2D array: Info

-->Psuedo Code:

1: Initialize the closed list as a 2D array and marked all it's cell initially as 0(not visited).

2: Initialize the open list and store the source cell firstly in it.

3: While the open list is not empty;

    -->Stores first entry of the open list somewhere.

    --> Pop front entry from the open list.
  
    --> Generate all 8 successors of that stored cell and set their parents as store cell
   
    --> for each successor:
        i) If the successor is the target cell then print the path from the stack.
        
        ii) Else, compute both g and h for successor
           successor.g = store.g + distance between successor and store
           successor.h = distance from goal to successor (This can be done using many ways, we will discuss three heuristics- Manhattan, Diagonal and Euclidean Heuristics).
           successor.f = successor.g + successor.h
           Note: In this code, I have used Manhattan Heuristics(abs(current_row-target_row)+abs(current_col-target_col)).

        iii) If a node with the same position as successor is in the OPEN list which has a lower f than successor, skip this successor.
        iV) If a node with the same position as successor  is in the CLOSED list which has a lower f than successor, skip this successor otherwise, add  the node to the open list end.  
  
    --> Push store on the closed list end (while loop)

Note: In my code, if a cell's diagonally successor is covered by two adjacent block cells, then you can't move to that diaganol successor.
      Ex: If a cell is (i,j), then if (i-1,j)=0 and (i,j-1)=0, then you can't go to (i-1,j-1) successor. 