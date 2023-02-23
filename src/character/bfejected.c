/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfejected.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Ejected player types
enum
{
  BFEjected_ArcMain_Idle0,
  BFEjected_ArcMain_Idle1,
  BFEjected_ArcMain_Idle2,
  BFEjected_ArcMain_Left0,
  BFEjected_ArcMain_Left1,
  BFEjected_ArcMain_Down0,
  BFEjected_ArcMain_Down1,
  BFEjected_ArcMain_Up0,
  BFEjected_ArcMain_Up1,
  BFEjected_ArcMain_Up2,
  BFEjected_ArcMain_Right0,
  BFEjected_ArcMain_LeftM0,
  BFEjected_ArcMain_LeftM1,
  BFEjected_ArcMain_DownM0,
  BFEjected_ArcMain_DownM1,
  BFEjected_ArcMain_UpM0,
  BFEjected_ArcMain_UpM1,
  BFEjected_ArcMain_RightM0,
  BFEjected_ArcMain_RightM1,
	
	BFEjected_ArcMain_Max,
};

#define BFEjected_Arc_Max BFEjected_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFEjected_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFEjected;

//Boyfriend Ejected player definitions
static const CharFrame char_bfejected_frame[] = {
  {BFEjected_ArcMain_Idle0, {  0,  0,118,111}, {158,157}}, //0 idle 1
  {BFEjected_ArcMain_Idle0, {118,  0,116,112}, {157,159}}, //1 idle 2
  {BFEjected_ArcMain_Idle0, {  0,112,116,112}, {157,159}}, //2 idle 3
  {BFEjected_ArcMain_Idle0, {116,112,115,113}, {156,160}}, //3 idle 4
  {BFEjected_ArcMain_Idle1, {  0,  0,115,113}, {156,160}}, //4 idle 5
  {BFEjected_ArcMain_Idle1, {115,  0,117,113}, {158,160}}, //5 idle 6
  {BFEjected_ArcMain_Idle1, {  0,113,118,112}, {158,159}}, //6 idle 7
  {BFEjected_ArcMain_Idle1, {118,113,118,112}, {159,159}}, //7 idle 8
  {BFEjected_ArcMain_Idle2, {  0,  0,118,113}, {159,160}}, //8 idle 9
  {BFEjected_ArcMain_Idle2, {118,  0,118,112}, {159,159}}, //9 idle 10

  {BFEjected_ArcMain_Left0, {  0,  0,106,118}, {159,159}}, //10 left 1
  {BFEjected_ArcMain_Left0, {106,  0,105,118}, {159,159}}, //11 left 2
  {BFEjected_ArcMain_Left0, {  0,118,106,117}, {157,158}}, //12 left 3
  {BFEjected_ArcMain_Left0, {106,118,108,118}, {159,158}}, //13 left 4
  {BFEjected_ArcMain_Left1, {  0,  0,107,117}, {158,158}}, //14 left 5

  {BFEjected_ArcMain_Down0, {  0,  0,120,108}, {160,151}}, //15 down 1
  {BFEjected_ArcMain_Down0, {120,  0,119,108}, {160,151}}, //16 down 2
  {BFEjected_ArcMain_Down0, {  0,108,116,106}, {159,151}}, //17 down 3
  {BFEjected_ArcMain_Down0, {116,108,118,106}, {161,151}}, //18 down 4
  {BFEjected_ArcMain_Down1, {  0,  0,117,106}, {160,151}}, //19 down 5

  {BFEjected_ArcMain_Up0, {  0,  0, 87,134}, {144,173}}, //20 up 1
  {BFEjected_ArcMain_Up0, { 87,  0, 86,135}, {144,173}}, //21 up 2
  {BFEjected_ArcMain_Up1, {  0,  0, 88,134}, {144,171}}, //22 up 3
  {BFEjected_ArcMain_Up1, { 88,  0, 88,135}, {144,171}}, //23 up 4
  {BFEjected_ArcMain_Up2, {  0,  0, 88,135}, {143,171}}, //24 up 5

  {BFEjected_ArcMain_Right0, {  0,  0,109,107}, {145,158}}, //25 right 1
  {BFEjected_ArcMain_Right0, {109,  0,108,108}, {141,157}}, //26 right 2
  {BFEjected_ArcMain_Right0, {  0,108,108,106}, {144,158}}, //27 right 3
  {BFEjected_ArcMain_Right0, {108,108,108,107}, {145,158}}, //28 right 4

  {BFEjected_ArcMain_LeftM0, {  0,  0,105,116}, {151,158}}, //29 leftm 1
  {BFEjected_ArcMain_LeftM0, {105,  0,108,118}, {153,161}}, //30 leftm 2
  {BFEjected_ArcMain_LeftM0, {  0,118,110,118}, {156,161}}, //31 leftm 3
  {BFEjected_ArcMain_LeftM0, {110,118,106,118}, {152,160}}, //32 leftm 4
  {BFEjected_ArcMain_LeftM1, {  0,  0,109,116}, {156,158}}, //33 leftm 5

  {BFEjected_ArcMain_DownM0, {  0,  0, 91,112}, {149,155}}, //34 downm 1
  {BFEjected_ArcMain_DownM0, { 91,  0, 94,113}, {152,155}}, //35 downm 2
  {BFEjected_ArcMain_DownM0, {  0,113, 95,113}, {152,155}}, //36 downm 3
  {BFEjected_ArcMain_DownM0, { 95,113, 92,113}, {149,155}}, //37 downm 4
  {BFEjected_ArcMain_DownM1, {  0,  0, 96,113}, {152,155}}, //38 downm 5

  {BFEjected_ArcMain_UpM0, {  0,  0, 80,128}, {151,163}}, //39 upm 1
  {BFEjected_ArcMain_UpM0, { 80,  0, 79,128}, {151,163}}, //40 upm 2
  {BFEjected_ArcMain_UpM0, {159,  0, 78,129}, {150,164}}, //41 upm 3
  {BFEjected_ArcMain_UpM1, {  0,  0, 78,129}, {149,164}}, //42 upm 4
  {BFEjected_ArcMain_UpM1, { 78,  0, 78,129}, {150,164}}, //43 upm 5

  {BFEjected_ArcMain_RightM0, {  0,  0,102,112}, {148,162}}, //44 rightm 1
  {BFEjected_ArcMain_RightM0, {102,  0,102,112}, {149,162}}, //45 rightm 2
  {BFEjected_ArcMain_RightM0, {  0,112,103,113}, {148,162}}, //46 rightm 3
  {BFEjected_ArcMain_RightM0, {103,112,102,112}, {148,162}}, //47 rightm 4
  {BFEjected_ArcMain_RightM1, {  0,  0,103,112}, {148,162}}, //48 rightm 5
};

