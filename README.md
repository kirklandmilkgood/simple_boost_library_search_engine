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

# 索引建立
索引分為正序索引以及倒序索引，正序表示從文件id得到文件內容，而倒序則是由文件內容得到文件id
在本專案中他們分別以一個struct表示：
struct frontIdx
{
int64_t _docId;
string _title;
string _url;
string _content;
} (正序： 由文件id定位到文件內容)

struct backwardIdx
{
int64_t _docId;
int _weight;
string _word;
} (倒序： 由關鍵字定位到文件id)

由於建立索引會需要對句子進行分詞的處理，這裡使用jieba函式庫(就是專案中cpp_jieba檔案夾裡的檔案)對文件內容進行分詞，此函式庫會根據各種詞彙、標點符號進行分詞，
我們再根據分詞的結果，把分詞出來的詞彙一個一個使用前面兩個struct建立索引

# 關鍵字搜尋
由於前面已經先建立索引了，利用關鍵字搜尋時，只需先用倒序索引找到各符合條件的文件id，再利用文件id查詢正序索引，取得文件的標題、url以及內文，方便後續呈現在網頁上，
另外由於倒序索引中有根據關鍵字在文件中出現的頻率進行權重的計算，因此在呈現搜尋結果時，會以此作為呈現時先後順序的依據

# 網路與伺服器
在此網頁部分使用http、ajax以及vue進行網頁內容的編寫，而後端則是使用c++ 的httplib與前面索引、搜尋的演算法進行串接，client以get方法請求資源，利用client傳送的
關鍵字進行搜尋，再將結果呈現在網頁上，如下面demo所展示：

搜尋引擎執行檔執行後，在瀏覽器上輸入程式預設的localhost:5000，可以看到以下頁面：

<img width="1111" alt="截圖 2023-03-22 下午1 40 23" src="https://user-images.githubusercontent.com/128550044/226813007-0a2dbc37-302d-4e85-af17-63c61bb91ebb.png">


在搜尋欄輸入關鍵字 "virtual"，搜尋結果如下展示：

<img width="1055" alt="截圖 2023-03-22 下午1 40 56" src="https://user-images.githubusercontent.com/128550044/226813043-2eafbfaf-a1e8-46c6-b966-58276705e771.png">

搜尋結果較多時可以按上下頁按鈕切換：


<img width="1072" alt="截圖 2023-03-22 下午1 41 07" src="https://user-images.githubusercontent.com/128550044/226813121-4050c1a0-2311-477e-85b4-26144e741d4a.png">
