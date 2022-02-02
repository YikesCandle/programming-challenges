#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <map>

// zadání 15puzzle
uint8_t board[4][4] = {
        {0,  9,   2,  3},
        {12,  1,   6,  8},
        {7,  5, 10, 4},
        {13, 14, 11,  15}};

// na úkor přesnosti vyřeší rychleji
int speedModifier = true; //tento často najde nekratší trasu

#define MEMORYLIM 1000 // MB


// kdyby to pořád nešlo
int speedUpgrade = true;

// ...
int megaSpeedUpgrade = false;



using namespace std;

typedef long long Id;
#define END_ID 1311768467463790320

class Node
{
    public:
    Id      parent;
    int     gCost;    
    int     hCost;
    uint8_t x;      
    uint8_t y;
    uint8_t board[4][4];

    Node(uint8_t board[][4], int x, int y);
    Node(uint8_t board[][4]);
    Node() : parent(77), gCost(INT32_MAX), hCost(__INT32_MAX__) { };
    int h_cost() const;
    int h_cost2() const;
    inline int f_cost() const;
    void print() const;
    inline Id get_id() const;
    inline bool operator==(const Node &B) const;
};

Node::Node(uint8_t board[][4], int x, int y)
: parent(77), gCost(INT32_MAX), hCost(__INT32_MAX__), x(x), y(y)
{
    for (int i = 0; i < 4; ++i) 
    for (int j = 0; j < 4; ++j) 
        this->board[i][j] = board[i][j];
}

Node::Node(uint8_t board[][4])
: parent(77), gCost(INT32_MAX), hCost(__INT32_MAX__)
{
    for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
    {
        if (board[i][j] == 0)
        {
            this->x = j; this->y = i;
        }
        this->board[i][j] = board[i][j];
    }
}

inline Id Node::get_id() const
{
    Id id = 0;
    for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
    {
        id <<= 4;
        id += this->board[i][j];
    }
    return id;
}

inline bool Node::operator==(const Node &B) const
{
    return this->get_id() == B.get_id();
}

int Node::h_cost() const
{
    if (this->hCost != __INT32_MAX__) return this->hCost;
    int correctX, correctY, total = 0, num;
    for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
    {
        num = this->board[i][j];
        if (num == 0)
            continue;
        else
            { correctX = --num % 4; correctY = num / 4; }
        int add = (abs(correctX - j) + abs(correctY - i));
        total += add;
        
        if (speedModifier && add == 0) 
        {
            if (megaSpeedUpgrade)
                total -= 100;
            else if (speedUpgrade)
                total -= 5;
            else
                total -= 1;
        }
        
    }
    return total;
}

inline int Node::f_cost() const
{
    return this->h_cost() + this->gCost; 
}

void Node::print() const
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            cout << setw(3) << int(this->board[i][j]);
        }
        cout << endl;
    }
}

bool is_solvable(Node &start)
{
    int invertedPieces = 0;
    for (int i = 0; i < 15; ++i)
    {
        if (start.board[i / 4][i % 4] < start.board[(i + 1) / 4][(i + 1) % 4])
            invertedPieces++;
    }
    cout << invertedPieces << endl;
    cout << start.y << endl;
    if (start.y % 2 == 0 && invertedPieces % 2 == 0)
        return false;
    if (start.y % 2 == 1 && invertedPieces % 2 == 1)
        return false;
    return true;
}

struct NodeId
{
    Id id;
    int fCost;
};

map<Id, Node> allNodes;
int loops = 0;
bool compare_nodes(const NodeId &a, const NodeId &b)
{
    if (a.fCost < b.fCost) return false;
    return true;
}

bool ignoreMemorylim = false;
void solve(Node &start)
{
    if (!is_solvable(start))
    {
        cout << "Unsolvable." << endl;
        return;
    }
    loops = 0;
    int mc = 0;
    int moveX[4] = {0, 0, 1, -1};
    int moveY[4] = {1, -1, 0, 0};
    vector<NodeId> openSet;

    start.gCost = 0;
    allNodes[start.get_id()] = start;
    openSet.push_back({start.get_id(), start.f_cost()});

    while (!openSet.empty())
    {
        loops++;
        pop_heap(openSet.begin(), openSet.end(), compare_nodes);
        NodeId minId = openSet.back();
        openSet.pop_back();

        Node & current = allNodes[minId.id]; 
        Id cId = current.get_id();
        if (cId == END_ID)
        {
            cout << endl << "END OF PATH" << endl;
            current.print();
            int count = 0;
            while (current.get_id() != start.get_id())
            {
                current = allNodes[current.parent];
                current.print();
                cout << endl;
                count ++;
            }
            cout << "^^^ PATH ^^^\n" << endl;
            cout << "Path length\t= " << count << endl;
            return;
        }
        if (mc++ % 20000 == 0)
        {
            cout << "\nIteration number:\t" << (mc - 1) / 1000  << "k" << endl;
            cout << "Open set size:\t\t" << openSet.size() / 1000 << "k" << " (" << openSet.size() * sizeof(Id) * 14 / (1024*1024*8) << " MB)\n";
            cout << "Number of visited nodes:" <<  allNodes.size() / 1000 << "k" << " (" << allNodes.size() * (sizeof(Id) + sizeof(Node)) * 14 / (1024*1024*8) << " MB)\n";
            cout << "Current best node:\n";
            current.print();
            cout << "hCost = " << current.h_cost() << endl;
            cout << "gCost = " << current.gCost << endl;
            if (!ignoreMemorylim && openSet.size() * sizeof(Id) * 14 / (1024*1024*8) + allNodes.size() * (sizeof(Id) + sizeof(Node)) * 14 / (1024*1024*8) > MEMORYLIM)
            {
                int input;
                cout << "Program překročil limit využívané paměti. Chcete pokračovat?" << endl;
                cout << "(1 == pokračovat)\n: " << endl;
                cin >> input;
                if (input != 1)
                {
                    cout << "Možná zkuste speedModifier." << endl;
                    return;
                }
                else ignoreMemorylim = true;
            }
        }
        for (int i = 0; i < 4; ++i)
        {
            Node newCurrent = current;
            newCurrent.x = current.x + moveX[i];
            newCurrent.y = current.y + moveY[i];

            if (newCurrent.x < 0 || newCurrent.y < 0 || newCurrent.x >= 4 || newCurrent.y >= 4)
                continue;
            
            swap(newCurrent.board[current.y][current.x], newCurrent.board[newCurrent.y][newCurrent.x]);

            Id nCid = newCurrent.get_id();
            map<Id, Node>::iterator nCit = allNodes.find(nCid);
            bool visited = (nCit != allNodes.end());
            if (visited) newCurrent = nCit->second;

            
            
            if ( !visited || newCurrent.gCost > current.gCost + 1)
            {
                if (!visited)
                {
                    Node *tmp = &allNodes[nCid];
                    *tmp = newCurrent;
                    tmp->parent = cId;
                    tmp->gCost = current.gCost + 1;
                    openSet.push_back({nCid, tmp->f_cost()});
                    push_heap(openSet.begin(), openSet.end(), compare_nodes);
                }
                Node & nodeTmp = nCit->second;
                nodeTmp.parent = cId;
                nodeTmp.gCost = current.gCost + 1;
                openSet.push_back({nCid, nCit->second.f_cost()});
                push_heap(openSet.begin(), openSet.end(), compare_nodes);
            }
        }
    }
}


int main()
{    
    Node start(board);
    solve(start);
    cout << "Nodes visited\t= "<< loops << endl;
}
