#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>
class StudentWorld;


// Actor base class
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir = 0, double size = 1.0, unsigned int depth = 0);
	virtual ~Actor() {}
	virtual void doSomething() = 0;
	StudentWorld* getWorld() const;
	bool isDead() const;
	void killIt();
	bool onScreen() const;
	bool isAlien() const;
	//virtual bool collideWProjectile(const int pt);
private:
	bool dead;
	int m_ID;
	StudentWorld * m_world;
};


// NachenBlaster class
class NachenBlaster : public Actor
{
public:
	NachenBlaster(StudentWorld* world);
	virtual ~NachenBlaster() {}
	virtual void doSomething();
	int getCabbage() const;
	int getTorpedo() const;
	int getHit() const;
	void setHit(int amt);
	void increaseTorpedo(int amt);
private:
	int m_hit;
	int m_cabbage;
	int m_flatulenceTorpedo;
};


// Star class
class Star : public Actor
{
public:
	Star(StudentWorld* world, double startX, double startY);
	virtual ~Star() {}
	void doSomething();
};



// Alien base class
//	Smallgon derived class
//	Smoregon derived class
//	Snagglegon derived class
class Alien : public Actor
{
public:
	Alien(StudentWorld* world, int imageID, double startX, double startY, double speed, double hit);
	virtual ~Alien() {}
	virtual void doSomething() = 0;
	double getHit() const;
	void setHit(int amt);
	double getSpeed() const;
	void setSpeed(double amt);
	int getPlan() const;
	void setPlan(int amt);
	int getDir() const;
	void setDir(int amt);
	void collideWProjectile(const int pt);
	void collideWPlayer(Actor* a, int hitPt);      // snagglegon:15  // smoregon:5  // smallgon:5
	virtual void afterCollide() = 0;
	bool withinRange() const;
private:
	double m_hit;
	double m_speed;
	int m_plan;
	int m_dir;
};


class Smallgon : public Alien
{
public:
	Smallgon(StudentWorld* world, double startX, double startY);
	virtual ~Smallgon() {}
	virtual void doSomething();
	virtual void afterCollide();
};

class Smoregon : public Alien
{
public:
	Smoregon(StudentWorld* world, double startX, double startY);
	virtual ~Smoregon() {}
	virtual void doSomething();
	virtual void afterCollide();
};

class Snagglegon : public Alien
{
public:
	Snagglegon(StudentWorld* world, double startX, double startY);
	virtual ~Snagglegon() {}
	virtual void doSomething();
	virtual void afterCollide();
};


// Projectile base class
//	Cabbage derived class
//	Turnip derived class
//	FlatulenceTorpedoes derived class
class Projectile : public Actor
{
public:
	Projectile(StudentWorld* world, int imageID, double startX, double startY, Direction dir = 0);
	virtual ~Projectile() {}
	virtual void doSomething() = 0;
};

class Cabbage : public Projectile
{
public:
	Cabbage(StudentWorld* world, double startX, double startY);
	virtual ~Cabbage() {}
	virtual void doSomething();

};

class Turnip : public Projectile
{
public:
	Turnip(StudentWorld* world, double startX, double startY);
	virtual ~Turnip() {}
	virtual void doSomething();

};

class FlatulenceTorpedo : public Projectile
{
public:
	FlatulenceTorpedo(StudentWorld* world, int id, double startX, double startY, Direction dir);
	virtual ~FlatulenceTorpedo() {}
	virtual void doSomething();
private:
	int ID;
};




// Explosion class
class Explosion : public Actor
{
public:
	Explosion(StudentWorld* world, double startX, double startY);
	virtual ~Explosion() {}
	virtual void doSomething();
private:
	double m_size;
};


// Goody base class
//	Repair Goodies derived class
//	Extra Life Goodies derived class
//	Flatulence Torpedo Goodies derived class
class Goodie : public Actor
{
public:
	Goodie(StudentWorld* world, int imageID, double startX, double startY);
	virtual ~Goodie() {}
	void doSomething();
	void collideWPlayer();
	virtual void afterCollide() = 0;
};

class RepairG : public Goodie
{
public:
	RepairG(StudentWorld* world, double startX, double startY);
	virtual ~RepairG() {}
	//virtual void doSomething();
	virtual void afterCollide();
};

class ExtraLifeG : public Goodie
{
public:
	ExtraLifeG(StudentWorld* world, double startX, double startY);
	virtual ~ExtraLifeG() {}
	//virtual void doSomething();
	virtual void afterCollide();
};

class FlatulenceTorpedoG : public Goodie
{
public:
	FlatulenceTorpedoG(StudentWorld* world, double startX, double startY);
	virtual ~FlatulenceTorpedoG() {}
	//virtual void doSomething();
	virtual void afterCollide();
};


#endif // ACTOR_H_