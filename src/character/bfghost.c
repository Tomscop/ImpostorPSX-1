/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfghost.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Ghost player types
enum
{
  BFGhost_ArcMain_Idle0,
  BFGhost_ArcMain_Idle1,
  BFGhost_ArcMain_Idle2,
  BFGhost_ArcMain_Left0,
  BFGhost_ArcMain_Left1,
  BFGhost_ArcMain_Left2,
  BFGhost_ArcMain_Down0,
  BFGhost_ArcMain_Up0,
  BFGhost_ArcMain_Up1,
  BFGhost_ArcMain_Up2,
  BFGhost_ArcMain_Right0,
  BFGhost_ArcMain_Right1,
  BFGhost_ArcMain_Right2,
  BFGhost_ArcMain_Right3,
  BFGhost_ArcMain_Right4,
  BFGhost_ArcMain_Right5,
  BFGhost_ArcMain_Right6,
  BFGhost_ArcMain_Right7,
  BFGhost_ArcMain_Right8,
  BFGhost_ArcMain_LeftM0,
  BFGhost_ArcMain_LeftM1,
  BFGhost_ArcMain_LeftM2,
  BFGhost_ArcMain_DownM0,
  BFGhost_ArcMain_DownM1,
  BFGhost_ArcMain_UpM0,
  BFGhost_ArcMain_UpM1,
  BFGhost_ArcMain_RightM0,
  BFGhost_ArcMain_RightM1,
  BFGhost_ArcMain_RightM2,
	
	BFGhost_ArcMain_Max,
};

#define BFGhost_Arc_Max BFGhost_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFGhost_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFGhost;

