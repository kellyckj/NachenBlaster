/*
	By Kelly Cheng
*/

#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}



StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir), alienOnScreen(0), alienNeedToDestroy(0), s1(0), s2(0), s3(0), s(0), maxAlien(0)
{}

StudentWorld::~StudentWorld() { cleanUp(); }


void StudentWorld::addActor(Actor * a)
{
	actor.push_back(a);
}

void StudentWorld::destroyedAnAlien()
{
	alienNeedToDestroy--;
}

void StudentWorld::setAlienOffScreen()
{
	alienOnScreen--;
}

bool StudentWorld::willCollide(const Actor* a, const Actor* b) const
{
	return (sqrt(pow(a->getX() - b->getX(), 2) + pow(a->getY() - b->getY(), 2)) < (.75 * (a->getRadius() + b->getRadius())));
}

Actor* StudentWorld::getOneCollidingAlien(const Actor* a)
{ 
	for (vector<Actor*>::iterator p = actor.begin(); p != actor.end(); p++)
		if ((*p)->isAlien())
			if (willCollide(*p, a))
				return *p;
	return nullptr;
}

NachenBlaster* StudentWorld::getCollidingPlayer(const Actor* a) const
{
	if (willCollide(player, a))
		return player;
	return nullptr;
}

void StudentWorld::removeDeadGameObjects()
{
	vector<Actor*>::iterator p = actor.begin();	
	while (p != actor.end())
	{
		if ((*p)->isDead())
		{
			delete (*p);
			p = actor.erase(p);
		}
		else
			p++;
	}
}

void StudentWorld::updateDisplayText()
{
	ostringstream oss;
	oss << "Lives: " << getLives() << "  Health: " << player->getHit() * 2 << "%  Score: "
		<< getScore() << "  Level: " << getLevel() << "  Cabbages: " << player->getCabbage() * 100 / 30
		<< "%  Torpedoes: " << player->getTorpedo();

	string text = oss.str();
	setGameStatText(text);
}

NachenBlaster* StudentWorld::getPlayer() { return player; }

int StudentWorld::init()
{
	player = new NachenBlaster(this);
	for (int i = 0; i < 30; i++)
		addActor(new Star(this, randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1)));
	alienOnScreen = 0;
	alienNeedToDestroy = 6 + 4 * getLevel();
	s1 = 60;
	s2 = 20 + 5 * getLevel();
	s3 = 5 + 10 * getLevel();
	s = s1 + s2 + s3;
	maxAlien = 4 + 0.5 * getLevel();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// do something
	for (vector<Actor*>::iterator p = actor.begin(); p != actor.end(); p++)
	{
		(*p)->doSomething();
		if (player->isDead())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (alienNeedToDestroy <= 0)
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	player->doSomething();

	// introduce new actors
	if (randInt(1, 15) == 1)
		addActor(new Star(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
	if (alienOnScreen < min(maxAlien, alienNeedToDestroy))
	{
		int ran = randInt(1, s);
		if (ran <= s1)
			addActor(new Smallgon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		else if (ran > s1 && ran <= s1 + s2)
			addActor(new Smoregon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		else
			addActor(new Snagglegon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1)));
		alienOnScreen++;
	}

	removeDeadGameObjects();
	updateDisplayText();
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if (!actor.empty())
	{
		for (vector<Actor*>::iterator p = actor.begin(); p != actor.end(); p++)
			if ((*p) != nullptr)
			    delete (*p);
		actor.clear();
		delete player;
		player = nullptr;
	}
}
