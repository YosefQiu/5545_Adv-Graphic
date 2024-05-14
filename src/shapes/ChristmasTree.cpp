#include "shapes/sphere.h"
#include "shapes/ChristmasTree.h"
#include "sampling.h"
#include "paramset.h"
#include "efloat.h"
#include "stats.h"
// #include <iostream> // for print
#define TREE_CONE_COUNT 3
namespace pbrt {

// ChristmasTree Method Definitions
ChristmasTree::ChristmasTree(const Transform *ObjectToWorld, const Transform *WorldToObject,
              bool reverseOrientation,
              BASE_CONFIG_NODE &config,
              TREE_CONFIG_NODE &treeConfig
           ): Shape(ObjectToWorld, WorldToObject, reverseOrientation),
            mBaseConfig(config),
            mTreeConfig(treeConfig),
            mBaseNode(),
            mTreeNode(TREE_CONE_COUNT)

    {
        auto tmpPos = mBaseConfig.mCylinder_InitialPos;
        mBaseConfig.mCylinder_Object2World = (*ObjectToWorld) * Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z));
        mBaseConfig.mCylinder_World2Object = (*WorldToObject) * Inverse(Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z)));

        tmpPos = mBaseConfig.mDisk_InitialPos;
        mBaseConfig.mDisk_Object2World = (*ObjectToWorld) * Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z));
        mBaseConfig.mDisk_World2Object = (*WorldToObject) * Inverse(Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z)));

        mBaseNode.mCylinder = std::make_shared<Cylinder>(
                                        &mBaseConfig.mCylinder_Object2World,
                                        &mBaseConfig.mCylinder_World2Object,
                                        false,
                                        mBaseConfig.mRadius,
                                        mBaseConfig.mZmin,
                                        mBaseConfig.mZmax,
                                        mBaseConfig.mPhiMax
                                    );
        mBaseNode.mDisk = std::make_shared<Disk>(
                                        &mBaseConfig.mDisk_Object2World,
                                        &mBaseConfig.mDisk_World2Object,
                                        false,
                                        1, mBaseConfig.mRadius, 0,
                                        mBaseConfig.mPhiMax);



        tmpPos = mTreeConfig.mCylinder_InitialPos;
        mTreeConfig.mCylinder_Object2World = (*ObjectToWorld) * Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z));
        mTreeConfig.mCylinder_World2Object = (*WorldToObject) * Inverse(Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z)));
        mTreeNode.mCylinder = std::make_shared<Cylinder>(
                                        &mTreeConfig.mCylinder_Object2World,
                                        &mTreeConfig.mCylinder_World2Object,
                                        false,
                                        mTreeConfig.mRadius,
                                        mTreeConfig.mZmin,
                                        mTreeConfig.mZmax,
                                        mTreeConfig.mPhiMax);
        tmpPos = mTreeConfig.mCone_InitialPos;
        tmpPos.z = -0.3f;
        for (auto i = 0; i < mTreeNode.mCone.size(); i++)
        {
            
            mTreeConfig.mCone_Object2World[i] = (*ObjectToWorld) * Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z));
            mTreeConfig.mCone_World2Object[i] = (*WorldToObject) * Inverse(Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z)));

            mTreeNode.mCone[i] = std::make_shared<Cone>(
                                        &mTreeConfig.mCone_Object2World[i],
                                        &mTreeConfig.mCone_World2Object[i],
                                        false,
                                        1.5f,
                                        mTreeConfig.mRadius * 2.0f,
                                        mTreeConfig.mPhiMax);

            tmpPos.z += 0.4f;
        }

        //tmpPos = mTreeConfig.mSphere_InitialPos;
        tmpPos.z = 1.7f;
        auto r = 0.4f;
        mTreeConfig.mSphere_Object2World = (*ObjectToWorld) * Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z));
        mTreeConfig.mSphere_World2Object = (*WorldToObject) * Inverse(Translate(Vector3f(tmpPos.x, tmpPos.y, tmpPos.z)));
        mTreeNode.mSphere = std::make_shared<Sphere>(
                                        &mTreeConfig.mSphere_Object2World,
                                        &mTreeConfig.mSphere_World2Object,
                                        false,
                                        r,
                                        -r,
                                        r,
                                        mTreeConfig.mPhiMax
                                    );

    }

