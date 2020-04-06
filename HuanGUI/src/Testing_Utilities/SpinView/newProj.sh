#!/bin/bash

# Get new project name
echo "What is the name of new project? "
read ProjName
# check if project existed
if [[ -d "$ProjName" ]]
then
	echo "Project name existed"
	exit 1
else
	# copy project from sample project
	cp -rd ./SampleProj "./$ProjName"
	# fix the Makefile
	echo "PROJ_NAME=$ProjName" > "./$ProjName/NewMakefile"
	cat "./$ProjName/Makefile" | tail -n +2 >> "./$ProjName/NewMakefile"
	rm "./$ProjName/Makefile"
	mv "./$ProjName/NewMakefile" "./$ProjName/Makefile"
fi
