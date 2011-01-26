#ifndef __SLON_ENGINE_PHYSICS_ALGORITHM_H__
#define __SLON_ENGINE_PHYSICS_ALGORITHM_H__

#include <boost/iterator/iterator_facade.hpp>
#include <sgl/Math/AABB.hpp>
#include <sgl/Math/Plane.hpp>

namespace slon {

/** Iterator for composing vertices from float tuples */
template< typename VertexType,
          typename FloatIteratorType,
          size_t   NumVertexComponents = VertexType::num_elements >
struct vertex_compose_iterator :
    public boost::iterator_facade<
        vertex_compose_iterator<VertexType, FloatIteratorType>,
        VertexType,
        boost::bidirectional_traversal_tag
    >
{
friend class boost::iterator_core_access;
public:
    typedef VertexType          vertex_type;
    typedef FloatIteratorType   float_iterator_type;

    /** Construct iterator.
     * @param _stride - stride in the float array between vertices.
     * @param _floatIter - iterator in the float array.
     */
    vertex_compose_iterator(const float_iterator_type& _floatIter, size_t _stride) :
        stride(_stride),
        floatIter(_floatIter)
    {}

private:
    void increment() { floatIter += stride; }
    void decrement() { floatIter -= stride; }

    bool equal(const vertex_compose_iterator& other) const
    {
        return stride == other.stride && floatIter == other.floatIter;
    }

    vertex_type& dereference() const
    {
        for(size_t i = 0; i<NumVertexComponents; ++i) {
            vertex[i] = *(floatIter + i);
        }
        return vertex;
    }

private:
    size_t                 stride;
    float_iterator_type    floatIter;
    mutable vertex_type    vertex;
};

template<typename T, int n, typename IteratorType>
vertex_compose_iterator<math::Matrix<T, n, 1>, IteratorType, 3>
vector_composer(const IteratorType& iterator, size_t stride = 3)
{
    typedef vertex_compose_iterator<math::Matrix<T, n, 1>, IteratorType, n> iterator_type;
    return iterator_type(iterator, stride);
}

/** Convert floating point tuple to vector. Tuple must overload operator [] */
template<typename Tuple>
math::Vector3f to_vec(const Tuple& tuple)
{
    return math::Vector3f(tuple[0], tuple[1], tuple[2]);
}

template<typename RealType, int n>
const math::Matrix<RealType, n, 1>& to_vec(const math::Matrix<RealType, n, 1>& vec) { return vec; }

template<typename RealType, int n>
const math::Matrix<RealType, 1, n>& to_vec(const math::Matrix<RealType, 1, n>& vec) { return vec; }

/** Compute axis aligned bounding box of the point cloud
 * @tparam Iterator - iterator type referencing at least
 * 3-component floating tuple. Tuple must overload operator [].
 * @param beginIter - point cloud begin iterator.
 * @param endIter - point cloud end iterator.
 */
template<typename T, typename Iterator>
math::AABB<T, 3> compute_aabb( const Iterator& beginIter,
							   const Iterator& endIter )
{
    assert(beginIter != endIter && "Point cloud must have at least one point.");

    math::AABB<T, 3> aabb( to_vec(*beginIter), to_vec(*beginIter) );
    for (Iterator i = beginIter; i != endIter; ++i)
    {
        aabb.extend( to_vec(*i) );
    }

    return aabb;
}

/** Transform Array of vertices by matrix
 * @tparam InIterator - iterator type referencing at least
 * n-component floating tuple. Tuple must be convertible to Vector<RealType, n, 1>.
 * @tparam OutIterator - iterator type referencing at least
 * n-component floating tuple. Tuple must be convertible to Vector<RealType, n, 1>.
 * @param beginIter - vertices begin iterator.
 * @param endIter - vertices end iterator.
 * @param outIter - output iterator
 * @param matrix - transformation matrix
 */
template< typename InIterator, 
          typename OutIterator,
          typename RealType,
          int n,
          int m>
void transform_by_matrix( const InIterator&                     beginIter,
                          const InIterator&                     endIter,
                          OutIterator                           outIter,
                          const math::Matrix<RealType, n, m>&   matrix )
{
    for (InIterator i = beginIter; i != endIter; ++i) {
        (*outIter++) = matrix * (*i);
    }
}

/** Remove element from the container swapping it with back and poping.
 * @return true if element removed
 */
template<typename Container, typename T>
bool quick_remove(Container& container, T val)
{
    typename Container::iterator iter = std::find(container.begin(), container.end(), val);
    if ( iter != container.end() )
    {
        std::swap( *iter, container.back() );
        container.pop_back();
        return true;
    }

    return false;
}

/** Remove element matching predicate from the container swapping it with back and poping.
 * @return true if element removed
 */
template<typename Container, typename Predicate>
bool quick_remove_if(Container& container, Predicate pred)
{
    typename Container::iterator iter = std::find_if(container.begin(), container.end(), pred);
    if ( iter != container.end() )
    {
        std::swap( *iter, container.back() );
        container.pop_back();
        return true;
    }

    return false;
}

/** Build convex hull of the point cloud inplace. Implement QuickHull
 * algorithm inplace.
 * @tparam Iterator - random access iterator type referencing at least
 * 3-component floating tuple. Tuple must overload operator [].
 * @param beginIter - point cloud begin iterator.
 * @param endIter - point cloud end iterator.
 * @return new end iterator of the point cloud. Returns beginIter if failed to
 * construct convex hull. This happens if vertices are coplanar.
 */
template<typename Iterator>
Iterator build_convex_hull( const Iterator& beginIter,
                            const Iterator& endIter )
{/*
    using namespace math;

    // point cloud at least must form tetrahedron
    if (endIter - beginIter < 4) {
        return beginIter;
    }

    // Initial. Find maximum tetrahedron. Store it in the first four
    // vertices of the point cloud.
    {
        Vector3f polyhedron[3];

        // find first two points as corners of the AABB
        Iterator secondIter = beginIter + 1;
        for (int axis = 0; axis < 3; ++axis)
        {
            for (Iterator i = beginIter + 2; i != endIter; ++i)
            {
                if ( (*i)[axis] > *(beginIter)[axis] ) {
                    std::swap(*i, *beginIter);
                }
                else if ( (*i)[axis] < *(secondIter)[axis] ) {
                    std::swap(*i, *secondIter);
                }
            }

            if (*beginIter != *secondIter) {
                break;
            }
        }

        // convex hull is point
        if (*beginIter == *secondIter) {
            return beginIter;
        }

        // find 3rd as most distant point from the line
        // formed by first two points.
        Vector3f direction   = normalize(polyhedron[1] - polyhedron[0]);
        maxDistance          = 0.0f;
        Iterator thirdIter   = beginIter + 2;
        for (Iterator i = beginIter + 3; i != endIter; ++i)
        {
            Vector3f point = to_vec(*i);
            float    dist  = abs( cross(direction, point) );
            if (dist > maxDistance)
            {
                masDistance = dist;
                std::swap(thirdIter, *i);
            }
        }

        // convex hull is 1D
        if (maxDistance == 0.0f) {
            return beginIter;
        }

        // find 4rth as most distant point from the plane
        // formed by first three points.
        Planef   plane       = Planef(polyhedron[0], polyhedron[1], polyhedron[2]);
        float    maxDistance = 0.0f;
        Iterator fourthIter  = beginIter + 3;
        for (Iterator i = beginIter + 4; i != endIter; ++i)
        {
            Vector3f point = to_vec(*i);
            if ( plane.distance_to_point(point) > maxDistance ) {
                std::swap(*fourthIter, *i);
            }
        }

        // convex hull is 2D
        if (maxDistance == 0.0f) {
            return beginIter;
        }
    }

    class convex_builder
    {
    private:
        typedef std::vector<int> int_vector;

        struct convex_face
        {
            convex_face( const Iterator&   v0,
                         const Iterator&   v1,
                         const Iterator&   v2,
                         const int_vector& unclaimedVertices )
            {
                Vector3f dirA   = to_vec(*v1) - to_vec(*v0);
                Vector3f dirB   = to_vec(*v2) - to_vec(*v0);
                Vector3f dir0   = to_vec(*v0) - center;
                Vector3f normal = cross(dirA, dirB);

                // normal must see outside the convex
                if ( dot(dir0, normal) > 0.0f )
                {
                    vertices[0] = v0;
                    vertices[1] = v1;
                    vertices[2] = v2;
                }
                else
                {
                    vertices[0] = v0;
                    vertices[1] = v2;
                    vertices[2] = v1;
                }

                plane = Planef( normalize(normal), to_vec(*v0) );

                // Add points to outside set. Move inside points at the end
                Iterator garbageIter = endUnclaimedIter;
                beginIter = beginOutsideIter = endOutsideIter = unclamedIter;
                for (size_t i = 0; i<unclaimedVertices.size(); ++i)
                {
                    Iterator vertex = unclaimed
                    if ( plane.distance_to_point(*i) > 0.0f ) {
                        std::swap(*endOutsideIter++, *i++);
                    }
                    else {
                        std::swap( *(--garbageIter), *i );
                    }
                }
            }

            //bool outside_empty() const { return beginOutsideIter == endOutsideIter; }

            math::Planef    plane;       // triangle plane
            Iterator        vertices[3]; // triangle vertices in clockwise order
            int_vector      outsideIndices;
        };

        Iterator find_farthest(const Planef& plane, const Iterator& beginIter, const Iterator& endIter)
        {
            Iterator farthestIter = beginIter;
            float    maxDistance  = 0.0f;
            for (Iterator i = beginIter; i != endIter; ++i)
            {
                float distance = plane.distance_to_point( to_vec(*i) );
                if (distance > maxDistance)
                {
                    maxDistance  = distance;
                    farthestIter = i;
                }
            }

            return farthestIter;
        }

        void remove_face(size_t faceIndex)
        {
            // remove face vertices
        }

        void add_to_outside_set( const  Iterator& unclaimedIter,
                                 const  Iterator& endIter,
                                 size_t faceIndex )
        {
            // empty outside set
            for (size_t i = faceIndex; i<faces.size(); ++i)
            {

            }

            for (Iterator i = unclaimedIter; i != endIter; ++i)
            {
                for (size_t i = faceIndex; i<faces.size(); ++i)
                {
                    if ( faces[i].plane.distance_to_point(*i) > 0.0f ) {
                        std::swap(*endOutsideIter++, *i);
                    }
                }
            }
        }

        void build_impl()
        {
            for (size_t i = 0; i < faces.size(); nothing)
            {
                if ( !faces[i].outside_empty() )
                {
                    // find farthest point, remove it from the outside set
                    Iterator farthestIter = find_farthest(faces[i].plane, faces[i].beginOutsideIter, faces[i].endOutsideIter);
                    std::swap(*faces[i].beginOutsideIter, *farthestIter);
                    farthestIter = faces[i].beginOutsideIter++;

                    // remove faces
                }
                else {
                    ++i;
                }
            }
        }

    public:
        convex_builder(const Iterator& unclamedIter, const Iterator& endIter)
        {
            center = Vector3f(0.0f);
            for (Iterator i = unclamedIter; i != unclamedIter + 5; ++i) {
                center += to_vec(*i);
            }
            center /= 4.0f;

            // build tetrahedron
            faces[0] = convex_face(unclamedIter,     unclamedIter + 1, unclaimedIter + 2, unclaimedIter,           endConvexIter);
            faces[1] = convex_face(unclamedIter,     unclamedIter + 1, unclaimedIter + 3, faces[0].endOutsideIter, endConvexIter);
            faces[2] = convex_face(unclamedIter,     unclamedIter + 2, unclaimedIter + 3, faces[1].endOutsideIter, endConvexIter);
            faces[3] = convex_face(unclamedIter + 1, unclamedIter + 2, unclaimedIter + 3, faces[2].endOutsideIter, endConvexIter);

            // cut vertices inside tetrahedron
            endConvexIter = faces[3].endOutsideIter;
        }

        Iterator operator () ()
        {
            build_impl();

            // merge convex

            return endConvexIter;
        }

    private:
        Iterator                    unclaimedIter;
        Iterator                    endConvexIter;
        Vector3f                    center; // center of the inital tetrahedron(always inside convex)
        std::vector<convex_face>    faces;
    };

    // convex builder
    convex_builder builder(beginIter + 5, endIter);
    return builder();*/
    return beginIter;
}

} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_ALGORITHM_H__