void updateBounds(Point3f& pmin, Point3f& pmax, const Point3f& newMin, const Point3f& newMax)
{
    pmin.x = std::min(pmin.x, newMin.x);
    pmin.y = std::min(pmin.y, newMin.y);
    pmin.z = std::min(pmin.z, newMin.z);

    pmax.x = std::max(pmax.x, newMax.x);
    pmax.y = std::max(pmax.y, newMax.y);
    pmax.z = std::max(pmax.z, newMax.z);
}

Bounds3f ChristmasTree::ObjectBound() const
{
    auto pmin = mBaseNode.mCylinder->ObjectBound().pMin;
    auto pmax = mBaseNode.mCylinder->ObjectBound().pMax;

    auto bounds = Union(mBaseNode.mCylinder->ObjectBound(), mBaseNode.mDisk->ObjectBound());
    bounds = Union(bounds, mTreeNode.mCylinder->ObjectBound());

    bounds = Union(bounds, mTreeNode.mSphere->ObjectBound());


    for (auto i = 0; i < mTreeNode.mCone.size(); i++)
    {
        auto tree_bounds = mTreeNode.mCone[i]->ObjectBound();
        bounds = Union(tree_bounds, bounds);
    }

    return bounds;
}


bool ChristmasTree::Intersect(const Ray &r, Float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersect);

    // get intersections information from two spheres
    SurfaceInteraction isect1, isect2, isect3, isect4;
    Float tHit1, tHit2, tHit3, tHit4;

    std::vector<SurfaceInteraction> isects_for_cone;
    std::vector<Float> tHits_for_cone;



    bool hit1 = mBaseNode.mCylinder->Intersect(r, &tHit1, &isect1, testAlphaTexture);
    bool hit2 = mBaseNode.mDisk->Intersect(r, &tHit2, &isect2, testAlphaTexture);
    bool hit3 = mTreeNode.mCylinder->Intersect(r, &tHit3, &isect3, testAlphaTexture);
    bool hit4 = mTreeNode.mSphere->Intersect(r, &tHit4, &isect4, testAlphaTexture);

    isect1.MaterialID = 0; // glasses
    isect2.MaterialID = 0;

    isect3.MaterialID = 1; // matte
    isect4.MaterialID = 2; // matel


    for (auto i = 0; i < mTreeNode.mCone.size(); i++)
    {
        SurfaceInteraction isect;
        Float tHit;
        if (mTreeNode.mCone[i]->Intersect(r, &tHit, &isect, testAlphaTexture))
        {
            isect.MaterialID = 3; // plastic
            isects_for_cone.push_back(isect);
            tHits_for_cone.push_back(tHit);
        }
    }

    bool hitSomething = false;
    Float closestHit = std::numeric_limits<Float>::max();



    if (hit1 && tHit1 < closestHit)
    {
        closestHit = tHit1;
        *isect = isect1;
        *tHit = tHit1;
        hitSomething = true;
    }
    if (hit2 && tHit2 < closestHit)
    {
        closestHit = tHit2;
        *isect = isect2;
        *tHit = tHit2;
        hitSomething = true;
    }
    if (hit3 && tHit3 < closestHit)
    {
        closestHit = tHit3;
        *isect = isect3;
        *tHit = tHit3;
        hitSomething = true;
    }
    if (hit4 && tHit4 < closestHit)
    {
        closestHit = tHit4;
        *isect = isect4;
        *tHit = tHit4;
        hitSomething = true;
    }

    for (auto i = 0; i < isects_for_cone.size(); i++)
    {
        if (tHits_for_cone[i] < closestHit)
        {
            closestHit = tHits_for_cone[i];
            *isect = isects_for_cone[i];
            *tHit = tHits_for_cone[i];
            hitSomething = true;
        }
    }

    return hitSomething;

}


bool ChristmasTree::IntersectP(const Ray &r, bool testAlphaTexture) const {
    ProfilePhase p(Prof::ShapeIntersectP);

    if (
        mBaseNode.mCylinder->IntersectP(r, testAlphaTexture) ||
        mBaseNode.mDisk->IntersectP(r, testAlphaTexture) ||
        mTreeNode.mCylinder->IntersectP(r, testAlphaTexture) ||
        mTreeNode.mSphere->IntersectP(r, testAlphaTexture)
        )
            return true;


    for (auto i = 0; i < mTreeNode.mCone.size(); i++)
    {
        if (mTreeNode.mCone[i]->IntersectP(r, testAlphaTexture))
            return true;
    }

    return false;
}


