#pragma once
#include <vector>

#include "utility/BitMap.hh"
#include "TaggedData.hh"

namespace sta {

class FastSchedQueue {
  struct LevelBucket : public std::vector<TaggedData*> {};
  size_t counts;
  size_t buckets_count;
  LevelBucket* buckets;
  BitMap bucket_elem;
  bool forward_access;

  size_t process_lvl;
  LevelBucket* current_bucket;

  size_t bucketId(size_t level) const { return forward_access ? level : buckets_count - level; }
  LevelBucket& setBucket(size_t level) {
    bucket_elem[bucketId(level)] = true;
    return buckets[bucketId(level)];
  }

 public:
  FastSchedQueue(size_t maxlevel, bool forward)
      : counts(0), buckets_count(maxlevel), buckets(new LevelBucket[maxlevel]), bucket_elem(maxlevel), forward_access(forward), process_lvl(-1), current_bucket(nullptr) {}
  void sched(TaggedData* d) {
    if (d && !d->sched) {
      d->sched = true;
      setBucket(d->tagged.v->level()).push_back(d);
      ++counts;
    }
  }
  bool empty() const { return counts == 0; }
  TaggedData* get() {
    if (empty()) return nullptr;
    if (current_bucket) {
      LevelBucket& b = *current_bucket;
      TaggedData* r = b.back();
      size_t level = r->tagged.v->level();
      b.pop_back();
      if (b.empty()) {
        bucket_elem[bucketId(level)] = false;
        current_bucket = nullptr;
      }
      --counts;
      if (counts == 0) { process_lvl = -1; }
      return r;
    }
    process_lvl = bucket_elem.next(process_lvl);
    current_bucket = current_bucket + process_lvl;
    return get();
  }
};

}  // end namespace sta
