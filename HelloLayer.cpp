#include <functional>

#include "HelloLayer.h"

#include "Controller.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool HelloLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B(255,255,255,255)) )
    {
        return false;
    }


	this->setTouchEnabled(true);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();
		
	Menu* closeMenu = initCloseMenu(visibleSize, origin);

	Menu* enterGameMenu = initEnterGameMenu(visibleSize, origin);
	 
	this->addChild(closeMenu, 1);
	this->addChild(enterGameMenu, 1);


	return true;
}

Menu* HelloLayer::initCloseMenu(Size visibleSize, Point origin)
{
	MenuItemImage* closeItem = MenuItemImage::create(
											"exit.png",
											"exit.png",
											[this](Object* sender){this->menuCloseCallback(sender);});
											//CC_CALLBACK_1(HelloLayer::menuCloseCallback, this));

	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2,
								origin.y  + closeItem->getContentSize().height/2)); // ×îÓÒÏÂ½Ç
	
	Menu* menu = Menu::create(closeItem, NULL);
	
	assert(menu);
	menu->setPosition(Point::ZERO);

	return menu;
}


Menu* HelloLayer::initEnterGameMenu(Size visibleSize, Point origin)
{
	MenuItemImage* enterGameItem = MenuItemImage::create(
											"enterGameNormal.png",
											"enterGameSelected.png",
											[this](Object* sender){this->menuEnterGameCallback(sender);});
											//CC_CALLBACK_1(HelloLayer::menuCloseCallback, this));

	enterGameItem->setPosition(Point(origin.x + visibleSize.width/2 /*- enterGameItem->getContentSize().width/2*/,
								origin.y  + visibleSize.height/2/* - enterGameItem->getContentSize().height/2*/)); 
	
	Menu* menu = Menu::create(enterGameItem, NULL);
	assert(menu);
	menu->setPosition(Point::ZERO);
	return menu;
}

void HelloLayer::menuCloseCallback(Object* pSender)
{
    Controller::endGame();
}


void HelloLayer::menuEnterGameCallback(Object* pSender)
{
	Controller::runGameScene();
}

