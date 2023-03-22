#pragma once

#include <stdint.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../cpp_jieba/include/cppjieba/Jieba.hpp"

using std::string;
using std::unordered_map;
using std::vector;

namespace searcher
{

    struct frontIdx
    {                    // 用於儲存正排索引的資訊
        int64_t _docId;  // 文檔id
        string _title;   // 標題
        string _url;     // 網址
        string _content; // 文檔內容
    };

    struct backwardIdx
    {
        int64_t _docId;
        int _weight;
        string _word;
    };

    class Index
    {
    public:
        Index();
        // 正排索引查詢
        const frontIdx *GetFrontIdx(const int64_t doc_id);
        // 到排索引查詢
        const vector<backwardIdx> *GetBackwardIdx(const string &key);
        // 建立正排與倒排索引
        bool Build(const string &input_path);
        // 對文檔內容進行分詞
        void CutWord(const string &input, vector<string> *output);

    private:
        // 根據預處理的文件內容，得到正排索引的一個節點
        frontIdx *BuildForward(const string &line);
        // 根據正排索引節點，建構到排索引節點
        void BuildInverted(const frontIdx &docinfo);
        // 正排索引
        vector<frontIdx> forward_index;
        // 倒排索引
        unordered_map<string, vector<backwardIdx>> inverted_index;
        // jieba分詞工具
        cppjieba::Jieba jieba;
    };

    class Searcher
    {
    public:
        Searcher()
            : index(new Index()) {}
        // 初始化，建構文檔索引
        bool Init(const string &input_path);
        // 對文檔進行搜索
        bool Search(const string &query, string *output);

    private:
        // 得到query前後文字，有顯示於前端頁面
        string GetShowContent(const string &content, const string &word);
        // 建立索引進行搜索
        Index *index;
    };
}