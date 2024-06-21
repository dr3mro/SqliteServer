import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 1000,
  duration: '120s',
};

export default function() {
  for (let i = 1; i <= 1000; i++) {
    let url = `http://172.20.0.2:8080/get/${i}`;

    let res = http.get(url);

    check(res, {
      'status is 200': (r) => r.status === 200,
    });
  }
}
