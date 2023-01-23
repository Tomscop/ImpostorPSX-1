/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redmd.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Red Meltdown character structure
enum
{
  RedMeltdown_ArcMain_Idle0,
  RedMeltdown_ArcMain_Idle1,
  RedMeltdown_ArcMain_Idle2,
  RedMeltdown_ArcMain_Idle3,
  RedMeltdown_ArcMain_Idle4,
  RedMeltdown_ArcMain_Idle5,
  RedMeltdown_ArcMain_Idle6,
  RedMeltdown_ArcMain_Idle7,
  RedMeltdown_ArcMain_Left0,
  RedMeltdown_ArcMain_Left1,
  RedMeltdown_ArcMain_Left2,
  RedMeltdown_ArcMain_Left3,
  RedMeltdown_ArcMain_Left4,
  RedMeltdown_ArcMain_Down0,
  RedMeltdown_ArcMain_Down1,
  RedMeltdown_ArcMain_Up0,
  RedMeltdown_ArcMain_Up1,
  RedMeltdown_ArcMain_Up2,
  RedMeltdown_ArcMain_Right0,
  RedMeltdown_ArcMain_Right1,
  RedMeltdown_ArcMain_Right2,
  RedMeltdown_ArcMain_Right3,
	
	RedMeltdown_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RedMeltdown_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RedMeltdown;

//Red Meltdown character definitions
static const CharFrame char_redmd_frame[] = {
  {RedMeltdown_ArcMain_Idle0, {  0,  0,141,105}, {167, 75}}, //0 idle 1
  {RedMeltdown_ArcMain_Idle0, {  0,106,140,105}, {167, 75}}, //1 idle 2
  {RedMeltdown_ArcMain_Idle1, {  0,  0,140,106}, {167, 76}}, //2 idle 3
  {RedMeltdown_ArcMain_Idle1, {  0,107,141,106}, {167, 76}}, //3 idle 4
  {RedMeltdown_ArcMain_Idle2, {  0,  0,138,108}, {167, 78}}, //4 idle 5
  {RedMeltdown_ArcMain_Idle2, {  0,109,138,108}, {167, 78}}, //5 idle 6
  {RedMeltdown_ArcMain_Idle3, {  0,  0,136,109}, {167, 79}}, //6 idle 7
  {RedMeltdown_ArcMain_Idle3, {  0,110,136,109}, {167, 79}}, //7 idle 8
  {RedMeltdown_ArcMain_Idle4, {  0,  0,135,110}, {167, 80}}, //8 idle 9
  {RedMeltdown_ArcMain_Idle4, {  0,111,135,110}, {167, 80}}, //9 idle 10
  {RedMeltdown_ArcMain_Idle5, {  0,  0,135,110}, {167, 80}}, //10 idle 11
  {RedMeltdown_ArcMain_Idle5, {  0,111,134,110}, {167, 80}}, //11 idle 12
  {RedMeltdown_ArcMain_Idle6, {  0,  0,135,110}, {167, 80}}, //12 idle 13
  {RedMeltdown_ArcMain_Idle6, {  0,111,135,110}, {167, 80}}, //13 idle 14
  {RedMeltdown_ArcMain_Idle7, {  0,  0,134,110}, {167, 80}}, //14 idle 15

  {RedMeltdown_ArcMain_Left0, {  0,  0,132,111}, {190, 81}}, //15 left 1
  {RedMeltdown_ArcMain_Left0, {  0,112,132,111}, {190, 81}}, //16 left 2
  {RedMeltdown_ArcMain_Left1, {  0,  0,130,112}, {185, 81}}, //17 left 3
  {RedMeltdown_ArcMain_Left1, {  0,113,131,112}, {185, 81}}, //18 left 4
  {RedMeltdown_ArcMain_Left2, {  0,  0,131,111}, {186, 81}}, //19 left 5
  {RedMeltdown_ArcMain_Left2, {  0,112,131,112}, {186, 81}}, //20 left 6
  {RedMeltdown_ArcMain_Left3, {  0,  0,131,112}, {186, 81}}, //21 left 7
  {RedMeltdown_ArcMain_Left3, {  0,113,131,112}, {186, 81}}, //22 left 8
  {RedMeltdown_ArcMain_Left4, {  0,  0,131,112}, {186, 81}}, //23 left 9
  {RedMeltdown_ArcMain_Left4, {  0,113,131,111}, {185, 81}}, //24 left 10

  {RedMeltdown_ArcMain_Down0, {  0,  0,123, 91}, {167, 61}}, //25 down 1
  {RedMeltdown_ArcMain_Down0, {124,  0,123, 90}, {167, 61}}, //26 down 2
  {RedMeltdown_ArcMain_Down0, {  0, 92,123, 93}, {167, 63}}, //27 down 3
  {RedMeltdown_ArcMain_Down0, {124, 92,123, 93}, {167, 63}}, //28 down 4
  {RedMeltdown_ArcMain_Down1, {  0,  0,123, 93}, {167, 63}}, //29 down 5
  {RedMeltdown_ArcMain_Down1, {124,  0,123, 93}, {167, 63}}, //30 down 6
  {RedMeltdown_ArcMain_Down1, {  0, 94,123, 93}, {167, 63}}, //31 down 7
  {RedMeltdown_ArcMain_Down1, {124, 94,123, 93}, {167, 63}}, //32 down 8

  {RedMeltdown_ArcMain_Up0, {  0,  0,123,117}, {167, 86}}, //33 up 1
  {RedMeltdown_ArcMain_Up0, {124,  0,123,116}, {167, 86}}, //34 up 2
  {RedMeltdown_ArcMain_Up0, {  0,118,125,114}, {167, 84}}, //35 up 3
  {RedMeltdown_ArcMain_Up0, {126,118,125,114}, {167, 84}}, //36 up 4
  {RedMeltdown_ArcMain_Up1, {  0,  0,124,114}, {167, 84}}, //37 up 5
  {RedMeltdown_ArcMain_Up1, {125,  0,124,115}, {166, 84}}, //38 up 6
  {RedMeltdown_ArcMain_Up1, {  0,116,124,114}, {167, 84}}, //39 up 7
  {RedMeltdown_ArcMain_Up1, {125,116,125,114}, {167, 84}}, //40 up 8
  {RedMeltdown_ArcMain_Up2, {  0,  0,125,114}, {167, 84}}, //41 up 9
  {RedMeltdown_ArcMain_Up2, {126,  0,125,114}, {167, 84}}, //42 up 10

  {RedMeltdown_ArcMain_Right0, {  0,  0,149,109}, {167, 79}}, //43 right 1
  {RedMeltdown_ArcMain_Right0, {  0,110,149,109}, {167, 79}}, //44 right 2
  {RedMeltdown_ArcMain_Right1, {  0,  0,150,110}, {167, 79}}, //45 right 3
  {RedMeltdown_ArcMain_Right1, {  0,111,149,110}, {167, 79}}, //46 right 4
  {RedMeltdown_ArcMain_Right2, {  0,  0,150,110}, {167, 79}}, //47 right 5
  {RedMeltdown_ArcMain_Right2, {  0,111,150,110}, {167, 79}}, //48 right 6
  {RedMeltdown_ArcMain_Right3, {  0,  0,150,110}, {167, 79}}, //49 right 7
  {RedMeltdown_ArcMain_Right3, {  0,111,151,110}, {167, 79}}, //50 right 8
};

static const Animation char_redmd_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 25, 26, 27, 28, 29, 30, 31, 32, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 43, 44, 45, 46, 47, 48, 49, 50, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Red Meltdown character functions
void Char_RedMeltdown_SetFrame(void *user, u8 frame)
{
	Char_RedMeltdown *this = (Char_RedMeltdown*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redmd_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RedMeltdown_Tick(Character *character)
{
	Char_RedMeltdown *this = (Char_RedMeltdown*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_RedMeltdown_SetFrame);
	Character_Draw(character, &this->tex, &char_redmd_frame[this->frame]);
}

void Char_RedMeltdown_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RedMeltdown_Free(Character *character)
{
	Char_RedMeltdown *this = (Char_RedMeltdown*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RedMeltdown_New(fixed_t x, fixed_t y)
{
	//Allocate redmd object
	Char_RedMeltdown *this = Mem_Alloc(sizeof(Char_RedMeltdown));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RedMeltdown_New] Failed to allocate redmd object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RedMeltdown_Tick;
	this->character.set_anim = Char_RedMeltdown_SetAnim;
	this->character.free = Char_RedMeltdown_Free;
	
	Animatable_Init(&this->character.animatable, char_redmd_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFBB2D30;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\REDMD.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "idle7.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
