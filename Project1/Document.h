#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Document
{
public:
    int id;
    string id_str;
    string title;
    string category;
    vector<string> tags;
    string text;

    int date;
    int citation_count;

    Document()
    {
        id = 0;
        id_str = "";
        title = "";
        category = "";
        text = "";
        date = 0;
        citation_count = 0;
    }

    Document(int id, string id_str, string title, string category, const vector<string>& tags, string text, int date = 0, int citation_count = 0)
    {
        this->id = id;
        this->id_str = id_str;
        this->title = title;
        this->category = category;
        this->tags = tags;
        this->text = text;
        this->date = date;
        this->citation_count = citation_count;
    }

    bool hasTag(const string& tag)
    {
        for (int i = 0; i < tags.size(); i++)
        {
            if (tags[i] == tag)
                return true;
        }
        return false;
    }

    void Display() const
    {
        cout << "--------------------------------------------------" << endl;
        cout << "ID: " << id << " (" << id_str << ")" << endl;
        cout << "Title: " << title << endl;
        cout << "Category: " << category << endl;
        if (date > 0)
            cout << "Date: " << date << endl;
        if (citation_count > 0)
            cout << "Citation Count: " << citation_count << endl;

        cout << "Tags: ";
        for (int i = 0; i < tags.size(); i++)
        {
            cout << tags[i];
            if (i + 1 < tags.size())
                cout << ", ";
        }
        cout << endl;

        cout << "Text: ";
        if (text.length() > 100)
            cout << text.substr(0, 100) << "..." << endl;
        else
            cout << text << endl;
        cout << "--------------------------------------------------" << endl;
    }
};

#endif