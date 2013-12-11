#ifndef __METALOADER_H__
#define __METALOADER_H__

#include <string>
#include <vector>
#include <unordered_map>

struct EnemyMeta
{
	enum SpeedEnum
	{
		verySlow = 0,
		slow,
		normal,
		fast,
		veryFast
	};
	SpeedEnum speed;
	int hp;
	int money;
};

struct WaveMeta
{
	std::string enemyName;
	std::vector<int> waveCount;
};

struct GameMeta
{
    std::unordered_map<std::string, EnemyMeta> enemys;
    std::vector<WaveMeta> waves;
};

GameMeta loadGameMeta(const std::string& jsonFileName);

void MetaLoaderTest();

#endif // !__METALOADER_H__
