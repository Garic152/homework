# Projekt Aufbau
Die Projektstruktur ist bereits vorgegeben und soll nicht verändert werden.

## lib
Hier befinden sich alle header (.h) Dateien die mit der Vorgabe mitgeliefert werden. Diese Dateien sollen nicht verändert werden.

## src
Hier befinden sich sowohl bereits mitgelieferte .c Dateien als auch die zu bearbeitenden.
Schaut in die Aufgabenstellung welche Dateien bearbeitet und abgegeben werden müssen.

## tests
In diesem Ordner befinden sich Tests. Es sind bereits ein paar Tests als Beispiel vorgegeben.
Weitere Tests sollen von euch selbst erstellt werden. Orientiert euch hierbei an den vorgegebenen Test.
Die Makefile erfasst alle `*.c` Dateien, die mit `test_` im Namen beginnen und im tests Ordner liegen.
Zum bauen und ausführen der Tests siehe unten.
Beim Schreiben eigener Tests ist darauf zu achten, dass die in Zeile 7 definierte Prozessanzahl mit der mit der Anzahl der ab Zeile 15 angegebenen Prozessen übereinstimmt. Achtet auch darauf in Zeile 9 den korrekten Namen des gewünschten Schedulingverfahrens einzutragen. Die möglichen Optionen hierfür sind im `enum scheduling_strategy` in der Datei `scheduler.h` gegeben.
Achtet außerdem darauf, dass nicht zwei Prozesse die gleiche Ankunftszeit haben.
Ändert auch die Zeile `char * expected_result = [...]`, so dass dieser String dem erwarteten Ergebnis entspricht.  

## build
Dieser Ordner wird automatisch erstellt beim Kompilieren (siehe unten `make`).
Hier landen alle Objekt und ausführbare Dateien.

# Makefile
Es wird eine Makefile mitgeliefert, welche das Bauen und Ausführen der Aufgabe automatisiert. Mit Hilfe des Tools `make` (unter jeder Distribution vorhanden und meistens bereits installiert) können unterschiedliche `targets` ausgeführt werden.
Die Makefile muss von euch NICHT angepasst werden.
Als C-Compiler wird `clang` genutzt. Falls die benötigten Pakete noch nicht installiert sind können sie unter Ubuntu mit 

    sudo apt install make clang

installiert werden.
## Make targets

### `make build`
Auch `make all` oder nur `make`.
Kompiliert alle zu bauenden .c Dateien. Dabei wird noch keine ausführbare Datei erstellt.

### `make all_tests`
Baut alle tests und führt diese aus.

### `make list_tests`
Gibt eine Liste aller vorhandenen Tests aus.

### `make test_*`
Führt nur den angegebenen Test aus. Dabei ist der Name der Test Datei auch gleich dem make target (ohne dem `.c`).
Beispiel: `make test_fcfs_simple`

### `make pack`
Verpackt den `src` Ordner mit Inhalt in ein abgabefertiges `zip` Archiv, welches dann auf ISIS hochgeladen werden kann. Hierfür ist das Tool `zip`erforderlich, welches z.B. bei Ubuntu Linux über das Kommando `apt install zip` installiert werden kann. Dieses Archiv muss und soll nicht weiter angepasst werden!

### `make clean`
Räumt auf.
