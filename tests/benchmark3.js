import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 1000,
  duration: '30s',
};

export default function() {
  // Generate a random integer between 1 and 1000
  let randomNumber = Math.floor(Math.random() * 1000) + 1;

  let url = `http://172.20.0.2:8080/get/${randomNumber}`;

  let res = http.get(url);

  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
