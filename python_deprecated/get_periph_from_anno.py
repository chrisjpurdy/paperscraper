import pandas as pd
import get_med_free_full_text as fulltext
import re

def get_periph(paper_id):
    csv_file_name = paper_id.replace(':','-')+'.csv'
    csv_file = pd.read_csv(csv_file_name)
    exact = csv_file['EXACT']
    prefix = csv_file['prefix']
    postfix = csv_file['postfix']
    small_periph = csv_file['small_periph']
    accession = csv_file['resource']

    for i in range(0,len(small_periph)):
        with open('paper_text_28348550.txt','r',encoding='utf-8') as f:
            text_file = f.read()
            text_file_backward = text_file[::-1]
            index_anno = text_file.find(small_periph[i])
            index_anno_r = len(text_file_backward) - index_anno
            
            if index_anno:
                fullstop_f = text_file.find('.',index_anno+len(small_periph[i]))
                fullstop_f_2 = text_file.find('.',fullstop_f+1)
                fullstop_r = text_file_backward.find('.',index_anno_r+len(small_periph[i]))
                fullstop_r_2 = text_file_backward.find('.',fullstop_f+1)
                
                fullstop_r_conv = len(text_file) - fullstop_r


                print(text_file[fullstop_r_conv+1:fullstop_f_2+1]+'\nAnnotation = '+exact[i])
            if index_anno == -1:
                print("\nAnnotation = "+exact[i]+" Not found")
        

# def get_periph_from_id(paper_id,list_small_periph):
#     list_to_return = []
#     success = fulltext.get_free_full_text(paper_id)
#     if not success:
#         print('No Full Text Available')
#         return list_small_periph
#     if success:
#         for i in range(0,len(list_small_periph)):
#             with open('paper_text_'+str(paper_id)+'.txt','r',encoding='utf-8') as f:
#                 text_file = f.read()
#                 text_file_backward = text_file[::-1]
#                 index_anno = text_file.find(list_small_periph[i])
#                 index_anno_r = len(text_file_backward) - index_anno
#                 if index_anno:
#                     fullstop_f = text_file.find('.',index_anno+len(list_small_periph[i]))
#                     fullstop_f_2 = text_file.find('.',fullstop_f+1)
#                     fullstop_r = text_file_backward.find('.',index_anno_r+len(list_small_periph[i]))
#                     fullstop_r_2 = text_file_backward.find('.',fullstop_f+1)
                    
#                     fullstop_r_conv = len(text_file) - fullstop_r
#                     list_to_return.append(text_file[fullstop_r_conv+1:fullstop_f_2+1])
#                 if index_anno == -1:
#                     print("Not Found",list_small_periph[i])
#                     list_to_return.append(list_small_periph[i])
        
#         print(list_to_return)
#         return list_to_return
    
def get_periph_from_id(paper_id,list_small_periph):
    success = fulltext.get_free_full_text(paper_id)
    if not success:
        print('No Full Text Available')
        return list_small_periph
    if success:
        paper_text = 0
        sentences = 0
        to_return = []
        with open('paper_text_'+str(paper_id)+'.txt','r',encoding='utf-8') as f:
            paper_text = f.read()
            sentences = split_into_sentences(paper_text)
        for i in range(0,len(list_small_periph)):
            print(i)
            found = False
            for z in range(0,len(sentences)):
                if list_small_periph[i] in sentences[z] and found == False:
                    #print(list_small_periph[i],'-   :MATCHES:   -',sentences[z])
                    # to_return.append(sentences[z])
                    if z == 0:

                        to_return.append(sentences[z]+sentences[z+1])
                    if z == len(list_small_periph):

                        to_return.append(sentences[z-1]+sentences[z])
                    if z != 0 and z != len(list_small_periph):
                        to_return.append(sentences[z-1]+sentences[z]+sentences[z+1])
                    found = True
                    
                if z == len(sentences)-1 and found == False:

                    to_return.append(list_small_periph[i])

        return to_return
        

def split_into_sentences(text):
    alphabets= "([A-Za-z])"
    prefixes = "(Mr|St|Mrs|Ms|Dr)[.]"
    suffixes = "(Inc|Ltd|Jr|Sr|Co|al)"
    starters = "(Mr|Mrs|Ms|Dr|He\s|She\s|It\s|They\s|Their\s|Our\s|We\s|But\s|However\s|That\s|This\s|Wherever)"
    acronyms = "([A-Z][.][A-Z][.](?:[A-Z][.])?)"
    websites = "[.](com|net|org|io|gov)"
    digits = "([0-9])"
    text = " " + text + "  "
    text = text.replace("\n"," ")
    text = re.sub(prefixes,"\\1<prd>",text)
    text = re.sub(websites,"<prd>\\1",text)
    if "Ph.D" in text: text = text.replace("Ph.D.","Ph<prd>D<prd>")
    text = re.sub("\s" + alphabets + "[.] "," \\1<prd> ",text)
    text = re.sub(acronyms+" "+starters,"\\1<stop> \\2",text)
    text = re.sub(alphabets + "[.]" + alphabets + "[.]" + alphabets + "[.]","\\1<prd>\\2<prd>\\3<prd>",text)
    text = re.sub(alphabets + "[.]" + alphabets + "[.]","\\1<prd>\\2<prd>",text)
    text = re.sub(" "+suffixes+"[.] "+starters," \\1<stop> \\2",text)
    text = re.sub(" "+suffixes+"[.]"," \\1<prd>",text)
    text = re.sub(" " + alphabets + "[.]"," \\1<prd>",text)
    text = re.sub(digits + "[.]" + digits,"\\1<prd>\\2",text)
    if "”" in text: text = text.replace(".”","”.")
    if "\"" in text: text = text.replace(".\"","\".")
    if "!" in text: text = text.replace("!\"","\"!")
    if "?" in text: text = text.replace("?\"","\"?")
    text = text.replace(".",".<stop>")
    text = text.replace("?","?<stop>")
    text = text.replace("!","!<stop>")
    text = text.replace("<prd>",".")
    sentences = text.split("<stop>")
    sentences = sentences[:-1]
    sentences = [s.strip() for s in sentences]
    return sentences

# sentences = 0
# with open('paper_text_28348550.txt','r',encoding = 'utf-8') as f:
#     text = f.read()
#     sentences = split_into_sentences(text)
# print(sentences)