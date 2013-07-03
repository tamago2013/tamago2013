
-include mk/subdir.mk


.PHONY: all projects clean

all:projects

projects:
	@for prj in ${PROJECTS}; do	\
		cd $$prj ;		\
		make ;			\
		cd - ;			\
	done

clean:
	@for prj in ${PROJECTS}; do	\
		cd $$prj ;		\
		make clean ;		\
		cd - ;			\
	done



