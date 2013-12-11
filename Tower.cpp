
#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>

#include "Log.h"
#include "Enemy.h"
#include "Funcs.h"
#include "Tower.h"


USING_NS_CC;

using std::placeholders::_1;

class Bullet : public Node
{
public:
	static Bullet* create()
	{
		Bullet* ret = new Bullet();
		assert(ret);
		ret->autorelease();
		ret->scheduleUpdate();
		return ret;
	}
	Bullet()
	{
		Sprite* bullet = Sprite::create("littleBullet.png");
		bullet->setScale(0.6f);
		this->addChild(bullet);
	}
private:
	void update(float dt)
	{
		this->setRotation(this->getRotation()-4);
	}
};


Tower::Tower(TMXTiledMap* towerInMap)
	: map_(towerInMap)
	, damage_(3)
{	
	range_ = 150;
	//Sprite* towerBody = Sprite::create("Player.png");
	Sprite* towerBody = Sprite::create("littleTowerBase.png");
	Sprite* towerBulletOnBody = Sprite::create("littleBullet.png");

	assert(towerBody && towerBulletOnBody);
	this->addChild(towerBody, 1);
	this->addChild(towerBulletOnBody, 1);
	towerBody->setPosition(towerBody->getPosition() + Point(0, -20));
	//towerBulletOnBody->setAnchorPoint(Point(towerBulletOnBody->getContentSize().width/2,towerBulletOnBody->getContentSize().height/2));
	Point onTowerBody = Point(towerBody->getContentSize().width/2, towerBody->getContentSize().height);
	onTowerBody = onTowerBody + towerBody->getPosition();
	onTowerBody = onTowerBody - Point(towerBulletOnBody->getContentSize().width/2.7 , towerBulletOnBody->getContentSize().height/4);
	towerBulletOnBody->setPosition(onTowerBody);

	this->setScale(0.7f);

	//this->scheduleUpdate();
}

bool Tower::canFireTo(Enemy* e)
{
	assert(this);
	assert(e);
	float len = getDistance(this->getPosition(), e->getPosition());
	return len < range_; 
}


void Tower::fireBulletTo(Enemy* enemyFireTo)
{
	Point fireTo = enemyFireTo->getPosition();

	float speedOfBullet = 480/1/2; // 480 pixel/s


	float length = getDistance(fireTo, this->getPosition());
	if(enemyFireTo->isAlive()) // Éä³Ì
	{
		
		Bullet* bullet =  Bullet::create();
		assert(bullet);
		map_->addChild(bullet);

		enemyFireTo->beforeAttacted(damage_);
		bullet->setPosition(this->getPosition());
		MoveTo* moveToPoint = MoveTo::create(length/speedOfBullet, fireTo);

		auto actionSeq = Sequence::create(
			moveToPoint, 
			//CCCallFuncN::create(this, callfuncN_selector(Tower::bulletMoveFinished)), 
			CallFuncN::create(std::bind(&Tower::bulletMoveFinished, this, _1, enemyFireTo)),
			NULL);

		bullet->runAction(actionSeq);
	}
}


void Tower::bulletMoveFinished(Node* sender, Enemy* e)
{
	Sprite* bullet = (Sprite*)sender;
	this->map_->removeChild(bullet, true);
	e->beAttacked(damage_);
}