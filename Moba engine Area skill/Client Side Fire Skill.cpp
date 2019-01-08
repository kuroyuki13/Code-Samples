#include "Gameplay\Skills.h"
#include "GamePlay\ChampionBase.h"
#include "Window\Input.h"
#include "Components\PlayerPositionComponent.h"
#include "Components\CastSkillComponent.h" 

//castskill component: skillnumber, target, targetplayerid
namespace MobaEngine
{
	// check if skill is off cooldown
	void Skill::CalculateCooldown(float _dt)
	{
		if (m_Fired)
		{
			cooldowncounter += _dt;
			if (cooldowncounter >= m_Cooldown)
			{
				m_Fired = false;
				cooldowncounter = 0;
			}
		}
	}
	/*
	all skills use the function replace castskill component with a skill number, target and player id as values.
	for skills that target a player, use the player ID.
	for skills that target a location, or direction, use the target variable. 
	in the replace castskill component function, only the required values will be used.
	*/

	// shooting a skillshot
	void SkillShot::Fire(EntitasPP::EntityPtr _owner, Input* _input, int _skillnumber)
	{
		//get location and direction of skill
		glm::vec3 target = _input->TempUnProject();
		glm::vec3 direction = glm::normalize(target - _owner->Get<PlayerPostitionComponent>()->value);

		//if possible to fire and clicked
		if (_input->isKeyPressed(Input::MOUSE_LEFT) & !m_Fired)
		{
			//replace skill Castskill component. 
			_owner->Replace<CastSkillComponent>(_skillnumber, target, 0);
			m_Fired = true;
		}
	}
	void AreaSkill::Fire(EntitasPP::EntityPtr _owner, Input* _input, int _skillnumber)
	{
		
		glm::vec3 target = _input->TempUnProject();
		glm::vec3 direction = glm::normalize(target - _owner->Get<PlayerPostitionComponent>()->value);
		if (_input->isKeyPressed(Input::MOUSE_LEFT) & !m_Fired)
		{
			_owner->Replace<CastSkillComponent>(_skillnumber, target, 0);
			m_Fired = true;
		}
	}
	void BuffSkill::Fire(EntitasPP::EntityPtr _owner, Input* _input, int _skillnumber)
	{
		//test code to test system, replace with proper variables.
		glm::vec3 target = _input->TempUnProject();
		glm::vec3 direction = glm::normalize(target - _owner->Get<PlayerPostitionComponent>()->value);
		if (_input->isKeyPressed(Input::MOUSE_LEFT) & !m_Fired)
		{
			_owner->Replace<CastSkillComponent>(_skillnumber, target, 0);
			m_Fired = true;
		}
	}
	void TargetSkill::Fire(EntitasPP::EntityPtr _owner, Input* _input, int _skillnumber)
	{
		//test code to test system, replace with proper variables.
		glm::vec3 target = _input->TempUnProject();
		glm::vec3 direction = glm::normalize(target - _owner->Get<PlayerPostitionComponent>()->value);
		if (_input->isKeyPressed(Input::MOUSE_LEFT) & !m_Fired)
		{
			_owner->Replace<CastSkillComponent>(_skillnumber, target, 1);// change the one to propper id when targetting works propperly.
			m_Fired = true;
		}
	}

}