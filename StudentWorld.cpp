#include "StudentWorld.h"
#include "Board.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

bool StudentWorld::isValidDirection(Actor* actor, int direction)
{
    // gets the next position in current direction and tests to see if that is a valid position
    int newx, newy;
    actor->getPositionInThisDirection(direction, 2, newx, newy); 
    return isValidPath(direction, newx, newy); 
}

bool StudentWorld::isAtFork(Actor* actor, int direction)
{
    // checks to see if there are more than 1 possible paths (and is not at a corner) to see if it is a fork or not
    // depending on the current direction
    switch (direction) {
    case GraphObject::left:
        if (isValidDirection(actor, GraphObject::right)) {
            if (isValidDirection(actor, GraphObject::left) && (isValidDirection(actor, GraphObject::up) || isValidDirection(actor, GraphObject::down)))
                return true;
            if (isValidDirection(actor, GraphObject::up) && isValidDirection(actor, GraphObject::down))
                return true;
        }
        break;
    case GraphObject::right:
        if (isValidDirection(actor, GraphObject::left)) {
            if (isValidDirection(actor, GraphObject::right) && (isValidDirection(actor, GraphObject::up) || isValidDirection(actor, GraphObject::down)))
                return true;
            if (isValidDirection(actor, GraphObject::up) && isValidDirection(actor, GraphObject::down))
                return true;
        }
        break;
    case GraphObject::up:
        if (isValidDirection(actor, GraphObject::down)) {
            if (isValidDirection(actor, GraphObject::up) && (isValidDirection(actor, GraphObject::left) || isValidDirection(actor, GraphObject::right)))
                return true;
            if (isValidDirection(actor, GraphObject::left) && isValidDirection(actor, GraphObject::right))
                return true;
        }
        break;
    case GraphObject::down:
        if (isValidDirection(actor, GraphObject::up)) {
            if (isValidDirection(actor, GraphObject::down) && (isValidDirection(actor, GraphObject::left) || isValidDirection(actor, GraphObject::right)))
                return true;
            if (isValidDirection(actor, GraphObject::left) && isValidDirection(actor, GraphObject::right))
                return true;
        }
        break;
    }
    return false;
}

