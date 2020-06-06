#ver1.2
import urllib.request, json
import ssl,csv,ast
import pandas as pd
import get_periph_from_anno as get_periph


def get_annotations_from_papers(query):
    query = str(query)
    paper_id = query.partition(":")[2]
    query = query.replace(':','%3A')
    query = query.replace('\n','')
    print(query)
    
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    try:
        res_url = urllib.request.urlopen(
                "https://www.ebi.ac.uk/europepmc/annotations_api/annotationsByArticleIds?articleIds="+query+"&type=Gene_Proteins&format=json",context=ctx
                )
        
        res = json.loads(res_url.read().decode())
        res_url.close()
    
        res = res[0]
        
        res = res['annotations']
        resources = []
        # print(res)
        
        
        for i in range(0,len(res)):
            try:
                anno_exact = res[i]['exact']
            except:
                anno_exact = 'None'
                
            try:
                anno_tags = res[i]['tags']
            except:
                anno_tags = 'None'
                
            try:
                anno_name = anno_tags[0]['name']
            except:
                anno_name = 'None'
                
            try:
                anno_res = anno_tags[0]['uri']
            except:
                anno_res = 'None'
                
            try:
                anno_type = res[i]['type']
            except:
                anno_type = 'None'
                
            try:
                anno_provider = res[i]['provider']
            except:    
                anno_provider = 'None'
                
            try:
                anno_freq = res[i]['frequency']
            except:
                anno_freq = 'None'
                
            try:
                anno_prefix = res[i]['prefix']
            except:
                anno_prefix = 'None'
            
            try:
                anno_postfix = res[i]['postfix']
            except:
                anno_postfix = 'None'
        
            resources.append({'TYPE':anno_type,'NAME':anno_name,'freq':anno_freq,'provider':anno_provider,'resource':anno_res,'prefix':anno_prefix,'EXACT':anno_exact,'postfix':anno_postfix,'small_periph':anno_prefix+anno_exact+anno_postfix,'source':query})
        
        
        
        small_periph_list = []
        keys = resources[0].keys()
        title = query.replace('%3A','-')
        
        csv_file = pd.read_csv('annotations.csv')
        to_append_first_time = []
        
        for i in range(0,len(resources)):
            small_periph_list.append(resources[i]['small_periph'])
        long_annotations = get_periph.get_periph_from_id(paper_id, small_periph_list)
    
        print(len(long_annotations),len(small_periph_list))
        #CHECK IF CSV FILE IS EMPTY, if so then first time population
        # if len(csv_file['EXACT']) == 0:
        for i in range(0,len(resources)):
            to_append_first_time.append([resources[i]['EXACT'],resources[i]['resource'],(title,long_annotations[i])])
            
            # print(resources[i]['EXACT'],resources[i]['resource'],resources[i]['small_periph'])
    
        # to_append_first_time.sort()
        for i in range(0,len(to_append_first_time)):
            csv_file.loc[len(csv_file)] = to_append_first_time[i]
        
        csv_file.to_csv('annotations.csv',index = False)
    except:
        print("Unable to locate paper")
            



    
    
    # if len(csv_file['EXACT']):
        
        
        
    # column = csv_file['EXACT']
    # column1 = csv_file['sources']
    
    
    
    # running_annotation_list = []
    # first_time_list_population = []
    # to_add = []
    # for i in range(0,len(column)):
    #     running_annotation_list.append(column[i])
    # # print(running_annotation_list)
    
    

    # if not len(column):
    #     for i in range(0,len(resources)):
    #         found = False
    #         if i == 0:
    #             first_time_list_population.append({'EXACT':resources[i]['EXACT'],'resource':resources[i]['resource'],'sources':[(query.replace('%3A',' '),long_annotations[i])]})
    #         if i != 0:
    #             for x in range(0,len(first_time_list_population)):
    #                 if resources[i]['EXACT'] == first_time_list_population[x]['EXACT'] and found == False:
    #                     first_time_list_population[x]['sources'].append((query.replace('%3A',' '),long_annotations[i]))
    #                     found = True
    #                 if x == len(first_time_list_population)-1 and found == False:
    #                     first_time_list_population.append({'EXACT':resources[i]['EXACT'],'resource':resources[i]['resource'],'sources':[(query.replace('%3A',' '),long_annotations[i])]})
        

    #     keys = first_time_list_population[0].keys()
    #     with open('annotations.csv', 'w+',newline ='',encoding='utf-8') as f:
    #         dict_writer = csv.DictWriter(f, keys)
    #         dict_writer.writeheader()
    #         dict_writer.writerows(first_time_list_population)
    
    
    # csv_file = pd.read_csv('annotations.csv')
    # column = csv_file['EXACT']
    # column1 = csv_file['sources']
    
    # if len(column):
    #     for i in range(0,len(resources)):
    #         found = False
    #         for x in range(0,len(column)):
    #             if resources[i]['EXACT'] == column[x] and found == False:
    #                 spotted_index = x
    #                 to_write = eval(str(column1[spotted_index]))
    #                 to_write.append((query.replace('%3A',' '),long_annotations[i]))
    #                 csv_file.at[x,'sources'] = to_write
    #                 csv_file.to_csv('annotations.csv',index = False)
    #                 found = True
                    
    #             if x == len(column)-1 and found == False:
    #                 if len(to_add) == 1:
    #                     if to_add[0]['EXACT'] == resources[i]['EXACT']:
    #                         to_add[0]['sources'].append((query.replace('%3A',' '),long_annotations[i]))
    #                     else:
    #                         to_add.append({'EXACT':resources[i]['EXACT'],'resource':resources[i]['resource'],'sources':[(query.replace('%3A',' '),long_annotations[i])]})
    #                 if len(to_add) == 0:
    #                     to_add.append({'EXACT':resources[i]['EXACT'],'resource':resources[i]['resource'],'sources':[(query.replace('%3A',' '),long_annotations[i])]})
    #                 if len(to_add) > 1:
    #                     found1 = False
    #                     for z in range(0,len(to_add)):

    #                         if resources[i]['EXACT'] == to_add[z]['EXACT'] and found1 == False:
    #                             to_add[z]['sources'].append((query.replace('%3A',' '),long_annotations[i]))
    #                             found1 = True
    #                         if z == len(to_add) -1 and found1 == False:
    #                             to_add.append({'EXACT':resources[i]['EXACT'],'resource':resources[i]['resource'],'sources':[(query.replace('%3A',' '),long_annotations[i])]})
                    


    #     csv_file = pd.read_csv('annotations.csv')
            
        
    #     for i in range(0,len(to_add)):
    #         csv_file.loc[len(csv_file)] = to_add[i]
    #     csv_file.to_csv('annotations.csv',index = False)
        
        
            
                    
                    
        

