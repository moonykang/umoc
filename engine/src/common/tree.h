#pragma once

#include <vector>

namespace util
{

class Tree
{
  public:
    Tree(Tree* parent) : parent(parent)
    {
    }

    bool isRoot()
    {
        return parent == nullptr;
    }

    void addChild(Tree* child)
    {
        children.push_back(child);
    }

  protected:
    Tree* parent;
    std::vector<Tree*> children;
};
} // namespace util