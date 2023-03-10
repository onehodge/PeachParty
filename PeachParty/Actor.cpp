#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"

//---------------------------Creature-------------------------------

void Creature::autoWalk()
{
    int direction=walkDir;
    int newX=getX(),newY=getY();
    getPositionInThisDirection(direction, 16, newX, newY);
    if(getWorld()->isSquare(newX, newY)==false)
    {
        if(direction == right || direction == left)
        {
            if(getWorld()->isSquare(getX(), getY()+16))
                direction = up;
            else direction = down;
        }
        else
        {
            if(getWorld()->isSquare(getX()+16, getY()))
                direction = right;
            else direction = left;
        }
        setWalkDir(direction);
    }
}

void Creature::teleport()
{
    int x,y;
    do{
        x=randInt(0, 15); x*=16;
        y=randInt(0, 15); y*=16;
    }while(!getWorld()->isSquare(x,y));
    moveTo(x,y);
}

void Creature::setWalkDir(int dir)
{
    walkDir = dir;
    if(dir==left) setDirection(180);
    else    setDirection(0);
}

//---------------------------Player-------------------------------

void Player::doSomething()
{
    if(walking==0) //waiting to roll
    {
        if(walkDir==1)
        {
            int direction;
            do{
                int randAct = randInt(1,4);
                direction = getWorld()->validActionAtFork(randAct, 1, getX(), getY());
            }while(direction == -1);
            setWalkDir(direction);
        }
        int action = getWorld()->getAction(player_num);
        if(action==ACTION_ROLL)
        {
            int die_roll = randInt(1,10);
            ticks_to_move = die_roll * 8;
            walking = 1;
        }
        if(action==ACTION_FIRE && hasVortex())
        {
            Vortex* v;
            v = new Vortex(getWorld(), getX(), getY(), walkDir);
            getWorld()->addActor(v);
            getWorld()->playSound(SOUND_PLAYER_FIRE);
            useVortex();
        }
        else  return;
    }
    
    else    //player walking state
    {
        playerWalk();
        if(ticks_to_move == 0)
            walking = 0;
    }
}

void Player::playerWalk()
{
    if(ticks_to_move%8==0)
    {
        int direction = walkDir;
        if(getWorld()->isDirectionalSquare(getX(), getY(), direction))
            setWalkDir(direction);
        else if(getWorld()->isFork(getX(), getY()))
        {
            int action = getWorld()->getAction(player_num);
            int tempDir = getWorld()->validActionAtFork(action, direction, getX(), getY());
            if(tempDir != -1)   setWalkDir(tempDir);
            else    return;
        }
        else autoWalk();
    }
    moveAtAngle(walkDir,2);
    ticks_to_move--;
}

void Player::playerTeleport()
{
    teleport();
    setWalkDir(1);
}

void Player::swapPos()
{
    Player* p2 = getWorld()->getOtherPlayer(this);
    
    int tempX = getX(), tempY = getY();
    moveTo(p2->getX(), p2->getY());
    p2->moveTo(tempX, tempY);
    
    int tempTicks = ticks_to_move;
    ticks_to_move = p2->ticks_to_move;
    p2->ticks_to_move = tempTicks;
    
    int tempWalkDir = walkDir;
    setWalkDir(p2->walkDir);
    p2->setWalkDir(tempWalkDir);
    
    bool tempWalk = walking;
    walking = p2->walking;
    p2->walking = tempWalk;
}

void Player::swapCoins()
{
    Player* p2 = getWorld()->getOtherPlayer(this);
    int tempCoins = countCoins();
    m_coins = p2->m_coins;
    p2->m_coins = tempCoins;
}

void Player::swapStars()
{
    Player* p2 = getWorld()->getOtherPlayer(this);
    int tempStars = countStars();
    m_stars = p2->m_stars;
    p2->m_stars = tempStars;
}

//---------------------------Square-------------------------------

void Square::doSomething()
{
    if(!isActive)  return;
    doTheThingToPlayer(getWorld()->getPeach(), peachHere);
    doTheThingToPlayer(getWorld()->getYoshi(), yoshiHere);
}

bool Square::didThePlayerJustLandOnMe(Player* p)
{
    if(p->isWalking()==0)
        if(p->getX()==getX() && p->getY()==getY())
            return true;
    return false;
}

bool Square::didThePlayerJustWalkPastMe(Player* p)
{
    if(p->isWalking()==1)
        if(p->getX()==getX() && p->getY()==getY())
            return true;
    return false;
}

//-------------------------CoinSquare-----------------------------

void CoinSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            if(m_addcoin == 3)
            {
                p->addCoins(3);
                getWorld()->playSound(SOUND_GIVE_COIN);
            }
            if(m_addcoin == -3)
            {
                p->deductCoins(3);
                getWorld()->playSound(SOUND_TAKE_COIN);
            }
            here=1;
        }
    }
    else    here=0;
}

//-------------------------StarSquare-----------------------------

void StarSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p) || didThePlayerJustWalkPastMe(p))
    {
        if(!here)
        {
            if(p->countCoins()>=20)
            {
                p->addCoins(-20);
                p->addStars(1);
                getWorld()->playSound(SOUND_GIVE_STAR);
            }
            here=1;
        }
    }
    else here=0;
}
//both walk past and land upon

//-----------------------DirectionalSquare---------------------------

void DirectionalSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p) || didThePlayerJustWalkPastMe(p))
    {
        if(!here)
        {
            p->setWalkDir(getDirection());
            here=1;
        }
    }
    else here=0;
}

