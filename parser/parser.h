#pragma once

#include <string>
#include <fstream>

using std::string;

struct DocInfo
{
    string _title;
    string _url;
    string _content;
};

extern string target_files_path;
extern string output_file_path;

bool ParseContent(const string &html, string *content);
void ParseTitle(const string &file_name, string *title);
bool ParseFile(const string &file_path, DocInfo *docinfo);
void WriteOutput(const DocInfo &docinfo, std::ofstream &ofstream);
void ParseActivate();
void CheckFileParsed();
