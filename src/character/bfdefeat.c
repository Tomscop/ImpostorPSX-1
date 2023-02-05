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
  BFDefeat_ArcMain_Idle0,
  BFDefeat_ArcMain_Idle1,
  BFDefeat_ArcMain_Left0,
  BFDefeat_ArcMain_Down0,
  BFDefeat_ArcMain_Up0,
  BFDefeat_ArcMain_Right0,
  BFDefeat_ArcMain_Miss0,
  BFDefeat_ArcMain_IdleS0,
  BFDefeat_ArcMain_IdleS1,
  BFDefeat_ArcMain_IdleS2,
  BFDefeat_ArcMain_LeftS0,
  BFDefeat_ArcMain_DownS0,
  BFDefeat_ArcMain_UpS0,
  BFDefeat_ArcMain_RightS0,
	
	BFDefeat_ArcMain_Max,
};

#define BFDefeat_Arc_Max BFDefeat_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFDefeat_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFDefeat;

//Boyfriend Defeat player definitions
static const CharFrame char_bfdefeat_frame[] = {
  {BFDefeat_ArcMain_Idle0, {  0,  0,103,103}, {160,159}}, //0 idle 1
  {BFDefeat_ArcMain_Idle0, {103,  0,104,103}, {160,159}}, //1 idle 2
  {BFDefeat_ArcMain_Idle0, {  0,103,104,103}, {160,159}}, //2 idle 3
  {BFDefeat_ArcMain_Idle0, {104,103,104,104}, {159,160}}, //3 idle 4
  {BFDefeat_ArcMain_Idle1, {  0,  0,103,104}, {159,160}}, //4 idle 5

  {BFDefeat_ArcMain_Left0, {  0,  0, 90,104}, {148,160}}, //5 left 1
  {BFDefeat_ArcMain_Left0, { 90,  0, 91,104}, {149,160}}, //6 left 2

  {BFDefeat_ArcMain_Down0, {  0,  0, 93,100}, {149,157}}, //7 down 1
  {BFDefeat_ArcMain_Down0, { 93,  0, 92,101}, {148,157}}, //8 down 2

  {BFDefeat_ArcMain_Up0, {  0,  0, 94,106}, {149,162}}, //9 up 1
  {BFDefeat_ArcMain_Up0, { 94,  0, 94,105}, {148,161}}, //10 up 2

  {BFDefeat_ArcMain_Right0, {  0,  0, 95,104}, {148,160}}, //11 right 1
  {BFDefeat_ArcMain_Right0, { 95,  0, 95,104}, {148,160}}, //12 right 2

  {BFDefeat_ArcMain_Miss0, {  0,  0, 93,104}, {151,160}}, //13 miss 1
  {BFDefeat_ArcMain_Miss0, { 93,  0, 93,104}, {151,159}}, //14 miss 2
  {BFDefeat_ArcMain_Miss0, {  0,104, 93,104}, {151,159}}, //15 miss 3
  
  {BFDefeat_ArcMain_IdleS0, {  0,  0,106,101}, {160,155}}, //16 idles 1
  {BFDefeat_ArcMain_IdleS0, {106,  0,106,101}, {160,155}}, //17 idles 2
  {BFDefeat_ArcMain_IdleS0, {  0,101,105,102}, {158,156}}, //18 idles 3
  {BFDefeat_ArcMain_IdleS0, {105,101,106,102}, {159,156}}, //19 idles 4
  {BFDefeat_ArcMain_IdleS1, {  0,  0,104,104}, {158,158}}, //20 idles 5
  {BFDefeat_ArcMain_IdleS1, {104,  0,104,104}, {158,158}}, //21 idles 6
  {BFDefeat_ArcMain_IdleS1, {  0,104,104,105}, {159,160}}, //22 idles 7
  {BFDefeat_ArcMain_IdleS1, {104,104,104,105}, {159,160}}, //23 idles 8
  {BFDefeat_ArcMain_IdleS2, {  0,  0,104,106}, {159,160}}, //24 idles 9
  {BFDefeat_ArcMain_IdleS2, {104,  0,104,105}, {159,160}}, //25 idles 10
  {BFDefeat_ArcMain_IdleS2, {  0,106,105,106}, {159,160}}, //26 idles 11

  {BFDefeat_ArcMain_LeftS0, {  0,  0,117,105}, {161,160}}, //27 lefts 1
  {BFDefeat_ArcMain_LeftS0, {117,  0,113,105}, {162,159}}, //28 lefts 2
  {BFDefeat_ArcMain_LeftS0, {  0,105,112,105}, {162,159}}, //29 lefts 3

  {BFDefeat_ArcMain_DownS0, {  0,  0, 94, 95}, {153,147}}, //30 downs 1
  {BFDefeat_ArcMain_DownS0, { 94,  0, 91, 96}, {151,149}}, //31 downs 2
  {BFDefeat_ArcMain_DownS0, {  0, 96, 90, 96}, {151,149}}, //32 downs 3

  {BFDefeat_ArcMain_UpS0, {  0,  0,102,124}, {154,178}}, //33 ups 1
  {BFDefeat_ArcMain_UpS0, {102,  0,105,121}, {156,175}}, //34 ups 2
  {BFDefeat_ArcMain_UpS0, {  0,124,105,121}, {156,175}}, //35 ups 3

  {BFDefeat_ArcMain_RightS0, {  0,  0, 98,100}, {142,154}}, //36 rights 1
  {BFDefeat_ArcMain_RightS0, { 98,  0, 96,102}, {142,156}}, //37 rights 2
  {BFDefeat_ArcMain_RightS0, {  0,102, 95,102}, {142,156}}, //38 rights 3
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
	{1, (const u8[]){ 36, 37, 38, ASCR_BACK, 1}},             //CharAnim_Right
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
		if ((Animatable_Ended(&character->animatable) &&
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
			(stage.song_step & 0x7) == 0) || 
			(character->animatable.anim == CharAnim_Special1 && 
			(stage.song_step & 0x7) == 0))
			character->set_anim(character, CharAnim_Idle);
	}

	if ((stage.stage_id == StageId_Defeat) && (stage.song_step == 272))
		Animatable_Init(&this->character.animatable, char_bfdefeat_anim2);
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFDefeat_SetFrame);
	Character_Draw(character, &this->tex, &char_bfdefeat_frame[this->frame]);
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
	Mem_Free(this->arc_main);
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
	
	this->character.health_i = 0;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-160,1);
	this->character.focus_y = FIXED_DEC(-147,1);
	this->character.focus_zoom = FIXED_DEC(509,512);
	
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFDEFEAT.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
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
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
