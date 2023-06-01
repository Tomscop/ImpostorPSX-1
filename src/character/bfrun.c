/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfrun.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Run player types
enum
{
  BFRun_ArcMain_Idle0,
  BFRun_ArcMain_Idle1,
  BFRun_ArcMain_Left0,
  BFRun_ArcMain_Left1,
  BFRun_ArcMain_Down0,
  BFRun_ArcMain_Up0,
  BFRun_ArcMain_Up1,
  BFRun_ArcMain_Right0,
	
	BFRun_ArcMain_Max,
};

#define BFRun_Arc_Max BFRun_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFRun_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFRun;

//Boyfriend Run player definitions
static const CharFrame char_bfrun_frame[] = {
  {BFRun_ArcMain_Idle0, {  0,  0, 83, 79}, {157,153}}, //0 idle 1
  {BFRun_ArcMain_Idle0, { 83,  0, 83, 78}, {156,153}}, //1 idle 2
  {BFRun_ArcMain_Idle0, {166,  0, 84, 80}, {157,158}}, //2 idle 3
  {BFRun_ArcMain_Idle0, {  0, 80, 85, 79}, {159,159}}, //3 idle 4
  {BFRun_ArcMain_Idle0, { 85, 80, 86, 80}, {160,159}}, //4 idle 5
  {BFRun_ArcMain_Idle0, {171, 80, 84, 82}, {157,152}}, //5 idle 6
  {BFRun_ArcMain_Idle0, {  0,162, 84, 80}, {157,153}}, //6 idle 7
  {BFRun_ArcMain_Idle0, { 84,162, 81, 81}, {155,158}}, //7 idle 8
  {BFRun_ArcMain_Idle0, {165,162, 81, 83}, {154,160}}, //8 idle 9
  {BFRun_ArcMain_Idle1, {  0,  0, 81, 85}, {155,159}}, //9 idle 10

  {BFRun_ArcMain_Left0, {  0,  0,109, 78}, {178,156}}, //10 left 1
  {BFRun_ArcMain_Left0, {109,  0,108, 78}, {176,156}}, //11 left 2
  {BFRun_ArcMain_Left0, {  0, 78,109, 79}, {176,156}}, //12 left 3
  {BFRun_ArcMain_Left0, {109, 78,109, 76}, {176,158}}, //13 left 4
  {BFRun_ArcMain_Left0, {  0,157,108, 76}, {176,158}}, //14 left 5
  {BFRun_ArcMain_Left0, {108,157,109, 75}, {176,157}}, //15 left 6
  {BFRun_ArcMain_Left1, {  0,  0,108, 79}, {176,156}}, //16 left 7
  {BFRun_ArcMain_Left1, {108,  0,108, 79}, {176,156}}, //17 left 8
  {BFRun_ArcMain_Left1, {  0, 79,109, 77}, {176,158}}, //18 left 9
  {BFRun_ArcMain_Left1, {109, 79,109, 77}, {176,158}}, //19 left 10
  {BFRun_ArcMain_Left1, {  0,158,108, 77}, {176,157}}, //20 left 11

  {BFRun_ArcMain_Down0, {  0,  0, 83, 76}, {153,149}}, //21 down 1
  {BFRun_ArcMain_Down0, { 83,  0, 85, 75}, {155,151}}, //22 down 2
  {BFRun_ArcMain_Down0, {168,  0, 85, 75}, {155,150}}, //23 down 3
  {BFRun_ArcMain_Down0, {  0, 76, 85, 75}, {154,151}}, //24 down 4
  {BFRun_ArcMain_Down0, { 85, 76, 86, 75}, {155,152}}, //25 down 5
  {BFRun_ArcMain_Down0, {  0,152, 86, 75}, {154,152}}, //26 down 6

  {BFRun_ArcMain_Up0, {  0,  0, 95, 91}, {168,165}}, //27 up 1
  {BFRun_ArcMain_Up0, { 95,  0, 96, 90}, {170,162}}, //28 up 2
  {BFRun_ArcMain_Up0, {  0, 91, 96, 90}, {170,162}}, //29 up 3
  {BFRun_ArcMain_Up0, { 96, 91, 95, 91}, {169,164}}, //30 up 4
  {BFRun_ArcMain_Up1, {  0,  0, 95, 90}, {168,164}}, //31 up 5
  {BFRun_ArcMain_Up1, { 95,  0, 95, 89}, {169,163}}, //32 up 6

  {BFRun_ArcMain_Right0, {  0,  0, 95, 80}, {161,157}}, //33 right 1
  {BFRun_ArcMain_Right0, { 95,  0, 94, 82}, {163,157}}, //34 right 2
  {BFRun_ArcMain_Right0, {  0, 82, 94, 81}, {163,157}}, //35 right 3
  {BFRun_ArcMain_Right0, { 94, 82, 94, 82}, {163,159}}, //36 right 4
  {BFRun_ArcMain_Right0, {  0,164, 93, 82}, {162,159}}, //37 right 5
  {BFRun_ArcMain_Right0, { 93,164, 93, 82}, {162,159}}, //38 right 6
};

