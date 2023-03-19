/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfchef.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Chef player types
enum
{
  BFChef_ArcMain_Idle0,
  BFChef_ArcMain_Idle1,
  BFChef_ArcMain_Idle2,
  BFChef_ArcMain_Left0,
  BFChef_ArcMain_Down0,
  BFChef_ArcMain_Up0,
  BFChef_ArcMain_Right0,
  BFChef_ArcMain_LeftM0,
  BFChef_ArcMain_LeftM1,
  BFChef_ArcMain_DownM0,
  BFChef_ArcMain_UpM0,
  BFChef_ArcMain_UpM1,
  BFChef_ArcMain_RightM0,
  BFChef_ArcMain_DownA0,
  BFChef_ArcMain_UpA0,
  BFChef_ArcMain_UpA1,
	
	BFChef_ArcMain_Max,
};

#define BFChef_Arc_Max BFChef_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFChef_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFChef;

//Boyfriend Chef player definitions
static const CharFrame char_bfchef_frame[] = {
  {BFChef_ArcMain_Idle0, {  0,  0, 99,133}, {159,156}}, //0 idle 1
  {BFChef_ArcMain_Idle0, { 99,  0, 98,133}, {159,156}}, //1 idle 2
  {BFChef_ArcMain_Idle1, {  0,  0, 99,134}, {158,157}}, //2 idle 3
  {BFChef_ArcMain_Idle1, { 99,  0,101,137}, {157,160}}, //3 idle 4
  {BFChef_ArcMain_Idle2, {  0,  0,101,137}, {157,160}}, //4 idle 5

  {BFChef_ArcMain_Left0, {  0,  0,105,132}, {168,155}}, //5 left 1
  {BFChef_ArcMain_Left0, {105,  0,105,132}, {168,155}}, //6 left 2

  {BFChef_ArcMain_Down0, {  0,  0, 98,124}, {162,147}}, //7 down 1
  {BFChef_ArcMain_Down0, { 98,  0,100,124}, {162,147}}, //8 down 2

  {BFChef_ArcMain_Up0, {  0,  0,114,132}, {155,156}}, //9 up 1
  {BFChef_ArcMain_Up0, {114,  0,114,132}, {155,155}}, //10 up 2

  {BFChef_ArcMain_Right0, {  0,  0,106,136}, {153,159}}, //11 right 1
  {BFChef_ArcMain_Right0, {106,  0,108,136}, {155,159}}, //12 right 2

  {BFChef_ArcMain_LeftM0, {  0,  0,105,132}, {166,156}}, //13 leftm 1
  {BFChef_ArcMain_LeftM0, {105,  0,105,128}, {166,152}}, //14 leftm 2
  {BFChef_ArcMain_LeftM1, {  0,  0,105,128}, {167,152}}, //15 leftm 3

  {BFChef_ArcMain_DownM0, {  0,  0, 98,126}, {163,150}}, //16 downm 1
  {BFChef_ArcMain_DownM0, { 98,  0, 99,125}, {163,148}}, //17 downm 2

  {BFChef_ArcMain_UpM0, {  0,  0,114,133}, {155,156}}, //18 upm 1
  {BFChef_ArcMain_UpM0, {114,  0,116,133}, {156,156}}, //19 upm 2
  {BFChef_ArcMain_UpM1, {  0,  0,114,134}, {154,157}}, //20 upm 3

  {BFChef_ArcMain_RightM0, {  0,  0,107,134}, {157,159}}, //21 rightm 1
  {BFChef_ArcMain_RightM0, {107,  0,108,134}, {156,159}}, //22 rightm 2

  {BFChef_ArcMain_DownA0, {  0,  0,100,122}, {164,146}}, //23 downa 1
  {BFChef_ArcMain_DownA0, {100,  0, 98,124}, {163,148}}, //24 downa 2
  {BFChef_ArcMain_DownA0, {  0,124, 98,124}, {162,148}}, //25 downa 3
  {BFChef_ArcMain_DownA0, { 98,124, 98,124}, {162,148}}, //26 downa 4

  {BFChef_ArcMain_UpA0, {  0,  0,110,136}, {157,159}}, //27 upa 1
  {BFChef_ArcMain_UpA0, {110,  0,110,136}, {157,159}}, //28 upa 2
  {BFChef_ArcMain_UpA1, {  0,  0,109,136}, {157,159}}, //29 upa 3
  {BFChef_ArcMain_UpA1, {109,  0,109,136}, {157,159}}, //30 upa 4
};

static const Animation char_bfchef_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{2, (const u8[]){ 23, 24, 25, 26, ASCR_BACK, 1}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{2, (const u8[]){ 27, 28, 29, 30, ASCR_BACK, 1}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 13, 14, 14, 15, 15, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 7, 16, 16, 17, 17, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 18, 19, 19, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 11, 21, 21, 22, 22, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Chef player functions
void Char_BFChef_SetFrame(void *user, u8 frame)
{
	Char_BFChef *this = (Char_BFChef*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfchef_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFChef_Tick(Character *character)
{
	Char_BFChef *this = (Char_BFChef*)character;
	
	if(character->animatable.anim  != CharAnim_Special1)
	{
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
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFChef_SetFrame);
	Character_Draw(character, &this->tex, &char_bfchef_frame[this->frame]);
}

void Char_BFChef_SetAnim(Character *character, u8 anim)
{
	Char_BFChef *this = (Char_BFChef*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFChef_Free(Character *character)
{
	Char_BFChef *this = (Char_BFChef*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFChef_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend chef object
	Char_BFChef *this = Mem_Alloc(sizeof(Char_BFChef));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFChef_New] Failed to allocate boyfriend chef object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFChef_Tick;
	this->character.set_anim = Char_BFChef_SetAnim;
	this->character.free = Char_BFChef_Free;
	
	Animatable_Init(&this->character.animatable, char_bfchef_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-160,1);
	this->character.focus_y = FIXED_DEC(-126,1);
	this->character.focus_zoom = FIXED_DEC(1130,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFCHEF.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
  "leftm0.tim",
  "leftm1.tim",
  "downm0.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "downa0.tim",
  "upa0.tim",
  "upa1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
