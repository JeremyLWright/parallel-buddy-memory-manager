Report.pdf: Report.tex NormalizedSpeedComp.png MegaTexture.bib
	pdflatex Report.tex
	bibtex Report.aux
	pdflatex Report.tex
	pdflatex Report.tex

NormalizedSpeedComp.png: MutexAllocSpeed.dat PlatformSpeed.dat SpinAllocSpeed.dat
	octave HighWater.m
	octave SpeedComp.m


