#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "cocos2d.h"

class Controller
{
public:
	static void run();
	static void runGameScene();
	static void endGame();
	static void GameOver();

private:
	static cocos2d::Scene* getHelloScene();
	static cocos2d::Scene* getGameScene();
	
	static cocos2d::Scene* initHelloScene();
	static cocos2d::Scene* initGameScene();
};

#endif