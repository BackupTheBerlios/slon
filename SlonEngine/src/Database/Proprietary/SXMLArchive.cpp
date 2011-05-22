#include "stdafx.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("database.SXML");

namespace slon {
namespace database {

SXMLOArchive::SXMLOArchive(unsigned version_)
:   version(version_)
{
}

// Override OArchive
int SXMLOArchive::registerReference(Serializable* serializable)
{
    std::pair<reference_map::iterator, bool> insPair = references.insert( std::make_pair(serializable, references.size()+1) );
    if (insPair.second) {
        return insPair.first->second;
    }

    return 0;
}

int SXMLOArchive::getReferenceId(Serializable* serializable) const
{
    reference_map::const_iterator it = references.find(serializable);
    if (it != references.end()) {
        return it->second;
    }

    return 0;
}

void SXMLOArchive::openChunk(const char* name)
{
    xmlpp::element child(name);
    currentElement.add_child(child);
    currentElement = child;
}

void SXMLOArchive::closeChunk()
{
    if ( !currentElement.get_parent() ) {
        throw serialization_error(AUTO_LOGGER, "Trying to close root chunk");
    }

    currentElement = *currentElement.get_parent();
}

void SXMLOArchive::writeReferenceChunk(int refId)
{
    xmlpp::element child("Reference");
    child.set_attribute("id", boost::lexical_cast<std::string>(refId));
    currentElement.add_child(child);
}

void SXMLOArchive::writeStringChunk(const char* name, const char* str, size_t size)
{
    xmlpp::element child(name);
    child.set_text(str); // FIXME
    currentElement.add_child(child);
}

void SXMLOArchive::writeStringChunk(const char* name, const wchar_t* str, size_t size)
{
    xmlpp::element child(name);
    //child.set_text(str); // FIXME
    currentElement.add_child(child);
}

void SXMLOArchive::writeToFile(filesystem::File& file) const
{
    // FIXME
    document.print_file("test.xml");
}

} // namespace database
} // namespace slon
