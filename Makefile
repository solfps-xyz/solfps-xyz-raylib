.PHONY: web clean

web:
	@echo "Building for web..."
	@source emsdk_env.sh && \
	cd webbuild && \
	emcmake cmake .. && \
	emmake make && \
	if [ -f *.html ]; then mv *.html index.html; fi && \
	if [ -f *.wasm ]; then mv *.wasm game.wasm; fi && \
	if [ -f *.data ]; then mv *.data game.data; fi && \
	if [ -f *.js ]; then mv *.js game.js; fi && \
	sed -i '' 's/raylib-wasm-template\.wasm/game.wasm/g' game.js
	

clean:
	rm -rf webbuild/*

setup:
	mkdir -p webbuild