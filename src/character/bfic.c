/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfic.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend IC player types
enum
{
	BFIC_ArcMain_BF0,
	BFIC_ArcMain_BF1,
	BFIC_ArcMain_BF2,
	BFIC_ArcMain_BF3,
	BFIC_ArcMain_BF5,
	BFIC_ArcMain_BF6,
	BFIC_ArcMain_Idle0,
	BFIC_ArcMain_Idle1,
	BFIC_ArcMain_Idle2,
	BFIC_ArcMain_Left0,
	BFIC_ArcMain_Down0,
	BFIC_ArcMain_Up0,
	BFIC_ArcMain_Up1,
	BFIC_ArcMain_Right0,
	
	BFIC_ArcMain_Max,
};

#define BFIC_Arc_Max BFIC_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFIC_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFIC;

//Boyfriend IC player definitions
static const CharFrame char_bfic_frame[] = {
	{BFIC_ArcMain_BF0, {  0,   0, 102,  99}, { 53,  92}}, //0 idle 1
	{BFIC_ArcMain_BF0, {103,   0, 102,  99}, { 53,  92}}, //1 idle 2
	{BFIC_ArcMain_BF0, {  0, 100, 102, 101}, { 53,  94}}, //2 idle 3
	{BFIC_ArcMain_BF0, {103, 100, 103, 104}, { 53,  97}}, //3 idle 4
	{BFIC_ArcMain_BF1, {  0,   0, 103, 104}, { 53,  97}}, //4 idle 5
	
	{BFIC_ArcMain_BF1, {104,   0,  96, 102}, { 56,  95}}, //5 left 1
	{BFIC_ArcMain_BF1, {  0, 105,  94, 102}, { 54,  95}}, //6 left 2
	
	{BFIC_ArcMain_BF1, { 95, 103,  94,  89}, { 52,  82}}, //7 down 1
	{BFIC_ArcMain_BF2, {  0,   0,  94,  90}, { 52,  83}}, //8 down 2
	
	{BFIC_ArcMain_BF2, { 95,   0,  93, 112}, { 41, 104}}, //9 up 1
	{BFIC_ArcMain_BF2, {  0,  91,  94, 111}, { 42, 103}}, //10 up 2
	
	{BFIC_ArcMain_BF2, { 95, 113, 102, 102}, { 41,  95}}, //11 right 1
	{BFIC_ArcMain_BF3, {  0,   0, 102, 102}, { 41,  95}}, //12 right 2
	
	{BFIC_ArcMain_BF5, {  0,   0,  93, 108}, { 52, 101}}, //13 left miss 1
	{BFIC_ArcMain_BF5, { 94,   0,  93, 108}, { 52, 101}}, //14 left miss 2
	
	{BFIC_ArcMain_BF5, {  0, 109,  95,  98}, { 50,  90}}, //15 down miss 1
	{BFIC_ArcMain_BF5, { 96, 109,  95,  97}, { 50,  89}}, //16 down miss 2
	
	{BFIC_ArcMain_BF6, {  0,   0,  90, 107}, { 44,  99}}, //17 up miss 1
	{BFIC_ArcMain_BF6, { 91,   0,  89, 108}, { 44, 100}}, //18 up miss 2
	
	{BFIC_ArcMain_BF6, {  0, 108,  99, 108}, { 42, 101}}, //19 right miss 1
	{BFIC_ArcMain_BF6, {100, 109, 101, 108}, { 43, 101}}, //20 right miss 2
	
  {BFIC_ArcMain_Idle0, {  0,  0,118,111}, {158,157}}, //21 idle 1
  {BFIC_ArcMain_Idle0, {118,  0,116,112}, {157,159}}, //22 idle 2
  {BFIC_ArcMain_Idle0, {  0,112,116,112}, {157,159}}, //23 idle 3
  {BFIC_ArcMain_Idle0, {116,112,115,113}, {156,160}}, //24 idle 4
  {BFIC_ArcMain_Idle1, {  0,  0,115,113}, {156,160}}, //25 idle 5
  {BFIC_ArcMain_Idle1, {115,  0,117,113}, {158,160}}, //26 idle 6
  {BFIC_ArcMain_Idle1, {  0,113,118,112}, {158,159}}, //27 idle 7
  {BFIC_ArcMain_Idle1, {118,113,118,112}, {159,159}}, //28 idle 8
  {BFIC_ArcMain_Idle2, {  0,  0,118,113}, {159,160}}, //29 idle 9
  {BFIC_ArcMain_Idle2, {118,  0,118,112}, {159,159}}, //30 idle 10

  {BFIC_ArcMain_Left0, {  0,  0,106,118}, {159,159}}, //31 left 1
  {BFIC_ArcMain_Left0, {106,  0,105,118}, {159,159}}, //32 left 2
  {BFIC_ArcMain_Left0, {  0,118,106,117}, {157,158}}, //33 left 3
  {BFIC_ArcMain_Left0, {106,118,108,118}, {159,158}}, //34 left 4

  {BFIC_ArcMain_Down0, {  0,  0,120,108}, {160,151}}, //35 down 1
  {BFIC_ArcMain_Down0, {120,  0,119,108}, {160,151}}, //36 down 2
  {BFIC_ArcMain_Down0, {  0,108,116,106}, {159,151}}, //37 down 3
  {BFIC_ArcMain_Down0, {116,108,118,106}, {161,151}}, //38 down 4

  {BFIC_ArcMain_Up0, {  0,  0, 87,134}, {144,173}}, //39 up 1
  {BFIC_ArcMain_Up0, { 87,  0, 86,135}, {144,173}}, //40 up 2
  {BFIC_ArcMain_Up1, {  0,  0, 88,134}, {144,171}}, //41 up 3
  {BFIC_ArcMain_Up1, { 88,  0, 88,135}, {144,171}}, //42 up 4

  {BFIC_ArcMain_Right0, {  0,  0,109,107}, {145,158}}, //43 right 1
  {BFIC_ArcMain_Right0, {109,  0,108,108}, {141,157}}, //44 right 2
  {BFIC_ArcMain_Right0, {  0,108,108,106}, {144,158}}, //45 right 3
  {BFIC_ArcMain_Right0, {108,108,108,107}, {145,158}}, //46 right 4
};

