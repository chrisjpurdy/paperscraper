import europe_pmc_annotations_finder as annofinder
with open('results.txt','r',encoding='utf-8') as f:
    papers = f.readlines()

for i in range(2154081,2154181):
    try:
        annofinder.get_annotations_from_papers(str(papers[i].replace('\n','')))
    except:
        print('not_found')