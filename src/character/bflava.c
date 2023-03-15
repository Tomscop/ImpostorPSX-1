/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bflava.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend Lava player types
enum
{
  BFLava_ArcMain_Idle0,
  BFLava_ArcMain_Idle1,
  BFLava_ArcMain_Left0,
  BFLava_ArcMain_Down0,
  BFLava_ArcMain_Up0,
  BFLava_ArcMain_Right0,
  BFLava_ArcMain_LeftM0,
  BFLava_ArcMain_DownM0,
  BFLava_ArcMain_UpM0,
  BFLava_ArcMain_RightM0,
	
	BFLava_ArcMain_Max,
};

#define BFLava_Arc_Max BFLava_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFLava_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
} Char_BFLava;

//Boyfriend Lava player definitions
static const CharFrame char_bflava_frame[] = {
  {BFLava_ArcMain_Idle0, {  0,  0, 95,103}, {157,155}}, //0 idle 1
  {BFLava_ArcMain_Idle0, { 95,  0, 95,103}, {158,156}}, //1 idle 2
  {BFLava_ArcMain_Idle0, {  0,103, 93,105}, {157,157}}, //2 idle 3
  {BFLava_ArcMain_Idle0, { 93,103, 94,107}, {158,160}}, //3 idle 4
  {BFLava_ArcMain_Idle1, {  0,  0, 94,107}, {157,160}}, //4 idle 5

  {BFLava_ArcMain_Left0, {  0,  0, 96,105}, {161,157}}, //5 left 1
  {BFLava_ArcMain_Left0, { 96,  0, 93,104}, {157,156}}, //6 left 2

  {BFLava_ArcMain_Down0, {  0,  0, 95, 95}, {154,146}}, //7 down 1
  {BFLava_ArcMain_Down0, { 95,  0, 95, 96}, {155,147}}, //8 down 2

  {BFLava_ArcMain_Up0, {  0,  0, 90,112}, {147,163}}, //9 up 1
  {BFLava_ArcMain_Up0, { 90,  0, 92,111}, {149,162}}, //10 up 2

  {BFLava_ArcMain_Right0, {  0,  0, 88,105}, {146,157}}, //11 right 1
  {BFLava_ArcMain_Right0, { 88,  0, 88,106}, {147,158}}, //12 right 2

  {BFLava_ArcMain_LeftM0, {  0,  0, 94,109}, {157,163}}, //13 leftm 1
  {BFLava_ArcMain_LeftM0, { 94,  0, 94,109}, {157,163}}, //14 leftm 2

  {BFLava_ArcMain_DownM0, {  0,  0, 96,100}, {156,155}}, //15 downm 1
  {BFLava_ArcMain_DownM0, { 96,  0, 96, 99}, {156,154}}, //16 downm 2

  {BFLava_ArcMain_UpM0, {  0,  0, 92,104}, {150,158}}, //17 upm 1
  {BFLava_ArcMain_UpM0, { 92,  0, 91,104}, {149,158}}, //18 upm 2

  {BFLava_ArcMain_RightM0, {  0,  0, 91,107}, {149,161}}, //19 rightm 1
  {BFLava_ArcMain_RightM0, { 91,  0, 93,110}, {149,164}}, //20 rightm 2
};

static const Animation char_bflava_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 6, 6, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 8, 8, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, 10, 10, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 11, 12, 12, 12, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
    
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special1
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special2
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_Special3
	
	{2, (const u8[]){ 13, 14, 14, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 15, 16, 16, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 17, 18, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 19, 20, 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend Lava player functions
void Char_BFLava_SetFrame(void *user, u8 frame)
{
	Char_BFLava *this = (Char_BFLava*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bflava_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFLava_Tick(Character *character)
{
	Char_BFLava *this = (Char_BFLava*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_BoilingPoint)
	{
		if (stage.song_step == 512)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 640)
			this->character.focus_zoom = FIXED_DEC(950,1024);
		if (stage.song_step == 1088)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 1116)
			this->character.focus_zoom = FIXED_DEC(998,1024);
		if (stage.song_step == 1120)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 1132)
			this->character.focus_zoom = FIXED_DEC(998,1024);
		if (stage.song_step == 1136)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 1149)
			this->character.focus_zoom = FIXED_DEC(950,1024);
		if (stage.song_step == 1214)
			this->character.focus_zoom = FIXED_DEC(903,1024);
		if (stage.song_step == 1218)
			this->character.focus_zoom = FIXED_DEC(950,1024);
		if (stage.song_step == 1278)
			this->character.focus_zoom = FIXED_DEC(903,1024);
		if (stage.song_step == 1282)
			this->character.focus_zoom = FIXED_DEC(950,1024);
		if (stage.song_step == 1296)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 1341)
			this->character.focus_zoom = FIXED_DEC(950,1024);
		if (stage.song_step == 1424)
			this->character.focus_zoom = FIXED_DEC(1045,1024);
		if (stage.song_step == 1552)
			this->character.focus_zoom = FIXED_DEC(950,1024);
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BFLava_SetFrame);
	Character_DrawCol(character, &this->tex, &char_bflava_frame[this->frame], 250, 137, 128);
}

void Char_BFLava_SetAnim(Character *character, u8 anim)
{
	Char_BFLava *this = (Char_BFLava*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFLava_Free(Character *character)
{
	Char_BFLava *this = (Char_BFLava*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFLava_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend lava object
	Char_BFLava *this = Mem_Alloc(sizeof(Char_BFLava));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFLava_New] Failed to allocate boyfriend lava object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFLava_Tick;
	this->character.set_anim = Char_BFLava_SetAnim;
	this->character.free = Char_BFLava_Free;
	
	Animatable_Init(&this->character.animatable, char_bflava_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 20;
	
	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-185,1);
	this->character.focus_y = FIXED_DEC(-140,1);
	this->character.focus_zoom = FIXED_DEC(950,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);

	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BFLAVA.ARC;1");
		
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
