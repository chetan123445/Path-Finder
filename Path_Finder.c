#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>

struct queue
{
    int row;
    int col;
    struct queue *link;
};
struct queue *front=NULL;
struct queue *rear=NULL;

struct Cell_details
{
    int parent_row;
    int parent_col;
    int f;
    int g;
    int h;
};

void add_in_OpenSet(int r,int c)
{
    struct queue *newnode=(struct queue*)malloc(sizeof(struct queue));
    newnode->row=r;
    newnode->col=c;
    newnode->link=NULL;
    if(front==NULL)
    {
        front=newnode;
    }
    else
    {
        rear->link=newnode;
    }
    rear=newnode;
}

bool isValid(int noderow,int nodecol,int rows,int cols)
{
    if(noderow<0 || nodecol<0 || noderow>=rows || nodecol>=cols)
    {
        return false;
    }
    return true;
}

bool isBlocked(int no_rows,int no_cols,int grid[][no_cols],int row,int col)
{
    if(grid[row][col]==0)
    {
        return true;
    }
    return false;
}

bool isSame(int srcrow,int srccol,int tarrow,int tarcol)
{
    if(srcrow==tarrow && srccol==tarcol)
    {
        return true;
    }
    return false;
}

bool isEmpty(struct queue *front)
{
    if(front==NULL)
    {
        return true;
    }
    return false;
}

int h_value(int row,int col,int tarrow,int tarcol)
{
    return abs(row-tarrow)+abs(col-tarcol);
}

struct stack
{
    int row;
    int col;
    struct stack *next;
    struct stack *previous;
};
struct stack *top=NULL;

void push(int row,int col)
{
    struct stack *newnode=(struct stack *)malloc(sizeof(struct stack));
    newnode->row=row;
    newnode->col=col;
    newnode->previous=NULL;
    newnode->next=NULL;
    if(top==NULL)
    {
        top=newnode;
    } 
    else
    {
        top->next=newnode;
        newnode->previous=top;
        top=newnode;
    }
}

void Path(int no_rows,int no_cols,struct Cell_details info[][no_cols],int tarrow,int tarcol)
{
    int row=tarrow;
    int col=tarcol;
    while (!(row==info[row][col].parent_row && col==info[row][col].parent_col))
    {
        push(row,col);
        int temp_row=info[row][col].parent_row;
        int temp_col=info[row][col].parent_col;
        row=temp_row;
        col=temp_col;
    }
    push(row,col);
    while(top!=NULL)
    {
        printf("(%d,%d)",top->row,top->col);
        top=top->previous;
        if(top!=NULL)
        {
            printf("->");
        }
    }
    printf("\n");
}

