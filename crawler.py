from bs4 import BeautifulSoup
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import requests
from urllib.request import urlretrieve
import re
from concurrent.futures import ThreadPoolExecutor
from urllib import parse
import os
import json
import random


root_url = 'https://www.boost.org/doc/libs/1_80_0/'
target_folder = '/Users/zhangxicheng/Desktop/boost_lib_file/'
json_path = '/Users/zhangxicheng/Desktop/'
url_title_pairs = {}
regex = re.compile('^(?!^http.*)^(?!javascript:.*)([^#@]+\.(htm|html)$)')


def dict_to_json():
    with open(json_path+'url_title_pairs.json', 'w+') as utp:
        json.dump(url_title_pairs, utp)


def download_file(source, title):
    target_path = target_folder+'{}.html'.format(title)
    with open(target_path, 'a+'):
        urlretrieve(source, target_path)


def get_web_title(target_url):
    soup = get_raw_text(target_url)
    web_title = soup.find('title').text.replace(u'\xa0', ' ')
    if web_title and '404 Not Found' not in web_title and target_url not in url_title_pairs.values():
        if web_title in url_title_pairs.keys():
            web_title = web_title + str(random.randint(1, 10000))

        url_title_pairs[web_title] = target_url
        download_file(target_url, web_title)


def get_raw_text(raw_url):
    resp = requests.get(raw_url)
    soup = BeautifulSoup(resp.text, 'html.parser')
    return soup


def next_layer_url_pinpoint(parent_url):
    soup = get_raw_text(parent_url)
    next_layer_url = soup.findAll('a', href=regex)
    next_layer_url = [parse.urljoin(parent_url, url.attrs['href']) for url in next_layer_url]
    with ThreadPoolExecutor(max_workers=15) as executor:
        executor.map(get_web_title, next_layer_url)


def webdriver_url_finder(raw_url):
    chrome_options = Options()
    chrome_options.add_argument('--headless')
    browser = webdriver.Chrome('./chromedriver', options=chrome_options)
    browser.get(raw_url)
    actual_url = browser.current_url
    soup = BeautifulSoup(browser.page_source, 'html.parser')
    web_title = soup.find('title').text.replace(u'\xa0', ' ')
    if web_title and '404 Not Found' not in web_title and actual_url not in url_title_pairs.values():
        if web_title in url_title_pairs.keys():
            web_title = web_title+str(random.randint(1, 10000))
        url_title_pairs[web_title] = actual_url
        browser.quit()
        download_file(actual_url, web_title)
        print(actual_url + ' ' + 'done' + '\n')


def find_actual_url(raw_url):
    with ThreadPoolExecutor(max_workers=15) as executor:
        executor.map(webdriver_url_finder, raw_url)


def lib_url_pinpoint(soup):
    url_parent = soup.findAll('dt', id=re.compile('^lib-'))
    url_list = [url.a.attrs['href'] for url in url_parent]
    url_list = [parse.urljoin(root_url, url) for url in url_list]
    return url_list


def lib_url_search():
    os.mkdir(target_folder)
    soup = get_raw_text(root_url)
    raw_url = lib_url_pinpoint(soup)
    find_actual_url(raw_url)
    parent_url = list(url_title_pairs.values())
    for url in parent_url:
        next_layer_url_pinpoint(url)
    dict_to_json()
    
    print(len(url_title_pairs), url_title_pairs)


def main():
    lib_url_search()


if __name__ == '__main__':
    main()