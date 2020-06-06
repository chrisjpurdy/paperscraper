#########################
#DEPRECATED - DO NOT USE#
#########################

import pandas as pd

def insert_row(row_number, df, row_value): 
	# Slice the upper half of the dataframe 
	df1 = df[0:row_number] 

	# Store the result of lower half of the dataframe 
	df2 = df[row_number:] 

	# Inser the row in the upper half dataframe 
	df1.loc[row_number]=row_value 

	# Concat the two dataframes 
	df_result = pd.concat([df1, df2]) 

	# Reassign the index labels 
	df_result.index = [*range(df_result.shape[0])] 

	# Return the updated dataframe 
	return df_result 

def prettify():
    pd.set_option('display.max_rows', 1000)
    list_slices = []
    csv_file = pd.read_csv('annotations.csv')
    column_exact = csv_file['EXACT']
    column_resource = csv_file['resource']
    column_sources = csv_file['sources']
    
    column_exact = pd.Series(column_exact).drop_duplicates().tolist() #list of unique genes
    for i in range(0,len(column_sources)):
        column_sources[i] = eval(column_sources[i])
    csv_file['sources'] = column_sources
    
    for i in range(0,len(column_exact)):
        to_slice = []
        for z in range(0,len(csv_file['EXACT'])):
            if csv_file['EXACT'][z] == column_exact[i]:
                to_slice.append(z)
        

        if len(to_slice):
            list_slices.append(csv_file[to_slice[0]:to_slice[len(to_slice)-1]+1])
    

    
    for a in range(0,len(list_slices)):
        print(type(list_slices[a]['sources']))
        

        

    

            
        
    # for i in range(0,len(column_sources)):
    #     column_sources[i] = eval(column_sources[i])
    
    # for i in range(0,len(column_exact)):
    #     found = False

    #     for z in range(0,len(csv_file['EXACT'])):
    #         print(z)
    #         if found == False and column_exact[i] == csv_file['EXACT'][z]:
    #             print(i,z)
    #             found = True

    # for i in range(0,len(column_sources)):
    #     if len(column_sources[i]) > 1: 
    #         for x in range(0,len(column_sources[i])):
    #             csv_file = insert_row(i+x,csv_file,[column_exact[i],column_resource[i],column_sources[i][x]])
    
    # csv_file.to_csv('annotations.csv',index = False)

