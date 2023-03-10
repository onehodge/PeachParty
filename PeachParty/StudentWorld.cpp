#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath): GameWorld(assetPath)
{
    m_peach=nullptr;
    m_yoshi=nullptr;
    m_board=nullptr;
    actors.clear();
    m_balance=0;
}

int StudentWorld::init()
{
	startCountdownTimer(5);  // this placeholder causes timeout after 5 seconds
    m_board = new Board;
    string board_file = assetPath() + "/board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = m_board->loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
        return GWSTATUS_BOARD_ERROR;
    else if (result == Board::load_fail_bad_format)
        return GWSTATUS_BOARD_ERROR;
    cerr<<"loaded board\n";
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 16; y++)
        {
            Board::GridEntry ge = m_board->getContentsOf(x,y);
            switch (ge)
            {
                case Board::empty:
                    cerr << "Location " << x << " " << y << " empty" << endl;
                    break;
                    
                case Board::player:
                    cerr << "player Location " << x << " " << y << endl;
                    m_peach = new Player(this, IID_PEACH, x*16, y*16, 1);
                    m_yoshi = new Player(this, IID_YOSHI, x*16, y*16, 2);
                    actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*16, y*16));
                    break;
                    
                case Board::blue_coin_square:
                    cerr << "Location " << x << " " << y << " = blue coin square" << endl;
                    actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*16, y*16));
                    break;
                case Board::red_coin_square:
                    cerr << "Location " << x << " " << y << " = red coin square" << endl;
                    actors.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, x*16, y*16));
                    break;
                    
                case Board::up_dir_square:
                    cerr << "Location " << x << " " << y << " = up dir square" << endl;
                    actors.push_back(new DirectionalSquare(this, x*16, y*16, 90));
                    break;
                case Board::down_dir_square:
                    cerr << "Location " << x << " " << y << " = down dir square" << endl;
                    actors.push_back(new DirectionalSquare(this, x*16, y*16, 270));
                    break;
                case Board::left_dir_square:
                    cerr << "Location " << x << " " << y << " = left dir square" << endl;
                    actors.push_back(new DirectionalSquare(this, x*16, y*16, 180));
                    break;
                case Board::right_dir_square:
                    cerr << "Location " << x << " " << y << " = right dir square" << endl;
                    actors.push_back(new DirectionalSquare(this, x*16, y*16, 0));
                    break;
                    
                case Board::event_square:
                    cerr << "Location " << x << " " << y << " = event square" << endl;
                    actors.push_back(new EventSquare(this, x*16, y*16));
                    break;
                case Board::bank_square:
                    cerr << "Location " << x << " " << y << " = bank square" << endl;
                    actors.push_back(new BankSquare(this, x*16, y*16));
                    break;
                case Board::star_square:
                    cerr << "Location " << x << " " << y << " = star square" << endl;
                    actors.push_back(new StarSquare(this, x*16, y*16));
                    break;
                    
                case Board::bowser:
                    cerr << "Location " << x << " " << y << " = bowser" << endl;
                    actors.push_back(new Bowser(this, x*16, y*16));
                    actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*16, y*16));
                    break;
                case Board::boo:
                    cerr << "Location " << x << " " << y << " = boo" << endl;
                    actors.push_back(new Boo(this, x*16, y*16));
                    actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*16, y*16));
                    break;
            }
        }
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    
    //Give each actor a chance to do something
    m_peach->doSomething();
    m_yoshi->doSomething();
    for(Actor* a : actors)
        a->doSomething();
    
    // Remove newly-inactive actors after each tick
    for(auto it=actors.begin();it!=actors.end();it++)
        if (!(*it)->isActive)
        {
            delete *it;
            actors.erase(it);
            it=actors.begin();
        }
    
    //update display text
    string text1= "P1 Roll: " + to_string(m_peach->countRolls()) + " Stars: " + to_string(m_peach->countStars()) + " $$: " + to_string(m_peach->countCoins());
    if(m_peach->hasVortex())    text1 +=" VOR";
    string text0= " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBankBalance());
    string text2= " | P2 Roll: " + to_string(m_yoshi->countRolls()) + " Stars: " + to_string(m_yoshi->countStars()) + " $$: " + to_string(m_yoshi->countCoins());
    if(m_yoshi->hasVortex())    text2 +=" VOR";
    string text = text1+text0+text2;
    setGameStatText(text);
    
    
    if(timeRemaining()<=0)
    {
        if(m_peach->countStars() > m_yoshi->countStars())
        {
            setFinalScore(m_peach->countStars(), m_peach->countCoins());
            return GWSTATUS_PEACH_WON;
        }
        else if(m_yoshi->countStars() > m_peach->countStars())
        {
            setFinalScore(m_yoshi->countStars(), m_yoshi->countCoins());
            return GWSTATUS_YOSHI_WON;
        }
        else
        {
            if(m_peach->countCoins() >= m_yoshi->countCoins())
            {
                setFinalScore(m_peach->countStars(), m_peach->countCoins());
                return GWSTATUS_PEACH_WON;
            }
            else
            {
                setFinalScore(m_yoshi->countStars(), m_yoshi->countCoins());
                return GWSTATUS_YOSHI_WON;
            }
        }
    }
    
    // the game isn't over yet so continue playing
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (Actor* a : actors)
        delete a;
    actors.clear();
    
    delete m_peach;
    m_peach = nullptr;
    delete m_yoshi;
    m_yoshi = nullptr;
}

Player* StudentWorld::getOtherPlayer(Player* p)
{
    if(p==m_peach)  return m_yoshi;
    if(p==m_yoshi)  return m_peach;
    return nullptr;
}

bool StudentWorld::isSquare(int x, int y)
{
    if(x<0 || x>255 || y<0 || y>255)
        return false;
    if(getBoard()->getContentsOf(x/16, y/16)==Board::empty)
        return false;
    return true;
        
}

bool StudentWorld::isFork(int x, int y)
{
    int countPaths=0;
    x/=16;  y/=16;
    if(getBoard()->getContentsOf(x+1, y) != Board::empty)   countPaths++;
    if(getBoard()->getContentsOf(x-1, y) != Board::empty)   countPaths++;
    if(getBoard()->getContentsOf(x, y+1) != Board::empty)   countPaths++;
    if(getBoard()->getContentsOf(x, y-1) != Board::empty)   countPaths++;
    if(countPaths>2)    return true;
    return false;
}

int StudentWorld::validActionAtFork(int action, int dir, int x, int y)
{
    switch(action)
    {
        case ACTION_UP:
            if(dir==270) return -1;
            if(isSquare(x,y+16)) return 90;
            else return -1;
            break;
        case ACTION_DOWN:
            if(dir==90)  return -1;
            if(isSquare(x,y-16)) return 270;
            else return -1;
            break;
        case ACTION_LEFT:
            if(dir==0)   return -1;
            if(isSquare(x-16,y)) return 180;
            else return -1;
            break;
        case ACTION_RIGHT:
            if(dir==180) return -1;
            if(isSquare(x+16,y)) return 0;
            else return -1;
            break;
        default:
            return -1;
    }
}

bool StudentWorld::isDirectionalSquare(int x, int y, int& dir)
{
    Board::GridEntry ge = getBoard()->getContentsOf(x/16, y/16);
    if(ge==Board::up_dir_square)    {dir=90;  return true;}
    if(ge==Board::down_dir_square)  {dir=270; return true;}
    if(ge==Board::left_dir_square)  {dir=180; return true;}
    if(ge==Board::right_dir_square) {dir=0;   return true;}
    return false;
}
