#include <cassert>
#include <exception>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>

#include "cocos2d.h"

#include "GameLayer.h"

#include "Funcs.h"
#include "Enemy.h"
#include "Tower.h"
#include "json.h"
#include "Log.h"

#include "MetaLoader.h"

#include <chrono>
#include <thread>

USING_NS_CC;

void doNothing(){}

std::vector<Point> loadRoadPoints() // TODO
{
	
	std::vector<Point> ret;
	ret.push_back(Point(0, 8));
	ret.push_back(Point(3, 8));
	ret.push_back(Point(3, 1));
	ret.push_back(Point(8, 1));
	ret.push_back(Point(8, 8));
	ret.push_back(Point(16, 8));
	ret.push_back(Point(16, 3));
	ret.push_back(Point(22, 3));
	return ret;
}

void registerEnemyAction(Enemy* enemy, TMXTiledMap* map)
{
	assert(enemy);

	Array* actionArray = Array::create();
	std::vector<Point> roadPoints = loadRoadPoints();
	

	Point lastPoint = roadPoints[0];
	for(auto iter = roadPoints.begin()+1; 
		iter < roadPoints.end(); 
		lastPoint = *iter, ++iter)
	{
		auto moveToNextPosition = MoveTo::create(getDistance(lastPoint, *iter)/enemy->getWalkingSpeed(), getPositionByCoord(*iter, map));
		actionArray->addObject(moveToNextPosition);
	}
	auto loseGame = CallFunc();
	// actionArrar->addObject();
	enemy->runAction(Sequence::create(actionArray));
}

std::vector<int> readWaveDataFromJson()
{
	int a[] = {0, 1};
	return std::vector<int>(std::begin(a), std::end(a));	
}


void GameLayer::onEnemyDying(Enemy* e, std::function<void()> afterDied)
{
	this->setMoney(this->getMoney() + 20);
	assert(enemys_.size() != 0);
	if(enemys_.size()>0)
	{
		LOG(std::string("delete enemy : ") + std::to_string(e - static_cast<Enemy*>(0)));
		int originSize = enemys_.size();
		//remove_if(enemys_.begin(), enemys_.end(), [=](Enemy* enemy){return enemy == e; });
		
		enemys_.erase(std::find(enemys_.begin(), enemys_.end(), e));
		assert(std::find(enemys_.begin(), enemys_.end(), e) == enemys_.end());
		LOG(std::string("decrese : " + std::to_string(originSize - enemys_.size())));
	}
	
	afterDied();
	//// 此处应该有延时 TODO

	map_->removeChild(e);/////////////////////////////////////
}

void GameLayer::addEnemy(const std::string& enemyName, std::function<void()> afterDied)
{
	using std::placeholders::_1;
	auto iter = enemysMeta_.find(enemyName); 
	assert(iter != enemysMeta_.end() && "cannot find enemy name in jsonFile");
	// create enemy and run action 
	auto enemy = Enemy::create(*(iter)); // maxHP TODO 明天要做的
	enemy->onDying = std::bind(&GameLayer::onEnemyDying, this, _1, std::move(afterDied));
	
	assert(loadRoadPoints().size() >= 1);
	auto originPos = getPositionByCoord(loadRoadPoints()[0], map_);
	enemy->setPosition(originPos);
	map_->addChild(enemy);
	
	registerEnemyAction(enemy, map_);

	LOG(std::string("push_back enemy : ") + std::to_string(enemy - static_cast<Enemy*>(0)));

	enemys_.push_back(enemy);
}

void GameLayer::addTower(cocos2d::Point p)
{
	auto tower = Tower::create(map_);
	auto towerPos = getPositionByCoord(p, map_);
	tower->setPosition(towerPos);
	map_->addChild(tower);
	towers_.push_back(tower);

	std::pair<int, int> intPoint(p.x, p.y);
	assert(!towersInMapPoint_.count(intPoint));
	towersInMapPoint_[intPoint] = tower;
}

