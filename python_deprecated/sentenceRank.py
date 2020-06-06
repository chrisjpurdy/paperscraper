import pandas as pd
import numpy as np
import re
from ast import literal_eval as make_tuple
pd.set_option('display.max_columns', 500)

commonWords = ["the", "or", "a", "and", "then", "that", "this", "these", "there", "he", "she", "where", "why", "who", "when", "how", "is", "was", "using", "of", "et", "al", "in", "same", "as", 'through']
	
def updateWord(model, word, success):
	word = word.lower()
	
	if word not in model.index:
		model.at[word, 'rank'] = int(success)
		model.at[word, 'count'] = 1
		model.at[word, 'isCommon'] = word in commonWords
	else:
		model.at[word, 'count'] += 1
		model.at[word, 'rank'] = model.at[word, 'rank'] + ((int(success) - model.at[word, 'rank']) / model.at[word, 'count'])
	
def getWordRank(model, word):
	word = word.lower()
	if (word not in model.index) or isCommonWord(model, word):
		return 0.5
	return model.at[word, 'rank']
	
def isCommonWord(model, word):
	word = word.lower()
	if (word in model.index):
		return bool(model.at[word, 'isCommon'])
	return False
	
def getWordList(sentence):
	sentence = sentence.replace(',',' ')
	sentence = sentence.replace('\"',' ')
	sentence = sentence.replace('\\',' ')
	sentence = sentence.replace('/',' ')
	sentence = sentence.replace(';',' ')
	sentence = sentence.replace(':',' ')
	sentence = sentence.replace('.',' ')
	sentence = sentence.replace('\n',' ')
	sentence = sentence.replace('\r',' ')
	sentence = sentence.replace('(',' ')
	sentence = sentence.replace(')',' ')
	return sentence.split(' ')

	
def updateRanks(model, gene_name, sentence, exact, good_words, bad_words, periph_useful):
	
	sentence.replace(gene_name,' ')
	
	words = getWordList(sentence)
	
	for word in words:
		
		modExactScore = int(exact or periph_useful)
		
		if (len(word) > 2): #skip blanks between multiple spaces and single letter words
		
			if not isCommonWord(model, word): # or (word in gene_words)): # and skips common words (and the gene being searched for???)
			
				# !!! might need changing since it moves ranking outside of 0 - 1
				if word in good_words:
					modExactScore = 2
				elif word in bad_words:
					modExactScore = -2
		
				if (word[len(word) - 1] in ['.','\'','`']): #trim off a prefixed fullstop or ' or `
					updateWord(model, word[:-1], modExactScore)
				else:
					updateWord(model, word, modExactScore)
				
		
def rankSentence(model, sentence):
	
	words = getWordList(sentence)
	
	averageRank = 0.0
	count = 0.0
	
	for word in words:
		count += 1.0
		averageRank += getWordRank(model, word)
	
	return (averageRank/count)
	

def loadAnnotFile(filename):
	print("-- Loading annotations file: " + filename + " --")
	data = pd.read_csv(filename)
	newAnnotData = pd.DataFrame({"score":data.Score, "gene_name":data.EXACT, "resource":data.resource, "sources":data.sources, "exact_correct":data.Exact_Correct, "correct_words":str(data.Periph_Include), "incorrect_words":str(data.Periph_Exclude), "periph_useful":data.Periph_Useful})
	newAnnotData.index.name = "index"
	
	for i in newAnnotData.index:
		
		newAnnotData.at[i, 'sources'] = make_tuple(newAnnotData.at[i, 'sources'])
		
		#split up correct and incorrect words
		newAnnotData.at[i, 'correct_words'] = newAnnotData.at[i, 'correct_words'].split(';')
		for word in newAnnotData.at[i, 'correct_words']:
			word = word.replace(' ', '')
		newAnnotData.at[i, 'incorrect_words'] = newAnnotData.at[i, 'incorrect_words'].split(';')
		for word in newAnnotData.at[i, 'incorrect_words']:
			word = word.replace(' ', '')
			
		exact_correct = str(newAnnotData.at[i, 'exact_correct']).lower().replace(' ', '')
		if (exact_correct in ["true","t","yes","correct"]):
			newAnnotData.at[i, 'exact_correct'] = True
		else:
			newAnnotData.at[i, 'exact_correct'] = False
			
		periph_useful = str(newAnnotData.at[i, 'periph_useful']).lower().replace(' ', '')
		if (periph_useful in ["true","t","yes","correct"]):
			newAnnotData.at[i, 'periph_useful'] = True
		else:
			newAnnotData.at[i, 'periph_useful'] = False
			
	return newAnnotData
	
	
def rankAnnotFile(model, filename):
	print("-- Loading annotations file for ranking: " + filename + " --")
	data = pd.read_csv(filename)
	
	data['Score'] = pd.to_numeric(data['Score'], downcast="float")
	
	for i in data.index:
		sentence = make_tuple(data.at[i, 'sources'])[1]
		print ("-- Ranking annotation: " + sentence + " --")
		data.at[i, 'Score'] = rankSentence(model, sentence)
		print (" - Sentence score: " + str(data.at[i, 'Score']))
	
	print("++ Saving annotation ranks to file: rankedAnnotations.csv ++")
	data.to_csv("rankedAnnotations.csv",index=False)
	
def loadModelFile(filename):
	print("-- Loading model file: " + filename + " --")
	data = pd.read_csv(filename, index_col='word')
	data.index.name = 'word'
	return data
	
def saveModelFile(model, filename):
	print("++ Saving model to file: " + filename + " ++")
	model.index.name = 'word'
	model.to_csv(filename)
	
def updateModel(model, annotData):
	print ("++ Updating model with annotations ++")
	for i in annotData.index:
		print(" + Updating with annotation: ", end='')
		print(annotData.at[i, 'sources'][1])
		updateRanks(model, annotData.at[i, 'gene_name'], annotData.at[i, 'sources'][1], annotData.at[i, 'exact_correct'], annotData.at[i, 'correct_words'], annotData.at[i, 'incorrect_words'], annotData.at[i, 'periph_useful'])

	
if __name__ == '__main__':
	
	wordModel = loadModelFile("model.csv")
	annotData = loadAnnotFile("annotations-lcf.csv")
	
	#updateModel(wordModel, annotData)
	
	rankAnnotFile(wordModel, "annotations-lcf.csv")

	#saveModelFile(wordModel, "model.csv")