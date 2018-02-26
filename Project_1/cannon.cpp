#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <string>
using namespace std;

void drawSquare();
void bullet(int dimensions[]);

int main(void)
{   
    srand(time(NULL));
    initscr();
    curs_set(0);
    drawSquare();
    int dimensions[] = { getmaxx(stdscr), getmaxy(stdscr) };
    // włączenie strzałek itd.
    // keypad( stdscr, TRUE );
    // wyłączenie ctrl+c, ctrl+z ...
    // raw();
    thread first (bullet, dimensions);
    first.join();
    getch();

    endwin();
    return 0;
}

void drawSquare()
{   
    start_color();
    init_pair(1,0,0);
    attrset(COLOR_PAIR(1));  

    for(int i = 0; i < 13; i++)
    {
        mvprintw( 0, i, "*");
        mvprintw( 13, i, "*");
    } 
    
    for(int i = 0; i < 14; i++)
    {
        mvprintw( i, 0, "*");
        mvprintw( i, 13, "*");
    } 
    attrset(use_default_colors()); 
}

void bullet(int dimensions[])
{   
    // Możliwe początkowe kierunki poruszania się pocisku:
    // [1, 1]
    // [2, 1]
    // [1, 2]
    //
    //   (0,0)         (11,0)  
    //      ++++++++++++++
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      +            +
    //      ++++++++++++++
    //   (0,11)        (11,11)

    pair<int, int> directions[] = { make_pair(3, 1),
                                    make_pair(2, 1),
                                    make_pair(1, 2)};
    pair<int, int> direction = directions[rand()%3];
    pair<int, int> currentLocation = make_pair(1, 12);
    int numberOfBounces = 0;
    char bullet[] = "o";
    char rubber[] = " ";
    
    for(int i = 0; i < 50; i++)
    {   
        mvprintw(currentLocation.second, currentLocation.first, bullet);
        refresh();
        this_thread::sleep_for(std::chrono::milliseconds(500)); 
        mvprintw( 15, 0, ("direction = [" + to_string(direction.first) +  ", " 
                                          + to_string(direction.second) 
                                          + "]").c_str());  
        mvprintw( 16, 0, (("bounces: " + to_string(numberOfBounces)).c_str()));  
        mvprintw( currentLocation.second, currentLocation.first, rubber);

        if(numberOfBounces>=3)
            break;
        
        currentLocation.first += direction.first;
        currentLocation.second -= direction.second;

        if(currentLocation.first >= 12)
        {
            currentLocation.first = 12;
            direction.first = -direction.first;
            numberOfBounces++;
        }
        if(currentLocation.first <= 1)
        {
            currentLocation.first = 1;
            direction.first = -direction.first;
            numberOfBounces++;
        }

        if(currentLocation.second >= 12)
        {
            currentLocation.second = 12;
            direction.second = -direction.second;
            numberOfBounces++;
        }
        if(currentLocation.second <= 1)
        {
            currentLocation.second = 1;
            direction.second = -direction.second;
            numberOfBounces++;
        } 
        
        
    }
    mvprintw( 17, 0, "Watek zakonczyl prace!" );  
}