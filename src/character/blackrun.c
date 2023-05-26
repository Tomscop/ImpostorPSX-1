/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blackrun.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BlackRun character structure
enum
{
  BlackRun_ArcMain_Idle0,
  BlackRun_ArcMain_Idle1,
  BlackRun_ArcMain_Idle2,
  BlackRun_ArcMain_Idle3,
  BlackRun_ArcMain_Left0,
  BlackRun_ArcMain_Left1,
  BlackRun_ArcMain_Down0,
  BlackRun_ArcMain_Down1,
  BlackRun_ArcMain_Up0,
  BlackRun_ArcMain_Up1,
  BlackRun_ArcMain_Right0,
  BlackRun_ArcMain_Right1,
  BlackRun_ArcMain_Right2,
  BlackRun_ArcMain_Scream0,
  BlackRun_ArcMain_Scream1,
  BlackRun_ArcMain_Scream2,
  BlackRun_ArcMain_Scream3,
  BlackRun_ArcMain_Scream4,
  BlackRun_ArcMain_Scream5,
  BlackRun_ArcMain_Scream6,
  BlackRun_ArcMain_Scream7,
  BlackRun_ArcMain_Scream8,
  BlackRun_ArcMain_Scream9,
  BlackRun_ArcMain_Scream10,
  BlackRun_ArcMain_Scream11,
  BlackRun_ArcMain_Scream12,
	
