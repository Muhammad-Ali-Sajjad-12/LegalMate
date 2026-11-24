#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "Document.h"
#include "LinkedList.h"

using namespace std;

// Represents a directed/undirected edge to a neighbor document
struct GraphEdge
{
    int targetId; // ID of the connected document
    int weight;   // Number of shared tags

    // Compare by targetId so LinkedList search and remove works
    bool operator==(const GraphEdge& other) const
    {
        return targetId == other.targetId;
    }
};

// Represents a Vertex in the Graph for a single document
struct GraphVertex
{
    int docId;
    vector<string> tags;
    LinkedList<GraphEdge> edges; // Adjacency list using Sajjad's custom LinkedList

    bool operator==(const GraphVertex& other) const
    {
        return docId == other.docId;
    }
};

// Represents a recommendation result (Level 1 direct, Level 2 indirect)
struct Recommendation
{
    int docId;
    int weight;   // Shared tags count
    int level;    // 1 for direct neighbor, 2 for 2-hop BFS neighbor
    int viaDocId; // For level 2: which document connected them

    bool operator==(const Recommendation& other) const
    {
        return docId == other.docId;
    }
};

class Graph
{
private:
    vector<GraphVertex> vertices;

    // Helper to count how many tags two documents share
    int countSharedTags(const vector<string>& tags1, const vector<string>& tags2)
    {
        int count = 0;
        for (int i = 0; i < (int)tags1.size(); i++)
        {
            for (int j = 0; j < (int)tags2.size(); j++)
            {
                if (tags1[i] == tags2[j])
                {
                    count++;
                    break;
                }
            }
        }
        return count;
    }

    // Helper to find a vertex by its docId
    GraphVertex* findVertex(int docId)
    {
        for (int i = 0; i < (int)vertices.size(); i++)
        {
            if (vertices[i].docId == docId)
            {
                return &vertices[i];
            }
        }
        return nullptr;
    }

public:
    Graph() 
    {

    }

    // Adds a document to the graph and connects it with any documents sharing tags
    void addDocument(const Document& doc)
    {
        // Avoid adding duplicate vertices
        if (findVertex(doc.id) != nullptr)
            return;

        GraphVertex newV;
        newV.docId = doc.id;
        newV.tags = doc.tags;

        // Compare tags with every already existing document
        for (int i = 0; i < (int)vertices.size(); i++)
        {
            int shared = countSharedTags(doc.tags, vertices[i].tags);
            if (shared > 0)
            {
                // Edge from new document to existing document
                GraphEdge e1 = { vertices[i].docId, shared };
                newV.edges.pushBack(e1);

                // Edge from existing document to new document
                GraphEdge e2 = { doc.id, shared };
                vertices[i].edges.pushBack(e2);
            }
        }

        vertices.push_back(newV);
    }

    // Removes a document and all edges pointing to it
    bool removeDocument(int docId)
    {
        int index = -1;
        for (int i = 0; i < (int)vertices.size(); i++)
        {
            if (vertices[i].docId == docId)
            {
                index = i;
                break;
            }
        }

        if (index == -1)
            return false;

        // Remove edge from all other vertices pointing to this document
        GraphEdge toRemove = { docId, 0 };
        for (int i = 0; i < (int)vertices.size(); i++)
        {
            vertices[i].edges.remove(toRemove);
        }

        // Remove the vertex itself
        vertices.erase(vertices.begin() + index);
        return true;
    }

    // BFS recommendations: Level 1 direct neighbors ranked by weight,
    // then Level 2 indirect neighbors (depth = 2)
    LinkedList<Recommendation> recommend(int docId, int maxDepth = 2)
    {
        LinkedList<Recommendation> results;
        GraphVertex* start = findVertex(docId);
        if (start == nullptr)
            return results;

        vector<int> visited;
        visited.push_back(docId);

        // Step 1: Collect Level 1 (Direct Neighbors)
        vector<Recommendation> l1;
        ListNode<GraphEdge>* curr = start->edges.getHead();
        while (curr != nullptr)
        {
            Recommendation r = { curr->data.targetId, curr->data.weight, 1, 0 };
            l1.push_back(r);
            visited.push_back(curr->data.targetId);
            curr = curr->next;
        }

        // Sort Level 1 by weight descending (highest shared tags first)
        for (int i = 0; i < (int)l1.size(); i++)
        {
            for (int j = i + 1; j < (int)l1.size(); j++)
            {
                if (l1[j].weight > l1[i].weight)
                {
                    swap(l1[i], l1[j]);
                }
            }
        }

        // Step 2: Collect Level 2 (BFS depth 2 via Level 1 neighbors)
        vector<Recommendation> l2;
        if (maxDepth >= 2)
        {
            for (int i = 0; i < (int)l1.size(); i++)
            {
                GraphVertex* neighbor = findVertex(l1[i].docId);
                if (neighbor == nullptr)
                    continue;

                ListNode<GraphEdge>* edge = neighbor->edges.getHead();
                while (edge != nullptr)
                {
                    bool alreadyVisited = false;
                    for (int v = 0; v < (int)visited.size(); v++)
                    {
                        if (visited[v] == edge->data.targetId)
                        {
                            alreadyVisited = true;
                            break;
                        }
                    }

                    if (!alreadyVisited)
                    {
                        Recommendation r2 = { edge->data.targetId, edge->data.weight, 2, l1[i].docId };
                        l2.push_back(r2);
                        visited.push_back(edge->data.targetId);
                    }
                    edge = edge->next;
                }
            }

            // Sort Level 2 by weight descending
            for (int i = 0; i < (int)l2.size(); i++)
            {
                for (int j = i + 1; j < (int)l2.size(); j++)
                {
                    if (l2[j].weight > l2[i].weight)
                    {
                        swap(l2[i], l2[j]);
                    }
                }
            }
        }

        // Step 3: Combine L1 and L2 into final LinkedList
        for (int i = 0; i < (int)l1.size(); i++)
            results.pushBack(l1[i]);

        for (int i = 0; i < (int)l2.size(); i++)
            results.pushBack(l2[i]);

        return results;
    }

    // Total unique edges count
    int edgeCount() const
    {
        int total = 0;
        for (int i = 0; i < (int)vertices.size(); i++)
        {
            total += vertices[i].edges.size();
        }
        return total / 2; // Each undirected edge counted once
    }

    // Total documents count in graph
    int vertexCount() const
    {
        return (int)vertices.size();
    }
};

#endif
