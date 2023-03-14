/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfpolus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Polus player types
enum
{
  BFPolus_ArcMain_Idle0,
  BFPolus_ArcMain_Idle1,
  BFPolus_ArcMain_Idle2,
  BFPolus_ArcMain_Left0,
  BFPolus_ArcMain_Left1,
  BFPolus_ArcMain_Down0,
  BFPolus_ArcMain_Down1,
  BFPolus_ArcMain_Up0,
  BFPolus_ArcMain_Up1,
  BFPolus_ArcMain_Right0,
  BFPolus_ArcMain_Right1,
  BFPolus_ArcMain_LeftM0,
  BFPolus_ArcMain_DownM0,
  BFPolus_ArcMain_UpM0,
  BFPolus_ArcMain_RightM0,
	
	BFPolus_ArcMain_Max,
};

#define BFPolus_Arc_Max BFPolus_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFPolus_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFPolus;

//Boyfriend Polus player definitions
static const CharFrame char_bfpolus_frame[] = {
  {BFPolus_ArcMain_Idle0, {  0,  0, 95,103}, {158,156}}, //0 idle 1
  {BFPolus_ArcMain_Idle0, { 95,  0, 95,103}, {159,156}}, //1 idle 2
  {BFPolus_ArcMain_Idle0, {  0,103, 95,103}, {159,157}}, //2 idle 3
  {BFPolus_ArcMain_Idle0, { 95,103, 95,103}, {159,156}}, //3 idle 4
  {BFPolus_ArcMain_Idle1, {  0,  0, 94,104}, {158,157}}, //4 idle 5
  {BFPolus_ArcMain_Idle1, { 94,  0, 93,104}, {157,157}}, //5 idle 6
  {BFPolus_ArcMain_Idle1, {  0,104, 94,107}, {158,160}}, //6 idle 7
  {BFPolus_ArcMain_Idle1, { 94,104, 94,107}, {158,160}}, //7 idle 8
  {BFPolus_ArcMain_Idle2, {  0,  0, 94,107}, {158,160}}, //8 idle 9
  {BFPolus_ArcMain_Idle2, { 94,  0, 94,107}, {158,160}}, //9 idle 10
  {BFPolus_ArcMain_Idle2, {  0,107, 94,107}, {158,160}}, //10 idle 11
  {BFPolus_ArcMain_Idle2, { 94,107, 94,107}, {159,160}}, //11 idle 12

  {BFPolus_ArcMain_Left0, {  0,  0, 95,105}, {163,158}}, //12 left 1
  {BFPolus_ArcMain_Left0, { 95,  0, 96,105}, {163,158}}, //13 left 2
  {BFPolus_ArcMain_Left0, {  0,105, 93,105}, {160,158}}, //14 left 3
  {BFPolus_ArcMain_Left0, { 93,105, 93,105}, {160,158}}, //15 left 4
  {BFPolus_ArcMain_Left1, {  0,  0, 93,105}, {160,158}}, //16 left 5
  {BFPolus_ArcMain_Left1, { 93,  0, 93,105}, {160,158}}, //17 left 6

  {BFPolus_ArcMain_Down0, {  0,  0, 96, 95}, {158,147}}, //18 down 1
  {BFPolus_ArcMain_Down0, { 96,  0, 96, 95}, {158,147}}, //19 down 2
  {BFPolus_ArcMain_Down0, {  0, 95, 94, 96}, {157,148}}, //20 down 3
  {BFPolus_ArcMain_Down0, { 94, 95, 95, 96}, {158,148}}, //21 down 4
  {BFPolus_ArcMain_Down1, {  0,  0, 94, 96}, {158,148}}, //22 down 5
  {BFPolus_ArcMain_Down1, { 94,  0, 95, 96}, {158,148}}, //23 down 6

  {BFPolus_ArcMain_Up0, {  0,  0, 91,113}, {148,165}}, //24 up 1
  {BFPolus_ArcMain_Up0, { 91,  0, 90,113}, {148,165}}, //25 up 2
  {BFPolus_ArcMain_Up0, {  0,113, 92,111}, {150,164}}, //26 up 3
  {BFPolus_ArcMain_Up0, { 92,113, 92,112}, {150,164}}, //27 up 4
  {BFPolus_ArcMain_Up1, {  0,  0, 91,112}, {149,164}}, //28 up 5
  {BFPolus_ArcMain_Up1, { 91,  0, 92,112}, {150,164}}, //29 up 6

  {BFPolus_ArcMain_Right0, {  0,  0, 89,106}, {147,158}}, //30 right 1
  {BFPolus_ArcMain_Right0, { 89,  0, 89,106}, {147,158}}, //31 right 2
  {BFPolus_ArcMain_Right0, {  0,106, 88,106}, {148,159}}, //32 right 3
  {BFPolus_ArcMain_Right0, { 88,106, 88,106}, {148,159}}, //33 right 4
  {BFPolus_ArcMain_Right1, {  0,  0, 88,106}, {148,159}}, //34 right 5
  {BFPolus_ArcMain_Right1, { 88,  0, 88,106}, {148,159}}, //35 right 6

  {BFPolus_ArcMain_LeftM0, {  0,  0, 93,108}, {159,161}}, //36 leftm 1
  {BFPolus_ArcMain_LeftM0, { 93,  0, 93,108}, {159,161}}, //37 leftm 2

  {BFPolus_ArcMain_DownM0, {  0,  0, 96,100}, {156,152}}, //38 downm 1
  {BFPolus_ArcMain_DownM0, { 96,  0, 96, 99}, {156,151}}, //39 downm 2

  {BFPolus_ArcMain_UpM0, {  0,  0, 92,105}, {153,158}}, //40 upm 1
  {BFPolus_ArcMain_UpM0, { 92,  0, 91,108}, {153,160}}, //41 upm 2

  {BFPolus_ArcMain_RightM0, {  0,  0, 92,110}, {151,163}}, //42 rightm 1
  {BFPolus_ArcMain_RightM0, { 92,  0, 93,107}, {151,160}}, //43 rightm 2
};

