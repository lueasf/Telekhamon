#include "getfromid.h"





// Chemin vers la database.
const char* db_path = "GameData/database/skills.db";





//function to get ennemi name from ennemi id
char *from_id_name(int id){
    sqlite3 *database;//init database
    sqlite3_stmt *data;//init statement
    sqlite3_open(db_path, &database);//open connection to database
    char temp [3];//buffer for string convertion of id
    itoa(id,temp,10);//string convertion of id
    char buffer[50] = "SELECT Name from Ennemies WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(buffer,temp),-1,&data,NULL);//compile request to byte code
    sqlite3_step(data);//execute request
    const unsigned char* req_name = sqlite3_column_text(data,0);//get name from matched data
    char* name = (char*)malloc(sizeof(char)*(strlen((const char*)req_name)+1));
    strcpy(name, (const char*)req_name);
    sqlite3_finalize(data);//finish request
    sqlite3_close(database);//close connection to database
    return name;
}

//function to get ennemi max health from ennemi id
int from_id_health(int id,int modifier){
sqlite3 *database;
    sqlite3_stmt *data;
    sqlite3_open(db_path, &database);
    char temp [3];
    itoa(id,temp,10);
    char buffer[50] = "SELECT HP from Ennemies WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(buffer,temp),-1,&data,NULL);
    sqlite3_step(data);
    int HP = sqlite3_column_int(data,0);
    sqlite3_finalize(data);
    sqlite3_close(database);
    return HP;
}

//function to get ennemi skill 1 from ennemi id
Skill* from_id_skill1(int id){
    sqlite3 *database;
    sqlite3_stmt *data;
    sqlite3_open(db_path, &database);
    char temp [3];
    itoa(id,temp,10);
    char buffer[60] = "SELECT * from skills WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(strcat(buffer,temp)," AND SkillNum = 1"),-1,&data,NULL);
    sqlite3_step(data);
    Skill* skill1 = malloc (sizeof(Skill));//init skill to return
    //set defined skill values to values from skill matched from request
    skill1->shield_t = sqlite3_column_int(data,1);
    skill1->poison_t = sqlite3_column_int(data,2);
    skill1->weakness_t = sqlite3_column_int(data,3);
    skill1->weak_t = sqlite3_column_int(data,4);
    skill1->brittle_t = sqlite3_column_int(data,5);
    skill1->wounded_t = sqlite3_column_int(data,6);
    skill1->strength_t = sqlite3_column_int(data,7);
    skill1->damage_t = sqlite3_column_int(data,8);
    skill1->damage_tick_t = sqlite3_column_int(data,9);
    skill1->target_type = sqlite3_column_int(data,10);
    skill1->shield_s = sqlite3_column_int(data,11);
    skill1->poison_s = sqlite3_column_int(data,12);
    skill1->weakness_s = sqlite3_column_int(data,13);
    skill1->weak_s = sqlite3_column_int(data,14);
    skill1->brittle_s = sqlite3_column_int(data,15);
    skill1->wounded_s = sqlite3_column_int(data,16);
    skill1->strength_s = sqlite3_column_int(data,17);
    skill1->damage_s = sqlite3_column_int(data,18);
    skill1->damage_tick_s = sqlite3_column_int(data,19);
    sqlite3_finalize(data);
    sqlite3_close(database);
    return skill1;
}

//function to get ennemi skill 2 from ennemi id
Skill* from_id_skill2(int id){
    sqlite3 *database;
    sqlite3_stmt *data;
    sqlite3_open(db_path, &database);
    char temp [3];
    itoa(id,temp,10);
    char buffer[60] = "SELECT * from skills WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(strcat(buffer,temp)," AND SkillNum = 2"),-1,&data,NULL);
    sqlite3_step(data);
    Skill* skill2 = malloc (sizeof(Skill));
    skill2->shield_t = sqlite3_column_int(data,1);
    skill2->poison_t = sqlite3_column_int(data,2);
    skill2->weakness_t = sqlite3_column_int(data,3);
    skill2->weak_t = sqlite3_column_int(data,4);
    skill2->brittle_t = sqlite3_column_int(data,5);
    skill2->wounded_t = sqlite3_column_int(data,6);
    skill2->strength_t = sqlite3_column_int(data,7);
    skill2->damage_t = sqlite3_column_int(data,8);
    skill2->damage_tick_t = sqlite3_column_int(data,9);
    skill2->target_type = sqlite3_column_int(data,10);
    skill2->shield_s = sqlite3_column_int(data,11);
    skill2->poison_s = sqlite3_column_int(data,12);
    skill2->weakness_s = sqlite3_column_int(data,13);
    skill2->weak_s = sqlite3_column_int(data,14);
    skill2->brittle_s = sqlite3_column_int(data,15);
    skill2->wounded_s = sqlite3_column_int(data,16);
    skill2->strength_s = sqlite3_column_int(data,17);
    skill2->damage_s = sqlite3_column_int(data,18);
    skill2->damage_tick_s = sqlite3_column_int(data,19);
    sqlite3_finalize(data);
    sqlite3_close(database);
    return skill2;
}

