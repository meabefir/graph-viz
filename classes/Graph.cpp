#include "Graph.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "Helper.h"
#include "Edge.h"
#include "GraphAnimation.h"
#include "AnimationComponent.h"

#include <concurrent_priority_queue.h>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>

extern sf::RenderWindow* window;
extern sf::Vector2f mouseWorldPos;
extern sf::Vector2f lastMouseWorldPos;
extern bool drawEdges;
extern bool drawCosts;
extern bool randomEdgeCosts;
extern bool nodeRadiusChanged;
extern bool edgeThicknessChanged;

Graph::Graph() :
    newEdgeLine(newEdgeVertex, 2, 2.5f, sf::Color::Red)
{
    // nodes are 1 indexed
    la.push_back(vector<weak_ptr<Node>>());
    lat.push_back(vector<weak_ptr<Node>>());
}

void Graph::handleEvent(sf::Event e)
{
    if (e.type == sf::Event::MouseButtonPressed)
    {
        if (e.mouseButton.button == sf::Mouse::Left)
        {
            // if not grabbing imgui window
            if (!ImGui::GetIO().WantCaptureMouse)
            {
                // checl if grabbing existing node
                nodeCaptured = getNodeUnderMouse();
                if (!nodeCaptured.expired())
                {
                    // if mouse over node, capture the node
                    objp(nodeCaptured)->capture();
                }
                else
                {
                    if (!selectedForNewEdge.expired())
                    {
                        // create node and edge
                        weak_ptr<Node> new_node = createNode("", mouseWorldPos);
                        this->createEdge(objp(selectedForNewEdge)->idx, obj(new_node).idx);
                        selectedForNewEdge = weak_ptr<Node>();
                    }
                    else
                        createNode(node_name, mouseWorldPos);
                }
            }
        }
        else if (e.mouseButton.button == sf::Mouse::Right)
        {
            weak_ptr<Node> mouse_over_node = getNodeUnderMouse();
            if (mouse_over_node.expired())
            {
                if (!selectedForNewEdge.expired())
                {
                    weak_ptr<Node> new_node = createNode("", mouseWorldPos);
                    this->createEdge(objp(selectedForNewEdge)->idx, obj(new_node).idx);
                    // std not sure if done right cuz im bad at c++
                    selectedForNewEdge = weak_ptr<Node>();
                }
            }
            else
            {
                if (selectedForNewEdge.expired())
                {
                    selectedForNewEdge = mouse_over_node;
                }
                else
                {
                    createEdge(objp(selectedForNewEdge)->idx, objp(mouse_over_node)->idx);
                    selectedForNewEdge = weak_ptr<Node>();
                }
            }
        }
    }
    else if (e.type == sf::Event::MouseButtonReleased)
    {
        if (e.mouseButton.button == sf::Mouse::Left)
        {
            if (!nodeCaptured.expired())
            {
                objp(nodeCaptured)->release();
                nodeCaptured = weak_ptr<Node>();
            }
        }
    }
    else if (e.type == sf::Event::KeyPressed)
    {
        //if (e.key.code == Buttons::deleteButton)
        if (e.key.code == Buttons::deleteButton)
        {
            deleteHeld = true;
        }
    }
    else if (e.type == sf::Event::KeyReleased)
    {
        //if (e.key.code == Buttons::deleteButton)
        if (e.key.code == Buttons::deleteButton)
        {
            deleteHeld = false;
        }
    }
}

weak_ptr<Node> Graph::getNodeUnderMouse()
{
    for (auto& node : nodes)
    {
        if (Vector::getLength(mouseWorldPos - node->pos) <= Node::radius)
        {
            return node;
        }
    }
    return weak_ptr<Node>();
}

void Graph::update(float delta)
{
    // update edges
    for (auto& edge : edges)
        edge->update(delta);
    // update nodes
    for (auto& node : nodes)
        node->update(delta);
    // push nodes against each other
    pushNodes(delta);

    // update animations
    updateAnimations(delta);

    // delete stuff
    updateDeleteComponent();

    clearDeleted();
}

void Graph::updateDeleteComponent()
{
    if (!deleteHeld)
        return;

    weak_ptr<Node> under_mouse = getNodeUnderMouse();
    if (!under_mouse.expired())
    {
        nodesToDelete.push_back(under_mouse);
    }

    for (auto& e : edges)
    {
        if (Vector::segmentsIntersect(e->vecFrom, e->vecTo, lastMouseWorldPos, mouseWorldPos))
        {
            edgesToDelete.push_back(weak_ptr<Edge>(e));
        }
    }
}

