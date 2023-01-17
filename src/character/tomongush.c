/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tomongush.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//TomongusH character structure
enum
{
  TomongusH_ArcMain_Idle0,
  TomongusH_ArcMain_Left0,
  TomongusH_ArcMain_Down0,
  TomongusH_ArcMain_Up0,
  TomongusH_ArcMain_Right0,
	
	TomongusH_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[TomongusH_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_TomongusH;

//TomongusH character definitions
static const CharFrame char_tomongush_frame[] = {
  {TomongusH_ArcMain_Idle0, {  0,  0, 45, 33}, {147,140}}, //0 idle 1
  {TomongusH_ArcMain_Idle0, { 46,  0, 41, 34}, {145,141}}, //1 idle 2
  {TomongusH_ArcMain_Idle0, { 88,  0, 39, 36}, {144,143}}, //2 idle 3
  {TomongusH_ArcMain_Idle0, {128,  0, 39, 38}, {144,145}}, //3 idle 4

  {TomongusH_ArcMain_Left0, {  0,  0, 51, 41}, {146,148}}, //4 left 1
  {TomongusH_ArcMain_Left0, { 52,  0, 51, 41}, {145,148}}, //5 left 2
  {TomongusH_ArcMain_Left0, {104,  0, 48, 37}, {145,144}}, //6 left 3

  {TomongusH_ArcMain_Down0, {  0,  0, 50, 33}, {147,140}}, //7 down 1
  {TomongusH_ArcMain_Down0, { 51,  0, 49, 34}, {146,141}}, //8 down 2
  {TomongusH_ArcMain_Down0, {101,  0, 43, 34}, {146,141}}, //9 down 3

  {TomongusH_ArcMain_Up0, {  0,  0, 45, 43}, {143,150}}, //10 up 1
  {TomongusH_ArcMain_Up0, { 46,  0, 45, 42}, {143,149}}, //11 up 2
  {TomongusH_ArcMain_Up0, { 92,  0, 37, 41}, {143,148}}, //12 up 3

  {TomongusH_ArcMain_Right0, {  0,  0, 59, 37}, {149,146}}, //13 right 1
  {TomongusH_ArcMain_Right0, { 60,  0, 59, 37}, {150,146}}, //14 right 2
  {TomongusH_ArcMain_Right0, {120,  0, 46, 37}, {150,146}}, //15 right 3
};

static const Animation char_tomongush_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, 6, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 12, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 13, 14, 15, 15, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//TomongusH character functions
void Char_TomongusH_SetFrame(void *user, u8 frame)
{
	Char_TomongusH *this = (Char_TomongusH*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tomongush_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_TomongusH_Tick(Character *character)
{
	Char_TomongusH *this = (Char_TomongusH*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_TomongusH_SetFrame);
	if (stage.stage_id != StageId_Rivals)
		Character_Draw(character, &this->tex, &char_tomongush_frame[this->frame]);
	else if ((stage.stage_id == StageId_Rivals) && (stage.song_step >= 1034))
		Character_Draw(character, &this->tex, &char_tomongush_frame[this->frame]);
}

void Char_TomongusH_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_TomongusH_Free(Character *character)
{
	Char_TomongusH *this = (Char_TomongusH*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_TomongusH_New(fixed_t x, fixed_t y)
{
	//Allocate tomongush object
	Char_TomongusH *this = Mem_Alloc(sizeof(Char_TomongusH));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_TomongusH_New] Failed to allocate tomongush object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_TomongusH_Tick;
	this->character.set_anim = Char_TomongusH_SetAnim;
	this->character.free = Char_TomongusH_Free;
	
	Animatable_Init(&this->character.animatable, char_tomongush_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFF8A572;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\TOMONGH.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
