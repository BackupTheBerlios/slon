#ifndef SLON_ENGINE_UTILITY_ALGORITHM_AABB_TREE_HPP
#define SLON_ENGINE_UTILITY_ALGORITHM_AABB_TREE_HPP

#include "../if_then_else.hpp"
#include "../math.hpp"
#include "../Memory/object_in_pool.hpp"
#include "spatial_node.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <boost/type_traits/is_const.hpp>
#include <functional>
#include <queue>
#include <sgl/Math/AABB.hpp>
#include <sgl/Math/Intersection.hpp>

namespace slon {

template<typename LeafData, typename RealType = float>
class aabb_tree
{
public:
    typedef math::Matrix<RealType, 3, 1>    vec3_type;
    typedef math::AABB<RealType, 3>         aabb_type;

    class leaf_node;
    class volume_node;

    class volume_node :
        public object_in_pool<volume_node>
    {
    friend class aabb_tree;
    public:
        explicit volume_node(const aabb_type& _volume) :
            volume(_volume),
            parent(0)
        {
            childs[0] = 0;
            childs[1] = 0;
        }

	    bool is_leaf() const     { return childs[1] == 0; }
	    bool is_internal() const { return childs[1] != 0; }

        /** Get index of the child */
        int indexof(const volume_node* child)
        {
            assert(child && child->parent == this);
            return int(child == childs[1]);
        }

        /** Get bound of the volume */
        const aabb_type& get_bounds() const { return volume; }

        /** Get child node */
        volume_node* get_child(int i) { return childs[i]; }

        /** Get child node */
        const volume_node* get_child(int i) const { return childs[i]; }

        /** Get parent node */
        volume_node* get_parent() { return parent; }

        /** Get parent node */
        const volume_node* get_parent() const { return parent; }

        /** setup volume node as child of this node */
        void set_child(int i, volume_node* child)
        {
            childs[i] = child;
            child->parent = this;
        }

        /** Try to convert to leaf node */
        leaf_node* as_leaf() { return is_leaf() ? static_cast<leaf_node*>(this) : 0; }

        /** Try to convert to leaf node */
        const leaf_node* as_leaf() const { return is_leaf() ? static_cast<const leaf_node*>(this) : 0; }

        /** Destroy node */
        void destroy() 
        {
            if ( is_leaf() ) {
                delete static_cast<leaf_node*>(this);
            }
            else {
                delete this;
            }
        }

    protected:
        /// Use destroy instead
        ~volume_node() {}

    private:
        aabb_type       volume;
	    volume_node*    parent;
        volume_node*    childs[2];
    };

    class leaf_node :
        public object_in_pool<leaf_node, volume_node>
    {
    public:
        leaf_node( const aabb_type&   _volume,
                   const LeafData&    _data )
        :   object_in_pool<leaf_node, volume_node>(_volume)
        ,   data(_data)
        {
        }

    public:
        LeafData data;
    };

    // DFS tree traverse iterator
    template<typename T>
    class iterator_impl :
        public boost::iterator_facade<
            iterator_impl<T>,
            T,
            boost::bidirectional_traversal_tag
        >
    {
    private:
        friend class aabb_tree;
        friend class boost::iterator_core_access;

        typedef typename if_then_else<boost::is_const<T>::value, 
                                      const volume_node, 
                                      volume_node>::type         volume_node_type;

        typedef typename if_then_else<boost::is_const<T>::value, 
                                      const leaf_node, 
                                      leaf_node>::type           leaf_node_type;
    private:
        void increment()
        {
            assert(node);

            // go up until we find nonvisited right child
            volume_node_type* parent = node->get_parent();
            while (parent && parent->get_child(1) == node) 
            { 
                node   = parent;
                parent = node->get_parent();
            }

            // go down, until leaf is reached
            if (parent) 
            {
                node = parent->get_child(1);
                while (node) {
                    node = node->get_child(0);
                }
            }
        }

        void decrement()
        {
            assert(node);

            // go up until we find nonvisited left child
            volume_node_type* parent = node->get_parent();
            while (parent && parent->get_child(0) == node) 
            { 
                node   = parent;
                parent = node->get_parent();
            }

            // go down, until leaf is reached
            if (parent) 
            {
                node = parent->get_child(0);
                while (node) {
                    node = node->get_child(1);
                }
            }
        }

        bool equal(iterator_impl const& other) const
        {
            return node == other.node;
        }

        T& dereference() const 
        { 
            assert(node);
            return get_node()->data; 
        }

    public:
        /// construct end iterator
        iterator_impl() {}
                  
        explicit iterator_impl(volume_node_type* node_)
        :   node(node_)
        {
            assert(node);
            while ( node->is_internal() ) {
                node = node->get_child(0);
            }
        }

        explicit iterator_impl(leaf_node_type* node_)
        :   node(node_)
        {
            assert(node);
        }

        operator bool () const { return (node != 0); }

        leaf_node_type* get_node() const { return static_cast<leaf_node_type*>(node); }

    private:
        volume_node_type* node;
    };

