#!/bin/env bash
curl -X POST \
     -H "Content-Type: application/json" \
     -d @dummy_patient.json \
     http://172.20.0.2:8080/api_v1/create_patient/ \
     -i
