import os

def parseResults(file_path):
	f = open(file_path,'r')
	count = 0
	psnr_count = False
	SP = PV = -1
	for l in (f.readlines()):
		if 'Total Time' in l:
			time = float(l.split()[2])
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


def runHM(struct, seq, qp, SP, alg, num_frames, out_file):
	setup = ("_".join([struct[:-4], seq[:-4], qp, "SP-"+SP, "Alg"+alg]))
	out_path = setup + ".txt"
	os.system("./TAppEncoderStatic -c ~/hm-cfgs/cropped/"+seq+" -c ../cfg/"+struct+" --SP_factor="+SP+" --budget_alg="+alg + " --QP="+qp+" -f " + num_frames+ " > " + out_path)
	results = parseResults(out_path)
	print >> out_file, setup, "\t", results
	

sequences = ["BQSquare.cfg", "RaceHorsesC.cfg"]
sequences = ["RaceHorsesC.cfg"]
num_frames = "32"
SPs = ["0.2","0.3","0.4","0.5","0.6","0.7", "0.8"]
budget_algs = ["0","1"]
#budget_algs = ["1"]
QPs = ["32"]
structs = ["encoder_lowdelay_main.cfg"]

out_file = open("control_results.csv","w")
print >> out_file, "Setup\tBitrate\tY-PSNR\tU-PSNR\tV-PSNR\tTime\tSP\tPV(avg)"

for struct in structs:
	for alg in budget_algs:
		for SP in SPs:
			for seq in sequences:
				for qp in QPs:
					runHM(struct, seq, qp, SP, alg, num_frames, out_file)




