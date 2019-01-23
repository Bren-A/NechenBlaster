#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <iostream>

using namespace std;

// Calculates the distance between two points
double dist(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
}

/////////////////////
// Actor functions //
/////////////////////

void Actor::dies()
{
	m_dead = true;
}

bool Actor::isDead() const
{
	return m_dead;
}

bool Actor::isTouching(Actor& b) const
{
	if (dist(getX(), getY(), b.getX(), b.getY())
		< (.75 * (getRadius() + b.getRadius())))
		return true;
	return false;
}

bool Actor::isPlayer() const
{
	return false;
}

bool Actor::isEnemy() const
{
	return false;
}

bool Actor::isGoodie() const
{
	return false;
}

bool Actor::isKilled() const
{
	return m_killed;
}

void Actor::kill()
{
	m_killed = true;
}

void Actor::damage(int n)
{
}

StudentWorld* Actor::getWorld() const
{
	return m_world;
}

void Actor::setWorld(StudentWorld* world)
{
	m_world = world;
}

Actor::~Actor()
{
}

/////////////////////////////
// NachenBlaster functions //
/////////////////////////////

bool NachenBlaster::isPlayer() const
{
	return true;
}

void NachenBlaster::doSomething()
{
	// Check to see if still alive, else return
	if (isDead())
		return;
	if (m_health <= 0)
		dies();
	// Check to see button pressed
	int key;
	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_LEFT:
			if (getX() > 5)
				moveTo(getX() - 6, getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getX() < VIEW_WIDTH - 6)
				moveTo(getX() + 6, getY());
			break;
		case KEY_PRESS_DOWN:
			if (getY() > 5)
				moveTo(getX(), getY() - 6);
			break;
		case KEY_PRESS_UP:
			if (getY() < VIEW_HEIGHT - 6)
				moveTo(getX(), getY() + 6);
			break;
		// If space is pressed && there's enough cabbage points to shoot
		case KEY_PRESS_SPACE:
			if (m_cabb >= 5)
			{
				m_cabb -= 5; // Decrement by 5 per shot
				getWorld()->addObjects(new Cabbage(getX() + 12, getY(), getWorld()));
				getWorld()->playSound(SOUND_PLAYER_SHOOT);
			}
			break;
		case KEY_PRESS_TAB:
			if (m_torps > 0)
			{
				m_torps--;
				getWorld()->addObjects(new Fartpedo(getX() + 12, getY(), isEnemy(), getWorld()));
				getWorld()->playSound(SOUND_TORPEDO);
			}
			break;
		}
	}
	if (m_cabb < MAX_CABBAGE_POINTS)
		m_cabb++;
}

int NachenBlaster::getHealth() const
{
	return (double(m_health)/MAX_HEALTH) * 100;
}

int NachenBlaster::cabbPower() const
{
	return (double(m_cabb) / MAX_CABBAGE_POINTS) * 100;
}

int NachenBlaster::nTorps() const
{
	return m_torps;
}

void NachenBlaster::changeTorps(bool inc)
{
	if (inc)
		m_torps += 5;
	else
		m_torps--;
}

int NachenBlaster::getID() const
{
	return IID_NACHENBLASTER;
}

void NachenBlaster::damage(int n)
{
	m_health -= n;
}

void NachenBlaster::flush()
{
	m_health = MAX_HEALTH;
}

NachenBlaster::~NachenBlaster()
{
}

/////////////////////
// Enemy functions //
/////////////////////

bool Enemy::isEnemy() const
{
	return true;
}

void Enemy::setHealth(int h)
{
	m_health = h;
}

int Enemy::getHealth() const
{
	return m_health;
}

void Enemy::setSpeed(double s)
{
	m_speed = s;
}

double Enemy::getSpeed() const
{
	return m_speed;
}

void Enemy::changePlan(int p)
{
	m_plan = p;
}

int Enemy::getPlan() const
{
	return m_plan;
}

int Enemy::getDir() const
{
	return m_dir;
}

void Enemy::changeDir(int d)
{
	m_dir = d;
}

