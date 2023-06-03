/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfmira.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Mira player types
enum
{
  BFMira_ArcMain_Idle0,
  BFMira_ArcMain_Idle1,
  BFMira_ArcMain_Left0,
  BFMira_ArcMain_Down0,
  BFMira_ArcMain_Up0,
  BFMira_ArcMain_Right0,
  BFMira_ArcMain_LeftM0,
  BFMira_ArcMain_DownM0,
  BFMira_ArcMain_UpM0,
  BFMira_ArcMain_RightM0,
	
	BFMira_ArcMain_Max,
};

#define BFMira_Arc_Max BFMira_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFMira_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFMira;

//Boyfriend Mira player definitions
static const CharFrame char_bfmira_frame[] = {
  {BFMira_ArcMain_Idle0, {  0,  0,103,100}, {159,155}}, //0 idle 1
  {BFMira_ArcMain_Idle0, {103,  0,103,101}, {159,155}}, //1 idle 2
  {BFMira_ArcMain_Idle0, {  0,101,102,102}, {158,156}}, //2 idle 3
  {BFMira_ArcMain_Idle0, {102,101,103,106}, {159,160}}, //3 idle 4
  {BFMira_ArcMain_Idle1, {  0,  0,103,106}, {158,160}}, //4 idle 5

  {BFMira_ArcMain_Left0, {  0,  0, 95,104}, {162,159}}, //5 left 1
  {BFMira_ArcMain_Left0, { 95,  0, 94,104}, {159,158}}, //6 left 2

  {BFMira_ArcMain_Down0, {  0,  0, 96, 93}, {155,147}}, //7 down 1
  {BFMira_ArcMain_Down0, { 96,  0, 94, 94}, {154,148}}, //8 down 2

  {BFMira_ArcMain_Up0, {  0,  0, 93,111}, {148,166}}, //9 up 1
  {BFMira_ArcMain_Up0, { 93,  0, 95,110}, {150,165}}, //10 up 2

  {BFMira_ArcMain_Right0, {  0,  0,103,104}, {146,158}}, //11 right 1
  {BFMira_ArcMain_Right0, {103,  0,103,104}, {146,159}}, //12 right 2

  {BFMira_ArcMain_LeftM0, {  0,  0, 93,107}, {158,157}}, //13 leftm 1
  {BFMira_ArcMain_LeftM0, { 93,  0, 94,105}, {159,155}}, //14 leftm 2

  {BFMira_ArcMain_DownM0, {  0,  0, 95, 98}, {155,146}}, //15 downm 1
  {BFMira_ArcMain_DownM0, { 95,  0, 95, 98}, {155,146}}, //16 downm 2

  {BFMira_ArcMain_UpM0, {  0,  0, 91,108}, {150,163}}, //17 upm 1
  {BFMira_ArcMain_UpM0, { 91,  0, 91,109}, {149,163}}, //18 upm 2

  {BFMira_ArcMain_RightM0, {  0,  0, 99,109}, {146,159}}, //19 rightm 1
  {BFMira_ArcMain_RightM0, { 99,  0,101,109}, {146,159}}, //20 rightm 2
};

static const Animation char_bfmira_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 15, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 17, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 19, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Mira player functions
void Char_BFMira_SetFrame(void *user, u8 frame)
{
	Char_BFMira *this = (Char_BFMira*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfmira_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFMira_Tick(Character *character)
{
	Char_BFMira *this = (Char_BFMira*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Heartbeat)
	{
		if (stage.song_step == 32)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 272)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 279)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 283)
			this->character.focus_zoom = FIXED_DEC(801+24,1024);
		if (stage.song_step == 286)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 287)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 288)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 412)
			this->character.focus_zoom = FIXED_DEC(747+24,1024);
		if (stage.song_step == 416)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 528)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 535)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 539)
			this->character.focus_zoom = FIXED_DEC(801+24,1024);
		if (stage.song_step == 542)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 604)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 608)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 612)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 616)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 620)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 644)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 647)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 652)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 665)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 672)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 736)
			this->character.focus_zoom = FIXED_DEC(679,1024);
		if (stage.song_step == 784)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 788)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 791)
			this->character.focus_zoom = FIXED_DEC(767+24,1024);
		if (stage.song_step == 795)
			this->character.focus_zoom = FIXED_DEC(801+24,1024);
		if (stage.song_step == 798)
			this->character.focus_zoom = FIXED_DEC(733+24,1024);
		if (stage.song_step == 800)
			this->character.focus_zoom = FIXED_DEC(679,1024);
	}
	if (stage.stage_id == StageId_Pinkwave)
	{
		if (stage.song_step == 8)
			this->character.focus_zoom = FIXED_DEC(679,1024);
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
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFMira_SetFrame);
	Character_Draw(character, &this->tex, &char_bfmira_frame[this->frame]);
}

void Char_BFMira_SetAnim(Character *character, u8 anim)
{
	Char_BFMira *this = (Char_BFMira*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFMira_Free(Character *character)
{
	Char_BFMira *this = (Char_BFMira*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFMira_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFMira *this = Mem_Alloc(sizeof(Char_BFMira));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFMira_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFMira_Tick;
	this->character.set_anim = Char_BFMira_SetAnim;
	this->character.free = Char_BFMira_Free;
	
	Animatable_Init(&this->character.animatable, char_bfmira_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-226,1);
	this->character.focus_y = FIXED_DEC(-192,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFMIRA.ARC;1");
		
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
