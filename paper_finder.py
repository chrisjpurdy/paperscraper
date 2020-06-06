#ver1.0
#get_ids_from_euro_pmc("\"Biological%20Nitrogen%20Fixation\"(Gene*%20OR%20Protein*%20OR%20Metabolite*)",'*')
import urllib as url
import xmltodict as xml

from json import loads,dumps
import ssl
page_size = 1000
titles = []

counter = 0

#this function recursively finds all papers indexed by europe pmc given a paper ID and cursor mark
#pass the value '*' into cursor mark to get annotations from
#annotation


def get_ids_from_euro_pmc(query,cursor_mark):
    global counter
    global titles
    query = str(query)
    if cursor_mark == '*':
        counter = 0
        titles = []
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    res_url = url.request.urlopen(
            "https://www.ebi.ac.uk/europepmc/webservices/rest/search?query="+query+"&resultType=lite&cursorMark="+cursor_mark+"&pageSize=1000&format=xml",context=ctx
            )
    res = res_url.read()
    res_url.close()
    res = xml.parse(res)
    res_dict = loads(dumps(res))
    try:
        res_dict = res_dict['responseWrapper']
        next_cursor_mark = res_dict['nextCursorMark']
        res_dict = res_dict['resultList']['result']
        print(next_cursor_mark)
        print(len(res_dict))
        for i in range(0,len(res_dict)):
            try:
                titles.append(res_dict[i]['source'] +':'+ res_dict[i]['id']+'\n')
            except:
                print("Can't read ID / Cursor Mark!")
        counter += 1
        if len(res_dict) == 1000:
            get_ids_from_euro_pmc(query,next_cursor_mark)
    
        return titles
    except:
        print("Can't read ID! / Cursor Mark")