void Graph::updateAnimations(float delta)
{
    vector<GraphAnimation*> animations_to_remove;
    for (auto& animation : animations)
    {
        if (animation->finished)
            animations_to_remove.push_back(animation);
        else
            animation->update(delta);
    }
    for (auto& animation : animations_to_remove)
    {
        animations.erase(find(animations.begin(), animations.end(), animation));
        delete animation;
    }
    animations_to_remove.clear();
}

void Graph::pushNodes(float delta)
{
    float diameter = Node::radius * 2.f;
    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = i + 1; j < nodes.size(); j++)
        {
            weak_ptr<Node> n1 = nodes[i];
            weak_ptr<Node> n2 = nodes[j];

            sf::Vector2f n1_to_n2 = obj(n2).pos - obj(n1).pos;
            float dist = Vector::getLength(n1_to_n2);
            sf::Vector2f n1_to_n2_norm = n1_to_n2 / dist;
            if (dist > diameter)
                continue;
            dist = diameter - dist;
            dist = max(dist, Node::radius / 2.f);

            n1_to_n2_norm *= delta * dist * 2.5f;
            n1_to_n2_norm *= 5.f;
            obj(n1).addImpulse(-n1_to_n2_norm);
            obj(n2).addImpulse(n1_to_n2_norm);
        }
    }
}

void Graph::draw(sf::RenderWindow* win)
{
    if (!selectedForNewEdge.expired())
    {
        newEdgeVertex[0].position = objp(selectedForNewEdge)->pos;
        newEdgeVertex[1].position = mouseWorldPos;
        newEdgeLine.setVertices(newEdgeVertex);
        newEdgeLine.draw(*win);
    }
    if (drawEdges)
        for (auto& edge : edges)
            edge->draw(win);
    for (auto& animation : animations)
        animation->draw(win);
    for (auto& node : nodes)
        node->draw(win);
}

