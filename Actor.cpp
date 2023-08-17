#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include <iostream>


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


////////////////////
////    Actor   ////
////////////////////


Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth, double size)
	: GraphObject(imageID, startX, startY, dir, depth, size), 
	m_world(world), 
	m_isActive(true), 
	m_walking_dir(dir),
	m_sprite_dir(0),
	m_ticks_to_move(0)
{}

// angle = walking direction, dir = which way sprite is facing
void Actor::force_walk_direction(int angle, int dir)
{
	// updates actor walking and sprite direction 
	m_walking_dir = angle;
	m_sprite_dir = dir;
	setDirection(dir);
}

void Actor::reset_walk_direction()
{
	// will keep picking a random direction until that direction is valid
	// then will update new walking/sprite direction
	bool keep_picking_random_direction = true;

	while (keep_picking_random_direction)
	{
		int random_dir = randInt(1, 4);
		if (random_dir == 1 && getWorld()->isValidDirection(this, right) && get_walking_dir() != left) {
			keep_picking_random_direction = false;
			force_walk_direction(right, 0);
		}
		else if (random_dir == 2 && getWorld()->isValidDirection(this, up) && get_walking_dir() != down) {
			keep_picking_random_direction = false;
			force_walk_direction(up, 0);
		}
		else if (random_dir == 3 && getWorld()->isValidDirection(this, left) && get_walking_dir() != right) {
			keep_picking_random_direction = false;
			force_walk_direction(left, 180);
		}
		else if (random_dir == 4 && getWorld()->isValidDirection(this, down) && get_walking_dir() != up) {
			keep_picking_random_direction = false;
			force_walk_direction(down, 0);
		}
	}
}

void Actor::move_perpendicularly()
{
	// will choose a random direction perpendicular to the current direction
	// then update walking/sprite direction accordingly
	if (get_walking_dir() == left || get_walking_dir() == right)
	{
		if (getWorld()->isValidDirection(this, up)) // face right and walk upwards
		{
			force_walk_direction(up, 0);
		}
		else // face right and walk downwards
		{
			force_walk_direction(down, 0);
		}
	}
	else // m_walking_direction is either up or down
	{
		if (getWorld()->isValidDirection(this, right)) // face right and walk rightwards
		{
			force_walk_direction(right, 0);
		}
		else // face left and walk leftwards
		{
			force_walk_direction(left, 180);
		}
	}
}

void Actor::teleport_to_random_sq()
{
	// get a pointer to a random square and store it in p
	Actor* p = getWorld()->get_random_square(this->getX(), this->getY());
	moveTo(p->getX(), p->getY());
}

/////// PLAYER ///////

Player::Player(int playerNum, StudentWorld* world, int imageID, int startX, int startY, int dir, int depth)
	: Actor(world, imageID, startX, startY, dir, depth),
	m_playerNum(playerNum),
	m_state(waiting_to_roll),
	m_coins(0),
	m_stars(0),
	m_has_vortex(false),
	m_diceroll(0)
{}

void Player::doSomething()
{
	if (m_state == waiting_to_roll)
	{
		// Actor has invalid direction (due to being teleported)
		if (!getWorld()->isValidDirection(this, get_walking_dir()) && !getWorld()->canWalkBack(this, get_walking_dir()))
		{
			reset_walk_direction(); 
		}

		// See if user pressed key 
		switch (getWorld()->getAction(m_playerNum))
		{
		case ACTION_ROLL:
			m_diceroll = randInt(1, 10);
			set_ticks_to_move(m_diceroll * 8);
			m_state = walking;
			break;
		case ACTION_FIRE:
			if (has_vortex()) 
			{
				int newx, newy;
				// get coordinates and square directly in front of player
				getPositionInThisDirection(get_walking_dir(), 16, newx, newy);
				// spawn the vortex onto that square
				getWorld()->addNewActor(new Vortex(get_walking_dir(),getWorld(), newx, newy));
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				m_has_vortex = false;
			}
			break;
		// user didn't press a key or pressed any other key
		case ACTION_NONE:
		default: 
			return;
			break;
		}
	}
	if (m_state == walking)
	{
		// If avatar is directly on top of a directional square 
		if (getWorld()->is_a_directional_square(getX(), getY())){
			; // DirectionalSquare handles interaction 
		}
		// else if avatar is directly on top of a square at a fork (with multiple possible directions)
		else if (getWorld()->isAtFork(this, get_walking_dir())) {
			// get user input 
			// if user input direction is valid update walking/sprite direction
			switch (getWorld()->getAction(m_playerNum)) 
			{
			case ACTION_LEFT:
				if (getWorld()->isValidDirection(this, left) && get_walking_dir() != right) {
					force_walk_direction(left, 180);
				}
				else 
					return;
				break;
			case ACTION_RIGHT:
				if (getWorld()->isValidDirection(this, right) && get_walking_dir() != left) {
					force_walk_direction(right, 0);
				}
				else 
					return;
				break;
			case ACTION_UP:
				if (getWorld()->isValidDirection(this, up) && get_walking_dir() != down) {
					force_walk_direction(up, 0);
				}
				else 
					return;
				break;
			case ACTION_DOWN:
				if (getWorld()->isValidDirection(this, down) && get_walking_dir() != up) {
					force_walk_direction(down, 0);
				}
				else 
					return;
				break;
			case ACTION_NONE:
			default:
				return;
				break;
			}
		}
		// else if avatar can't continue moving forward in its current direction and is not at a fork 
		else if (!getWorld()->isValidDirection(this, get_walking_dir()))
		{
			move_perpendicularly();
		}

		// move 2 pixels in the walk direction
		moveAtAngle(get_walking_dir(), 2);

		decrement_ticks_to_move();
		if (get_ticks_to_move() % 8 == 0)
			m_diceroll--; 
		if (get_ticks_to_move() == 0)
			m_state = waiting_to_roll;
	}
}


