# Kruger Game Engine

## Building Kruger

1. Open the KRG.sln
2. Open the NavPower.props file (it's under the '6. Property Sheets' solution folder) and change this line: <NAVPOWER_INCLUDED>true</NAVPOWER_INCLUDED> to <NAVPOWER_INCLUDED>false</NAVPOWER_INCLUDED>
3. Build "KRG.Scripts.Reflect" - this will generate all the kruger reflection data
4. Build the "Application" solution folder - this will build all the applications needed for KRG to run.

## Quick Start

There are several demo maps in KRG, to launch the engine into one of these maps, start the engine ("KrugerEngine.exe") with either of the following commandlines:

* -map data://maps/BR_minimal.map
* -map data://maps/BR_full.map
* -map data://maps/AnimTest_minimal.map
* -map data://maps/AnimTest_full.map

## Applications

* Engine - this is the standalone engine client
* Resource Editor - This is a editor for previewing and editor various KRG resources
* Resource Server - This is a standalone application that will compile and provide resources to the various KRG applications
* Reflector - This generates the KRG reflection data
* Resource Compiler - This processes resource compilation requests
* Tester - Empty console app used for random testing

## Notes
If you use the SmartCommandLineArguments plugin for VS (https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments) then there are supplied saved arguments to help you start the engine.