void Graph::updateGUI()
{
    ImGui::Begin("graph");
    if (ImGui::CollapsingHeader("HELP"))
    {
        ImGui::TextWrapped("Left Click to create new node");
        ImGui::Separator();
        ImGui::TextWrapped("Right Click on two nodes in succession to connect them");
        ImGui::Separator();
        ImGui::TextWrapped("Right Click on a node and then right click anywhere on the canvas to create a new node and connect it with the first one");
        ImGui::Separator();
        ImGui::TextWrapped("Edit the distance between nodes in the EDIT GRAPH category");
        ImGui::Separator();
        ImGui::TextWrapped("Hold 'D' and hover across nodes or edges to remove them from the graph");
    }
    if (ImGui::CollapsingHeader("EDIT GRAPH"))
    {
        // random edge costs
        ImGui::Checkbox("Random edge costs on edge creation", &randomEdgeCosts);
        ImGui::Separator();
        // oriented or not
        ImGui::Checkbox("Oriented (create one way edges)", &isOriented);
        ImGui::Separator();
        // new node
        ImGui::Text("Add node");
        // ImGui::SameLine();
        if (ImGui::Button("Create Node"))
        {
            createNode(node_name, { window->getSize().x / 2.f + Random::randf() - .5f, window->getSize().y / 2.f + +Random::randf() - .5f });
        }
        ImGui::Separator();
        ImGui::Text("Add edge");
        ImGui::PushItemWidth(100);
        ImGui::InputInt("From", &edge_from);
        ImGui::SameLine();
        ImGui::InputInt("To", &edge_to);
        ImGui::PopItemWidth();
        if (ImGui::Button("Create Edge"))
        {
            createEdge(edge_from, edge_to);
        }
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(.9f, .0f, .0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(.8f, .0f, .0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(1.f, .0f, .0f));

        if (ImGui::Button("Delete single nodes"))
        {
            this->deleteSingleNodes();
        }
        ImGui::PopStyleColor(3);
        ImGui::Separator();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (edges.size())
            if (ImGui::TreeNode("Edit edge cost"))
            {
                ImGui::TextWrapped("grab and drag left/right or double click to edit");
                ImGui::Separator();
                ImGui::PushItemWidth(100);
                for (auto& e : edges)
                {
                    e->drawGUI();
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Drawing options"))
    {
        ImGui::Text("Random settings");
        ImGui::Checkbox("Draw node name", &drawNodeNames);
        ImGui::Checkbox("Draw node id", &drawNodeId);
        ImGui::Checkbox("Draw edges", &drawEdges);
        ImGui::Checkbox("Draw costs", &drawCosts);
        ImGui::Separator();
        ImGui::PushItemWidth(200);
        ImGui::SliderFloat("Min edge length", &Edge::min_length, 50.f, Edge::max_length);
        ImGui::SliderFloat("Max edge length", &Edge::max_length, Edge::min_length, 1000.f);
        if (ImGui::SliderFloat("Node radius", &Node::radius, 10.f, 100.f))
        {
            nodeRadiusChanged = true;
            Node::radiusSquared = Node::radius * Node::radius;
        }
        else
            nodeRadiusChanged = false;
        if (ImGui::SliderFloat("Edge thickness", &Edge::thickness, 1.f, 10.f))
        {
            edgeThicknessChanged = true;
        }
        else
            edgeThicknessChanged = false;
        ImGui::PopItemWidth();
    }
    ImGui::Separator();
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Graph Algorithms"))
    {
        ImGui::PushItemWidth(100);
        if (ImGui::TreeNode("BFS"))
        {
            ImGui::InputInt("bfs start node", &bfs_start_node);
            if (ImGui::Button("start bfs"))
            {
                bfs(bfs_start_node);
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("DFS"))
        {
            ImGui::InputInt("dfs start node", &dfs_start_node);
            if (ImGui::Button("start dfs"))
            {
                dfs3(dfs_start_node);
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::Button("KRUSKALL"))
        {
            kruskall();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("PRIM"))
        {
            ImGui::TextWrapped("Current implementation considers all edges to be undirected");
            ImGui::Separator();
            ImGui::InputInt("prim start node", &prim_start_node);
            if (ImGui::Button("start prim"))
            {
                prim(prim_start_node);
            }
            ImGui::TreePop();
        }
        ImGui::PopItemWidth();
    }
    ImGui::Separator();
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Animation player"))
    {
        for (auto& animation : animations)
        {
            animation->drawGUI();

            if (animation != animations[animations.size() - 1])
                ImGui::Separator();
        }
    }
    ImGui::End();
}

weak_ptr<Node> Graph::createNode(const char name[], sf::Vector2f pos)
{
    shared_ptr<Node> new_node = make_shared<Node>(name, pos, idx++);
    nodes.push_back(new_node);
    la.push_back(vector<weak_ptr<Node>>());
    lat.push_back(vector<weak_ptr<Node>>());
    weak_ptr<Node> wp = new_node;
    return wp;
}

void Graph::createEdge(int from, int to, bool both_ways)
{
    // if graph is not oriented
    if (isOriented == false and both_ways)
        createEdge(to, from, false);

    if (from == to)
        return;

    weak_ptr<Node> node_from;
    weak_ptr<Node> node_to;

    for (auto& node : nodes)
    {
        if (node->idx == from)
            node_from = node;
        else if (node->idx == to)
            node_to = node;
    }

    if (node_from.expired() || node_to.expired())
        return;
    // if edge already exists, return
    for (auto& edge : edges)
        if (objp(edge->from) == objp(node_from) && objp(edge->to) == objp(node_to))
            return;

    // cout << objp(node_from) << " TO " << objp(node_to) << '\n';
    int random_cost = 0;
    if (randomEdgeCosts)
    {
        random_cost = (int)(Random::randf() * 1000);
    }
    auto new_edge = make_shared<Edge>(node_from, node_to, random_cost);
    edges.push_back(new_edge);

    // push both ways in adjacecy list
    la[obj(node_from).idx].push_back(node_to);
    lat[obj(node_to).idx].push_back(node_from);
}

void Graph::deleteEdge(weak_ptr<Edge> e)
{
    edgesToDelete.push_back(e);
}

weak_ptr<Node> Graph::getNodeById(int id)
{
    for (auto& node : nodes)
        if (node->idx == id)
            return node;
    return weak_ptr<Node>();
}

weak_ptr<Edge> Graph::getEdgeBetween(weak_ptr<Node> from, weak_ptr<Node> to)
{
    for (auto& e : edges)
    {
        if (objp(e->from) == objp(from) and objp(e->to) == objp(to))
        {
            return e;
        }
    }
    return weak_ptr<Edge>();
}

void Graph::deleteSingleNodes()
{
    for (auto& np : nodes)
    {
        if (la[np->idx].size() == 0 and lat[np->idx].size() == 0)
        {
            nodesToDelete.push_back(np);
        }
    }
}

void Graph::clearDeleted()
{
    for (auto& wn : nodesToDelete)
    {
        // remove node from la
        la[obj(wn).idx].clear();
        lat[obj(wn).idx].clear();
        nodes.erase(find(nodes.begin(), nodes.end(), objp(wn)));
        for (auto& e : edges)
        {
            if (objp(e->from) == objp(wn) or objp(e->to) == objp(wn))
            {
                // if not there already, add it
                bool found = false;
                for (auto& we : edgesToDelete)
                    if (objp(we) == e)
                    {
                        found = true;
                        break;
                    }
                if (!found)
                    edgesToDelete.push_back(e);
            }
        }
    }
    nodesToDelete.clear();
    for (auto& we : edgesToDelete)
    {
        edges.erase(find(edges.begin(), edges.end(), objp(we)));
    }
    edgesToDelete.clear();
    // delete expired weak ptr from 
    for (int i = 0; i < la.size(); i++)
    {
        /*auto it = la[i].begin();
        while (it != la[i].end())
        {
            if ((*it).expired())
            {
                cout << " deleted\n";
                it = la[i].erase(it);
            }
            else ++it;
        }*/
        // sau
        la[i].erase(remove_if(la[i].begin(), la[i].end(), [](weak_ptr<Node>& wp) {return wp.expired(); }), la[i].end());
    }
    for (int i = 0; i < lat.size(); i++)
    {
        lat[i].erase(remove_if(lat[i].begin(), lat[i].end(), [](weak_ptr<Node>& wp) {return wp.expired(); }), lat[i].end());
    }
}

void Graph::bfs(int s)
{
    weak_ptr<Node> start_node = getNodeById(s);
    if (start_node.expired())
        return;

    GraphAnimation* new_animation = new GraphAnimation();
    char idxChar[10];
    _itoa_s(s, idxChar, 10);
    char name[100] = "BFS animation - ";
    strcat_s(name, idxChar);
    new_animation->setName(name);

    vector<int> viz(idx);
    for (auto& el : viz)
        el = -1;

    queue<int> q;
    q.push(s);
    viz[s] = 0;
    int current_dist = 1;

    while (q.size())
    {
        // cout << q.front() << ' ';
        weak_ptr<Node> from_node = getNodeById(q.front());
        for (auto node : la[q.front()])
        {
            if (viz[obj(node).idx] == -1)
            {
                viz[obj(node).idx] = viz[q.front()] + 1;
                if (viz[obj(node).idx] != current_dist)
                {
                    current_dist = viz[obj(node).idx];
                    new_animation->setLastStopsQueue();
                }

                weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
                if (edge.expired())
                    continue;

                AnimationComponent* new_edge_an = new EdgeAnimationReversed(edge, sf::Color::Red);
                // EdgeAnimation* new_edge_an = new EdgeAnimationMiddle(edge, sf::Color::Red);
                // i pause it only if its the last edge that is going to get visited

                new_edge_an->setStopAnimationsInQueue(false);
                new_animation->addAnimation(new_edge_an);

                q.push(obj(node).idx);
            }
        }
        q.pop();
    }
    // cout << new_animation->size() << '\n';
    // cout << '\n';
    new_animation->addAnimation(new PauseAnimation());
    new_animation->addAnimation(new PauseAnimation());
    animations.push_back(new_animation);
}

void Graph::dfs(int s)
{
    weak_ptr<Node> start_node = getNodeById(s);
    if (start_node.expired())
        return;

    GraphAnimation* new_animation = new GraphAnimation();
    char idxChar[10];
    _itoa_s(s, idxChar, 10);
    char name[100] = "DFS animation - ";
    strcat_s(name, idxChar);
    new_animation->setName(name);

    vector<int> viz(idx);
    stack<int> st;
    st.push(s);
    viz[s] = 1;

    while (st.size())
    {
        // cout << " top este " << st.top() << '\n';
        bool found = false;
        weak_ptr<Node> from_node = getNodeById(st.top());
        for (auto node : la[st.top()])
        {
            if (node.expired())
                continue;
            if (!viz[obj(node).idx])
            {
                weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
                if (edge.expired())
                    continue;

                EdgeAnimation* new_edge_an = new EdgeAnimation(edge, sf::Color::Red);
                new_animation->addAnimation(new_edge_an);

                // cout << " nodul " << node->idx << '\n';
                st.push(objp(node)->idx);
                viz[objp(node)->idx] = 1;
                found = true;
                break;
            }
        }
        if (!found)
            st.pop();
    }
    new_animation->addAnimation(new PauseAnimation());
    animations.push_back(new_animation);
}

void Graph::dfs2(int s)
{
    weak_ptr<Node> start_node = getNodeById(s);
    if (start_node.expired())
        return;

    GraphAnimation* new_animation = new GraphAnimation();
    char idxChar[10];
    _itoa_s(s, idxChar, 10);
    char name[100] = "DFS animation - ";
    strcat_s(name, idxChar);
    new_animation->setName(name);

    vector<int> viz(idx);

    dfs22(s, viz, new_animation);

    viz[s] = 1;

    new_animation->addAnimation(new PauseAnimation());
    animations.push_back(new_animation);
}

void Graph::dfs22(int s, vector<int>& viz, GraphAnimation* ga)
{
    weak_ptr<Node> from_node = getNodeById(s);
    viz[s] = 1;
    unordered_map<int, int> visited;
    for (auto node : la[s])
    {
        if (node.expired())
            continue;
        if (!viz[objp(node)->idx])
        {
            weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
            if (edge.expired())
                continue;

            EdgeAnimation* new_edge_an = new EdgeAnimation(edge, sf::Color::Red);
            ga->addAnimation(new_edge_an);

            visited[objp(node)->idx] = 1;

            dfs22(objp(node)->idx, viz, ga);
        }
    }
    for (auto node : la[s])
    {
        if (node.expired())
            continue;
        if (viz[objp(node)->idx] and visited.find(objp(node)->idx) == visited.end())
        {
            weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
            if (edge.expired())
                continue;

            EdgeAnimationDotted* new_edge_an = new EdgeAnimationDotted(edge, sf::Color::Blue);
            ga->addAnimation(new_edge_an);
        }
    }
}

void Graph::dfs3(int s)
{
    weak_ptr<Node> start_node = getNodeById(s);
    if (start_node.expired())
        return;

    GraphAnimation* new_animation = new GraphAnimation();
    char idxChar[10];
    _itoa_s(s, idxChar, 10);
    char name[100] = "DFS animation - ";
    strcat_s(name, idxChar);
    new_animation->setName(name);

    vector<int> viz(idx);

    dfs33(s, viz, new_animation);

    viz[s] = 1;

    new_animation->addAnimation(new PauseAnimation());
    animations.push_back(new_animation);
}

void Graph::dfs33(int s, vector<int>& viz, GraphAnimation* ga)
{
    weak_ptr<Node> from_node = getNodeById(s);
    viz[s] = 1;
    unordered_map<int, int> visited;
    for (auto node : la[s])
    {
        if (node.expired())
            continue;
        if (!viz[objp(node)->idx])
        {
            weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
            if (edge.expired())
                continue;

            EdgeAnimation* new_edge_an = new EdgeAnimation(edge, sf::Color::Red);
            ga->addAnimation(new_edge_an);

            visited[objp(node)->idx] = 1;

            dfs33(objp(node)->idx, viz, ga);
            AnimationComponent* remove_prev = new GraphAnimationAlterRemoveBefore(ga);
            ga->addAnimation(remove_prev);

            AnimationComponent* new_edge_an_reversed = new EdgeAnimationReversed(edge, sf::Color::Red);
            ga->addAnimation(new_edge_an_reversed);

            /*AnimationComponent* remove_prev2 = new GraphAnimationAlterRemoveBefore(ga);
            ga->addAnimation(remove_prev2);*/
        }
    }
    /*for (auto node : la[s])
    {
        if (node.expired())
            continue;
        if (viz[objp(node)->idx] and visited.find(objp(node)->idx) == visited.end())
        {
            weak_ptr<Edge> edge = getEdgeBetween(from_node, node);
            if (edge.expired())
                continue;

            EdgeAnimationDotted* new_edge_an = new EdgeAnimationDotted(edge, sf::Color::Blue);
            ga->addAnimation(new_edge_an);
        }
    }*/
}


void Graph::kruskall()
{
    if (edges.size() == 0)
        return;

    GraphAnimation* new_animation = new GraphAnimation();

    char name[100] = "KRUSKALL";
    new_animation->setName(name);

    int* parents = new int[(size_t)idx + 1];
    for (int i = 1; i <= idx; i++)
        parents[i] = 0;

    sort(edges.begin(), edges.end(), [](shared_ptr<Edge>& e1, shared_ptr<Edge>& e2) {
        return (e1->getCost() < e2->getCost());
        });

    for (auto& e : edges)
    {
        int from = e->from_int;
        int to = e->to_int;
        int from_parent = getParent(parents, from);
        int to_parent = getParent(parents, to);

        if (from_parent != to_parent)
        {
            parents[from_parent] = to_parent;
            AnimationComponent* new_comp = new EdgeAnimationMiddle(e, sf::Color::Magenta);
            new_animation->addAnimation(new_comp);
        }
        else
        {
            AnimationComponent* new_comp = new EdgeAnimationMiddle(e, sf::Color::Red);
            new_animation->addAnimation(new_comp);
            AnimationComponent* new_alter_comp = new GraphAlterRemoveEdge(this, e, 0.f);
            new_animation->addAnimation(new_alter_comp);
        }
    }
    animations.push_back(new_animation);
    delete[] parents;
}

int Graph::getParent(int* parents, int x)
{
    while (parents[x] != 0)
    {
        x = parents[x];
    }
    return x;
}

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

class edgeCompare
{
public:
    bool operator() (const weak_ptr<Edge>& e1, const weak_ptr<Edge>& e2)
    {
        return (e1.lock()->getCost() > e2.lock()->getCost());
    }
};

bool Graph::alreadyUsedEdge(weak_ptr<Edge> edge, vector<weak_ptr<Edge>>& vec)
{
    for (auto& e : vec)
        if (edge.lock() == e.lock())
            return true;
    return false;
}

void Graph::prim(int s)
{
    weak_ptr<Node> start_node = getNodeById(s);
    if (start_node.expired())
        return;

    GraphAnimation* new_animation = new GraphAnimation();

    unordered_map<int, int> used;
    used[s] = 1;

    priority_queue<weak_ptr<Edge>, vector<weak_ptr<Edge>>, edgeCompare> pq;
    // remove e->to_int == s if graph should if prim alg should care that the graph is oriented
    for (auto& e : edges)
        if (e->from_int == s or e->to_int == s)
        {
            pq.push(e);
            AnimationComponent* an_comp = new EdgeAnimationDotted(e, sf::Color::Blue, .2f);
            new_animation->addAnimation(an_comp);
        }

    // keep a vector of used edges to not remove them
    vector<weak_ptr<Edge>> used_edges;

    int edges_explored = 0;
    //while (used.size() != nodes.size())
    while (true)
    {
        // while least expensive node has both nodes in used
        while (pq.size() and used.find(pq.top().lock()->from_int) != used.end() and
            used.find(pq.top().lock()->to_int) != used.end())
        {
            bool found = alreadyUsedEdge(pq.top(), used_edges);
            if (!found)
            {
                // add bad animation from this edge
                AnimationComponent* new_bad_animation = new EdgeAnimationMiddle(pq.top(), sf::Color::Red);
                new_animation->addAnimation(new_bad_animation);
                // graph alter component
                AnimationComponent* graph_alter_comp = new GraphAlterRemoveEdge(this, pq.top());
                new_animation->addAnimation(graph_alter_comp);
            }
            pq.pop();
        }
        if (pq.size() == 0)
            break;
        // at this point, edge on top is the good one, so add it to apm
        AnimationComponent* new_animation_comp = new EdgeAnimationMiddle(pq.top(), sf::Color::Magenta);
        new_animation->addAnimation(new_animation_comp);
        // ad the one that isnt in used yet
        int node_idx_added;
        if (used.find(pq.top().lock()->from_int) == used.end())
        {
            node_idx_added = pq.top().lock()->from_int;
        }
        else
        {
            node_idx_added = pq.top().lock()->to_int;
        }
        used[node_idx_added] = 1;
        used_edges.push_back(pq.top());
        pq.pop();
        // add nodes from node_idx_added to the pq
        // remove e->to_int == s if graph should if prim alg should care that the graph is oriented
        for (auto& e : edges)
            if (e->from_int == node_idx_added or e->to_int == node_idx_added)
            {
                pq.push(e);
                // only animate edges that havent been highlighted yet
                if (!alreadyUsedEdge(e, used_edges))
                {
                    AnimationComponent* an_comp = new EdgeAnimationDotted(e, sf::Color::Blue, .2f);
                    new_animation->addAnimation(an_comp);
                }
            }
    }
    AnimationComponent* pause_an = new PauseAnimation();
    new_animation->addAnimation(pause_an);
    animations.push_back(new_animation);
}