void Player::swap_positions()
{
	// get pointer to other player 
	Player* other = getWorld()->get_other_player(this);

	// store all variables of other in temp variables
	int temp_x = other->getX();
	int temp_y = other->getY();
	int temp_ticks_to_move = other->get_ticks_to_move();
	int temp_walk_dir = other->get_walking_dir();
	int temp_sprite_dir = other->get_sprite_dir();
	PlayerState temp_player_state = other->m_state;

	// set other to equal og player
	other->moveTo(this->getX(), this->getY());
	other->set_ticks_to_move(this->get_ticks_to_move());
	other->force_walk_direction(this->get_walking_dir(), this->get_sprite_dir());
	other->m_state = this->m_state;

	// set og player to equal the other
	this->moveTo(temp_x, temp_y);
	this->set_ticks_to_move(temp_ticks_to_move);
	this->force_walk_direction(temp_walk_dir, temp_sprite_dir);
	this->m_state = temp_player_state; 
}

void Player::swap_stars()
{
	// get pointer to other player
	Player* other = getWorld()->get_other_player(this);

	// create temp variable 
	int temp_stars = other->get_stars();

	// swap
	other->reset_stars();
	other->adjust_stars(this->m_stars);

	this->reset_stars();
	this->adjust_stars(temp_stars);
}

void Player::swap_coins()
{
	// get pointer to other player
	Player* other = getWorld()->get_other_player(this);

	// create temp
	int temp_coins = other->get_coins();
	
	// swap
	other->reset_coins();
	other->adjust_coins(this->m_coins);

	this->reset_coins();
	this->adjust_coins(temp_coins);
}



/////// VORTEX /////////

Vortex::Vortex(int fire_dir, StudentWorld* world, int startX, int startY, int dir, int depth, int imageID )
	: Actor(world, imageID, startX, startY, dir, depth)
{
	// initializes m_walking_direction properly in Actor class
	force_walk_direction(fire_dir, 0);
}

void Vortex::doSomething()
{
	if (is_active())
	{
		// move forward 2 pixels
		moveAtAngle(get_walking_dir(), 2); 

		// check if flew offscreen
		if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT)
		{
			set_inactive();
		}

		// temp pointer to store possible impactable actor that vortex collided with
		Actor* temp = nullptr;
		
		if (getWorld()->overlaps_with_impactable(this, temp))
		{
			// have that actor handle interaction after being hit by vortex
			temp->hit_by_vortex();
			set_inactive();
			getWorld()->playSound(SOUND_HIT_BY_VORTEX);
		}
	}
	else
		return;
}

//////////////////////////////
////  Activates on Player ////
//////////////////////////////

ActivatesOnPlayer::ActivatesOnPlayer(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth)
	: Actor(world, imageID, startX, startY, dir, depth), m_peach_is_new(true), m_yoshi_is_new(true)
{}


/////////////////
////  SQUARE ////
/////////////////

Square::Square(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth)
	: ActivatesOnPlayer(world, imageID, startX, startY, dir, depth)
{}

