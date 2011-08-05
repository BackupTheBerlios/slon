#include "stdafx.h"
#include "Database/DatabaseManager.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::database;

// wrappers
bool saveLibraryDef(const std::string& path,
				    const library_ptr& library)
{
    return saveLibrary(path, library);
}

void exportDatabaseManager()
{
    def("saveLibrary", saveLibrary);
    def("saveLibrary", saveLibraryDef);
}