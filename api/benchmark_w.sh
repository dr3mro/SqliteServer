#!/bin/bash

start_time=$(date +%s)

for i in {1..10000}; do
  curl --silent -o /dev/null -X POST -H "Content-Type: application/json" -d @patient.json http://172.20.0.2:8080/v1/store -i& 
done

wait

end_time=$(date +%s)
execution_time=$((end_time - start_time))

echo "Total execution time: $execution_time seconds"

