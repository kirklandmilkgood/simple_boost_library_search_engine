#include "parser.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <json/json.h>
#include <json/value.h>
#include "../common_tools/util.hpp"

using boost::filesystem::recursive_directory_iterator;
using Json::Reader;
using Json::Value;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::unordered_map;
using std::vector;

string target_files_path = "../../boost_lib_file/";
string output_file_path = "../../files_parsed.txt";

bool ParseContent(const string &html, string *content)
{
    bool is_content = true;
    for (auto c : html)
    {
        if (is_content)
        {
            if (c == '<')
            {
                is_content = false;
            }
            else
            {
                if (c == '\n' || c == '\r' || c == '\0')
                {
                    c = ' ';
                }
                content->push_back(c);
            }
        }
        else
        {
            if (c == '>')
            {
                is_content = true;
            }
            // 忽略標籤中內容 <a>
        }
    }
    return true;
}

void ParseTitle(const string &file_name, string *title)
{
    for (auto c : file_name)
    {
        if (c == '\n' || c == '\r')
            continue;
        title->push_back(c);
    }
}

bool ParseFile(const string &file_path, DocInfo *docinfo)
{
    string html;
    bool ret = common::Util::Read(file_path, &html);
    if (!ret)
    {
        cout << file_path << "file read error" << endl;
        return false;
    }
    ret = ParseContent(html, &docinfo->_content);
    if (!ret)
    {
        cout << "cotent analysis error" << endl;
        return false;
    }
    return true;
}

void WriteOutput(const DocInfo &docinfo, std::ofstream &ofstream)
{
    ofstream << docinfo._title << "\3" << docinfo._url
             << "\3" << docinfo._content << endl;
}

void ParseActivate()
{
    recursive_directory_iterator end_iter;
    ofstream output_file(output_file_path);
    ifstream url_file("../../url_title_pairs.json");
    Reader reader;
    Value data;
    reader.parse(url_file, data);
    for (recursive_directory_iterator iter(target_files_path); iter != end_iter; iter++)
    {
        if (iter->path().extension() == ".htm" || iter->path().extension() == ".html")
        {
            string file_path = iter->path().string();
            auto key = iter->path().stem().string();
            DocInfo docinfo;
            docinfo._url = data[key].asString();
            ParseTitle(key, &docinfo._title);
            bool ret = ParseFile(file_path, &docinfo);
            if (!ret)
            {
                cout << "file parsing error:" << docinfo._title << endl;
                continue;
            }
            WriteOutput(docinfo, output_file);
        }
    }
    output_file.close();
    url_file.close();
}

void CheckFileParsed()
{
    ifstream parsed_file(output_file_path);
    if (parsed_file.peek() == '\0')
    {
        ParseActivate();
    }
    parsed_file.close();
}