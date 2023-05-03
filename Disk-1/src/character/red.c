/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "red.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Red character structure
enum
{
	Red_ArcMain_Idle0,
	Red_ArcMain_Idle1,
	Red_ArcMain_Idle2,
	Red_ArcMain_Left0,
	Red_ArcMain_Left1,
	Red_ArcMain_Down0,
	Red_ArcMain_Up0,
	Red_ArcMain_Right0,
	Red_ArcMain_LeftAlt0,
	Red_ArcMain_LeftAlt1,
	Red_ArcMain_LeftAlt2,
	Red_ArcMain_RightAlt0,
	Red_ArcMain_RightAlt1,
	Red_ArcMain_RightAlt2,
	Red_ArcMain_Look0,
	Red_ArcMain_Look1,
	Red_ArcMain_Look2,
	
	Red_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Red_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Red;

//Red character definitions
static const CharFrame char_red_frame[] = {
	{Red_ArcMain_Idle0, {  0,  0,146, 85}, {169, 76}}, //0 idle 1
	{Red_ArcMain_Idle0, {  0, 86,147, 84}, {170, 75}}, //1 idle 2
	{Red_ArcMain_Idle1, {  0,  0,146, 85}, {169, 76}}, //2 idle 3
	{Red_ArcMain_Idle1, {  0, 86,145, 90}, {169, 81}}, //3 idle 4
	{Red_ArcMain_Idle2, {  0,  0,143, 92}, {168, 83}}, //4 idle 5
	
	{Red_ArcMain_Left0, {  0,  0,164, 88}, {212, 79}}, //5 left 1
	{Red_ArcMain_Left0, {  0, 89,164, 88}, {208, 79}}, //6 left 2
	{Red_ArcMain_Left1, {  0,  0,165, 88}, {209, 79}}, //7 left 3
	
	{Red_ArcMain_Down0, {  0,  0,116, 91}, {149, 77}}, //8 down 1
	{Red_ArcMain_Down0, {117,  0,129,100}, {159, 91}}, //9 down 2
	{Red_ArcMain_Down0, {  0,101,129,100}, {159, 91}}, //10 down 3
	
	{Red_ArcMain_Up0, {  0,  0, 98,101}, {146, 92}}, //11 up 1
	{Red_ArcMain_Up0, { 99,  0, 96, 91}, {143, 82}}, //12 up 2
	{Red_ArcMain_Up0, {  0,102, 96, 92}, {143, 83}}, //13 up 3
	
	{Red_ArcMain_Right0, {  0,  0, 97, 95}, {125, 77}}, //14 right 1
	{Red_ArcMain_Right0, { 98,  0,105, 93}, {132, 77}}, //15 right 2
	{Red_ArcMain_Right0, {  0, 96,106, 93}, {132, 77}}, //16 right 3
	
	{Red_ArcMain_LeftAlt0, {  0,  0,154, 89}, {202, 78}}, //17 lefta 1
	{Red_ArcMain_LeftAlt0, {  0, 90,166, 89}, {201, 78}}, //18 lefta 2
	{Red_ArcMain_LeftAlt1, {  0,  0,166, 89}, {202, 78}}, //19 lefta 3
	{Red_ArcMain_LeftAlt1, {  0, 90,166, 89}, {202, 78}}, //20 lefta 4
	{Red_ArcMain_LeftAlt2, {  0,  0,166, 89}, {202, 78}}, //21 lefta 5

	{Red_ArcMain_RightAlt0, {  0,  0,189, 89}, {148, 77}}, //22 righta 1
	{Red_ArcMain_RightAlt0, {  0, 90,199, 87}, {154, 77}}, //23 righta 2
	{Red_ArcMain_RightAlt1, {  0,  0,197, 87}, {154, 77}}, //24 righta 3
	{Red_ArcMain_RightAlt1, {  0, 88,197, 87}, {153, 77}}, //25 righta 4
	{Red_ArcMain_RightAlt2, {  0,  0,198, 87}, {153, 77}}, //26 righta 5

	{Red_ArcMain_Look0, {  0,  0,145, 92}, {168, 82}}, //27 look 1
	{Red_ArcMain_Look0, {  0, 93,146, 90}, {169, 81}}, //28 look 2
	{Red_ArcMain_Look1, {  0,  0,146, 91}, {169, 81}}, //29 look 3
	{Red_ArcMain_Look1, {  0, 92,145, 87}, {170, 78}}, //30 look 4
	{Red_ArcMain_Look2, {  0,  0,147, 87}, {167, 78}}, //31 look 5
	{Red_ArcMain_Look2, {  0, 88,148, 88}, {168, 78}}, //32 look 6
};

static const Animation char_red_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, 7, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 14, 15, 16, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_red_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 17, 18, 19, 20, 21, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 22, 23, 24, 25, 26, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
    
    {1, (const u8[]){ 4, 4, 27, 27, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 31, 31, 32, ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special1
};

//Red character functions
void Char_Red_SetFrame(void *user, u8 frame)
{
	Char_Red *this = (Char_Red*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_red_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Red_Tick(Character *character)
{
	Char_Red *this = (Char_Red*)character;
	
	if(character->animatable.anim != CharAnim_Special1)
	{
	   //Perform idle dance
	   if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
			Character_PerformIdle(character);
	}
	
	if (stage.stage_id == StageId_Sabotage)
		Animatable_Init(&this->character.animatable, char_red_anim2);
	else
		Animatable_Init(&this->character.animatable, char_red_anim);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Sabotage: //Uh
				if (stage.song_step == 816)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Red_SetFrame);
	Character_Draw(character, &this->tex, &char_red_frame[this->frame]);
}

void Char_Red_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Red_Free(Character *character)
{
	Char_Red *this = (Char_Red*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Red_New(fixed_t x, fixed_t y)
{
	//Allocate red object
	Char_Red *this = Mem_Alloc(sizeof(Char_Red));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Red_New] Failed to allocate red object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Red_Tick;
	this->character.set_anim = Char_Red_SetAnim;
	this->character.free = Char_Red_Free;
	
	Animatable_Init(&this->character.animatable, char_red_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFFBB2D30;
	
	this->character.focus_x = FIXED_DEC(-39,1);
	this->character.focus_y = FIXED_DEC(-76,1);
	this->character.focus_zoom = FIXED_DEC(509,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\RED.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"left0.tim",
		"left1.tim",
		"down0.tim",
		"up0.tim",
		"right0.tim",
		"lefta0.tim",
		"lefta1.tim",
		"lefta2.tim",
		"righta0.tim",
		"righta1.tim",
		"righta2.tim",
		"look0.tim",
		"look1.tim",
		"look2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