void Square::doSomething()
{
	if (is_active())
	{
		// reset peach and yoshi to "new" players after they move off the square
		if (!getWorld()->intersecting_peach(this))
			set_peach_new_true();
		if (!getWorld()->intersecting_yoshi(this))
			set_yoshi_new_true();

		// check if new player (peach) intersected with a square
		if (getWorld()->intersecting_peach(this) && peach_is_new())
		{
			set_peach_new_false();
			doSomethingDifferent(getWorld()->get_Peach()); // subclasses handle whether peach landed on or moved over square
		}
		// check if new player (yoshi) intersected with a square
		if (getWorld()->intersecting_yoshi(this) && yoshi_is_new())
		{
			set_yoshi_new_false();
			doSomethingDifferent(getWorld()->get_Yoshi()); // subclasses handle whether yoshi landed on or moved over square
		}
	}
	else
		return;
}


///// COIN SQUARE //////

CoinSquare::CoinSquare(int adjust_coins_by, StudentWorld* world, int imageID, int startX, int startY)
	: Square(world, imageID, startX, startY), m_adjust_coins_by(adjust_coins_by)
{}


void CoinSquare::doSomethingDifferent(Player* player)
{
	// if player landed on square adjust their coins depending on what type of CoinSquare it is
	if (player->get_state() == 0) {
		if (m_adjust_coins_by == 3)
		{
			player->adjust_coins(m_adjust_coins_by);
			getWorld()->playSound(SOUND_GIVE_COIN);
		}
		else
		{
			getWorld()->playSound(SOUND_TAKE_COIN);
			if (player->get_coins() < 3)
				player->reset_coins(); // if player has less than 3 coins take away as many as they have
			else
				player->adjust_coins(m_adjust_coins_by);
		}
	}
}


///// BANK SQUARE //////

BankSquare::BankSquare(StudentWorld* world, int startX, int startY, int imageID)
	: Square(world, imageID, startX, startY)
{}

void BankSquare::doSomethingDifferent(Player* player)
{
	// if player landed on square
	if (player->get_state() == 0) 
	{
		player->adjust_coins(getWorld()->get_bank_coins());
		getWorld()->reset_bank_coins();
		getWorld()->playSound(SOUND_WITHDRAW_BANK);
	}
	// if player walked over square
	else
	{
		if (player->get_coins() < 5)
		{
			// if player has less than 5 coins, deposit however many they have into central bank
			getWorld()->deposit_bank_coins(player->get_coins());
			player->reset_coins();
		}
		else
		{
			getWorld()->deposit_bank_coins(5);
			player->adjust_coins(-5);
		}
		getWorld()->playSound(SOUND_DEPOSIT_BANK);
	}
}


////// DIRECTIONAL SQUARE ////////

DirectionalSquare::DirectionalSquare(StudentWorld* world, int startX, int startY, int dir, int imageID)
	: Square(world, imageID, startX, startY, dir), m_forced_direction(dir)
{}

void DirectionalSquare::doSomethingDifferent(Player* player)
{
	// checking state is not necessary but I added it in to keep the format consistent
	if (player->get_state() == 0 || player->get_state() == 1) {
		// update player walking/sprite direction based on directional square 
		if (m_forced_direction == left)
			player->force_walk_direction(left, 180);
		else
			player->force_walk_direction(m_forced_direction, 0); 
	}
}

////// DROPPING SQUARE /////////

DroppingSquare::DroppingSquare(StudentWorld* world, int startX, int startY, int imageID)
	: Square(world, imageID, startX, startY)
{}

void DroppingSquare::doSomethingDifferent(Player* player)
{
	// if player landed on square
	if (player->get_state() == 0)
	{
		// 50% chance droppingsquare will activate
		if (randInt(1, 2) == 1)
		{
			// will take away coins
			if (player->get_coins() < 10)
				player->reset_coins();
			else
				player->adjust_coins(-10);
		}
		else
		{
			// will take away star
			if (player->get_stars() >= 1)
				player->adjust_stars(-1);
		}
		getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
	}
}


////// EVENT SQUARE ////////

EventSquare::EventSquare(StudentWorld* world, int startX, int startY, int imageID)
	: Square(world, imageID, startX, startY)
{}

void EventSquare::doSomethingDifferent(Player* player)
{
	if (player->get_state() == 0)
	{
		// 1 in 3 chances of a situation happening
		int random = randInt(1, 3);

		// teleport player to random square 
		if (random == 1)
		{
			player->teleport_to_random_sq(); 
			getWorld()->playSound(SOUND_PLAYER_TELEPORT);
		}
		// swap player position and movement state
		else if (random == 2)
		{
			player->swap_positions();
			set_peach_new_false(); // update markers so event square won't activate on newly teleported player
			set_yoshi_new_false();
			getWorld()->playSound(SOUND_PLAYER_TELEPORT);
		}
		// give player a vortex projectile 
		else
		{
			if (player->has_vortex() == false)
				player->equip_vortex();
			getWorld()->playSound(SOUND_GIVE_VORTEX);
		}
	}
}

