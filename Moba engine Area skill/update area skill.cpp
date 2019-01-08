#include "Systems\AreaSkillUpdateSystem.h"
#include "TrickReplication\Entitas\Pool.hpp"
#include "Components\PlayerIdComponent.h"
#include "Components\PlayerPositionComponent.h"
#include "Components\PlayerStatsComponent.h"
#include "Components\IsAttackableComponent.h"
#include "SkillComponents.h"
#include "Components\TakeDamageComponent.h"
#include "glm\glm.hpp"


void AreaSkillUpdateSystem::Initialize()
{
	// create observers that will collect entities that match the required querries (skills, and targets)
	_observer = _pool->GetGroup(Matcher_AllOf(isAreaSkillComponent))->CreateObserver(EntitasPP::GroupEventType::OnEntityAdded);
	_observer->Activate();
	_targets = _pool->GetGroup(Matcher_AllOf(PlayerPostitionComponent, PlayerIdComponent))->CreateObserver(EntitasPP::GroupEventType::OnEntityAdded);
	_targets->Activate();
	timeStep = 0.015625f;//change with trickcore timestep  !!! test. 
}

void AreaSkillUpdateSystem::SetPool(EntitasPP::Pool* pool)
{
	_pool = pool;
}

void AreaSkillUpdateSystem::FixedExecute()
{
	//react on updated skill entities
	for (auto e : _observer->GetCollectedEntities())
	{
		//skip if null or disabled
		if (!e->IsEnabled()) continue;
		if (e == nullptr) continue;
		bool destroy_me = false;

		//get position and radius values
		glm::vec3 pos = e->Get<SkillPositionComponent>()->value;
		float rad = e->Get<SkillRadiusComponent>()->value;

		//check if delay has passed.
		float timer = e->Get<CounterComponent>()->value;
		if (timer < e->Get<SkillDelayComponent>()->value)
		{
			e->Replace<CounterComponent>(timer + timeStep);
		}
		else if (timer > e->Get<SkillDelayComponent>()->value && !destroy_me)
		{
			//if delay has passed check for players within entities radius and do damage
			for (auto t : _targets->GetCollectedEntities())
			{
				if (t->Get<PlayerIdComponent>()->value != e->Get<OwnedByPlayerComponent>()->value)
				{
					// use basic circle collision check to determine if player is within radius.
					float radius_target = 1.f; //change this to different radius that is suitable for target.
					float distance = glm::distance(pos, t->Get<PlayerPostitionComponent>()->value);
					float hit_distance = rad*rad + radius_target*radius_target;
					if (distance < hit_distance)
					{
						//player is within radius, add damage component. reaction on this component will handle damage.
						t->Add<TakeDamageComponent>(e->Get<SkillDamageComponent>()->p_damage, e->Get<SkillDamageComponent>()->m_damage);
						destroy_me = true;
					}
				}
			}
			destroy_me = true;
		}




		//destroy skill entity if it has reached its max range or hit an enemy.
		if (destroy_me)
		{
			for (auto eee : _pool->GetEntities())
			{
				if (eee == e)
				{
					_pool->DestroyEntity(e);
				}
			}
		}
	}
	//_observer->ClearCollectedEntities();

}


