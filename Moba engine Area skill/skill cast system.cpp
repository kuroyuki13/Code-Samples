#include "SkillCastSystem.h"
#include "TrickReplication/Network/GameServer.h"
#include <TrickCore/Logger.h>
#include <iostream>
#include "GameServerMain.h"
#include "SkillEnum.h"
//components
#include "Components\CastSkillComponent.h"
#include "Components\ServerHolderComponent.h"
#include "Components\ChampionSkillFourComponent.h"
#include "Components\ChampionSkillThreeComponent.h"
#include "Components\ChampionSkillTwoComponent.h"
#include "Components\ChampionSkillOneComponent.h"
#include "Components\PlayerPositionComponent.h"
#include "Components\PlayerIdComponent.h"
#include "Components\EntityCounterComponent.h"
#include "Components\EntityIdComponent.h"
#include "Components\PlayerStatsComponent.h"
#include "SkillComponents.h"

/* 
this system allows for an easily expanded skillset in the game.
Each champion has four skills, their information (type, speed, cooldown, damage etc.) stored in their respective entity.
when a skill is cast from player side it corresponds to the skill number.
in this system the correct information corresponding to that skill number is retrieved, after a network entity of the 
propper type is created.
the fixed update system corresponding to that skill type on the network will then handle its updates.
*/



SkillCastSystem::SkillCastSystem() : IReactiveSystem(*this)
{
	// setting up observer to collect updated components
	auto m = Matcher_AllOf(CastSkillComponent);
	trigger = m.OnEntityAdded();
}

void SkillCastSystem::SetPool(EntitasPP::Pool* pool)
{
	_pool = pool;
	auto e = _pool->GetEntities(Matcher_AllOf(ServerHolderComponent));
	if (e.size() > 1) Logger::LogWarning("ERROR: More than one ServerHolder found");
	else if (e.size() == 0) Logger::LogWarning("ERROR: No server holder found");
	else {
		//_server = e[0];
		_server = e[0]->Get<ServerHolderComponent>()->server;
	}
}

void SkillCastSystem::Initialize()
{


}

