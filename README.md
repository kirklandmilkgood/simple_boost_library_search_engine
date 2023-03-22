# simple_boost_library_search_engine
簡易 c++ boost 函式庫文件搜尋引擎

boost在c++的函式庫中具有僅次於STL函式庫的重要地位，其當中定義了許多高效、好用的函數，但其官方網站的文檔有些雜亂，且並沒有所謂站內搜尋的功能，
因此本專案主要想解決想查詢boost函數文件時，需翻找半天的麻煩，並藉此練習使用C++進行字串處理與查詢

本專案之函數原始資料皆來自於boost官方網站：

<img width="710" alt="截圖 2023-03-22 下午12 43 07" src="https://user-images.githubusercontent.com/128550044/226804676-0b8690a7-d7b8-4013-bf34-eb8788c2d815.png">

本專案依照功能主要可分為5個部分： 爬蟲、字串處理、索引建立、根據關鍵字進行搜尋以及網路與伺服器
主要的對應檔案為： crawler.py  parser.cpp  searcher.cpp  
