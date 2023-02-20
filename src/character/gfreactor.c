/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfreactor.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFReactor character structure
enum
{
  GFReactor_ArcMain_GFRL0,
  GFReactor_ArcMain_GFRL1,
  GFReactor_ArcMain_GFRL2,
  GFReactor_ArcMain_GFRR0,
  GFReactor_ArcMain_GFRR1,
  GFReactor_ArcMain_GFRR2,
	
	GFReactor_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFReactor_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFReactor;

//GFReactor character definitions
static const CharFrame char_gfreactor_frame[] = {
  {GFReactor_ArcMain_GFRL0, {  0,  0, 96,104}, {159,156}}, //0 gfrl 1
  {GFReactor_ArcMain_GFRL0, { 96,  0, 96,104}, {160,156}}, //1 gfrl 2
  {GFReactor_ArcMain_GFRL0, {  0,104, 96,104}, {160,156}}, //2 gfrl 3
  {GFReactor_ArcMain_GFRL0, { 96,104, 96,104}, {159,156}}, //3 gfrl 4
  {GFReactor_ArcMain_GFRL1, {  0,  0, 96,104}, {159,156}}, //4 gfrl 5
  {GFReactor_ArcMain_GFRL1, { 96,  0, 96,104}, {159,156}}, //5 gfrl 6
  {GFReactor_ArcMain_GFRL1, {  0,104, 96,105}, {159,157}}, //6 gfrl 7
  {GFReactor_ArcMain_GFRL1, { 96,104, 97,106}, {159,158}}, //7 gfrl 8
  {GFReactor_ArcMain_GFRL2, {  0,  0, 98,106}, {159,158}}, //8 gfrl 9
  {GFReactor_ArcMain_GFRL2, { 98,  0, 98,106}, {159,158}}, //9 gfrl 10

  {GFReactor_ArcMain_GFRR0, {  0,  0, 98,104}, {159,156}}, //10 gfrr 1
  {GFReactor_ArcMain_GFRR0, { 98,  0, 99,103}, {160,155}}, //11 gfrr 2
  {GFReactor_ArcMain_GFRR0, {  0,104, 99,103}, {160,155}}, //12 gfrr 3
  {GFReactor_ArcMain_GFRR0, { 99,104, 98,103}, {159,155}}, //13 gfrr 4
  {GFReactor_ArcMain_GFRR1, {  0,  0, 98,104}, {159,156}}, //14 gfrr 5
  {GFReactor_ArcMain_GFRR1, { 98,  0, 98,104}, {159,156}}, //15 gfrr 6
  {GFReactor_ArcMain_GFRR1, {  0,104, 98,104}, {159,155}}, //16 gfrr 7
  {GFReactor_ArcMain_GFRR1, { 98,104, 98,106}, {159,157}}, //17 gfrr 8
  {GFReactor_ArcMain_GFRR2, {  0,  0, 98,106}, {159,158}}, //18 gfrr 9
};

static const Animation char_gfreactor_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 9, 9, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 10, 11, 12, 13, 14, 15, 16, 17, 17, 17, 18, 18, 18, 9, 9, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFReactor character functions
void Char_GFReactor_SetFrame(void *user, u8 frame)
{
	Char_GFReactor *this = (Char_GFReactor*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfreactor_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFReactor_Tick(Character *character)
{
	Char_GFReactor *this = (Char_GFReactor*)character;
	
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

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFReactor_SetFrame);
	Character_DrawParallax(character, &this->tex, &char_gfreactor_frame[this->frame], parallax);
}

void Char_GFReactor_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFReactor_Free(Character *character)
{
	Char_GFReactor *this = (Char_GFReactor*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFReactor_New(fixed_t x, fixed_t y)
{
	//Allocate gfreactor object
	Char_GFReactor *this = Mem_Alloc(sizeof(Char_GFReactor));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFReactor_New] Failed to allocate gfreactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFReactor_Tick;
	this->character.set_anim = Char_GFReactor_SetAnim;
	this->character.free = Char_GFReactor_Free;
	
	Animatable_Init(&this->character.animatable, char_gfreactor_anim);
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
	this->arc_main = IO_Read("\\GF\\GFREAC.ARC;1");
		
	const char **pathp = (const char *[]){
  "gfrl0.tim",
  "gfrl1.tim",
  "gfrl2.tim",
  "gfrr0.tim",
  "gfrr1.tim",
  "gfrr2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
