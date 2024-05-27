#pragma once

//structure that represents skills
typedef struct{
    int shield_t;//shield applied on target
    int poison_t;//poison applied on target
    int weakness_t;//weakness applied on target
    int weak_t;//weak applied on target
    int brittle_t;//brittle applied on target
    int wounded_t;//wounded  applied on target
    int strength_t;//strength applied on target
    int damage_t;//damages dealt to target
    int damage_tick_t;//number of times damages are dealt to target

    int target_type;//targetting type (0=player only, 1=self only, 2= all allies, 3= all)
    
    int shield_s;//shield applied on self
    int poison_s;//poison applied on self
    int weakness_s;//weakness applied on self
    int weak_s;//weak applied on self
    int brittle_s;//brittle applied on self
    int wounded_s;//wounded applied on self
    int strength_s;//strength applied on self
    int damage_s;//damages dealt to self
    int damage_tick_s;//number of times damages are dealt to self
}Skill;



void printSkill(Skill* skill);
