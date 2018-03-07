#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <string>
#include <mutex>
#include <vector>
using namespace std;

bool keyPressed = false;
mutex tMutex;

class Bullet
{   
    private:
        const int KEY_Q = 113;
        const char bullets[5] = {'o', 'c', '*', '0', 'O'};
        const char rubber = ' ';
        const pair<int, int> directions[3] = { make_pair(3, 1),
                                               make_pair(2, 1),
                                               make_pair(1, 2)};
        int ch;
        int numberOfBounces;
        char bullet;
        pair<int, int> direction;
        pair<int, int> currentLocation = make_pair(1, 12);
        
    public:
        void run()
        {   
            tMutex.lock();  
            numberOfBounces = 0;
            bullet = bullets[rand()%5];
            direction = directions[rand()%3];
            tMutex.unlock();

            while(numberOfBounces < 3)
            {   
                tMutex.lock();
                mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", bullet);
                refresh();
                tMutex.unlock();

                this_thread::sleep_for(std::chrono::milliseconds(400)); 

                tMutex.lock();
                mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", rubber);
                
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
                if((ch = getch()) == KEY_Q)
                {   
                    keyPressed = true;
                    tMutex.unlock();
                    return;
                }
                tMutex.unlock();
            }
        }    
};

class Threads
{   
    public:
        Threads()
        {
            run();
        }
    private:
        void run()
        {   
            Bullet bullet;
            vector<thread> threads;        
            while(!keyPressed)
            {   
                threads.push_back(thread(&Bullet::run, bullet)); 
                this_thread::sleep_for(chrono::milliseconds(1000));
            }

            for(int i = 0; i < threads.size(); i++)
                threads.at(i).join();
        }
};

class Arena
{   
    public:
        Arena()
        {
            drawArena();
        }
    private:
        void drawArena()
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
};

int main(void)
{   
    srand(time(NULL));
    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();
    Arena *arena = new Arena();
    Threads *threads = new Threads();

    delete arena;
    delete threads;
    endwin();
    return 0;
}
