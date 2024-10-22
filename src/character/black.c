/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "black.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Black character structure
enum
{
  Black_ArcMain_Idle0,
  Black_ArcMain_Idle1,
  Black_ArcMain_Idle2,
  Black_ArcMain_Idle3,
  Black_ArcMain_Idle4,
  Black_ArcMain_Idle5,
  Black_ArcMain_Left0,
  Black_ArcMain_Left1,
  Black_ArcMain_Left2,
  Black_ArcMain_Down0,
  Black_ArcMain_Down1,
  Black_ArcMain_Down3,
  Black_ArcMain_Down4,
  Black_ArcMain_Up0,
  Black_ArcMain_Up1,
  Black_ArcMain_Up2,
  Black_ArcMain_Right0,
  Black_ArcMain_Right1,
  Black_ArcMain_Right2,
  Black_ArcMain_IdleA0,
  Black_ArcMain_IdleA1,
  Black_ArcMain_IdleA2,
  Black_ArcMain_IdleA3,
  Black_ArcMain_LeftA0,
  Black_ArcMain_LeftA1,
  Black_ArcMain_DownA0,
  Black_ArcMain_DownA1,
  Black_ArcMain_UpA0,
  Black_ArcMain_UpA1,
  Black_ArcMain_RightA0,
  Black_ArcMain_RightA1,
	