static const Animation char_bfrun_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, ASCR_BACK, 1}},             //CharAnim_Left
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 21, 22, 23, 24, 25, 26, 22, 23, 24, 25, 26, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 27, 28, 29, 30, 31, 32, 28, 29, 30, 31, 32, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 33, 34, 35, 36, 37, 38, 34, 35, 36, 37, 38, ASCR_BACK, 1}},             //CharAnim_Right
	{2, (const u8[]){ 5, 6, 7, 8, 9, ASCR_BACK, 1}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
};

//Boyfriend Run player functions
void Char_BFRun_SetFrame(void *user, u8 frame)
{
	Char_BFRun *this = (Char_BFRun*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfrun_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFRun_Tick(Character *character)
{
	Char_BFRun *this = (Char_BFRun*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Danger)
	{
		if (stage.song_step == 1)
		{
			this->character.focus_x = FIXED_DEC(-242,1);
			this->character.focus_y = FIXED_DEC(-211,1);
			this->character.focus_zoom = FIXED_DEC(447,1024);
		}
		if (stage.song_beat == 64)
		{
			this->character.focus_x = FIXED_DEC(-300,1);
			this->character.focus_y = FIXED_DEC(-193,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 96)
		{
			this->character.focus_x = FIXED_DEC(-236,1);
			this->character.focus_y = FIXED_DEC(-186,1);
			this->character.focus_zoom = FIXED_DEC(794,1024);
		}
		if (stage.song_beat == 128)
		{
			this->character.focus_x = FIXED_DEC(-300,1);
			this->character.focus_y = FIXED_DEC(-193,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 155)
		{
			this->character.focus_x = FIXED_DEC(-427,1);
			this->character.focus_y = FIXED_DEC(-182,1);
			this->character.focus_zoom = FIXED_DEC(924,1024);
		}
		if (stage.song_beat == 160)
		{
			this->character.focus_x = FIXED_DEC(-300,1);
			this->character.focus_y = FIXED_DEC(-193,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 192)
		{
			this->character.focus_x = FIXED_DEC(-236,1);
			this->character.focus_y = FIXED_DEC(-186,1);
			this->character.focus_zoom = FIXED_DEC(794,1024);
		}
		if (stage.song_beat == 256)
		{
			this->character.focus_x = FIXED_DEC(-300,1);
			this->character.focus_y = FIXED_DEC(-193,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 288)
		{
			this->character.focus_x = FIXED_DEC(-236,1);
			this->character.focus_y = FIXED_DEC(-186,1);
			this->character.focus_zoom = FIXED_DEC(794,1024);
		}
		if (stage.song_beat == 320)
		{
			this->character.focus_x = FIXED_DEC(-242,1);
			this->character.focus_y = FIXED_DEC(-211,1);
			this->character.focus_zoom = FIXED_DEC(447,1024);
		}
		if (stage.song_beat == 384)
		{
			this->character.focus_x = FIXED_DEC(-236,1);
			this->character.focus_y = FIXED_DEC(-186,1);
			this->character.focus_zoom = FIXED_DEC(794,1024);
		}
	}
	
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
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFRun_SetFrame);
	Character_Draw(character, &this->tex, &char_bfrun_frame[this->frame]);
}

void Char_BFRun_SetAnim(Character *character, u8 anim)
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

void Char_BFRun_Free(Character *character)
{
	Char_BFRun *this = (Char_BFRun*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFRun_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFRun *this = Mem_Alloc(sizeof(Char_BFRun));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFRun_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFRun_Tick;
	this->character.set_anim = Char_BFRun_SetAnim;
	this->character.free = Char_BFRun_Free;
	
	Animatable_Init(&this->character.animatable, char_bfrun_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-242,1);
	this->character.focus_y = FIXED_DEC(-211,1);
	this->character.focus_zoom = FIXED_DEC(447,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFRUN.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
