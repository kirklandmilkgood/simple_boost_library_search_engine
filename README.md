# simple_boost_library_search_engine
簡易 c++ boost 函式庫文件搜尋引擎

boost在c++的函式庫中具有僅次於STL函式庫的重要地位，其當中定義了許多高效、好用的函數，但其官方網站的文檔有些雜亂，且並沒有所謂站內搜尋的功能，
因此本專案主要想解決想查詢boost函數文件時，需翻找半天的麻煩，並藉此練習使用C++進行字串處理與查詢

本專案之函數原始資料皆來自於boost官方網站：

<img width="710" alt="截圖 2023-03-22 下午12 43 07" src="https://user-images.githubusercontent.com/128550044/226804676-0b8690a7-d7b8-4013-bf34-eb8788c2d815.png">

本專案依照功能主要可分為5個部分： 爬蟲、字串處理、索引建立、根據關鍵字進行搜尋以及網路與伺服器
主要的對應檔案為： crawler.py  parser.cpp  searcher.cpp  以及main.cpp

# 爬蟲
首先利用python的beautifulsoup以及正則表達式等package對boost網站上函數文件的部分進行爬蟲，根據其網頁html檔來擷取我們想要的資訊，使用正則表達式主要是因為，網站上有許多
圖片、pdf檔等等無法使用本專案解析的內容，因此用正則表達式判斷各網頁連結是否含有.jpg .pdf等等我們不想要的內容，有的話直接忽略

# 字串處理
根據python爬取後的結果，我們再使用c++  string的各種內建函式進行解析，以得到各html檔的標題、內文以及url，例如使用find、substr等函數來篩選掉html中標籤的部分(ex: <title> </title>)，然後再將解析的結果存到一個txt檔中 (如下圖所示，一行代表一個網頁的解析結果，將其標題 url 以及內文記錄在一行中)


<img width="1251" alt="截圖 2023-03-22 下午1 05 38" src="https://user-images.githubusercontent.com/128550044/226807110-50e10e34-ebcb-42ea-b316-2d1c8cb218fc.png">
