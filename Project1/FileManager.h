#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Document.h"

using namespace std;

class FileManager
{
public:
    // Helper to remove extra spaces from start and end of a string
    string trim(string s)
    {
        int start = 0;
        while (start < (int)s.length() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        {
            start++;
        }

        int end = (int)s.length() - 1;
        while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r'))
        {
            end--;
        }

        if (start > end)
            return "";

        return s.substr(start, end - start + 1);
    }

    // Helper to extract integer id from id_str like "DOC-003" -> 3
    int extract_id(string id_str)
    {
        int id = 0;
        for (int i = 0; i < (int)id_str.length(); i++)
        {
            if (id_str[i] >= '0' && id_str[i] <= '9')
            {
                id = id * 10 + (id_str[i] - '0');
            }
        }
        return id;
    }

    // Helper to split tags separated by semicolon ';'
    vector<string> split_tags(string tags_str)
    {
        vector<string> tags;
        string current_tag = "";

        for (int i = 0; i < (int)tags_str.length(); i++)
        {
            if (tags_str[i] == ';')
            {
                string clean_tag = trim(current_tag);
                if (clean_tag != "")
                {
                    tags.push_back(clean_tag);
                }
                current_tag = "";
            }
            else
            {
                current_tag += tags_str[i];
            }
        }

        string clean_tag = trim(current_tag);
        if (clean_tag != "")
        {
            tags.push_back(clean_tag);
        }

        return tags;
    }

    // Loads all documents from documents.csv
    vector<Document> loadDocuments(string filename = "documents.csv")
    {
        vector<Document> docs;
        ifstream fin(filename);

        if (!fin.is_open())
        {
            cout << "Warning: Could not open " << filename << endl;
            return docs;
        }

        string line;
        while (getline(fin, line))
        {
            line = trim(line);

            // Skip empty lines or comment lines
            if (line == "" || line[0] == '#')
                continue;

            // Split line by '|' into 5 parts: ID | Title | Category | Tags | Text
            stringstream ss(line);
            string id_str, title, category, tags_str, text;

            if (getline(ss, id_str, '|') &&
                getline(ss, title, '|') &&
                getline(ss, category, '|') &&
                getline(ss, tags_str, '|') &&
                getline(ss, text))
            {
                id_str = trim(id_str);
                title = trim(title);
                category = trim(category);
                tags_str = trim(tags_str);
                text = trim(text);

                int id = extract_id(id_str);
                vector<string> tags = split_tags(tags_str);

                Document doc(id, id_str, title, category, tags, text);
                docs.push_back(doc);
            }
        }

        fin.close();
        cout << "Loaded " << docs.size() << " documents from " << filename << endl;
        return docs;
    }

    // Saves all documents to documents.csv
    bool saveDocuments(const vector<Document>& docs, string filename = "documents.csv")
    {
        ofstream fout(filename);

        if (!fout.is_open())
        {
            cout << "Error: Could not open " << filename << " for writing." << endl;
            return false;
        }

        for (int i = 0; i < (int)docs.size(); i++)
        {
            const Document& d = docs[i];

            // Join tags with ';'
            string tags_combined = "";
            for (int j = 0; j < (int)d.tags.size(); j++)
            {
                tags_combined += d.tags[j];
                if (j + 1 < (int)d.tags.size())
                {
                    tags_combined += ";";
                }
            }

            // Write: ID | Title | Category | Tags | Text
            fout << d.id_str << " | "
                 << d.title << " | "
                 << d.category << " | "
                 << tags_combined << " | "
                 << d.text << "\n";
        }

        fout.close();
        cout << "Successfully saved " << docs.size() << " documents to " << filename << endl;
        return true;
    }
};

#endif
