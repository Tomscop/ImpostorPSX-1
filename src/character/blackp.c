/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blackp.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BlackP character structure
enum
{
	BlackP_ArcMain_idle0,
	BlackP_ArcMain_idle1,
	BlackP_ArcMain_idle2,
	BlackP_ArcMain_idle3,
	BlackP_ArcMain_idle4,
	BlackP_ArcMain_left0,
	BlackP_ArcMain_left1,
	BlackP_ArcMain_left2,
	BlackP_ArcMain_left3,
	BlackP_ArcMain_down0,
	BlackP_ArcMain_down1,
	BlackP_ArcMain_down2,
	BlackP_ArcMain_down3,
	BlackP_ArcMain_up0,
	BlackP_ArcMain_up1,
	BlackP_ArcMain_up2,
	BlackP_ArcMain_up3,
	BlackP_ArcMain_right0,
	BlackP_ArcMain_right1,
	BlackP_ArcMain_right2,
	BlackP_ArcMain_right3,

	BlackP_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlackP_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BlackP;

//BlackP character definitions
static const CharFrame char_blackp_frame[] = {
	{BlackP_ArcMain_idle0,{0,0,190,123},{110,120}}, //0 Idle
	{BlackP_ArcMain_idle0,{0,124,189,126},{110,123}}, //1 Idle
	{BlackP_ArcMain_idle1,{0,0,186,127},{104,124}}, //2 Idle
	{BlackP_ArcMain_idle1,{0,128,186,127},{100,124}}, //3 Idle
	{BlackP_ArcMain_idle2,{0,0,182,126},{98,123}}, //4 Idle
	{BlackP_ArcMain_idle2,{0,127,182,124},{98,121}}, //5 Idle
	{BlackP_ArcMain_idle3,{0,0,188,122},{104,119}}, //6 Idle
	{BlackP_ArcMain_idle3,{0,123,188,121},{108,118}}, //7 Idle
	{BlackP_ArcMain_idle4,{0,0,188,122},{109,119}}, //8 Idle
	
	{BlackP_ArcMain_left0,{0,0,236,174},{180,168}}, //9 Left
	{BlackP_ArcMain_left1,{0,0,236,150},{174,142}}, //10 Left
	{BlackP_ArcMain_left2,{0,0,230,154},{164,146}}, //11 Left
	{BlackP_ArcMain_left3,{0,0,228,156},{162,149}}, //12 Left
	
	{BlackP_ArcMain_down0,{0,0,256,202},{128,178}}, //13 Down
	{BlackP_ArcMain_down1,{0,0,218,175},{109,165}}, //14 Down
	{BlackP_ArcMain_down2,{0,0,212,164},{105,155}}, //15 Down
	{BlackP_ArcMain_down3,{0,0,208,162},{104,153}}, //16 Down
	
	{BlackP_ArcMain_up0,{0,0,168,202},{85,183}}, //17 Up
	{BlackP_ArcMain_up1,{0,0,158,166},{82,160}}, //18 Up
	{BlackP_ArcMain_up2,{0,0,156,160},{79,153}}, //19 Up
	{BlackP_ArcMain_up3,{0,0,156,158},{79,153}}, //20 Up
	
	{BlackP_ArcMain_right0,{0,0,256,162},{47,135}}, //21 Right
	{BlackP_ArcMain_right1,{0,0,210,194},{41,143}}, //22 Right
	{BlackP_ArcMain_right2,{0,0,184,194},{43,138}}, //23 Right
	{BlackP_ArcMain_right3,{0,0,184,196},{43,140}}, //24 Right
	

};

static const Animation char_blackp_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_Idle
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},		//CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_LeftAlt
	{2, (const u8[]){ 13, 14, 15, 16, ASCR_BACK, 1}},		//CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_DownAlt
	{2, (const u8[]){ 17, 18, 19, 20, ASCR_BACK, 1}},		//CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_UpAlt
	{2, (const u8[]){ 21, 22, 23, 24, ASCR_BACK, 1}},		//CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		//CharAnim_RightAlt

};

//BlackP character functions
void Char_BlackP_SetFrame(void *user, u8 frame)
{
	Char_BlackP *this = (Char_BlackP*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blackp_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlackP_Tick(Character *character)
{
	Char_BlackP *this = (Char_BlackP*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Finale)
	{
		if (stage.song_beat == 32)
		{
			this->character.focus_x = FIXED_DEC(65,1);
			this->character.focus_y = FIXED_DEC(-200,1);
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		}
		if (stage.song_beat == 48)
		{
			this->character.focus_x = FIXED_DEC(65,1);
			this->character.focus_y = FIXED_DEC(-200,1);
			this->character.focus_zoom = FIXED_DEC(1086,1024);
		}
		if (stage.song_beat == 64)
		{
			this->character.focus_x = FIXED_DEC(65,1);
			this->character.focus_y = FIXED_DEC(-200,1);
			this->character.focus_zoom = FIXED_DEC(1628,1024);
		}
		if (stage.song_beat == 67)
		{
			this->character.focus_x = FIXED_DEC(65,1);
			this->character.focus_y = FIXED_DEC(-200,1);
			this->character.focus_zoom = FIXED_DEC(3259,1024);
		}
		if (stage.song_beat == 68) //normal focus
		{
			this->character.focus_x = FIXED_DEC(86,1);
			this->character.focus_y = FIXED_DEC(-152,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 494)
		{
			this->character.focus_x = FIXED_DEC(86,1);
			this->character.focus_y = FIXED_DEC(-152,1);
			this->character.focus_zoom = FIXED_DEC(13,10);
		}
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BlackP_SetFrame);
	Character_DrawCol(character, &this->tex, &char_blackp_frame[this->frame], 200, 128, 128);
}

void Char_BlackP_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BlackP_Free(Character *character)
{
	Char_BlackP *this = (Char_BlackP*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlackP_New(fixed_t x, fixed_t y)
{
	//Allocate blackp object
	Char_BlackP *this = Mem_Alloc(sizeof(Char_BlackP));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlackP_New] Failed to allocate blackp object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlackP_Tick;
	this->character.set_anim = Char_BlackP_SetAnim;
	this->character.free = Char_BlackP_Free;
	
	Animatable_Init(&this->character.animatable, char_blackp_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;
	
	//health bar color
	this->character.health_bar = 0xFFFF1675;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-200,1);
	this->character.focus_zoom = FIXED_DEC(543,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(22738,10000);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLACKP.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
