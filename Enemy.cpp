#include "Enemy.h"
#include "Log.h"
USING_NS_CC;

#include <tuple>
#include <functional>
#include <algorithm>

std::tuple<Node*, ProgressTimer*> createHP(const char* backgroundImage, const char* progressImage, float percentage)
{
	assert(backgroundImage && progressImage && percentage > -1 && percentage < 101);
	Node* ret = Node::create();
	ret->setAnchorPoint(Point(0, 0));
	
	auto HPBackground = Sprite::create(backgroundImage);
	assert(HPBackground);
	HPBackground->setAnchorPoint(Point(0, 0));

	auto hpSprite = Sprite::create(progressImage);
	assert(hpSprite);
	auto HPProgress = ProgressTimer::create(hpSprite);

	//HPProgress->setType(kCCProgressTimerTypeBar);
	HPProgress->setType(ProgressTimerType::BAR);
	HPProgress->setMidpoint(Point(0, 1));
	HPProgress->setBarChangeRate(Point(1, 0));
	HPProgress->setAnchorPoint(Point(0, 0));

	HPProgress->setPercentage(percentage);
	
	// 默认是扇形啊……
	ret->addChild(HPBackground, 2);
	ret->addChild(HPProgress, 3);

	assert(ret);
	return std::make_tuple(ret, HPProgress);
}

Enemy::Enemy(const std::pair<const std::string, EnemyMeta>& EnemyMessage)
	: currentHP_(EnemyMessage.second.hp)
	, willBeHP_(EnemyMessage.second.hp)
{
	metaMessage_ = &EnemyMessage;
	
	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.5); // TODO///////////////////////////
	animation->setLoops(-1); // 无限次
	//animation->setRestoreOriginalFrame(true);//是否回到第一帧 

	std::string name = getName(); // 注意名字是name图像是name1.png 12要插在png前
	name += "1.png";
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(name.c_str()));
	*std::find(name.begin(), name.end(), '1') = '2';
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(name.c_str()));

	Sprite* enemyBody = Sprite::create();
	enemyBody->setContentSize(SpriteFrameCache::getInstance()->getSpriteFrameByName(name.c_str())->getOriginalSize());
	FiniteTimeAction* animate =  Animate::create(animation); 
	enemyBody->runAction(animate);

	assert(enemyBody);
	enemyBody->setScale(0.8f);
	this->addChild(enemyBody, 1);

	Node* enemyHP = NULL;
	std::tie(enemyHP, this->HPProgress) = createHP("HPTrack.png", "HPProgress.png", 15);


	enemyHP->setScaleX(0.1f);
	enemyHP->setScaleY(0.3f);

	enemyHP->setPosition(enemyBody->getPosition() + Point(- enemyBody->getContentSize().width/2, enemyBody->getContentSize().height/1.5));


	
	this->addChild(enemyHP,5);
	
	this->HPProgress->setPercentage(100*currentHP_/float(getMaxHP()));
}


Enemy* Enemy::create(const std::pair<const std::string, EnemyMeta>& EnemyMessage)
{
	Enemy* enemy = new Enemy(EnemyMessage);
	assert(enemy);
	enemy->autorelease();

	// enemy->scheduleUpdate(); // 注意要 有在这个调用才会自动schedule
	
	return enemy; 
}

void Enemy::beforeAttacted(int damage)
{
	assert(isAlive());
	willBeHP_ -= damage;
	if(willBeHP_ <= 0)
	{
		active_ = false;
	}
}


void Enemy::beAttacked(int damage)
{
	currentHP_ -= damage;
	this->HPProgress->setPercentage(100*currentHP_/getMaxHP());
	if(currentHP_ <= 0)
	{
		//this->setVisible(false);
		if(onDying) onDying(this);
		
	}
}

void Enemy::update(float dt) 
{
	
}