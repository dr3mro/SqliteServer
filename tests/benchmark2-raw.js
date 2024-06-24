import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 1000,
  duration: '30s',
};

export default function() {
  let res = http.get(
      'http://172.20.0.2:8080/api_v1/read_patient_basic_information/100003?');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
