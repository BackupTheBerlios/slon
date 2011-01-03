#ifndef __SLON_ENGINE_GRAPHICS_ATTRIBUTE_TABLE_H__
#define __SLON_ENGINE_GRAPHICS_ATTRIBUTE_TABLE_H__

#include "../../Utility/referenced.hpp"
#include "../../Utility/unique_string.hpp"
#include <boost/intrusive_ptr.hpp>
#include <vector>

namespace slon {
namespace graphics {
namespace detail {

/** For convinience all attributes sharing same name(semantic) will
 * be bound to the same stages. You can query attribute from the shared
 * attribute table. If attribute is already used by some effect, table
 * will return thats attribute index, otherwise it will return unused 
 * attribute index.
 */
class AttributeTable :
    public Referenced
{
public:
    static const int MAX_NUM_ATTRIBUTES = 16;

public:
    struct binding :
        public referenced
    {
    friend class AttributeTable;
    private:
        binding(unique_string       _name,
                unsigned            _index,
                AttributeTable*     _table) :
            name(_name),
            index(_index),
            table(_table)
        {
            assert(table);
        }

    public:
        ~binding()
        {
            table->removeBinding(this);
        }

    public:
        const unique_string name;
        const unsigned      index;

    private:
        boost::intrusive_ptr<AttributeTable> table;
    };
    typedef boost::intrusive_ptr<binding> binding_ptr;

public:
    AttributeTable();

    /** Query attribute. If attribute is already used return that attribute.
     * Otherwise make attribute with unused index. You must keep requested attribute
     * all time you are using it.
     */
    binding_ptr queryAttribute(unique_string name);

private:
    /** Remove attribute from the table. It is called in the attribute destructor. */
    void removeBinding(binding* attr);

private:
    binding*  bindings[MAX_NUM_ATTRIBUTES];
};

typedef boost::intrusive_ptr<AttributeTable>        attribute_table_ptr;
typedef boost::intrusive_ptr<const AttributeTable>  const_attribute_table_ptr;

/** Get AttributeTable used by current GraphicsManager. */
AttributeTable& currentAttributeTable();

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_ATTRIBUTE_TABLE_H__