bool GameLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	//MetaLoaderTest();
	auto gameMeta = loadGameMeta("MetaLoaderTest.json");
	enemysMeta_ = std::move(gameMeta.enemys);
	wavesMeta_ = std::move(gameMeta.waves);
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	std::string name1;
	std::string name2;
	
	cache->addSpriteFramesWithFile("enemys.plist", "enemys.png");
	

	// auto schedule
	this->scheduleUpdate();
	this->setTouchEnabled(true);

	// init tiledMap
	initMap();

	// add tower on map

	
	moneyLabel_.reset(MoneyLabel::create());

	moneyLabel_->setMoney(0);
	this->addChild(moneyLabel_->getNode(), 12);
	Point p(map_->getPositionX() + map_-> getContentSize().width/2, map_->getPositionY() - moneyLabel_->getNode()->getContentSize().height/2 );
	moneyLabel_->getNode()->setPosition(p);

	setMoney(100);

	return true;
}

void GameLayer::onTouchesEnded (const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{

	Touch* touch = touches[0];
	assert(touch);
	Point location = Director::getInstance()->convertToGL(touch->getLocationInView());
	Point locationInMap = convertPointInLayerToPointInMap(location, map_);
	Point locationCoord = getCoordByPosition(locationInMap, map_);

	Point locationInMapHUANYUAN = getPositionByCoord(locationCoord, map_); 

	auto toString = []
	(Point p) -> std::string
	{
		return std::string("X : ")+std::to_string(p.x)+"\tY : " + std::to_string(p.y);
	};

	if(!towersInMapPoint_.count(std::pair<int,int>(locationCoord.x, locationCoord.y)))
	{
		addTower(locationCoord);
		setMoney(getMoney() - 50);
	}
	else
	{

	}

}



void GameLayer::initMap()
{
	count_ = 0;
	countIndex_ = 0;
	waveIndex_ = 0;

	Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
        
	map_ = TMXTiledMap::create("test.tmx");
	assert(map_);
	this->addChild(map_);


	map_->setPosition(Point(
		visibleSize.width/2  + origin.x - map_->getContentSize().width/2, 
		visibleSize.height/2 + origin.y - map_->getContentSize().height/2));

	Array* pChildrenArray = map_->getChildren();

	SpriteBatchNode* child = NULL;
	Object* pObject = NULL;

	
	CCARRAY_FOREACH(pChildrenArray, pObject)
	{
		child = (SpriteBatchNode*)pObject;
		if (!child) break;
		
		child -> getTexture() -> setAntiAliasTexParameters();
		// antialias  反锯齿
	}
}





void GameLayer::update(float dt)
{
	++count_;
	assert(map_);
	if(count_%40 == 0)
	{
		for(Tower* t : towers_)
		{
			for(Enemy* e : enemys_)
			{
				if(!e || !t) LOG("assertion filed at line 233");
				if(e->isAlive() && t->canFireTo(e))
				{
					t->fireBulletTo(e);
					break;
				}

			}
		}
	}
	if(count_%80 == 0)
	{
		onTimeCountPoint();	
	}

}

void GameLayer::onTimeCountPoint()
{
	if(countIndex_ >= 0 &&  countIndex_ < wavesMeta_[waveIndex_].waveCount.size())
	{   
		unsigned int sizeOfThisWave = wavesMeta_[waveIndex_].waveCount.size();
		int enemysCountAtThisIndex = wavesMeta_[waveIndex_].waveCount[countIndex_];
		if(enemysCountAtThisIndex >= 1)//for(int i = 1; i <= enemysCountInThisWave; ++i) // count 为 1 时  加一个enemy
		{
			if(countIndex_ == sizeOfThisWave - 1) // 加入那一波的最后一个小兵
			{
				if(waveIndex_ == wavesMeta_.size() - 1)
				{
					addEnemy(wavesMeta_[waveIndex_].enemyName,
						[](){ LOG("you win"); exit(0); });
				}
				
				else // 前 n-1 波
				{
					addEnemy(wavesMeta_[waveIndex_].enemyName,
						[&]()
						{
							// 此处应该有延时 TODO
							countIndex_ = 0;
							++waveIndex_;
						});
				}
				
			}
			else
			{
				addEnemy(wavesMeta_[waveIndex_].enemyName, doNothing);
			}
		}

		 ++countIndex_;

	}
}