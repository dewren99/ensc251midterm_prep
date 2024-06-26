
// Name        : midtermPrep.cpp
// Description : midterm preparation program
// Copyright   : Copyright (c) 2023 School of Engineering Science
//============================================================================

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ensc251 {

static unsigned count = 0;

class TreeNode {
 private:
  typedef std::shared_ptr<TreeNode> TreeNodeSP;
  typedef std::vector<TreeNodeSP> TreeNodeSPVect;

 protected:
  TreeNodeSPVect childSPVector;
  typedef void (TreeNode::*action_func)(void);
  typedef void (TreeNode::*traverse_func)(void);

 public:
  virtual ~TreeNode() {
    // do not modify this insertion on OUT
    std::cout << "Destroying TreeNode with " << childSPVector.size()
              << " children." << std::endl;
  }
  TreeNode(){};

  virtual std::shared_ptr<TreeNode> clone() const {
    return std::make_shared<TreeNode>(*this);
  };

  TreeNode(const TreeNode& nodeToCopy) {
    for (auto childSP : nodeToCopy.childSPVector) {
      add_childP(childSP->clone());
    }
  }

  friend void swap(TreeNode& first,
                   TreeNode& second)  // the swap function (should never fail!)
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    swap(first.childSPVector, second.childSPVector);
  }

  TreeNode& operator=(
      TreeNode other)  // note: argument passed by value, copy made!
  {
    swap(*this, other);  // swap *this with other
    return *this;        // by convention, always return *this
  }  // swapped other (original *this) is destroyed, releasing the memory

  void add_childP(TreeNodeSP child) { childSPVector.push_back(child); }
  void add_children(const TreeNodeSPVect& childPV) {
    childSPVector.insert(childSPVector.end(), childPV.begin(), childPV.end());
  }
  const TreeNodeSPVect& get_children() const { return childSPVector; }

  void traverse_children_post_order(traverse_func tf, action_func af) {
    for (auto childP : childSPVector) {
      (childP.get()->*tf)();  // traverse child's children using tf
    }
    (this->*af)();
  }

  void count_action() { count++; }

  void count_traverse() {
    traverse_children_post_order(&ensc251::TreeNode::count_traverse,
                                 &ensc251::TreeNode::count_action);
  }
  void reset_count() { count = 0; }
  unsigned get_count() const { return count; }
};
}  // namespace ensc251

class DirectoryEntry : public ensc251::TreeNode {
 private:
  std::string m_name;

 public:
  DirectoryEntry(std::string_view name) : m_name{name} {}

  friend void swap(DirectoryEntry& a, DirectoryEntry& b) noexcept {
    using std::swap;
    swap(static_cast<ensc251::TreeNode&>(a),
         static_cast<ensc251::TreeNode&>(b));
    swap(a.m_name, b.m_name);
  }

  void set_name(const std::string_view& name) { m_name = name; }
  const std::string& get_name() { return m_name; }

  virtual void print_action() = 0;

  void print_traverse() {
    traverse_children_post_order(
        static_cast<traverse_func>(&DirectoryEntry::print_traverse),
        static_cast<action_func>(&DirectoryEntry::print_action));
  }
};

class File : public DirectoryEntry {
 public:
  File(std::string_view name) : DirectoryEntry(name) {}
  std::shared_ptr<TreeNode> clone() const override {
    return std::make_shared<File>(*this);
  };

  void print_action() override {
    std::cout << std::setw(20) << this->get_name() + "\tF" << std::endl;
  }
};

class Directory : public DirectoryEntry {
 public:
  Directory(std::string_view dir_name) : DirectoryEntry{dir_name} {}
  std::shared_ptr<TreeNode> clone() const override {
    return std::make_shared<Directory>(*this);
  };

  void print_action() override {
    std::cout << std::setw(20) << this->get_name() + "\t|"
              << " ";
    std::cout << this->childSPVector.size() << std::endl;
  }
};

int main() {
  auto root_dir1P = std::make_shared<Directory>("Dir1");
  {
    Directory root_dir2(*root_dir1P);
    root_dir2.set_name("Dir3");
    root_dir1P->add_childP(std::make_shared<File>("File1"));

    root_dir1P->print_traverse();
    root_dir2.print_traverse();  // not in table

    root_dir2 = *root_dir1P;
  }
  return 0;
}
