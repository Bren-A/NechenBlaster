#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

// My additions
#include <list>
#include <iostream>
#include "Actor.h"

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: m_ship(nullptr), m_nStars(0), m_nKills(0), GameWorld(assetDir)
{
	
}

int StudentWorld::init()
{
	setShip(new NachenBlaster(this));
	m_objects.push_back(m_ship);
	
	setStars();
	m_nKills = 0;
	return GWSTATUS_CONTINUE_GAME;
}

NachenBlaster* StudentWorld::getShip()
{
	return m_ship;
}

void StudentWorld::setShip(NachenBlaster* a)
{
	m_ship = a;
}

int StudentWorld::enemiesOnScreen()
{
	int enemies = 0;
	for (list<Actor*>::iterator i = m_objects.begin(); i != m_objects.end(); i++)
		if ((*i)->isEnemy())
			enemies++;
	return enemies;
}

bool StudentWorld::addShip()
{
	// Remaining ships to destroy
	double remaining = (6 + (4 * getLevel())) - m_nKills;
	double maxEnemies = 4 + (.5*getLevel());
	int onScreen = enemiesOnScreen();
	if (onScreen < maxEnemies && onScreen < remaining)
		return true;
	return false;
}

void StudentWorld::addEnemy()
{
	int s1 = 60;
	int s2 = 20 + getLevel() * 5;
	int s3 = 5 + getLevel() * 10;
	int s = s1 + s2 + s3;
	int odds = randInt(1, s);
	if (odds < s3)
		m_objects.push_back(new Snagglegon(10 * (1 + (getLevel() - 1) * .1), VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
	else if (odds >= s3 && odds < s3 + s2)
		m_objects.push_back(new Smoregon(5 * (1 + (getLevel() - 1) * .1), VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
	else
		m_objects.push_back(new Smallgon(5 * (1 + (getLevel() - 1) * .1), VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
}

bool StudentWorld::addStar()
{
	if (randInt(0, 14) == 7)
		return true;
	return false;
}

bool StudentWorld::beatLevel() const
{
	if (((6 + (4 * getLevel())) - m_nKills) <= 0)
		return true;
	return false;
}
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	
	list<Actor*>::iterator i = m_objects.begin();
	while (m_objects.end() != m_objects.begin() && i != m_objects.end())
	{
		(*i)->doSomething();
		if ((*i)->isDead())
		{
			// Set new iterator to object currently needed to delete
			list<Actor*>::iterator temp = i;

			// Check if it's at the beginning of the list
			if (i == m_objects.begin())
			{
				i++; i++; // Move forward instead of back 
			}

			i--;

			// Decrement starcount if dead one was star
			//if ((*temp)->getID() == IID_STAR)
				//m_nStars--;
			// Increment to keep track of enemy kills
			if ((*temp)->isKilled())
				m_nKills++;
			// Delete object at iterator
			delete *temp;

			// Remove iterator
			m_objects.erase(temp);

			// Add more stars if they have gone off screen and have been deleted
			//if (m_nStars <= MAX_STARS_ON_SCREEN)
				//m_objects.push_back(new Star(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		}
		i++;
	}
	if (m_ship->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (beatLevel())
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	if (addShip())
		addEnemy();
	if(addStar())
		m_objects.push_back(new Star(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
	printHead();
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator i = m_objects.begin();
	while (m_objects.end() != m_objects.begin() && i != m_objects.end())
	{
		delete *i;
		m_objects.erase(i);
		i = m_objects.begin();
	}
}

void StudentWorld::addObjects(Actor* a)
{
	m_objects.push_back(a);
}

std::list<Actor*>::iterator StudentWorld::getBegin()
{
	return m_objects.begin();
}

std::list<Actor*>::iterator StudentWorld::getEnd()
{
	return m_objects.end();
}

void StudentWorld::setStars()
{
		setStarsRange(30, 0, VIEW_WIDTH - 1);
}

void StudentWorld::setStarsRange(int n, int r1, int r2)
{
	for (int i = 0; i < n; i++)
	{
		m_objects.push_back(new Star(randInt(r1, r2), randInt(0, VIEW_HEIGHT - 1)));
		m_nStars++;
	}
}

void StudentWorld::printHead()
{
	string head = "";
	head += "Lives: " + to_string(getLives());
	head += "  Health: " + to_string(m_ship->getHealth()) + "%";
	head += "  Cabbages: " + to_string(m_ship->cabbPower()) + "%";
	head += "  Score: " + to_string(getScore());
	head += "  Level: " + to_string(getLevel());
	head += "  Torpedoes: " + to_string(m_ship->nTorps());
	setGameStatText(head);
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