static const Animation char_bfpolus_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 8, 9, ASCR_CHGANI, CharAnim_Special1}}, //CharAnim_Idle
	{1, (const u8[]){ 12, 13, 14, 15, 16, 17, 14, 15, 16, 17, 14, 15, 16, 17, 14, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 18, 19, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, 20, 21, 22, 23, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 24, 25, 26, 27, 28, 29, 26, 27, 28, 29, 26, 27, 28, 29, 26, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 30, 31, 32, 33, 34, 35, 32, 33, 34, 35, 32, 33, 34, 35, 32, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {1, (const u8[]){ 10, 11, 8, 9, ASCR_CHGANI, CharAnim_Special1}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 36, 37, 37, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 38, 39, 39, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 40, 41, 41, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 42, 43, 43, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Polus player functions
void Char_BFPolus_SetFrame(void *user, u8 frame)
{
	Char_BFPolus *this = (Char_BFPolus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfpolus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFPolus_Tick(Character *character)
{
	Char_BFPolus *this = (Char_BFPolus*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFPolus_SetFrame);
	Character_Draw(character, &this->tex, &char_bfpolus_frame[this->frame]);
}

void Char_BFPolus_SetAnim(Character *character, u8 anim)
{
	Char_BFPolus *this = (Char_BFPolus*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFPolus_Free(Character *character)
{
	Char_BFPolus *this = (Char_BFPolus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFPolus_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFPolus *this = Mem_Alloc(sizeof(Char_BFPolus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFPolus_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFPolus_Tick;
	this->character.set_anim = Char_BFPolus_SetAnim;
	this->character.free = Char_BFPolus_Free;
	
	Animatable_Init(&this->character.animatable, char_bfpolus_anim);
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
	this->arc_main = IO_Read("\\PLAYER\\BFPOLUS.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
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
