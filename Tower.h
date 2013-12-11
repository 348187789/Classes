#ifndef __TOWER_H__
#define __TOWER_H__

#include "cocos2d.h"
#include <cassert>
#include "Enemy.h"

class Tower : public cocos2d::Node
{
public:

	Tower(cocos2d::TMXTiledMap* towerInMap);

	void fireBulletTo(Enemy* enemyFireTo);


	static Tower* create(cocos2d::TMXTiledMap* towerInMap)
	{
		Tower* ret = new Tower(towerInMap);
		assert(ret);
		ret->autorelease();
		return ret;
	}

	bool canFireTo(Enemy* e);

	void bulletMoveFinished(cocos2d::Node* sender, Enemy* e);

	cocos2d::TMXTiledMap* map_;
	float range_;


private:
	int damage_;
};



#endif // !__TOWER_H__