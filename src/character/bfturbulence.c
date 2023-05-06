/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfturbulence.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Turbulence player types
enum
{
  BFTurbulence_ArcMain_Idle0,
  BFTurbulence_ArcMain_Idle1,
  BFTurbulence_ArcMain_Left0,
  BFTurbulence_ArcMain_Down0,
  BFTurbulence_ArcMain_Up0,
  BFTurbulence_ArcMain_Right0,
  BFTurbulence_ArcMain_LeftM0,
  BFTurbulence_ArcMain_DownM0,
  BFTurbulence_ArcMain_UpM0,
  BFTurbulence_ArcMain_RightM0,
  BFTurbulence_ArcMain_Peace0,
	
	BFTurbulence_ArcMain_Max,
};

#define BFTurbulence_Arc_Max BFTurbulence_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFTurbulence_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFTurbulence;

//Boyfriend Turbulence player definitions
static const CharFrame char_bfturbulence_frame[] = {
  {BFTurbulence_ArcMain_Idle0, {  0,  0,103, 95}, {158,155}}, //0 idle 1
  {BFTurbulence_ArcMain_Idle0, {103,  0,103, 97}, {158,157}}, //1 idle 2
  {BFTurbulence_ArcMain_Idle0, {  0, 97,101,102}, {156,159}}, //2 idle 3
  {BFTurbulence_ArcMain_Idle0, {101, 97,104,103}, {159,160}}, //3 idle 4
  {BFTurbulence_ArcMain_Idle1, {  0,  0,104,103}, {159,160}}, //4 idle 5

  {BFTurbulence_ArcMain_Left0, {  0,  0,102, 96}, {152,155}}, //5 left 1
  {BFTurbulence_ArcMain_Left0, {102,  0,102, 95}, {153,156}}, //6 left 2

  {BFTurbulence_ArcMain_Down0, {  0,  0, 90, 94}, {140,155}}, //7 down 1
  {BFTurbulence_ArcMain_Down0, { 90,  0, 91, 94}, {142,155}}, //8 down 2

  {BFTurbulence_ArcMain_Up0, {  0,  0,113, 93}, {163,160}}, //9 up 1
  {BFTurbulence_ArcMain_Up0, {113,  0,111, 95}, {161,161}}, //10 up 2

  {BFTurbulence_ArcMain_Right0, {  0,  0,102,103}, {153,167}}, //11 right 1
  {BFTurbulence_ArcMain_Right0, {102,  0,102,103}, {154,167}}, //12 right 2

  {BFTurbulence_ArcMain_LeftM0, {  0,  0,108, 93}, {161,156}}, //13 leftm 1
  {BFTurbulence_ArcMain_LeftM0, {108,  0,108, 93}, {160,156}}, //14 leftm 2

  {BFTurbulence_ArcMain_DownM0, {  0,  0, 98, 95}, {150,156}}, //15 downm 1
  {BFTurbulence_ArcMain_DownM0, { 98,  0, 97, 95}, {149,156}}, //16 downm 2

  {BFTurbulence_ArcMain_UpM0, {  0,  0,107, 91}, {159,158}}, //17 upm 1
  {BFTurbulence_ArcMain_UpM0, {107,  0,107, 90}, {159,157}}, //18 upm 2

  {BFTurbulence_ArcMain_RightM0, {  0,  0,108, 99}, {160,165}}, //19 rightm 1
  {BFTurbulence_ArcMain_RightM0, {108,  0,106,101}, {157,167}}, //20 rightm 2

  {BFTurbulence_ArcMain_Peace0, {  0,  0,105, 99}, {158,155}}, //21 peace 1
  {BFTurbulence_ArcMain_Peace0, {105,  0,104,104}, {156,160}}, //22 peace 2
  {BFTurbulence_ArcMain_Peace0, {  0,104,104,104}, {156,160}}, //23 peace 3
};

static const Animation char_bfturbulence_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {2, (const u8[]){ 21, 22, 23, ASCR_BACK, 1}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 15, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 17, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 19, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Turbulence player functions
void Char_BFTurbulence_SetFrame(void *user, u8 frame)
{
	Char_BFTurbulence *this = (Char_BFTurbulence*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfturbulence_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFTurbulence_Tick(Character *character)
{
	Char_BFTurbulence *this = (Char_BFTurbulence*)character;
	
	//NOTE FOR WHEN I COME BACK ADD THE PEACE ANIM THING AT THE END
	
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
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Turbulence: //Lmao get rekt
				if (stage.song_step == 1268)
					character->set_anim(character, CharAnim_Special1);
				break;
			default:
				break;
		}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFTurbulence_SetFrame);
	Character_Draw(character, &this->tex, &char_bfturbulence_frame[this->frame]);
}

void Char_BFTurbulence_SetAnim(Character *character, u8 anim)
{
	Char_BFTurbulence *this = (Char_BFTurbulence*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFTurbulence_Free(Character *character)
{
	Char_BFTurbulence *this = (Char_BFTurbulence*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFTurbulence_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend reactor object
	Char_BFTurbulence *this = Mem_Alloc(sizeof(Char_BFTurbulence));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFTurbulence_New] Failed to allocate boyfriend reactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFTurbulence_Tick;
	this->character.set_anim = Char_BFTurbulence_SetAnim;
	this->character.free = Char_BFTurbulence_Free;
	
	Animatable_Init(&this->character.animatable, char_bfturbulence_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	
	this->character.focus_x = FIXED_DEC(-254,1);
	this->character.focus_y = FIXED_DEC(-171,1);
	this->character.focus_zoom = FIXED_DEC(700,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(13,10);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFTURB.ARC;1");
		
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
  "peace0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
