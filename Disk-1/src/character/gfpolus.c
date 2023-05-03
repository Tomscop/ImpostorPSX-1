/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfpolus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFPolus character structure
enum
{
  GFPolus_ArcMain_GF0,
  GFPolus_ArcMain_GF2,
  GFPolus_ArcMain_GF4,
  GFPolus_ArcMain_GF6,
  GFPolus_ArcMain_GF8,
  GFPolus_ArcMain_GF10,
  GFPolus_ArcMain_GF12,
  GFPolus_ArcMain_GF14,
  GFPolus_ArcMain_GF16,
  GFPolus_ArcMain_GF18,
	
	GFPolus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFPolus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFPolus;

//GFPolus character definitions
static const CharFrame char_gfpolus_frame[] = {
  {GFPolus_ArcMain_GF0, {  0,  0,175,163}, {159,145}}, //0 gf 1
  {GFPolus_ArcMain_GF0, {  0,  0,175,163}, {159,145}}, //0 gf 1
  {GFPolus_ArcMain_GF2, {  0,  0,177,162}, {160,145}}, //2 gf 3
  {GFPolus_ArcMain_GF2, {  0,  0,177,162}, {160,145}}, //2 gf 3
  {GFPolus_ArcMain_GF4, {  0,  0,176,163}, {159,145}}, //4 gf 5
  {GFPolus_ArcMain_GF4, {  0,  0,176,163}, {159,145}}, //4 gf 5
  {GFPolus_ArcMain_GF6, {  0,  0,175,164}, {159,146}}, //6 gf 7
  {GFPolus_ArcMain_GF6, {  0,  0,175,164}, {159,146}}, //6 gf 7
  {GFPolus_ArcMain_GF8, {  0,  0,175,168}, {159,150}}, //8 gf 9
  {GFPolus_ArcMain_GF8, {  0,  0,175,168}, {159,150}}, //8 gf 9
  
  {GFPolus_ArcMain_GF10, {  0,  0,175,166}, {159,149}}, //10 gf 11
  {GFPolus_ArcMain_GF10, {  0,  0,175,166}, {159,149}}, //10 gf 11
  {GFPolus_ArcMain_GF12, {  0,  0,177,166}, {160,149}}, //12 gf 13
  {GFPolus_ArcMain_GF12, {  0,  0,177,166}, {160,149}}, //12 gf 13
  {GFPolus_ArcMain_GF14, {  0,  0,176,167}, {159,149}}, //14 gf 15
  {GFPolus_ArcMain_GF14, {  0,  0,176,167}, {159,149}}, //14 gf 15
  {GFPolus_ArcMain_GF16, {  0,  0,175,166}, {159,149}}, //16 gf 17
  {GFPolus_ArcMain_GF16, {  0,  0,175,166}, {159,149}}, //16 gf 17
  {GFPolus_ArcMain_GF18, {  0,  0,175,167}, {159,150}}, //18 gf 19
  {GFPolus_ArcMain_GF18, {  0,  0,175,167}, {159,150}}, //18 gf 19
};

static const Animation char_gfpolus_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 9, 9, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 10, 11, 12, 13, 14, 15, 16, 17, 17, 17, 18, 18, 18, 19, 19, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFPolus character functions
void Char_GFPolus_SetFrame(void *user, u8 frame)
{
	Char_GFPolus *this = (Char_GFPolus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfpolus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFPolus_Tick(Character *character)
{
	Char_GFPolus *this = (Char_GFPolus*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_GFPolus_SetFrame);
	Character_DrawCol(character, &this->tex, &char_gfpolus_frame[this->frame], 196, 137, 153);
}

void Char_GFPolus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFPolus_Free(Character *character)
{
	Char_GFPolus *this = (Char_GFPolus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFPolus_New(fixed_t x, fixed_t y)
{
	//Allocate gfpolus object
	Char_GFPolus *this = Mem_Alloc(sizeof(Char_GFPolus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFPolus_New] Failed to allocate gfpolus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFPolus_Tick;
	this->character.set_anim = Char_GFPolus_SetAnim;
	this->character.free = Char_GFPolus_Free;
	
	Animatable_Init(&this->character.animatable, char_gfpolus_anim);
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
	this->arc_main = IO_Read("\\GF\\GFPOLUS.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf2.tim",
  "gf4.tim",
  "gf6.tim",
  "gf8.tim",
  "gf10.tim",
  "gf12.tim",
  "gf14.tim",
  "gf16.tim",
  "gf18.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
