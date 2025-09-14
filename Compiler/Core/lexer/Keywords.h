//------------------------------------------------------------------------------
/*
* Author: Perijn
* Version: V1.0.0
* File: Keywords.h
* Moodified: 18/03/2025
*/
//------------------------------------------------------------------------------

#pragma once
#ifndef _KEYWORDS_H_
#define _KEYWORDS_H_

//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // isdigit, isaplha, isalnum

//------------------------------------------------------------------------------

typedef enum {
    KEYWORD_NONE = 0,
    KEYWORD_IF,
    KEYWORD_ELSE,

    KEYWORD_CHAR,
    KEYWORD_SHORT,
    KEYWORD_INT,
    KEYWORD_LONG,

    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,

    KEYWORD_TYPEDEF,

    KEYWORD_UNKNOWN = 255,
} keyword_t;

//------------------------------------------------------------------------------

typedef struct TrieNode {
    uint8_t c;
    uint8_t isEndOfWord;
    keyword_t value;
    struct TrieNode* left, * mid, * right;
} TrieNode;

// Function to create a new node
static TrieNode* createNode(char c) {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    node->c = c;
    node->isEndOfWord = 0;
    node->value = KEYWORD_NONE;
    node->left = node->mid = node->right = NULL;
    return node;
}

// Insert a word into the TST
static TrieNode* insert(TrieNode* root, const char* word, keyword_t keywordId) {
    if (*word == '\0') return root;

    if (root == NULL) {
        root = createNode(*word);
    }

    if (*word < root->c) {
        root->left = insert(root->left, word, keywordId);
    }
    else if (*word > root->c) {
        root->right = insert(root->right, word, keywordId);
    }
    else {
        if (*(word + 1) == '\0')
        {
            root->isEndOfWord = 1;
            root->value = keywordId;  // Assign unique enum ID
        }
        else
        {
            root->mid = insert(root->mid, word + 1, keywordId);
        }
    }

    return root;
}

// Search for a word in the TST
static keyword_t search(TrieNode* root, const char* word) {
    if (root == NULL) return KEYWORD_NONE;

    if (*word < root->c) {
        return search(root->left, word);
    }
    else if (*word > root->c) {
        return search(root->right, word);
    }
    else {
        if (*(word + 1) == '\0')
            if (root->isEndOfWord)
            {
                return (keyword_t)root->value;
            }
            else {
                return KEYWORD_NONE;
            }
        return search(root->mid, word + 1);
    }
}

static const char* getKeywordName(keyword_t id)
{
    switch (id)
    {
    case KEYWORD_IF:
        return "if";
    case KEYWORD_ELSE:
        return "else";
    case KEYWORD_CHAR:
        return "char";
    case KEYWORD_SHORT:
        return "short";
    case KEYWORD_WHILE:
        return "while";
    case KEYWORD_FOR:
        return "for";
    case KEYWORD_RETURN:
        return "return";
    case KEYWORD_INT:
        return "int";
    case KEYWORD_TYPEDEF:
        return "typedef";
    default:
        return "";
    }
}

// Insert all C++ keywords
static void insertKeywords(TrieNode** root) {
   /* const char* keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
        "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long",
        "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct",
        "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };

    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        *root = insert(*root, keywords[i]);
    }*/

    for (uint8_t i = 1; i < KEYWORD_UNKNOWN; i++)
    {
        keyword_t test = (keyword_t)i;
        const char* KeyWord = getKeywordName((keyword_t)i);

        if (KeyWord[0] == '\0')
            return;

        *root = insert(*root, KeyWord, (keyword_t)i);
    }
}

//------------------------------------------------------------------------------

#endif // !_COMPILER_TOKENIZER_H_

//------------------------------------------------------------------------------
