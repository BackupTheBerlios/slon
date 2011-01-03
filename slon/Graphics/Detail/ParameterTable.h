#ifndef __SLON_ENGINE_GRAPHICS_PARAMETER_TABLE_H__
#define __SLON_ENGINE_GRAPHICS_PARAMETER_TABLE_H__

#include "../../Utility/referenced.hpp"
#include "../../Utility/unique_string.hpp"
#include "../ParameterBinding.h"
#include <map>

namespace slon {
namespace graphics {
namespace detail {

/** Parameter table stores named parameter binding points for
 * effects purposes.
 */
class ParameterTable :
    public Referenced
{
private:
    template<typename T>
    class parameter_binding :
        public graphics::parameter_binding<T>
    {
    public:
        parameter_binding(unique_string     name_,
                          ParameterTable*   parameterTable_,
                          T*                values          = 0,
                          unsigned          count           = 0,
                          bool              dynamic         = true) :
            graphics::parameter_binding<T>(values, count, dynamic),
            name(name_),
            parameterTable(parameterTable_)
        {
            assert(parameterTable);
        }

        ~parameter_binding()
        {
            assert(parameterTable);
            parameterTable->removeParameterBinding(name);
        }

    private:
        unique_string                           name;
        boost::intrusive_ptr<ParameterTable>    parameterTable;
    };

    typedef std::map<unique_string, abstract_parameter_binding*>    parameter_map;
    typedef parameter_map::iterator                                 parameter_iterator;

public:
    boost::intrusive_ptr<abstract_parameter_binding> getParameterBinding(unique_string name)
    {
        parameter_iterator paramIter = parameters.find(name);
        if ( paramIter != parameters.end() ) {
            return boost::intrusive_ptr<abstract_parameter_binding>(paramIter->second);
        }

        return boost::intrusive_ptr<abstract_parameter_binding>();
    }

    template<typename T>
    boost::intrusive_ptr< parameter_binding<T> > getParameterBinding(unique_string name)
    {
        parameter_iterator paramIter = parameters.find(name);
        if ( paramIter != parameters.end() ) {
            return boost::intrusive_ptr< parameter_binding<T> >( dynamic_cast<parameter_binding<T>*>(paramIter->second) );
        }

        return boost::intrusive_ptr< parameter_binding<T> >();
    }

    template<typename T>
    boost::intrusive_ptr< parameter_binding<T> > addParameterBinding(unique_string  name,
                                                                     T*             values,
                                                                     unsigned       count,
                                                                     bool           dynamic = true)
    {
        boost::intrusive_ptr< parameter_binding<T> > parameter( new parameter_binding<T>(name, this, values, count, dynamic) );
        if ( parameters.insert( parameter_map::value_type( name, parameter.get() ) ).second ) {
            return parameter;
        }

        return boost::intrusive_ptr< parameter_binding<T> >();
    }

private:
    void removeParameterBinding(unique_string name)
    {
        parameters.erase(name);
    }

private:
    parameter_map parameters;
};

typedef boost::intrusive_ptr<ParameterTable>        parameter_table_ptr;
typedef boost::intrusive_ptr<const ParameterTable>  const_parameter_table_ptr;

/** Get AttributeTable used by current GraphicsManager. */
ParameterTable& currentParameterTable();

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_PARAMETER_TABLE_H__
