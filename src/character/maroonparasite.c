/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "maroonparasite.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//MaroonParasite character structure
enum
{
  MaroonParasite_ArcMain_Idle0,
  MaroonParasite_ArcMain_Idle1,
  MaroonParasite_ArcMain_Idle2,
  MaroonParasite_ArcMain_Idle3,
  MaroonParasite_ArcMain_Idle4,
  MaroonParasite_ArcMain_Idle5,
  MaroonParasite_ArcMain_Idle6,
  MaroonParasite_ArcMain_Idle7,
  MaroonParasite_ArcMain_Idle8,
  MaroonParasite_ArcMain_Idle9,
  MaroonParasite_ArcMain_Left0,
  MaroonParasite_ArcMain_Left1,
  MaroonParasite_ArcMain_Left2,
  MaroonParasite_ArcMain_Left3,
  MaroonParasite_ArcMain_Left4,
  MaroonParasite_ArcMain_Down0,
  MaroonParasite_ArcMain_Down1,
  MaroonParasite_ArcMain_Down2,
  MaroonParasite_ArcMain_Down3,
  MaroonParasite_ArcMain_Down4,
  MaroonParasite_ArcMain_Up0,
  MaroonParasite_ArcMain_Up1,
  MaroonParasite_ArcMain_Up2,
  MaroonParasite_ArcMain_Up3,
  MaroonParasite_ArcMain_Up4,
  MaroonParasite_ArcMain_Up5,
  MaroonParasite_ArcMain_Right0,
  MaroonParasite_ArcMain_Right1,
  MaroonParasite_ArcMain_Right2,
  MaroonParasite_ArcMain_Right3,
	
