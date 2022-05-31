



file1 = "test2DSamcra"
file2 = "2Dres-sorted"






def in_dict(filename):
	dict =  {}
	with open(filename,'r') as f:
		for l in f.readlines():
			l = l.strip()
			if len(l.split(" ")) == 3:
				node, delay, cost = l.split(" ")
			else:
				node, delay, cost, _ = l.split(" ")
			cost = int(cost)
			if node not in dict: 
				dict[node] = {}
			if delay in dict[node]:
				dict[node][delay] = min(dict[node][delay], cost)
			else:
				dict[node][delay] = cost
	return dict
			


dic1 = in_dict(file1)
dic2 = in_dict(file2)

# check if dic1 is included in dic2
for node in dic1: 
	for delay in dic1[node]:
		if node not in dic2:
			print("NODE {} not in {}".format(node, file2))
			continue
		elif delay not in dic2[node]:
			print("DELAY {} not in {} for node {}".format(delay, file2, node))
		else:
			cost1 = dic1[node][delay]
			cost2 = dic2[node][delay]
			if cost1 != cost2: 
				print("{} vs {} for node {} and delay {}".format(cost1,cost2, node, delay))
			
