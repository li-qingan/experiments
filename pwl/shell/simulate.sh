BIN_PATH=/home/qali/experiments/pwl/bin
SYMBOL=$BIN_PATH/getSymbol

PIN_ROOT=/home/qali/src/pin-2.13-65163-gcc.4.4.7-linux
PIN=$PIN_ROOT/pin
PIN_TOOL_PATH=$PIN_ROOT/source/tools/SimpleExamples
PIN_TOOL=$PIN_TOOL_PATH/obj-intel64/$1.so

APPLICATION=$2

#1. obtain global address by reading dwarf debug info
echo $SYMBOL $APPLICATION
$SYMBOL $APPLICATION
echo cp $APPLICATION.symbol symbol.txt
cp $APPLICATION.symbol symbol.txt

#2. Generate trace by simulating
echo $PIN -t $PIN_TOOL -- ./$APPLICATION
$PIN -t $PIN_TOOL -- ./$APPLICATION


