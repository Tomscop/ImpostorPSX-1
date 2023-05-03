/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfairship.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFAirship character structure
enum
{
  GFAirship_ArcMain_GF0,
  GFAirship_ArcMain_GF1,
	
	GFAirship_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFAirship_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFAirship;

//GFAirship character definitions
static const CharFrame char_gfairship_frame[] = {
  {GFAirship_ArcMain_GF0, {  0,  0,155, 57}, {160,156}}, //0 gf 1
  {GFAirship_ArcMain_GF0, {  0, 57,154, 57}, {159,156}}, //1 gf 2
  {GFAirship_ArcMain_GF0, {  0,114,153, 57}, {159,156}}, //2 gf 3
  {GFAirship_ArcMain_GF0, {  0,171,153, 57}, {159,157}}, //3 gf 4
  {GFAirship_ArcMain_GF1, {  0,  0,153, 58}, {159,157}}, //4 gf 5
  {GFAirship_ArcMain_GF1, {  0, 58,153, 58}, {159,157}}, //5 gf 6
};

static const Animation char_gfairship_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 0, 1, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 0, 0, 1, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFAirship character functions
void Char_GFAirship_SetFrame(void *user, u8 frame)
{
	Char_GFAirship *this = (Char_GFAirship*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfairship_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFAirship_Tick(Character *character)
{
	Char_GFAirship *this = (Char_GFAirship*)character;
	
	//Initialize Pico test
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{			
		//Perform dance
	    if ((stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0) && (character->animatable.anim  != CharAnim_Special1))
		{
			//Switch animation
			if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
				character->set_anim(character, CharAnim_RightAlt);
			else
				character->set_anim(character, CharAnim_LeftAlt);
		}
	}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFAirship_SetFrame);
	Character_Draw(character, &this->tex, &char_gfairship_frame[this->frame]);
}

void Char_GFAirship_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFAirship_Free(Character *character)
{
	Char_GFAirship *this = (Char_GFAirship*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFAirship_New(fixed_t x, fixed_t y)
{
	//Allocate gfairship object
	Char_GFAirship *this = Mem_Alloc(sizeof(Char_GFAirship));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFAirship_New] Failed to allocate gfairship object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFAirship_Tick;
	this->character.set_anim = Char_GFAirship_SetAnim;
	this->character.free = Char_GFAirship_Free;
	
	Animatable_Init(&this->character.animatable, char_gfairship_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;

	//health bar color
	this->character.health_bar = 0xFFA5004A;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\GF\\GFAIRSHP.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