bool Enemy::hitsShip()
{
	if (isTouching(*(getWorld()->getShip())))
	{
		dies();
		kill();
		(getWorld()->getShip())->damage(5);
		getWorld()->increaseScore(250);
		getWorld()->playSound(SOUND_DEATH);
		getWorld()->addObjects(new Explosion(getX(), getY(), getWorld()));
		return true;
	}
	return false;
}

void Enemy::damage(int n)
{
	m_health -= n;
	if (m_health > 0)
		getWorld()->playSound(SOUND_BLAST);
}

void Enemy::moveShip()
{
	switch (getDir())
	{
	case Left:
		moveTo(getX() - getSpeed(), getY());
		break;
	case UpLeft:
		moveTo(getX() - getSpeed(), getY() + getSpeed());
		break;
	case DownLeft:
		moveTo(getX() - getSpeed(), getY() - getSpeed());
		break;
	}
	changePlan(getPlan() - 1);
}

bool Enemy::remove()
{
	if (getX() < 0 || getHealth() <= 0)
	{
		dies();
		if (getHealth() <= 0)
		{
			// Increase player score if enemy health reaches zero
			kill();
			getWorld()->increaseScore(250);
			getWorld()->playSound(SOUND_DEATH);
			getWorld()->addObjects(new Explosion(getX(), getY(), getWorld()));
		}
		return true;
	}
	return false;
}

void Enemy::checkPlan()
{
	if (getPlan() == 0 || getY() >= VIEW_HEIGHT - 1 || getY() < 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)
			changeDir(DownLeft);
		else if (getY() <= 0)
			changeDir(UpLeft);
		else
			changeDir(randInt(0, 2));
		changePlan(randInt(1, 32));
	}
}

bool Enemy::attack()
{
	// If NachenBlaster is left of ship
	if (getWorld()->getShip()->getX() < getX())
		// And is within abs value of 4 to Smallgon
		if (abs(getWorld()->getShip()->getY() - getY()) <= 4)
			// Odds to fire
			if (randInt(1, 100) <= (1.0 / ((20.0 / getWorld()->getLevel()) + 5)) * 100)
				return true;
	return false;
}
void Enemy::fireTurnip()
{
		getWorld()->addObjects(new Turnip(getX() - 14, getY(), getWorld()));
		getWorld()->playSound(SOUND_ALIEN_SHOOT); 
}

void Enemy::dropGoodie()
{}

////////////////////////
// Smallgon functions //
////////////////////////

void Smallgon::doSomething()
{
	if (isDead())
		return;
	if (remove())
		return;
	hitsShip();
	// Changes direction if path becomes 0, or hits top/bottom of screen
	checkPlan();
	if (attack())
	{
		fireTurnip();
		return;
	}
	moveShip();
	hitsShip();
}

int Smallgon::getID() const
{
	return IID_SMALLGON;
}

////////////////////////
// Smoregon functions //
////////////////////////

void Smoregon::doSomething()
{
	if (isDead())
		return;
	if (getHealth() <= 0)
		dropGoodie();
	if (remove())
		return;
	if (hitsShip())
		dropGoodie();
	checkPlan();
	// Either attacks or rams player with equal odds
	if (attack())
	{
		fireTurnip();
		return;
	}
	else if (attack())
	{
		changeDir(Left);
		changePlan(VIEW_WIDTH);
		setSpeed(5);
	}
	moveShip();
	if (hitsShip())
		dropGoodie();
}

void::Smoregon::dropGoodie()
{
	if (randInt(0, 2) < 1)
	{
		if (randInt(0, 1) == 1)
			getWorld()->addObjects(new Repair(getX(), getY(), getWorld()));
		else
			getWorld()->addObjects(new Torpedo(getX(), getY(), getWorld()));
	}
}

int Smoregon::getID() const
{
	return IID_SMOREGON;
}

//////////////////////////
// Snagglegon functions //
//////////////////////////

int Snagglegon::getID() const
{
	return IID_SNAGGLEGON;
}

