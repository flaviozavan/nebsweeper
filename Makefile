#This makefile does not work with windows
all clean: TARGET += $(MAKECMDGOALS)
all: server curses sdl launcher
clean: server-clean curses-clean sdl-clean launcher-clean

server curses:
	$(MAKE) -C nebsweeper-$@
	ln -sf ../nebsweeper-$@/nebsweeper-$@ bin/

sdl:
	$(MAKE) -C nebsweeper-$@
	ln -sf ../nebsweeper-$@/nebsweeper-$@ bin/
	ln -sf ../../nebsweeper-$@/data/font.png bin/data/
	ln -sf ../../nebsweeper-$@/data/tiles.png bin/data/
   
launcher:
	$(MAKE) -C nebsweeper-$@
	ln -sf ../nebsweeper-$@/nebsweeper-$@ bin/
	ln -sf ../../nebsweeper-$@/data/launcher.glade bin/data/
   
server-clean:
	$(MAKE) clean -C nebsweeper-server
	rm -f bin/nebsweeper-server

curses-clean:
	$(MAKE) clean -C nebsweeper-curses
	rm -f bin/nebsweeper-curses

sdl-clean:
	$(MAKE) clean -C nebsweeper-sdl
	rm -f bin/nebsweeper-sdl bin/data/tiles.png
	rm -f bin/nebsweeper-sdl bin/data/font.png

launcher-clean:
	$(MAKE) clean -C nebsweeper-launcher
	rm -f bin/nebsweeper-launcher bin/data/launcher.glade

.PHONY: all clean
