#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// class hierachy

//   GraphObject
//		Actor
//			Player
//			ActivatesOnPlayer
//				Enemy
//					Bowser
//					Boo
//				Square
//					BankSquare
//					CoinSquare
//					DirecitonalSquare
//					DroppingSquare
//					EventSquare
//					StarSquare
//			Vortex

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth, double size = 1);

	virtual void doSomething() = 0;
	virtual bool is_a_square() const = 0;
	virtual bool can_be_hit_by_vortex() const = 0;

	virtual bool can_change_player_dir() const { return false; }; // will only be true for DirectionalSquares
	virtual void hit_by_vortex() { ; }; // can implement in the future if want to expand actors able to be impacted

	bool is_active() const { return m_isActive; };
	void set_inactive() { m_isActive = false; };

	// getters
	StudentWorld* getWorld() const { return m_world; };
	int get_walking_dir() const { return m_walking_dir; };
	int get_sprite_dir() const { return m_sprite_dir; };
	int get_ticks_to_move() const { return m_ticks_to_move; }; 

	// setters
	void set_ticks_to_move(int amt) { m_ticks_to_move = amt; };
	void decrement_ticks_to_move() { m_ticks_to_move--; };

	// deals with directional changes for Players and Enemeies
	void force_walk_direction(int angle, int dir); // changes actor's sprite and walking direction
	void reset_walk_direction(); // picks a random valid direction and sets new sprite/walk direction
	void move_perpendicularly(); // picks a random valid perpendicular direction and sets new sprite/walk direction
	void teleport_to_random_sq(); // picks random square on board and teleports actor to it
private:
	StudentWorld* m_world;
	bool m_isActive;
	int m_walking_dir;
	int m_sprite_dir;
	int m_ticks_to_move;
};

class Player : public Actor
{
public:
	Player(int playerNum, StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 0);
	virtual void doSomething();
	virtual bool is_a_square() const { return false; };
	virtual bool can_be_hit_by_vortex() const { return false; };

	enum PlayerState { waiting_to_roll, walking };

	// getters
	int get_dice() const { return m_diceroll; };
	int get_stars() const { return m_stars; };
	int get_coins() const { return m_coins; };
	bool has_vortex() const { return m_has_vortex; };
	PlayerState get_state() const { return m_state; };

	// setters 
	void reset_coins() { m_coins = 0; };
	void reset_stars() { m_stars = 0; };
	void adjust_stars(const int amt) { m_stars = m_stars + amt; };
	void adjust_coins(const int amt) { m_coins = m_coins + amt; };
	void equip_vortex() { m_has_vortex = true; };

	// swapping XX with another player
	void swap_positions(); // swaps m_ticks_to_move and state with the other player 
	void swap_stars();
	void swap_coins();

private:
	int m_playerNum; // Peach: 1, Yoshi: 2
	PlayerState m_state; // PlayerState: 0 -> waiting_to_roll, 1 -> walking
	int m_coins;
	int m_stars;
	bool m_has_vortex; 
	int m_diceroll; 
};

class Vortex : public Actor
{
public:
	Vortex(int fire_dir, StudentWorld* world, int startX, int startY, int dir = right, int depth = 0, int imageID = IID_VORTEX);
	virtual void doSomething();
	virtual bool is_a_square() const { return false; };
	virtual bool can_be_hit_by_vortex() const { return false; };
};

class ActivatesOnPlayer : public Actor
{
public:
	ActivatesOnPlayer(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth); //bool activate_when_go_lands??

	// marker to test if player that stepped onto square/baddie is "new"
	bool peach_is_new() const { return m_peach_is_new; };
	bool yoshi_is_new() const { return m_yoshi_is_new; };
	
	// set marker to false (player just stepped on)
	void set_peach_new_false() { m_peach_is_new = false; };
	void set_yoshi_new_false() { m_yoshi_is_new = false; };
	
	// set marker to true (once player steps off)
	void set_peach_new_true() { m_peach_is_new = true; };
	void set_yoshi_new_true() { m_yoshi_is_new = true; };
private:
	bool m_peach_is_new; 
	bool m_yoshi_is_new; 
};

class Square : public ActivatesOnPlayer
{
public:
	Square(StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 1);
	virtual void doSomething();
	bool is_a_square() const { return true; };
	bool can_be_hit_by_vortex() const { return false; };
private:
	virtual void doSomethingDifferent(Player* player) = 0; 
};

class CoinSquare : public Square
{
public:
	CoinSquare(int adjust_coins_by, StudentWorld* world, int imageID, int startX, int startY);
	virtual void doSomethingDifferent(Player* player);
private:
	int m_adjust_coins_by; 
};

class BankSquare : public Square
{
public:
	BankSquare(StudentWorld* world, int startX, int startY, int imageID = IID_BANK_SQUARE); 
	virtual void doSomethingDifferent(Player* player);
};

class DirectionalSquare : public Square
{
public:
	DirectionalSquare(StudentWorld* world, int startX, int startY, int dir, int imageID = IID_DIR_SQUARE); 
	virtual void doSomethingDifferent(Player* player);
	virtual bool can_change_player_dir() const { return true; };
private:
	int m_forced_direction; 
};

class DroppingSquare : public Square
{
public:
	DroppingSquare(StudentWorld* world, int startX, int startY, int imageID = IID_DROPPING_SQUARE);
	virtual void doSomethingDifferent(Player* player);
};

class EventSquare : public Square
{
public:
	EventSquare(StudentWorld* world, int startX, int startY, int imageID = IID_EVENT_SQUARE);
	virtual void doSomethingDifferent(Player* player);
};

class StarSquare : public Square
{
public:
	StarSquare(StudentWorld* world, int startX, int startY, int imageID = IID_STAR_SQUARE);
	virtual void doSomethingDifferent(Player* player);
};

class Enemy : public ActivatesOnPlayer
{
public:
	Enemy(StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 0); // int num_sq_to_move, int number_of_ticks_to_pause, bool activate_when_go_lands ???
	virtual void doSomething();
	virtual bool is_a_square() const { return false; };
	virtual bool can_be_hit_by_vortex() const { return true; };

	enum EnemyState { paused, walking };
		
	virtual void hit_by_vortex(); // handles interaction between baddie and vortex

	// getters
	int get_sq_to_move() const { return m_squares_to_move; };
	int get_ticks_to_pause() const { return m_ticks_to_pause; };
	EnemyState get_state() const { return m_state; };

	// setters
	void set_squares_to_move(int amt) { m_squares_to_move = amt; };
	void set_ticks_to_pause(int amt) { m_ticks_to_pause = amt; };
	void decrement_ticks_to_pause() { m_ticks_to_pause--; };
	void set_state(EnemyState state) { m_state = state; }; 
	
private:
	int m_squares_to_move;
	int m_ticks_to_pause;
	EnemyState m_state; // 0 -> paused, 1 -> walking
	virtual void doPausedAction(Player* player) = 0;
	virtual void newDroppingSquare(Actor* actor) = 0;
	
	// bool activate_when_go_lands??
};

class Bowser : public Enemy
{
public:
	Bowser(StudentWorld* world,  int startX, int startY, int imageID = IID_BOWSER);
	virtual void doPausedAction(Player* player);
	virtual void newDroppingSquare(Actor* actor);
};

class Boo : public Enemy
{
public:
	Boo(StudentWorld* world, int startX, int startY, int imageID = IID_BOO);
	virtual void doPausedAction(Player* player);
	virtual void newDroppingSquare(Actor* actor) { ; }; // Boo doesn't implement it
};


#endif // ACTOR_H_