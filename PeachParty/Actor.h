#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

/*
Hierarchy:

 Actor
    Creature
        Player
        Baddie
            Bowser
            Boo
    Squares
        ...Various Squares
    Vortex
 
*/

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth, double size): GraphObject(imageID, startX, startY, dir, depth, size), m_world(world), isActive(true)  {}
    virtual void doSomething()=0;
    StudentWorld* getWorld() {return m_world;}
    virtual bool isMobile()=0;
    virtual bool vortexImpactable()=0;
    virtual void vortexHit()=0;
    bool isActive;
    
private:
    StudentWorld* m_world;
};

//---------------------------Creatures-------------------------------

class Creature: public Actor
{
public:
    Creature(StudentWorld* world, int imageID, int startX, int startY): Actor(world, imageID, startX, startY, right, 0, 1.0), walkDir(right) {}
    virtual void doSomething()=0;
    virtual bool isMobile() {return true;}
    void autoWalk();
    void teleport();
    int walkDir;
    void setWalkDir(int dir);
};

class Player: public Creature
{

public:
    Player(StudentWorld* world, int imageID, int startX, int startY, int num): Creature(world, imageID, startX, startY), m_coins(0), m_stars(0), m_vortex(false), player_num(num), ticks_to_move(0), walking(0) {}
    virtual void doSomething();
    virtual bool vortexImpactable() {return false;}
    virtual void vortexHit() {return;}
    
    int countCoins() const {return m_coins;}
    int countStars() const {return m_stars;}
    int countRolls() const {return ticks_to_move/8;}
    bool hasVortex() const {return m_vortex;}
    bool isWalking() const {return walking;}
    
    void addCoins(int c)    {m_coins+=c;}
    void addStars(int s)    {m_stars+=s;}
    void deductCoins(int c) {if(m_coins<c) m_coins=0;  else m_coins-=c;}
    void giveVortex()   {m_vortex=true;}
    void useVortex()    {m_vortex=false;}
    
    void swapPos();
    void swapCoins();
    void swapStars();
    
    void playerTeleport();
    void playerWalk();
    
private:
    int m_coins;
    int m_stars;
    bool m_vortex;
    bool walking;
    int player_num;
    int ticks_to_move;

};

//---------------------------Baddies-------------------------------

class Baddie: public Creature
{
public:
    Baddie(StudentWorld* world, int imageID, int startX, int startY): Creature(world, imageID, startX, startY), travel_dist(0), walking(0), pause_counter(180), ticks_to_move(0), peachHere(false), yoshiHere(false) {}
    virtual void doSomething()=0;
    virtual void doTheThingToPlayer(Player* p, bool& here)=0;
    virtual bool vortexImpactable() {return true;}
    virtual void vortexHit();

    bool didThePlayerJustLandOnMe(Player* p);
    void baddieWait();
    void baddieWalk();

protected:
    int travel_dist;
    int pause_counter;
    int ticks_to_move;
    bool walking;
    bool peachHere;
    bool yoshiHere;
};

class Bowser: public Baddie
{
public:
    Bowser(StudentWorld* world, int startX, int startY): Baddie(world, IID_BOWSER, startX, startY) {}
    virtual void doSomething();
    virtual void doTheThingToPlayer(Player* p, bool& here);
};

class Boo: public Baddie
{
public:
    Boo(StudentWorld* world, int startX, int startY): Baddie(world, IID_BOO, startX, startY) {}
    virtual void doSomething();
    virtual void doTheThingToPlayer(Player* p, bool& here);
};

//---------------------------Squares-------------------------------

class Square: public Actor
{
public:
    Square(StudentWorld* world, int imageID, int startX, int startY, int dir): Actor(world, imageID, startX, startY, dir, 1, 1.0), peachHere(false), yoshiHere(false) {}
    virtual void doSomething();
    virtual void doTheThingToPlayer(Player* p, bool& here)=0;
    virtual bool isMobile() {return false;}
    virtual bool vortexImpactable() {return false;}
    virtual void vortexHit() {return;}
    bool didThePlayerJustLandOnMe(Player* p);
    bool didThePlayerJustWalkPastMe(Player* p);
    
protected:
    bool peachHere;
    bool yoshiHere;
};

class CoinSquare: public Square
{
public:
    CoinSquare(StudentWorld* world, int imageID, int startX, int startY): Square(world, imageID, startX, startY, right), m_addcoin(0)
    {
        if(imageID == IID_BLUE_COIN_SQUARE)
            m_addcoin = 3;
        if(imageID == IID_RED_COIN_SQUARE)
            m_addcoin = -3;
    }
    virtual void doTheThingToPlayer(Player* p, bool& here);
    
private:
    int m_addcoin;
};

class StarSquare: public Square
{
public:
    StarSquare(StudentWorld* world, int startX, int startY): Square(world, IID_STAR_SQUARE, startX, startY, right) {}
    virtual void doTheThingToPlayer(Player* p, bool& here);

};

class DirectionalSquare: public Square
{
public:
    DirectionalSquare(StudentWorld* world, int startX, int startY, int dir): Square(world, IID_DIR_SQUARE, startX, startY, dir) {}
    virtual void doTheThingToPlayer(Player* p, bool& here);

};

class BankSquare: public Square
{
public:
    BankSquare(StudentWorld* world, int startX, int startY): Square(world, IID_BANK_SQUARE, startX, startY, right), m_balance(0) {}
    virtual void doTheThingToPlayer(Player* p, bool& here);
private:
    int m_balance;
};

class EventSquare: public Square
{
public:
    EventSquare(StudentWorld* world, int startX, int startY): Square(world, IID_EVENT_SQUARE, startX, startY, right) {}
    virtual void doTheThingToPlayer(Player* p, bool& here);

};

class DroppingSquare: public Square
{
public:
    DroppingSquare(StudentWorld* world, int startX, int startY): Square(world, IID_DROPPING_SQUARE, startX, startY, right) {}
    virtual void doTheThingToPlayer(Player* p, bool& here);
    
};

//---------------------------Vortex-------------------------------

class Vortex: public Actor
{
public:
    Vortex(StudentWorld* world, int startX, int startY, int dir): Actor(world, IID_VORTEX, startX, startY, dir, 0, 1.0) {}
    virtual void doSomething();
    virtual bool isMobile() {return true;}
    virtual bool vortexImpactable() {return false;}
    virtual void vortexHit() {return;}
    bool determineOverlap(int x1, int y1, int x2, int y2);
};

#endif // ACTOR_H_
