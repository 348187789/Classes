#ifndef __FUNCS_H__
#define __FUNCS_H__

#include "cocos2d.h"



bool isValidPosition(cocos2d::Point pos, cocos2d::TMXTiledMap* map);

bool isValidTileCoord(cocos2d::Point tileCoord, cocos2d::TMXTiledMap* map);

cocos2d::Point getCoordByPosition(cocos2d::Point pos, cocos2d::TMXTiledMap* map);

cocos2d::Point getPositionByCoord(cocos2d::Point coord, cocos2d::TMXTiledMap* map);

void setViewPointCenter(cocos2d::Point pos, cocos2d::Scene* scene, cocos2d::TMXTiledMap* map);

float getDistance(cocos2d::Point p1, cocos2d::Point p2);

cocos2d::Point convertPointInLayerToPointInMap(cocos2d::Point pointInLayer, cocos2d::TMXTiledMap* map);




#endif // !__FUNCS_H__
