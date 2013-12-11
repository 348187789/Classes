#include "cocos2d.h"
#include "Controller.h"

#include "HelloLayer.h"
#include "GameLayer.h"

#include <cassert>

USING_NS_CC;

void Controller::run()
{
	Director::getInstance()->runWithScene(getHelloScene());
}

void Controller::runGameScene()
{
	Director::getInstance()->pushScene(getGameScene());
}
Scene* getGameOverScene()
{
	assert(false); // TODO
	return NULL;
}

void Controller::GameOver()
{
	Director::getInstance()->replaceScene(getGameOverScene());
}

void Controller::endGame()
{
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


Scene* Controller::getHelloScene()
{
	static Scene* helloScene = initHelloScene();
	assert(helloScene);
	return helloScene;
}


Scene* Controller::getGameScene()
{
	static Scene* gameScene = initGameScene();
	assert(gameScene);
	return gameScene;
}


Scene* Controller::initHelloScene()
{
	// 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloLayer::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

	return scene;
}

Scene* Controller::initGameScene()
{
	
	// 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameLayer::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

	return scene;

}