///// STAR SQUARE /////////

StarSquare::StarSquare(StudentWorld* world, int startX, int startY, int imageID)
	: Square(world, imageID, startX, startY)
{}

void StarSquare::doSomethingDifferent(Player* player)
{
	// checking state is not necessary but I added it in to keep the format consistent
	if (player->get_state() == 0 || player->get_state() == 1) {
		if (player->get_coins() < 20)
		{
			return;
		}
		else
		{
			// give player a star and take away coins
			player->adjust_coins(-20);
			player->adjust_stars(1);
			getWorld()->playSound(SOUND_GIVE_STAR);
		}
	}
}


/////////////////////
////    ENEMY   /////
/////////////////////

Enemy::Enemy(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth)
	: ActivatesOnPlayer(world, imageID, startX, startY, dir, depth),
	m_squares_to_move(0),
	m_ticks_to_pause(180),
	m_state(paused)
{}

void Enemy::doSomething()
{
	// if enemy is in PAUSED state
	if (get_state() == paused)
	{
		// reset peach and yoshi to "new" players after they move away from enemy 
		if (!getWorld()->intersecting_peach(this))
			set_peach_new_true();
		if (!getWorld()->intersecting_yoshi(this))
			set_yoshi_new_true();

		// if Enemy and Peach are on the same square, and Yoshi is in WAITING_TO_ROLL state
		if (getWorld()->enemy_lands_with_peach(this) && peach_is_new())
		{
			set_peach_new_false();
			doPausedAction(getWorld()->get_Peach());
		}
		// if Enemy and Yoshi are on the same square, and Yoshi is in WAITING_TO_ROLL state
		if (getWorld()->enemy_lands_with_peach(this) && peach_is_new())
		{
			set_yoshi_new_false();
			doPausedAction(getWorld()->get_Yoshi());
		}

		// decrement pause counter 
		decrement_ticks_to_pause();

		// check if pause counter reaches 0
		if (get_ticks_to_pause() == 0)
		{
			set_squares_to_move(randInt(1, 10));
			set_ticks_to_move(get_sq_to_move() * 8);

			// pick a new random direction to walk that is legal
			// update the direction the enemy sprite faces based on the walk direction
			reset_walk_direction();

			// set enemy state to WALKING
			set_state(walking); 
		}
	}

	// if enemy in WALKING state
	if (get_state() == walking)
	{
		// at a fork so will pick random valid walking direction
		if (getWorld()->isAtFork(this, get_walking_dir()))
		{
			reset_walk_direction();
		}
		// can't keep moving forward so will pick random valid perpendicular direction
		else if (!getWorld()->isValidDirection(this, get_walking_dir()))
		{
			move_perpendicularly();
		}

		// move 2 pixels in the walk direction	
		moveAtAngle(get_walking_dir(), 2);
		decrement_ticks_to_move();

		if (get_ticks_to_move() == 0)
		{
			set_state(paused); 
			set_ticks_to_pause(180);

			newDroppingSquare(this); // probably don't need to call with this
		}
	}
}

void Enemy::hit_by_vortex()
{
	// teleport baddie to random square
	teleport_to_random_sq();

	// set new walking/sprite direction properly 
	// set state to pause
	force_walk_direction(right, 0);
	set_state(paused);
	set_ticks_to_pause(180);
}

/////// BOWSER ////////

Bowser::Bowser( StudentWorld* world, int startX, int startY, int imageID)
	: Enemy(world, imageID, startX, startY)
{}

void Bowser::doPausedAction(Player* player)
{
	// 50% chance of activating 
	if (randInt(0, 1) == 1)
	{
		player->reset_coins();
		getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
	}
}

void Bowser::newDroppingSquare(Actor* actor)
{
	// 25% chance of bowser creating a dropping square
	if (randInt(1, 4) == 4)
	{
		// get a pointer to the square beneath bowser and set it to inactive
		Actor* sq = getWorld()->get_square_at_location(this->getX(), this->getY());
		sq->set_inactive();

		// add a new dropping square underneath bowser
		getWorld()->addNewActor(new DroppingSquare(getWorld(), this->getX(), this->getY()));
		getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
	}
}


///// BOO /////////

Boo::Boo(StudentWorld* world, int startX, int startY, int imageID)
	: Enemy(world, imageID, startX, startY)
{}

void Boo::doPausedAction(Player* player)
{
	// 50% chance of either event happening 
	if (randInt(0, 1) == 1)
	{
		// swap players coins
		player->swap_coins();
	}
	else
	{
		// swap player stars
		player->swap_stars();
	}
	getWorld()->playSound(SOUND_BOO_ACTIVATE);
}
