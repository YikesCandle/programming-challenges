#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <unistd.h>

/*

Usage: ./a.out N -p -c   (in this order)

 N  : nuber of queens (int) (with -O2 it can solve up to n = 100 000)
-p  : prints result   (optional) (can print only n<=100)
-c  : fancy output    (optional, works only with -p flag) (can print only n<=50)


Compile:
e.g.   g++ -std=c++14 -Wall -pedantic -O2 main.cpp

use -O2 for better performance

*/
using namespace std;

void print_board(vector<int> &, bool);
bool p = false, c = false;
#define CLEAR "\033[2J"
#define HOME_ROW "\033[H"

// ========================== MACROS =============================
// updates diagonals after adding or deleting queen
    // noval doesn't recalculate any hValue (min value for hill-climbing)
    #define add_queen(i, j, value)                          \
        {                                                   \
            if (d1[i + candidate[j]]++) value++;            \
            if (d2[n - 1 - i + candidate[j]]++) value++;    \
        }
    #define del_queen(i, j, value)                          \
        {                                                   \
            if (--d1[i + candidate[j]]) value--;            \
            if (--d2[n - 1 - i + candidate[j]]) value--;    \
        }
    #define add_queen_noval(i, j) \
        d1[i + candidate[j]]++; d2[n - 1 - i + candidate[j]]++
        
    #define del_queen_noval(i, j) \
        --d1[i + candidate[j]]; --d2[n - 1 - i + candidate[j]]

    // updates diagonal after swaping two queens
    // noval doest change any value
    #define update_diag(i, j, value)                    \
        del_queen(i, i, value); del_queen(j, j, value); \
        add_queen(i, j, value); add_queen(j, i, value)

    #define update_diag_noval(i, j)                     \
        del_queen_noval(i, i); del_queen_noval(j, j);   \
        add_queen_noval(i, j); add_queen_noval(j, i)
// ========================== MACROS =============================


vector<int> solve_nqueens(int n, int maxIter = INT32_MAX)
{
    // solution below n=4 does not exist
    if (n < 4)
        return vector<int>();

    // create candidate
    vector<int> candidate;
    for (int i = 0; i < n; ++i)
        candidate.push_back(i);

    // randomize candidate
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(candidate.begin(), candidate.end(), default_random_engine(seed));

    // init diagonals
    int min = 0, tmp;
    vector<int> d1(n * 2 - 1, 0), d2(n * 2 - 1, 0);
    for (int i = 0; i < n; ++i)
        add_queen(i, i, min);

    // hill-climbing
    for (int sth = 0; sth < maxIter; ++sth)
    {
        // randomizer, escape from local min
        int i = rand() % n, j = rand() % n;
        if (j == i) j = (j + 1) % n;
        update_diag(i, j, min);
        swap(candidate[i], candidate[j]);
        
        for (int i = 0; i < n; ++i)
        {
            // if no colisions, continue
            if (d1[i + candidate[i]] == 1 && d2[n - 1 - i + candidate[i]] == 1)
                continue;

            for (int j = i + 1; j < n; ++j)
            {
                // modify candidate
                tmp = min;
                update_diag(i, j, tmp);
                swap(candidate[i], candidate[j]);

                if (tmp <= min)
                {
                    // new candiate is good
                    print_board(candidate, c);
                    min = tmp;
                }
                else
                {
                    // new candidate is bad, change it back
                    update_diag_noval(i, j);
                    swap(candidate[i], candidate[j]);
                }
                // solution was found
                if (!min) return candidate;
                    
            }
        }
    }
    // max iter was reached, return last candidate
    return candidate;
}

//                   end of algorithm
// ================================================================
// ================================================================

void print_board(vector<int> & board, bool color = false)
{
    if (board.size() * (int(c) + 1) > 120)
        return;
    usleep(150000);
    cout << CLEAR << HOME_ROW;
    for (size_t i = 0; i < board.size() * (int(c) + 1); ++i)
        cout << '-';
    cout << endl;
    int n = board.size();
    if (!n)
    {
        cout << "No solution." << endl;
        return;
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (color)
            {
                if ((i + j) % 2)
                    cout << "\033[48;5;255;1;30m";
                else
                    cout << "\033[48;5;232;1;37m";
                if (board[i] == j)
                    cout << "QE";
                else
                    cout << "  ";
            }
            else
            {
                if (board[i] == j)
                    cout << "Q";
                else
                    cout << ".";
            }
            
        }
        if (color) cout << "\033[0;0m";
        cout << endl;
    }
    if (color) cout << "\033[0;0m";
    cout << endl;
}

inline void usage()
{
    cout << "Usage: ./a.out N -p -c (in this order)" << endl << endl;
    cout << " N\t: nuber of queens (int)" << endl;
    cout << "-p\t: prints result (optional) (can print only n<=100)" << endl;
    cout << "-c\t: fancy output (optional) (can print only n<=50)" << endl;
}

int main(int args, char **argv)
{
    if (args < 2 || args > 4)
    {
        usage();
        return 1;
    }
    // flags
    if (args == 3 || args == 4)
    {
        if (strcmp(argv[2], "-p"))
        {
            usage();
            return 1;
        }
        p = true;
        if (args == 4)
        {
            if (strcmp(argv[3], "-c"))
            {
                usage();
                return 1;
            }
            c = true;
        }
    }
    srand(time(nullptr));

    int n = stoi(argv[1]);
    unsigned first = chrono::system_clock::now().time_since_epoch().count();
    vector<int> result = solve_nqueens(n);
    unsigned last = chrono::system_clock::now().time_since_epoch().count();

    if (p) 
    {
        if (n * (int(c) + 1) > 120)
            cout << "Too large to be printed." << endl;
        else
            print_board(result, c);
    }
    else
    {
        if (!result.size())
        {
            cout << "No solution." << endl;
            return 0;
        }
    }
    cout << "Solved in " << floorf((last - first) / 1000) / 1000 << " seconds." << endl;

    return 0;
    
}
