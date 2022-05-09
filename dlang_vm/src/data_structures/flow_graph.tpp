// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "flow_graph.h"

template<typename T>
Node<T>::Node(size_t id, std::shared_ptr<T> val) : id_(id), val_(val) {}

template<typename T>
size_t Node<T>::getId() {
  return id_;
}

template<typename T>
std::shared_ptr<T> Node<T>::getValue() {
  return val_;
}

template<typename T>
void Node<T>::setValue(std::shared_ptr<T> val) {
  val_ = val;
}

template<typename T>
const std::vector<std::shared_ptr<Node<T>>>& Node<T>::getSuccessors() {
  return succ_;
}

template<typename T>
const std::vector<std::shared_ptr<Node<T>>>& Node<T>::getPredecessors() {
  return pred_;
}

template<typename T>
void FlowGraph<T>::addNodeLine(int id, ValPtr val) {
  // Update the start Id
  if (startId_ == std::nullopt) {
    startId_ = id;
  }

  // Add the node to the graph
  auto node = std::make_shared<Node<T>>(id, val);
  if (lastNode_) {
    addEdge(lastNode_, node);
    lastNode_->nextLine_ = node;
  }
  lastNode_ = node;

  // Connect the edges
  connectEdges(node);
}

template<typename T>
void FlowGraph<T>::addNode(int id, ValPtr val) {
  // Update the start Id
  if (startId_ == std::nullopt) {
    startId_ = id;
  }

  // Add the node to the graph
  auto node = std::make_shared<Node<T>>(id, val);
  lastNode_ = node;

  // Connect the edges
  connectEdges(node);
}

template<typename T>
void FlowGraph<T>::addEdge(int id) {
  if (idFirstNode_.count(id)) {
    // Add the edge if the destination already exists
    addEdge(lastNode_, idFirstNode_.at(id));
  } else {
    // Add to the list of missing edges otherwise
    missingEdges_[id].push_back(lastNode_);
  }
}

template<typename T>
void FlowGraph<T>::addOutEdge() {
  addEdge(lastNode_, nullptr);
}

template<typename T>
typename FlowGraph<T>::NodePtr FlowGraph<T>::getStartNode() const {
  if (startId_ == std::nullopt) {
    return nullptr;
  }
  return idFirstNode_.at(startId_.value());
}

template<typename T>
void FlowGraph<T>::connectEdges(NodePtr node) {
  // Checks if it is the first node for this id
  if (!idFirstNode_.count(node->getId())) {
    idFirstNode_.insert({node->getId(), node});
    // Connect all missing edges to this node
    for (const auto& nodeFrom : missingEdges_[node->getId()]) {
      addEdge(nodeFrom, node);
    }
  }
}

template<typename T>
void FlowGraph<T>::addEdge(NodePtr nodeFrom, NodePtr nodeTo) {
  nodeFrom->succ_.push_back(nodeTo);
  if (nodeTo) {
    nodeTo->pred_.push_back(nodeFrom);
  }
}

template<typename T>
typename FlowGraph<T>::Line FlowGraph<T>::getLine() {
  Line line;
  for (auto node = getStartNode(); node; node = node->nextLine_) {
    line.push_back(node);
  }
  return line;
}

template<typename T>
typename FlowGraph<T>::Blocks FlowGraph<T>::getBlocks() {
  Blocks blocks;
  bool active = false;
  for (auto node : getLine()) {
    if (node->succ_.size() != 1) {
      blocks.emplace_back();
      active = true;
    }
    if (active) {
      blocks.back().push_back(node);
    }
    if (active && (node->succ_.size() != 1 || (!node->succ_.at(0)))) {
      active = false;
    }
  }
  return blocks;
}
