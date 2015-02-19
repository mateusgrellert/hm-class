import os
from joblib import Parallel, delayed  
import multiprocessing
import itertools


def parseResults(file_path):
	f = open(file_path,'r')
	count = 0
	psnr_count = False
	SP = PV = -1
	time = 0.0
	
	for l in (f.readlines()):
		if '[ET' in l:
			poc = int(l.split()[1])
			if poc >= 4:
				time += float(l.split()[23])
		elif 'Bytes written to file' in l:
			bitrate = float(l.split('(')[1].split()[0])
		elif 'SUMMARY' in l:
			psnr_count = True
		elif 'SP' in l:
			SP = float(l.split()[1])
			PV = float(l.split()[3])
		elif count == 2:
			y_psnr = float(l.split()[3])
			u_psnr = float(l.split()[4])
			v_psnr = float(l.split()[5])
			psnr = (4*y_psnr+u_psnr+v_psnr)/6.0
		if psnr_count:
			count += 1

	if SP == -1:
		return ('\t'.join([str(x) for x in [bitrate, y_psnr,u_psnr, v_psnr, time]]))
	else:
		return ('\t'.join([str(x) for x in [bitrate, y_psnr,u_psnr, v_psnr, time, SP, PV]]))


def runHM(struct, seq, qp, SP, alg, kp,ki,kd, num_frames):
	file_tail = "_".join([qp, kp,ki,kd])
	out_file = open("control_results_"+ file_tail +".csv","w")
	
	setup = ("_".join([struct[:-4], seq[:-4], qp, "SP-"+SP, "Alg"+alg, "KP"+kp, "KI"+ki,"KD"+kd]))
	out_path = setup + ".txt"

	cmd_line = "./TAppEncoderStatic -c ~/hm-cfgs/cropped/"+seq+" -c ../cfg/"+struct
	cmd_line += " --SP_factor="+SP+" --budget_alg="+alg + " --QP="+qp
	cmd_line += " --KP="+kp+" --KI="+ki+" --KD="+kd
	cmd_line += " -f " + num_frames+ " > " + out_path

	os.system(cmd_line)
	results = parseResults(out_path)
	os.system("rm "+out_path)
	print >> out_file, setup, "\t", results
	out_file.close()	

def condenseResults(struct, seq, qp, SP, alg, kp,ki,kd, num_frames):
	file_tail = "_".join([qp, kp,ki,kd])
	in_file = open("control_results_"+ file_tail +".csv","r")
	out_file = open("control_results_overall.csv","a")
	for l in in_file.readlines():
		print >> out_file, l.strip("\n")

	in_file.close()
	out_file.close()
	os.system("rm control_results_"+ file_tail +".csv")



sequences = ["BQSquare.cfg", "RaceHorsesC.cfg"]
sequences = ["RaceHorsesC.cfg"]
num_frames = "62"
num_frames = "9"
SPs = ["0.2","0.3","0.4","0.5","0.6","0.7", "0.8"]
SPs = ["0.6"]
budget_algs = ["0","1"]
budget_algs = ["1"]
QPs = ["22","27","32","37"]
QPs = ["32"]

KPs = ["0.0", "0.2", "0.4", "0.6", "0.8","1.0", "1.2", "1.4", "1.6"]
KIs = ["0.0", "0.2", "0.4", "0.6", "0.8","1.0", "1.2", "1.4", "1.6"]
KDs = ["0.0", "0.2", "0.4", "0.6", "0.8","1.0", "1.2", "1.4", "1.6"]

KPs = KPs[0:1]
KIs = KIs[0:1]
KDs = KDs[0:2]

Ks = [KPs, KIs, KDs]
Ks = list(itertools.product(*Ks))

structs = ["encoder_lowdelay_main.cfg"]

num_cores = multiprocessing.cpu_count() 
out_file = open("control_results_overall.csv","w")
print >> out_file, "Setup\tBitrate\tY-PSNR\tU-PSNR\tV-PSNR\tTime\tSP\tPV(avg)"
out_file.close()

for struct in structs:
	for alg in budget_algs:
		for SP in SPs:
			for seq in sequences:
				for qp in QPs:
					Parallel(n_jobs=num_cores)(delayed(runHM)(struct, seq, qp, SP, alg, kp, ki, kd, num_frames) for kp,ki,kd in Ks)
					#Parallel(n_jobs=num_cores)(delayed(runHM)(struct, seq, qp, SP, alg, kp, "1.0", "1.0", num_frames) for kp in KPs)
					#Parallel(n_jobs=num_cores)(delayed(runHM)(struct, seq, qp, SP, alg, "1.0", ki, "1.0", num_frames) for ki in KIs)
					#Parallel(n_jobs=num_cores)(delayed(runHM)(struct, seq, qp, SP, alg, "1.0", "1.0", kd, num_frames) for kd in KDs)

for struct in structs:
	for alg in budget_algs:
		for SP in SPs:
			for seq in sequences:
				for qp in QPs:
					for kp, ki, kd in Ks:
						condenseResults(struct, seq, qp, SP, alg, kp, ki, kd, num_frames)





