# Kruger Prototype Game Engine

Kruger is a game engine framework, basically an engine without a renderer. It contains a lot of the boilerplate machinery required for resource management, serialization, gameplay, etc. It provides a lot of references for how to potentially build a resource system, basic reflection and a AAA-level animation system. It could be a good framework for building your own renderer or just to experiment with as most of the tedious/annoying/time consuming pieces are already there.

## Important Note

>This is a prototype engine that I use to mess around with, as such there will be bugs and crashes (primarily with the tooling side). It is meant as a reference or a starter pack not as a production ready tool. Please don't expect me to be actively maintaining it. That said, feel free to flag issues and bugs and I will try to fix them when I have some time.

## What's included

* Basic core of a game engine (serialization, math, string handling, logging, etc...)
* Libclang based c++ reflection and code-generation
* Compiler based resource system with hot-reloading
* Hybrid Actor/ECS object model - <https://www.youtube.com/watch?v=jjEsB611kxs>
* AAA quality high performance animation graph system - <https://www.youtube.com/watch?v=R-T3Mk5oDHI&t=5427s>
* Basic animation compression
* Powered Ragdoll implmentation using PhysX articulations
* Basic editor infrastructure and tooling using DearImgui

## Requirements

* Visual Studio 2019 (latest version since C++17 is required)
* FBX SDK

## Building Kruger

Kruger uses vanilla msbuild for its build system. There are a set of property sheets that control all the build settings for KRG in the "code/property sheets" folder. Additionally, there are some NMAKE based deployment scripts in the "code/scripts/deploy" folder.

1. Download the external dependencies (<https://drive.google.com/file/d/1whDvXeZKd3P4nbxTmmukPds4SG3saYra/view?usp=sharing>) and extract into the KRG external folder ( X:/Path/KRG/External )
2. Download and install the FBX SDK (required for editor) - <https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1>
3. Open the KRG.sln
4. Build the "KRG.Scripts.Deploy" project - this will deploy all required DLLs
5. REBUILD the "KRG.Scripts.Reflect" project - this will generate all the kruger reflection data
6. Build the "Application" solution folder - this will build all the applications needed for KRG to run.

## Applications

Easiest way to get started, is just set the "KRG.Applications.Editor" as the startup project and hit run. If you want to run the engine, use the "KRG.Applications.Engine" project with the "-map data://path_to_map.map" argument.

* Engine - this is the standalone engine client
* Editor - This is the Kruger editor
* Resource Server - This is a standalone application that will compile and provide resources to the various KRG applications
* Reflector - This generates the KRG reflection data
* Resource Compiler - This processes resource compilation requests
* Tester - Empty console app used for random testing

## Thirdparty projects used

* Cereal
* EASTL
* DearImgui
* EnkiTS
* iniparser
* PCG
* xxhash
* rpmalloc
* concurrentqueue
* Game Networking Sockets
* PhysX
* cgltf
* pfd
* sqlite
* subprocess

These middleware packages are also integrated by require a license to use (so they are disabled by default)

* Live++
* Superluminal
* Navpower

## Extra Info

If you use the SmartCommandLineArguments plugin for VS (<https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments>) then there are supplied saved arguments to help you start the engine.
