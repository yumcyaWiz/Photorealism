#ifndef BVH_H
#define BVH_H
#include "../aabb.h"
#include "../accel.h"

enum class BVH_PARTITION_TYPE {
  MEDIAN,
  EQSIZE,
  SAH
};


template <typename T>
class BVH : public Accel<T> {
  private:
    struct BVHPrimitiveInfo {
      AABB aabb;
      Vec3 center;
      int index;

      BVHPrimitiveInfo() {};
      BVHPrimitiveInfo(const AABB& _aabb, int _index) : aabb(_aabb), index(_index) {
        center = 0.5f*aabb.pMin + 0.5*aabb.pMax;
      };
    };


    struct BVHNode {
      AABB aabb;
      BVHNode* left;
      BVHNode* right;
      int axis;
      int offset;
      int nPrims;

      BVHNode() {};

      void initLeaf(const AABB& _aabb, int _offset, int _nPrims) {
        aabb = _aabb;
        offset = _offset;
        nPrims = _nPrims;
        left = right = nullptr;
      };
      void initNode(BVHNode* _left, BVHNode* _right, int _axis) {
        left = _left;
        right = _right;
        aabb = mergeAABB(left->aabb, right->aabb);
        axis = _axis;
        nPrims = 0;
      }
    };


  public:
    BVH_PARTITION_TYPE type;
    BVHNode* root;
    int maxPrimsInLeaf;

    BVH(const std::vector<std::shared_ptr<T>>& _prims, int _maxPrimsInLeaf) : Accel<T>(_prims), maxPrimsInLeaf(_maxPrimsInLeaf) {
      type = BVH_PARTITION_TYPE::MEDIAN;
      constructBVH();
    };


    void constructBVH() {
      std::vector<BVHPrimitiveInfo> primInfo(this->prims.size());
      for(size_t i = 0; i < primInfo.size(); i++) {
        AABB aabb = this->prims[i]->worldBound();
        primInfo[i] = BVHPrimitiveInfo(aabb, i);
      }

      std::vector<std::shared_ptr<T>> orderedPrims;
      root = buildBVH(0, this->prims.size(), primInfo, orderedPrims);
      this->prims.swap(orderedPrims);
    };


    BVHNode* buildBVH(int start, int end, std::vector<BVHPrimitiveInfo>& primInfo, std::vector<std::shared_ptr<T>>& orderedPrims) {
      BVHNode* node = new BVHNode();

      int nPrims = end - start;
      AABB aabb;
      for(int i = start; i < end; i++) {
        aabb = mergeAABB(aabb, primInfo[i].aabb);
      }

      if(nPrims <= maxPrimsInLeaf) {
        int offset = orderedPrims.size();
        for(int i = start; i < end; i++) {
          orderedPrims.push_back(this->prims[primInfo[i].index]);
        }
        node->initLeaf(aabb, offset, nPrims);
        return node;
      }

      AABB centerAABB;
      for(int i = start; i < end; i++) {
        centerAABB = mergeAABB(centerAABB, primInfo[i].center);
      }

      int axis = maximumExtent(centerAABB);
      if(centerAABB.pMin[axis] == centerAABB.pMax[axis]) {
        int offset = orderedPrims.size();
        for(int i = start; i < end; i++) {
          orderedPrims.push_back(this->prims[primInfo[i].index]);
        }
        node->initLeaf(aabb, offset, nPrims);
        return node;
      }

      int mid = (start + end)/2;
      if(this->type == BVH_PARTITION_TYPE::MEDIAN) {
        float midPoint = 0.5*centerAABB.pMin[axis] + 0.5*centerAABB.pMax[axis];
        BVHPrimitiveInfo* midPtr = std::partition(&primInfo[start], &primInfo[end-1]+1, [axis, midPoint](const BVHPrimitiveInfo& x) {
            return x.center[axis] < midPoint;
        });
        mid = midPtr - &primInfo[0];
      }

      BVHNode* left = buildBVH(start, mid, primInfo, orderedPrims);
      BVHNode* right = buildBVH(mid, end, primInfo, orderedPrims);
      node->initNode(left, right, axis);
      return node;
    };


    bool intersect(BVHNode* node, const Ray& ray, Hit& res) const {
      if(!node->aabb.intersect(ray)) return false;
      ray.hitCount++;

      if(node->nPrims != 0) {
        bool hit = false;
        for(int i = 0; i < node->nPrims; i++) {
          int index = node->offset + i;
          if(this->prims[index]->intersect(ray, res)) {
            if(isNan(res.hitPos)) {
              std::cout << "asdf" << std::endl;
            }
            hit = true;
            ray.tmax = res.t;
          }
        }
        return hit;
      }
      else {
        bool hit_left, hit_right;
        if(ray.direction[node->axis] > 0) {
          hit_left = intersect(node->left, ray, res);
          hit_right = intersect(node->right, ray, res);
        }
        else {
          hit_right = intersect(node->right, ray, res);
          hit_left = intersect(node->left, ray, res);
        }
        return hit_left || hit_right;
      }
    };


    bool intersect(const Ray& ray, Hit& res) const {
      return intersect(root, ray, res);
    };
};
#endif
