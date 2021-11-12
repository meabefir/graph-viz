#pragma once
#include <memory>
#include <vector>
#include "sfLine.h"
#include <set>

class Node;
class Edge;
class GraphAnimation;

using namespace std;
class Graph
{
    vector<vector<weak_ptr<Node>>> la;
    vector<vector<weak_ptr<Node>>> lat;
    vector<shared_ptr<Node>> nodes;
    vector<weak_ptr<Node>> nodesToDelete;
    vector<shared_ptr<Edge>> edges;
    vector<weak_ptr<Edge>> edgesToDelete;
    vector<GraphAnimation*> animations;
    int idx = 1;
    weak_ptr<Node> nodeCaptured;

    sf::Vertex newEdgeVertex[2];
    sfLine newEdgeLine;
    weak_ptr<Node> selectedForNewEdge;

    bool deleteHeld = false;

    // imgui vars
    char node_name[100];
    int edge_from = 1;
    int edge_to = 1;
    int bfs_start_node = 1;
    int dfs_start_node = 1;
    int prim_start_node = 1;
    bool isOriented = true;

public:
    Graph();
    void handleEvent(sf::Event e);
    weak_ptr<Node> getNodeUnderMouse();
    void update(float delta);
    void updateAnimations(float delta);
    void updateDeleteComponent();

    void pushNodes(float delta);
    void draw(sf::RenderWindow* win);
    void updateGUI();

    weak_ptr<Node> createNode(const char name[], sf::Vector2f pos = { 0.f, 0.f });
    void createEdge(int from, int to, bool both_ways = true);

    void deleteEdge(weak_ptr<Edge> e);

    weak_ptr<Node> getNodeById(int id);
    weak_ptr<Edge> getEdgeBetween(weak_ptr<Node> from, weak_ptr<Node> to);

    void deleteSingleNodes();
    void clearDeleted();

    void bfs(int s);

    void dfs(int s);
    // this dfs shows return edges kinda
    void dfs2(int s);
    void dfs22(int s, vector<int>& viz, GraphAnimation* ga);

    void dfs3(int s);
    void dfs33(int s, vector<int>& viz, GraphAnimation* ga);

    void kruskall();
    int getParent(int parents[], int x);

    bool alreadyUsedEdge(weak_ptr<Edge> edge, vector<weak_ptr<Edge>>& vec);
    void prim(int start);
};