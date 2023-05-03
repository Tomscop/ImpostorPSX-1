/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "warchief.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Warchief character structure
enum
{
  Warchief_ArcMain_Idle0,
  Warchief_ArcMain_Idle1,
  Warchief_ArcMain_Left0,
  Warchief_ArcMain_Left1,
  Warchief_ArcMain_Down0,
  Warchief_ArcMain_Up0,
  Warchief_ArcMain_Up1,
  Warchief_ArcMain_Right0,
  Warchief_ArcMain_Right1,
	
	Warchief_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Warchief_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Warchief;

//Warchief character definitions
static const CharFrame char_warchief_frame[] = {
  {Warchief_ArcMain_Idle0, {  0,  0,126,114}, {153,132}}, //0 idle 1
  {Warchief_ArcMain_Idle0, {126,  0,123,116}, {150,134}}, //1 idle 2
  {Warchief_ArcMain_Idle0, {  0,116,121,121}, {147,138}}, //2 idle 3
  {Warchief_ArcMain_Idle0, {121,116,120,121}, {147,138}}, //3 idle 4
  {Warchief_ArcMain_Idle1, {  0,  0,119,120}, {146,137}}, //4 idle 5
  {Warchief_ArcMain_Idle1, {119,  0,120,120}, {146,137}}, //5 idle 6

  {Warchief_ArcMain_Left0, {  0,  0,118,112}, {152,130}}, //6 left 1
  {Warchief_ArcMain_Left0, {118,  0,118,113}, {152,131}}, //7 left 2
  {Warchief_ArcMain_Left0, {  0,113,117,114}, {149,132}}, //8 left 3
  {Warchief_ArcMain_Left0, {117,113,116,114}, {148,132}}, //9 left 4
  {Warchief_ArcMain_Left1, {  0,  0,115,114}, {147,132}}, //10 left 5
  {Warchief_ArcMain_Left1, {115,  0,116,114}, {148,132}}, //11 left 6
  {Warchief_ArcMain_Left1, {  0,114,115,115}, {148,132}}, //12 left 7

  {Warchief_ArcMain_Down0, {  0,  0,105, 99}, {122,116}}, //13 down 1
  {Warchief_ArcMain_Down0, {105,  0,104,102}, {120,119}}, //14 down 2
  {Warchief_ArcMain_Down0, {  0,102,105,103}, {121,120}}, //15 down 3
  {Warchief_ArcMain_Down0, {105,102,105,103}, {121,120}}, //16 down 4

  {Warchief_ArcMain_Up0, {  0,  0,130,114}, {139,132}}, //17 up 1
  {Warchief_ArcMain_Up0, {  0,114,128,114}, {138,132}}, //18 up 2
  {Warchief_ArcMain_Up1, {  0,  0,128,114}, {138,132}}, //19 up 3

  {Warchief_ArcMain_Right0, {  0,  0,131,114}, {116,132}}, //20 right 1
  {Warchief_ArcMain_Right0, {  0,114,134,113}, {119,131}}, //21 right 2
  {Warchief_ArcMain_Right1, {  0,  0,129,113}, {119,131}}, //22 right 3
  {Warchief_ArcMain_Right1, {  0,113,133,113}, {118,131}}, //23 right 4
};

static const Animation char_warchief_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 6, 6, 7, 7, 8, 9, 10, 11, 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 13, 14, 15, 16, 16, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 17, 18, 19, 19, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 20, 21, 22, 23, 23, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Warchief character functions
void Char_Warchief_SetFrame(void *user, u8 frame)
{
	Char_Warchief *this = (Char_Warchief*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_warchief_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Warchief_Tick(Character *character)
{
	Char_Warchief *this = (Char_Warchief*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Victory)
	{
		if (stage.song_step == 129)
		{
			this->character.focus_x = FIXED_DEC(21,1);
			this->character.focus_zoom = FIXED_DEC(950,1024);
		}
		if (stage.song_step == -29)
			this->character.health_bar = 0xFF663093;
		if (stage.song_step == 464)
			this->character.health_bar = 0xFFFFC8E2;
		if (stage.song_step == 720)
			this->character.health_bar = 0xFF277FE7;
		if (stage.song_step == 992)
			this->character.health_bar = 0xFF663093;
		if (stage.song_step == 1056)
			this->character.health_bar = 0xFFFFC8E2;
		if (stage.song_step == 1120)
			this->character.health_bar = 0xFF277FE7;
	}
		
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Warchief_SetFrame);
	if (stage.stage_id == StageId_VotingTime)
		Character_Draw(character, &this->tex, &char_warchief_frame[this->frame]);
	else if ((stage.camswitch == 0) || (stage.camswitch == 1) || (stage.camswitch == 4))
		Character_DrawCol(character, &this->tex, &char_warchief_frame[this->frame], 175, 175, 175);
}

void Char_Warchief_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Warchief_Free(Character *character)
{
	Char_Warchief *this = (Char_Warchief*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Warchief_New(fixed_t x, fixed_t y)
{
	//Allocate warchief object
	Char_Warchief *this = Mem_Alloc(sizeof(Char_Warchief));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Warchief_New] Failed to allocate warchief object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Warchief_Tick;
	this->character.set_anim = Char_Warchief_SetAnim;
	this->character.free = Char_Warchief_Free;
	
	Animatable_Init(&this->character.animatable, char_warchief_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFF663093;
	
	if (stage.stage_id == StageId_VotingTime)
	{
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-118,1);
	this->character.focus_zoom = FIXED_DEC(1018,1024);
	}
	else if (stage.stage_id == StageId_Victory)
	{
	this->character.focus_x = FIXED_DEC(57,1);
	this->character.focus_y = FIXED_DEC(-118,1);
	this->character.focus_zoom = FIXED_DEC(1000,1024);
	}
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\WARCHIEF.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
