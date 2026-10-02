#include <iostream>
using namespace std;
int currentPlayer;
char currentMarker;
char board[3][3]={{'1','2','3'},{'4','5','6'},{'7','8','9'}};
void drawBoard()
{
    cout<<" "<<board[0][0]<<" | "<<board[0][1]<<" | "<<board[0][2]<<" \n";
    cout<<"---|---|---\n";
    cout<<" "<<board[1][0]<<" | "<<board[1][1]<<" | "<<board[1][2]<<" \n";
    cout<<"---|---|---\n";
    cout<<" "<<board[2][0]<<" | "<<board[2][1]<<" | "<<board[2][2]<<" \n";
    cout<<"---|---|---\n";
}



void game()
{
    cout<<"Player1 Choose Your Marker :  X or O";
    char markerP1;
    cin>>markerP1;

    currentPlayer=1;
    currentMarker= markerP1;
    drawBoard();
}


int main()
{
    game();
    return 0;
}