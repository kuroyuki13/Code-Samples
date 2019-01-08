#include "TrickReplication/Network/GameServer.h"
#include <Systems/TakeDamageSystem.h>
#include "Components/PlayerStatsComponent.h"
#include "Components/PlayerPositionComponent.h"
#include <TrickCore/Logger.h>
#include <iostream>

TakeDamageSystem::TakeDamageSystem() : IReactiveSystem(*this)
{
	// reactive system that collects all take damage components.
	auto m = Matcher_AllOf(TakeDamageComponent);
	trigger = m.OnEntityAdded();
}

void TakeDamageSystem::SetPool(EntitasPP::Pool* pool)
{
	_pool = pool;
}

void TakeDamageSystem::Initialize()
{
}

void TakeDamageSystem::Execute(std::vector<EntitasPP::EntityPtr> entities)
{
	// execute the reaction on an updated (added) TakeDamageComponent
	for (auto e : entities)
	{
		if (!e->IsEnabled()) continue;
		//Get components
		auto damageComponent = e->Get<TakeDamageComponent>();
		auto statsComponent = e->Get<PlayerStatsComponent>();

		//debug messages
		std::cout << damageComponent->attackPower << " , ";
		std::cout << damageComponent->abilityPower << " , ";
		std::cout << statsComponent->health << "\nl";

		//Check if the damage will kill the target
		if (statsComponent->health <= damageComponent->attackPower + damageComponent->abilityPower) //[TODO] DamageReduction
		{
			// if damage kills the target, reset targets values to default and respawn.
			//"Kill" entity [TODO]
			int health = 10; //CHANGE TO MAXHEALTH [TODO]
			int armor = statsComponent->armour;
			int attackdamage = statsComponent->attack_damage;
			int abilitypower = statsComponent->ability_power;
			int magicresist = statsComponent->magic_resist;
			int movement = statsComponent->movement_speed;
			//update the stats component and position component (respawn)
			e->Replace<PlayerStatsComponent>(health, armor, attackdamage, abilitypower, magicresist, movement);

			e->Replace<PlayerPostitionComponent>(glm::vec3(0, 5, 0)); //Reappear centered, in air

			Logger::CustomLog(Logger::Color::Red, "Player KILLED");
		}
		else
		{
			// if not killed by damage
			//Reduce health
			int health = statsComponent->health - damageComponent->attackPower;
			int armor = statsComponent->armour;
			int attackdamage = statsComponent->attack_damage;
			int abilitypower = statsComponent->ability_power;
			int magicresist = statsComponent->magic_resist;
			int movement = statsComponent->movement_speed;

			//update stats component with damage taken.
			e->Replace<PlayerStatsComponent>(health, armor, attackdamage, abilitypower, magicresist, movement);
			Logger::CustomLog(Logger::Color::Red, "Player Damaged");
		}

		// remove the takedamage component, it has been handled.
		std::cout << statsComponent->health;
		e->Remove<TakeDamageComponent>();
	}
}
