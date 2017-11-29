#ifndef QOAED_POINT_QUADTREE_H
#define QOAED_POINT_QUADTREE_H

#include <array>
#include <list>
#include <set>

#include "exceptions.h"

namespace qoaed {

template <class Value, class Key = unsigned long>
class PointQuadtree {
private:

  struct Node;
  using Childs = std::array<Node*, 4>;

  struct Node {
    Key    x, y;
    Value  val;
    Childs childs;

    Node(const Key& x, const Key& y, const Value& val):
      x(x), y(y), val(val) {
        childs = {0, 0, 0, 0};
      }
  };

public:

  class NodeVisitor {
  private:
    Node* n;

  public:
    NodeVisitor(Node* n): n(n) {}

    const Key& get_x() const { return n->x; }
    const Key& get_y() const { return n->y; }
    Value& operator*() const { return n->val; }
  };

  using Nodes = std::list<NodeVisitor>;

  struct Rect {
    Key min_x, min_y;
    Key max_x, max_y;

    Rect(const Key& min_x, const Key& min_y, const Key& max_x, const Key& max_y):
      min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y) {}

    bool contains(const Key& x, const Key& y) const {
      bool cx, cy;

      cx = (x <= max_x && x >= min_x);
      cy = (y <= max_y && x >= min_y);

      return cx && cy;
    }
  };

private:
  Node* m_root;
  Key   max_x, max_y;

  static const int NW = 0;
  static const int NE = 1;
  static const int SE = 2;
  static const int SW = 3;

public:

  PointQuadtree()                                   : m_root(0), max_x(0), max_y(0) {}
  PointQuadtree(const Key& max_x, const Key& max_y) : m_root(0), max_x(max_x), max_y(max_y) {}

  void insert(const Key& x, const Key& y, const Value& val) {
    if (!in_range(x, y)) return;
    m_root = insert(m_root, x, y, val);
  }

  Nodes ranged_query(const Rect& rect) {
    Nodes node_set;
    ranged_query(m_root, rect, node_set);
    return node_set;
  }

  /*
  Nodes spherical_query(const Key& x, const Key& y, const Key& radius) {
    if (!in_range(x,y)) return;

  }
  */

private:

  bool in_range(const Key& x, const Key& y) {
    if (!max_x || !max_y) throw DimensionsMissing();

    if (x >= max_x) return false;
    if (y >= max_y) return false;

    return true;
  }

  Node* insert(Node* n, const Key& x, const Key& y, const Value& val) {
    if (!n) return new Node(x, y, val);

    else if (x < n->x && y < n->y)
      n->childs[SW] = insert(n->childs[SW], x, y, val);

    else if (x < n->x && y > n->y)
      n->childs[NW] = insert(n->childs[NW], x, y, val);

    else if (x > n->x && y < n->y)
      n->childs[SE] = insert(n->childs[SE], x, y, val);

    else if (x > n->x && y > n->y)
      n->childs[NE] = insert(n->childs[NE], x, y, val);

    return n;
  }

  void ranged_query(Node* n, const Rect& rect, Nodes& nodes) {
    if (!n) return;

    if (rect.contains(n->x, n->y)) nodes.emplace_back(n);

    if (rect.min_x < n->x && rect.min_y < n->y)
      ranged_query(n->childs[SW], rect, nodes);

    if (rect.min_x < n->x && rect.min_y > n->y)
      ranged_query(n->childs[NW], rect, nodes);

    if (rect.max_x > n->x && rect.max_y < n->y)
      ranged_query(n->childs[NE], rect, nodes);

    if (rect.max_x > n->x && rect.max_y > n->y)
      ranged_query(n->childs[SE], rect, nodes);
  }

};

}

#endif