	MaroonParasite_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[MaroonParasite_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_MaroonParasite;

//MaroonParasite character definitions
static const CharFrame char_maroonparasite_frame[] = {
  {MaroonParasite_ArcMain_Idle0, {  0,  0,155,116}, {189,164}}, //0 idle 1
  {MaroonParasite_ArcMain_Idle0, {  0,116,156,116}, {191,164}}, //1 idle 2
  {MaroonParasite_ArcMain_Idle1, {  0,  0,157,116}, {191,164}}, //2 idle 3
  {MaroonParasite_ArcMain_Idle1, {  0,116,157,116}, {191,164}}, //3 idle 4
  {MaroonParasite_ArcMain_Idle2, {  0,  0,157,118}, {192,166}}, //4 idle 5
  {MaroonParasite_ArcMain_Idle2, {  0,118,157,117}, {192,165}}, //5 idle 6
  {MaroonParasite_ArcMain_Idle3, {  0,  0,163,117}, {195,166}}, //6 idle 7
  {MaroonParasite_ArcMain_Idle3, {  0,117,164,117}, {196,166}}, //7 idle 8
  {MaroonParasite_ArcMain_Idle4, {  0,  0,163,117}, {194,166}}, //8 idle 9
  {MaroonParasite_ArcMain_Idle4, {  0,117,163,116}, {195,165}}, //9 idle 10
  {MaroonParasite_ArcMain_Idle5, {  0,  0,163,116}, {193,165}}, //10 idle 11
  {MaroonParasite_ArcMain_Idle5, {  0,116,163,117}, {193,165}}, //11 idle 12
  {MaroonParasite_ArcMain_Idle6, {  0,  0,163,117}, {194,165}}, //12 idle 13
  {MaroonParasite_ArcMain_Idle6, {  0,117,153,115}, {189,163}}, //13 idle 14
  {MaroonParasite_ArcMain_Idle7, {  0,  0,154,115}, {189,163}}, //14 idle 15
  {MaroonParasite_ArcMain_Idle7, {  0,115,155,115}, {190,163}}, //15 idle 16
  {MaroonParasite_ArcMain_Idle8, {  0,  0,164,116}, {194,164}}, //16 idle 17
  {MaroonParasite_ArcMain_Idle8, {  0,116,163,116}, {194,164}}, //17 idle 18
  {MaroonParasite_ArcMain_Idle9, {  0,  0,164,116}, {194,164}}, //18 idle 19
  {MaroonParasite_ArcMain_Idle9, {  0,116,163,116}, {193,164}}, //19 idle 20

  {MaroonParasite_ArcMain_Left0, {  0,  0,163,122}, {198,169}}, //20 left 1
  {MaroonParasite_ArcMain_Left0, {  0,122,164,122}, {198,169}}, //21 left 2
  {MaroonParasite_ArcMain_Left1, {  0,  0,166,116}, {199,162}}, //22 left 3
  {MaroonParasite_ArcMain_Left1, {  0,116,169,116}, {201,162}}, //23 left 4
  {MaroonParasite_ArcMain_Left2, {  0,  0,169,123}, {202,163}}, //24 left 5
  {MaroonParasite_ArcMain_Left2, {  0,123,169,117}, {202,163}}, //25 left 6
  {MaroonParasite_ArcMain_Left3, {  0,  0,168,116}, {201,162}}, //26 left 7
  {MaroonParasite_ArcMain_Left3, {  0,116,169,116}, {202,162}}, //27 left 8
  {MaroonParasite_ArcMain_Left4, {  0,  0,169,116}, {201,162}}, //28 left 9
  {MaroonParasite_ArcMain_Left4, {  0,116,169,116}, {202,162}}, //29 left 10

  {MaroonParasite_ArcMain_Down0, {  0,  0,150,135}, {189,182}}, //30 down 1
  {MaroonParasite_ArcMain_Down1, {  0,  0,151,135}, {190,182}}, //31 down 2
  {MaroonParasite_ArcMain_Down2, {  0,  0,141,125}, {183,166}}, //32 down 3
  {MaroonParasite_ArcMain_Down2, {  0,125,140,125}, {181,166}}, //33 down 4
  {MaroonParasite_ArcMain_Down3, {  0,  0,155,120}, {187,153}}, //34 down 5
  {MaroonParasite_ArcMain_Down3, {  0,120,155,125}, {187,153}}, //35 down 6
  {MaroonParasite_ArcMain_Down4, {  0,  0,156,109}, {188,153}}, //36 down 7
  {MaroonParasite_ArcMain_Down4, {  0,109,156,109}, {188,153}}, //37 down 8

  {MaroonParasite_ArcMain_Up0, {  0,  0,207,126}, {213,174}}, //38 up 1
  {MaroonParasite_ArcMain_Up0, {  0,126,207,126}, {213,174}}, //39 up 2
  {MaroonParasite_ArcMain_Up1, {  0,  0,180,117}, {200,166}}, //40 up 3
  {MaroonParasite_ArcMain_Up1, {  0,117,180,117}, {199,166}}, //41 up 4
  {MaroonParasite_ArcMain_Up2, {  0,  0,173,118}, {193,167}}, //42 up 5
  {MaroonParasite_ArcMain_Up2, {  0,118,173,118}, {194,167}}, //43 up 6
  {MaroonParasite_ArcMain_Up3, {  0,  0,174,118}, {195,167}}, //44 up 7
  {MaroonParasite_ArcMain_Up3, {  0,118,174,118}, {195,167}}, //45 up 8
  {MaroonParasite_ArcMain_Up4, {  0,  0,175,119}, {198,167}}, //46 up 9
  {MaroonParasite_ArcMain_Up4, {  0,119,175,119}, {198,167}}, //47 up 10
  {MaroonParasite_ArcMain_Up5, {  0,  0,176,119}, {198,167}}, //48 up 11

  {MaroonParasite_ArcMain_Right0, {  0,  0,168,120}, {188,168}}, //49 right 1
  {MaroonParasite_ArcMain_Right0, {  0,120,168,120}, {188,168}}, //50 right 2
  {MaroonParasite_ArcMain_Right1, {  0,  0,166,122}, {195,165}}, //51 right 3
  {MaroonParasite_ArcMain_Right1, {  0,122,165,118}, {194,165}}, //52 right 4
  {MaroonParasite_ArcMain_Right2, {  0,  0,163,120}, {192,165}}, //53 right 5
  {MaroonParasite_ArcMain_Right2, {  0,120,163,119}, {192,165}}, //54 right 6
  {MaroonParasite_ArcMain_Right3, {  0,  0,162,118}, {190,165}}, //55 right 7
};

static const Animation char_maroonparasite_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 30, 31, 32, 33, 34, 35, 36, 37, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 49, 50, 51, 52, 53, 54, 55, 55, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//MaroonParasite character functions
void Char_MaroonParasite_SetFrame(void *user, u8 frame)
{
	Char_MaroonParasite *this = (Char_MaroonParasite*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_maroonparasite_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_MaroonParasite_Tick(Character *character)
{
	Char_MaroonParasite *this = (Char_MaroonParasite*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_MaroonParasite_SetFrame);
	Character_Draw(character, &this->tex, &char_maroonparasite_frame[this->frame]);
}

void Char_MaroonParasite_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_MaroonParasite_Free(Character *character)
{
	Char_MaroonParasite *this = (Char_MaroonParasite*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_MaroonParasite_New(fixed_t x, fixed_t y)
{
	//Allocate maroonparasite object
	Char_MaroonParasite *this = Mem_Alloc(sizeof(Char_MaroonParasite));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_MaroonParasite_New] Failed to allocate maroonparasite object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_MaroonParasite_Tick;
	this->character.set_anim = Char_MaroonParasite_SetAnim;
	this->character.free = Char_MaroonParasite_Free;
	
	Animatable_Init(&this->character.animatable, char_maroonparasite_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF6C0F32;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(16664,10000);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MAROONP.ARC;1");
	
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
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "down4.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "up4.tim",
  "up5.tim",
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
