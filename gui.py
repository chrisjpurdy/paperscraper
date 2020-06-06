#1.0
from tkinter import ttk,messagebox,filedialog,Tk
import paper_finder,europe_pmc_annotations_finder
import glob2,os

str_window_title = "PBank Gene Locator"
str_window_error = "Error"

str_btn_open_file = "Open Text File of Keywords"
str_btn_get_papers_from_text = "Get Papers"
str_btn_get_annotations_from_papers = "Get Annotations from Papers"
str_btn_get_periph_from_annotations = "Get Peripheral info from Annotations"

str_error = "Error"
str_error_file_selection = "Unable to select file"
str_error_file_read = "Unable to read file"

file_types = [
    ('Text files', '*.txt'),  #list of filetypes, could be expanded
]

required_terms = "\"gene\"%20OR%20\"bacteria\"%20OR%20\"metabolite\"%20AND "

class Get_Keywords_App:
    file_loc = None
    txt_file = None
    papers = None
    def __init__(self, master):
        self.label = ttk.Label(master)
        self.label.grid(row = 0, column = 0, columnspan = 2)

        ttk.Button(master, text = str_btn_open_file,
                   command = self.select_file).grid(row = 1, column = 0)

        ttk.Button(master, text = str_btn_get_papers_from_text,
                   command = self.get_papers_from_text).grid(row = 1, column = 1)
        
        ttk.Button(master, text = str_btn_get_annotations_from_papers,
                   command = self.get_annotations_from_papers).grid(row = 1, column = 2)
        
        ttk.Button(master, text = str_btn_get_periph_from_annotations,
                   command = self.get_periph_from_annotations).grid(row = 1, column = 3)

    def select_file(self):
        try:
            file_loc = filedialog.askopenfilenames(initialdir=".",filetypes=file_types)
            self.file_loc = file_loc[0] #tuple to path name string
            with open(self.file_loc, 'r') as f:
                self.txt_file = f.read().splitlines()
        except:
            messagebox.showerror(str_error,str_error_file_selection)
            
    def get_papers_from_text(self):
        for i in range(0,len(self.txt_file)):
            print("query: " + self.txt_file[i])
            try:
                query = self.txt_file[i].replace(' ','%20')
                query = '\"' + query + '\"'
                ids = paper_finder.get_ids_from_euro_pmc(query,'*')
                with open(self.txt_file[i]+'.txt','w+',encoding="utf-8") as f:
                    f.writelines(ids)
            except:
                #if no results, try without exact search
                try:
                    query = self.txt_file[i].replace(' ','%20')
                    ids = paper_finder.get_ids_from_euro_pmc(query,'*')
                    with open(self.txt_file[i]+'.txt','w+',encoding="utf-8") as f:
                        f.writelines(ids)
                except:
                    print("No results found for search term "+self.txt_file[i])
                    
        filenames = glob2.glob('*.txt')
        while 'keywords.txt' in filenames:filenames.remove('keywords.txt')
        while 'results.txt' in filenames:filenames.remove('results.txt')
        print(filenames)
        

        with open('results.txt', 'w') as outfile:
            for fname in filenames:
                with open(fname) as infile:
                    for line in infile:
                        outfile.write(line)
        
        for i in range(0,len(filenames)):
            try:
                os.remove(filenames[i])
            except:
                print("Unable to remove "+filenames[i])
        
                    
    def get_annotations_from_papers(self):
         with open('results.txt') as f:
             papers_to_anno = f.readlines()
             print(papers_to_anno)
        #     for i in range(113301,len(papers_to_anno)):
        #         europe_pmc_annotations_finder.get_annotations_from_papers(papers_to_anno[i])
        
    def get_periph_from_annotations(self):
        print('click')
        
                
                    
def main():              
    main = Tk() 
    main.title("Meta-Analysis & Retrieval of Plant-Beneficial genes")
    app = Get_Keywords_App(main)
    main.mainloop()

if __name__ == "__main__": 
    main()