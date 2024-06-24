import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 1000,
  duration: '30s',
};

export default function() {
  let res = http.get(
      'http://localhost:8080/api_v1/read_patient_basic_information/1000038');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
