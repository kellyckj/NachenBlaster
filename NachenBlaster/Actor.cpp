/*
	By Kelly Cheng
*/

#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include <iostream>


//////////////////////////////////////////////////////////////////////////////////
// Actor //
//////////////////////////////////////////////////////////////////////////////////
Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, dir, size, depth), dead(false), m_ID(imageID), m_world(world)
{}

StudentWorld* Actor::getWorld() const { return m_world; }

bool Actor::isDead() const { return dead; }

void Actor::killIt() { dead = true; }

bool Actor::isAlien() const
{
	return (m_ID == IID_SMALLGON || m_ID == IID_SMOREGON || m_ID == IID_SNAGGLEGON);
}

bool Actor::onScreen() const
{
	if (getX() >= 0 && getX() < VIEW_WIDTH && getY() >= 0 && getY() < VIEW_HEIGHT)
		return true;
	return false;
}




//////////////////////////////////////////////////////////////////////////////////
// NachenBlaster (player) //
//////////////////////////////////////////////////////////////////////////////////
NachenBlaster::NachenBlaster(StudentWorld* world)
	: Actor(world, IID_NACHENBLASTER, 0, 128, 0, 1.0, 0), m_hit(50), m_cabbage(30), m_flatulenceTorpedo(0)
{}

void NachenBlaster::doSomething()
{
	if (m_hit <= 0)
		killIt();
	if (isDead())
		return;

	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (m_cabbage >= 5)
			{
				m_cabbage = m_cabbage - 5;
				getWorld()->addActor(new Cabbage(getWorld(), getX() + 12, getY()));
				getWorld()->playSound(SOUND_PLAYER_SHOOT);
			}
			break;
		case KEY_PRESS_TAB:
			if (getTorpedo() > 0)
			{
				m_flatulenceTorpedo--;
				getWorld()->addActor(new FlatulenceTorpedo(getWorld(), IID_NACHENBLASTER, getX() + 12, getY(), 0));
				getWorld()->playSound(SOUND_TORPEDO);
			}
			break;
		case KEY_PRESS_LEFT:
			if (getX() - 6 >= 0)
				moveTo(getX() - 6, getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getX() + 6 < VIEW_WIDTH)
				moveTo(getX() + 6, getY());
			break;
		case KEY_PRESS_DOWN:
			if (getY() - 6 >= 0)
				moveTo(getX(), getY() - 6);
			break;
		case KEY_PRESS_UP:
			if (getY() + 6 < VIEW_HEIGHT)
				moveTo(getX(), getY() + 6);
			break;
		}
	}

	if (m_cabbage < 30)
		m_cabbage++;
	if (m_cabbage <= 0)
		m_cabbage = 0;
}

int NachenBlaster::getCabbage() const { return m_cabbage; }

int NachenBlaster::getTorpedo() const { return m_flatulenceTorpedo; }

int NachenBlaster::getHit() const { return m_hit; }

void NachenBlaster::setHit(int amt)
{
	if (m_hit - amt <= 0)
		killIt();
	else if (amt == 50)
		m_hit = 50;
	else
		m_hit -= amt;
}

void NachenBlaster::increaseTorpedo(int amt)
{
	m_flatulenceTorpedo += amt;
}



//////////////////////////////////////////////////////////////////////////////////
// Star //
//////////////////////////////////////////////////////////////////////////////////
Star::Star(StudentWorld* world, double startX, double startY)
	: Actor(world, IID_STAR, startX, startY, 0, randInt(5, 50) / 100.0, 3)
{}

void Star::doSomething()
{
	if (getX() <= -1)
		killIt();
	moveTo(getX() - 1, getY());
}



//////////////////////////////////////////////////////////////////////////////////
// Aliens //
//////////////////////////////////////////////////////////////////////////////////
Alien::Alien(StudentWorld* world, int imageID, double startX, double startY, double speed, double hit)
	: Actor(world, imageID, startX, startY, 0, 1.5, 1), m_hit(hit), m_speed(speed),
	m_plan(0), m_dir(0)
{}