//Boyfriend Ghost player definitions
static const CharFrame char_bfghost_frame[] = {
  {BFGhost_ArcMain_Idle0, {  0,  0,104, 91}, {156,150}}, //0 idle 1
  {BFGhost_ArcMain_Idle0, {104,  0,105, 92}, {157,151}}, //1 idle 2
  {BFGhost_ArcMain_Idle0, {  0, 92,104, 92}, {156,153}}, //2 idle 3
  {BFGhost_ArcMain_Idle0, {104, 92,103, 94}, {154,156}}, //3 idle 4
  {BFGhost_ArcMain_Idle1, {  0,  0,102, 94}, {154,157}}, //4 idle 5
  {BFGhost_ArcMain_Idle1, {102,  0,102, 95}, {154,158}}, //5 idle 6
  {BFGhost_ArcMain_Idle1, {  0, 95,102, 95}, {154,158}}, //6 idle 7
  {BFGhost_ArcMain_Idle1, {102, 95,102, 95}, {154,158}}, //7 idle 8
  {BFGhost_ArcMain_Idle2, {  0,  0,102, 94}, {154,157}}, //8 idle 9
  {BFGhost_ArcMain_Idle2, {102,  0,102, 94}, {154,157}}, //9 idle 10
  {BFGhost_ArcMain_Idle2, {  0, 94,102, 94}, {154,156}}, //10 idle 11
  {BFGhost_ArcMain_Idle2, {102, 94,102, 94}, {154,156}}, //11 idle 12

  {BFGhost_ArcMain_Left0, {  0,  0,101, 89}, {159,156}}, //12 left 1
  {BFGhost_ArcMain_Left0, {101,  0, 97, 92}, {157,156}}, //13 left 2
  {BFGhost_ArcMain_Left0, {  0, 92, 97, 92}, {157,156}}, //14 left 3
  {BFGhost_ArcMain_Left0, { 97, 92, 97, 92}, {158,156}}, //15 left 4
  {BFGhost_ArcMain_Left1, {  0,  0, 97, 92}, {157,157}}, //16 left 5
  {BFGhost_ArcMain_Left1, { 97,  0, 98, 92}, {157,157}}, //17 left 6
  {BFGhost_ArcMain_Left1, {  0, 92, 98, 92}, {157,157}}, //18 left 7
  {BFGhost_ArcMain_Left1, { 98, 92, 98, 92}, {156,157}}, //19 left 8
  {BFGhost_ArcMain_Left2, {  0,  0, 98, 92}, {156,157}}, //20 left 9
  {BFGhost_ArcMain_Left2, { 98,  0, 98, 92}, {155,156}}, //21 left 10
  {BFGhost_ArcMain_Left2, {  0, 92, 97, 92}, {155,156}}, //22 left 11
  {BFGhost_ArcMain_Left2, { 97, 92, 96, 92}, {156,155}}, //23 left 12

  {BFGhost_ArcMain_Down0, {  0,  0,109, 76}, {155,143}}, //24 down 1
  {BFGhost_ArcMain_Down0, {109,  0,110, 76}, {156,145}}, //25 down 2
  {BFGhost_ArcMain_Down0, {  0, 76,110, 77}, {155,144}}, //26 down 3
  {BFGhost_ArcMain_Down0, {110, 76,111, 76}, {156,143}}, //27 down 4
  {BFGhost_ArcMain_Down0, {  0,153,110, 76}, {154,143}}, //28 down 5

  {BFGhost_ArcMain_Up0, {  0,  0, 93,109}, {149,163}}, //29 up 1
  {BFGhost_ArcMain_Up0, { 93,  0, 95,108}, {151,163}}, //30 up 2
  {BFGhost_ArcMain_Up0, {  0,109, 95,108}, {150,163}}, //31 up 3
  {BFGhost_ArcMain_Up0, { 95,109, 95,109}, {150,164}}, //32 up 4
  {BFGhost_ArcMain_Up1, {  0,  0, 95,109}, {150,164}}, //33 up 5
  {BFGhost_ArcMain_Up1, { 95,  0, 95,108}, {150,163}}, //34 up 6
  {BFGhost_ArcMain_Up1, {  0,109, 94,108}, {148,162}}, //35 up 7
  {BFGhost_ArcMain_Up1, { 94,109, 94,108}, {149,161}}, //36 up 8
  {BFGhost_ArcMain_Up2, {  0,  0, 95,109}, {150,161}}, //37 up 9
  {BFGhost_ArcMain_Up2, { 95,  0, 94,109}, {150,161}}, //38 up 10
  {BFGhost_ArcMain_Up2, {  0,109, 94,108}, {151,161}}, //39 up 11
  {BFGhost_ArcMain_Up2, { 94,109, 95,109}, {151,162}}, //40 up 12

  {BFGhost_ArcMain_Right0, {  0,  0,102, 91}, {145,155}}, //41 right 1
  {BFGhost_ArcMain_Right0, {102,  0,101, 91}, {145,155}}, //42 right 2
  {BFGhost_ArcMain_Right0, {  0, 91,101, 89}, {146,156}}, //43 right 3
  {BFGhost_ArcMain_Right0, {101, 91,102, 89}, {146,156}}, //44 right 4
  {BFGhost_ArcMain_Right1, {  0,  0,101, 89}, {145,156}}, //45 right 5
  {BFGhost_ArcMain_Right1, {101,  0,101, 89}, {145,156}}, //46 right 6
  {BFGhost_ArcMain_Right1, {  0, 89,101, 89}, {145,157}}, //47 right 7
  {BFGhost_ArcMain_Right1, {101, 89,101, 89}, {145,157}}, //48 right 8
  {BFGhost_ArcMain_Right2, {  0,  0,102, 89}, {145,157}}, //49 right 9
  {BFGhost_ArcMain_Right2, {102,  0,102, 89}, {146,157}}, //50 right 10
  {BFGhost_ArcMain_Right2, {  0, 89,102, 89}, {144,157}}, //51 right 11
  {BFGhost_ArcMain_Right2, {102, 89,102, 89}, {145,157}}, //52 right 12
  {BFGhost_ArcMain_Right3, {  0,  0,102, 89}, {144,157}}, //53 right 13
  {BFGhost_ArcMain_Right3, {102,  0,102, 89}, {144,157}}, //54 right 14
  {BFGhost_ArcMain_Right3, {  0, 89,102, 89}, {143,156}}, //55 right 15
  {BFGhost_ArcMain_Right3, {102, 89,102, 89}, {143,156}}, //56 right 16
  {BFGhost_ArcMain_Right4, {  0,  0,102, 89}, {143,156}}, //57 right 17
  {BFGhost_ArcMain_Right4, {102,  0,102, 89}, {143,156}}, //58 right 18
  {BFGhost_ArcMain_Right4, {  0, 89,101, 89}, {143,156}}, //59 right 19
  {BFGhost_ArcMain_Right4, {101, 89,101, 89}, {143,156}}, //60 right 20
  {BFGhost_ArcMain_Right5, {  0,  0,102, 89}, {143,155}}, //61 right 21
  {BFGhost_ArcMain_Right5, {102,  0,102, 90}, {143,156}}, //62 right 22
  {BFGhost_ArcMain_Right5, {  0, 90,101, 90}, {144,156}}, //63 right 23
  {BFGhost_ArcMain_Right5, {101, 90,101, 90}, {144,156}}, //64 right 24
  {BFGhost_ArcMain_Right6, {  0,  0,101, 90}, {144,155}}, //65 right 25
  {BFGhost_ArcMain_Right6, {101,  0,102, 90}, {144,155}}, //66 right 26
  {BFGhost_ArcMain_Right6, {  0, 90,102, 90}, {145,155}}, //67 right 27
  {BFGhost_ArcMain_Right6, {102, 90,101, 90}, {145,155}}, //68 right 28
  {BFGhost_ArcMain_Right7, {  0,  0,102, 90}, {145,156}}, //69 right 29
  {BFGhost_ArcMain_Right7, {102,  0,102, 90}, {145,156}}, //70 right 30
  {BFGhost_ArcMain_Right7, {  0, 90,101, 90}, {146,156}}, //71 right 31
  {BFGhost_ArcMain_Right7, {101, 90,101, 90}, {146,156}}, //72 right 32
  {BFGhost_ArcMain_Right8, {  0,  0,101, 89}, {146,156}}, //73 right 33
  {BFGhost_ArcMain_Right8, {101,  0,101, 89}, {146,156}}, //74 right 34
  {BFGhost_ArcMain_Right8, {  0, 89,102, 90}, {146,157}}, //75 right 35
  {BFGhost_ArcMain_Right8, {102, 89,101, 90}, {146,157}}, //76 right 36

  {BFGhost_ArcMain_LeftM0, {  0,  0,100, 89}, {159,157}}, //77 leftm 1
  {BFGhost_ArcMain_LeftM0, {100,  0, 90, 95}, {155,158}}, //78 leftm 2
  {BFGhost_ArcMain_LeftM0, {  0, 95, 90, 93}, {155,156}}, //79 leftm 3
  {BFGhost_ArcMain_LeftM0, { 90, 95, 90, 97}, {154,159}}, //80 leftm 4
  {BFGhost_ArcMain_LeftM1, {  0,  0, 90, 98}, {155,160}}, //81 leftm 5
  {BFGhost_ArcMain_LeftM1, { 90,  0, 90, 98}, {155,160}}, //82 leftm 6
  {BFGhost_ArcMain_LeftM1, {  0, 98, 90, 98}, {154,160}}, //83 leftm 7
  {BFGhost_ArcMain_LeftM1, { 90, 98, 90, 93}, {155,155}}, //84 leftm 8
  {BFGhost_ArcMain_LeftM2, {  0,  0, 90, 97}, {155,159}}, //85 leftm 9
  {BFGhost_ArcMain_LeftM2, { 90,  0, 90, 98}, {155,159}}, //86 leftm 10

  {BFGhost_ArcMain_DownM0, {  0,  0,109, 76}, {154,148}}, //87 downm 1
  {BFGhost_ArcMain_DownM0, {109,  0,113, 90}, {156,155}}, //88 downm 2
  {BFGhost_ArcMain_DownM0, {  0, 90,114, 88}, {157,153}}, //89 downm 3
  {BFGhost_ArcMain_DownM0, {114, 90,113, 88}, {155,153}}, //90 downm 4
  {BFGhost_ArcMain_DownM1, {  0,  0,113, 89}, {156,151}}, //91 downm 5
  {BFGhost_ArcMain_DownM1, {113,  0,113, 89}, {155,152}}, //92 downm 6
  {BFGhost_ArcMain_DownM1, {  0, 89,113, 88}, {156,151}}, //93 downm 7

  {BFGhost_ArcMain_UpM0, {  0,  0, 89,116}, {150,161}}, //94 upm 1
  {BFGhost_ArcMain_UpM0, { 89,  0, 87,119}, {148,161}}, //95 upm 2
  {BFGhost_ArcMain_UpM0, {  0,119, 89,117}, {149,160}}, //96 upm 3
  {BFGhost_ArcMain_UpM0, { 89,119, 89,117}, {149,159}}, //97 upm 4
  {BFGhost_ArcMain_UpM1, {  0,  0, 88,117}, {148,159}}, //98 upm 5
  {BFGhost_ArcMain_UpM1, { 88,  0, 88,117}, {148,159}}, //99 upm 6

  {BFGhost_ArcMain_RightM0, {  0,  0, 96, 97}, {146,159}}, //100 rightm 1
  {BFGhost_ArcMain_RightM0, { 96,  0, 95,100}, {145,162}}, //101 rightm 2
  {BFGhost_ArcMain_RightM0, {  0,100, 95, 99}, {146,160}}, //102 rightm 3
  {BFGhost_ArcMain_RightM0, { 95,100, 95, 96}, {147,157}}, //103 rightm 4
  {BFGhost_ArcMain_RightM1, {  0,  0, 95, 99}, {146,159}}, //104 rightm 5
  {BFGhost_ArcMain_RightM1, { 95,  0, 95, 99}, {146,159}}, //105 rightm 6
  {BFGhost_ArcMain_RightM1, {  0, 99, 95, 99}, {146,159}}, //106 rightm 7
  {BFGhost_ArcMain_RightM1, { 95, 99, 95, 99}, {146,159}}, //107 rightm 8
  {BFGhost_ArcMain_RightM2, {  0,  0, 95, 98}, {146,158}}, //108 rightm 9
  {BFGhost_ArcMain_RightM2, { 95,  0, 95, 99}, {146,158}}, //109 rightm 10
};