	Black_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Black_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Black;

//Black character definitions
static const CharFrame char_black_frame[] = {
  {Black_ArcMain_Idle0, {  0,  0,100, 99}, {214,155}}, //0 idle 1
  {Black_ArcMain_Idle0, {101,  0, 99, 98}, {213,154}}, //1 idle 2
  {Black_ArcMain_Idle0, {  0,100,100, 98}, {214,154}}, //2 idle 3
  {Black_ArcMain_Idle0, {101,100,100, 97}, {214,153}}, //3 idle 4
  {Black_ArcMain_Idle1, {  0,  0,100, 96}, {212,152}}, //4 idle 5
  {Black_ArcMain_Idle1, {101,  0,100, 97}, {212,153}}, //5 idle 6
  {Black_ArcMain_Idle1, {  0, 98,100, 96}, {212,152}}, //6 idle 7
  {Black_ArcMain_Idle1, {101, 98,100, 96}, {212,152}}, //7 idle 8
  {Black_ArcMain_Idle2, {  0,  0,100, 96}, {211,152}}, //8 idle 9
  {Black_ArcMain_Idle2, {101,  0,100, 97}, {211,153}}, //9 idle 10
  {Black_ArcMain_Idle2, {  0, 98,100, 98}, {210,154}}, //10 idle 11
  {Black_ArcMain_Idle2, {101, 98,100, 99}, {210,155}}, //11 idle 12
  {Black_ArcMain_Idle3, {  0,  0, 99,100}, {210,156}}, //12 idle 13
  {Black_ArcMain_Idle3, {100,  0, 99,101}, {210,157}}, //13 idle 14
  {Black_ArcMain_Idle3, {  0,102, 99,101}, {210,157}}, //14 idle 15
  {Black_ArcMain_Idle3, {100,102, 98,101}, {211,157}}, //15 idle 16
  {Black_ArcMain_Idle4, {  0,  0, 98,102}, {211,158}}, //16 idle 17
  {Black_ArcMain_Idle4, { 99,  0, 99,102}, {212,158}}, //17 idle 18
  {Black_ArcMain_Idle4, {  0,103, 99,102}, {212,158}}, //18 idle 19
  {Black_ArcMain_Idle4, {100,103, 99,102}, {212,158}}, //19 idle 20
  {Black_ArcMain_Idle5, {  0,  0, 99,102}, {213,158}}, //20 idle 21
  {Black_ArcMain_Idle5, {100,  0, 99,102}, {213,158}}, //21 idle 22
  {Black_ArcMain_Idle5, {  0,103, 99,101}, {213,157}}, //22 idle 23
  {Black_ArcMain_Idle5, {100,103, 99,101}, {212,157}}, //23 idle 24
  {Black_ArcMain_Idle5, {100,103, 99,101}, {212,157}}, //24 idle 25

  {Black_ArcMain_Left0, {  0,  0,148, 96}, {245,149}}, //25 left 1
  {Black_ArcMain_Left0, {  0, 96,149, 89}, {245,142}}, //26 left 2
  {Black_ArcMain_Left1, {  0,  0,148, 88}, {244,141}}, //27 left 3
  {Black_ArcMain_Left1, {  0, 88,145, 87}, {244,140}}, //28 left 4
  {Black_ArcMain_Left2, {  0,  0,132, 87}, {247,140}}, //29 left 5
  {Black_ArcMain_Left2, {132,  0, 98, 90}, {217,146}}, //30 left 6

  {Black_ArcMain_Down0, {  0,  0,139,131}, {237,185}}, //31 down 1
  {Black_ArcMain_Down1, {  0,  0,143,138}, {239,193}}, //32 down 2
  {Black_ArcMain_Down1, {  0,  0,143,138}, {239,193}}, //33 down 3
  {Black_ArcMain_Down3, {  0,  0,142,142}, {239,193}}, //34 down 4
  {Black_ArcMain_Down4, {  0,  0,122,125}, {228,179}}, //35 down 5
  {Black_ArcMain_Down4, {122,  0, 85,109}, {210,164}}, //36 down 6
  {Black_ArcMain_Down4, {  0,125, 75, 98}, {202,151}}, //37 down 7

  {Black_ArcMain_Up0, {  0,  0,155,109}, {253,150}}, //38 up 1
  {Black_ArcMain_Up0, {  0,109,158,111}, {251,150}}, //39 up 2
  {Black_ArcMain_Up1, {  0,  0,154,115}, {250,154}}, //40 up 3
  {Black_ArcMain_Up1, {  0,115,141,111}, {251,153}}, //41 up 4
  {Black_ArcMain_Up2, {  0,  0,153,105}, {253,152}}, //42 up 5
  {Black_ArcMain_Up2, {  0,105,162, 89}, {262,144}}, //43 up 6

  {Black_ArcMain_Right0, {  0,  0,172,102}, {211,150}}, //44 right 1
  {Black_ArcMain_Right0, {  0,102,173, 99}, {212,149}}, //45 right 2
  {Black_ArcMain_Right1, {  0,  0,172, 97}, {212,147}}, //46 right 3
  {Black_ArcMain_Right1, {  0, 97,171, 97}, {210,145}}, //47 right 4
  {Black_ArcMain_Right2, {  0,  0,143,101}, {192,144}}, //48 right 5
  {Black_ArcMain_Right2, {144,  0, 82,113}, {196,153}}, //49 right 6
  
	{Black_ArcMain_IdleA0, {0,   0, 255, 208}, {128+320, 210+97}}, //50 idle a 1
	{Black_ArcMain_IdleA1, {0,   0, 211, 199}, {97+320, 201+97}}, //51 idle a 2
	{Black_ArcMain_IdleA2, {0,   0, 239, 201}, {114+320, 202+97}}, //52 idle a 3
	{Black_ArcMain_IdleA3, {0,   0, 254, 207}, {127+320, 208+97}}, //53 idle a 4

	{Black_ArcMain_LeftA0, {0,   0, 253, 210}, {148+320, 211+97}}, //54 left a 1
	{Black_ArcMain_LeftA1, {0,   0, 255, 204}, {165+320, 206+97}}, //55 left a 2

	{Black_ArcMain_DownA0, {0,   0, 255, 202}, {122+320, 204+97}}, //56 down a 1
	{Black_ArcMain_DownA1, {0,   0, 255, 190}, {126+320, 192+97}}, //57 down a 2

	{Black_ArcMain_UpA0, {0,   0, 250, 240}, {122+320, 241+97}}, //58 up a 1
	{Black_ArcMain_UpA1, {0,   0, 255, 186}, {131+320, 187+97}}, //59 up a 2

	{Black_ArcMain_RightA0, {0,   0, 196, 193}, {59+320, 194+97}}, //60 right a 1
	{Black_ArcMain_RightA1, {0,   0, 185, 203}, {50+320, 204+97}}, //61 right a 2
};

static const Animation char_black_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 25, 26, 27, 28, 29, 30, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 31, 32, 33, 34, 35, 36, 37, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 38, 39, 40, 41, 42, 43, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 44, 45, 46, 47, 48, 49, ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_black_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 50,  51,  52,  53, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 54,  55, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 56,  57, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 58,  59, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){60, 61, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Black character functions
void Char_Black_SetFrame(void *user, u8 frame)
{
	Char_Black *this = (Char_Black*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_black_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Black_Tick(Character *character)
{
	Char_Black *this = (Char_Black*)character;
	
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
		if (stage.song_step == 400)
		{
			this->character.focus_x = FIXED_DEC(-211,1);
			this->character.focus_zoom = FIXED_DEC(427,512);
		}
		if (stage.song_step == 656)
		{
			this->character.focus_x = FIXED_DEC(-179,1);
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
			this->character.focus_x = FIXED_DEC(-256,1);
			this->character.focus_y = FIXED_DEC(-184,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
			Animatable_Init(&this->character.animatable, char_black_anim2);
			character->set_anim(character, CharAnim_Up);
			this->character.health_i = 9;
			this->character.health_bar = 0xFFFF0000;
			this->character.size = FIXED_DEC(1,1);
		}
		if (stage.song_step == 1440)
		{
			this->character.focus_x = FIXED_DEC(-211,1);
			this->character.focus_y = FIXED_DEC(-178,1);
			this->character.focus_zoom = FIXED_DEC(427,512);
			Animatable_Init(&this->character.animatable, char_black_anim);
			this->character.health_i = 6;
			this->character.health_bar = 0xFFD70009;
			this->character.size = FIXED_DEC(166,100);
		}
		if (stage.song_step == 1696)
		{
			this->character.focus_x = FIXED_DEC(-179,1);
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
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Black_SetFrame);
	if ((stage.stage_id == StageId_Defeat) && ((stage.song_step <= 1167) || (stage.song_step >= 1440)))
		Character_DrawCol(character, &this->tex, &char_black_frame[this->frame], 200, 128, 128);
	else if (stage.stage_id == StageId_Defeat)
		Character_Draw(character, &this->tex, &char_black_frame[this->frame]);
	
	if ((stage.stage_id == StageId_Finale) && (stage.song_step <= 271))
		Character_DrawCol(character, &this->tex, &char_black_frame[this->frame], 200, 128, 128);
}

void Char_Black_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Black_Free(Character *character)
{
	Char_Black *this = (Char_Black*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Black_New(fixed_t x, fixed_t y)
{
	//Allocate black object
	Char_Black *this = Mem_Alloc(sizeof(Char_Black));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Black_New] Failed to allocate black object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Black_Tick;
	this->character.set_anim = Char_Black_SetAnim;
	this->character.free = Char_Black_Free;
	
	Animatable_Init(&this->character.animatable, char_black_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFFD70009;
	
	if (stage.stage_id == StageId_Defeat)
	{
		this->character.focus_x = FIXED_DEC(-179,1);
		this->character.focus_y = FIXED_DEC(-178,1);
		this->character.focus_zoom = FIXED_DEC(417,512);
	}
	else
	{
		this->character.focus_x = FIXED_DEC(65,1);
		this->character.focus_y = FIXED_DEC(-115,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(166,100);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLACK.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "down3.tim",
  "down4.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
	"idlea0.tim",
	"idlea1.tim",
	"idlea2.tim",
	"idlea3.tim",
	"lefta0.tim",
	"lefta1.tim",
	"downa0.tim",
	"downa1.tim",
	"upa0.tim",
	"upa1.tim",
	"righta0.tim",
	"righta1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
