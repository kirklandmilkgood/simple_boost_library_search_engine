#include <iostream>
#include <string>
#include "httplib.h"
#include "../searcher/searcher.h"

const std::string g_input_path = "./files_parsed.txt";
const std::string g_root_path = "./http/WWW";

searcher::Searcher search;

void GetWebData(const httplib::Request &request, httplib::Response &response)
{
    if (!request.has_param("query"))
    {
        response.set_content("請求參數錯誤", "text/plain;charset=utf-8");
        return;
    }
    std::string query = request.get_param_value("query");
    std::string result;
    search.Search(query, &result);
    response.set_content(result, "application/json;charset=utf-8");
    std::cout << result << std::endl;
}

int main()
{
    bool ret = search.Init(g_input_path);
    if (ret == false)
    {
        std::cout << "searcher init error " << std::endl;
        return 1;
    }

    httplib::Server server;
    server.set_base_dir(g_root_path.c_str());
    server.Get("/searcher", GetWebData);
    server.listen("localhost", 5000);
    return 0;
}