	BlackRun_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlackRun_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BlackRun;

//BlackRun character definitions
static const CharFrame char_blackrun_frame[] = {
  {BlackRun_ArcMain_Idle0, {  0,  0,147, 82}, {174,210}}, //0 idle 1
  {BlackRun_ArcMain_Idle0, {  0, 82,147, 85}, {174,211}}, //1 idle 2
  {BlackRun_ArcMain_Idle0, {  0,167,145, 86}, {174,212}}, //2 idle 3
  {BlackRun_ArcMain_Idle1, {  0,  0,135, 81}, {170,208}}, //3 idle 4
  {BlackRun_ArcMain_Idle1, {  0, 81,121, 83}, {161,204}}, //4 idle 5
  {BlackRun_ArcMain_Idle1, {121, 81,101, 83}, {149,202}}, //5 idle 6
  {BlackRun_ArcMain_Idle1, {  0,164, 92, 77}, {149,203}}, //6 idle 7
  {BlackRun_ArcMain_Idle1, { 92,164, 92, 75}, {150,205}}, //7 idle 8
  {BlackRun_ArcMain_Idle2, {  0,  0, 93, 80}, {150,210}}, //8 idle 9
  {BlackRun_ArcMain_Idle2, { 93,  0, 93, 80}, {150,210}}, //9 idle 10
  {BlackRun_ArcMain_Idle2, {  0, 80, 95, 83}, {149,212}}, //10 idle 11
  {BlackRun_ArcMain_Idle2, { 95, 80,107, 86}, {149,212}}, //11 idle 12
  {BlackRun_ArcMain_Idle2, {  0,166,118, 87}, {148,210}}, //12 idle 13
  {BlackRun_ArcMain_Idle2, {118,166,124, 88}, {151,208}}, //13 idle 14
  {BlackRun_ArcMain_Idle3, {  0,  0,143, 78}, {170,209}}, //14 idle 15

  {BlackRun_ArcMain_Left0, {  0,  0,132, 83}, {166,202}}, //15 left 1
  {BlackRun_ArcMain_Left0, {  0, 83,130, 82}, {166,201}}, //16 left 2
  {BlackRun_ArcMain_Left0, {  0,166,134, 81}, {166,200}}, //17 left 3
  {BlackRun_ArcMain_Left1, {  0,  0,145, 76}, {171,200}}, //18 left 4
  {BlackRun_ArcMain_Left1, {  0, 76,162, 77}, {177,203}}, //19 left 5

  {BlackRun_ArcMain_Down0, {  0,  0,151, 87}, {143,203}}, //20 down 1
  {BlackRun_ArcMain_Down0, {  0, 87,152, 87}, {143,203}}, //21 down 2
  {BlackRun_ArcMain_Down1, {  0,  0,142, 85}, {144,204}}, //22 down 3
  {BlackRun_ArcMain_Down1, {  0, 85,138, 81}, {146,206}}, //23 down 4
  {BlackRun_ArcMain_Down1, {  0,170,140, 80}, {153,207}}, //24 down 5

  {BlackRun_ArcMain_Up0, {  0,  0,111,115}, {166,225}}, //25 up 1
  {BlackRun_ArcMain_Up0, {111,  0,111,116}, {166,226}}, //26 up 2
  {BlackRun_ArcMain_Up0, {  0,116,113,113}, {166,224}}, //27 up 3
  {BlackRun_ArcMain_Up0, {113,116,126,104}, {166,220}}, //28 up 4
  {BlackRun_ArcMain_Up1, {  0,  0,151, 88}, {168,214}}, //29 up 5

  {BlackRun_ArcMain_Right0, {  0,  0,174, 91}, {185,210}}, //30 right 1
  {BlackRun_ArcMain_Right0, {  0, 91,174, 92}, {186,210}}, //31 right 2
  {BlackRun_ArcMain_Right1, {  0,  0,168, 95}, {182,210}}, //32 right 3
  {BlackRun_ArcMain_Right1, {  0, 95,158, 93}, {177,211}}, //33 right 4
  {BlackRun_ArcMain_Right2, {  0,  0,138, 83}, {169,208}}, //34 right 5

  {BlackRun_ArcMain_Scream0, {  0,  0,124, 92}, {168,218}}, //35 scream 1
  {BlackRun_ArcMain_Scream0, {124,  0,121, 83}, {165,216}}, //36 scream 2
  {BlackRun_ArcMain_Scream0, {  0, 92,122, 89}, {164,215}}, //37 scream 3
  {BlackRun_ArcMain_Scream0, {122, 92,121, 88}, {163,215}}, //38 scream 4
  {BlackRun_ArcMain_Scream1, {  0,  0,121, 83}, {163,215}}, //39 scream 5
  {BlackRun_ArcMain_Scream1, {121,  0,120, 83}, {163,215}}, //40 scream 6
  {BlackRun_ArcMain_Scream1, {  0, 83,121, 83}, {163,215}}, //41 scream 7
  {BlackRun_ArcMain_Scream1, {121, 83,120, 84}, {163,215}}, //42 scream 8
  {BlackRun_ArcMain_Scream1, {  0,167,121, 87}, {163,215}}, //43 scream 9
  {BlackRun_ArcMain_Scream1, {121,167,121, 87}, {163,215}}, //44 scream 10
  {BlackRun_ArcMain_Scream2, {  0,  0,124, 86}, {164,215}}, //45 scream 11
  {BlackRun_ArcMain_Scream2, {124,  0,124, 86}, {164,215}}, //46 scream 12
  {BlackRun_ArcMain_Scream2, {  0, 86,129, 85}, {166,216}}, //47 scream 13
  {BlackRun_ArcMain_Scream3, {  0,  0,129, 85}, {166,216}}, //48 scream 14
  {BlackRun_ArcMain_Scream3, {  0, 85,139, 84}, {168,217}}, //49 scream 15
  {BlackRun_ArcMain_Scream3, {  0,170,139, 83}, {168,217}}, //50 scream 16
  {BlackRun_ArcMain_Scream4, {  0,  0,153, 90}, {172,220}}, //51 scream 17
  {BlackRun_ArcMain_Scream4, {  0, 90,165, 84}, {176,227}}, //52 scream 18
  {BlackRun_ArcMain_Scream5, {  0,  0,130,165}, {165,304}}, //53 scream 19
  {BlackRun_ArcMain_Scream6, {  0,  0,132,135}, {166,272}}, //54 scream 20
  {BlackRun_ArcMain_Scream7, {  0,  0,137,108}, {168,250}}, //55 scream 21
  {BlackRun_ArcMain_Scream7, {  0,108,135,101}, {166,245}}, //56 scream 22
  {BlackRun_ArcMain_Scream8, {  0,  0,129,106}, {165,253}}, //57 scream 23
  {BlackRun_ArcMain_Scream8, {  0,106,128,102}, {165,247}}, //58 scream 24
  {BlackRun_ArcMain_Scream8, {128,106,127,102}, {164,249}}, //59 scream 25
  {BlackRun_ArcMain_Scream9, {  0,  0,127,103}, {165,250}}, //60 scream 26
  {BlackRun_ArcMain_Scream9, {127,  0,128,102}, {164,249}}, //61 scream 27
  {BlackRun_ArcMain_Scream9, {  0,103,127,104}, {163,251}}, //62 scream 28
  {BlackRun_ArcMain_Scream9, {127,103,127,103}, {165,250}}, //63 scream 29
  {BlackRun_ArcMain_Scream10, {  0,  0,128,102}, {165,248}}, //64 scream 30
  {BlackRun_ArcMain_Scream10, {  0,102,128,103}, {164,250}}, //65 scream 31
  {BlackRun_ArcMain_Scream10, {128,102,127,103}, {165,250}}, //66 scream 32
  {BlackRun_ArcMain_Scream11, {  0,  0,128,103}, {165,250}}, //67 scream 33
  {BlackRun_ArcMain_Scream11, {  0,103,128,103}, {164,250}}, //68 scream 34
  {BlackRun_ArcMain_Scream11, {128,103,127,105}, {165,252}}, //69 scream 35
  {BlackRun_ArcMain_Scream12, {  0,  0,127,104}, {165,250}}, //70 scream 36
  {BlackRun_ArcMain_Scream12, {127,  0,127,104}, {164,251}}, //71 scream 37
};

static const Animation char_blackrun_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 15, 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 20, 21, 22, 23, 24, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 25, 26, 27, 28, 29, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 30, 31, 32, 33, 34, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, ASCR_BACK, 1}},         //CharAnim_Special2
};

