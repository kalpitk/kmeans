import matplotlib.pyplot as plt
import pandas as pd
import sys

colors = ['#741AAC','#00E0EB','#D4B0CB','#04D3BC','#7A797D','#81B622','#FF0000','#00FF00']
marker = ['+','o','^','s','p','*','3','D']
POINTS_FILEPATH = "./CSV/P"
CENTERS_FILEPATH = "./CSV/C"

def path_exist(turn,iter):
    try:
        with open(POINTS_FILEPATH+str(turn)+"_"+str(iter)+".csv",'r') as fh:
            return True
    except IOError:
        return False

turn = sys.argv[1]
while path_exist(turn,0):
    print("Turn : "+str(turn))
    iter=argv[2]
    while path_exist(turn,iter):
        plt.cla()
        
        points_df = pd.read_csv(POINTS_FILEPATH+str(turn)+"_"+str(iter)+".csv")
        centres_df = pd.read_csv(CENTERS_FILEPATH+str(turn)+"_"+str(iter)+".csv")

        points_labels = points_df.values.tolist()
        centres_labels = centres_df.values.tolist()

        plt.xlabel('Xvalues')
        plt.ylabel('Yvalues')

        #print(points_labels)
        #print(centres_labels)

        for point in points_labels:
            plt.scatter(point[0],point[1], color = colors[int(point[2])-1], marker='o', s=50)
        
        for index,centre in enumerate(centres_labels):
            plt.scatter(centre[0],centre[1], color='#000000' , marker='x', s=100)

        plt.draw()
        plt.pause(0.1)

        iter=iter+1

    plt.pause(3)
    turn = turn+1        
    
