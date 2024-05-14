
#include "shapes/snowman.h"
#include "sampling.h"
#include "paramset.h"
#include "efloat.h"
#include "stats.h"
#include <iostream> // for print


namespace pbrt {
// STAT_COUNTER("SNOWMAN/SNOWMAN_OBJECT_INTERSECTION1", nAnyObjectIntersectionTests1);
// STAT_COUNTER("SNOWMAN/SNOWMAN_OBJECT_INTERSECTION2", nAnyObjectIntersectionTests2);
// STAT_COUNTER("SNOWMAN/SNOWMAN_ANY_SHAPE_INTERSECTION", nCompositeShapeIntersectionTests);
// STAT_COUNTER("SNOWMAN/SNOWMAN_BOTH_SHAPE_INTERSECTION", nBothShapeIntersectionTests);
// std::atomic<bool> anyIntersectionOccurred(false);

// SnowMan Method Definitions
SnowMan::SnowMan(const Transform *ObjectToWorld, const Transform *WorldToObject,
           bool reverseOrientation,
           Float radiusH, Float radiusB,
           Point3f &posH, Point3f &posB,
           Float phiMax): Shape(ObjectToWorld, WorldToObject, reverseOrientation),
            radiusH(radiusH),
            radiusB(radiusB),
            posH(posH),
            posB(posB),
            phiMax(Radians(Clamp(phiMax, 0, 360))),
            object2worldHead((*ObjectToWorld) * Translate(Vector3f(posH.x, posH.y, posH.z))),
            world2objectHead((*WorldToObject) * Inverse(Translate(Vector3f(posH.x, posH.y, posH.z)))),
            object2worldBody((*ObjectToWorld) * Translate(Vector3f(posB.x, posB.y, posB.z))),
            world2objectBody((*WorldToObject) * Inverse(Translate(Vector3f(posB.x, posB.y, posB.z)))),
            sphereHead(&object2worldHead, &world2objectHead, false, radiusH, -radiusH, radiusH, phiMax),  // 360.f
            sphereBody(&object2worldBody, &world2objectBody, false, radiusB, -radiusB, radiusB, phiMax){}


SnowMan::~SnowMan()
{
    // yf::MyStatManager::ReportAll();
    // delete sphereHead;
    // delete sphereBody;
}

Bounds3f SnowMan::ObjectBound() const {
    Point3f pmin, pmax;
    pmin.x = std::min(posH.x - radiusH, posB.x - radiusB);
    pmax.x = std::max(posH.x + radiusH, posB.x + radiusB);
    pmin.y = std::min(posH.y - radiusH, posB.y - radiusB);
    pmax.y = std::max(posH.y + radiusH, posB.y + radiusB);
    pmin.z = std::min(posH.z - radiusH, posB.z - radiusB);
    pmax.z = std::max(posH.z + radiusH, posB.z + radiusB);

    return Bounds3f(pmin, pmax);
}


bool SnowMan::Intersect(const Ray &r, Float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersect);

    // get intersections information from two spheres
    SurfaceInteraction isect1, isect2;
    Float tHit1, tHit2;
    bool hit1 = sphereHead.Intersect(r, &tHit1, &isect1, testAlphaTexture);
    bool hit2 = sphereBody.Intersect(r, &tHit2, &isect2, testAlphaTexture);
    bool hitSomething = false;

    isect1.MaterialID = 0;
    isect2.MaterialID = 1;

    // union

    if (hit1)
    {
        *isect = isect1;
        *tHit = tHit1;
        if (hit2){
            if (tHit2 < tHit1){
                *isect = isect2;
                *tHit = tHit2;
            }
        }
        // ++nAnyObjectIntersectionTests1;
        // yf::INCREMENT_STAT_COUNTER("SNOWMAN/MY_SNOWMAN_OBJECT_INTERSECTION1", 1);
        hitSomething = true;
    }
    else if (hit2)
    {
        *isect = isect2;
        *tHit = tHit2;
        // ++nAnyObjectIntersectionTests2;
        // yf::INCREMENT_STAT_COUNTER("SNOWMAN/MY_SNOWMAN_OBJECT_INTERSECTION2", 1);
        hitSomething = true;
    }

    if (hit1 || hit2)
    {
        // yf::INCREMENT_STAT_COUNTER("SNOWMAN/MYSNOWMAN_ANY_SHAPE_INTERSECTION", 1);
        // ++nCompositeShapeIntersectionTests;
        hitSomething = true;
    }

    return hitSomething;
}


bool SnowMan::IntersectP(const Ray &r, bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersectP);

    if ( sphereHead.IntersectP(r, testAlphaTexture) ||
         sphereBody.IntersectP(r, testAlphaTexture)
        )
        return true;

    return false;
}


Float SnowMan::Area() const {
    return sphereHead.Area() + sphereBody.Area();
}

Interaction SnowMan::Sample(const Point2f &u, Float *pdf) const 
{
    auto totalArea = Area();
    auto headArea = sphereHead.Area();
    auto bodyArea = sphereBody.Area();

    auto headPdf = headArea / totalArea;
    auto bodyPdf = bodyArea / totalArea; // 这行其实是可选的，因为它应该等于1 - headPdf

    Interaction sample;

    if (u[0] < headPdf) {
        // 对于头部采样，调整u值以确保均匀采样
        Point2f remappedU = Point2f(std::min(u[0] / headPdf, OneMinusEpsilon), u[1]);
        sample = sphereHead.Sample(remappedU, pdf);
        *pdf *= headPdf; // 调整pdf以反映整个雪人形状的采样密度
    } else {
        // 对于身体采样，同样调整u值
        Point2f remappedU = Point2f(std::min((u[0] - headPdf) / bodyPdf, OneMinusEpsilon), u[1]);
        sample = sphereBody.Sample(remappedU, pdf);
        *pdf *= bodyPdf; // 同上
    }

    return sample;
}


std::shared_ptr<Shape> CreateSnowManShape(const Transform *o2w,
                                         const Transform *w2o,
                                         bool reverseOrientation,
                                         const ParamSet &params) {
    Float radiusH = params.FindOneFloat("radiusH", 1.f);
    Float radiusB = params.FindOneFloat("radiusB", .5f);
    Point3f posH = params.FindOnePoint3f("posH", Point3f(0, 0, 0));
    Point3f posB = params.FindOnePoint3f("posB", Point3f(0, 0, 1.1));
    Float phimax = params.FindOneFloat("phimax", 360.f);
    return std::make_shared<SnowMan>(o2w, w2o, reverseOrientation,
                                    radiusH, radiusB,
                                    posH, posB, phimax);
}

}  // namespace pbrt
