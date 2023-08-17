#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	
	// pushes a new actor to vector m_actor 
	void addNewActor(Actor* new_actor) { m_actors.push_back(new_actor); };

	//// HANDLES DIRECTIONS ////
	
	// checks if actor can keep moving in current direction
	bool isValidDirection(Actor* actor, int direction);

	// checks if actor is at a fork
	bool isAtFork(Actor* actor, int direction);

	// checks if actor came from a valid path (aka wasn't teleported in) 
	bool canWalkBack(Actor* actor, int direction); 

	//// HANDLES CENTRAL BANK ///// 
	int get_bank_coins() const { return m_bank; };
	void deposit_bank_coins(int coins) { m_bank += coins; };
	void reset_bank_coins() { m_bank = 0; };

	// getters
	Player* get_Peach() const { return m_peach; };
	Player* get_Yoshi() const { return m_yoshi; };

	/// HANDLES INTERACTIONS //////

	// peach landed OR moved onto same location (e.g. Square)
	bool intersecting_peach(GraphObject* actor) const; 

	// yoshi landed OR moved onto same location (e.g. Square)
	bool intersecting_yoshi(GraphObject* actor) const;

	// peach landed on same location as enemy
	bool enemy_lands_with_peach(GraphObject* enemy) const; 

	// yoshi landed on same location as enemy
	bool enemy_lands_with_yoshi(GraphObject* enemy) const;

	// vortex overlapped with impactable object (e.g. enemey) and stores a pointer to that object
	bool overlaps_with_impactable(Actor* vortex, Actor*& storage); 


	//// HANDLES GETTING OTHER ACTORS ////
	
	// checks if location has a directional square 
	bool is_a_directional_square(int x, int y); 

	// returns a pointer to a square at specified location, nullptr if DNE
	Actor* get_square_at_location(int x, int y); 

	// returns pointer to random square on the board
	Actor* get_random_square(int oldX, int oldY); 

	// returns pointer to other player object
	Player* get_other_player(Player* p) const; 

private:
	Player* m_peach;
	Player* m_yoshi;
	std::vector<Actor*> m_actors;
	int m_bank; 

	// checks if location is on a square 
	bool isValidPath(int direction, int x, int y) const; // called by isValidDirection
	bool peachWon() const; 
};

#endif // STUDENTWORLD_H_
