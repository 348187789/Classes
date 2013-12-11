
#include "Funcs.h"

USING_NS_CC;

bool isValidPosition(Point pos, TMXTiledMap* map)
{
	if(!map) throw std::exception("invalid ptr");
	return	pos.x >= 0 && 
			pos.y >= 0 &&
			pos.x <= map->getContentSize().width && 
			pos.y <= map->getContentSize().height; 
}

bool isValidTileCoord(Point tileCoord, TMXTiledMap* map)
{
	if(!map) throw std::exception("invalid ptr");
	return	tileCoord.x >= 0 &&
			tileCoord.y >= 0 &&
			tileCoord.x <  map->getMapSize().width &&
			tileCoord.y <  map->getMapSize().height;
}

Point getCoordByPosition(Point pos, TMXTiledMap* map)
{
	if(!map) throw std::exception("invalid ptr");
	return 
	  isValidPosition(pos, map) 
		? Point(static_cast<int>(pos.x/map->getTileSize().width), static_cast<int>(pos.y/map->getTileSize().height))
		: Point(-1, -1);
}

Point getPositionByCoord(Point coord, TMXTiledMap* map)
{
	if(!map) throw std::exception("invalid ptr");
	int x = (coord.x*map->getTileSize().width) + map->getTileSize().width/2;
	int y = (coord.y*map->getTileSize().height) + map->getTileSize().height/2;
	return Point(x, y);
}

void setViewPointCenter(Point pos, Scene* scene, TMXTiledMap* map)
{
	Size winSize = Director::getInstance()->getWinSize();
	float winWidth = winSize.width;
	float winHeight = winSize.height;

	float scale = scene->getScale();

	int x = MAX(pos.x, winWidth/2/scale); // scale缩放比例，仅当scalex==scaley时可调用getscale
	int y = MAX(pos.y, winHeight/2/scale); // scale缩放比例，仅当scalex==scaley时可调用getscale

	x = MIN(x, map->getContentSize().width - winWidth/2/scale);
	y = MIN(y, map->getContentSize().height - winHeight/2/scale);

	Point actualPosition = Point(x, y);

	Point centerOfView = Point(winSize.width/2, winSize.height/2);
	Point viewPoint = centerOfView - actualPosition;

	map->setPosition(viewPoint);
}

float getDistance(Point p1, Point p2)
{
	auto p = p1 - p2;
	return sqrtf(abs(p.x * p.x + p.y * p.y));
}


Point convertPointInLayerToPointInMap(Point pointInLayer, TMXTiledMap* map)
{
	assert(map);
	Point mapPosition = map->getPosition();
	Point mapAnchorPoint = map->getAnchorPoint();
	Point mapPositionWhenAnchorPointInLeftDown = mapPosition - mapAnchorPoint;
	Point PointInMap = pointInLayer - mapPositionWhenAnchorPointInLeftDown;
	return PointInMap;
}