bool StudentWorld::isValidPath(int direction, int x, int y) const
{
    for (auto i : m_actors)
    {   
        if (i->is_a_square()) // checks to see if i is a square
        {
            // return true immediately if coordinate matches another actors 
            if (x == i->getX() && y == i->getY())
                return true;

            // checks to see actor that called this function is "on" the square 
            // depending on the direction, it means it either shares an X/Y coordinate and has its
            // X/Y coordinate within certain bounds 
            switch (direction)
            {
            case GraphObject::left:
                if (y == i->getY())
                {
                    if (x > i->getX() && x < i->getX() + SPRITE_WIDTH)
                        return true;
                }
                break;
            case GraphObject::right:
                if (y == i->getY()) {
                    if (x < i->getX() && x > i->getX() - SPRITE_WIDTH)
                        return true;
                }
                break;
            case GraphObject::up:
                if (x == i->getX())
                {
                    if (y < i->getY() && y > i->getY() - SPRITE_HEIGHT)
                        return true;
                }
                break;
            case GraphObject::down:
                if (x == i->getX())
                {
                    if (y > i->getY() && y < i->getY() + SPRITE_HEIGHT)
                        return true;
                }
                break;
            }
        }
    }
    return false;
}

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), m_bank(0)
{
    m_peach = nullptr;
    m_yoshi = nullptr;
    m_actors.clear();
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    Board bd;

    string board_file = assetPath() + "board0" + std::to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = bd.loadBoard(board_file);

    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        for (int y = 0; y < BOARD_HEIGHT; y++) { // might need to use doubles in the future???
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                Board::GridEntry ge = bd.getContentsOf(x, y);

                switch (ge)
                {
                case Board::empty:
                    cout << "Location " << x << "," << y << " is empty\n";
                    break;

                case Board::boo:
                    cout << "Location " << x << "," << y << " has a Boo and a blue coin square\n";
                    m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    m_actors.push_back(new Boo(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;

                case Board::bowser:
                    cout << "Location " << x << "," << y << " has a Bowser and a blue coin square\n";
                    m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    m_actors.push_back(new Bowser(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;

                case Board::player:
                    cout << "Location " << x << "," << y << " has Peach & Yoshi and a blue coin square\n";

                    m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    m_peach = new Player(1, this, IID_PEACH, x * SPRITE_WIDTH, y * SPRITE_HEIGHT);
                    m_yoshi = new Player(2, this, IID_YOSHI, x * SPRITE_WIDTH, y * SPRITE_HEIGHT);
                    break;

                case Board::red_coin_square:
                    cout << "Location " << x << "," << y << " has a red coin square\n";
                    m_actors.push_back(new CoinSquare(-3, this, IID_RED_COIN_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;

                case Board::blue_coin_square:
                    cout << "Location " << x << "," << y << " has a blue coin square\n";
                    m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;
                case Board::left_dir_square:
                    cout << "Location " << x << "," << y << " has a left directional square\n";
                    m_actors.push_back(new DirectionalSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, GraphObject::left));
                    break;
                case Board::right_dir_square:
                    cout << "Location " << x << "," << y << " has a right directional square\n";
                    m_actors.push_back(new DirectionalSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, GraphObject::right));
                    break;
                case Board::up_dir_square:
                    cout << "Location " << x << "," << y << " has an up directional square\n";
                    m_actors.push_back(new DirectionalSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, GraphObject::up));
                    break;
                case Board::down_dir_square:
                    cout << "Location " << x << "," << y << " has a down directional square\n";
                    m_actors.push_back(new DirectionalSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, GraphObject::down));
                    break;
                case Board::event_square:
                    cout << "Location " << x << "," << y << " has a event square\n";
                    m_actors.push_back(new EventSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;
                case Board::bank_square:
                    cout << "Location " << x << "," << y << " has a bank square\n";
                    m_actors.push_back(new DroppingSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;
                case Board::star_square:
                    cout << "Location " << x << "," << y << " has a event square\n";
                    m_actors.push_back(new StarSquare(this, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
                    break;
                }
            }
        }
    }
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // give each actor a chance to do something
    m_peach->doSomething();
    m_yoshi->doSomething(); 

    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->is_active()) {
            (*it)->doSomething(); 
        }
    }

    // remove newly-inactive actors after each tick 
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (!(*it)->is_active()) {
            delete *it;
            m_actors.erase(it);
            it = m_actors.begin(); // restart the loop because if I directly store what erase() returns it skips an object
        }
    }

    //update the Game status line
    ostringstream oss;

    oss << "P1 Roll: " << m_peach->get_dice(); 
    oss << " Stars: " << m_peach->get_stars();
    oss << " $$: " << m_peach->get_coins(); 
    if (m_peach->has_vortex())
        oss << " VOR";

    oss << " | Time: " << timeRemaining();
    oss << " | Bank: " << m_bank;
    oss << " | P2 Roll: " << m_yoshi->get_dice();
    oss << " Stars: " << m_yoshi->get_stars();
    oss << " $$: " << m_yoshi->get_coins();
    if (m_yoshi->has_vortex())
        oss << " VOR";

    string s = oss.str();
    setGameStatText(s);


    // check if game is over
    if (timeRemaining() <= 0)
    {
        this->playSound(SOUND_GAME_FINISHED);
        if (peachWon()) 
        {
            setFinalScore(m_peach->get_stars(), m_peach->get_coins());
            return GWSTATUS_PEACH_WON;
        }
        else   // yoshi won
        {
            setFinalScore(m_yoshi->get_stars(), m_yoshi->get_coins());
            return GWSTATUS_YOSHI_WON;
        }
    }

    // the isn't over yet so continue playing 
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_peach;
    delete m_yoshi;
    m_peach = nullptr;
    m_yoshi = nullptr;

    for (auto i : m_actors)
        delete i;
    m_actors.clear();
}


bool StudentWorld::peachWon() const
{
    // winner is the player who collected the most stars and coins 
    if (m_peach->get_stars() > m_yoshi->get_stars()) {
        return true;
    }
    else if (m_peach->get_stars() < m_yoshi->get_stars()) {
        return false;
    }
    // in case of equal stars, the number of coins is used as a tie-breaker 
    else { 
        if (m_peach->get_coins() > m_yoshi->get_coins()) {
            return true;
        }
        else if (m_peach->get_coins() < m_yoshi->get_coins()) {
            return false;
        }
        // if both players have the same # of stars and coins, a winner is randomly picked
        else {
            int winner = randInt(1, 2);
            if (winner == 1)
                return true;
            else
                return false;
        }
    }
}

bool StudentWorld::intersecting_peach(GraphObject* actor) const {
    // checks if calling actor shares the same coordinates as peach
    if (actor->getX() == m_peach->getX() && actor->getY() == m_peach->getY())
        return true;
    else
        return false;
}

bool StudentWorld::intersecting_yoshi(GraphObject* actor) const {
    // calling actor checks if it shares the same coordinates as yoshi or not
    if (actor->getX() == m_yoshi->getX() && actor->getY() == m_yoshi->getY())
        return true;
    else
        return false;
}

bool StudentWorld::enemy_lands_with_peach(GraphObject* enemy) const {
    // checks if enemy calling function shares coordinates with peach 
    // AND peach is in a WAITING_TO_ROLL state (0)
    if (enemy->getX() == m_peach->getX() && enemy->getY() == m_peach->getY() && m_peach->get_state() == 0) {
        return true;
    }
    else
        return false;
}

bool StudentWorld::enemy_lands_with_yoshi(GraphObject* enemy) const {
    // checks if enemy calling function shares coordinates with yoshi 
    // AND yoshi is in a WAITING_TO_ROLL state (0)
    if (enemy->getX() == m_yoshi->getX() && enemy->getY() == m_yoshi->getY() && m_yoshi->get_state() == 0) {
        return true;
    }
    else
        return false;
}

Actor* StudentWorld::get_square_at_location(int x, int y) 
{
    // returns pointer to square if there is one at given location
    // else returns nullptr
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->is_a_square() && (*it)->getX() == x && (*it)->getY() == y && (*it)->is_active())
        {
            return (*it);
        }
    }
    return nullptr; 
}

