# Kruger Game Engine

## Building Kruger

1. Download the external dependencies (<https://drive.google.com/file/d/1whDvXeZKd3P4nbxTmmukPds4SG3saYra/view?usp=sharing>) and extract into the KRG external folder ( X:/Path/KRG/External )
2. Open the KRG.sln
3. REBUILD the "KRG.Scripts.Reflect" project - this will generate all the kruger reflection data
4. Build the "Application" solution folder - this will build all the applications needed for KRG to run.

## Basic Info

### Applications

* Engine - this is the standalone engine client
* Editor - This is the Kruger editor
* Resource Server - This is a standalone application that will compile and provide resources to the various KRG applications
* Reflector - This generates the KRG reflection data
* Resource Compiler - This processes resource compilation requests
* Tester - Empty console app used for random testing

## Notes
If you use the SmartCommandLineArguments plugin for VS (https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments) then there are supplied saved arguments to help you start the engine.
