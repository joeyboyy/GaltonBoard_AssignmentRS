#default settings: set terminal svg size 600,480 fixed enhanced font 'Arial,12' butt dashlength 1.0
set terminal svg size 800,600 fixed enhanced font 'Arial,18' butt dashlength 1.0

set style fill transparent solid 0.4 border
unset key

set xlabel "k"
set ylabel "#balls in k-th box"
set xlabel offset 0,1
set ylabel offset 2,0
set xtics offset 0,0.3

unset pointintervalbox

# plot all files
files = system("ls out/sim* -1")
data = ""
do for [i=1:words(files)] {
	### retrieve file names
	f = word(files,i)
	name = f[9:strstrt(f,".dat")-1]
	filesim = "out/sim_".name.".dat"
	filebin = "out/binom_".name.".dat"
	filenor = "out/normal_".name.".dat"

	### read file height and #balls from file name
	height = name[strstrt(name,"_h")+2 : strstrt(name,"_b")-1]
	nballs = name[strstrt(name,"_b")+2:]

	### (silently) generate statistical summary of 2nd col of file (in particular sets STATS_max = max of all entries in column 2)
	set xrange [*:*] ; set yrange [*:*] # <- reset ranges for stats command
	stats filesim using 2 nooutput

	### Set plotting frame
	xmin = -0.5
	xmax = height+0.5
	#ymax = 1.5 * nballs * binom(int((n+1)*p), n, p) #Mode of binomial PDF used
	ymax = STATS_max*1.04
	set xrange [xmin : xmax]
	set yrange [0 : ymax]

	### Verbose tics if size allows it...
	if (height < 25) {
		set xtics 1
	}
	else {
		set xtics autofreq
	}
	
	set output "plots/".name.".svg"
	set title "Galton board height: ".height.", #balls: ".nballs
	plot filesim with boxes, \
		 filebin with impulses lw 100/height + 1, \
		 filenor with lines lw 3

	if (height > 200) {
		set output "plots/".name."nobinom.svg"
		set title "Galton board height: ".height.", #balls: ".nballs
		plot filesim with boxes, \
			filenor with lines lw 3
	}

	set output "plots/".name."weps.svg"
	set title "Galton board height: ".height.", #balls: ".nballs
	plot filesim with boxes, \
		 filebin with errorbars lw (height < 10 ? 5 : 50/height) + 1
	
}

