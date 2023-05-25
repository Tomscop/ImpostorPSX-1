/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bflegs.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BFLegs character structure
enum
{
  BFLegs_ArcMain_Legs0,
	
	BFLegs_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFLegs_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BFLegs;

//BFLegs character definitions
static const CharFrame char_bflegs_frame[] = {
  {BFLegs_ArcMain_Legs0, {  0,  0, 49, 52}, {156, 94}}, //0 legs 1
  {BFLegs_ArcMain_Legs0, { 49,  0, 52, 50}, {159, 94}}, //1 legs 2
  {BFLegs_ArcMain_Legs0, {101,  0, 93, 50}, {178,106}}, //2 legs 3
  {BFLegs_ArcMain_Legs0, {  0, 52, 96, 55}, {182,109}}, //3 legs 4
  {BFLegs_ArcMain_Legs0, { 96, 52, 90, 53}, {181,103}}, //4 legs 5
  {BFLegs_ArcMain_Legs0, {186, 52, 51, 47}, {157, 94}}, //5 legs 6
  {BFLegs_ArcMain_Legs0, {  0,107, 62, 48}, {159, 95}}, //6 legs 7
  {BFLegs_ArcMain_Legs0, { 62,107, 98, 48}, {174,103}}, //7 legs 8
  {BFLegs_ArcMain_Legs0, {  0,162, 98, 54}, {177,106}}, //8 legs 9
  {BFLegs_ArcMain_Legs0, { 98,162, 93, 54}, {179,103}}, //9 legs 10
};

static const Animation char_bflegs_anim[CharAnim_Max] = {
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{2, (const u8[]){ 5, 6, 7, 8, 9, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//BFLegs character functions
void Char_BFLegs_SetFrame(void *user, u8 frame)
{
	Char_BFLegs *this = (Char_BFLegs*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bflegs_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFLegs_Tick(Character *character)
{
	Char_BFLegs *this = (Char_BFLegs*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFLegs_SetFrame);
	Character_Draw(character, &this->tex, &char_bflegs_frame[this->frame]);
}

void Char_BFLegs_SetAnim(Character *character, u8 anim)
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

void Char_BFLegs_Free(Character *character)
{
	Char_BFLegs *this = (Char_BFLegs*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFLegs_New(fixed_t x, fixed_t y)
{
	//Allocate bflegs object
	Char_BFLegs *this = Mem_Alloc(sizeof(Char_BFLegs));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFLegs_New] Failed to allocate bflegs object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFLegs_Tick;
	this->character.set_anim = Char_BFLegs_SetAnim;
	this->character.free = Char_BFLegs_Free;
	
	Animatable_Init(&this->character.animatable, char_bflegs_anim);
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
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\LEGS.ARC;1");
	
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
