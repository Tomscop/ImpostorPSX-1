/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blacklegs.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BlackLegs character structure
enum
{
  BlackLegs_ArcMain_Legs0,
	
	BlackLegs_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlackLegs_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BlackLegs;

//BlackLegs character definitions
static const CharFrame char_blacklegs_frame[] = {
  {BlackLegs_ArcMain_Legs0, {  0,  0, 48, 40}, {143, 55}}, //0 legs 1
  {BlackLegs_ArcMain_Legs0, { 48,  0, 48, 39}, {143, 55}}, //1 legs 2
  {BlackLegs_ArcMain_Legs0, { 96,  0, 48, 37}, {143, 55}}, //2 legs 3
  {BlackLegs_ArcMain_Legs0, {144,  0, 48, 41}, {143, 55}}, //3 legs 4
  {BlackLegs_ArcMain_Legs0, {192,  0, 49, 44}, {144, 55}}, //4 legs 5
  {BlackLegs_ArcMain_Legs0, {  0, 44, 50, 44}, {144, 55}}, //5 legs 6
  {BlackLegs_ArcMain_Legs0, { 50, 44, 61, 44}, {149, 55}}, //6 legs 7
  {BlackLegs_ArcMain_Legs0, {111, 44, 69, 39}, {153, 55}}, //7 legs 8
  {BlackLegs_ArcMain_Legs0, {180, 44, 70, 37}, {153, 55}}, //8 legs 9
  {BlackLegs_ArcMain_Legs0, {  0, 88, 71, 37}, {152, 55}}, //9 legs 10
  {BlackLegs_ArcMain_Legs0, { 71, 88, 69, 39}, {151, 55}}, //10 legs 11
  {BlackLegs_ArcMain_Legs0, {140, 88, 58, 42}, {147, 55}}, //11 legs 12
  {BlackLegs_ArcMain_Legs0, {198, 88, 50, 42}, {144, 55}}, //12 legs 13
  {BlackLegs_ArcMain_Legs0, {  0,132, 49, 42}, {144, 55}}, //13 legs 14
};

static const Animation char_blacklegs_anim[CharAnim_Max] = {
	{1, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 2, 3, 4, 5, 6, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{1, (const u8[]){ 7, 8, 8, 9, 10, 11, 12, 13, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//BlackLegs character functions
void Char_BlackLegs_SetFrame(void *user, u8 frame)
{
	Char_BlackLegs *this = (Char_BlackLegs*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blacklegs_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlackLegs_Tick(Character *character)
{
	Char_BlackLegs *this = (Char_BlackLegs*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
	{
		Character_CheckEndSing(character);
		
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			if ((Animatable_Ended(&character->animatable) || character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_RightAlt) &&
				(character->animatable.anim != CharAnim_Left &&
				 character->animatable.anim != CharAnim_Down &&
				 character->animatable.anim != CharAnim_Up &&
				 character->animatable.anim != CharAnim_Right) &&
				(stage.song_step & 0x3) == 0)
				character->set_anim(character, CharAnim_Idle);
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BlackLegs_SetFrame);
	Character_Draw(character, &this->tex, &char_blacklegs_frame[this->frame]);
}

void Char_BlackLegs_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Idle)
	{
		if (character->animatable.anim == CharAnim_LeftAlt)
			anim = CharAnim_RightAlt;
		else
			anim = CharAnim_LeftAlt;
		character->sing_end = FIXED_DEC(0x7FFF,1);
	}
	else
	{
		Character_CheckStartSing(character);
	}
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_BlackLegs_Free(Character *character)
{
	Char_BlackLegs *this = (Char_BlackLegs*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlackLegs_New(fixed_t x, fixed_t y)
{
	//Allocate blacklegs object
	Char_BlackLegs *this = Mem_Alloc(sizeof(Char_BlackLegs));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlackLegs_New] Failed to allocate blacklegs object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlackLegs_Tick;
	this->character.set_anim = Char_BlackLegs_SetAnim;
	this->character.free = Char_BlackLegs_Free;
	
	Animatable_Init(&this->character.animatable, char_blacklegs_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(21671,10000);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLAKLEGS.ARC;1");
	
	const char **pathp = (const char *[]){
  "legs0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