//-------------------------BankSquare-----------------------------

void BankSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            p->addCoins(getWorld()->getBankBalance());
            getWorld()->withdrawFromBank();
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
            here=1;
        }
    }
    else if(didThePlayerJustWalkPastMe(p))
    {
        if(!here)
        {
            int depositcash=5;
            if(p->countCoins()<5)
                depositcash=p->countCoins();
            p->deductCoins(5);
            getWorld()->depositIntoBank(depositcash);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            here=1;
        }
    }
    else here=0;
}

//-------------------------EventSquare-----------------------------

void EventSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            int rando = randInt(1,3);       //int rando = 2;
            switch(rando)
            {
                case 1:
                    p->swapPos();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    peachHere=1;yoshiHere=1;
                    break;
                case 2:
                    p->playerTeleport();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    break;
                case 3:
                    p->giveVortex();
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                    break;
            }
            here=1;
        }
    }
    else here=0;
}

//-----------------------DroppingSquare---------------------------

void DroppingSquare::doTheThingToPlayer(Player* p, bool& here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            if(p->countStars()==0)
                p->deductCoins(10);
            else
            {
                int rando = randInt(1,2);
                if(rando==1)    p->deductCoins(10);
                else            p->addStars(-1);
            }
            getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
            here=1;
        }
    }
    else here=0;
}

//---------------------------Baddie-------------------------------

void Baddie::vortexHit()
{
    teleport();
    setWalkDir(0);
    walking = false;
    pause_counter = 180;
}

bool Baddie::didThePlayerJustLandOnMe(Player* p)
{
    if(p->isWalking()==0 && walking==0)
        if(p->getX()==getX() && p->getY()==getY())
            return true;
    return false;
}

void Baddie::baddieWait()
{
    doTheThingToPlayer(getWorld()->getPeach(), peachHere);
    doTheThingToPlayer(getWorld()->getYoshi(), yoshiHere);
    pause_counter--;
    if(pause_counter==0)
    {
        int squares_to_move = randInt(1,10);
        ticks_to_move = squares_to_move * 8;
        int direction;
        do{
            int randAct = randInt(1,4);
            direction = getWorld()->validActionAtFork(randAct, 1, getX(), getY());
        }while(direction == -1);
        setWalkDir(direction);
        walking = 1;
    }
}

void Baddie::baddieWalk()
{
    if(ticks_to_move%8==0)
    {
        if(getWorld()->isFork(getX(), getY()))
        {
            int direction;
            do{
                int randAct = randInt(1,4);
                direction = getWorld()->validActionAtFork(randAct, walkDir, getX(), getY());
            }while(direction == -1);
            setWalkDir(direction);
        }
        else autoWalk();
    }
    moveAtAngle(walkDir,2);
    ticks_to_move--;
}


//---------------------------Bowser-------------------------------

void Bowser::doSomething()
{
    if(walking==0)  baddieWait();
    if(walking==1)
    {
        baddieWalk();
        if(ticks_to_move == 0)
        {
            walking = 0;
            pause_counter = 180;
            int rando = randInt(1,4);       //int rando = 1;
            if(rando==1)
            {
                for(Actor* b : getWorld()->getActors())
                    if(b->getX()==getX() && b->getY()==getY() && b->isMobile()==false)  //square
                        b->isActive=false;
                DroppingSquare* ds;
                ds = new DroppingSquare(getWorld(),getX(),getY());
                getWorld()->addActor(ds);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
            }
        }
    }
}

void Bowser::doTheThingToPlayer(Player *p, bool &here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            int rando = randInt(1,2);       //int rando = 1;
            if(rando==1)
            {
                p->addCoins( - p->countCoins());
                p->addStars( - p->countStars());
                getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
            here=1;
        }
    }
    else here=0;
}
//----------------------------Boo--------------------------------

void Boo::doSomething()
{
    if(walking==0)  baddieWait();
    if(walking==1)
    {
        baddieWalk();
        if(ticks_to_move == 0)
        {
            walking = 0;
            pause_counter = 180;
        }
    }
}

void Boo::doTheThingToPlayer(Player *p, bool &here)
{
    if(didThePlayerJustLandOnMe(p))
    {
        if(!here)
        {
            int rando = randInt(1,2);
            if(rando==1)    p->swapCoins();
            else            p->swapStars();
            getWorld()->playSound(SOUND_BOO_ACTIVATE);
            here=1;
        }
    }
    else here=0;
}

//---------------------------Vortex-------------------------------

void Vortex::doSomething()
{
    if(!isActive) return;
    moveAtAngle(getDirection(),2);
    if(getX()<0 || getX()>255 || getY()<0 || getY()>255)
        isActive=false;
    for(Actor* b : getWorld()->getActors())
    {
        if(b->vortexImpactable())
            if(determineOverlap(getX(), getY(), b->getX(), b->getY()))
            {
                b->vortexHit();
                isActive=false;
                getWorld()->playSound(SOUND_HIT_BY_VORTEX);
            }
    }
    return;
}

bool Vortex::determineOverlap(int x1, int y1, int x2, int y2)
{
    int xDist,yDist;
    if(x1>x2) xDist = x1-x2;  else xDist = x2-x1;
    if(y1>y2) yDist = y1-y2;  else yDist = y2-y1;
    if(xDist<16 && yDist<16)
        return true;
    return false;    
}
