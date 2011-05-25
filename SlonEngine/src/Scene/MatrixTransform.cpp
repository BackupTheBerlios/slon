#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/MatrixTransform.h"
#include "Utility/error.hpp"
#include <sgl/Math/MatrixFunctions.hpp>

DECLARE_AUTO_LOGGER("scene.MatrixTransform");

using namespace slon;
using namespace scene;
using namespace math;

MatrixTransform::MatrixTransform() :
    transformDirty(false),
    invTransformDirty(false)
{
	localToWorld = transform    = math::make_identity<float, 4>();
    worldToLocal = invTransform = math::make_identity<float, 4>();
}
	
MatrixTransform::MatrixTransform(const math::Matrix4f& transform_) :
	transform(transform_),
    transformDirty(false),
    invTransformDirty(true)
{
	localToWorld = math::make_identity<float, 4>();
    worldToLocal = math::make_identity<float, 4>();
}

// Override Serializable
const char* MatrixTransform::getSerializableName() const
{
	return "scene::MatrixTransform";
}

void MatrixTransform::serialize(database::OArchive& ar) const
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }
	
	// serialize base class
	Transform::serialize(ar);

	// serialize data
	ar.writeChunk("transform", transform.data(), transform.num_elements);
	ar.writeChunk("invTransform", invTransform.data(), invTransform.num_elements);
	ar.writeChunk("transformDirty", &transformDirty);
	ar.writeChunk("invTransformDirty", &invTransformDirty);
}

void MatrixTransform::deserialize(database::IArchive& ar)
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

	// deserialize base class
	Transform::deserialize(ar);

	// deserialize data
}

// Override transform
const math::Matrix4f& MatrixTransform::getTransform() const
{
    if (transformDirty)
    {
        const_cast<MatrixTransform*>(this)->transform = math::invert(invTransform);
        transformDirty = false;
    }

    return transform;
}

const math::Matrix4f& MatrixTransform::getInverseTransform() const
{
    if (invTransformDirty)
    {
        const_cast<MatrixTransform*>(this)->invTransform = math::invert(transform);
        invTransformDirty = false;
    }

    return invTransform;
}

void MatrixTransform::setTransform(const Matrix4f& matrix)
{
    transform         = matrix;
    invTransformDirty = true;
    update();
}

void MatrixTransform::setInverseTransform(const Matrix4f& matrix)
{
    invTransform   = matrix;
    transformDirty = true;
    update();
}

void MatrixTransform::setTransformAndInverse(const math::Matrix4f& matrix, const math::Matrix4f& invMatrix)
{
    transform      = matrix;
    invTransform   = invMatrix;
    transformDirty = invTransformDirty = false;
    update();
}    

void MatrixTransform::accept(log::LogVisitor& visitor) const
{
    visitor << "MatrixTransform";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "transform =" << log::detailed(getTransform(), true)  
            << "localToWorld =" << log::detailed(getLocalToWorld(), true);
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}