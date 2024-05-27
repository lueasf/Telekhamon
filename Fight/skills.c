#include "skills.h"

#include <stdio.h>





void printSkill(Skill* skill) {
    printf("Skill:\n");

    printf("shield_t: %d\n", skill->shield_t);
    printf("poison_t: %d\n", skill->poison_t);
    printf("weakness_t: %d\n", skill->weakness_t);
    printf("weak_t: %d\n", skill->weak_t);
    printf("brittle_t: %d\n", skill->brittle_t);
    printf("wounded_t: %d\n", skill->wounded_t);
    printf("strength_t: %d\n", skill->strength_t);
    printf("damage_t: %d\n", skill->damage_t);
    printf("damage_tick_t: %d\n", skill->damage_tick_t);

    printf("target_type: %d\n", skill->target_type);

    printf("shield_s: %d\n", skill->shield_s);
    printf("poison_s: %d\n", skill->poison_s);
    printf("weakness_s: %d\n", skill->weakness_s);
    printf("weak_s: %d\n", skill->weak_s);
    printf("brittle_s: %d\n", skill->brittle_s);
    printf("wounded_s: %d\n", skill->wounded_s);
    printf("strength_s: %d\n", skill->strength_s);
    printf("damage_s: %d\n", skill->damage_s);
    printf("damage_tick_s: %d\n", skill->damage_tick_s);
    printf("\n");
}