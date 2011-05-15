#ifndef SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_SUBSET
#define SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_SUBSET

#include <boost/shared_array.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <sgl/IndexBuffer.h>
#include <sgl/Math/AABB.hpp>
#include <sgl/Program.h>
#include <sgl/VertexBuffer.h>
#include <vector>
#include "../Detail/AttributeTable.h"
#include "../Effect.h"
#include "../Renderable.h"

namespace slon {
namespace graphics {
	
/** Mesh representation convenient for storing. */
class MeshData :
    public Referenced
{
friend class Mesh;
public:
    static const int MAX_NUM_ATTRIBUTES = 16;

    struct attribute_array
    {
        std::string                 name;
        unsigned                    size;
        unsigned                    count;
        sgl::SCALAR_TYPE            type;
        boost::shared_array<char>   data;

        attribute_array(unsigned            _size   = 0,
                        unsigned            _count  = 0,
                        sgl::SCALAR_TYPE    _type   = sgl::UNKNOWN ) :
            size(_size),
            count(_count),
            type(_type)
        {}

        size_t attribute_size() const
        {
            return size * sgl::SCALAR_TYPE_TRAITS[type].sizeInBits / 8;
        }
    };

    struct indices_array
    {
        unsigned                        count;
        boost::shared_array<unsigned>   indices;

        indices_array(unsigned _count = 0) :
            count(_count)
        {}
    };

public:
	/** Get attributes array by index */
	const attribute_array& getAttributes(unsigned index) const;
	
	/** Get indices array by index */
	const indices_array& getIndices(unsigned index) const;

    /** Append attribute array to the vertex array.
     * @param index - attribute index.
     * @param size - number of attribute components.
     * @param count - number of attributes in the array.
     * @param type - type of the attribute component.
     * @param data - attribute data.
     */
    void setAttributes( const std::string&  name,
                        unsigned            index,
                        unsigned            size,
                        unsigned            count,
                        sgl::SCALAR_TYPE    type,
                        const void*         data );

    /** Setup indices of attributes.
     * @param attribute - attribute for indexing.
     * @param indices - indices for attribute.
     */
    void setIndices( unsigned           attribute,
                     unsigned           numIndices,
                     const unsigned*    indices );

    /** Get number of specified attributes. */
    unsigned getcount(unsigned attribute);

    /** Try convert attribute array to specified type. */
    template<typename AttributeType>
    const AttributeType* queryAttributeData(unsigned attribute) const;

private:
    attribute_array     attributeArrays[MAX_NUM_ATTRIBUTES];
    indices_array       indicesArrays[MAX_NUM_ATTRIBUTES];
};

typedef boost::intrusive_ptr<MeshData>			mesh_data_ptr;
typedef boost::intrusive_ptr<const MeshData>	mesh_data_const_ptr;

/** Mesh representation convinient for rendering */
class Mesh :
    public Referenced
{
public:
    static const int LOCK_READ  = 1;
    static const int LOCK_WRITE = 1 << 1;

    class buffer_lock_impl
    {
    public:
        buffer_lock_impl(sgl::Buffer* buffer,
                         const int    mask);
        ~buffer_lock_impl();

        void* data() { return ptr; }

    private:
        void*                     ptr;
        sgl::ref_ptr<sgl::Buffer> buffer;
    };

    typedef boost::shared_ptr<buffer_lock_impl> buffer_lock;

    enum SEMANTIC
    {
        POSITION,
		NORMAL,
        TANGENT,
		COLOR,
        BONE_INDEX,
        BONE_WEIGHT,
        ATTRIBUTE,
		TEXCOORD,
    };

    struct attribute
    {
        detail::AttributeTable::binding_ptr binding;
        unsigned                            size;
        unsigned                            offset;
        SEMANTIC                            semantic;
        sgl::SCALAR_TYPE                    type;
    };

    typedef std::vector<attribute>              attribute_vector;
    typedef attribute_vector::iterator          attribute_iterator;
    typedef attribute_vector::const_iterator    attribute_const_iterator;

    /** Description for constructing mesh. */
    struct DESC
    {
        math::AABBf                     aabb;           /// AABB covering mesh
        size_t                          vertexSize;     /// Size of the vertex
        size_t                          numAttributes;  /// Number of attributes in the mesh vertex buffer
        const attribute*                attributes;     /// Array of attributes
        sgl::IndexBuffer::INDEX_TYPE    indexType;      /// Type of the indices in the index buffer
        sgl::VertexBuffer*              vertexBuffer;
        sgl::IndexBuffer*               indexBuffer;
    };

    struct subset :
        public Referenced,
        public Renderable
    {
    friend class Mesh;
    protected:
	    subset( Mesh*   _mesh,
			    Effect* _effect) :
            mesh(_mesh),
            effect(_effect)
        {}
        virtual ~subset() {}

    public:
        // Override Renderable
        Effect* getEffect() const       { return effect.get(); }

        /** Setup effect for rendering subset */
        void setEffect(Effect* _effect) { effect.reset(_effect); }

    protected:
        Mesh*       mesh;
        effect_ptr  effect;
    };

    typedef boost::intrusive_ptr<subset>    subset_ptr;
    typedef std::vector<subset_ptr>         subset_vector;
    typedef subset_vector::iterator         subset_iterator;
    typedef subset_vector::const_iterator   subset_const_iterator;

public:
    struct plain_subset :
        public subset
    {
    friend class Mesh;
    private:
	    plain_subset( Mesh*                 mesh,
				      Effect*               effect,
                      sgl::PRIMITIVE_TYPE   _primitiveType,
                      unsigned              _startVertex,
                      unsigned              _numVertices ) :
            subset(mesh, effect),
            primitiveType(_primitiveType),
            startVertex(_startVertex),
            numVertices(_numVertices)
        {}

    public:
        // Override Renderable
        void render() const;

		sgl::PRIMITIVE_TYPE getPrimitiveType() const { return primitiveType; }

    private:
        sgl::PRIMITIVE_TYPE primitiveType;
        unsigned            startVertex;
        unsigned            numVertices;
    };

    struct indexed_subset :
        public subset
    {
    friend class Mesh;
    private:
	    indexed_subset( Mesh*               mesh,
				        Effect*             effect,
                        sgl::PRIMITIVE_TYPE _primitiveType,
                        unsigned            _startIndex,
                        unsigned            _numIndices ) :
            subset(mesh, effect),
            primitiveType(_primitiveType),
            startIndex(_startIndex),
            numIndices(_numIndices)
        {}

    public:
        // Override Renderable
        void render() const;
		
		sgl::PRIMITIVE_TYPE getPrimitiveType() const { return primitiveType; }

		unsigned getStartIndex() const { return startIndex; }

		unsigned getNumIndices() const { return numIndices; }

    private:
        sgl::PRIMITIVE_TYPE primitiveType;
        unsigned            startIndex;
        unsigned            numIndices;
    };

    template<typename T>
    class accessor
    {
    private:
        template<typename Y>
        class iterator_impl :
            public boost::iterator_facade<
                iterator_impl<Y>,
                Y,
                boost::random_access_traversal_tag
            >
        {
        private:
            friend class accessor;
            friend class boost::iterator_core_access;

        private:
            iterator_impl(void* data_, size_t offset, size_t stride_) :
                data((char*)data_),
                stride(stride_)
            {
                data += offset;
            }

            void increment()            { data += stride; }            
            void decrement()            { data -= stride; }
            void advance(ptrdiff_t n)   { data += stride * n; }

            ptrdiff_t distance_to(const iterator_impl& iter) const
            {
                assert( abs(iter.data - data) % stride == 0 );
                return (iter.data - data) / static_cast<ptrdiff_t>(stride);
            }

            Y&   dereference() const                    { return (*reinterpret_cast<Y*>(data)); }
            bool equal(const iterator_impl& iter) const { return data == iter.data && stride == iter.stride; }

        private:
            char*   data;
            size_t  stride;
        };

    public:
        typedef iterator_impl<T>        iterator;
        typedef iterator_impl<const T>  const_iterator;

    public:
        accessor( Mesh*                     mesh_, 
                  attribute_const_iterator  attr,
                  const buffer_lock&        lock_ ) :
            mesh(mesh_),
            lock(lock_),
            offset(attr->offset)
        {
            stride = mesh->getVertexSize();
        }

        accessor( Mesh*                 mesh_, 
                  const buffer_lock&    lock_ ) :
            mesh(mesh_),
            lock(lock_),
            offset(0)
        {
            stride = sizeof(T);
        }

        // item access
        T&       operator [] (unsigned i)       { return *reinterpret_cast<T*>( (char*)lock->data() + stride * i + offset); }
        const T& operator [] (unsigned i) const { return *reinterpret_cast<const T*>( (char*)lock->data() + stride * i + offset); }

        // iterator access
        iterator        begin()         { return iterator(lock->data(), offset, stride); }
        const_iterator  begin() const   { return iterator(lock->data(), offset, stride); }
        iterator        end()           { return begin() + size(); }
        const_iterator  end() const     { return begin() + size(); }
        size_t          size() const    { return mesh->getNumVertices(); }

    private:
        Mesh*       mesh;
        buffer_lock lock;
        size_t      stride;
        size_t      offset;
    };

    typedef accessor<math::Vector4f>    vec4f_accessor;
    typedef accessor<math::Vector3f>    vec3f_accessor;
    typedef accessor<math::Vector2f>    vec2f_accessor;
    typedef accessor<float>             float_accessor;

    typedef accessor<math::Vector4i>    vec4i_accessor;
    typedef accessor<math::Vector3i>    vec3i_accessor;
    typedef accessor<math::Vector2i>    vec2i_accessor;
    typedef accessor<int>               int_accessor;
    typedef accessor<unsigned int>      uint_accessor;

    typedef accessor<math::Vector4s>    vec4s_accessor;
    typedef accessor<math::Vector3s>    vec3s_accessor;
    typedef accessor<math::Vector2s>    vec2s_accessor;
    typedef accessor<short>             short_accessor;
    typedef accessor<unsigned short>    ushort_accessor;

    typedef accessor<math::Vector4c>    vec4c_accessor;
    typedef accessor<math::Vector3c>    vec3c_accessor;
    typedef accessor<math::Vector2c>    vec2c_accessor;
    typedef accessor<char>              char_accessor;
    typedef accessor<unsigned char>     uchar_accessor;

private:
    // noncopyable
    Mesh(const Mesh&);
    Mesh& operator = (const Mesh&);

public:
	Mesh(const MeshData* meshData);
    Mesh(const DESC& desc);

    /** Add primitives subset to the mesh.
     * @param effect - effect for rendering subset.
     * @param primitiveType - sgl primitive type.
     * @param startVertex - starting vertex in the vertex array.
     * @param numVertices - number of vertices comprising the subset.
     */
    plain_subset* addPlainSubset( Effect*             effect,
                                  sgl::PRIMITIVE_TYPE primitiveType,
                                  unsigned            startVertex,
                                  unsigned            numVertices );

    /** Add indexed subset to the mesh.
     * @param effect - effect for rendering subset.
     * @param primitiveType - sgl primitive type.
     * @param startIndex - start index in the index buffer.
     * @param numIndices - number of indices comprising the subset.
     */
    indexed_subset* addIndexedSubset( Effect*             effect,
                                      sgl::PRIMITIVE_TYPE primitiveType,
                                      unsigned            startIndex,
                                      unsigned            numIndices );

    /** Remove subset from the mesh.
     * @return true if mesh succesfully removed.
     */
    bool removeSubset(subset* meshSubset);

    /** Get iterator addressing first vertex attribute definition. */
    attribute_const_iterator firstAttribute() const { return attributes.begin(); }

    /** Get iterator addressing end vertex attribute definition. */
    attribute_const_iterator endAttribute() const   { return attributes.end(); }

    /** Get iterator addressing first subset of the mesh. */
    subset_iterator  firstSubset()  { return subsets.begin(); }

    /** Get iterator addressing end subset of the mesh. */
    subset_iterator  endSubset()    { return subsets.end(); }

    /** Get iterator addressing first subset of the mesh. */
    subset_const_iterator  firstSubset() const  { return subsets.begin(); }

    /** Get iterator addressing end subset of the mesh. */
    subset_const_iterator  endSubset() const    { return subsets.end(); }

    /** Get i'th mesh subset. */
    subset& getSubset(unsigned i)           { assert(i < subsets.size() ); return *subsets[i]; }

    /** Get circumscribed AABB. */
    const math::AABBf& getBounds() const    { return aabb; }

    /** Get size of the single vertex of the mesh. */
    size_t getVertexSize() const            { return vertexSize; }

    /** Get number of vertices in the mesh */
    size_t getNumVertices() const           { return vertexBuffer->Size() / vertexSize; }

    sgl::IndexBuffer::INDEX_TYPE getIndexType() const   { return indexType; }

    /** Lock mesh vertex data. Lock fails if buffer is already locked. */
    buffer_lock lockVertexBuffer(int mask)  { return buffer_lock(new buffer_lock_impl(vertexBuffer.get(), mask)); }

    /** Lock mesh index data. Lock fails if buffer is already locked. */
    buffer_lock lockIndexBuffer(int mask)   { return buffer_lock(new buffer_lock_impl(indexBuffer.get(), mask)); }

    /** Create shallow copy of the mesh. This includes copying
     * pointers to the IBO, VBO and cloning subsets.
     */
    Mesh* shallowCopy() const;

	/** Get data used to contruct mesh */
	const MeshData* getData() const { return data.get(); }

private:
    void dirtyVertexLayout();

private:
    math::AABBf                         aabb;
    sgl::IndexBuffer::INDEX_TYPE        indexType;
    sgl::ref_ptr<sgl::VertexLayout>     vertexLayout;
    sgl::ref_ptr<sgl::VertexBuffer>     vertexBuffer;
    sgl::ref_ptr<sgl::IndexBuffer>      indexBuffer;

    size_t                              vertexSize;
	subset_vector                       subsets;
    attribute_vector                    attributes;
	mesh_data_const_ptr					data;
};

typedef boost::intrusive_ptr<Mesh>          mesh_ptr;
typedef boost::intrusive_ptr<const Mesh>    const_mesh_ptr;

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_RENDERABLE_MESH_SUBSET
