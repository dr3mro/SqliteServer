import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 100,
  duration: '30s',
};

export default function() {
  let res = http.get(
      'http://localhost:18080/api_v1/read_patient_basic_information/100040');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
