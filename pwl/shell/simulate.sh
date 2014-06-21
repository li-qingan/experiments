BIN_PATH=/home/qali/project/experiments/pwl/bin
SYMBOL=$BIN_PATH/getSymbol


PIN=/home/qali/Ins/pin-2.13-65163-gcc.4.4.7-linux/pin
PIN_TOOL_PATH=/home/qali/Ins/pin-2.13-65163-gcc.4.4.7-linux/source/tools/SimpleExamples
PIN_TOOL=$PIN_TOOL_PATH/obj-ia32/$1.so

APPLICATION=$2

#1. obtain global address
echo $SYMBOL $APPLICATION
$SYMBOL $APPLICATION
echo cp $APPLICATION.symbol symbol.txt
cp $APPLICATION.symbol symbol.txt

#2. start simulating
echo $PIN -t $PIN_TOOL -- ./$APPLICATION
$PIN -t $PIN_TOOL -- ./$APPLICATION

