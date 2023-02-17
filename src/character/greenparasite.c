/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "greenparasite.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GreenParasite character structure
enum
{
  GreenParasite_ArcMain_Idle0,
  GreenParasite_ArcMain_Idle1,
  GreenParasite_ArcMain_Idle2,
  GreenParasite_ArcMain_Idle3,
  GreenParasite_ArcMain_Idle4,
  GreenParasite_ArcMain_Idle5,
  GreenParasite_ArcMain_Idle6,
  GreenParasite_ArcMain_Idle7,
  GreenParasite_ArcMain_Idle8,
  GreenParasite_ArcMain_Idle9,
  GreenParasite_ArcMain_Idle10,
  GreenParasite_ArcMain_Idle11,
  GreenParasite_ArcMain_Idle12,
  GreenParasite_ArcMain_Idle13,
  GreenParasite_ArcMain_Left0,
  GreenParasite_ArcMain_Left1,
  GreenParasite_ArcMain_Left2,
  GreenParasite_ArcMain_Left3,
  GreenParasite_ArcMain_Left4,
  GreenParasite_ArcMain_Down0,
  GreenParasite_ArcMain_Down1,
  GreenParasite_ArcMain_Down2,
  GreenParasite_ArcMain_Down3,
  GreenParasite_ArcMain_Up0,
  GreenParasite_ArcMain_Up1,
  GreenParasite_ArcMain_Up2,
  GreenParasite_ArcMain_Up3,
  GreenParasite_ArcMain_Right0,
  GreenParasite_ArcMain_Right1,
  GreenParasite_ArcMain_Right2,
  GreenParasite_ArcMain_Right3,
	
