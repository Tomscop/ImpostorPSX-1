/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "events.h"
#include "stage.h"
#include "timer.h"
#include "random.h"
#include "mutil.h"

Events event_speed;

static void Events_Check(Event* event)
{
	//Events
	switch(event->event & EVENTS_FLAG_VARIANT)
	{
		case EVENTS_FLAG_SPEED: //Scroll Speed!!
		{
			event_speed.value1 = event->value1;
			event_speed.value2 = event->value2;
			break;
		}
		case EVENTS_FLAG_GF: //Set GF Speed!!
		{
			stage.gf_speed = (event->value1 >> FIXED_SHIFT) * 4;
			break;
		}
		case EVENTS_FLAG_CAMZOOM: //Add Camera Zoom!!
		{
			stage.charbump += event->value1;
			stage.bump += event->value2;
			break;
		}
		case EVENTS_FLAG_FLASH: //Flash!!
		{
			if((event->value1 == 0) || ((event->value1 >> FIXED_SHIFT) == 1))
			{
				stage.flash = FIXED_DEC(255,1);
				stage.flashspd = FIXED_DEC(1000,1);
			}
			if((event->value1 >> FIXED_SHIFT) == 2)
				stage.black = true;
			if((event->value1 >> FIXED_SHIFT) == 3)
				stage.black = false;
			break;
		}
		case EVENTS_FLAG_BEEP: //Reactor Beep!!
		{
			stage.bump += FIXED_DEC(3,100); //0.03
			stage.charbump += FIXED_DEC(15,1000); //0.015
			stage.reactor = FIXED_DEC(255,1);
			if (event->value1 == 409)
				stage.reactorspd = FIXED_DEC(875,1);
			if ((event->value1 >> FIXED_SHIFT) == 1)
				stage.reactorspd = FIXED_DEC(1000,1);
			break;
		}
		case EVENTS_FLAG_BOP: //Alter Camera Bop!!
		{
			if ((event->value2 >> FIXED_SHIFT) == 1)
			{
				stage.bop1 = 0x3;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 2)
			{
				stage.bop1 = 0x7;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 4)
			{
				stage.bop1 = 0xF;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) == 8)
			{
				stage.bop1 = 0x1F;
				stage.bop2 = 0;
			}
			if ((event->value2 >> FIXED_SHIFT) >= 40)
			{
				stage.bop2 = 222;
			}
			break;
		}
		case EVENTS_FLAG_EXTZOOM: //Extra Camera Zoom!!
		{
			if (event->value1 == 0)
			{
				stage.player->focus_zoom = stage.player->zoom_save;
				stage.opponent->focus_zoom = stage.opponent->zoom_save;
			}
			else
			{
				stage.player->focus_zoom += event->value1;
				stage.opponent->focus_zoom += event->value1;
			}
			break;
		}
		default: //nothing lol
		break;
	}
}

void Events_Tick(void)
{
	//Scroll Speed!
	stage.speed += (FIXED_MUL(stage.ogspeed, event_speed.value1) - stage.speed) / (((event_speed.value2 / 60) + 1));
}

void Events_StartEvents(void)
{
	for (Event *event = stage.cur_event; event->pos != 0xFFFF; event++)
	{
		//Update event pointer
		if (event->pos > (stage.note_scroll >> FIXED_SHIFT))
			break;

		else
			stage.cur_event++;

		if (event->event & EVENTS_FLAG_PLAYED)
			continue;

		Events_Check(event);
		event->event |= EVENTS_FLAG_PLAYED;
	}

	//Same thing but for event.json
	if (stage.event_chart_data != NULL)
	{
		for (Event *event = stage.event_cur_event; event->pos != 0xFFFF; event++)
		{
			//Update event pointer
			if (event->pos > (stage.note_scroll >> FIXED_SHIFT))
				break;

			else
				stage.event_cur_event++;

			if (event->event & EVENTS_FLAG_PLAYED)
				continue;

			Events_Check(event);
			event->event |= EVENTS_FLAG_PLAYED;
		}
	}

	Events_Tick();
}

//Initialize some stuffs
void Events_Load(void)
{
	//Scroll Speed
	event_speed.value1 = FIXED_UNIT;
	event_speed.value2 = 0;
}