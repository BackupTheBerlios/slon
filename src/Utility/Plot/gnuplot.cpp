#include "stdafx.h"
#include "Utility/tmp_file.hpp"
#include "Utility/Plot/gnuplot.h"
#include "Filesystem/File.h"
#include "Filesystem/FileSystemManager.h"

namespace slon {

void gnuplot::define_macro(const std::string& name, const std::string& value)
{
	macros[name] = value;
}

void gnuplot::undef_macro(const std::string& name)
{
	macros.erase(name);
}

bool gnuplot::have_macro(const std::string& name) const
{
	return macros.count(name) > 0;
}

void gnuplot::clear_macros()
{
	macros.clear();
}

void gnuplot::set_data(unsigned stage, const std::string& fileName)
{
	remove_data(stage);
	dataFiles[stage] = fileName;
}

void gnuplot::set_data_source(unsigned stage, const std::string& data)
{
	remove_data(stage);
	dataSources[stage] = data;
}

void gnuplot::remove_data(unsigned stage)
{
	dataFiles.erase(stage);
	dataSources.erase(stage);
}

bool gnuplot::have_data(unsigned stage) const
{
	return (dataFiles.count(stage) > 0) || (dataSources.count(stage) > 0);
}

void gnuplot::clear_data()
{
	dataFiles.clear();
	dataSources.clear();
}

int gnuplot::execute(const std::string& fileName) const
{
	filesystem::file_ptr file( filesystem::asFile( filesystem::currentFileSystemManager().getNode(fileName.c_str()) ) );
	if ( file && file->open(filesystem::File::in) ) 
	{
		std::string source;
		source.resize( (size_t)file->size() );
		file->read( &source[0], file->size() );
		return execute_source(source);
	}

	return -1;
}

int gnuplot::execute_source(const std::string& source) const
{
	typedef boost::shared_ptr<tmp_file> tmp_file_ptr;

	tmp_file script;

	// write macros
	for (macro_map::const_iterator iter  = macros.begin(); 
								   iter != macros.end(); 
								   ++iter)
	{
		script << iter->first << " = \"" << iter->second << "\";\n";
	}

	// write data files
	for (data_map::const_iterator iter  = dataFiles.begin(); 
								  iter != dataFiles.end(); 
								  ++iter)
	{
		script << iter->first << " = \"" << iter->second << "\";\n";
	}

	// transfer data sources via temp files
	std::vector<tmp_file_ptr> dataTmpFiles;
	for (data_map::const_iterator iter  = dataSources.begin(); 
								  iter != dataSources.end(); 
								  ++iter)
	{
		tmp_file_ptr tf(new tmp_file);
		script << "DATA" << iter->first << " = \"" << tf->name() << "\";\n";
		*tf << iter->second; // write data
		dataTmpFiles.push_back(tf);
	}

	// write other source
	script << source;

	std::string cmd = std::string("\"") + GNUPLOT_EXECUTABLE + "\" " + script.name();
	return system( cmd.c_str() );
}

} // namespace slon
