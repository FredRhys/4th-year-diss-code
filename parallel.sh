#!/bin/bash

for i in {1..10}; do
	./basic $(((i-1)*100000+1)) $((i*100000)) 2097151 &
done

wait
echo "done :)"