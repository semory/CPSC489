#ifndef __CS_COLLISION_H
#define __CS_COLLISION_H

// AABB
#pragma region AABB

struct alignas(16) AABB_halfdim {
 float center[4];
 float widths[4];
};

inline void load_unit(AABB_halfdim& b)
{
 b.center[0] = 0.0f;
 b.center[1] = 0.0f;
 b.center[2] = 0.0f;
 b.widths[0] = 1.0f;
 b.widths[1] = 1.0f;
 b.widths[2] = 1.0f;
}

inline void scale(AABB_halfdim& b, float value)
{
 b.widths[0] *= value;
 b.widths[1] *= value;
 b.widths[2] *= value;
}

inline void scale(AABB_halfdim& b, const float* v)
{
 b.widths[0] *= v[0];
 b.widths[1] *= v[1];
 b.widths[2] *= v[2];
}

inline void translate(AABB_halfdim& b, const float* v)
{
 b.center[0] += v[0];
 b.center[1] += v[1];
 b.center[2] += v[2];
}

#pragma endregion AABB

// OBB
#pragma region OBB

struct alignas(16) OBB {
 float center[4];
 float x[4];
 float y[4];
 float z[4];
 float widths[4];
 void load_unit(void);
 void scale(float value);
 void scale(const float* v);
 void translate(const float* v);
 void vertices(float b[][3]);
};

inline void OBB::load_unit(void)
{
 center[0] = 0.0f;
 center[1] = 0.0f;
 center[2] = 0.0f;
 x[0] = 1.0f; x[1] = 0.0f; x[2] = 0.0f;
 y[0] = 0.0f; y[1] = 1.0f; y[2] = 0.0f;
 z[0] = 0.0f; z[1] = 0.0f; z[2] = 1.0f;
 widths[0] = 1.0f;
 widths[1] = 1.0f;
 widths[2] = 1.0f;
}

inline void OBB::scale(float value)
{
 widths[0] *= value;
 widths[1] *= value;
 widths[2] *= value;
}

inline void OBB::scale(const float* v)
{
 widths[0] *= v[0];
 widths[1] *= v[1];
 widths[2] *= v[2];
}

inline void OBB::translate(const float* v)
{
 center[0] += v[0];
 center[1] += v[1];
 center[2] += v[2];
}

inline void OBB::vertices(float b[][3])
{
 // +
 float cx1 = center[0] + widths[0]*x[0];
 float cx2 = center[1] + widths[0]*x[1];
 float cx3 = center[2] + widths[0]*x[2];
 // +, +
 float xpy1 = cx1 + widths[1]*y[0];
 float xpy2 = cx2 + widths[1]*y[1];
 float xpy3 = cx3 + widths[1]*y[2];
 // +
 b[0][0] = xpy1 + widths[2]*z[0];
 b[0][1] = xpy2 + widths[2]*z[1];
 b[0][2] = xpy3 + widths[2]*z[2];
 // -
 b[1][0] = xpy1 - widths[2]*z[0];
 b[1][1] = xpy2 - widths[2]*z[1];
 b[1][2] = xpy3 - widths[2]*z[2];
 // +, -
 xpy1 = cx1 - widths[1]*y[0];
 xpy2 = cx2 - widths[1]*y[1];
 xpy3 = cx3 - widths[1]*y[2];
 // +
 b[2][0] = xpy1 + widths[2]*z[0];
 b[2][1] = xpy2 + widths[2]*z[1];
 b[2][2] = xpy3 + widths[2]*z[2];
 // -
 b[3][0] = xpy1 - widths[2]*z[0];
 b[3][1] = xpy2 - widths[2]*z[1];
 b[3][2] = xpy3 - widths[2]*z[2];

 // -
 cx1 = center[0] - widths[0]*x[0];
 cx2 = center[1] - widths[0]*x[1];
 cx3 = center[2] - widths[0]*x[2];
 // -, +
 xpy1 = cx1 + widths[1]*y[0];
 xpy2 = cx2 + widths[1]*y[1];
 xpy3 = cx3 + widths[1]*y[2];
 // +
 b[4][0] = xpy1 + widths[2]*z[0];
 b[4][1] = xpy2 + widths[2]*z[1];
 b[4][2] = xpy3 + widths[2]*z[2];
 // -
 b[5][0] = xpy1 - widths[2]*z[0];
 b[5][1] = xpy2 - widths[2]*z[1];
 b[5][2] = xpy3 - widths[2]*z[2];
 // -, -
 xpy1 = cx1 - widths[1]*y[0];
 xpy2 = cx2 - widths[1]*y[1];
 xpy3 = cx3 - widths[1]*y[2];
 // +
 b[6][0] = xpy1 + widths[2]*z[0];
 b[6][1] = xpy2 + widths[2]*z[1];
 b[6][2] = xpy3 + widths[2]*z[2];
 // -
 b[7][0] = xpy1 - widths[2]*z[0];
 b[7][1] = xpy2 - widths[2]*z[1];
 b[7][2] = xpy3 - widths[2]*z[2];
}

