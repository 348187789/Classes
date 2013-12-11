
#include "MetaLoader.h"

#include "Json.h"
#include "Log.h"

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

GameMeta loadGameMeta(const std::string& jsonFileName)
{
	GameMeta gameMeta;
	JsonErrorCode e;
	JsonObject jsonMeta(jsonFileName.c_str(), e);
	if(e)
	{
		LOG("parseError");
		LOG(e.what());
		return gameMeta;
	}

	// else
	auto& jsonEnemys = jsonMeta["enemys"];
	auto& jsonWaves = jsonMeta["waves"];
	
	
	///////////////////////////////enemys
	const unordered_map<string, JsonObject>& enemys = jsonEnemys.toDict();

	unordered_map<string, EnemyMeta::SpeedEnum> getSpeed;
	getSpeed["verySlow"] = EnemyMeta::SpeedEnum::verySlow;
	getSpeed["slow"] = EnemyMeta::SpeedEnum::slow;
	getSpeed["normal"] = EnemyMeta::SpeedEnum::normal;
	getSpeed["fast"] = EnemyMeta::SpeedEnum::fast;
	getSpeed["veryFast"] = EnemyMeta::SpeedEnum::veryFast;
	

	EnemyMeta enemyMeta;
	string speedType;
	
	for(const auto& enemy : enemys)
	{
		const JsonObject& jsonEnemyMeta = enemy.second;
		enemyMeta.speed = getSpeed[jsonEnemyMeta["speed"].toString()];	
		enemyMeta.hp = jsonEnemyMeta["hp"].toInt();
		enemyMeta.money = jsonEnemyMeta["money"].toInt();
		
		gameMeta.enemys[enemy.first] = enemyMeta;
		// enemy.first ¾ÍÊÇ enemyÃû×Ö
	}

	////////////////////////////////waves
	const vector<JsonObject>& waves = jsonWaves.toList();

	WaveMeta waveMeta;
	for(const auto& wave : waves)
	{
		waveMeta.waveCount.clear();
		const unordered_map<string, JsonObject>& jsonWaveMeta = wave.toDict();
		waveMeta.enemyName = (jsonWaveMeta.find("enemy"))->second.toString();
		for(const JsonObject& i : (jsonWaveMeta.find("waveCount")->second.toList()))
		{
			waveMeta.waveCount.push_back(i.toInt());
		}
		gameMeta.waves.push_back(waveMeta);
	}
	return gameMeta;
}
void logEnemyMeta(const EnemyMeta& meta)
{
	static const char* speedStr[]  = {"verySlow","slow","normal","fast","veryFast"};
	LOG(string("Speed : ") + speedStr[meta.speed]);
	LOG(string("hp : " + std::to_string(meta.hp)));
	LOG(string("money : ") + std::to_string(meta.money));
}

void logWaveMeta(const WaveMeta& meta)
{
	string str;
	for (auto i : meta.waveCount){ str.push_back('0' + i); }
	LOG(string("enemyName : ") + meta.enemyName);
	LOG("waveCount : " + str);
}

void logMeta(const GameMeta& meta)
{
	LOG("/*****************************");
	LOG("enemys : ");
	for(const auto& p : meta.enemys)
	{
		LOG("/-----------------------------");
		LOG(string("enemyName : ") + p.first );
		logEnemyMeta(p.second);	
	}
	LOG("/*****************************");
	
	LOG("/*****************************");
	LOG("waves : ");
	for(const auto& wave : meta.waves)
	{
		LOG("/-----------------------------");
		logWaveMeta(wave);
	}
	LOG("/*****************************");

}

void MetaLoaderTest()
{
	GameMeta meta = loadGameMeta("MetaLoaderTest.json");
	logMeta(meta);
}