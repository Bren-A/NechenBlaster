#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int id, int x, int y, StudentWorld* world = nullptr, int depth = 0) : 
		m_killed(false), m_world(world), m_dead(false), GraphObject(id, x, y, 0, 1, depth) 
	{
	
	}
	// Checks if it can interact with the player or not
	virtual bool isEnemy() const ; // Enemy objects
	virtual bool isGoodie() const;

	// Returns true if Actor is the player character or not
	virtual bool isPlayer() const;
	virtual void doSomething() = 0;

	void dies(); // Changes to dead state
	bool isDead() const; // Returns whether the Actor is dead or not (i.e. Actually dies or goes off the screen)
	
	// Gets sprite ID to figure out what kind of object it is
	virtual int getID() const = 0; 
	
	// Decreases health for actors that have health
	virtual void damage(int n);
	
	bool isKilled() const;
	void kill();
	// Checks if the current object is touching another object
	bool isTouching(Actor& b) const;
	void setWorld(StudentWorld* world);
	StudentWorld* getWorld() const;
	virtual ~Actor();
private:
	StudentWorld* m_world;
	bool m_dead;
	bool m_killed;
};

// Constants for NachenBlaster
const int MAX_CABBAGE_POINTS = 30;
const int MAX_HEALTH = 50;

class NachenBlaster : public Actor
{
public:
	NachenBlaster(StudentWorld* curr) : 
		m_health(MAX_HEALTH), m_cabb(MAX_CABBAGE_POINTS), m_torps(0), Actor(IID_NACHENBLASTER, 0, 128, curr)
	{
	}
	void doSomething();
	bool isPlayer() const;
	int getID() const;
	int getHealth() const;
	int cabbPower() const;
	int nTorps() const;
	void changeTorps(bool inc);
	void damage(int n);
	// Sets health to 50
	void flush();
	~NachenBlaster();
private:
	
	int m_health;
	int m_cabb;
	int m_torps;
};

// Enemies 
class Enemy : public Actor
{
public:
	Enemy(int id, int x, int y, StudentWorld* curr) 
		: m_dir(Left), m_health(10), m_speed(0), m_plan(0), Actor(id, x, y, curr, 1)
	{
	}

	enum dir { UpLeft = 0, Left = 1, DownLeft = 2 };
	bool isEnemy() const;
	void setHealth(int h);
	int getHealth() const;
	void setSpeed(double s);
	double getSpeed() const;
	void changePlan(int p);
	int getPlan() const;

	// Determines what goodies to drop
	virtual void dropGoodie();
	// Functions for if it is/should move left, northwest, or southwest
	int getDir() const;
	void changeDir(int d);
	// Checks if Enemy has crashed into ship and died
	virtual bool hitsShip();
	void damage(int n);

	// Checks if it is dead or goes off screen
	virtual bool remove();
	void moveShip();
	// Determines how the ship will move
	virtual void checkPlan();

	// Determines if the ship will fire a turnip
	void fireTurnip();

	// Determines odds that they should attack
	virtual bool attack();
private:
	// Keeps track of ships current direction
	int m_dir;
	int m_health;
	double m_speed;
	int m_plan;
};

class Smallgon : public Enemy
{
public:
	Smallgon(int h, int x, int y, StudentWorld* curr) : Enemy(IID_SMALLGON, x, y, curr)
	{	
		setSize(1.5);
		//setHealth(5 * (1 + (getWorld()->getLevel() - 1) * .1));
		setHealth(h);
		setSpeed(2.0);
	}
	void doSomething();
	int getID() const;
private:

};
class Smoregon : public Enemy
{
public:
	Smoregon(int h, int x, int y, StudentWorld* curr) : Enemy(IID_SMOREGON, x, y, curr)
	{
		setSize(1.5);
		//setHealth(5 * (1 + (getWorld()->getLevel() - 1) * .1));
		setHealth(h);
		setSpeed(2.0);
	}
	int getID() const;
	void doSomething();
	void dropGoodie();
private:

};

class Snagglegon : public Enemy
{
public:
	Snagglegon(int h, int x, int y, StudentWorld* curr) : Enemy(IID_SNAGGLEGON, x, y, curr)
	{
		setSize(1.5);
		//setHealth(10 * (1 + (getWorld()->getLevel() - 1) * .1));
		setHealth(h);
		setSpeed(1.75);
		changeDir(DownLeft);
	}
	int getID() const;
	void doSomething();
	void checkPlan();
	bool attack();
	void dropGoodie();
	bool remove();
	bool hitsShip();
private:

};

// Projectiles
class Projectile : public Actor
{
public:
	Projectile(int id, int x, int y, StudentWorld* curr) : Actor(id, x, y, curr,  1)
	{
		setSize(.5);
	}
	// Depending if projectile is enemy projectile or not, it will decide if it's attacking properly
	void checkDamage(int n, bool enemy);
private:

};

class Cabbage : public Projectile
{
public:
	Cabbage(int x, int y, StudentWorld* curr) : Projectile(IID_CABBAGE, x, y, curr)
	{
	}
	int getID() const;
	void doSomething();
};

class Turnip : public Projectile
{
public:
	Turnip(int x, int y, StudentWorld* curr) : Projectile(IID_TURNIP, x, y, curr)
	{
	}
	int getID() const;
	void doSomething();
	
};

class Fartpedo : public Projectile
{
public:
	Fartpedo(int x, int y, bool enemy, StudentWorld* curr) : m_enemy(enemy), Projectile(IID_TORPEDO, x, y, curr)
	{
		if (enemy)
			setDirection(180);
	}
	int getID() const;
	void doSomething();
private:
	bool m_enemy;
};

// Items

class Goodie : public Actor
{
public: 
	Goodie(int id, int x, int y, StudentWorld* curr) : Actor(id, x, y, curr, 1)
	{
		setSize(.5);
	}
	bool isGoodie();
	bool getGoodie();
};

class Repair : public Goodie
{
public:
	Repair(int x, int y, StudentWorld* curr) : Goodie(IID_REPAIR_GOODIE, x, y, curr)
	{
	}
	int getID() const;
	void doSomething();
};

class Life : public Goodie
{
public:
	Life(int x, int y, StudentWorld* curr) : Goodie(IID_LIFE_GOODIE, x, y, curr)
	{
	}
	int getID() const;
	void doSomething();
private:
};

class Torpedo : public Goodie
{
public:
	Torpedo(int x, int y, StudentWorld* curr) : Goodie(IID_TORPEDO_GOODIE, x, y, curr)
	{
	}
	int getID() const;
	void doSomething();
};

// Passive
class Star : public Actor
{
public:
	Star(int x, int y) : Actor(IID_STAR, x, y, nullptr, 3) // Depth of 3
	{
		// Random size between .05 and .5
		setSize(randInt(5, 50) / 100.0);
	}
	void doSomething();
	int getID() const;
private:
};

const int NUM_TICKS = 4;

class Explosion : public Actor
{
public:
	Explosion(int x, int y, StudentWorld* curr) : m_ticks(0), Actor(IID_EXPLOSION, x, y, curr)
	{
		setSize(1);
	}
	void doSomething();
	int getID() const;
private:
	int m_ticks;
};

#endif // ACTOR_H_