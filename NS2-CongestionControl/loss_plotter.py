import matplotlib.pyplot as plt

def getXYForLossPlot(address):
	times1 = []; numlostPackets1 = []; times2 = []; numlostPackets2 = []; currentNumPacketsLost1 = 0; currentNumPacketsLost2 = 0
	file = open(address, 'r')
	line = file.readline()
	while line:
		line = line.split()
		if(line[0] == "d"):
			if(line[-4] == "0.0"):
				currentNumPacketsLost1 += 1
				times1.append(float(line[1]))
				numlostPackets1.append(currentNumPacketsLost1)
			
			elif(line[-4] == "1.0"):
				currentNumPacketsLost2 += 1
				times2.append(float(line[1]))
				numlostPackets2.append(currentNumPacketsLost2)

		line = file.readline()
	
	return times1 , numlostPackets1 , times2 , numlostPackets2


def getLossRate(times , numLostPackets):
	rate = []
	for i in range(len(times)):
		rate.append(numLostPackets[i] / times[i])

	return rate



cubicX_1 , cubicY_1 , cubicX_2 , cubicY_2 = getXYForLossPlot("./cubic/allTraces.tr")
renoX_1 , renoY_1 , renoX_2 , renoY_2 = getXYForLossPlot("./reno/allTraces.tr")
yeahX_1 , yeahY_1 , yeahX_2 , yeahY_2 = getXYForLossPlot("./yeah/allTraces.tr")



plt.plot(cubicX_1 , getLossRate(cubicX_1 , cubicY_1) , c = "blue")
plt.plot(cubicX_2 , getLossRate(cubicX_2 , cubicY_2) , c = "red")
plt.title("Cubic")
plt.show()

plt.plot(renoX_1 , getLossRate(renoX_1 , renoY_1) , c = "blue")
plt.plot(renoX_2 , getLossRate(renoX_2 , renoY_2) , c = "red")
plt.title("Reno")
plt.show()

# Reno zoomed out Version
# plt.plot(list(range(10)) + renoX_1 + list(range(12 , 1001)), ([0] * 10) + getLossRate(renoX_1 , renoY_1) + [0] * 989 , c = "blue")
# plt.plot(renoX_2 + list(range(1001)) , getLossRate(renoX_2 , renoY_2) + [0] * 1001 , c = "red")
# plt.title("Reno")
# plt.show()

plt.plot(yeahX_1 , getLossRate(yeahX_1 , yeahY_1) , c = "blue")
plt.plot(yeahX_2 , getLossRate(yeahX_2 , yeahY_2) , c = "red")
plt.title("Yeah")
plt.show()

# Yeah better version
# plt.plot(yeahX_1 , getLossRate(yeahX_1 , yeahY_1) , c = "blue")
# plt.plot(list(range(665)) + yeahX_2 + list(range(942 , 1001)) , [0] * 665 + getLossRate(yeahX_2 , yeahY_2) + [0] * 59 , c = "red")
# plt.title("Yeah")
# plt.show()