    typedef iterator_impl<LeafData>         iterator;
    typedef iterator_impl<const LeafData>   const_iterator;

public:
    aabb_tree();
    ~aabb_tree();

    /** Get bounding box of the aabb_tree */
    const math::AABBf& get_bounds() const
    {
        if (root) {
            return root->get_bounds();
        }

        return bounds<aabb_type>::inv_infinite();
    }

    /** Check wether tree is empty */
    bool empty() const { return root == 0; }

    /** Use manually constructed tree */
    void set_root(volume_node* root_) { root = root_; }

    /** Get root AABB node of the tree*/
    volume_node* get_root() { return root; }
    
    /** Get root AABB node of the tree*/
    const volume_node* get_root() const { return root; }

    /** Get begin iterator */
    iterator begin() { return iterator(root); }

    /** Get begin iterator */
    const_iterator begin() const { return const_iterator(root); }

    /** Get end iterator */
    iterator end() { return iterator(); }

    /** Get end iterator */
    const_iterator  end() const { return const_iterator(); }

    /** Insert new element in the tree.
     * @param volume - axis aligned bounding box of the element.
     * @param leafData - data to store in the leaf.
     * @return inserted node.
     */
    iterator insert(const aabb_type& volume, const LeafData& leafData);

    /** Insert new element in the tree at specified location.
     * @param parent - parent AABB node for the item. Should be internal node.
     * @param volume - axis aligned bounding box of the element.
     * @param leafData - data to store in the leaf.
     * @param asLeft - true if you wish to insert node as left child, otherwise false.
     * @return inserted node.
     */
    iterator insert(volume_node& parent, const aabb_type& volume, const LeafData& leafData, bool asLeft = false);

    /** Remove node from the tree. */
    void remove(iterator iter);

    /** Update AABB of the node.
     * @param iter - iterator pointing node to update.
     * @param volume - new volume.
     * @return iterator pointing new node position.
     */
    iterator update(iterator iter, const aabb_type& volume);

    /** Remove all nodes from aabb_tree */
    void clear();

private:
    // update bouding hierarchy from node to root
    void relax(volume_node* node);

    // insert leaf node in the tree
    void insert_leaf(volume_node* root, leaf_node* leaf);

    // remove leaf node from the tree
    void remove_leaf(volume_node* root, leaf_node* leaf);

    // Manhattan distance
    RealType proximity(const aabb_type& a, const aabb_type& b)
    {
        const typename aabb_type::vec_type d = (a.minVec + a.maxVec) - (b.minVec + b.maxVec); // vector between centers
	    return abs(d.x) + abs(d.y) + abs(d.z);
    }

