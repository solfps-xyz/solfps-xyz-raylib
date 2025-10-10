# solfps.xyz

> Enjoy 1 vs 1 Multiplayer Team Deathmatch First Person Shooter Game on Solana enabled by Magicblock

## Prerequisites
1. Install Emscripten
2. Activate the Emsripten Environment

## Build Steps
1. Clone Raylib Git Submodule
2. Build Raylib for WebAssembly
```sh
cd lib/raylib/src
emmake make PLATFORM=PLATFORM_WEB -B
cd ../../..
```
3. Build Project
```sh
mkdir -p webbuild
cd webbuild
emcmake cmake ..
emmake make
```
4. Run localhost web server to view the game
```
npx serve
```
