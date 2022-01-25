# Kruger Prototype Game Engine

## Requirements

* Visual Studio 2019 (latest version since C++17 is required)
* FBX SDK

## Building Kruger

1. Download the external dependencies (<https://drive.google.com/file/d/1whDvXeZKd3P4nbxTmmukPds4SG3saYra/view?usp=sharing>) and extract into the KRG external folder ( X:/Path/KRG/External )
2. Download and install the FBX SDK (required for editor) - <https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1>
3. Open the KRG.sln
4. Build the "KRG.Scripts.Deploy" project - this will deploy all required DLLs
5. REBUILD the "KRG.Scripts.Reflect" project - this will generate all the kruger reflection data
6. Build the "Application" solution folder - this will build all the applications needed for KRG to run.

## Basic Info

Kruger is a game engine framework, basically an engine without a renderer. It contains a lot of the boilerplate machinery required for resource management, serialization, gameplay, etc...

What's included:

* Basic core of a game engine (serialization, math, string handling, logging, etc...)
* Libclang based c++ reflection and code-generation
* Compiler based resource system with hot-reloading
* Hybrid Actor/ECS object model - <https://www.youtube.com/watch?v=jjEsB611kxs>
* AAA quality high performance animation graph system - <https://www.youtube.com/watch?v=R-T3Mk5oDHI&t=5427s>
* Basic animation compression
* Powered Ragdoll implmentation using PhysX articulations
* Basic editor infrastructure and tooling using DearImgui

### Thirdparty projects used

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

### Build System

Kruger uses vanilla msbuild for its build system. There are a set of property sheets that control all the build settings for KRG in the "code/property sheets" folder. Additionally, there are some NMAKE based deployment scripts in the "code/scripts/deploy" folder.

### Applications

* Engine - this is the standalone engine client
* Editor - This is the Kruger editor
* Resource Server - This is a standalone application that will compile and provide resources to the various KRG applications
* Reflector - This generates the KRG reflection data
* Resource Compiler - This processes resource compilation requests
* Tester - Empty console app used for random testing

## Notes

If you use the SmartCommandLineArguments plugin for VS (<https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments>) then there are supplied saved arguments to help you start the engine.