void PathFinder(int no_rows,int no_cols,int grid[][no_cols],int srcrow,int srccol,int tarrow,int tarcol)
{
    if(isBlocked(no_rows,no_cols,grid,srcrow,srccol))
    {
        printf("The source is blocked\n");
        return;
    }
    if(isBlocked(no_rows,no_cols,grid,tarrow,tarcol))
    {
        printf("The target is blocked\n");
        return;
    }
    if(isSame(srcrow,srccol,tarrow,tarcol))
    {
        printf("The target node is same as the source node\n");
        return;
    }

    int close_list[no_rows][no_cols];
    for(int i=0;i<no_rows;i++)
    {
        for(int j=0;j<no_cols;j++)
        {
            close_list[i][j]=0;
        }
    }
    
    struct Cell_details info[no_rows][no_cols];
    for(int i=0;i<no_rows;i++)
    {
        for(int j=0;j<no_cols;j++)
        {
            info[i][j].f=INT_MAX;
            info[i][j].g=INT_MAX;
            info[i][j].h=INT_MAX;
            info[i][j].parent_row=-1;
            info[i][j].parent_col=-1;
        }
    }
    
    info[srcrow][srccol].f=0;
    info[srcrow][srccol].g=0;
    info[srcrow][srccol].h=0;
    info[srcrow][srccol].parent_row=srcrow;
    info[srcrow][srccol].parent_col=srccol;

    add_in_OpenSet(srcrow,srccol);
    close_list[srcrow][srccol]=1;
    bool Destination_found=false;
    int g_new,h_new,f_new;

    while(!isEmpty(front))
    {
        struct queue *store=front;
        front=front->link;
        int i=store->row;
        int j=store->col;
        if(i-1>=0)
        {
            if(i-1==tarrow && j==tarcol)
            {
                info[i-1][j].parent_row=i;
                info[i-1][j].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
            }
            else if(close_list[i-1][j]==0 && isBlocked(no_rows,no_cols,grid,i-1,j)==false)
            {
                close_list[i-1][j]=1;
                g_new=info[i][j].g+1;
                h_new=h_value(i-1,j,tarrow,tarcol);
                f_new=g_new+h_new;
                if(info[i-1][j].f==INT_MAX || info[i-1][j].f>f_new)
                {
                    add_in_OpenSet(i-1,j);
                    info[i-1][j].f=f_new;
                    info[i-1][j].g=g_new;
                    info[i-1][j].h=h_new;
                    info[i-1][j].parent_row=i;
                    info[i-1][j].parent_col=j;
                }
            }
        }
        if(i+1<no_rows)
        {
            if(i+1==tarrow && j==tarcol)
            {
                info[i+1][j].parent_row=i;
                info[i+1][j].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
            }
            else if(close_list[i+1][j]==0 && isBlocked(no_rows,no_cols,grid,i+1,j)==false)
            {
                close_list[i+1][j]=1;
                g_new=info[i][j].g+1;
                h_new=h_value(i+1,j,tarrow,tarcol);
                f_new=g_new+h_new;
                if(info[i+1][j].f==INT_MAX || info[i+1][j].f>f_new)
                {
                    add_in_OpenSet(i+1,j);
                    info[i+1][j].f=f_new;
                    info[i+1][j].g=g_new;
                    info[i+1][j].h=h_new;
                    info[i+1][j].parent_row=i;
                    info[i+1][j].parent_col=j;
                }
            }
        }
        if(j-1>=0)
        {
            if(i==tarrow && j-1==tarcol)
            {
                info[i][j-1].parent_row=i;
                info[i][j-1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
            }
            else if(close_list[i][j-1]==0 && isBlocked(no_rows,no_cols,grid,i,j-1)==false)
            {
                close_list[i][j-1]=1;
                g_new=info[i][j].g+1;
                h_new=h_value(i,j-1,tarrow,tarcol);
                f_new=g_new+h_new;
                if(info[i][j-1].f==INT_MAX || info[i][j-1].f>f_new)
                {
                    add_in_OpenSet(i,j-1);
                    info[i][j-1].f=f_new;
                    info[i][j-1].g=g_new;
                    info[i][j-1].h=h_new;
                    info[i][j-1].parent_row=i;
                    info[i][j-1].parent_col=j;
                }
            }
        }
        if(j+1<no_cols)
        {
            if(i==tarrow && j+1==tarcol)
            {
                info[i][j+1].parent_row=i;
                info[i][j+1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
            }
            else if(close_list[i][j+1]==0 && isBlocked(no_rows,no_cols,grid,i,j+1)==false)
            {
                close_list[i][j+1]=1;
                g_new=info[i][j].g+1;
                h_new=h_value(i,j+1,tarrow,tarcol);
                f_new=g_new+h_new;
                if(info[i][j+1].f==INT_MAX || info[i][j+1].f>f_new)
                {
                    add_in_OpenSet(i,j+1);
                    info[i][j+1].f=f_new;
                    info[i][j+1].g=g_new;
                    info[i][j+1].h=h_new;
                    info[i][j+1].parent_row=i;
                    info[i][j+1].parent_col=j;
                }
            }
        }
        if(i-1>=0 && j-1>=0 && (close_list[i-1][j]==1 && close_list[i][j-1]==0) || (close_list[i-1][j]==0 && close_list[i][j-1]==1) || (close_list[i-1][j]==1 && close_list[i][j-1]==1))// left->up diagonal cell
         {
             if(i-1==tarrow && j-1==tarcol)
             {
                info[i-1][j-1].parent_row=i;
                info[i-1][j-1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
             }
             else if(close_list[i-1][j-1]==0 && isBlocked(no_rows,no_cols,grid,i-1,j-1)==false)
             {
                    close_list[i-1][j-1]=1;
                    g_new=info[i][j].g+1;
                    h_new=h_value(i-1,j-1,tarrow,tarcol);
                    f_new=g_new+h_new;
                    if(info[i-1][j-1].f==INT_MAX || info[i-1][j-1].f>f_new)
                    {
                        add_in_OpenSet(i-1,j-1);
                        // Update the details of this cell
                        info[i-1][j-1].f=f_new;
                        info[i-1][j-1].g=g_new;
                        info[i-1][j-1].h=h_new;
                        info[i-1][j-1].parent_row=i;
                        info[i-1][j-1].parent_col=j;
                    }
             }
         }
         if(i-1>=0 && j+1<no_cols && (close_list[i-1][j]==1 && close_list[i][j+1]==0) || (close_list[i-1][j]==0 && close_list[i][j+1]==1) || (close_list[i-1][j]==1 && close_list[i][j+1]==1))// right->up diagonal cell
         {
             if(i-1==tarrow && j+1==tarcol)
             {
                info[i-1][j+1].parent_row=i;
                info[i-1][j+1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
             }
             else if(close_list[i-1][j+1]==0 && isBlocked(no_rows,no_cols,grid,i-1,j+1)==false)
             {
                    close_list[i-1][j+1]=1;
                    g_new=info[i][j].g+1;
                    h_new=h_value(i-1,j+1,tarrow,tarcol);
                    f_new=g_new+h_new;
                    if(info[i-1][j+1].f==INT_MAX || info[i-1][j+1].f>f_new)
                    {
                        add_in_OpenSet(i-1,j+1);
                        // Update the details of this cell
                        info[i-1][j+1].f=f_new;
                        info[i-1][j+1].g=g_new;
                        info[i-1][j+1].h=h_new;
                        info[i-1][j+1].parent_row=i;
                        info[i-1][j+1].parent_col=j;
                    }
             }
         }
         if(i+1<no_rows && j-1>=0 && (close_list[i+1][j]==1 && close_list[i][j-1]==0) || (close_list[i+1][j]==0 && close_list[i][j-1]==1) || (close_list[i+1][j]==1 && close_list[i][j-1]==1))// left->down diagonal cell
         {
             if(i+1==tarrow && j-1==tarcol)
             {
                info[i+1][j-1].parent_row=i;
                info[i+1][j-1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
             }
             else if(close_list[i+1][j-1]==0 && isBlocked(no_rows,no_cols,grid,i+1,j-1)==false)
             {
                    close_list[i+1][j-1]=1;
                    g_new=info[i][j].g+1;
                    h_new=h_value(i+1,j-1,tarrow,tarcol);
                    f_new=g_new+h_new;
                    if(info[i+1][j-1].f==INT_MAX || info[i+1][j-1].f>f_new)
                    {
                        add_in_OpenSet(i+1,j-1);
                        // Update the details of this cell
                        info[i+1][j-1].f=f_new;
                        info[i+1][j-1].g=g_new;
                        info[i+1][j-1].h=h_new;
                        info[i+1][j-1].parent_row=i;
                        info[i+1][j-1].parent_col=j;
                    }
             }
         }
         if(i+1<no_rows && j+1<no_cols && (close_list[i+1][j]==1 && close_list[i][j+1]==0) || (close_list[i+1][j]==0 && close_list[i][j+1]==1) || (close_list[i+1][j]==1 && close_list[i][j+1]==1))// right->down diagonal cell
         {
             if(i+1==tarrow && j+1==tarcol)
             {
                info[i+1][j+1].parent_row=i;
                info[i+1][j+1].parent_col=j;
                Destination_found=true;
                printf("We have reached the target node using the following path(row,col):\n");
                Path(no_rows,no_cols,info,tarrow,tarcol);
                return;
             }
             else if(close_list[i+1][j+1]==0 && isBlocked(no_rows,no_cols,grid,i+1,j+1)==false)
             {
                    close_list[i+1][j+1]=1;
                    g_new=info[i][j].g+1;
                    h_new=h_value(i+1,j+1,tarrow,tarcol);
                    f_new=g_new+h_new;
                    if(info[i+1][j+1].f==INT_MAX || info[i+1][j+1].f>f_new)
                    {
                        add_in_OpenSet(i+1,j+1);
                        // Update the details of this cell
                        info[i+1][j+1].f=f_new;
                        info[i+1][j+1].g=g_new;
                        info[i+1][j+1].h=h_new;
                        info[i+1][j+1].parent_row=i;
                        info[i+1][j+1].parent_col=j;
                    }
             }
         }
    }
    if(Destination_found==false)
    {
        printf("We couldn't reach the target node\n");
    }
}

int main()
{
    printf("                        INPUT\n");
    int no_rows;// number of rows for the grid
    printf("Enter number of rows for grid:");
    scanf("%d",&no_rows);

    int no_cols;// number of columns for the grid
    printf("Enter number of columns for grid:");
    scanf("%d",&no_cols);
    
    printf("\n");
    int grid[no_rows][no_cols];
    // input 1 for clear path or 0 for an obstacle
    printf("Enter the Grid(0-obstacle / 1-clear path):\n");
    for(int i=0;i<no_rows;i++)
    {
        for(int j=0;j<no_cols;j++)
        {
            scanf("%d",&grid[i][j]);
        }
    }
    
    printf("\n");
    //printing the inputed grid
    printf("The grid entered by you is as follows:\n");
    for(int i=0;i<no_rows;i++)
    {
        for(int j=0;j<no_cols;j++)
        {
            printf("|");
           printf("%d",grid[i][j]); 
           printf("|");
        }
        printf("\n");
    }
    
    printf("\n");
    int srcrow;// Enter source node
    int srccol;
    printf("Enter the row for source node:");
    scanf("%d",&srcrow);
    printf("Enter the col for source node:");
    scanf("%d",&srccol);
    printf("\n");
    
    bool x=isValid(srcrow,srccol,no_rows,no_cols);//to check that the provided source node exist on the grid or not
    
    int tarrow;// Enter target node
    int tarcol;
    printf("Enter the row for target node:");
    scanf("%d",&tarrow);
    printf("Enter the col for target node:");
    scanf("%d",&tarcol);
    printf("\n");
    
    bool y=isValid(tarrow,tarcol,no_rows,no_cols);//to check that the provided target node exist on the grid or not

    printf("                        OUTPUT\n");
    
    if(x==false || y==false)
    {
        if(x==false && y==false)
        {
            printf("Both source node and target node are not valid\n");
        }
        else if(x==false || y==true)
        {
            printf("Source node is not valid\n");
        }
        else
        {
            printf("Target node is not valid\n");
        }
    }
    else// if all nodes are valid
    {
       PathFinder(no_rows,no_cols,grid,srcrow,srccol,tarrow,tarcol); 
    }
    return 0;
}