double Alien::getHit() const { return m_hit; }

void Alien::setHit(int amt) { m_hit -= amt; }

double Alien::getSpeed() const { return m_speed; }

void Alien::setSpeed(double amt) { m_speed = amt; }

int Alien::getPlan() const { return m_plan; }

void Alien::setPlan(int amt) { m_plan = amt; }

int Alien::getDir() const { return m_dir; }

void Alien::setDir(int amt) { m_dir = amt; }

void Alien::collideWProjectile(const int pt)
{
	setHit(pt);
	if (getHit() <= 0)
	{
		killIt();
		getWorld()->destroyedAnAlien();
		getWorld()->setAlienOffScreen();
		getWorld()->playSound(SOUND_DEATH);
		getWorld()->addActor(new Explosion(getWorld(), getX(), getY()));
		afterCollide();
	}
	else
		getWorld()->playSound(SOUND_BLAST);
}

void Alien::collideWPlayer(Actor* a, int hitPt)    // hit points:  // snagglegon:15  // smoregon:5  // smallgon:5
{
	if (getWorld()->willCollide(this, getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->setHit(hitPt);
		killIt();
		getWorld()->destroyedAnAlien();
		getWorld()->setAlienOffScreen();
		getWorld()->playSound(SOUND_DEATH);
		getWorld()->addActor(new Explosion(getWorld(), getX(), getY()));
		static_cast<Alien*>(a)->afterCollide();
	}
}

bool Alien::withinRange() const
{
	return (getWorld()->getPlayer()->getX() < getX() &&
		(getWorld()->getPlayer()->getY() >= getY() - 4 &&
			getWorld()->getPlayer()->getY() <= getY() + 4));
}


Smallgon::Smallgon(StudentWorld* world, double startX, double startY)
	: Alien(world, IID_SMALLGON, startX, startY, 2.0, 5 * (1 + (world->getLevel() - 1) * 0.1))
{}

void Smallgon::afterCollide()
{
	getWorld()->increaseScore(250);
}

void Smallgon::doSomething()
{
	if (isDead())
		return;
	if (getX() < 0)
	{
		killIt();
		getWorld()->setAlienOffScreen();
		return;
	}


	// check if collide with the player
	collideWPlayer(this, 5);
	

	// change flight plan
	// 225: left and down; 135: left and up; 180: left
	if (getY() >= VIEW_HEIGHT - 1)
		setDir(225);
	else if (getY() <= 0)
		setDir(135);
	else if (getPlan() == 0)
	{
		int ran = randInt(1, 3);
		if (ran == 1)
			setDir(225);
		else if (ran == 2)
			setDir(135);
		else
			setDir(180);
	}
	setPlan(randInt(1, 32));

	// fire turnip
	if (withinRange())
	{
		int ran = randInt(1, (20 / getWorld()->getLevel()) + 5);
		if (ran == 1)
		{
			getWorld()->addActor(new Turnip(getWorld(), getX() - 14, getWorld()->getPlayer()->getY()));
			getWorld()->playSound(SOUND_ALIEN_SHOOT);
		}
	}

	// move
	if (getDir() == 135)
		moveTo(getX() - getSpeed(), getY() + getSpeed());
	else if (getDir() == 225)
		moveTo(getX() - getSpeed(), getY() - getSpeed());
	else
		moveTo(getX() - getSpeed(), getY());
	setPlan(getPlan() - 1);
}


Smoregon::Smoregon(StudentWorld* world, double startX, double startY)
	: Alien(world, IID_SMOREGON, startX, startY, 2.0, 5 * (1 + (world->getLevel() - 1) * 0.1))
{}

void Smoregon::afterCollide()
{
	getWorld()->increaseScore(250);
	if (randInt(1, 3) == 1)
	{
		if (randInt(1, 2) == 1)
			getWorld()->addActor(new RepairG(getWorld(), getX(), getY()));
		else
			getWorld()->addActor(new FlatulenceTorpedoG(getWorld(), getX(), getY()));
	}
}

void Smoregon::doSomething()
{
	if (isDead())
		return;
	if (getX() < 0)
	{
		killIt();
		getWorld()->setAlienOffScreen();
		return;
	}


	// check if collide with the player
	collideWPlayer(this, 5);
	

	// change flight plan
	// 225: left and down; 135: left and up; 180: left
	if (getY() >= VIEW_HEIGHT - 1)
		setDir(225);
	else if (getPlan() == VIEW_WIDTH - 1)
		setDir(180);
	else if (getY() <= 0)
		setDir(135);
	else if (getPlan() == 0)
	{
		int ran = randInt(1, 3);
		if (ran == 1)
			setDir(225);
		else if (ran == 2)
			setDir(135);
		else
			setDir(180);
	}
	setPlan(randInt(1, 32));


	// fire turnip
	if (withinRange())
	{
		if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
		{
			getWorld()->addActor(new Turnip(getWorld(), getX() - 14, getWorld()->getPlayer()->getY()));
			getWorld()->playSound(SOUND_ALIEN_SHOOT);
			return;
		}
		if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
		{
			setDir(180);
			setPlan(VIEW_WIDTH);
			setSpeed(5);
		}
	}

	// move
	if (getDir() == 135)
		moveTo(getX() - getSpeed(), getY() + getSpeed());
	else if (getDir() == 225)
		moveTo(getX() - getSpeed(), getY() - getSpeed());
	else
		moveTo(getX() - getSpeed(), getY());
	setPlan(getPlan() - 1);
}


Snagglegon::Snagglegon(StudentWorld* world, double startX, double startY)
	: Alien(world, IID_SNAGGLEGON, startX, startY, 1.75, 10 * (1 + (world->getLevel() - 1) * 0.1))
{}

void Snagglegon::afterCollide()
{
	getWorld()->increaseScore(1000);
	if (randInt(1, 6) == 1)
		getWorld()->addActor(new ExtraLifeG(getWorld(), getX(), getY()));
}

void Snagglegon::doSomething()
{
	if (isDead())
		return;
	if (getX() < 0)
	{
		killIt();
		getWorld()->setAlienOffScreen();
		return;
	}


	// check if collide with the player
	collideWPlayer(this, 15);
	

	// change flight plan
	// 225: left and down; 135: left and up; 180: left
	if (getY() >= VIEW_HEIGHT - 1)
		setDir(225);
	else if (getY() <= 0)
		setDir(135);

	// fire flatulence torpedo
	if (withinRange())
	{
		if (randInt(1, (15 / getWorld()->getLevel()) + 10) == 1)
		{
			getWorld()->addActor(new FlatulenceTorpedo(getWorld(), IID_SNAGGLEGON, getX() - 14, getWorld()->getPlayer()->getY(), 180));
			getWorld()->playSound(SOUND_TORPEDO);
			return;
		}
	}

	// move
	if (getDir() == 135)
		moveTo(getX() - getSpeed(), getY() + getSpeed());
	else
		moveTo(getX() - getSpeed(), getY() - getSpeed());
}





//////////////////////////////////////////////////////////////////////////////////
// Projectiles //
//////////////////////////////////////////////////////////////////////////////////
Projectile::Projectile(StudentWorld* world, int imageID, double startX, double startY, Direction dir)
	: Actor(world, imageID, startX, startY, dir, 0.5, 1)
{}


Cabbage::Cabbage(StudentWorld* world, double startX, double startY)
	: Projectile(world, IID_CABBAGE, startX, startY, 0)
{}

void Cabbage::doSomething()
{
	if (isDead())
		return;
	if (!onScreen())
	{
		killIt();
		return;
	}

	// collide with any alien
	Actor* a = getWorld()->getOneCollidingAlien(this);
	if (a != nullptr)
	{
		static_cast<Alien*>(a)->collideWProjectile(2);
		killIt();
	}
	else
	{
		moveTo(getX() + 8, getY());
		setDirection(getDirection() + 20);
	}
}


Turnip::Turnip(StudentWorld* world, double startX, double startY)
	: Projectile(world, IID_TURNIP, startX, startY, 0)
{}

void Turnip::doSomething()
{
	if (isDead())
		return;
	if (!onScreen())
	{
		killIt();
		return;
	}
	// collide with player
	if (getWorld()->willCollide(this, getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->setHit(2);
		killIt();
		getWorld()->playSound(SOUND_DEATH);
	}
	else
	{
		moveTo(getX() - 6, getY());
		setDirection(getDirection() + 20);
	}
}


FlatulenceTorpedo::FlatulenceTorpedo(StudentWorld* world, int id, double startX, double startY, Direction dir)
	: Projectile(world, IID_TORPEDO, startX, startY, dir), ID(id)
{}

void FlatulenceTorpedo::doSomething()
{
	if (isDead())
		return;
	if (!onScreen())
	{
		killIt();
		return;
	}

	// collide with aliens
	if (ID == IID_NACHENBLASTER)
	{
		Actor* a = getWorld()->getOneCollidingAlien(this);
		if (a != nullptr)
		{
			static_cast<Alien*>(a)->collideWProjectile(8);
			killIt();
		}
	}
	// collide with the player
	else if (ID == IID_SNAGGLEGON && getWorld()->willCollide(this, getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->setHit(8);
		getWorld()->increaseScore(1000);
		killIt();
		getWorld()->playSound(SOUND_DEATH);
	}

	if (ID == IID_NACHENBLASTER)
		moveTo(getX() + 8, getY());
	else if (ID == IID_SNAGGLEGON)
		moveTo(getX() - 8, getY());
}


//////////////////////////////////////////////////////////////////////////////////
// Explosion //
//////////////////////////////////////////////////////////////////////////////////
Explosion::Explosion(StudentWorld* world, double startX, double startY)
	: Actor(world, IID_EXPLOSION, startX, startY, 0, 1, 0), m_size(1)
{}

void Explosion::doSomething()
{
	m_size *= 1.5;
	setSize(m_size);
	if (m_size == (1.5 * 1.5 * 1.5 * 1.5))   // after calling this function four times
		killIt();
}




//////////////////////////////////////////////////////////////////////////////////
// Goodies //
//////////////////////////////////////////////////////////////////////////////////
Goodie::Goodie(StudentWorld* world, int imageID, double startX, double startY)
	: Actor(world, imageID, startX, startY, 0, 0.5, 1)
{}

void Goodie::collideWPlayer()
{
	if (getWorld()->willCollide(this, getWorld()->getPlayer()))
	{
		getWorld()->increaseScore(100);
		killIt();
		getWorld()->playSound(SOUND_GOODIE);
		afterCollide();
	}
	else
		moveTo(getX() - 0.75, getY() - 0.75);
}

void Goodie::doSomething()
{
	if (isDead())
		return;
	if (!onScreen())
	{
		killIt();
		return;
	}

	collideWPlayer();
}


RepairG::RepairG(StudentWorld* world, double startX, double startY)
	: Goodie(world, IID_REPAIR_GOODIE, startX, startY)
{}

void RepairG::afterCollide()
{
	getWorld()->getPlayer()->setHit(10);
}


ExtraLifeG::ExtraLifeG(StudentWorld* world, double startX, double startY)
	: Goodie(world, IID_LIFE_GOODIE, startX, startY)
{}

void ExtraLifeG::afterCollide()
{
	getWorld()->incLives();
}


FlatulenceTorpedoG::FlatulenceTorpedoG(StudentWorld* world, double startX, double startY)
	: Goodie(world, IID_TORPEDO_GOODIE, startX, startY)
{}

void FlatulenceTorpedoG::afterCollide()
{
	getWorld()->getPlayer()->increaseTorpedo(5);
}
