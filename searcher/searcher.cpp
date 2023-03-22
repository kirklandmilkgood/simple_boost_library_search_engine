#include "searcher.h"
#include "../common_tools/util.hpp"
#include <json/value.h>
#include <json/writer.h>
#include <json/json.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;

namespace searcher
{

    // jieba分詞辭典路徑
    const char *const DICT_PATH = "cpp_jieba/dict/jieba.dict.utf8";
    const char *const HMM_PATH = "cpp_jieba/dict/hmm_model.utf8";
    const char *const USER_DICT_PATH = "cpp_jieba/dict/user.dict.utf8";
    const char *const IDF_PATH = "cpp_jieba/dict/idf.utf8";
    const char *const STOP_WORD_PATH = "cpp_jieba/dict/stop_words.utf8";

    Index::Index()
        : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)

    {
        forward_index.clear();
        inverted_index.clear();
    }
    // 查正排索引

    const frontIdx *Index::GetFrontIdx(const int64_t doc_id)
    {
        if (doc_id < 0 || doc_id >= forward_index.size())
        {
            return nullptr;
        }
        return &forward_index[doc_id];
    }
    // 查倒排索引

    const vector<backwardIdx> *Index::GetBackwardIdx(const string &key)
    {
        auto it = inverted_index.find(key);
        if (it == inverted_index.end())
        {
            return nullptr;
        }
        return &(it->second);
    }

    // jieba分詞
    void Index::CutWord(const string &input, vector<string> *output)
    {
        jieba.CutForSearch(input, *output);
    }

    // 根據一行預處理的文件，得到一個正排索引節點，並插入正排容器中
    frontIdx *Index::BuildForward(const string &line)
    {
        vector<string> nums;
        common::Util::Spilt(line, "\3", &nums);
        if (nums.size() != 3)
        {
            cout << "file num error " << nums.size() << endl;
            return nullptr;
        }
        frontIdx docinfo;
        docinfo._docId = forward_index.size();
        docinfo._title = nums[0];
        docinfo._url = nums[1];
        docinfo._content = nums[2];
        forward_index.push_back(std::move(docinfo));

        return &forward_index.back();
    }

    // 根據正排索引節點，建構倒排索引節點
    void Index::BuildInverted(const frontIdx &docinfo)
    {
        // 統計關鍵字在標題 正文 出現的次數
        struct WordCnt
        {
            int _titleCnt;   // 標題出現次數
            int _contentCnt; // 正文出現次數
            WordCnt()
                : _titleCnt(0), _contentCnt(0) {}
        };

        unordered_map<string, WordCnt> WordMap;
        // 對標題進行分詞
        vector<string> titleWord;
        CutWord(docinfo._title, &titleWord);
        for (string word : titleWord)
        {
            // 轉小寫
            boost::to_lower(word);
            WordMap[word]._titleCnt++;
        }

        // 對正文進行分詞
        vector<string> contentWord;
        CutWord(docinfo._content, &contentWord);
        for (string word : contentWord)
        {
            boost::to_lower(word);
            WordMap[word]._contentCnt++;
        }

        // 統計結果，插入倒排索引中
        for (const auto &word_pair : WordMap)
        {
            backwardIdx backIdx;
            backIdx._docId = docinfo._docId;

            // 定義加權方式：10*titleCnt + contentCnt
            backIdx._weight = 10 * word_pair.second._titleCnt + word_pair.second._contentCnt;
            backIdx._word = word_pair.first;

            vector<backwardIdx> &back_vector = inverted_index[word_pair.first];
            back_vector.push_back(std::move(backIdx));
        }
    }

    // 建立索引
    bool Index::Build(const string &input_path)
    {
        cout << input_path << "build index begin" << endl;
        std::ifstream file(input_path);
        if (!file.is_open())
        {
            cout << input_path << "file open error" << endl;
            return false;
        }
        string line;
        int idx = 0;
        static string progress("|/-\\");

        while (std::getline(file, line))
        {
            frontIdx *docinfo = BuildForward(line);
            if (docinfo == nullptr)
            {
                cout << "forward build error" << endl;
                continue;
            }

            BuildInverted(*docinfo);

            if (!docinfo->_docId % 100)
            {
                cout << "\r" << progress[idx % 4] << docinfo->_docId << "sucessed " << std::flush;
                idx++;
            }
        }
        cout << "index build successed" << endl;
        file.close();
        return true;
    }

    // searcher類
    const int lengthText = 160;
    // 初始化，建構文檔索引
    bool Searcher::Init(const string &input_path)
    {
        return index->Build(input_path);
    }

    string Searcher::GetShowContent(const string &content, const string &word)
    {
        size_t idx = content.find(word);
        string ans("");
        int pos = 0;
        int len = lengthText;
        if (idx == string::npos)
        {
            len = std::min(len, (int)content.size());
        }
        else
        {
            pos = std::max(0, (int)((int)idx - lengthText / 2));
            len = std::min((int)lengthText / 2, (int)(content.size() - idx));
        }
        ans = content.substr(pos, len);
        ans += "...";
        return ans;
    }

    // 對文本進行搜尋
    bool Searcher::Search(const string &query, string *output)
    {
        // 分詞
        vector<string> words;
        index->CutWord(query, &words);

        // 根據分詞結果，進行倒排索引，得到相關文檔
        vector<backwardIdx> wordsResult;
        for (string word : words)
        {
            boost::to_lower(word);
            auto *backList = index->GetBackwardIdx(word);
            if (backList == nullptr)
            {
                // 無此關鍵字
                continue;
            }
            wordsResult.insert(wordsResult.end(), backList->begin(), backList->end());
        }
        // 排序
        std::sort(wordsResult.begin(), wordsResult.end(),
                  [](const backwardIdx &le, const backwardIdx &ri)
                  { return le._weight > ri._weight; });

        // 包裝成json格式
        Json::Value value;
        for (const auto &backidx : wordsResult)
        {
            // 根據id查詢正排索引
            const frontIdx *docinfo = index->GetFrontIdx(backidx._docId);

            Json::Value tmp;
            tmp["title"] = docinfo->_title;
            tmp["url"] = docinfo->_url;
            tmp["desc"] = GetShowContent(docinfo->_content, backidx._word);
            value.append(tmp);
        }
        Json::FastWriter writer;
        *output = writer.write(value);
        return true;
    }
}
