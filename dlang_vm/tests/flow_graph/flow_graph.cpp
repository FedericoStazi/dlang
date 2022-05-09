// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include <gtest/gtest.h>

#include "../../src/data_structures/flow_graph.h"

TEST(FlowGraph, EmptyGraph) {
  // Create an empty graph
  FlowGraph<int> graph;

  // Test the absence of a start node
  ASSERT_EQ(graph.getStartNode(), nullptr);
}

TEST(FlowGraph, OneNode) {
  // Create a graph with one node
  FlowGraph<int> graph;
  graph.addNode(0, std::make_shared<int>(0));

  // Test the node
  auto node = graph.getStartNode();
  ASSERT_EQ(node->getId(), 0);
  ASSERT_EQ(*node->getValue(), 0);
  ASSERT_EQ(node->getPredecessors().size(), 0);
  ASSERT_EQ(node->getSuccessors().size(), 0);
}

TEST(FlowGraph, LineOfNodes) {
  // Create a graph with 3 nodes on a line
  FlowGraph<int> graph;
  for (int i=0; i<3; i++) {
    graph.addNodeLine(i, std::make_shared<int>(i));
  }

  // Test the first node
  auto node0 = graph.getLine().at(0);
  ASSERT_EQ(node0->getId(), 0);
  ASSERT_EQ(*node0->getValue(), 0);
  ASSERT_EQ(node0->getPredecessors().size(), 0);
  ASSERT_EQ(node0->getSuccessors().size(), 1);

  // Test the second node
  auto node1 = graph.getLine().at(1);
  ASSERT_EQ(node1->getId(), 1);
  ASSERT_EQ(*node1->getValue(), 1);
  ASSERT_EQ(node1->getPredecessors().size(), 1);
  ASSERT_EQ(node1->getSuccessors().size(), 1);

  // Test the third node
  auto node2 = graph.getLine().at(2);
  ASSERT_EQ(node2->getId(), 2);
  ASSERT_EQ(*node2->getValue(), 2);
  ASSERT_EQ(node2->getPredecessors().size(), 1);
  ASSERT_EQ(node2->getSuccessors().size(), 0);
}

TEST(FlowGraph, CycleOfNodes) {
  // Create a graph with 3 nodes in a cycle
  FlowGraph<int> graph;
  for (int i=0; i<3; i++) {
    graph.addNodeLine(i, std::make_shared<int>(i));
  }
  graph.addEdge(0);

  // Test the first node
  auto node0 = graph.getLine().at(0);
  ASSERT_EQ(node0->getId(), 0);
  ASSERT_EQ(*node0->getValue(), 0);
  ASSERT_EQ(node0->getPredecessors().size(), 1);
  ASSERT_EQ(node0->getSuccessors().size(), 1);

  // Test the second node
  auto node1 = graph.getLine().at(1);
  ASSERT_EQ(node1->getId(), 1);
  ASSERT_EQ(*node1->getValue(), 1);
  ASSERT_EQ(node1->getPredecessors().size(), 1);
  ASSERT_EQ(node1->getSuccessors().size(), 1);

  // Test the third node
  auto node2 = graph.getLine().at(2);
  ASSERT_EQ(node2->getId(), 2);
  ASSERT_EQ(*node2->getValue(), 2);
  ASSERT_EQ(node2->getPredecessors().size(), 1);
  ASSERT_EQ(node2->getSuccessors().size(), 1);
}

TEST(FlowGraph, NodeBeforeEdge) {
  // Create a graph with two nodes
  FlowGraph<int> graph;
  graph.addNode(0, std::make_shared<int>(0));
  graph.addNode(1, std::make_shared<int>(1));
  graph.addEdge(0);

  // Test the first node
  auto node0 = graph.getStartNode();
  ASSERT_EQ(node0->getId(), 0);
  ASSERT_EQ(*node0->getValue(), 0);
  ASSERT_EQ(node0->getPredecessors().size(), 1);
  ASSERT_EQ(node0->getSuccessors().size(), 0);

  // Test the second node
  auto node1 = node0->getPredecessors().at(0);
  ASSERT_EQ(node1->getId(), 1);
  ASSERT_EQ(*node1->getValue(), 1);
  ASSERT_EQ(node1->getPredecessors().size(), 0);
  ASSERT_EQ(node1->getSuccessors().size(), 1);
}

TEST(FlowGraph, EdgeBeforeNode) {
  // Create a graph with two nodes
  FlowGraph<int> graph;
  graph.addNode(0, std::make_shared<int>(0));
  graph.addEdge(1);
  graph.addNode(1, std::make_shared<int>(1));

  // Test the first node
  auto node0 = graph.getStartNode();
  ASSERT_EQ(node0->getId(), 0);
  ASSERT_EQ(*node0->getValue(), 0);
  ASSERT_EQ(node0->getPredecessors().size(), 0);
  ASSERT_EQ(node0->getSuccessors().size(), 1);

  // Test the second node
  auto node1 = node0->getSuccessors().at(0);
  ASSERT_EQ(node1->getId(), 1);
  ASSERT_EQ(*node1->getValue(), 1);
  ASSERT_EQ(node1->getPredecessors().size(), 1);
  ASSERT_EQ(node1->getSuccessors().size(), 0);
}