#pragma endregion OBB

// BOUNDING SPHERES
#pragma region BOUNDING_SPHERE

struct alignas(16) BV_sphere {
 float center[3];
 float radius;
};

inline void load_unit(BV_sphere& b)
{
 b.center[0] = 0.0f;
 b.center[1] = 0.0f;
 b.center[2] = 0.0f;
 b.radius    = 1.0f;
}

inline void scale(BV_sphere& b, float value)
{
 b.radius *= value;
}

inline void translate(BV_sphere& b, const float* v)
{
 b.center[0] += v[0];
 b.center[1] += v[1];
 b.center[2] += v[2];
}

#pragma endregion BOUNDING_SPHERE

// AABB INTERSECT WITH X
#pragma region AABB_INTERSECTION

inline bool AABB_intersect(const AABB_halfdim& aabb, const float* v)
{
 // x-axis test
 if(v[0] < (aabb.center[0] - aabb.widths[0])) return false;
 if(v[0] > (aabb.center[0] + aabb.widths[0])) return false;

 // y-axis test
 if(v[1] < (aabb.center[1] - aabb.widths[1])) return false;
 if(v[1] > (aabb.center[1] + aabb.widths[1])) return false;

 // z-axis test
 if(v[2] < (aabb.center[2] - aabb.widths[2])) return false;
 if(v[2] > (aabb.center[2] + aabb.widths[2])) return false;

 return true;
}

