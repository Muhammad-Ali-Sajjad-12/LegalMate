#pragma once
#include <string>
#include "LinkedList.h"

// textutils.h - by sajjad
// text helpers: lowercase, stop word check and tokenizer
// tokenizer turns a sentence into keywords for the hash table
// functions are inline so this header can be included in many cpp files
// without getting multiple definition errors

// Converts every capital letter to lowercase: "Tenant RENT" -> "tenant rent"
inline std::string toLowerCase(const std::string& text) {
    std::string result = text;   // work on a copy, the original stays unchanged

    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] - 'A' + 'a';
        }
    }
    return result;
}

// Returns true for filler words that carry no meaning in a search.
// The word must already be lowercase.
inline bool isStopWord(const std::string& word) {
    // static: the list is built once, not every time the function runs
    static const std::string stopWords[] = {
        "a", "an", "the", "and", "or", "but", "of", "to", "in", "on", "at",
        "by", "for", "from", "with", "as", "about", "is", "are", "was", "were",
        "be", "been", "it", "its", "this", "that", "these", "those",
        "i", "me", "my", "we", "our", "you", "your", "he", "she", "they",
        "his", "her", "their", "them", "what", "which", "who", "when", "where",
        "why", "how", "if", "then", "so", "do", "does", "did", "can", "could",
        "should", "would", "will", "may", "might", "must", "has", "have",
        "had", "not", "no", "any", "all", "some", "there", "here", "very",
        "just", "also", "dont", "doesnt", "didnt", "hasnt", "havent", "isnt",
        "cant", "wont", "im"
    };
    const int stopWordCount = sizeof(stopWords) / sizeof(stopWords[0]);

    for (int i = 0; i < stopWordCount; i++) {
        if (stopWords[i] == word) {
            return true;
        }
    }
    return false;
}

// True for characters that can be part of a word: letters and digits
inline bool isWordChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

// Adds a finished word to the list if it is useful, then empties it
// for the next word. Skips one-letter words, stop words and repeats.
inline void addKeyword(std::string& word, LinkedList<std::string>& keywords) {
    if (word.length() >= 2 && !isStopWord(word) && !keywords.contains(word)) { // using 'contains' to avoid duplicates
        keywords.pushBack(word);
    }
    word = "";
}

// Splits text into lowercase keywords:
// "My tenant hasn't paid rent!" -> tenant, paid, rent
inline LinkedList<std::string> tokenize(const std::string& text) {
    LinkedList<std::string> keywords;
    std::string lower = toLowerCase(text);
    std::string word = "";

    for (size_t i = 0; i < lower.length(); i++) {
        char c = lower[i];

        if (isWordChar(c)) {
            word += c;                     // still inside a word
        } else if (c == '\'') {
            continue;                      // skip apostrophes: "hasn't" -> "hasnt"
        } else {
            addKeyword(word, keywords);    // space or punctuation ends the word
        }
    }
    addKeyword(word, keywords);            // the text may end mid-word

    return keywords;
}