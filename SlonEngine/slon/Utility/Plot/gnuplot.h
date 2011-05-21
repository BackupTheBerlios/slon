#ifndef __SLON_ENGINE_UTILITY_GNUPLOT_H__
#define __SLON_ENGINE_UTILITY_GNUPLOT_H__

#include <boost/unordered_map.hpp>
#include <map>
#include "Config.h"

namespace slon {

/** Wrapper around gnuplot script to provide simple interface from c++.
 * Example script("mem_test.plot"):
 * \code
 * set terminal png;
 * set output @OUTPUT;
 * set title "Memory test performance";
 * plot @DATA0 using 1:2 title 'memcpy' with lines, 
 * @DATA0 using 1:3 title 'memmove' with lines, 
 * @DATA0 using 1:4 title 'std::copy' with lines, 
 * @DATA0 using 1:5 title 'memset' with lines, 
 * @DATA0 using 1:6 title 'std::fill' with lines;
 * pause -1
 * \uncode
 * And you can use gnuplot class to build performance graphics as follows:
 * \code
 * // do some performance timings, consider they form the table in the file "timings.dat"
 * gnuplot gp;
 * gp.define_macro_quoted("OUTPUT", "plot.png");
 * gp.set_data(0, "timings.dat");
 * if (gp.execute("mem_test.plot") != 0) {
 *     std::cerr << "Can't plot shiny mem_test graphic" << std::endl;
 * }
 * \uncode
 */
class gnuplot
{
private:
	typedef boost::unordered_map<std::string, std::string>	macro_map;
	typedef std::map<unsigned, std::string>					data_map;

public:
	/** Define macro for gnuplot script. Overwrite existent macro if present.
	 * @param name - macro name.
	 * @param value - macro value.
	 */
	void define_macro(const std::string& name, const std::string& value);

	/** Define macro for gnuplot script and enclose it with double quotes. Overwrite existent macro if present.
	 * @param name - macro name.
	 * @param value - macro value.
	 */
	void define_macro_quoted(const std::string& name, const std::string& value);

	/** Remove macro from gnuplot script. */
	void undef_macro(const std::string& name);

	/** Check wether macro is defined. */
	bool have_macro(const std::string& name) const;

	/** Remove all macros from script. */
	void clear_macros();

	/** Setup data from file. Will setup macro @DATA{stage} to provided fileName.
	 * @param stage - data id.
	 * @param data - data source.
	 * @see set_data_source
	 */
	void set_data_file(unsigned stage, const std::string& fileName);
	
	/** Setup data. Will create temporary file while executing script, containing data; 
	 * will set macro @DATA{stage} to provided tmp file name.
	 * @param stage - data id.
	 * @param data - data source.
	 * @see set_data
	 */
	void set_data_source(unsigned stage, const std::string& data);

	/** Remove data from specified stage. */
	void remove_data(unsigned stage);

	/** Check wether specified data stage is present. */
	bool have_data(unsigned stage) const;

	/** Remove all data sources and macros */
	void clear_data();

	/** Execute gnuplot script.
	 * @param file - gnuplot script file name.
	 * @return gnuplot program return code.
	 */
	int execute(const std::string& file) const;

	/** Execute gnuplot script source.
	 * @param source - gnuplot script.
	 * @return gnuplot program return code.
	 */
	int execute_source(const std::string& source) const;

private:
	macro_map	macros;
	data_map	dataFiles;
	data_map	dataSources;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_GNUPLOT_H__