inline bool AABB_intersect(const AABB_halfdim& aabb, const BV_sphere& s)
{
 // http://www.mrtc.mdh.se/projects/3Dgraphics/paperF.pdf
 // This intersection test is called the ARVO test. I follow the paper algorithm pretty
 // strictly, but made one slight modification. I do a rejection test after each axis
 // since the squared distances are often pretty large. When the box is far away, the
 // squared distance from closest box-axis to the sphere center can be larger than the
 // squared sphere radius, so why not do the test now?

 // squared distance
 const float squared_radius = s.radius*s.radius;
 float distance = 0.0f;
 float b_min;
 float b_max;
 float d;

 // squared distance from sphere origin to AABB x_min or x_max, depending on which is closer
 b_min = aabb.center[0] - aabb.widths[0];
 b_max = aabb.center[0] + aabb.widths[0];
 if(s.center[0] < b_min) {
    d = s.center[0] - b_min;
    distance += d*d;
   }
 else if(s.center[0] > b_max) {
    d = s.center[0] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // squared distance from sphere origin to AABB y_min or y_max, depending on which is closer
 b_min = aabb.center[1] - aabb.widths[1];
 b_max = aabb.center[1] + aabb.widths[1];
 if(s.center[1] < b_min) {
    d = s.center[1] - b_min;
    distance += d*d;
   }
 else if(s.center[1] > b_max) {
    d = s.center[1] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // squared distance from sphere origin to AABB z_min or z_max, depending on which is closer
 b_min = aabb.center[2] - aabb.widths[2];
 b_max = aabb.center[2] + aabb.widths[2];
 if(s.center[2] < b_min) {
    d = s.center[2] - b_min;
    distance += d*d;
   }
 else if(s.center[2] > b_max) {
    d = s.center[2] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // if squared radius encompasses squared distance sum, it intersects
 return true;
}

inline bool AABB_intersect(const AABB_halfdim& b1, const AABB_halfdim& b2)
{
 // x-axis test
 if((b2.center[0] + b2.widths[0]) < (b1.center[0] - b1.widths[0])) return false;
 if((b1.center[0] + b1.widths[0]) < (b2.center[0] - b2.widths[0])) return false;

 // y-axis test
 if((b2.center[1] + b2.widths[1]) < (b1.center[1] - b1.widths[1])) return false;
 if((b1.center[1] + b1.widths[1]) < (b2.center[1] - b2.widths[1])) return false;

 // z-axis test
 if((b2.center[2] + b2.widths[2]) < (b1.center[2] - b1.widths[2])) return false;
 if((b1.center[2] + b1.widths[2]) < (b2.center[2] - b2.widths[2])) return false;
}

#pragma endregion AABB_INTERSECTION

// OBB INTERSECT WITH X
#pragma region OBB_INTERSECTION

inline bool OBB_intersect(const OBB& obb, const float* v)
{
 // move OBB coordinate system to origin
 // p[0] = v[0] - obb.center[0],
 // p[1] = v[1] - obb.center[1],
 // p[2] = v[2] - obb.center[2]

 // this is OBB matrix
 // obb.x[0] obb.x[1] obb.x[2]
 // obb.y[0] obb.y[1] obb.y[2]
 // obb.z[0] obb.z[1] obb.z[2]

 // this is OBB matrix inverse (transpose)
 // obb.x[0] obb.y[0] obb.z[0]
 // obb.x[1] obb.y[1] obb.z[1]
 // obb.x[2] obb.y[2] obb.z[2]

 // to align OBB axes to world space axes, multiply OBB coordinate system by its matrix inverse
 // the result is an AABB test
 // | obb.x[0] obb.y[0] obb.z[0] |   | p[0] |   | obb.x[0]*p[0] + obb.y[0]*p[1] + obb.z[0]*p[2] |
 // | obb.x[1] obb.y[1] obb.z[1] | * | p[1] | = | obb.x[1]*p[0] + obb.y[1]*p[1] + obb.z[1]*p[2] |
 // | obb.x[2] obb.y[2] obb.z[2] |   | p[2] |   | obb.x[2]*p[0] + obb.y[2]*p[1] + obb.z[2]*p[2] |

 // move OBB to origin
 float p[3] = {
  v[0] - obb.center[0],
  v[1] - obb.center[1],
  v[2] - obb.center[2]
 };

 // align OBB to world (multiplying by inverse)
 float q[3] = {
  obb.x[0]*p[0] + obb.y[0]*p[1] + obb.z[0]*p[2],
  obb.x[1]*p[0] + obb.y[1]*p[1] + obb.z[1]*p[2],
  obb.x[2]*p[0] + obb.y[2]*p[1] + obb.z[2]*p[2] 
 };

 // x-axis test
 if(q[0] < -obb.widths[0]) return false;
 if(q[0] > +obb.widths[0]) return false;

 // y-axis test
 if(q[1] < -obb.widths[1]) return false;
 if(q[1] > +obb.widths[1]) return false;

 // z-axis test
 if(q[2] < -obb.widths[2]) return false;
 if(q[2] > +obb.widths[2]) return false;

 return true;
}

inline bool OBB_intersect(const OBB& obb, const BV_sphere& s)
{
 // translate OBB to origin
 float so[3] = {
  s.center[0] - obb.center[0],
  s.center[1] - obb.center[1],
  s.center[2] - obb.center[2]
 };

 // align OBB to world (multiplying by inverse)
 // now an AABB-vs-sphere test
 float dots[3] = {
  obb.x[0]*so[0] + obb.y[0]*so[1] + obb.z[0]*so[2],
  obb.x[1]*so[0] + obb.y[1]*so[1] + obb.z[1]*so[2],
  obb.x[2]*so[0] + obb.y[2]*so[1] + obb.z[2]*so[2] 
 };

 // squared distance
 const float squared_radius = s.radius*s.radius;
 float distance = 0.0f;
 float b_min;
 float b_max;
 float d;

 // squared distance from sphere origin to AABB x_min or x_max, depending on which is closer
 b_min = -obb.widths[0];
 b_max = +obb.widths[0];
 if(so[0] < b_min) {
    d = so[0] - b_min;
    distance += d*d;
   }
 else if(so[0] > b_max) {
    d = so[0] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // squared distance from sphere origin to AABB y_min or y_max, depending on which is closer
 b_min = -obb.widths[1];
 b_max = +obb.widths[1];
 if(so[1] < b_min) {
    d = so[1] - b_min;
    distance += d*d;
   }
 else if(so[1] > b_max) {
    d = so[1] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // squared distance from sphere origin to AABB z_min or z_max, depending on which is closer
 b_min = -obb.widths[2];
 b_max = +obb.widths[2];
 if(so[2] < b_min) {
    d = so[2] - b_min;
    distance += d*d;
   }
 else if(so[2] > b_max) {
    d = so[2] - b_max;
    distance += d*d;
   }
 if(squared_radius < distance) return false;

 // if squared radius encompasses squared distance sum, it intersects
 return true;
}

#pragma endregion OBB_INTERSECTION

#endif