static const Animation char_bfghost_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 24, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, 27, 28, 25, 26, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Special1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 77, 78, 79, 80, 81, 82, 83, 84, 85, 85, 86, 86, 84, 84, 85, 85, 86, 86, 84, 84, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 90, 90, 91, 91, 92, 92, 93, 93, 90, 90, 91, 91, 92, 92, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 29, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 41, 100, 101, 102, 103, 104, 105, 106, 107, 107, 108, 105, 106, 106, 109, 109, 105, 105, 106, 106, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Ghost player functions
void Char_BFGhost_SetFrame(void *user, u8 frame)
{
	Char_BFGhost *this = (Char_BFGhost*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfghost_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFGhost_Tick(Character *character)
{
	Char_BFGhost *this = (Char_BFGhost*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFGhost_SetFrame);
	Character_Draw(character, &this->tex, &char_bfghost_frame[this->frame]);
}

void Char_BFGhost_SetAnim(Character *character, u8 anim)
{
	Char_BFGhost *this = (Char_BFGhost*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFGhost_Free(Character *character)
{
	Char_BFGhost *this = (Char_BFGhost*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFGhost_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend ghost object
	Char_BFGhost *this = Mem_Alloc(sizeof(Char_BFGhost));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFGhost_New] Failed to allocate boyfriend ghost object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFGhost_Tick;
	this->character.set_anim = Char_BFGhost_SetAnim;
	this->character.free = Char_BFGhost_Free;
	
	Animatable_Init(&this->character.animatable, char_bfghost_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 2;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-160,1);
	this->character.focus_y = FIXED_DEC(-147,1);
	this->character.focus_zoom = FIXED_DEC(509,512);
	
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFGHOST.ARC;1");
		
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
  "right4.tim",
  "right5.tim",
  "right6.tim",
  "right7.tim",
  "right8.tim",
  "leftm0.tim",
  "leftm1.tim",
  "leftm2.tim",
  "downm0.tim",
  "downm1.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "rightm1.tim",
  "rightm2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
