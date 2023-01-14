/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tomongus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Tomongus character structure
enum
{
  Tomongus_ArcMain_Idle0,
  Tomongus_ArcMain_Left0,
  Tomongus_ArcMain_Down0,
  Tomongus_ArcMain_Up0,
  Tomongus_ArcMain_Right0,
  Tomongus_ArcMain_Huh0,
	
	Tomongus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Tomongus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Tomongus;

//Tomongus character definitions
static const CharFrame char_tomongus_frame[] = {
  {Tomongus_ArcMain_Idle0, {  0,  0, 45, 38}, {147,141}}, //0 idle 1
  {Tomongus_ArcMain_Idle0, { 46,  0, 41, 40}, {145,143}}, //1 idle 2
  {Tomongus_ArcMain_Idle0, { 88,  0, 39, 41}, {144,144}}, //2 idle 3
  {Tomongus_ArcMain_Idle0, {128,  0, 39, 42}, {144,145}}, //3 idle 4

  {Tomongus_ArcMain_Left0, {  0,  0, 48, 44}, {147,147}}, //4 left 1
  {Tomongus_ArcMain_Left0, { 49,  0, 48, 42}, {146,145}}, //5 left 2

  {Tomongus_ArcMain_Down0, {  0,  0, 45, 37}, {147,140}}, //6 down 1
  {Tomongus_ArcMain_Down0, { 46,  0, 43, 38}, {146,141}}, //7 down 2

  {Tomongus_ArcMain_Up0, {  0,  0, 37, 47}, {143,150}}, //8 up 1
  {Tomongus_ArcMain_Up0, { 38,  0, 37, 46}, {143,149}}, //9 up 2

  {Tomongus_ArcMain_Right0, {  0,  0, 48, 43}, {149,148}}, //10 right 1
  {Tomongus_ArcMain_Right0, { 49,  0, 48, 43}, {150,148}}, //11 right 2

  {Tomongus_ArcMain_Huh0, {  0,  0, 39, 38}, {144,141}}, //12 huh 1
  {Tomongus_ArcMain_Huh0, { 40,  0, 39, 38}, {144,141}}, //13 huh 2
};

static const Animation char_tomongus_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){ 3, 12, 12, 13, 13, 12, 12, 13, 13, 12, 12, 12, 12, 12, 12, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Tomongus character functions
void Char_Tomongus_SetFrame(void *user, u8 frame)
{
	Char_Tomongus *this = (Char_Tomongus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tomongus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Tomongus_Tick(Character *character)
{
	Char_Tomongus *this = (Char_Tomongus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Tomongus_SetFrame);
	Character_Draw(character, &this->tex, &char_tomongus_frame[this->frame]);
}

void Char_Tomongus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Tomongus_Free(Character *character)
{
	Char_Tomongus *this = (Char_Tomongus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Tomongus_New(fixed_t x, fixed_t y)
{
	//Allocate tomongus object
	Char_Tomongus *this = Mem_Alloc(sizeof(Char_Tomongus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Tomongus_New] Failed to allocate tomongus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Tomongus_Tick;
	this->character.set_anim = Char_Tomongus_SetAnim;
	this->character.free = Char_Tomongus_Free;
	
	Animatable_Init(&this->character.animatable, char_tomongus_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFFF6770;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\TOMONGUS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "huh0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
