#!/bin/bash

for i in {1..10}; do
	./basic $(((i-1)*10000+1)) $((i*10000)) &
done

wait
echo "done :)"