for directory in `ls */ -d`
do
	cd $directory
	python3 ../compile.py
	cd ..
done