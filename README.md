# solfps.xyz

> Enjoy 1 vs 1 Multiplayer Team Deathmatch First Person Shooter Game on Solana enabled by Magicblock

## Prerequisites
1. Install Emscripten
2. Activate the Emscripten Environment

## Build Steps
1. Clone Raylib Git Submodule
```sh
git submodule update --init --recursive
```

2. Build Project for WebAssembly
```sh
mkdir -p webbuild
cd webbuild
emcmake cmake ..
emmake make
```

3. Run localhost web server to view the game
```sh
npx serve
```

## Desktop Build (Optional)
To build for desktop instead of web, create a separate build directory:
```sh
mkdir -p build
cd build
cmake ..
make
./raylib-wasm-template
```
