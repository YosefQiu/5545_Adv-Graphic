#include <memory>
#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_SHAPES_ChristmasTree_H
#define PBRT_SHAPES_ChristmasTree_H

#include "shape.h"
#include "sphere.h"
#include "cylinder.h"
#include "disk.h"
#include "cone.h"
#include "../core/transform.h"

namespace pbrt {


// Christmas tree Declarations
class ChristmasTree : public Shape {
  public:
    struct BASE_CONFIG_NODE
    {
        Float mRadius;
        Float mZmin;
        Float mZmax;
        Float mPhiMax;
        Float mHeight;
        Point3f mCylinder_InitialPos;
        Point3f mDisk_InitialPos;
        Transform mCylinder_Object2World;
        Transform mCylinder_World2Object;
        Transform mDisk_Object2World;
        Transform mDisk_World2Object;
    };
    struct BASE_NODE
    {
        std::shared_ptr<Cylinder> mCylinder;
        std::shared_ptr<Disk> mDisk;
        BASE_NODE() : mCylinder(nullptr), mDisk(nullptr) {}
    };

    struct TREE_CONFIG_NODE // sphere, cone, cylinder
    {
        Float mRadius;
        Float mZmin;
        Float mZmax;
        Float mPhiMax;
        Float mHeight;
        Point3f mCylinder_InitialPos;
        Point3f mCone_InitialPos;
        Point3f mSphere_InitialPos;
        Transform mCylinder_Object2World;
        Transform mCylinder_World2Object;

        Transform mSphere_Object2World;
        Transform mSphere_World2Object;

        std::vector<Transform> mCone_Object2World;
        std::vector<Transform> mCone_World2Object;

      TREE_CONFIG_NODE()  = default;
      TREE_CONFIG_NODE(size_t size = 1) 
      {
        if(!mCone_Object2World.empty()) mCone_Object2World.clear();
        if(!mCone_World2Object.empty()) mCone_World2Object.clear();
        mCone_Object2World.resize(size);
        mCone_World2Object.resize(size);
      }

        
    };

    struct TREE_NODE 
    {
        std::shared_ptr<Cylinder> mCylinder;
        std::vector<std::shared_ptr<Cone> > mCone;
        std::shared_ptr<Sphere> mSphere;
        TREE_NODE() : mCylinder(nullptr), mSphere(nullptr) {};
        TREE_NODE(size_t size = 1) : mCylinder(nullptr) , mSphere(nullptr)
        {
          if(!mCone.empty()) mCone.clear();
          mCone.resize(size);
        }
    };

  public:
    // ChristmasTree Public Methods
    ChristmasTree(const Transform *ObjectToWorld, const Transform *WorldToObject, bool reverseOrientation,
            BASE_CONFIG_NODE &config, TREE_CONFIG_NODE &treeConfig);


    Bounds3f ObjectBound() const;
    bool Intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect,
                   bool testAlphaTexture) const;
    bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
    Interaction Sample(const Point2f &u, Float *pdf) const;
    Float Area() const;

  private:
    // ChristmasTree Private Data
    BASE_CONFIG_NODE    mBaseConfig;
    BASE_NODE           mBaseNode;
    TREE_CONFIG_NODE    mTreeConfig;
    TREE_NODE           mTreeNode;
};


std::shared_ptr<Shape> CreateChristmasTreeShape(const Transform *o2w,
                                         const Transform *w2o,
                                         bool reverseOrientation,
                                         const ParamSet &params);

}  // namespace pbrt

#endif  // PBRT_SHAPES_ChristmasTree_H
