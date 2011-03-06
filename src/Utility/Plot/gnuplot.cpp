#include "stdafx.h"
#include "Log/Logger.h"
#include "Filesystem/File.h"
#include "Filesystem/FileSystemManager.h"
#include "Utility/tmp_file.hpp"
#include "Utility/Plot/gnuplot.h"

__DEFINE_LOGGER__("utility.gnuplot")

namespace {

    // return string replacing '\' with '\\\\'
    std::string slasherize(const std::string& str)
    {
        std::string result;
        for (size_t i = 0; i<str.length(); ++i) 
        {
            if (str[i] == '\\') {
                result.append("\\\\\\\\");
            }
            else {
                result += str[i];
            }
        }
        return result;
    }

} // anonymous namespace

namespace slon {

void gnuplot::define_macro(const std::string& name, const std::string& value)
{
	macros[name] = value;
}

void gnuplot::define_macro_quoted(const std::string& name, const std::string& value)
{
    define_macro(name, "\\\"" + value + "\\\"");
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

void gnuplot::set_data_file(unsigned stage, const std::string& fileName)
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
    script.open();

	// write macros
    script << "set macro;\n";
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
		script << iter->first << " = \"\\\"" << iter->second << "\\\"\";\n";
	}

	// transfer data sources via temp files
	std::vector<tmp_file_ptr> dataTmpFiles;
	for (data_map::const_iterator iter  = dataSources.begin(); 
								  iter != dataSources.end(); 
								  ++iter)
	{
		tmp_file_ptr tf(new tmp_file);
		script << "DATA" << iter->first << " = \"\\\"" << slasherize(tf->name()) << "\\\"\";\n";
        tf->open();
		*tf << iter->second; // write data
        tf->close();
		dataTmpFiles.push_back(tf);
	}

	// write other source
	script << source;
    script.close();

    tmp_file    gnuplotLog;
    std::string cmd(std::string("\"") + GNUPLOT_EXECUTABLE + "\" " + script.name() + " 1> " + gnuplotLog.name() + " 2>&1");
    int         result = system( cmd.c_str() );

    std::ifstream inScript( script.name().c_str() );
    std::ifstream inLog( gnuplotLog.name().c_str() );
    logger << log::S_FLOOD << "Executing gnuplot script:\n" 
                           << log::indent() 
                           << std::string( std::istreambuf_iterator<char>(inScript), std::istreambuf_iterator<char>() )
                           << "\n=============================================================\n"
                           << std::string( std::istreambuf_iterator<char>(inLog), std::istreambuf_iterator<char>() )
                           << log::unindent();
    logger.flush();

	return result;
}

} // namespace slon