Actor* StudentWorld::get_random_square(int oldX, int oldY)
{
    // will looping until function picks (from m_actors) an active square that is different (has different coordinates)
    bool keep_picking_random_index = true;

    while (keep_picking_random_index)
    {
        int random = randInt(0, m_actors.size() - 1);

        if (m_actors[random]->is_a_square() == true && m_actors[random]->getX() != oldX && m_actors[random]->getY() != oldY)
        {
            keep_picking_random_index = false;
            return m_actors[random];
        }
    }
    return m_actors[0]; // will never run, just to get rid of "all paths must return" warning 
                        // returns m_actors[0] which is always a BlueSquare
}

Player* StudentWorld::get_other_player(Player* p) const
{
    //cout << "p: " << p << endl;
    //cout << "m_peach: " << m_peach << endl;
    //cout << "m_yoshi: " << m_yoshi << endl;

    if (p == m_peach)
        return m_yoshi;
    else
        return m_peach; 
}

bool StudentWorld::canWalkBack(Actor* actor, int direction)
{
    // tests to see if the actor came from some direction 
    // used to check if actor wasn't teleported 
    switch (direction)
    {
    case GraphObject::left:
        if (isValidDirection(actor, GraphObject::right))
            return true; 
        break;
    case GraphObject::right:
        if (isValidDirection(actor, GraphObject::left))
            return true;
        break;
    case GraphObject::up:
        if (isValidDirection(actor, GraphObject::down))
            return true;
        break;
    case GraphObject::down:
        if (isValidDirection(actor, GraphObject::up))
            return true;
        break;
    }
    return false;
}

bool StudentWorld::overlaps_with_impactable(Actor* vortex, Actor*& storage)
{
    // loops through vectors of m_actors to find impactable actors
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->can_be_hit_by_vortex() && (*it)->is_active())
        {
            // checks each direction to see if impactable actor is within the vortex's bounding box
        
            // right
            if (vortex->get_walking_dir() == 0)
            {
                // vortex spawned on same square as baddie
                if (vortex->getX() - 2 == (*it)->getX() && vortex->getY() == (*it)->getY()) {
                    storage = *it;
                    return true;
                }

                if ((vortex->getX() + 15 >= (*it)->getX() && vortex->getX() <= (*it)->getX()) && (vortex->getY() + 15 >= (*it)->getY() && vortex->getY() - 15 <= (*it)->getY())) {
                    storage = *it;
                    return true;
                }
            }
            // up 
            else if (vortex->get_walking_dir() == 90)
            {
                // vortex spawned on same square as baddie
                if (vortex->getX() == (*it)->getX() && vortex->getY() - 2 == (*it)->getY()) {
                    storage = *it;
                    return true;
                }

                if ((vortex->getX() + 15 >= (*it)->getX() && vortex->getX() - 15 <= (*it)->getX()) && (vortex->getY() + 15 >= (*it)->getY() && vortex->getY() <= (*it)->getY())) {
                    storage = *it;
                    return true;
                }
            }
            // left
            else if (vortex->get_walking_dir() == 180)
            {
                // vortex spawned on same square as baddie
                if (vortex->getX() + 2 == (*it)->getX() && vortex->getY() == (*it)->getY()) {
                    storage = *it;
                    return true;
                }

                if ((vortex->getX() - 15 <= (*it)->getX() && vortex->getX() >= (*it)->getX()) && (vortex->getY() + 15 >= (*it)->getY() && vortex->getY() - 15 <= (*it)->getY())) {
                    storage = *it;
                    return true;
                }
            }
            // down
            else
            {
                // vortex spawned on same square as baddie
                if (vortex->getX() == (*it)->getX() && vortex->getY() + 2 == (*it)->getY()) {
                    storage = *it;
                    return true;
                }

                if ((vortex->getX() + 15 >= (*it)->getX() && vortex->getX() - 15 <= (*it)->getX()) && (vortex->getY() - 15 <= (*it)->getY() && vortex->getY() >= (*it)->getY())) {
                    storage = *it;
                    return true;
                }
            }
        }
    }
    return false; 
}

bool StudentWorld::is_a_directional_square(int x, int y)
{
    // checks to see if given location has a directional square 
    if (get_square_at_location(x, y) != nullptr && get_square_at_location(x, y)->can_change_player_dir() && get_square_at_location(x, y)->is_active())
    {
        return true;
    }
    else
        return false;
}


