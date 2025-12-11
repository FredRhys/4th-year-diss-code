#!/bin/bash

for i in {1..10}; do
	./basic $(((i-1)*1000+1)) $((i*1000)) 2000000 &
done

wait
echo "done :)"