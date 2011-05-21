#include <sgl/Math/AABB.hpp>
#include <sgl/Math/Intersection.hpp>
#include <functional>
#include <queue>
#include "../referenced.hpp"
#include "../math.hpp"
#include "spatial_node.hpp"

namespace slon {

template<typename LeafData, typename RealType = float>
class aabb_tree
{
public:
    typedef math::Matrix<RealType, 3, 1>    vec3_type;
    typedef math::AABB<RealType, 3>         aabb_type;

    struct volume_node :
        public spatial_node
    {
    friend class aabb_tree;
    public:
	    inline bool	is_leaf() const		{ return childs[1] == 0; }
	    inline bool	is_internal() const	{ return childs[1] != 0; }

        /** Get index of the child */
        inline int indexof(const volume_node* child)
        {
            assert(child && child->parent == this);
            return int(child == childs[1]);
        }

        /** Get bound of the volume */
        inline const aabb_type& get_bounds() const { return volume; }

        /** Get child node */
        inline const volume_node* get_child(int i) const { return childs[i]; }

    private:
        volume_node( const aabb_type& _volume,
                     const LeafData&  _leafData) :
            leafData(_leafData),
            volume(_volume),
            parent(0)
        {
            childs[0] = 0;
            childs[1] = 0;
        }

        volume_node( const aabb_type& _volume,
                     volume_node*     _parent ) :
            volume(_volume),
            parent(_parent)
        {
            childs[0] = 0;
            childs[1] = 0;
        }

    public:
        LeafData leafData;

    private:
        aabb_type       volume;
	    volume_node*    parent;
        volume_node*    childs[2];
    };

public:
    aabb_tree();
    ~aabb_tree();

    /** Get bounding box of the aabb_tree */
    const math::AABBf& getBounds() const
    {
        if (root) {
            return root->get_bounds();
        }

        return bounds<aabb_type>::inv_infinite();
    }

    /** Check wether tree is empty */
    bool empty() const { return root == 0; }

    /** Get root volume node */
    const volume_node* getRoot() const { return root; }

    /** Insert new element in the tree.
     * @param volume - axis aligned bounding box of the element.
     * @param leafData - data to store in the leaf.
     * @return inserted node.
     */
    volume_node* insert(const aabb_type& volume, const LeafData& leafData);

    /** Remove node from the tree. */
    void remove(volume_node* node);

    /** Update AABB of the node.
     * @param node - node to update.
     * @param volume - new volume.
     */
    void update(const aabb_type& volume, volume_node* node);

    /** Remove all nodes from aabb_tree */
    void clear();

private:
    // insert leaf node in the tree
    void insertLeaf(volume_node* root, volume_node* leaf);

    // remove leaf node from the tree
    void removeLeaf(volume_node* root, volume_node* leaf);

    // Manhattan distance
    inline RealType	proximity(const aabb_type& a, const aabb_type& b)
    {
        const typename aabb_type::vec_type d = (a.minVec + a.maxVec) - (b.minVec + b.maxVec); // vector between centers
	    return abs(d.x) + abs(d.y) + abs(d.z);
    }

    // select closest node from the childs
    inline volume_node* selectClosest(const volume_node* v, volume_node* a, volume_node* b)
    {
        return proximity(v->volume, a->volume) < proximity(v->volume, b->volume) ? a : b;
    }

private:
    volume_node* root;
    size_t       numLeaves;
};

template<typename LeafData, typename RealType>
aabb_tree<LeafData, RealType>::aabb_tree() :
    root(0),
    numLeaves(0)
{
}

template<typename LeafData, typename RealType>
aabb_tree<LeafData, RealType>::~aabb_tree()
{
    clear();
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::insertLeaf(volume_node* root, volume_node* leaf)
{
	if (!root) {
		this->root = leaf;
	}
	else
	{
        while ( !root->is_leaf() ) {
			root = selectClosest(leaf, root->childs[0], root->childs[1]);
        }

        // insert new child
		volume_node* prev = root->parent;
        volume_node* node = new volume_node(math::merge(leaf->volume, root->volume), prev);
		if (prev)
		{
			prev->childs[prev->indexof(root)] = node;
			node->childs[0]				      = root; root->parent = node;
			node->childs[1]				      = leaf; leaf->parent = node;

            // expand hierarchy nodes
            prev = node->parent;
			while ( prev != 0 && !contains(prev->volume, node->volume) )
            {
                prev->volume = math::merge(prev->childs[0]->volume, prev->childs[1]->volume);
                prev         = prev->parent;
			}
		}
		else
		{
			node->childs[0]	= root; root->parent = node;
			node->childs[1]	= leaf; leaf->parent = node;
			this->root	    = node;
		}
	}
}

template<typename LeafData, typename RealType>
typename aabb_tree<LeafData, RealType>::volume_node* aabb_tree<LeafData, RealType>::insert(const aabb_type& volume, const LeafData& leafData)
{
    volume_node* leaf = new volume_node(volume, leafData);
	insertLeaf(root, leaf);
	++numLeaves;
	return leaf;
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::removeLeaf(volume_node* parent, volume_node* leaf)
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
void aabb_tree<LeafData, RealType>::remove(volume_node* node)
{
    assert( node && node->is_leaf() );
	removeLeaf(root, node);
    delete node;
}

template<typename LeafData, typename RealType>
void aabb_tree<LeafData, RealType>::update(const aabb_type& volume, volume_node* node)
{
	removeLeaf(root, node);
    node->volume = volume;
	insertLeaf(root, node);
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

        root      = 0;
        numLeaves = 0;
    }
}

/** Perform function on elements intersecting specified volume.
 * @tparam Volume - type of the volume body(AABB, Frustum, etc.).
 * @param tree - tree for gathering elements.
 * @param volume - volume for gathering.
 * @param iterator - iterator for adding gathered elements.
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

    const volume_node* root = tree.getRoot();
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
                if ( functor(root->leafData) ) {
					return;
				}
            }
        }
    }
}

} // namesapce slon
