import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 1000,
  duration: '30s',
};

export default function() {
  let params = {
    headers: {
      'Accept-Encoding': 'deflate',
    },
  };

  let res = http.get(
      'http://172.20.0.2:8080/api_v1/read_patient_basic_information/100003?',
      params);
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}