void SkillCastSystem::Execute(std::vector<EntitasPP::EntityPtr> entities)
{
	for (auto e : entities)
	{
		//get the skill number of the updated skill. this skill number is tied to the champion, which has the information on which skill is cast
		Skillstate number = (Skillstate)e->Get<CastSkillComponent>()->skill_number;
		Skilltype type = NoSkill;

		//switch on the skill number and fill in values, information taken from the numbered skill of that champion
		switch (number)
		{
		case SkillOne:
		{
			auto comp1 = e->Get<ChampionSkillOneComponent>();
			type = comp1->Type;
			range = comp1->range;
			radius_or_width = comp1->radius_or_width;
			speed_or_delay = comp1->speed_or_delay;
			duration = comp1->duration;
			m_damage = comp1->m_damage;
			p_damage = comp1->p_damage;
			cooldown = comp1->cooldown;
			break;
		}
		case SkillTwo:
		{
			auto comp2 = e->Get<ChampionSkillTwoComponent>();
			type = comp2->Type;
			range = comp2->range;
			radius_or_width = comp2->radius_or_width;
			speed_or_delay = comp2->speed_or_delay;
			duration = comp2->duration;
			m_damage = comp2->m_damage;
			p_damage = comp2->p_damage;
			cooldown = comp2->cooldown;
			break;
		}
		case SkillThree:
		{
			auto comp3 = e->Get<ChampionSkillThreeComponent>();
			type = comp3->Type;
			range = comp3->range;
			radius_or_width = comp3->radius_or_width;
			speed_or_delay = comp3->speed_or_delay;
			duration = comp3->duration;
			m_damage = comp3->m_damage;
			p_damage = comp3->p_damage;
			cooldown = comp3->cooldown;
			break;
		}
		case SkillFour:
		{
			auto comp4 = e->Get<ChampionSkillFourComponent>();
			type = comp4->Type;
			range = comp4->range;
			radius_or_width = comp4->radius_or_width;
			speed_or_delay = comp4->speed_or_delay;
			duration = comp4->duration;
			m_damage = comp4->m_damage;
			p_damage = comp4->p_damage;
			cooldown = comp4->cooldown;
			break;
		}
		default:
			break;
		}
		printf("type of attack: %d\n", type);
		//change to use different champion types when implemented;
		//check for the type of the skill and create a corresponding entity on the network, add all components required to that entity.
		switch (type)
		{
		case StraightShot:
		{
			glm::vec3 dir = glm::normalize(e->Get<CastSkillComponent>()->SkillTarget - e->Get<PlayerPostitionComponent>()->value);
			glm::vec3 target = e->Get<PlayerPostitionComponent>()->value + dir*range;
			target.y = 0.f;
			auto skill = _server->CreateNetworkEntity(_server->main_pool, _server->main_room, true, [this, e, target](EntitasPP::EntityPtr skill)
			{
				int entityCount = _server->entityCounter->Get<EntityCounterComponent>()->value;
				skill->Add<EntityIdComponent>(entityCount);
				_server->entityCounter->Replace<EntityCounterComponent>(entityCount + 1);

				skill->Add<SkillTargetComponent>(target);
				skill->Add<SkillPositionComponent>(e->Get<PlayerPostitionComponent>()->value);
				auto aaa = target;
				skill->Add<SkillSpeedComponent>(speed_or_delay);
				skill->Add<SkillWidthComponent>(radius_or_width);
				skill->Add<SkillDamageComponent>(m_damage, p_damage);
				skill->Add<OwnedByPlayerComponent>(e->Get<PlayerIdComponent>()->value);
				skill->Add<isSkillshotComponent>(true);
			});
			printf("i have recieved word you want to attack\n");
			break;
		}
		case Area:
		{
			auto skill = _server->CreateNetworkEntity(_server->main_pool, _server->main_room, true, [this, e](EntitasPP::EntityPtr skill)
			{
				int entityCount = _server->entityCounter->Get<EntityCounterComponent>()->value;
				skill->Add<EntityIdComponent>(entityCount);
				_server->entityCounter->Replace<EntityCounterComponent>(entityCount + 1);

				skill->Add<SkillPositionComponent>(e->Get<CastSkillComponent>()->SkillTarget);
				skill->Add<SkillDelayComponent>(speed_or_delay);
				skill->Add<SkillRadiusComponent>(radius_or_width);
				skill->Add<SkillDamageComponent>(m_damage, p_damage);
				skill->Add<OwnedByPlayerComponent>(e->Get<PlayerIdComponent>()->value);
				skill->Add<isAreaSkillComponent>(true);
				skill->Add<CounterComponent>(0.f);
			});
			break;
		}
		case Target:
		{
			auto skill = _server->CreateNetworkEntity(_server->main_pool, _server->main_room, true, [this, e](EntitasPP::EntityPtr skill)
			{
				int entityCount = _server->entityCounter->Get<EntityCounterComponent>()->value;
				skill->Add<EntityIdComponent>(entityCount);
				_server->entityCounter->Replace<EntityCounterComponent>(entityCount + 1);

				skill->Add<SkillPositionComponent>(e->Get<CastSkillComponent>()->SkillTarget);
				skill->Add<SkillSpeedComponent>(speed_or_delay);
				skill->Add<SkillRadiusComponent>(radius_or_width);
				skill->Add<SkillDamageComponent>(m_damage, p_damage);
				skill->Add<TargetPlayerIdComponent>(e->Get<CastSkillComponent>()->PlayerTargetID);
				skill->Add<OwnedByPlayerComponent>(e->Get<PlayerIdComponent>()->value);
				skill->Add<isTargetSkillComponent>(true);
			});
			break;
		}
		case Buff:
		{
			auto skill = _server->CreateNetworkEntity(_server->main_pool, _server->main_room, true, [this, e](EntitasPP::EntityPtr skill)
			{
				int entityCount = _server->entityCounter->Get<EntityCounterComponent>()->value;
				skill->Add<EntityIdComponent>(entityCount);
				_server->entityCounter->Replace<EntityCounterComponent>(entityCount + 1);

				skill->Add<SkillDurationComponent>(duration);
				skill->Add<TargetPlayerIdComponent>(e->Get<CastSkillComponent>()->PlayerTargetID);
				skill->Add<OwnedByPlayerComponent>(e->Get<PlayerIdComponent>()->value);
				skill->Add<isBuffSkillComponent>(true);
				skill->Add<CounterComponent>(0.f);
				skill->Add<SkillOldStatComponent>((float)e->Get<PlayerStatsComponent>()->movement_speed);
			});
			break;
		}
		}

	}
}