static const Animation char_bfejected_anim[PlayerAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, ASCR_CHGANI, CharAnim_Special1}}, //CharAnim_Idle
	{1, (const u8[]){ 10, 11, 12, 13, 14, 12, 13, 14, 12, 13, 14, 12, 13, 14, 12, 13, 14, 12, ASCR_CHGANI, CharAnim_Special2}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 15, 16, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, ASCR_CHGANI, CharAnim_Special3}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 20, 21, 22, 23, 24, 22, 23, 24, 22, 23, 24, 22, 23, 24, 22, 23, 24, 22, ASCR_CHGANI, PlayerAnim_Peace}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 25, 26, 27, 25, 28, 27, 25, 28, 27, 25, 28, 27, 25, 28, 27, 25, 28, 27, ASCR_CHGANI, PlayerAnim_Sweat}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {1, (const u8[]){ 7, 8, 9, ASCR_CHGANI, CharAnim_Special1}},       //CharAnim_Special1
    {1, (const u8[]){ 12, 13, 14, ASCR_CHGANI, CharAnim_Special2}},       //CharAnim_Special2
    {1, (const u8[]){ 17, 18, 19, ASCR_CHGANI, CharAnim_Special3}},       //CharAnim_Special3
	
	{1, (const u8[]){ 29, 30, 31, 32, 33, 31, 32, 33, 31, 32, 33, 31, 32, 33, 31, 32, 33, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 34, 35, 36, 37, 38, 36, 37, 38, 36, 37, 38, 36, 37, 38, 36, 37, 38, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 39, 40, 41, 42, 43, 41, 42, 43, 41, 42, 43, 41, 42, 43, 41, 42, 43, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 44, 45, 46, 47, 48, 46, 47, 48, 46, 47, 48, 46, 47, 48, 46, 47, 48, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{1, (const u8[]){ 22, 23, 24, ASCR_CHGANI, PlayerAnim_Peace}},       //PlayerAnim_Peace
    {1, (const u8[]){ 27, 25, 28, ASCR_CHGANI, PlayerAnim_Sweat}},       //PlayerAnim_Sweat
};

//Boyfriend Ejected player functions
void Char_BFEjected_SetFrame(void *user, u8 frame)
{
	Char_BFEjected *this = (Char_BFEjected*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfejected_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFEjected_Tick(Character *character)
{
	Char_BFEjected *this = (Char_BFEjected*)character;
	
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFEjected_SetFrame);
	Character_Draw(character, &this->tex, &char_bfejected_frame[this->frame]);
}

void Char_BFEjected_SetAnim(Character *character, u8 anim)
{
	Char_BFEjected *this = (Char_BFEjected*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFEjected_Free(Character *character)
{
	Char_BFEjected *this = (Char_BFEjected*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFEjected_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend ejected object
	Char_BFEjected *this = Mem_Alloc(sizeof(Char_BFEjected));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFEjected_New] Failed to allocate boyfriend ejected object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFEjected_Tick;
	this->character.set_anim = Char_BFEjected_SetAnim;
	this->character.free = Char_BFEjected_Free;
	
	Animatable_Init(&this->character.animatable, char_bfejected_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 2;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-352,1);
	this->character.focus_y = FIXED_DEC(-254,1);
	this->character.focus_zoom = FIXED_DEC(277,512);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFEJECT.ARC;1");
		
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
  "up2.tim",
  "right0.tim",
  "leftm0.tim",
  "leftm1.tim",
  "downm0.tim",
  "downm1.tim",
  "upm0.tim",
  "upm1.tim",
  "rightm0.tim",
  "rightm1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
