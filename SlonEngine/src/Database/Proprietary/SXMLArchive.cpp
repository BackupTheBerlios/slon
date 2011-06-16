#include "stdafx.h"
#include "Database/DatabaseManager.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Database/Serializable.h"
#include "FileSystem/File.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"
#include "Utility/base64.hpp"
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
	
	openedElement = document.first_child_element();
	if ( !openedElement ) {
		throw serialization_error(AUTO_LOGGER, "Trying to deserialize from empty document");
	}
	else if ( strcmp("sxml", openedElement->get_value()) != 0 ) {
		throw serialization_error(AUTO_LOGGER, "Root SXML element is not <sxml>");
	}
	else if ( !openedElement->has_attribute("version") ) {
		throw serialization_error(AUTO_LOGGER, "SXML document doesn't have version attribute");
	}
	nextElement = openedElement->first_child_element();
	version     = openedElement->get_attribute_value<unsigned>("version", version);

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
		if ( top->has_attribute("id") ) 
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

Serializable* SXMLIArchive::readSerializable()
{
	Serializable* serializable = 0;
	if (!nextElement) {
		return 0;
	}
	else if ( nextElement->has_attribute("refId") ) {
		serializable = readReference( nextElement->get_attribute_value<size_t>("refId") );
	}
	else {
		serializable = readSerializable(*nextElement);
	}

	nextElement = nextElement->next_sibling_element();
	return serializable;
}

const SXMLIArchive::chunk_info& SXMLIArchive::readChunkInfo()
{
	chunkInfo.size   =  openedElement->has_attribute("size") ? openedElement->get_attribute_value<size_t>("size") : 1;
	chunkInfo.refId  =  openedElement->has_attribute("refId") ? openedElement->get_attribute_value<size_t>("refId") : 0;
	chunkInfo.isLeaf = !openedElement->first_child_element();
	return chunkInfo;
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
	assert(openedElement);
	if (!nextElement) {
		return false;
	}
	else if ( strcmp(nextElement->get_value(), name) != 0 ) {
		return false;
	}
	else 
	{
		openedElement = nextElement;
		nextElement   = openedElement->first_child_element();
	}

	info = readChunkInfo();
	return true;
}

void SXMLIArchive::closeChunk()
{
	assert( openedElement && openedElement->get_parent() && "trying to close root chunk" );

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

void SXMLIArchive::read(void* data)
{
	if (!openedElement) {
		throw serialization_error(log::logger_ptr(), "Trying to read raw data from unopened chunk.");
	}
    else if (!chunkInfo.isLeaf) {
		throw serialization_error(log::logger_ptr(), "Trying to read raw data from not leaf chunk.");
	}

    std::string dataStr = base64_decode( openedElement->get_text() );
	memcpy( data, dataStr.data(), dataStr.length() );
}

SXMLOArchive::SXMLOArchive(unsigned version_)
:   version(version_)
{
	xmlpp::element child("sxml");
	child.set_attribute_value("version", version);
	currentElement = xmlpp::element_iterator( document.add_child(child) );
}

// Override OArchive
int SXMLOArchive::registerReference(const void* ptr)
{
    std::pair<reference_map::iterator, bool> insPair = references.insert( std::make_pair(ptr, references.size()+1) );
    if (insPair.second) {
        return insPair.first->second;
    }

    return 0;
}

int SXMLOArchive::getReferenceId(const void* ptr) const
{
    reference_map::const_iterator it = references.find(ptr);
    if (it != references.end()) {
        return it->second;
    }

    return 0;
}

void SXMLOArchive::openChunk(const char* name, const void* ptr)
{
    xmlpp::element child(name);
	currentElement = xmlpp::element_iterator(currentElement->add_child(child));
    if (ptr)
    {
        std::pair<reference_map::iterator, bool> insPair = references.insert( std::make_pair(ptr, references.size()+1) );
        if (!insPair.second) {
            throw serialization_error(AUTO_LOGGER, "Object already serialized.");
        }

        currentElement->set_attribute_value("id", insPair.first->second);
    }
}	

void SXMLOArchive::closeChunk()
{
    if ( !currentElement || !currentElement->get_parent() ) {
        throw serialization_error(AUTO_LOGGER, "Trying to close root chunk");
    }

    currentElement = currentElement->get_parent();
}

void SXMLOArchive::writeReferenceChunk(int refId)
{
    xmlpp::element child("Reference");
    child.set_attribute("refId", boost::lexical_cast<std::string>(refId));
    currentElement->add_child(child);
}

void SXMLOArchive::writeSerializable(const Serializable* serializable, bool writeReferenceIfPossible, bool rememberReference)
{
	if (writeReferenceIfPossible) 
	{
		if (int refId = getReferenceId(serializable)) 
		{
			writeReferenceChunk(refId);
			return;
		}
	}

    openChunk("temp", rememberReference ? serializable : 0);
    currentElement->set_value( serializable->serialize(*this) );
    closeChunk();
}

void SXMLOArchive::writeBinaryChunk(const char* name, const void* data, size_t size)
{
    xmlpp::element child(name);
	child.set_attribute_value("size", size);
    child.set_text( base64_encode(reinterpret_cast<const unsigned char*>(data), size).c_str() );
    currentElement->add_child(child);
}

void SXMLOArchive::writeStringChunk(const char* name, const char* str, size_t size)
{
    xmlpp::element child(name);
	if (size > 1) {
		child.set_attribute_value("size", size);
	}
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
