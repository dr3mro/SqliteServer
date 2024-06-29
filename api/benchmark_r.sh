#!/bin/bash

start_time=$(date +%s)

for i in {1..500}; do
  curl --silent -o /dev/null -X GET -H "Content-Type: application/json" -d @get_patient.json http://172.20.0.2:8080/v1/retrieve -i &
done

wait

end_time=$(date +%s)
execution_time=$((end_time - start_time))

echo "Total execution time: $execution_time seconds"

