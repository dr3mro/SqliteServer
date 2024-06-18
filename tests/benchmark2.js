import http from 'k6/http';
import { check } from 'k6';

export let options = {
    vus: 1000,
    duration: '30s',
};


export default function () {
  let res = http.get('http://localhost:18080/get');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}

