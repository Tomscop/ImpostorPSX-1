/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfdefeat.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Defeat player types
enum
{
  BFDefeat_Arc1_Idle0,
  BFDefeat_Arc1_Idle1,
  BFDefeat_Arc1_A,
  BFDefeat_Arc1_B,
  BFDefeat_Arc1_Miss0,
  BFDefeat_Arc2_IdleS0,
  BFDefeat_Arc2_IdleS1,
  BFDefeat_Arc2_IdleS2,
  BFDefeat_Arc2_LeftS0,
  BFDefeat_Arc2_DownS0,
  BFDefeat_Arc2_UpS0,
  BFDefeat_Arc2_RightS0,
	
	BFDefeat_ArcMain_Max,
};

#define BFDefeat_Arc_Max BFDefeat_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_1, arc_2;
	IO_Data arc_ptr[BFDefeat_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFDefeat;

//Boyfriend Defeat player definitions
static const CharFrame char_bfdefeat_frame[] = {
  {BFDefeat_Arc1_Idle0, {  0,  0,103,103}, {160,159-2}}, //0 idle 1
  {BFDefeat_Arc1_Idle0, {103,  0,104,103}, {160,159-2}}, //1 idle 2
  {BFDefeat_Arc1_Idle0, {  0,103,104,103}, {160,159-2}}, //2 idle 3
  {BFDefeat_Arc1_Idle0, {104,103,104,104}, {159,160-2}}, //3 idle 4
  {BFDefeat_Arc1_Idle1, {  0,  0,103,104}, {159,160-2}}, //4 idle 5

  {BFDefeat_Arc1_A, {  0,  0, 90,104}, {148,160-2}}, //5 left 1
  {BFDefeat_Arc1_A, { 90,  0, 91,104}, {149,160-2}}, //6 left 2

  {BFDefeat_Arc1_A, {  0,104, 93,100}, {149,157-2}}, //7 down 1
  {BFDefeat_Arc1_A, { 93,104, 92,101}, {148,157-2}}, //8 down 2

  {BFDefeat_Arc1_B, {  0,  0, 94,106}, {149,162-2}}, //9 up 1
  {BFDefeat_Arc1_B, { 94,  0, 94,105}, {148,161-2}}, //10 up 2

  {BFDefeat_Arc1_B, {  0,106, 95,104}, {148,160-2}}, //11 right 1
  {BFDefeat_Arc1_B, { 95,106, 95,104}, {148,160-2}}, //12 right 2

  {BFDefeat_Arc1_Miss0, {  0,  0, 93,104}, {151,160-2}}, //13 miss 1
  {BFDefeat_Arc1_Miss0, { 93,  0, 93,104}, {151,159-2}}, //14 miss 2
  {BFDefeat_Arc1_Miss0, {  0,104, 93,104}, {151,159-2}}, //15 miss 3
  
  {BFDefeat_Arc2_IdleS0, {  0,  0,106,101}, {160,155}}, //16 idles 1
  {BFDefeat_Arc2_IdleS0, {106,  0,106,101}, {160,155}}, //17 idles 2
  {BFDefeat_Arc2_IdleS0, {  0,101,105,102}, {158,156}}, //18 idles 3
  {BFDefeat_Arc2_IdleS0, {105,101,106,102}, {159,156}}, //19 idles 4
  {BFDefeat_Arc2_IdleS1, {  0,  0,104,104}, {157,158}}, //20 idles 5
  {BFDefeat_Arc2_IdleS1, {104,  0,104,104}, {157,158}}, //21 idles 6
  {BFDefeat_Arc2_IdleS1, {  0,104,104,105}, {158,159}}, //22 idles 7
  {BFDefeat_Arc2_IdleS1, {104,104,104,105}, {158,159}}, //23 idles 8
  {BFDefeat_Arc2_IdleS2, {  0,  0,104,106}, {158,159}}, //24 idles 9
  {BFDefeat_Arc2_IdleS2, {104,  0,104,105}, {158,159}}, //25 idles 10
  {BFDefeat_Arc2_IdleS2, {  0,106,105,106}, {159,159}}, //26 idles 11

  {BFDefeat_Arc2_LeftS0, {  0,  0,117,105}, {161,160}}, //27 lefts 1
  {BFDefeat_Arc2_LeftS0, {117,  0,113,105}, {162,159}}, //28 lefts 2
  {BFDefeat_Arc2_LeftS0, {  0,105,112,105}, {162,159}}, //29 lefts 3

  {BFDefeat_Arc2_DownS0, {  0,  0, 94, 95}, {153,147}}, //30 downs 1
  {BFDefeat_Arc2_DownS0, { 94,  0, 91, 96}, {151,149}}, //31 downs 2
  {BFDefeat_Arc2_DownS0, {  0, 96, 90, 96}, {151,149}}, //32 downs 3

  {BFDefeat_Arc2_UpS0, {  0,  0,102,124}, {154,178}}, //33 ups 1
  {BFDefeat_Arc2_UpS0, {102,  0,105,121}, {156,175}}, //34 ups 2
  {BFDefeat_Arc2_UpS0, {  0,124,105,121}, {156,175}}, //35 ups 3

  {BFDefeat_Arc2_RightS0, {  0,  0, 98,100}, {142,154}}, //36 rights 1
  {BFDefeat_Arc2_RightS0, { 98,  0, 96,102}, {142,156}}, //37 rights 2
  {BFDefeat_Arc2_RightS0, {  0,102, 95,102}, {142,156}}, //38 rights 3
};

static const Animation char_bfdefeat_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, 8, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, 10, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 11, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

static const Animation char_bfdefeat_anim2[PlayerAnim_Max] = {
	{1, (const u8[]){ 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 24, ASCR_CHGANI, CharAnim_Special1}}, //CharAnim_Idle
	{2, (const u8[]){ 27, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 30, 31, 32, 32, 32, 32, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 33, 34, 35, 35, 35, 35, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 36, 37, 38, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {1, (const u8[]){ 25, 26, 24, ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 13, 14, 15, 14, 15, 14, 15, 14, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Defeat player functions
void Char_BFDefeat_SetFrame(void *user, u8 frame)
{
	Char_BFDefeat *this = (Char_BFDefeat*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfdefeat_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFDefeat_Tick(Character *character)
{
	Char_BFDefeat *this = (Char_BFDefeat*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Defeat)
	{
		if (stage.song_step == 128)
		{
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
		if (stage.song_step == 192)
		{
			this->character.focus_zoom = FIXED_DEC(577,512);
		}
		if (stage.song_step == 272)
		{
			this->character.focus_zoom = FIXED_DEC(177,256);
		}
		if (stage.song_step == 460)
		{
			this->character.focus_x = FIXED_DEC(-188,1);
			this->character.focus_zoom = FIXED_DEC(427,512);
		}
		if (stage.song_step == 656)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_zoom = FIXED_DEC(177,256);
		}
		if (stage.song_step == 776)
		{
			this->character.focus_zoom = FIXED_DEC(427,512);
		}
		if (stage.song_step == 848)
		{
			this->character.focus_zoom = FIXED_DEC(247,256);
		}
		if (stage.song_step == 912)
		{
			this->character.focus_zoom = FIXED_DEC(281,256);
		}
		if (stage.song_step == 976)
		{
			this->character.focus_zoom = FIXED_DEC(75,64);
		}
		if (stage.song_step == 1040)
		{
			this->character.focus_zoom = FIXED_DEC(427,512);
		}
		if (stage.song_step == 1168)
		{
			this->character.focus_x = FIXED_DEC(-188,1);
			this->character.focus_y = FIXED_DEC(-128,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
			this->character.health_bar = 0xFF5EFF35;
			this->character.health_i = 8;
		}
		if (stage.song_step == 1440)
		{
			this->character.focus_y = FIXED_DEC(-140,1);
			this->character.focus_zoom = FIXED_DEC(427,512);
			this->character.health_bar = 0xFF29B5D6;
			this->character.health_i = 0;
		}
		if (stage.song_step == 1696)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_zoom = FIXED_DEC(247,256);
		}
		if (stage.song_step == 1824)
		{
			this->character.focus_zoom = FIXED_DEC(281,256);
		}
		if (stage.song_step == 1888)
		{
			this->character.focus_zoom = FIXED_DEC(75,64);
		}
		if (stage.song_step == 1952)
		{
			this->character.focus_zoom = FIXED_DEC(1875,128);
		}
	}
	if (stage.stage_id == StageId_Finale)
	{
		if (stage.song_beat == 32)
		{
			this->character.focus_x = FIXED_DEC(-433,1);
			this->character.focus_y = FIXED_DEC(-130,1);
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		}
		if (stage.song_beat == 48)
		{
			this->character.focus_x = FIXED_DEC(-152,1);
			this->character.focus_y = FIXED_DEC(-130,1);
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		}
		if (stage.song_beat == 64)
		{
			this->character.focus_x = FIXED_DEC(-152,1);
			this->character.focus_y = FIXED_DEC(-130,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.song_beat == 67)
		{
			this->character.focus_x = FIXED_DEC(-152,1);
			this->character.focus_y = FIXED_DEC(-130,1);
			this->character.focus_zoom = FIXED_DEC(16,10);
		}
		if (stage.song_beat == 68) //normal focus
		{
			this->character.focus_x = FIXED_DEC(-249,1);
			this->character.focus_y = FIXED_DEC(-204,1);
			this->character.focus_zoom = FIXED_DEC(703+24,1024);
		}
		if (stage.song_beat == 494)
		{
			this->character.focus_x = FIXED_DEC(-249,1);
			this->character.focus_y = FIXED_DEC(-218,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
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
		if (((character->animatable.anim != CharAnim_Left &&
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
			(stage.song_step & 0x7) == 0) || 
			(character->animatable.anim == CharAnim_Special1 && 
			(stage.song_step & 0x7) == 0))
			character->set_anim(character, CharAnim_Idle);
	}

	if ((stage.stage_id == StageId_Defeat) && (stage.song_step == 272))
		Animatable_Init(&this->character.animatable, char_bfdefeat_anim2);
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFDefeat_SetFrame);
	if ((stage.stage_id == StageId_DoubleKill) && (stage.song_step >= 3408))
		Character_DrawCol(character, &this->tex, &char_bfdefeat_frame[this->frame], 200, 128, 128);
	if ((stage.stage_id == StageId_Defeat) && ((stage.song_step <= 1167) || (stage.song_step >= 1440)))
		Character_DrawCol(character, &this->tex, &char_bfdefeat_frame[this->frame], 200, 128, 128);
	if (stage.stage_id == StageId_Finale)
		Character_DrawCol(character, &this->tex, &char_bfdefeat_frame[this->frame], 200, 128, 128);
}

void Char_BFDefeat_SetAnim(Character *character, u8 anim)
{
	Char_BFDefeat *this = (Char_BFDefeat*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFDefeat_Free(Character *character)
{
	Char_BFDefeat *this = (Char_BFDefeat*)character;
	
	//Free art
	if (stage.stage_id == StageId_Defeat)
		Mem_Free(this->arc_main);
	if (stage.stage_id == StageId_DoubleKill)
		Mem_Free(this->arc_1);
	if (stage.stage_id == StageId_Finale)
		Mem_Free(this->arc_2);
}

Character *Char_BFDefeat_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend defeat object
	Char_BFDefeat *this = Mem_Alloc(sizeof(Char_BFDefeat));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFDefeat_New] Failed to allocate boyfriend defeat object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFDefeat_Tick;
	this->character.set_anim = Char_BFDefeat_SetAnim;
	this->character.free = Char_BFDefeat_Free;
	
	if (stage.stage_id != StageId_Finale)
		Animatable_Init(&this->character.animatable, char_bfdefeat_anim);
	else
		Animatable_Init(&this->character.animatable, char_bfdefeat_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	if (stage.stage_id != StageId_Finale)
		this->character.health_i = 0;
	else
		this->character.health_i = 20;
	
	//health bar color
	if (stage.stage_id != StageId_Finale)
		this->character.health_bar = 0xFF29B5D6;
	else
		this->character.health_bar = 0xFF35D8FF;
	
	if (stage.stage_id == StageId_Defeat)
	{
		this->character.focus_x = FIXED_DEC(-226,1);
		this->character.focus_y = FIXED_DEC(-140,1);
		this->character.focus_zoom = FIXED_DEC(417,512);
	}
	else if (stage.stage_id == StageId_Finale)
	{
		this->character.focus_x = FIXED_DEC(-297,1);
		this->character.focus_y = FIXED_DEC(-183,1);
		this->character.focus_zoom = FIXED_DEC(543,1024);
	}
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	//Load scene specific art
	switch (stage.stage_id)
	{
		case StageId_DoubleKill: //Double Kill
		{
			this->arc_1 = IO_Read("\\PLAYER\\BFDEFET1.ARC;1");
			
			const char **pathp = (const char *[]){
				"idle0.tim",
				"idle1.tim",
				"a.tim",
				"b.tim",
				"miss0.tim",
				NULL
			};
			IO_Data *arc_ptr = &this->arc_ptr[BFDefeat_Arc1_Idle0];
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_1, *pathp);
			this->arc_main = NULL;
			this->arc_2 = NULL;
			break;
		}
		case StageId_Defeat: //Defeat
		{
			this->arc_main = IO_Read("\\PLAYER\\BFDEFEAT.ARC;1");
			
			const char **pathp = (const char *[]){
				"idle0.tim",
				"idle1.tim",
				"a.tim",
				"b.tim",
				"miss0.tim",
				"idles0.tim",
				"idles1.tim",
				"idles2.tim",
				"lefts0.tim",
				"downs0.tim",
				"ups0.tim",
				"rights0.tim",
				NULL
			};
			IO_Data *arc_ptr = &this->arc_ptr[BFDefeat_Arc1_Idle0];
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			this->arc_1 = NULL;
			this->arc_2 = NULL;
			break;
		}
		case StageId_Finale: //Finale
		{
			this->arc_2 = IO_Read("\\PLAYER\\BFDEFET2.ARC;1");
			
			const char **pathp = (const char *[]){
				"miss0.tim",
				"idles0.tim",
				"idles1.tim",
				"idles2.tim",
				"lefts0.tim",
				"downs0.tim",
				"ups0.tim",
				"rights0.tim",
				NULL
			};
			IO_Data *arc_ptr = &this->arc_ptr[BFDefeat_Arc1_Miss0];
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_2, *pathp);
			this->arc_1 = NULL;
			this->arc_main = NULL;
			break;
		}
		default:
			this->arc_1 = NULL;
			break;
	}
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
