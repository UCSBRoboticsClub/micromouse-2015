//
//  main.cpp
//  Micromouse
//
//  Created by Gauthier Dieppedalle on 12/17/14.
//  Copyright (c) 2014 Gauthier Dieppedalle. All rights reserved.
//

#include <iostream>
#include <stack>

using namespace std;

#define NORTH      1
#define EAST       2
#define SOUTH      3
#define WEST       4

#define row 16
#define column 16

struct wall{
    int north;
    int east;
    int south;
    int west;
};

struct cell{
    int x;
    int y;
};

void printMaze(struct wall maze[row][column]){
    std::cout << "MAZE SHAPE:\n";
    for(int i = 0; i <= row; i++){
        std::cout << "_ ";
    }
    std::cout << std::endl;
    for (int i = 0; i <column; i++){
        for (int j = 0; j <row; j++){
            if(maze[i][j].west == 1){
                std::cout << '|';
            }
            else{
                std::cout << " ";
            }
            
            if(maze[i][j].south == 1){
                std::cout << "_";
            }
            else{
                std::cout << " ";
            }
        }
        
        std::cout << "|" << std::endl;
        
    }
    std::cout << std::endl;
}

void printDistance(int distance[row][column]){
    std::cout << "MAZE DISTANCE:\n";
    
    for (int i = 0; i <column; i++){
        for (int j = 0; j <row; j++){
            if (distance[i][j]<=9 && distance[i][j]>=0) {
                std::cout << distance[i][j] << "  ";
            }
            else{
                std::cout << distance[i][j] << " ";
            }
        }
        std::cout << std::endl;
    }
}


void updateDistanceValues(int x, int y, struct wall wallMap[row][column], int distance[row][column]){
    //This stack stores the cells that the micromouse went on
    stack <cell> cellsWent;
    
    //Make sure the stack is empty
    while (cellsWent.size() != 0) {
        cellsWent.pop();
    }
    
    //Push the current cell to the stack
    cellsWent.push({x,y});
    
    //Minimum Value of its open neighbor
    int minValue;
    bool minValueObtain=false;
    
    int tempx;
    int tempy;
    
    while (cellsWent.size() != 0) {
        
        //Current Cells
        tempx=cellsWent.top().x;
        tempy=cellsWent.top().y;
        
        
        cout << "Element at the top of stack: x="<< tempx << " y=" << tempy <<endl;
        
        
        minValueObtain=false;
        //Takes the minimum value of its open neighbor
        if (tempy>0 && wallMap[tempy][tempx].north==0){
            if (minValueObtain==false){
                minValue=distance[tempy-1][tempx];
            }
            else {
                if (minValue>=distance[tempy-1][tempx]) {
                    minValue=distance[tempy-1][tempx];
                }
            }
            minValueObtain=true;
            cout <<"minValue is north \n";
        }
        
        if (tempy<16 && wallMap[tempy][tempx].south==0){
            
            if (minValueObtain==false){
                minValue=distance[tempy+1][tempx];
            }
            else {
                if (minValue>=distance[tempy+1][tempx]) {
                    minValue=distance[tempy+1][tempx];
                }
            }
            minValueObtain=true;
            
        }
        
        if (tempx<16 && wallMap[tempy][tempx].east==0){
            if (minValueObtain==false){
                minValue=distance[tempy][tempx+1];
            }
            else {
                if (minValue>=distance[tempy][tempx+1]) {
                    minValue=distance[tempy][tempx+1];
                }
            }
            minValueObtain=true;
            cout <<"minValue is east \n";
        }
        
        if (tempx>0 && wallMap[tempy][tempx].west==0){
            if (minValueObtain==false){
                minValue=distance[tempy][tempx-1];
            }
            else {
                if (minValue>=distance[tempy][tempx-1]) {
                    minValue=distance[tempy][tempx-1];
                }
            }
            minValueObtain=true;
            cout <<"minValue is west \n";
        }
        
        
        cout << "The min open neighbor value is "<< minValue << endl;
        if ((distance[tempy][tempx]-1 != minValue)){
            
            
            distance[tempy][tempx]=minValue+1;
            cellsWent.pop();
            if (y>0 && wallMap[tempy][tempx].north==0){
                cellsWent.push({tempx,tempy-1});
                cout << 1 << endl;
            }
            if (y<16 && wallMap[tempy][tempx].south==0){
                cellsWent.push({tempx,tempy+1});
                cout << 2 << endl;
            }
            if (x<16 && wallMap[tempy][tempx].east==0){
                cellsWent.push({tempx+1,tempy});
                cout << 3 << endl;
            }
            if (x>0 && wallMap[tempy][tempx].west==0){
                cellsWent.push({tempx-1,tempy});
                cout << 4 << endl;
            }
        }
        else{
            cellsWent.pop();
        }
        printDistance(distance);
    }
}

void updateWallMap(int x, int y, struct wall maze[row][column], struct wall wallMap[row][column]){
    wallMap[y][x].north=maze[y][x].north;
    wallMap[y][x].east=maze[y][x].east;
    wallMap[y][x].south=maze[y][x].south;
    wallMap[y][x].west=maze[y][x].west;
}


struct cell neighborCellLowestDistance(int x, int y, struct wall wallMap[row][column], int distance[row][column]){
    int minValue;
    bool minValueObtain=false;
    struct cell minCell;
    
    //Takes the minimum value of its open neighbor
    if (y>0 && wallMap[y][x].north==0){
        if (minValueObtain==false){
            minValue=distance[y-1][x];
            minCell.x=x;
            minCell.y=y-1;
        }
        else {
            if (minValue>distance[y-1][x]) {
                minValue=distance[y-1][x];
                minCell.x=x;
                minCell.y=y-1;
            }
        }
        minValueObtain=true;
        
    }
    