	GreenParasite_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GreenParasite_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GreenParasite;

//GreenParasite character definitions
static const CharFrame char_greenparasite_frame[] = {
  {GreenParasite_ArcMain_Idle0, {  0,  0,133,109}, {206,169}}, //0 idle 1
  {GreenParasite_ArcMain_Idle0, {  0,109,134,108}, {206,168}}, //1 idle 2
  {GreenParasite_ArcMain_Idle1, {  0,  0,135,107}, {207,167}}, //2 idle 3
  {GreenParasite_ArcMain_Idle1, {  0,107,135,108}, {207,168}}, //3 idle 4
  {GreenParasite_ArcMain_Idle2, {  0,  0,142,106}, {211,167}}, //4 idle 5
  {GreenParasite_ArcMain_Idle2, {  0,106,142,107}, {211,167}}, //5 idle 6
  {GreenParasite_ArcMain_Idle3, {  0,  0,137,105}, {206,166}}, //6 idle 7
  {GreenParasite_ArcMain_Idle3, {  0,105,137,106}, {206,166}}, //7 idle 8
  {GreenParasite_ArcMain_Idle4, {  0,  0,125,105}, {201,166}}, //8 idle 9
  {GreenParasite_ArcMain_Idle4, {125,  0,125,106}, {202,166}}, //9 idle 10
  {GreenParasite_ArcMain_Idle4, {  0,106,120,106}, {200,167}}, //10 idle 11
  {GreenParasite_ArcMain_Idle4, {120,106,120,107}, {200,167}}, //11 idle 12
  {GreenParasite_ArcMain_Idle5, {  0,  0,120,106}, {201,167}}, //12 idle 13
  {GreenParasite_ArcMain_Idle5, {120,  0,121,106}, {202,166}}, //13 idle 14
  {GreenParasite_ArcMain_Idle5, {  0,106,126,107}, {206,167}}, //14 idle 15
  {GreenParasite_ArcMain_Idle5, {126,106,126,108}, {206,168}}, //15 idle 16
  {GreenParasite_ArcMain_Idle6, {  0,  0,134,108}, {210,168}}, //16 idle 17
  {GreenParasite_ArcMain_Idle6, {  0,108,134,109}, {210,169}}, //17 idle 18
  {GreenParasite_ArcMain_Idle7, {  0,  0,135,109}, {206,169}}, //18 idle 19
  {GreenParasite_ArcMain_Idle7, {  0,109,135,110}, {206,170}}, //19 idle 20
  {GreenParasite_ArcMain_Idle8, {  0,  0,133,109}, {206,169}}, //20 idle 21
  {GreenParasite_ArcMain_Idle8, {  0,109,133,109}, {206,169}}, //21 idle 22
  {GreenParasite_ArcMain_Idle9, {  0,  0,134,107}, {210,168}}, //22 idle 23
  {GreenParasite_ArcMain_Idle9, {  0,107,134,108}, {210,169}}, //23 idle 24
  {GreenParasite_ArcMain_Idle9, {134,107,120,107}, {201,168}}, //24 idle 25
  {GreenParasite_ArcMain_Idle10, {  0,  0,121,107}, {201,168}}, //25 idle 26
  {GreenParasite_ArcMain_Idle10, {121,  0,119,106}, {200,167}}, //26 idle 27
  {GreenParasite_ArcMain_Idle10, {  0,107,119,106}, {200,167}}, //27 idle 28
  {GreenParasite_ArcMain_Idle10, {119,107,119,106}, {200,167}}, //28 idle 29
  {GreenParasite_ArcMain_Idle11, {  0,  0,119,106}, {201,167}}, //29 idle 30
  {GreenParasite_ArcMain_Idle11, {119,  0,124,106}, {201,167}}, //30 idle 31
  {GreenParasite_ArcMain_Idle11, {  0,106,125,106}, {201,167}}, //31 idle 32
  {GreenParasite_ArcMain_Idle12, {  0,  0,137,108}, {205,169}}, //32 idle 33
  {GreenParasite_ArcMain_Idle12, {  0,108,137,107}, {205,168}}, //33 idle 34
  {GreenParasite_ArcMain_Idle13, {  0,  0,139,108}, {210,169}}, //34 idle 35
  {GreenParasite_ArcMain_Idle13, {  0,108,139,108}, {211,168}}, //35 idle 36

  {GreenParasite_ArcMain_Left0, {  0,  0,133,110}, {225-5,162+5}}, //36 left 1
  {GreenParasite_ArcMain_Left0, {  0,110,133,110}, {225-5,162+5}}, //37 left 2
  {GreenParasite_ArcMain_Left1, {  0,  0,133,109}, {224-5,160+5}}, //38 left 3
  {GreenParasite_ArcMain_Left1, {  0,109,133,109}, {224-5,160+5}}, //39 left 4
  {GreenParasite_ArcMain_Left2, {  0,  0,133,108}, {223-5,159+5}}, //40 left 5
  {GreenParasite_ArcMain_Left2, {  0,108,133,109}, {223-5,160+5}}, //41 left 6
  {GreenParasite_ArcMain_Left3, {  0,  0,128,108}, {219-5,159+5}}, //42 left 7
  {GreenParasite_ArcMain_Left3, {128,  0,127,109}, {218-5,160+5}}, //43 left 8
  {GreenParasite_ArcMain_Left3, {  0,109,127,109}, {218-5,160+5}}, //44 left 9
  {GreenParasite_ArcMain_Left3, {127,109,120,113}, {216-5,159+5}}, //45 left 10
  {GreenParasite_ArcMain_Left4, {  0,  0,120,113}, {216-5,159+5}}, //46 left 11
  {GreenParasite_ArcMain_Left4, {120,  0,120,114}, {216-5,160+5}}, //47 left 12
  {GreenParasite_ArcMain_Left4, {  0,114,120,114}, {216-5,160+5}}, //48 left 13
  {GreenParasite_ArcMain_Left4, {120,114,120,114}, {216-5,160+5}}, //49 left 14

  {GreenParasite_ArcMain_Down0, {  0,  0,116, 94}, {183+9,137+15}}, //50 down 1
  {GreenParasite_ArcMain_Down0, {116,  0,115, 94}, {183+9,137+15}}, //51 down 2
  {GreenParasite_ArcMain_Down0, {  0, 94,115, 95}, {184+9,140+15}}, //52 down 3
  {GreenParasite_ArcMain_Down0, {115, 94,115, 95}, {183+9,140+15}}, //53 down 4
  {GreenParasite_ArcMain_Down1, {  0,  0,116, 95}, {185+9,141+15}}, //54 down 5
  {GreenParasite_ArcMain_Down1, {116,  0,115, 95}, {185+9,141+15}}, //55 down 6
  {GreenParasite_ArcMain_Down1, {  0, 95,116, 95}, {186+9,142+15}}, //56 down 7
  {GreenParasite_ArcMain_Down1, {116, 95,117, 94}, {187+9,141+15}}, //57 down 8
  {GreenParasite_ArcMain_Down2, {  0,  0,120, 95}, {189+9,142+15}}, //58 down 9
  {GreenParasite_ArcMain_Down2, {120,  0,120, 95}, {189+9,142+15}}, //59 down 10
  {GreenParasite_ArcMain_Down2, {  0, 95,116, 95}, {186+9,142+15}}, //60 down 11
  {GreenParasite_ArcMain_Down2, {116, 95,115, 95}, {186+9,142+15}}, //61 down 12
  {GreenParasite_ArcMain_Down3, {  0,  0,114, 95}, {185+9,142+15}}, //62 down 13
  {GreenParasite_ArcMain_Down3, {114,  0,114, 95}, {184+9,141+15}}, //63 down 14

  {GreenParasite_ArcMain_Up0, {  0,  0,103,113}, {187+9,175-3}}, //64 up 1
  {GreenParasite_ArcMain_Up0, {103,  0,103,116}, {187+9,178-3}}, //65 up 2
  {GreenParasite_ArcMain_Up0, {  0,116,103,111}, {187+9,173-3}}, //66 up 3
  {GreenParasite_ArcMain_Up0, {103,116,103,111}, {187+9,173-3}}, //67 up 4
  {GreenParasite_ArcMain_Up1, {  0,  0,102,107}, {186+9,170-3}}, //68 up 5
  {GreenParasite_ArcMain_Up1, {102,  0,103,107}, {187+9,170-3}}, //69 up 6
  {GreenParasite_ArcMain_Up1, {  0,107,102,104}, {186+9,167-3}}, //70 up 7
  {GreenParasite_ArcMain_Up1, {102,107,103,104}, {187+9,167-3}}, //71 up 8
  {GreenParasite_ArcMain_Up2, {  0,  0,102,102}, {186+9,166-3}}, //72 up 9
  {GreenParasite_ArcMain_Up2, {102,  0,103,102}, {187+9,166-3}}, //73 up 10
  {GreenParasite_ArcMain_Up2, {  0,102,103,102}, {187+9,165-3}}, //74 up 11
  {GreenParasite_ArcMain_Up2, {103,102,103,101}, {187+9,165-3}}, //75 up 12
  {GreenParasite_ArcMain_Up3, {  0,  0,103,102}, {187+9,165-3}}, //76 up 13
  {GreenParasite_ArcMain_Up3, {103,  0,103,101}, {187+9,165-3}}, //77 up 14

  {GreenParasite_ArcMain_Right0, {  0,  0,127,107}, {169+15,158+8}}, //78 right 1
  {GreenParasite_ArcMain_Right0, {127,  0,127,107}, {169+15,158+8}}, //79 right 2
  {GreenParasite_ArcMain_Right0, {  0,107,123,107}, {170+15,158+8}}, //80 right 3
  {GreenParasite_ArcMain_Right0, {123,107,123,106}, {170+15,158+8}}, //81 right 4
  {GreenParasite_ArcMain_Right1, {  0,  0,118,106}, {167+15,158+8}}, //82 right 5
  {GreenParasite_ArcMain_Right1, {118,  0,118,106}, {167+15,159+8}}, //83 right 6
  {GreenParasite_ArcMain_Right1, {  0,106,115,105}, {166+15,158+8}}, //84 right 7
  {GreenParasite_ArcMain_Right1, {115,106,115,105}, {166+15,158+8}}, //85 right 8
  {GreenParasite_ArcMain_Right2, {  0,  0,115,106}, {166+15,159+8}}, //86 right 9
  {GreenParasite_ArcMain_Right2, {115,  0,115,105}, {166+15,158+8}}, //87 right 10
  {GreenParasite_ArcMain_Right2, {  0,106,108,106}, {166+15,159+8}}, //88 right 11
  {GreenParasite_ArcMain_Right2, {108,106,109,105}, {166+15,159+8}}, //89 right 12
  {GreenParasite_ArcMain_Right3, {  0,  0,108,105}, {166+15,158+8}}, //90 right 13
  {GreenParasite_ArcMain_Right3, {108,  0,108,104}, {166+15,158+8}}, //91 right 14
};

static const Animation char_greenparasite_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//GreenParasite character functions
void Char_GreenParasite_SetFrame(void *user, u8 frame)
{
	Char_GreenParasite *this = (Char_GreenParasite*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_greenparasite_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GreenParasite_Tick(Character *character)
{
	Char_GreenParasite *this = (Char_GreenParasite*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GreenParasite_SetFrame);
	Character_Draw(character, &this->tex, &char_greenparasite_frame[this->frame]);
}

void Char_GreenParasite_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GreenParasite_Free(Character *character)
{
	Char_GreenParasite *this = (Char_GreenParasite*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GreenParasite_New(fixed_t x, fixed_t y)
{
	//Allocate greenparasite object
	Char_GreenParasite *this = Mem_Alloc(sizeof(Char_GreenParasite));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GreenParasite_New] Failed to allocate greenparasite object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GreenParasite_Tick;
	this->character.set_anim = Char_GreenParasite_SetAnim;
	this->character.free = Char_GreenParasite_Free;
	
	Animatable_Init(&this->character.animatable, char_greenparasite_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF003315;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(63,128);
	
	this->character.size = FIXED_DEC(25,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GREENPAR.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "idle7.tim",
  "idle8.tim",
  "idle9.tim",
  "idle10.tim",
  "idle11.tim",
  "idle12.tim",
  "idle13.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
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
