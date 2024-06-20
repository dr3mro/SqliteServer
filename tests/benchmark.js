import http from 'k6/http';
import { check, sleep } from 'k6';

export let options = {
  stages: [
    // Ramp-up from 1 to 50 users over 30 seconds
    { duration: '30s', target: 50 },
    // Stay at 50 users for 1 minute
    { duration: '1m', target: 50 },
    // Ramp-down from 50 to 0 users over 30 seconds
    { duration: '30s', target: 0 }
  ],
};

export default function () {
  let res = http.get('http://localhost:8080/get');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
  //sleep(1); // Adjust if necessary to control request rate
}