//function to get ennemi skill 3 from ennemi id
Skill* from_id_skill3(int id){
    sqlite3 *database;
    sqlite3_stmt *data;
    sqlite3_open(db_path, &database);
    char temp [3];
    itoa(id,temp,10);
    char buffer[60] = "SELECT * from skills WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(strcat(buffer,temp)," AND SkillNum = 3"),-1,&data,NULL);
    sqlite3_step(data);
    Skill* skill3 = malloc (sizeof(Skill));
    skill3->shield_t = sqlite3_column_int(data,1);
    skill3->poison_t = sqlite3_column_int(data,2);
    skill3->weakness_t = sqlite3_column_int(data,3);
    skill3->weak_t = sqlite3_column_int(data,4);
    skill3->brittle_t = sqlite3_column_int(data,5);
    skill3->wounded_t = sqlite3_column_int(data,6);
    skill3->strength_t = sqlite3_column_int(data,7);
    skill3->damage_t = sqlite3_column_int(data,8);
    skill3->damage_tick_t = sqlite3_column_int(data,9);
    skill3->target_type = sqlite3_column_int(data,10);
    skill3->shield_s = sqlite3_column_int(data,11);
    skill3->poison_s = sqlite3_column_int(data,12);
    skill3->weakness_s = sqlite3_column_int(data,13);
    skill3->weak_s = sqlite3_column_int(data,14);
    skill3->brittle_s = sqlite3_column_int(data,15);
    skill3->wounded_s = sqlite3_column_int(data,16);
    skill3->strength_s = sqlite3_column_int(data,17);
    skill3->damage_s = sqlite3_column_int(data,18);
    skill3->damage_tick_s = sqlite3_column_int(data,19);
    sqlite3_finalize(data);
    sqlite3_close(database);
    return skill3;
}

//function to get ennemi skill 4 from ennemi id (may be null)
Skill* from_id_skill4(int id){
    sqlite3 *database;
    sqlite3_stmt *data;
    sqlite3_open(db_path, &database);
    char temp [3];
    itoa(id,temp,10);
    char buffer[60] = "SELECT * from skills WHERE ID = ";
    sqlite3_prepare_v2(database,strcat(strcat(buffer,temp)," AND SkillNum = 4"),-1,&data,NULL);
    sqlite3_step(data);
    if (sqlite3_column_count(data) < 4) {
        return NULL;
    }
    else {
        Skill* skill4 = malloc (sizeof(Skill));
        skill4->shield_t = sqlite3_column_int(data,1);
        skill4->poison_t = sqlite3_column_int(data,2);
        skill4->weakness_t = sqlite3_column_int(data,3);
        skill4->weak_t = sqlite3_column_int(data,4);
        skill4->brittle_t = sqlite3_column_int(data,5);
        skill4->wounded_t = sqlite3_column_int(data,6);
        skill4->strength_t = sqlite3_column_int(data,7);
        skill4->damage_t = sqlite3_column_int(data,8);
        skill4->damage_tick_t = sqlite3_column_int(data,9);
        skill4->target_type = sqlite3_column_int(data,10);
        skill4->shield_s = sqlite3_column_int(data,11);
        skill4->poison_s = sqlite3_column_int(data,12);
        skill4->weakness_s = sqlite3_column_int(data,13);
        skill4->weak_s = sqlite3_column_int(data,14);
        skill4->brittle_s = sqlite3_column_int(data,15);
        skill4->wounded_s = sqlite3_column_int(data,16);
        skill4->strength_s = sqlite3_column_int(data,17);
        skill4->damage_s = sqlite3_column_int(data,18);
        skill4->damage_tick_s = sqlite3_column_int(data,19);
        sqlite3_finalize(data);
        sqlite3_close(database);
        return skill4;
    };

}

