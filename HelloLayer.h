#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"


class HelloLayer : public cocos2d::LayerColor
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer


    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
	cocos2d::Menu* initCloseMenu(cocos2d::Size visibleSize, cocos2d::Point origin);

	cocos2d::Menu* initEnterGameMenu(cocos2d::Size visibleSize, cocos2d::Point origin);

    void menuCloseCallback(cocos2d::Object* pSender);

	void menuEnterGameCallback(cocos2d::Object* pSender);
    
	//void addTarget();*/

	//void spriteMoveFinished(cocos2d::Node* sender);

	//void gameLogic(float dt);

    //virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);



    // implement the "static create()" method manually
    CREATE_FUNC(HelloLayer);


};




#endif // __HELLOWORLD_SCENE_H__
