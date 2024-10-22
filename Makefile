TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
	src/intro/intro.c \
	src/mutil.c \
	src/random.c \
	src/archive.c \
	src/font.c \
	src/trans.c \
	src/loadscr.c \
	src/menu.c \
	src/events.c \
	src/stage.c \
	src/debug.c \
	src/save.c \
	src/psx/psx.c \
	src/psx/io.c \
	src/psx/gfx.c \
	src/psx/audio.c \
	src/psx/pad.c \
	src/psx/timer.c \
	src/animation.c \
	src/character.c \
	src/character/bf.c \
	src/character/bfghost.c \
	src/character/bflights.c \
	src/character/bfreactor.c \
	src/character/bfejected.c \
	src/character/bfrun.c \
	src/character/bflegs.c \
	src/character/bfdefeat.c \
	src/character/bfic.c \
	src/character/bfpolus.c \
	src/character/bflava.c \
	src/character/bfairship.c \
	src/character/bfmira.c \
	src/character/pinkpretender.c \
	src/character/bfchef.c \
	src/character/bfturbulence.c \
	src/character/picorc.c \
	src/character/red.c \
	src/character/redmd.c \
	src/character/greenst.c \
	src/character/green.c \
	src/character/greenreactor.c \
	src/character/greenparasite.c \
	src/character/yellow.c \
	src/character/white.c \
	src/character/blackrun.c \
	src/character/blacklegs.c \
	src/character/whitedk.c \
	src/character/blackdk.c \
	src/character/black.c \
	src/character/blackp.c \
	src/character/monotone.c \
	src/character/maroon.c \
	src/character/maroonparasite.c \
	src/character/gray.c \
	src/character/pink.c \
	src/character/chefogus.c \
	src/character/jorsawsee.c \
	src/character/redmungus.c \
	src/character/warchief.c \
	src/character/jelqer.c \
	src/character/redmungusvt.c \
	src/character/redmungusparasite.c \
	src/character/jorsawseeghost.c \
	src/character/powers.c \
	src/character/gf.c \
	src/character/speaker.c \
	src/character/gfghost.c \
	src/character/speakerghost.c \
	src/character/gfreactor.c \
	src/character/gfejected.c \
	src/character/gfdanger.c \
	src/character/gfpolus.c \
	src/character/gfairship.c \
	src/character/gfmira.c \
	src/stage/polus.c \
	src/stage/mira.c \
	src/stage/reactor.c \
	src/stage/ejected.c \
	src/stage/airship.c \
	src/stage/runaway.c \
	src/stage/cargo.c \
	src/stage/defeat.c \
	src/stage/finale.c \
	src/stage/skeld.c \
	src/stage/polusmaroon.c \
	src/stage/lava.c \
	src/stage/powerroom.c \
	src/stage/plantroom.c \
	src/stage/pretender.c \
	src/stage/kitchen.c \
	src/stage/lounge.c \
	src/stage/meeting.c \
	src/stage/turbulence.c \
	src/stage/victory.c \
	src/object.c \
	src/object/combo.c \
	src/object/splash.c \
	src/pause.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
