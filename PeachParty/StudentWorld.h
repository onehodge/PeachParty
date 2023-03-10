#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld() {cleanUp();}
    
    Board* getBoard() const  {return m_board;}
    Player* getPeach() const {return m_peach;}
    Player* getYoshi() const {return m_yoshi;}
    vector<Actor*> getActors() const {return actors;}
    void addActor(Actor *actor) {actors.push_back(actor);}
    
    int getBankBalance() const  {return m_balance;}
    void depositIntoBank(int c) {m_balance += c;}
    void withdrawFromBank()     {m_balance = 0;}
    
    Player* getOtherPlayer(Player* p);
    bool isSquare(int x, int y);
    bool isFork(int x, int y);
    int validActionAtFork(int action, int dir, int x, int y);
    bool isDirectionalSquare(int x, int y, int& dir);
    

private:
    Player* m_peach;
    Player* m_yoshi;
    vector<Actor*> actors;
    Board* m_board;
    int m_balance;
    
};

#endif // STUDENTWORLD_H_
