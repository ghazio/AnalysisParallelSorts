import pandas as pd
import plotly.express as px


def main():

    # # read = pd.read_csv('mugwort__size.csv')
    # # GFG = pd.Excelwriter('mugwortSize_excel.xlsx')
    # # read.to_excel(GFG, index = False)

    # # GFG.save()
    desired_file = input("Enter csv file: ")
    # read_file = pd.read_csv(r,desired_file)
    # read_file.to_excel('r','/home/ookoron1/cs87/Project-ookoron1-mrandha1-mmcarth1/source/Radix/Sequential_radix/data/mugwort__size.xlsx', index = None, header = True)
    File = pd.read_csv(desired_file)

    graph = px.line(File, x = 'size', y = 'runtime', title = desired_file + "RadixSort Seqeuntial Grpah")
    graph.show()
main()








    








#/home/cs87/Project-ookoron1-mrandha1-mmcarth1/source/Radix/Sequential_radix/data/mugwort__size.csv
