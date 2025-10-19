# Contract integration headers:
# Combat: shoot.h, reload.h, apply_damage.h, respawn.h, switch_weapon.h
# Lobby: join_game.h, leave_game.h, set_ready.h, start_game.h, end_game.h
# Init: init_player.h, init_game.h
# Movement: movement.h

.PHONY: web clean

web:
	@echo "Building for web..."
	cd webbuild && \
	emcmake cmake .. && \
	emmake make && \
	if [ -f *.html ]; then mv *.html index.html; fi && \
	if [ -f *.wasm ]; then mv *.wasm game.wasm; fi && \
	if [ -f *.data ]; then mv *.data game.data; fi && \
	if [ -f *.js ]; then mv *.js game.js; fi
	

clean:
	rm -rf webbuild/*

setup:
	mkdir -p webbuild