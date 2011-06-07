#include "stdafx.h"
#include "Database/DatabaseManager.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Database/Serializable.h"
#include "FileSystem/File.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"
#include <boost/iostreams/stream_buffer.hpp>

DECLARE_AUTO_LOGGER("database.SXML");

namespace slon {
namespace database {
	
SXMLIArchive::SXMLIArchive()
:	version(0)
{
}

void SXMLIArchive::readFromFile(filesystem::File& file)
{
    filesystem::file_buffer buf(file);
    std::istream is(&buf);
    is >> document;

	referenceSerializables.clear();
	referenceElements.clear();

	// search for all referenced elements, use DFS
	std::deque<xmlpp::element_iterator> elements;
	std::copy( document.first_child_element(), document.end_child_element(), std::back_inserter(elements) );
	while ( !elements.empty() ) 
	{
		xmlpp::element_iterator top = elements.back();
		elements.pop_back();

		// add children to queue
		std::copy( top->first_child_element(), top->end_child_element(), std::back_inserter(elements) );

		// check if element have reference id
		if ( top->has_attribute("refId") ) 
		{
			std::pair<reference_element_map::iterator, bool> insPair = referenceElements.insert( std::make_pair(top->get_attribute_value<size_t>("id"), top) );
			if (!insPair.second) {
				throw serialization_error(AUTO_LOGGER, "Encountered two elements with same reference id.");
			}
		}
	}
}

Serializable* SXMLIArchive::readReference(int refId)
{
	reference_serializable_map::iterator itS = referenceSerializables.find(refId);
	if ( itS != referenceSerializables.end() ) {
		return itS->second;
	}

	reference_element_map::iterator itE = referenceElements.find(refId);
	if ( itE == referenceElements.end() ) {
		return 0;
	}

	return readSerializable(*itE->second);
}

Serializable* SXMLIArchive::readSerializableOrReference()
{
	Serializable* serializable = 0;
	if (!nextElement) {
		return 0;
	}
	else if ( nextElement->has_attribute("id") ) {
		serializable = readReference( nextElement->get_attribute_value<size_t>("id") );
	}
	else {
		serializable = readSerializable(*nextElement);
	}

	nextElement = nextElement->next_sibling_element();
	return serializable;
}

void SXMLIArchive::readChunkInfo()
{
	chunkInfo.size   =  openedElement->has_attribute("count") ? openedElement->get_attribute_value<size_t>("count") : 1;
	chunkInfo.refId  =  openedElement->has_attribute("refId") ? openedElement->get_attribute_value<size_t>("refId") : 0;
	chunkInfo.isLeaf = !openedElement->first_child_element();
}

Serializable* SXMLIArchive::readSerializable(xmlpp::element& el)
{
	Serializable* serializable = currentDatabaseManager().createSerializableByName( el.get_value() );
	if (serializable)
	{
		// remember state
		xmlpp::element_iterator prevOpenedElement = openedElement;
		xmlpp::element_iterator prevNextElement   = nextElement;
		chunk_info				prevChunkInfo     = chunkInfo;

		// deserialize
		openedElement = xmlpp::element_iterator(el);
		nextElement   = el.first_child_element();
		readChunkInfo();
		try
		{
			serializable->deserialize(*this);
		}
		catch (...)
		{
			delete serializable;
			serializable = 0;
		}
		
		// restore state
		openedElement = prevOpenedElement;
		nextElement   = prevNextElement;
		chunkInfo     = prevChunkInfo;

		if ( el.has_attribute("id") && serializable ) 
		{
			bool res = referenceSerializables.insert( std::make_pair(el.get_attribute_value<size_t>("id"), serializable) ).second;
			assert(res);
		}
	}

	return serializable;
}

bool SXMLIArchive::openChunk(const char* name, chunk_info& info)
{
	if ( !openedElement )
	{
		if ( !document.first_child_element() ) {
			return false;
		}
		else if ( strcmp(document.first_child_element()->get_value(), name) != 0 ) {
			return false;
		}

		openedElement = document.first_child_element();
		nextElement   = openedElement->first_child_element();
	}
	else if ( !nextElement ) {
		return false;
	}
	else if ( strcmp(nextElement->get_value(), name) != 0 ) {
		return false;
	}
	else 
	{
		openedElement = nextElement;
		nextElement   = openedElement->next_sibling_element();
	}

	readChunkInfo();
	return true;
}

void SXMLIArchive::closeChunk()
{
	nextElement   = openedElement->next_sibling_element();
	openedElement = openedElement->get_parent();
	if (openedElement) {
		readChunkInfo();
	}
	else 
	{
		chunkInfo.isLeaf = false;
		chunkInfo.refId  = 0;
		chunkInfo.size   = 0;
	}
}

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
    if (!currentElement) {
		currentElement = xmlpp::element_iterator(document.add_child(child));
	}
	else {
		currentElement = xmlpp::element_iterator(currentElement->add_child(child));
	}
}	

void SXMLOArchive::closeChunk()
{
    if (!currentElement) {
        throw serialization_error(AUTO_LOGGER, "Trying to close root chunk");
    }

    currentElement = currentElement->get_parent();
}

void SXMLOArchive::writeSerializablOrReference(Serializable* serializable)
{
	if ( int refId = getReferenceId(serializable) ) {
		writeReferenceChunk(refId);
	}
	else 
	{
		openChunk( serializable->getSerializableName() );
		serializable->serialize(*this);
		closeChunk();
	}
}

void SXMLOArchive::writeReferenceChunk(int refId)
{
    xmlpp::element child("Reference");
    child.set_attribute("id", boost::lexical_cast<std::string>(refId));
    currentElement->add_child(child);
}

void SXMLOArchive::writeStringChunk(const char* name, const char* str, size_t size)
{
    xmlpp::element child(name);
    child.set_text( str[size] == '\0' ? str : std::string(str, str + size).c_str() );
    currentElement->add_child(child);
}

void SXMLOArchive::writeStringChunk(const char* name, const wchar_t* str, size_t size)
{
    xmlpp::element child(name);
    //child.set_text(str); // FIXME
    currentElement->add_child(child);
}

void SXMLOArchive::writeToFile(filesystem::File& file) const
{
    filesystem::file_buffer buf(file);
    std::ostream os(&buf);
    document.print_file(os);
}

} // namespace database
} // namespace slon
