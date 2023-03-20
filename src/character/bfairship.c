/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfairship.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Airship player types
enum
{
  BFAirship_ArcMain_Idle0,
  BFAirship_ArcMain_Idle1,
  BFAirship_ArcMain_Idle2,
  BFAirship_ArcMain_Left0,
  BFAirship_ArcMain_Down0,
  BFAirship_ArcMain_Up0,
  BFAirship_ArcMain_Right0,
  BFAirship_ArcMain_LeftM0,
  BFAirship_ArcMain_DownM0,
  BFAirship_ArcMain_UpM0,
  BFAirship_ArcMain_RightM0,
	
	BFAirship_ArcMain_Max,
};

#define BFAirship_Arc_Max BFAirship_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFAirship_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFAirship;

//Boyfriend Airship player definitions
static const CharFrame char_bfairship_frame[] = {
  {BFAirship_ArcMain_Idle0, {  0,  0, 99,104}, {160,158}}, //0 idle 1
  {BFAirship_ArcMain_Idle0, { 99,  0, 99,104}, {160,158}}, //1 idle 2
  {BFAirship_ArcMain_Idle0, {  0,104, 99,104}, {160,158}}, //2 idle 3
  {BFAirship_ArcMain_Idle0, { 99,104,100,104}, {160,158}}, //3 idle 4
  {BFAirship_ArcMain_Idle1, {  0,  0, 99,105}, {159,159}}, //4 idle 5
  {BFAirship_ArcMain_Idle1, { 99,  0,100,105}, {159,159}}, //5 idle 6
  {BFAirship_ArcMain_Idle1, {  0,105,100,106}, {159,160}}, //6 idle 7
  {BFAirship_ArcMain_Idle1, {100,105,101,106}, {159,160}}, //7 idle 8
  {BFAirship_ArcMain_Idle2, {  0,  0,100,106}, {159,160}}, //8 idle 9
  {BFAirship_ArcMain_Idle2, {100,  0,100,106}, {159,160}}, //9 idle 10
  {BFAirship_ArcMain_Idle2, {  0,106,100,106}, {158,160}}, //10 idle 11
  {BFAirship_ArcMain_Idle2, {100,106,100,106}, {158,160}}, //11 idle 12

  {BFAirship_ArcMain_Left0, {  0,  0, 96,104}, {163,158}}, //12 left 1
  {BFAirship_ArcMain_Left0, { 96,  0, 94,104}, {160,158}}, //13 left 2

  {BFAirship_ArcMain_Down0, {  0,  0, 95, 92}, {157,145}}, //14 down 1
  {BFAirship_ArcMain_Down0, { 95,  0, 95, 93}, {157,146}}, //15 down 2

  {BFAirship_ArcMain_Up0, {  0,  0, 93,111}, {149,166}}, //16 up 1
  {BFAirship_ArcMain_Up0, { 93,  0, 94,110}, {150,165}}, //17 up 2
  {BFAirship_ArcMain_Up0, {  0,111, 95,110}, {151,165}}, //18 up 3
  {BFAirship_ArcMain_Up0, { 95,111, 95,110}, {151,165}}, //19 up 4

  {BFAirship_ArcMain_Right0, {  0,  0,102,104}, {148,158}}, //20 right 1
  {BFAirship_ArcMain_Right0, {102,  0,103,104}, {149,159}}, //21 right 2
  {BFAirship_ArcMain_Right0, {  0,104,103,104}, {149,159}}, //22 right 3
  {BFAirship_ArcMain_Right0, {103,104,102,104}, {148,159}}, //23 right 4

  {BFAirship_ArcMain_LeftM0, {  0,  0, 93,107}, {158,161}}, //24 leftm 1
  {BFAirship_ArcMain_LeftM0, { 93,  0, 94,109}, {160,163}}, //25 leftm 2

  {BFAirship_ArcMain_DownM0, {  0,  0, 95, 98}, {157,152}}, //26 downm 1
  {BFAirship_ArcMain_DownM0, { 95,  0, 96, 98}, {157,152}}, //27 downm 2

  {BFAirship_ArcMain_UpM0, {  0,  0, 91,108}, {152,160}}, //28 upm 1
  {BFAirship_ArcMain_UpM0, { 91,  0, 90,108}, {151,160}}, //29 upm 2

  {BFAirship_ArcMain_RightM0, {  0,  0,100,107}, {152,160}}, //30 rightm 1
  {BFAirship_ArcMain_RightM0, {100,  0,101,107}, {152,160}}, //31 rightm 2
};

static const Animation char_bfairship_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 10, 11, ASCR_CHGANI, CharAnim_Special1}}, //CharAnim_Idle
	{2, (const u8[]){ 12, 13, 13, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 14, 15, 15, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 16, 17, 18, 19, 19, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {1, (const u8[]){ 10, 11, ASCR_CHGANI, CharAnim_Special1}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 12, 24, 24, 25, 25, 25, 25, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 14, 26, 26, 27, 27, 27, 27, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 16, 28, 28, 29, 29, 29, 29, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 20, 30, 30, 31, 31, 31, 31, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Airship player functions
void Char_BFAirship_SetFrame(void *user, u8 frame)
{
	Char_BFAirship *this = (Char_BFAirship*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfairship_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFAirship_Tick(Character *character)
{
	Char_BFAirship *this = (Char_BFAirship*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if ((character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFAirship_SetFrame);
	Character_Draw(character, &this->tex, &char_bfairship_frame[this->frame]);
}

void Char_BFAirship_SetAnim(Character *character, u8 anim)
{
	Char_BFAirship *this = (Char_BFAirship*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFAirship_Free(Character *character)
{
	Char_BFAirship *this = (Char_BFAirship*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFAirship_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFAirship *this = Mem_Alloc(sizeof(Char_BFAirship));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFAirship_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFAirship_Tick;
	this->character.set_anim = Char_BFAirship_SetAnim;
	this->character.free = Char_BFAirship_Free;
	
	Animatable_Init(&this->character.animatable, char_bfairship_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-170,1);
	this->character.focus_y = FIXED_DEC(-147,1);
	this->character.focus_zoom = FIXED_DEC(1086,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFAIRSHP.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "downm0.tim",
  "upm0.tim",
  "rightm0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
