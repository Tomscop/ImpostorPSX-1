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
	src/movie.c \
	src/psx/strplay.c \
	src/animation.c \
	src/character.c \
	src/character/bf.c \
	src/character/bfghost.c \
	src/character/bfreactor.c \
	src/character/bfejected.c \
	src/character/bfdefeat.c \
	src/character/bfchef.c \
	src/character/picorc.c \
	src/character/bfpixel.c \
	src/character/kid.c \
	src/character/red.c \
	src/character/redmd.c \
	src/character/greenst.c \
	src/character/green.c \
	src/character/greenreactor.c \
	src/character/greenparasite.c \
	src/character/yellow.c \
	src/character/white.c \
	src/character/whitedk.c \
	src/character/blackdk.c \
	src/character/black.c \
	src/character/blackp.c \
	src/character/maroon.c \
	src/character/maroonparasite.c \
	src/character/gray.c \
	src/character/pink.c \
	src/character/chefogus.c \
	src/character/powers.c \
	src/character/tomongus.c \
	src/character/henry.c \
	src/character/charles.c \
	src/character/jerma.c \
	src/character/nuzzus.c \
	src/character/tobyfox.c \
	src/character/brown.c \
	src/character/dave.c \
	src/character/amogus.c \
	src/character/dad.c \
	src/character/gf.c \
	src/character/speaker.c \
	src/character/gfghost.c \
	src/character/speakerghost.c \
	src/character/gfreactor.c \
	src/character/gfejected.c \
	src/character/gfpixel.c \
	src/stage/polus.c \
	src/stage/reactor.c \
	src/stage/ejected.c \
	src/stage/airship.c \
	src/stage/defeat.c \
	src/stage/lobby.c \
	src/stage/cafeteria.c \
	src/stage/henry.c \
	src/stage/jermaroom.c \
	src/stage/idk.c \
	src/stage/electrical.c \
	src/stage/daveoffice.c \
	src/stage/week1.c \
	src/stage/dummy.c \
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
