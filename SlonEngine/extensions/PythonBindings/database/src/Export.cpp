#include "stdafx.h"
#include "PyLibrary.h"
#include "PyDatabaseManager.h"

BOOST_PYTHON_MODULE(database)
{
    exportLibrary();
    exportDatabaseManager();
}