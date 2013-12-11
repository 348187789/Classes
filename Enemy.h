#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"
#include "MetaLoader.h"
#include <functional>

class Enemy : public cocos2d::Node
{
private:
	int currentHP_;
	int willBeHP_;
	bool active_;
	float walkingSpeed_;
	const std::pair<const std::string, EnemyMeta>* metaMessage_;
public:

	std::function<void(Enemy*)> onDying;

public:

	const std::string& getName(){ return metaMessage_->first; }
	int getMaxHP(){ return metaMessage_->second.hp; }
	float getWalkingSpeed(){ return (metaMessage_->second.speed + 1)/1.5; } //  TODO

	Enemy(int maxHP);
	Enemy(const std::pair<const std::string, EnemyMeta>& EnemyMessage);

	static Enemy* create(int maxHP);
	static Enemy* create(const std::pair<const std::string, EnemyMeta>& EnemyMessage);

	void beforeAttacted(int damage);
	void beAttacked(int damage);

	inline bool isAlive(){ return active_; }

	void update(float dt) override;

	cocos2d::ProgressTimer* HPProgress;
};

#endif // !__ENEMY_H__
