/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfghost.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFGhost character structure
enum
{
  GFGhost_ArcMain_GFGhost0,
  GFGhost_ArcMain_GFGhost1,
  GFGhost_ArcMain_GFGhost2,
  GFGhost_ArcMain_GFGhost3,
  GFGhost_ArcMain_Appear0,
	
	GFGhost_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFGhost_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_GFGhost;

//GFGhost character definitions
static const CharFrame char_gfghost_frame[] = {
  {GFGhost_ArcMain_GFGhost0, {  0,  0, 73, 92}, {154,151}}, //0 gfghost 1
  {GFGhost_ArcMain_GFGhost0, { 73,  0, 73, 92}, {154,150}}, //1 gfghost 2
  {GFGhost_ArcMain_GFGhost0, {146,  0, 73, 91}, {154,151}}, //2 gfghost 3
  {GFGhost_ArcMain_GFGhost0, {  0, 92, 73, 92}, {154,152}}, //3 gfghost 4
  {GFGhost_ArcMain_GFGhost0, { 73, 92, 73, 92}, {153,153}}, //4 gfghost 5
  {GFGhost_ArcMain_GFGhost0, {146, 92, 78, 93}, {155,156}}, //5 gfghost 6
  {GFGhost_ArcMain_GFGhost1, {  0,  0, 78, 92}, {155,156}}, //6 gfghost 7
  {GFGhost_ArcMain_GFGhost1, { 78,  0, 78, 92}, {155,156}}, //7 gfghost 8
  {GFGhost_ArcMain_GFGhost1, {156,  0, 81, 93}, {158,157}}, //8 gfghost 9
  {GFGhost_ArcMain_GFGhost1, {  0, 93, 81, 93}, {158,157}}, //9 gfghost 10
  {GFGhost_ArcMain_GFGhost1, { 81, 93, 81, 92}, {158,157}}, //10 gfghost 11
  {GFGhost_ArcMain_GFGhost1, {162, 93, 82, 93}, {159,157}}, //11 gfghost 12
  {GFGhost_ArcMain_GFGhost2, {  0,  0, 81, 92}, {157,154}}, //12 gfghost 13
  {GFGhost_ArcMain_GFGhost2, { 81,  0, 80, 92}, {156,154}}, //13 gfghost 14
  {GFGhost_ArcMain_GFGhost2, {161,  0, 81, 91}, {157,153}}, //14 gfghost 15
  {GFGhost_ArcMain_GFGhost2, {  0, 92, 80, 92}, {156,152}}, //15 gfghost 16
  {GFGhost_ArcMain_GFGhost2, { 80, 92, 80, 91}, {156,152}}, //16 gfghost 17
  {GFGhost_ArcMain_GFGhost2, {160, 92, 78, 92}, {154,152}}, //17 gfghost 18
  {GFGhost_ArcMain_GFGhost3, {  0,  0, 73, 92}, {148,153}}, //18 gfghost 19
  {GFGhost_ArcMain_GFGhost3, { 73,  0, 73, 92}, {148,153}}, //19 gfghost 20
  {GFGhost_ArcMain_GFGhost3, {146,  0, 73, 92}, {148,152}}, //20 gfghost 21
  {GFGhost_ArcMain_GFGhost3, {  0, 92, 72, 92}, {148,152}}, //21 gfghost 22

  {GFGhost_ArcMain_Appear0, {  0,  0, 75, 92}, {156,154}}, //22 appear 1
  {GFGhost_ArcMain_Appear0, { 75,  0, 75, 93}, {156,154}}, //23 appear 2
  {GFGhost_ArcMain_Appear0, {150,  0, 75, 93}, {156,154}}, //24 appear 3
  {GFGhost_ArcMain_Appear0, {  0, 93, 75, 93}, {156,154}}, //25 appear 4
  {GFGhost_ArcMain_Appear0, { 75, 93, 75, 93}, {156,154}}, //26 appear 5
  {GFGhost_ArcMain_Appear0, {150, 93, 75, 93}, {156,154}}, //27 appear 6
  {GFGhost_ArcMain_Appear0, {  0,  0,  1,  1}, {156,154}}, //28 appear 7
};

static const Animation char_gfghost_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Left
	{1, (const u8[]){ 0, 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10, 11, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                                 //CharAnim_Right
	{1, (const u8[]){ 12, 13, 14, 14, 15, 16, 17, 18, 18, 18, 19, 20, 20, 21, 21, ASCR_BACK, 1}}, //CharAnim_RightAlt
	
	{1, (const u8[]){ 28, 28, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, ASCR_CHGANI, CharAnim_LeftAlt}}, //CharAnim_Special1
};

//GFGhost character functions
void Char_GFGhost_SetFrame(void *user, u8 frame)
{
	Char_GFGhost *this = (Char_GFGhost*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfghost_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFGhost_Tick(Character *character)
{
	Char_GFGhost *this = (Char_GFGhost*)character;
	
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
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Sabotage: //Hi
				if (stage.song_step == 816)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}

	//Get parallax
	fixed_t parallax;
	parallax = FIXED_UNIT;
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFGhost_SetFrame);
	if (stage.stage_id != StageId_Sabotage)
		Character_DrawParallax(character, &this->tex, &char_gfghost_frame[this->frame], parallax);
	else if ((stage.stage_id == StageId_Sabotage) && (stage.song_step >= 816))
		Character_DrawParallax(character, &this->tex, &char_gfghost_frame[this->frame], parallax);
}

void Char_GFGhost_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFGhost_Free(Character *character)
{
	Char_GFGhost *this = (Char_GFGhost*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GFGhost_New(fixed_t x, fixed_t y)
{
	//Allocate gfghost object
	Char_GFGhost *this = Mem_Alloc(sizeof(Char_GFGhost));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFGhost_New] Failed to allocate gfghost object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFGhost_Tick;
	this->character.set_anim = Char_GFGhost_SetAnim;
	this->character.free = Char_GFGhost_Free;
	
	Animatable_Init(&this->character.animatable, char_gfghost_anim);
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
	this->arc_main = IO_Read("\\CHAR2\\GFGHOST.ARC;1");
		
	const char **pathp = (const char *[]){
  "gfghost0.tim",
  "gfghost1.tim",
  "gfghost2.tim",
  "gfghost3.tim",
  "appear0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
