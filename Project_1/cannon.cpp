#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <string>
#include <mutex>
using namespace std;

void drawSquare();
void run();

mutex myMutex;
bool keyPressed = false;

int main(void)
{   
    srand(time(NULL));
    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();
    drawSquare();
    
    run();

    endwin();
    return 0;
}

struct Bullet
{   
    void run()
    {   
        int ch;
        

        // Możliwe początkowe kierunki poruszania się pocisku:
        // [3, 1]
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
        const char bullets[] = {'o', 'c', '*', '0', 'O'};
        const char rubber = ' ';
        const pair<int, int> directions[] = { make_pair(3, 1),
                                              make_pair(2, 1),
                                              make_pair(1, 2)};
        //myMutex.lock();
        pair<int, int> direction = directions[rand()%3];
        pair<int, int> currentLocation = make_pair(1, 12);
        int numberOfBounces = 0;
        char bullet = bullets[rand()%5];

        //myMutex.unlock();

        while(numberOfBounces < 3)
        {   
            myMutex.lock();
            //debug //////////////////
            mvwprintw(stdscr, 18 , 0, "Watek:[%d](%c) bounces: {%d}", 
                                         this_thread::get_id(), bullet, numberOfBounces);
            //////////////////////////
            mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", bullet);
            refresh();
            myMutex.unlock();

            this_thread::sleep_for(std::chrono::milliseconds(400)); 

            //myMutex.lock();
            mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", rubber);
           // myMutex.unlock();

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
            
            // Sprawdzenie czy klawisz został wciśnięty
            // 113 = 'q'
            if((ch = getch()) == 113)
            {
                keyPressed = true;
                return;
            }
        }
        //debug //////////////////
        //myMutex.lock();
        mvwprintw(stdscr, 19, 0, "%s:[%d](%c) %s", "Watek ", 
                                         this_thread::get_id(), bullet, "zakonczyl prace!");
        //  myMutex.unlock();
        //////////////////////////
    }    
};

void run()
{   
    Bullet bullet;
    int returnValue;

    while(keyPressed == false)
    {
        thread bulletThread(&Bullet::run, bullet);

        bulletThread.join();
    }
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