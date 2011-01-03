#ifndef __SLON_ENGINE_DATABASE_STORAGE_H__
#define __SLON_ENGINE_DATABASE_STORAGE_H__

#include <boost/function.hpp>
#include <map>

namespace slon {
namespace database {

/** Storage stores items in associative container */
template<typename Key, typename StorageItem>
class Storage
{
public:
    typedef Key                                     key_type;
    typedef StorageItem                             item_type;
    typedef boost::function<item_type ()>           loader_type;

    typedef std::map<Key, StorageItem>              container_type;
    typedef typename container_type::value_type     value_type;
    typedef typename container_type::iterator       item_iterator;
    typedef typename container_type::const_iterator const_item_iterator;

public:
    /** Search for item in the storage */
    item_iterator find(const key_type& key)
    {
        item_iterator iter = container.find(key);
        return iter;
    }

    /** Search for item in the storage. If not found, then load it and add to storage */
    item_iterator load(const key_type& key, const loader_type& func)
    {
        item_iterator iter = container.find(key);
        if ( iter != container.end() ) {
            return iter;
        }

        return container.insert( value_type(key, func()) ).first;
    }

    /** Get iterator addressing item after last item */
    item_iterator end() { return container.end(); }

    /** Remove single item from the storage */
    void remove(const item_iterator& iterator) { container.erase(iterator); }

    /** Remove all items from the storage */
    void clear() { container.clear(); }

private:
    container_type container;
};

/** Storage stores items in associative container. Singleton */
template<typename Key, typename StorageItem>
class SingletonStorage :
	public Storage<Key, StorageItem>
{
public:
    /** Get instance */
    static SingletonStorage* Instance()
    {
    	if ( !instance.get() ) {
    		instance.reset(new SingletonStorage);
    	}

    	return instance.get();
    }

private:
    static std::auto_ptr<SingletonStorage> instance;
};

// static decl
template<typename Key, typename StorageItem>
std::auto_ptr< SingletonStorage<Key, StorageItem> > SingletonStorage<Key, StorageItem>::instance;

/** Look for item in the storage. If not found then call
 * loader and add item to the storage.
 * @param storage - storage where to look for item.
 * @param key - key of the item.
 * @param func - item loader functor.
 */
template<typename Key, typename StorageItem>
StorageItem load( Storage<Key, StorageItem>& 		 	    			 storage,
				  const Key& 							 	    		 key,
				  const typename Storage<Key, StorageItem>::loader_type& func )
{
    return storage.load(key, func)->second;
}

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_STORAGE_H__
