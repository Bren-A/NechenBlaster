#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

// Added classes
#include <list>
#include "Actor.h"

// New constants
const int MAX_STARS_ON_SCREEN = 30; // Makes sure there aren't too many stars on screen

class Actor;
class NachenBlaster;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	// Returns the beginning of the list
	std::list<Actor*>::iterator getBegin();
	// Returns the end of the list
	std::list<Actor*>::iterator getEnd();

	// Adds Actors to the list
	void addObjects(Actor* a);

	// Returns pointer to actor at given index
	NachenBlaster* getShip();
	~StudentWorld();
private:
	// Prints out gameStats at the top of the screen
	void printHead();
	// Determines if a new ship needs to be added or not
	bool addShip();
	void addEnemy();
	int enemiesOnScreen();
	// Sets stars for the beginning of the game
	void setStars();
	// Creates a set number of stars and plots them randomly between two given x values
	void setStarsRange(int nStars, int r1, int r2);

	// Determines if level won or not
	bool beatLevel() const;
	// Odds to add a new star during the tick
	bool addStar();
	std::list<Actor*> m_objects;
	// Keeps track of the number of stars on screen
	int m_nStars;
	int m_nKills;
	// Keeps track of the current ship item
	NachenBlaster* m_ship; 
	// Assigns ship to m_ship
	void setShip(NachenBlaster* curr);
};

#endif // STUDENTWORLD_H_