bool Snagglegon::attack()
{
	// If NachenBlaster is left of ship
	if (getWorld()->getShip()->getX() < getX())
		// And is within abs value of 4 to Smallgon
		if (abs(getWorld()->getShip()->getY() - getY()) <= 4)
			// Odds to fire
			if (randInt(1, 100) <= (1.0 / ((15.0 / getWorld()->getLevel()) + 10)) * 100)
				return true;
	return false;
}

void Snagglegon::checkPlan()
{
	if (getPlan() == 0 || getY() >= VIEW_HEIGHT - 1 || getY() < 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)
			changeDir(DownLeft);
		else if (getY() <= 0)
			changeDir(UpLeft);
	}
}

bool Snagglegon::hitsShip()
{
	if (isTouching(*(getWorld()->getShip())))
	{
		dies();
		kill();
		(getWorld()->getShip())->damage(15);
		getWorld()->increaseScore(1000);
		getWorld()->playSound(SOUND_DEATH);
		getWorld()->addObjects(new Explosion(getX(), getY(), getWorld()));
		return true;
	}
	return false;
}

bool Snagglegon::remove()
{
	if (getX() < 0 || getHealth() <= 0)
	{
		dies();
		if (getHealth() <= 0)
		{
			kill();
			// Increase player score if enemy health reaches zero
			getWorld()->increaseScore(1000);
			getWorld()->playSound(SOUND_DEATH);
			getWorld()->addObjects(new Explosion(getX(), getY(), getWorld()));
		}
		return true;
	}
	return false;
}

void::Snagglegon::dropGoodie()
{
	if (randInt(0, 5) == 3)
	{
		getWorld()->addObjects(new Life(getX(), getY(), getWorld()));
	}
}

void Snagglegon::doSomething()
{
	if (isDead())
		return;
	if (getHealth() <= 0)
		dropGoodie();
	if (remove())
		dies();
	if (hitsShip())
		dropGoodie();
	checkPlan();
	if(attack())
	{ 
		getWorld()->addObjects(new Fartpedo(getX() - 14, getY(), isEnemy(), getWorld()));
		getWorld()->playSound(SOUND_TORPEDO);
		return;
	}
	moveShip();
	if (hitsShip())
		dropGoodie();
}

//////////////////////////
// Projectile functions //
//////////////////////////

void Projectile::checkDamage(int n, bool enemy)
{
	if (isDead())
		return;
	// Check if it is fired from NachenBlaster or not
	if (!enemy)
	{
		// Loop through all the items in the list
		for (list<Actor*>::iterator i = getWorld()->getBegin(); getWorld()->getBegin() != getWorld()->getEnd()
			&& i != getWorld()->getEnd();
			i++)
			if ((*i)->isEnemy() && isTouching(*(*i))) // If object is enemy and cabbage is touching the enemy
			{
				dies();
				(*i)->damage(n); // Deal damage
			}
	}
	else if (enemy)
	{
		if (isTouching(*getWorld()->getShip()))
		{
			dies();
			getWorld()->getShip()->damage(n);
			getWorld()->playSound(SOUND_BLAST);
		}
	}
}

///////////////////////
// Cabbage functions //
///////////////////////

const int CABBAGE_DMG = 2;

void Cabbage::doSomething()
{
	// Do nothing if it is dead
	if (isDead())
		return;
	
	// Dies if goes off the screen
	if (getX() >= VIEW_WIDTH)
		dies();
	
	// Check if it can deal damage, if possible, do so
	checkDamage(CABBAGE_DMG, false);

	moveTo(getX() + 8, getY());
	setDirection(getDirection() + 20);

	// Check if can now deal damage after moving
	checkDamage(CABBAGE_DMG, false);
}

int Cabbage::getID() const
{
	return IID_CABBAGE;
}

//////////////////////
// Turnip functions //
//////////////////////

const int TURNIP_DMG = 2;

int Turnip::getID() const
{
	return IID_TURNIP;
}

