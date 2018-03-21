#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <string>
#include <mutex>
#include <vector>
#include <condition_variable>
using namespace std;

const int KEY_Q = 113;

static mutex cvMutex;
static condition_variable cv;
static int numberOfBullets = 0;
static bool keyNotPressed = true;

class Bullet
{   
    private:
        
        const char bullets[5] = {'o', 'c', '*', '@', 'x'};
        const char rubber = ' ';
        const pair<int, int> directions[3] = { make_pair(3, 1),
                                               make_pair(2, 1),
                                               make_pair(1, 2)};
        int numberOfBounces;
        char bullet;
        bool passedHalfOnce;
        pair<int, int> direction;
        pair<int, int> currentLocation = make_pair(1, 12);
        
    public:
        void run()
        {   
            unique_lock<mutex> lck(cvMutex);
            passedHalfOnce = false;
            numberOfBullets++;
            numberOfBounces = 0;
            bullet = bullets[rand()%5];
            direction = directions[rand()%3];
            lck.unlock();
            cv.notify_all();

            while(numberOfBounces < 3)
            {   
                lck.lock();
                cv.wait(lck, [](){return numberOfBullets != 0 || !keyNotPressed;});

                if(currentLocation.first > 12)
                {
                    currentLocation.first = 12;
                    direction.first = -direction.first;
                    numberOfBounces++;
                }
                if(currentLocation.first < 1)
                {
                    currentLocation.first = 1;
                    direction.first = -direction.first;
                    numberOfBounces++;            
                }

                if(currentLocation.second > 12)
                {
                    currentLocation.second = 12;
                    direction.second = -direction.second;
                    numberOfBounces++;
                }
                if(currentLocation.second < 1)
                {
                    currentLocation.second = 1;
                    direction.second = -direction.second;
                    numberOfBounces++;
                } 
                
                if(currentLocation.second <= 6 && !passedHalfOnce)
                {
                    numberOfBullets--;
                    passedHalfOnce = true;
                }

                if(currentLocation.second > 7 && numberOfBounces > 0)
                {
                    if(passedHalfOnce)
                    {    
                        numberOfBullets++;
                        passedHalfOnce = false;
                        cv.notify_all();
                        
                    }
                    else
                        if(numberOfBounces == 3)
                        {
                            numberOfBullets--;  
                        }
                                              
                }

                mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", bullet);
                refresh();
                lck.unlock();

                this_thread::sleep_for(chrono::milliseconds(400)); 

                lck.lock();
                mvwprintw(stdscr, currentLocation.second, currentLocation.first, "%c", 
                                                          currentLocation.second == 7 ? '-' : rubber);
                currentLocation.first += direction.first;
                currentLocation.second -= direction.second;
                lck.unlock();
            }
        } 
};

class Threads
{   
    public:
        Threads(){}

        void run()
        {   
            Bullet bullet;
            vector<thread> threads; 

            while(keyNotPressed)
            {   
                threads.push_back(thread(&Bullet::run, bullet)); 
                this_thread::sleep_for(chrono::milliseconds(4000));
                keyNotPressed = (getch() != KEY_Q);
            }
            cv.notify_all();

            for(auto& t : threads)
                t.join();
        }
};

class Arena
{   
    public:
        Arena(){}

        void drawArena()
        {   
            start_color();
            init_pair(1,0,0);
            attrset(COLOR_PAIR(0));  

            for(int i = 0; i < 13; i++)
            {
                mvprintw( 0, i, "*");
                mvprintw( 13, i, "*");
                mvprintw( 7, i, "-");
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
    arena->drawArena();

    Threads *threads = new Threads();
    threads->run();

    delete arena;
    delete threads;
    endwin();
    return 0;
}
