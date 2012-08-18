BroodwarBotQ
============

BSD 3-clauses, Copyright 2010-2012 Gabriel Synnaeve

Protoss bot for StarCraft: Broodwar 1.16.1, tested with BWAPI 3.7.4 (rev 4160)

C++ DLL whose repository can be found at https://github.com/SnippyHolloW/BroodwarBotQ
Source zip https://github.com/SnippyHolloW/BroodwarBotQ/zipball/master

To run, it requires: 
- MacroProject.dll (the output of the first VCPROJ of the solution)
- the existence of "bwapi-data\AI\tables\[PvPx|TvPx|ZvPx].table" (found in tables.7z),
  see http://emotion.inrialpes.fr/people/synnaeve/index_files/OpeningPrediction_proceedings.pdf
- the existence of the "bwapi-data\AI\terrain" folder (so that it can serialize its terrain analysis)
- the "bwapi-data\read" and "bwapi-data\write" folders for online (between games) learning

To compile (MacroProject), it requires:
 - BWAPI, with the path to the lib in BWAPI_DIR
 - boost (>= 1.42), with the path to the lib (needs staged serialization) in BOOST_DIR

Nota bene: it may take quite long on the first start to perform BWTA + my own terrain analysis, 
everything is then serialized and only loaded from disk. Additionally, 
loading probability tables from disk takes a few seconds too.