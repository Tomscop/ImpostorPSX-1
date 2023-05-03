/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfmira.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFMira character structure
enum
{
  GFMira_ArcMain_GF0,
  GFMira_ArcMain_GF1,
  GFMira_ArcMain_GF2,
  GFMira_ArcMain_GF3,
  GFMira_ArcMain_GF4,
  GFMira_ArcMain_GF5,
  GFMira_ArcMain_GF6,
  GFMira_ArcMain_GF7,
  GFMira_ArcMain_GF8,
  GFMira_ArcMain_GF9,
  GFMira_ArcMain_GF10,
  GFMira_ArcMain_GF11,
  GFMira_ArcMain_GF12,
	
	GFMira_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFMira_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFMira;

//GFMira character definitions
static const CharFrame char_gfmira_frame[] = {
  {GFMira_ArcMain_GF0, {  0,  0,176,159}, {159,155}}, //0 gf 1
  {GFMira_ArcMain_GF1, {  0,  0,176,159}, {159,154}}, //1 gf 2
  {GFMira_ArcMain_GF2, {  0,  0,176,160}, {159,155}}, //2 gf 3
  {GFMira_ArcMain_GF3, {  0,  0,176,160}, {159,156}}, //3 gf 4
  {GFMira_ArcMain_GF4, {  0,  0,176,163}, {159,158}}, //4 gf 5
  {GFMira_ArcMain_GF5, {  0,  0,175,163}, {159,158}}, //5 gf 6
  {GFMira_ArcMain_GF6, {  0,  0,176,163}, {159,158}}, //6 gf 7
  {GFMira_ArcMain_GF7, {  0,  0,177,160}, {160,155}}, //7 gf 8
  {GFMira_ArcMain_GF8, {  0,  0,175,160}, {159,155}}, //8 gf 9
  {GFMira_ArcMain_GF9, {  0,  0,176,161}, {160,156}}, //9 gf 10
  {GFMira_ArcMain_GF10, {  0,  0,176,162}, {160,157}}, //10 gf 11
  {GFMira_ArcMain_GF11, {  0,  0,175,162}, {159,157}}, //11 gf 12
  {GFMira_ArcMain_GF12, {  0,  0,176,162}, {159,157}}, //12 gf 13
};

static const Animation char_gfmira_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{2, (const u8[]){ 7, 8, 9, 10, 11, 12, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFMira character functions
void Char_GFMira_SetFrame(void *user, u8 frame)
{
	Char_GFMira *this = (Char_GFMira*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfmira_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFMira_Tick(Character *character)
{
	Char_GFMira *this = (Char_GFMira*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_GFMira_SetFrame);
	Character_Draw(character, &this->tex, &char_gfmira_frame[this->frame]);
}

void Char_GFMira_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFMira_Free(Character *character)
{
	Char_GFMira *this = (Char_GFMira*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFMira_New(fixed_t x, fixed_t y)
{
	//Allocate gfmira object
	Char_GFMira *this = Mem_Alloc(sizeof(Char_GFMira));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFMira_New] Failed to allocate gfmira object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFMira_Tick;
	this->character.set_anim = Char_GFMira_SetAnim;
	this->character.free = Char_GFMira_Free;
	
	Animatable_Init(&this->character.animatable, char_gfmira_anim);
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
	this->arc_main = IO_Read("\\GF\\GFMIRA.ARC;1");
		
	const char **pathp = (const char *[]){
  "gf0.tim",
  "gf1.tim",
  "gf2.tim",
  "gf3.tim",
  "gf4.tim",
  "gf5.tim",
  "gf6.tim",
  "gf7.tim",
  "gf8.tim",
  "gf9.tim",
  "gf10.tim",
  "gf11.tim",
  "gf12.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