//BlackRun character functions
void Char_BlackRun_SetFrame(void *user, u8 frame)
{
	Char_BlackRun *this = (Char_BlackRun*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blackrun_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlackRun_Tick(Character *character)
{
	Char_BlackRun *this = (Char_BlackRun*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Danger)
	{
		if (stage.song_step == 1)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(407,1024);
		}
		if (stage.song_beat == 64)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 96)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 128)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 155)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(924,1024);
		}
		if (stage.song_beat == 160)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 192)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 256)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 288)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 320)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(407,1024);
		}
		if (stage.song_beat == 384)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
	}
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2))
	{
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Danger: //AAAAAAAAAAAAAAA
				if (stage.song_step == 624)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 632)
					character->set_anim(character, CharAnim_Special2);
				break;
			default:
				break;
		}
	//switch character at step 639
		
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BlackRun_SetFrame);
	Character_Draw(character, &this->tex, &char_blackrun_frame[this->frame]);
}

void Char_BlackRun_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BlackRun_Free(Character *character)
{
	Char_BlackRun *this = (Char_BlackRun*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlackRun_New(fixed_t x, fixed_t y)
{
	//Allocate blackrun object
	Char_BlackRun *this = Mem_Alloc(sizeof(Char_BlackRun));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlackRun_New] Failed to allocate blackrun object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlackRun_Tick;
	this->character.set_anim = Char_BlackRun_SetAnim;
	this->character.free = Char_BlackRun_Free;
	
	Animatable_Init(&this->character.animatable, char_blackrun_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFF3B1A51;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(924,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(21671,10000);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLACKRUN.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "scream0.tim",
  "scream1.tim",
  "scream2.tim",
  "scream3.tim",
  "scream4.tim",
  "scream5.tim",
  "scream6.tim",
  "scream7.tim",
  "scream8.tim",
  "scream9.tim",
  "scream10.tim",
  "scream11.tim",
  "scream12.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
