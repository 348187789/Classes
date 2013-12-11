#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "MetaLoader.h"

#include <list>
#include <string>
#include <vector>
#include <cassert>
#include <memory>

#include <unordered_map>


class Enemy;
class Tower;

void doNothing();

class HashForIntPair
{
public:
	unsigned int operator()(std::pair<int,int> p) const
	{
		auto ret = p.first;
		ret += p.second*33;
		return ret; 
	}
};

class MoneyLabel
{
public: 
	inline static MoneyLabel* create(){ return new MoneyLabel(); }
	inline int getMoney() { return money_; }
	inline void setMoney(int m) { money_ = m; updateString(); node_->setString(moneyString_.c_str());} 
	cocos2d::LabelTTF* getNode(){ return node_; };

private:
	inline void updateString(){ moneyString_ = std::to_string(money_); }
	MoneyLabel() : money_(0), node_(cocos2d::LabelTTF::create("Money : 200","Arial", 15)){ assert(node_); }

	cocos2d::LabelTTF* node_;
	int money_;
	std::string moneyString_;
};

class GameLayer : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer


    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

	void initMap();
    
	inline int getMoney(){ return moneyLabel_->getMoney(); }
	inline void setMoney(int m){ moneyLabel_->setMoney(m); }
    
	void update(float fd);
	void onTimeCountPoint();


	void onTouchesEnded (const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event) override;

	void onEnemyDying(Enemy* e, std::function<void()> afterDied);


    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

private:
	

	void addEnemy(const std::string& enemyName, std::function<void()> afterDied = doNothing);
	void addTower(cocos2d::Point p);

	cocos2d::TMXTiledMap* map_;

	std::vector<Tower*> towers_;
	std::vector<Enemy*> enemys_;

	std::unordered_map< std::pair<int, int>, Tower* ,HashForIntPair> towersInMapPoint_;

	//std::list<Enemy*> enemysInList_;

	int count_;
	unsigned int waveIndex_;
	unsigned int countIndex_;

	std::auto_ptr<MoneyLabel> moneyLabel_;

    std::unordered_map<std::string, EnemyMeta> enemysMeta_;
    std::vector<WaveMeta> wavesMeta_;

	typedef decltype(wavesMeta_) wavesMetaType;

};


#endif