import sys, csv, random
#source: https://stackoverflow.com/questions/44691524/write-a-2d-array-to-a-csv-file-with-delimiter

'''
Questions:
Should I eventually incorporate negative edges weights?
Should I enforce that output graphs be connected?
'''
if __name__ == "__main__":
    file_name = "tests/" + sys.argv[1] + ".txt"
    n = int(sys.argv[1])
    #print("Generating graph with " + str(n) + " vertices...")

    #Create a 2D array in which to build our matrix
    out = [[0]*n for i in range(n)]
    for i in range(n):
        for j in range(n):
            if not i == j:
                #Roughly half of all edges are "infinite"
                out[i][j] = random.randint(1, 2000)

    #save the result to a csv
    with open(file_name,"w+") as out_txt:
        csvWriter = csv.writer(out_txt,delimiter=' ')
        csvWriter.writerows(out)
