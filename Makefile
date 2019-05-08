default: simplecpu

program: simplecpu
	gcc simplecpu.c -o simplecpu

clean:
	-rm -f simplecpu
