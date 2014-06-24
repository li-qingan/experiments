BIN_PATH=/home/qali/project/experiments/pwl/bin

if [ $1 -eq 0 ]
then
	shift
	ALLOCATOR=defaultAllocator
	ARGS=$@		
elif [ $1 -eq 1 ]
then
	shift
	ALLOCATOR=optiAllocator
	ARGS=$@
else
	echo "The first argument should be either 0 or 1"
	exit 1
fi

#1. start allocating
echo $BIN_PATH/$ALLOCATOR $ARGS
$BIN_PATH/$ALLOCATOR $ARGS


