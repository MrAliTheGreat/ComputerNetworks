import matplotlib.pyplot as plt

def getXYForRttPlot(address):
	times1 = []; RTTs1 = []; times2 = []; RTTs2 = []
	file = open(address, 'r')
	line = file.readline()
	while line:
		line = line.split()
		if("rtt_" in line):
			if(line[1] == '0'):
				times1.append(float(line[0]))
				RTTs1.append(float(line[-1]))
			elif(line[1] == '1'):
				times2.append(float(line[0]))
				RTTs2.append(float(line[-1]))
		
		line = file.readline()
	
	return times1 , RTTs1 , times2 , RTTs2




cubicX_1 , cubicY_1 , cubicX_2 , cubicY_2 = getXYForRttPlot("./cubic/allTraces.tr")
renoX_1 , renoY_1 , renoX_2 , renoY_2 = getXYForRttPlot("./reno/allTraces.tr")
yeahX_1 , yeahY_1 , yeahX_2 , yeahY_2 = getXYForRttPlot("./yeah/allTraces.tr")


plt.plot(cubicX_1 , cubicY_1 , c = "blue")
plt.plot(cubicX_2 , cubicY_2 , c = "red")
plt.title("Cubic")
plt.show()

plt.plot(renoX_1 , renoY_1 , c = "blue")
plt.plot(renoX_2 , renoY_2 , c = "red")
plt.title("Reno")
plt.show()

plt.plot(yeahX_1 , yeahY_1 , c = "blue")
plt.plot(yeahX_2 , yeahY_2 , c = "red")
plt.title("Yeah")
plt.show()