Float ChristmasTree::Area() const {
    float area = 0.0f;



    area += mBaseNode.mCylinder->Area();
    area += mBaseNode.mDisk->Area();
    area += mTreeNode.mCylinder->Area();
    area += mTreeNode.mSphere->Area();
    for (auto i = 0; i < mTreeNode.mCone.size(); i++)
    {
        area += mTreeNode.mCone[i]->Area();
    }
    return area;
}

Interaction ChristmasTree::Sample(const Point2f &u, Float *pdf) const {
    float totalArea = Area();
    std::vector<float> partsAreas = {
        mBaseNode.mCylinder->Area(),
        mBaseNode.mDisk->Area(),
        mTreeNode.mCylinder->Area(),
        mTreeNode.mSphere->Area()
    };
    for (auto& cone : mTreeNode.mCone) {
        partsAreas.push_back(cone->Area());
    }

    std::vector<float> cdf;
    cdf.push_back(0); 
    for (size_t i = 0; i < partsAreas.size(); ++i) {
        cdf.push_back(cdf.back() + partsAreas[i] / totalArea);
    }

    size_t partIndex = std::lower_bound(cdf.begin(), cdf.end(), u[0]) - cdf.begin() - 1;
    Interaction sample;

    float remappedU0 = (u[0] - cdf[partIndex]) / (cdf[partIndex + 1] - cdf[partIndex]);
    Point2f remappedU = Point2f(remappedU0, u[1]);

    if (partIndex == 0) {
        sample = mBaseNode.mCylinder->Sample(remappedU, pdf);
    } else if (partIndex == 1) {
        sample = mBaseNode.mDisk->Sample(remappedU, pdf);
    } else if (partIndex == 2) {
        sample = mTreeNode.mCylinder->Sample(remappedU, pdf);
    } else if (partIndex == 3) {
        sample = mTreeNode.mSphere->Sample(remappedU, pdf);
    } else {
        sample = mTreeNode.mCone[partIndex - 4]->Sample(remappedU, pdf);
    }

    *pdf *= partsAreas[partIndex] / totalArea;

    return sample;
}


std::shared_ptr<Shape> CreateChristmasTreeShape(const Transform *o2w,
                                         const Transform *w2o,
                                         bool reverseOrientation,
                                         const ParamSet &params) {

    Float cylinder_radius = params.FindOneFloat("cylinder_radius", 2.0f);
    Float cylinder_phi_max = params.FindOneFloat("cylinder_phi_max", 360.0f);
    Float cylinder_z_min = params.FindOneFloat("cylinder_z_min", -1.0);
    Float cylinder_z_max = params.FindOneFloat("cylinder_z_max", -0.8);
    Point3f cylinder_initial_pos = params.FindOnePoint3f("cylinder_pos", Point3f(0.0, 0.0, -0.5));
    Point3f disk_top_initial_pos = params.FindOnePoint3f("disk_pos", Point3f(0, 0, -1.8));


    ChristmasTree::BASE_CONFIG_NODE config;
    config.mRadius = cylinder_radius;
    config.mPhiMax = cylinder_phi_max;
    config.mZmin = cylinder_z_min;
    config.mZmax = cylinder_z_max;
    config.mHeight = std::abs(cylinder_z_min) - std::abs(cylinder_z_max);
    config.mCylinder_InitialPos = cylinder_initial_pos;
    config.mDisk_InitialPos = disk_top_initial_pos;

    auto h = config.mHeight;
    ChristmasTree::TREE_CONFIG_NODE treeConfig(TREE_CONE_COUNT);
    treeConfig.mRadius = cylinder_radius / 4.0f;
    treeConfig.mPhiMax = cylinder_phi_max;
    treeConfig.mZmin = cylinder_z_max;
    treeConfig.mHeight = h;
    treeConfig.mZmax = treeConfig.mZmin + ( 8.0f * treeConfig.mHeight);
    treeConfig.mCylinder_InitialPos = cylinder_initial_pos;
    treeConfig.mSphere_InitialPos = cylinder_initial_pos;
    treeConfig.mCone_InitialPos = cylinder_initial_pos;


    return std::make_shared<ChristmasTree>(o2w, w2o, reverseOrientation,
                                    config, treeConfig);
}

}  // namespace pbrt
