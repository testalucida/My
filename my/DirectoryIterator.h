#pragma once

#include <my/Signal.hpp>

#include <string>
#include <vector>
#include <memory>

namespace my {

struct Entry {
public:
	std::string directory;
	std::string name;
	//char sortvalue[512];
	char lastWrite[30];
	unsigned long size;
	bool isDirectory;
    bool isNormalFile; //DT_REG
	bool isReadOnly;
	bool isSystem;
	bool isHidden;
	bool isArchived;
};

typedef std::shared_ptr<Entry> EntryPtr;

struct EntryFoundEvent {
	EntryPtr entryPtr;
};

/**
	Klasse iteriert durch ein Verzeichnis und meldet jedes Vorkommen
	einer Datei oder eines Unterverzeichnisses.
	Die Meldung erfolgt optional sofort beim Finden der Datei oder
	am Ende des Iterationsvorgangs und nach Sortierung der Ergebnismenge.
	�ber den Konstruktor kann gesteuert werden, ob Unterverzeichnisse einbezogen
	werden sollen und ob die Ergebnismenge sortiert werden soll.
	Der Iterationsvorgang kann durch Aufruf von stop() abgebrochen werden.
*/
class DirectoryIterator {
public:
	Signal < DirectoryIterator, EntryFoundEvent > signalEntryFound;
	Signal < DirectoryIterator, NULLPARM > signalTerminated;
public:
	/**Iterates current directory. Includes subdirectories. Sorting == true. */
	DirectoryIterator();
	DirectoryIterator( const std::string &startDir, bool includeSubDirs = true, bool sort = true );
	~DirectoryIterator();
	void iterate();
	/** Cancels current iterating */
	void stop() { _stop = true; }
private:
	DirectoryIterator( const std::string &startDir, bool includeSubDirs, bool sort, bool *pStop );
	void iterate( const std::string &dir, const std::string & filePattern );
	void rememberEntry( EntryPtr pEntry );
private:
	std::string _startDir;
	bool _includeSubDirs;
	bool _stop;
	bool *_pMasterStop;
	bool _sort;
	std::vector<EntryPtr> _entries;
};

} //nsp my