    // select closest node from the childs
    volume_node* select_closest(const volume_node* v, volume_node* a, volume_node* b)
    {
        return proximity(v->volume, a->volume) < proximity(v->volume, b->volume) ? a : b;
    }

private:
    volume_node* root;
};

template<typename LeafData, typename RealType>
aabb_tree<LeafData, RealType>::aabb_tree() :
    root(0)
{
}

template<typename LeafData, typename RealType>
aabb_tree<LeafData, RealType>::~aabb_tree()
{
    clear();
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::relax(volume_node* node)
{
    assert(node);

    // extend hierarchy nodes if needed
    volume_node* prev = node->parent;
	while ( prev != 0 && !contains(prev->volume, node->volume) )
    {
        prev->volume = math::merge(prev->childs[0]->volume, prev->childs[1]->volume);
        prev         = prev->parent;
	}
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::insert_leaf(volume_node* root, leaf_node* leaf)
{
	if (!root) {
		this->root = leaf;
	}
	else
	{
        while ( !root->is_leaf() ) {
			root = select_closest(leaf, root->childs[0], root->childs[1]);
        }

        // insert new child
		volume_node* prev = root->parent;
        volume_node* node = new volume_node(math::merge(leaf->volume, root->volume));
		if (prev)
		{
            prev->set_child(prev->indexof(root), node);
            node->set_child(0, root);
            node->set_child(1, leaf);
            relax(node);
		}
		else
		{
            node->set_child(0, root);
            node->set_child(1, leaf);
			this->root = node;
		}
	}
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::remove_leaf(volume_node* parent, leaf_node* leaf)
{
	if (leaf == root) {
		this->root = 0;
	}
	else
	{
		volume_node* parent  = leaf->parent;
		volume_node* prev    = parent->parent;
		volume_node* sibling = parent->childs[1 - parent->indexof(leaf)];

        if (prev)
		{
			prev->childs[prev->indexof(parent)] = sibling;
			sibling->parent = prev;
			delete parent;

			while (prev)
			{
				const aabb_type& prevVolume = prev->volume;
                prev->volume = math::merge(prev->childs[0]->volume, prev->childs[1]->volume);
				if (prevVolume != prev->volume) {
					prev = prev->parent;
				}
                else {
                    break;
                }
			}
		}
		else
		{
		    this->root      = sibling;
			sibling->parent = 0;
			delete parent;
		}
	}
}

template<typename LeafData, typename RealType>
typename aabb_tree<LeafData, RealType>::iterator aabb_tree<LeafData, RealType>::insert(const aabb_type& volume, const LeafData& leafData)
{
    leaf_node* leaf = new leaf_node(volume, leafData);
	insert_leaf(root, leaf);
	return iterator(leaf);
}

template<typename LeafData, typename RealType>
typename aabb_tree<LeafData, RealType>::iterator aabb_tree<LeafData, RealType>::insert(volume_node& parent, const aabb_type& volume, const LeafData& leafData, bool asLeft = false)
{
    assert( parent.is_internal() );

    int childId = asLeft ? 0 : 1;
    int otherId = asLeft ? 1 : 0;
 
    volume_node* leaf  = new leaf_node(volume, leafData);
    volume_node* other = new volume_node(math::merge(parent->childs[0]->volume, parent->childs[1]->volume), &parent);
    other->set_child(0, parent->childs[0]);
    other->set_child(1, parent->childs[1]);
    parent->set_child(childId, leaf);
    parent->set_child(otherId, other);
    relax(leaf);

    return iterator(leaf);
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::remove(iterator iter)
{
    leaf_node* node = iter.get_node();
    assert(node);
	remove_leaf(root, node);
    node->destroy();
}

template<typename LeafData, typename RealType>
typename aabb_tree<LeafData, RealType>::iterator aabb_tree<LeafData, RealType>::update(iterator iter, const aabb_type& volume)
{
    leaf_node* leaf = iter.get_node();
	remove_leaf(root, leaf);
    leaf->volume = volume;
    insert_leaf(root, leaf);
	return iterator(leaf);
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::clear()
{
    if (root)
    {
        std::queue<volume_node*> queue;
        queue.push(root);

        while ( !queue.empty() )
        {
            volume_node* node = queue.front();
            queue.pop();

            if ( node->is_internal() )
            {
                queue.push( node->childs[0] );
                queue.push( node->childs[1] );
            }

            delete node;
        }

        root = 0;
    }
}

/** Perform function on leafe nodes.
 * @param tree - tree for gathering elements.
 * @param functor - perform functor(visitor). Return true to stop traverse.
 */
template< typename LeafData,
          typename RealType,
          typename Functor >
void perform_on_leaves( const aabb_tree<LeafData, RealType>& tree,
                        Functor                              functor )
{
    typedef typename aabb_tree<LeafData, RealType>::volume_node volume_node;
    typedef typename aabb_tree<LeafData, RealType>::leaf_node   leaf_node;

    const volume_node* root = tree.get_root();
    if (root)
    {
        std::queue<const volume_node*> queue;
        queue.push(root);

        while ( !queue.empty() )
        {
            root = queue.front();
            queue.pop();

            if ( root->is_internal() )
            {
                queue.push( root->get_child(0) );
                queue.push( root->get_child(1) );
            }
            else 
			{
                if ( functor(static_cast<const leaf_node*>(root)->data) ) {
					return;
				}
            }
        }
    }
}

/** Perform function on elements intersecting specified volume.
 * @tparam Volume - type of the volume body(AABB, Frustum, etc.).
 * @param tree - tree for gathering elements.
 * @param volume - volume for gathering.
 * @param functor - perform functor(visitor). Return true to stop traverse.
 */
template< typename LeafData,
          typename RealType,
          typename Functor,
          typename Volume >
void perform_on_leaves( const aabb_tree<LeafData, RealType>& tree,
                        const Volume&                        volume,
                        Functor                              functor )
{
    typedef typename aabb_tree<LeafData, RealType>::volume_node volume_node;
    typedef typename aabb_tree<LeafData, RealType>::leaf_node   leaf_node;

    const volume_node* root = tree.get_root();
    if ( root && math::test_intersection( volume, root->get_bounds() ) )
    {
        std::queue<const volume_node*> queue;
        queue.push(root);

        while ( !queue.empty() )
        {
            root = queue.front();
            queue.pop();

            if ( root->is_internal() )
            {
                if ( math::test_intersection( volume, root->get_child(0)->get_bounds() ) ) {
                    queue.push( root->get_child(0) );
                }
                if ( math::test_intersection( volume, root->get_child(1)->get_bounds() ) ) {
                    queue.push( root->get_child(1) );
                }
            }
            else 
			{
                if ( functor(static_cast<const leaf_node*>(root)->data) ) {
					return;
				}
            }
        }
    }
}

} // namespace slon

#endif // SLON_ENGINE_UTILITY_ALGORITHM_AABB_TREE_HPP