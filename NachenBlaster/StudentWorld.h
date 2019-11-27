/*
	By Kelly Cheng
*/

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	void addActor(Actor* a);
	void destroyedAnAlien();
	void setAlienOffScreen();
	void updateDisplayText();
	void removeDeadGameObjects();
	NachenBlaster * getPlayer();
	bool willCollide(const Actor* a, const Actor* b) const;
	Actor* getOneCollidingAlien(const Actor* a);
	NachenBlaster* getCollidingPlayer(const Actor* a) const;
private:
	std::vector<Actor*> actor;
	NachenBlaster* player;
	int alienOnScreen;
	int alienNeedToDestroy;
	int s1;
	int s2;
	int s3;
	int s;
	int maxAlien;
};

#endif // STUDENTWORLD_H_