static const Animation char_bfic_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{1, (const u8[]){ 13, 13, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 15, 15, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 17, 17, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 19, 19, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

static const Animation char_bfic_anim2[PlayerAnim_Max] = {
	{1, (const u8[]){ 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 28, 29, 30, 28, 29, 30, 28, 29, ASCR_CHGANI, CharAnim_Special1}}, //CharAnim_Idle
	{1, (const u8[]){ 31, 32, 33, 34, 32, 33, 34, 32, 33, 34, 32, 33, 34, 32, 33, 34, 32, 33, ASCR_CHGANI, CharAnim_Special2}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{1, (const u8[]){ 35, 36, 37, 36, 39, 37, 36, 39, 37, 36, 39, 37, 36, 39, 37, 36, 39, 37, ASCR_CHGANI, CharAnim_Special3}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{1, (const u8[]){ 39, 40, 41, 42, 40, 41, 42, 40, 41, 42, 40, 41, 42, 40, 41, 42, 40, 41, ASCR_CHGANI, PlayerAnim_Peace}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{1, (const u8[]){ 43, 44, 45, 43, 46, 45, 43, 46, 45, 43, 46, 45, 43, 46, 45, 43, 46, 45, ASCR_CHGANI, PlayerAnim_Sweat}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {1, (const u8[]){ 28, 29, 30, ASCR_CHGANI, CharAnim_Special1}},       //CharAnim_Special1
    {1, (const u8[]){ 33, 34, 32, ASCR_CHGANI, CharAnim_Special2}},       //CharAnim_Special2
    {1, (const u8[]){ 37, 36, 39, ASCR_CHGANI, CharAnim_Special3}},       //CharAnim_Special3
	
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_LeftMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_DownMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_UpMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_RightMiss
	
	{1, (const u8[]){ 41, 42, 40, ASCR_CHGANI, PlayerAnim_Peace}},       //PlayerAnim_Peace
	{1, (const u8[]){ 45, 43, 46, ASCR_CHGANI, PlayerAnim_Sweat}},       //PlayerAnim_Sweat
};

//Boyfriend IC player functions
void Char_BFIC_SetFrame(void *user, u8 frame)
{
	Char_BFIC *this = (Char_BFIC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfic_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFIC_Tick(Character *character)
{
	Char_BFIC *this = (Char_BFIC*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_IdentityCrisis)
	{
		
		
		//Switches
		if ((stage.song_step == -29) || (stage.song_step == 1500) || (stage.song_step == 3072) || (stage.song_step == 3296)) //normal
			Animatable_Init(&this->character.animatable, char_bfic_anim);
		if ((stage.song_step == 1168) || (stage.song_step == 2878) || (stage.song_step == 3280)) //fly
			Animatable_Init(&this->character.animatable, char_bfic_anim2);
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFIC_SetFrame);
	Character_Draw(character, &this->tex, &char_bfic_frame[this->frame]);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_IdentityCrisis: //switches
				if (stage.song_step == 1168)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 1500)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 2878)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 3072)
					character->set_anim(character, CharAnim_Left);
				if (stage.song_step == 3280)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 3296)
					character->set_anim(character, CharAnim_Up);
				break;
			default:
				break;
		}
}

void Char_BFIC_SetAnim(Character *character, u8 anim)
{
	Char_BFIC *this = (Char_BFIC*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFIC_Free(Character *character)
{
	Char_BFIC *this = (Char_BFIC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFIC_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend ic object
	Char_BFIC *this = Mem_Alloc(sizeof(Char_BFIC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFIC_New] Failed to allocate boyfriend ic object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFIC_Tick;
	this->character.set_anim = Char_BFIC_SetAnim;
	this->character.free = Char_BFIC_Free;
	
	Animatable_Init(&this->character.animatable, char_bfic_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-352,1);
	this->character.focus_y = FIXED_DEC(-254,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFIC.ARC;1");
		
	const char **pathp = (const char *[]){
		"bf0.tim",   //BF_ArcMain_BF0
		"bf1.tim",   //BF_ArcMain_BF1
		"bf2.tim",   //BF_ArcMain_BF2
		"bf3.tim",   //BF_ArcMain_BF3
		"bf5.tim",   //BF_ArcMain_BF5
		"bf6.tim",   //BF_ArcMain_BF6
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
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
