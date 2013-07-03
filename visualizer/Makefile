INCLUDES = -I../../include -I../ssmtype -I../gndlib -I../tkglib -I../mtmlib

a.out: visualizer.cpp
	g++ ${INCLUDES} -o visualizer.out visualizer.cpp -lglut -lGLU -lssm

clean:
	rm *.out
