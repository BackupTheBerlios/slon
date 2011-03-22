#if defined(ENABLE_NORMAL_HEIGHT_MAP) || defined(ENABLE_DIFFUSE_SPECULAR_MAP)
#define ENABLE_TEXTURING
#endif

#define N     64
#define EPS   0.001

// attributes
attribute vec4  bone_weight;
attribute vec4  bone_index;
attribute vec4  position;
attribute vec3 	normal;
attribute vec2	texcoord;

// uniforms
uniform mat4  worldViewMatrix;
uniform mat4  projectionMatrix;
uniform mat4  worldViewProjMatrix;
uniform mat3  normalMatrix;
#ifdef USE_BONE_MATRICES
uniform mat4  boneMatrices[N];
#else
uniform vec4  boneRotations[N];
uniform vec3  boneTranslations[N];
#endif

// varyings
#ifndef DEPTH_ONLY
varying vec3  	fp_position;
#ifndef ENABLE_NORMAL_HEIGHT_MAP
varying vec3	fp_normal;
#endif
#ifdef ENABLE_TEXTURING
varying	vec2	fp_texcoord;
#endif
#endif // DEPTH_ONLY

vec4 quat_mul(in vec4 q1, in vec4 q2)
{
    vec3 v1 = q1.xyz;
    vec3 v2 = q2.xyz;
    return vec4( cross(v1, v2) + q1.w * v2 + q2.w * v1, q1.w * q2.w - dot(v1, v2) );
}

vec4 conjugate(in vec4 q)
{
	return vec4(-q.x, -q.y, -q.z, q.w);
}

vec3 quat_rotate(in vec3 p, in vec4 q)
{
    vec4 temp = quat_mul( conjugate(q), vec4(p, 0.0) );
    return quat_mul(temp, q).xyz;
}

vec3 transform_position(in int bone, in vec4 pos)
{
#ifdef USE_BONE_MATRICES
    return (boneMatrices[bone] * pos).xyz;
#else
	return boneTranslations[bone] + quat_rotate(pos.xyz, boneRotations[bone]);
#endif
}

vec3 transform_normal(in int bone, in vec3 normal)
{
#ifdef USE_BONE_MATRICES
    return ( boneMatrices[bone] * vec4(normal, 0.0) ).xyz;
#else
	return quat_rotate(normal, boneRotations[bone]);
#endif
}

void main()
{
    vec4 pos  = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 norm = vec3(0.0, 0.0, 0.0);
    int  index;

    if (bone_weight.x > EPS)
    {
        index    = int(bone_index.x);
        pos.xyz += bone_weight.x * transform_position(index, position);
        norm    += bone_weight.x * transform_normal(index, normal);
    }

    if (bone_weight.y > EPS)
    {
        index    = int(bone_index.y);
        pos.xyz += bone_weight.y * transform_position(index, position);
        norm    += bone_weight.y * transform_normal(index, normal);
    }

    if (bone_weight.z > EPS)
    {
        index    = int(bone_index.z);
        pos.xyz += bone_weight.z * transform_position(index, position);
        norm    += bone_weight.z * transform_normal(index, normal);
    }

    if (bone_weight.w > EPS)
    {                                          
        index    = int(bone_index.w);
        pos.xyz += bone_weight.w * transform_position(index, position);
        norm    += bone_weight.w * transform_normal(index, normal);
    }

#ifdef DEPTH_ONLY
	gl_Position = worldViewProjMatrix * pos;
	
#else // DEPTH_ONLY
    fp_position = (worldViewMatrix * pos).xyz;

#ifndef ENABLE_NORMAL_HEIGHT_MAP
    fp_normal   = normalMatrix * norm;
#endif

#ifdef ENABLE_TEXTURING
    fp_texcoord = texcoord;
#endif

    gl_Position = worldViewProjMatrix * pos;
#endif // DEPTH_ONLY
}
