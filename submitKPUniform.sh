#for i in {5111,5113,5392,5393}
#for i in {5723..5732}
#for i in {5723..5756}
#for i in {5177,5723}
for i in {0..999}
do
	name="bsub -q l ./bin/Linux-g++/hyptpc1 param/conf/KpUniformCH2.conf rootfiles_ana/KpUniformHist0${i}_Geant4.root g4macro/run_1M.mac"
	$name
done

