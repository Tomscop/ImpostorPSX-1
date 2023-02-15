/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfreactor.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Reactor player types
enum
{
  BFReactor_ArcMain_Idle0,
  BFReactor_ArcMain_Idle1,
  BFReactor_ArcMain_Left0,
  BFReactor_ArcMain_Down0,
  BFReactor_ArcMain_Up0,
  BFReactor_ArcMain_Right0,
  BFReactor_ArcMain_LeftM0,
  BFReactor_ArcMain_DownM0,
  BFReactor_ArcMain_UpM0,
  BFReactor_ArcMain_RightM0,
	
	BFReactor_ArcMain_Max,
};

#define BFReactor_Arc_Max BFReactor_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFReactor_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFReactor;

//Boyfriend Reactor player definitions
static const CharFrame char_bfreactor_frame[] = {
  {BFReactor_ArcMain_Idle0, {  0,  0,103, 99}, {159,155}}, //0 idle 1
  {BFReactor_ArcMain_Idle0, {103,  0,103, 99}, {159,155}}, //1 idle 2
  {BFReactor_ArcMain_Idle0, {  0, 99,102,100}, {158,156}}, //2 idle 3
  {BFReactor_ArcMain_Idle0, {102, 99,104,104}, {159,160}}, //3 idle 4
  {BFReactor_ArcMain_Idle1, {  0,  0,103,104}, {158,160}}, //4 idle 5

  {BFReactor_ArcMain_Left0, {  0,  0, 96,102}, {161,159}}, //5 left 1
  {BFReactor_ArcMain_Left0, { 96,  0, 94,102}, {157,158}}, //6 left 2

  {BFReactor_ArcMain_Down0, {  0,  0, 94, 90}, {155,147}}, //7 down 1
  {BFReactor_ArcMain_Down0, { 94,  0, 94, 91}, {154,148}}, //8 down 2

  {BFReactor_ArcMain_Up0, {  0,  0, 93,112}, {147,165+2}}, //9 up 1
  {BFReactor_ArcMain_Up0, { 93,  0, 95,111}, {148,164+2}}, //10 up 2

  {BFReactor_ArcMain_Right0, {  0,  0,103,103}, {147,158}}, //11 right 1
  {BFReactor_ArcMain_Right0, {103,  0,103,103}, {147,159}}, //12 right 2

  {BFReactor_ArcMain_LeftM0, {  0,  0, 93,106}, {157,162}}, //13 leftm 1
  {BFReactor_ArcMain_LeftM0, { 93,  0, 93,104}, {158,160}}, //14 leftm 2

  {BFReactor_ArcMain_DownM0, {  0,  0, 95, 99}, {157,156}}, //15 downm 1
  {BFReactor_ArcMain_DownM0, { 95,  0, 96, 98}, {157,155}}, //16 downm 2

  {BFReactor_ArcMain_UpM0, {  0,  0, 91,106}, {151,161+2}}, //17 upm 1
  {BFReactor_ArcMain_UpM0, { 91,  0, 90,107}, {151,161+2}}, //18 upm 2

  {BFReactor_ArcMain_RightM0, {  0,  0,100,106}, {149,162}}, //19 rightm 1
  {BFReactor_ArcMain_RightM0, {100,  0,101,106}, {149,162}}, //20 rightm 2
};

static const Animation char_bfreactor_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 5, 13, 13, 14, 14, 14, 14, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 7, 15, 15, 16, 16, 16, 16, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 9, 17, 17, 18, 18, 18, 18, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 11, 19, 19, 20, 20, 20, 20, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Reactor player functions
void Char_BFReactor_SetFrame(void *user, u8 frame)
{
	Char_BFReactor *this = (Char_BFReactor*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfreactor_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFReactor_Tick(Character *character)
{
	Char_BFReactor *this = (Char_BFReactor*)character;
	
	if (stage.stage_id == StageId_Reactor)
	{
		if (stage.song_step == 258)
		{
			this->character.focus_x = FIXED_DEC(-158,1);
			this->character.focus_y = FIXED_DEC(-150,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 512)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-163,1);
			this->character.focus_zoom = FIXED_DEC(957,1024);
		}
		if (stage.song_step == 770)
		{
			this->character.focus_x = FIXED_DEC(-158,1);
			this->character.focus_y = FIXED_DEC(-150,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 896)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-163,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 1026)
		{
			this->character.focus_x = FIXED_DEC(-158,1);
			this->character.focus_y = FIXED_DEC(-150,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 1280)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-163,1);
			this->character.focus_zoom = FIXED_DEC(957,1024);
		}
		if (stage.song_step == 1538)
		{
			this->character.focus_x = FIXED_DEC(-158,1);
			this->character.focus_y = FIXED_DEC(-150,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 1916)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-136,1);
			this->character.focus_zoom = FIXED_DEC(619,512);
		}
		if (stage.song_step == 2176)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-163,1);
			this->character.focus_zoom = FIXED_DEC(69,64);
		}
		if (stage.song_step == 2432)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-136,1);
			this->character.focus_zoom = FIXED_DEC(619,512);
		}
		if (stage.song_step == 2688)
		{
			this->character.focus_x = FIXED_DEC(-212,1);
			this->character.focus_y = FIXED_DEC(-163,1);
			this->character.focus_zoom = FIXED_DEC(957,1024);
		}
	}
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFReactor_SetFrame);
	Character_Draw(character, &this->tex, &char_bfreactor_frame[this->frame]);
}

void Char_BFReactor_SetAnim(Character *character, u8 anim)
{
	Char_BFReactor *this = (Char_BFReactor*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFReactor_Free(Character *character)
{
	Char_BFReactor *this = (Char_BFReactor*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFReactor_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFReactor *this = Mem_Alloc(sizeof(Char_BFReactor));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFReactor_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFReactor_Tick;
	this->character.set_anim = Char_BFReactor_SetAnim;
	this->character.free = Char_BFReactor_Free;
	
	Animatable_Init(&this->character.animatable, char_bfreactor_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-212,1);
	this->character.focus_y = FIXED_DEC(-163,1);
	this->character.focus_zoom = FIXED_DEC(957,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFREAC.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
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
