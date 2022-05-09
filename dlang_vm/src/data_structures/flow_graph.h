// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <unordered_map>
#include <memory>
#include <optional>
#include <vector>

template<typename T> class FlowGraph;

template<typename T>
class Node {
 public:
  Node(size_t id, std::shared_ptr<T> val);

  size_t getId();
  std::shared_ptr<T> getValue();
  void setValue(std::shared_ptr<T> val);

  const std::vector<std::shared_ptr<Node>>& getSuccessors();
  const std::vector<std::shared_ptr<Node>>& getPredecessors();

 private:
  size_t id_;
  std::shared_ptr<T> val_;

  friend FlowGraph<T>;
  std::vector<std::shared_ptr<Node>> succ_;
  std::vector<std::shared_ptr<Node>> pred_;
  std::shared_ptr<Node> nextLine_ = nullptr;
};

template<typename T>
class FlowGraph {
 public:
  using ValPtr = std::shared_ptr<T>;
  using NodePtr = std::shared_ptr<Node<T>>;
  using Line = std::vector<NodePtr>;
  using Blocks = std::vector<std::vector<NodePtr>>;

  void addNodeLine(int id, ValPtr val);
  void addNode(int id, ValPtr val);
  void addEdge(int id);
  void addOutEdge();

  NodePtr getStartNode() const;
  Line getLine();
  Blocks getBlocks();

 private:
  void connectEdges(NodePtr node);
  void addEdge(NodePtr nodeFrom, NodePtr nodeTo);

  std::optional<int> startId_;
  NodePtr lastNode_;
  std::unordered_map<size_t, NodePtr> idFirstNode_;
  std::unordered_map<size_t, std::vector<NodePtr>> missingEdges_;
};

#include "flow_graph.tpp"