    if (y<16 && wallMap[y][x].south==0){
        if (minValueObtain==false){
            minValue=distance[y+1][x];
            minCell.x=x;
            minCell.y=y+1;
        }
        else {
            if (minValue>distance[y+1][x]) {
                minValue=distance[y+1][x];
                minCell.x=x;
                minCell.y=y+1;
            }
        }
        minValueObtain=true;
        
        }
        
        if (x<16 && wallMap[y][x].east==0){
            if (minValueObtain==false){
                minValue=distance[y][x+1];
                minCell.x=x+1;
                minCell.y=y;
            }
            else {
                if (minValue>distance[y][x+1]) {
                    minValue=distance[y][x+1];
                    minCell.x=x+1;
                    minCell.y=y;
                }
            }
            minValueObtain=true;
            
        }
        
        if (x>0 && wallMap[y][x].west==0){
            if (minValueObtain==false){
                minValue=distance[y][x-1];
                minCell.x=x-1;
                minCell.y=y;
            }
            else {
                if (minValue>distance[y][x-1]) {
                    minValue=distance[y][x-1];
                    minCell.x=x-1;
                    minCell.y=y;
                }
            }
            minValueObtain=true;
        }
        return minCell;
        }
        
        
        
        
        int main(int argc, const char * argv[]) {
            
            //Double Array used only for testing purposes
            //Maze Walls is saved in maze
            struct wall maze[row][column]={
                {{1,0,0,1},{1,0,1,0},{1,1,0,0},{1,0,0,1},{1,0,1,0},{1,1,1,0},{1,0,0,1},{1,0,0,0},{1,0,0,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,0,0}},
                {{0,1,0,1},{1,0,0,1},{0,1,1,0},{0,0,0,1},{1,0,1,0},{1,0,0,0},{0,1,0,0},{0,1,1,1},{0,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,0,0},{0,1,0,1}},
                {{0,1,0,1},{0,0,1,1},{1,1,0,0},{0,1,0,1},{1,0,0,1},{0,1,0,0},{0,0,1,1},{1,0,1,0},{0,1,1,0},{1,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0},{0,1,0,1}},
                {{0,1,0,1},{1,0,0,1},{0,1,1,0},{0,0,1,1},{0,1,0,0},{0,1,1,1},{1,0,0,1},{1,0,1,0},{1,0,1,0},{0,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,0,0},{0,1,0,1}},
                {{0,1,0,1},{0,1,0,1},{1,0,0,1},{1,1,0,0},{0,0,1,1},{1,1,1,0},{0,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0},{0,1,0,1}},
                {{0,1,0,1},{0,0,0,1},{0,1,0,0},{0,0,1,1},{1,0,0,0},{1,0,1,0},{0,1,1,0},{1,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0}},
                {{0,0,1,1},{0,1,0,0},{0,0,1,1},{1,1,1,0},{0,0,1,1},{1,0,0,0},{1,1,0,0},{0,1,0,1},{1,1,1,1},{1,1,1,1},{1,1,0,1},{1,1,0,1},{1,1,0,1},{1,1,0,1},{1,1,0,1},{1,1,0,1}},
                {{1,0,0,1},{0,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0},{0,1,0,1},{0,0,0,1},{1,1,0,0},{1,0,1,1},{0,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,1,0,1},{1,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,0,0},{0,1,1,1},{0,0,1,1},{0,1,1,0},{1,0,1,1},{0,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,1,0,1},{0,1,0,1},{1,0,0,1},{1,0,1,0},{1,0,1,0},{0,1,1,0},{1,0,1,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,0,1,1},{0,1,0,0},{0,1,0,1},{1,1,0,1},{1,0,1,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,0}},
                {{1,0,0,1},{0,1,0,0},{0,1,0,1},{0,1,0,1},{1,0,1,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{1,0,1,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,0,0,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,1,0,0},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1},{0,1,0,1}},
                {{0,1,0,1},{0,1,0,1},{0,0,1,1},{0,1,1,0},{1,0,0,1},{1,0,0,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0},{0,1,1,1},{0,1,1,1},{0,1,1,1},{0,1,1,1},{0,1,1,1},{0,1,0,1}},
                {{0,1,1,1},{0,0,1,1},{1,0,1,0},{1,0,1,0},{0,1,1,0},{0,0,1,1},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{1,0,1,0},{0,1,1,0}}};
            
            
            struct wall wallMap[row][column]={
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}},
                {{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2}}};
            
            //Initial position of the micromouse
            int x=0;
            int y=15;
            
            //Distance from center of maze
            int distance[row][column]={
                {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14},
                {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
                {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
                {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
                {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
                {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
                {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
                {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
                {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
                {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
                {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
                {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
                {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
                {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
                {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
                {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14}};
            
            
            
            while ((x!=7 && y!=7) || (x!=7 && y!=8) || (x!=8 && y!=8) || (x!=8 && y!=7)){
                printMaze(maze);
                cout << "The Micromouse is on cell x=" << x << " and y=" << y << "." <<endl;
                printDistance(distance);
                
                updateWallMap(x, y, maze, wallMap);
                updateDistanceValues(x, y, wallMap, distance);
                
                struct cell minCell;
                minCell=neighborCellLowestDistance(x, y, wallMap, distance);
                x=minCell.x;
                y=minCell.y;
            }
            
            
            
            std::cout << "Hello, World!\n";
            return 0;
        }