void Turnip::doSomething()
{
	// Do nothing if it is dead
	if (isDead())
		return;

	// Dies if goes off the screen
	if (getX() < 0)
		dies();

	checkDamage(TURNIP_DMG, true);

	moveTo(getX() - 6, getY());
	setDirection(getDirection() + 20);

	// Check if can now deal damage after moving
	checkDamage(TURNIP_DMG, true);
}

////////////////////////
// Fartpedo functions //
////////////////////////

const int TORPEDO_DMG = 8;

int Fartpedo::getID() const
{
	return IID_TORPEDO;
}

void Fartpedo::doSomething()
{
	if (isDead())
		return;
	if (getX() < 0 || getX() >= VIEW_WIDTH)
		dies();
	
	if (m_enemy)
		checkDamage(TORPEDO_DMG, true);
	else 
		checkDamage(TORPEDO_DMG, false);
	int direction = 1;
	
	if (m_enemy) // If enemy switch direction
		direction *= -1;
	moveTo(getX() + (8 * direction), getY());
	
	if (m_enemy)
		checkDamage(TORPEDO_DMG, true);
	else
		checkDamage(TORPEDO_DMG, false);

}

//////////////////////
// Goodie functions //
//////////////////////

bool Goodie::isGoodie()
{
	return true;
}

bool Goodie::getGoodie()
{
	if (!isDead() && isTouching(*(getWorld()->getShip())))
	{
		getWorld()->increaseScore(100);
		dies();
		getWorld()->playSound(SOUND_GOODIE);
		return true;
	}
	return false;
}

//////////////////////
// Repair functions //
//////////////////////
int Repair::getID() const
{
	return IID_REPAIR_GOODIE;
}

void Repair::doSomething()
{
	if (isDead())
		return;
	if ((getX() < 0 || getX() >= VIEW_WIDTH)
		|| (getY() < 0 || getY() >= VIEW_HEIGHT))
		dies();
	if (getGoodie())
	{
		getWorld()->getShip()->damage(-10);
		if (getWorld()->getShip()->getHealth() > MAX_HEALTH)
			getWorld()->getShip()->flush();
		return;
	}
	moveTo(getX() - .75, getY() - .75);
	if (getGoodie())
	{
		getWorld()->getShip()->damage(-10);
		if (getWorld()->getShip()->getHealth() > 50)
			getWorld()->getShip()->flush();
	}
}

///////////////////////
// Torpedo functions //
///////////////////////

int Torpedo::getID() const
{
	return IID_TORPEDO_GOODIE;
}

void Torpedo::doSomething()
{
	if (isDead())
		return;
	if ((getX() < 0 || getX() >= VIEW_WIDTH)
		|| (getY() < 0 || getY() >= VIEW_HEIGHT))
		dies();
	if (getGoodie())
	{
		getWorld()->getShip()->changeTorps(true);
		return;
	}
	moveTo(getX() - .75, getY() - .75);
	if (getGoodie())
		getWorld()->getShip()->changeTorps(true);
}

///////////////////////////
// Life goodie functions //
///////////////////////////

int Life::getID() const
{
	return IID_LIFE_GOODIE;
}
void Life::doSomething()
{
	if (isDead())
		return;
	if ((getX() < 0 || getX() >= VIEW_WIDTH) 
		|| (getY() < 0 || getY() >= VIEW_HEIGHT))
		dies();
	// If goodie is touching player, increase lives and score
	if (getGoodie())
	{
		getWorld()->incLives();
		return;
	}
	moveTo(getX() - .75, getY() - .75);
	if (getGoodie())
		getWorld()->incLives();
}

/////////////////////////
// Explosion functions //
/////////////////////////

void Explosion::doSomething()
{
	if (m_ticks < 4)
	{
		setSize(getSize() * 1.5);
		m_ticks++;
	}
	else
		dies();
}

int Explosion::getID() const
{
	return IID_EXPLOSION;
}

////////////////////
// Star functions //
////////////////////

void Star::doSomething()
{
	if (getX() < 0)
		dies();
	moveTo(getX() - 1, getY());
}
int Star::getID() const
{
	return IID_STAR;
}

