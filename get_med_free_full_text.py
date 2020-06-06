#sites = ["https://europepmc.org/article/med/28348550#free-full-text"]
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import re
def get_free_full_text(sites):
    driver = webdriver.Firefox()
    print(type(sites))
    i = 0
    if type(sites) is str:
        driver.get('https://europepmc.org/article/med/'+str(sites)+'#free-full-text')
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "abstract")))
        try:
            fulltext = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "fulltextcontent")))
            sitetext = fulltext.text    
            driver.close()
        except:
            return -1
            
        open("paper_text_" + str(sites) + ".txt","w",encoding='utf-8').write(sitetext)
        return 1
    if type(sites) is list:
        for site in sites:
            driver.get('https://europepmc.org/article/med/'+str(site)+'#free-full-text')
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "abstract")))
            try:
                fulltext = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "fulltextcontent")))
                sitetext = fulltext.text
                driver.close()
            except:
                print('')
        
            open("paper_text_" + str(site) + ".txt","w",encoding='utf-8').write(sitetext)
            i+=1
        


